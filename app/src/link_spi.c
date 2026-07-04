/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 *
 * link_spi: SPI peripheral (slave) transport for the 33-byte exchange.
 *
 * Porting map (spec section 4):
 *  - setup() SPCR (SPE|SPIE|CPHA)  -> spi_config SPI_OP_MODE_SLAVE |
 *                                     SPI_MODE_CPHA | SPI_WORD_SET(8)
 *  - cableselect() on D2 RISING    -> cs_sense GPIO EXTI on CS rising:
 *                                     re-arm point, buffer swap
 *  - ISR(SPI_STC_vect) per byte    -> full-frame transfer, validated
 *                                     after the transaction
 *
 * Scheduling rules (spec section 3.1): nothing in this path logs,
 * allocates, or blocks on anything but the transfer itself. The slave
 * spi_transceive blocks until the controller completes the frame, so
 * it runs in a dedicated cooperative link thread (driver capability
 * confirmed in-tree, roadmap section 11.5); the CS-rising EXTI is the
 * authoritative framing signal. If measured re-arm latency misses the
 * Gate G1 deadline, the pre-approved fallback is a thin LL-level
 * transfer layer beneath this unchanged adapter API (spec section 10).
 */

#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/sys/atomic.h>

#include <lib/rimlink/rimlink.h>

#include "link_spi.h"

#define LINK_THREAD_STACK_SIZE 1536
#define LINK_THREAD_PRIORITY -2 /* cooperative high */

#define SPI_NODE DT_NODELABEL(spi1)
#define HAVE_SPI DT_NODE_HAS_STATUS(SPI_NODE, okay)

#define CS_SENSE_NODE DT_NODELABEL(cs_sense)
#define HAVE_CS_SENSE DT_NODE_EXISTS(CS_SENSE_NODE)

#define LINK_READY_NODE DT_NODELABEL(link_ready)
#define HAVE_LINK_READY DT_NODE_EXISTS(LINK_READY_NODE)

#define ERR_LED_NODE DT_NODELABEL(err_led)
#define HAVE_ERR_LED DT_NODE_EXISTS(ERR_LED_NODE)

#if HAVE_SPI
static const struct device *const spi_dev = DEVICE_DT_GET(SPI_NODE);
#endif
#if HAVE_CS_SENSE
static const struct gpio_dt_spec cs_sense =
	GPIO_DT_SPEC_GET(CS_SENSE_NODE, gpios);
static struct gpio_callback cs_cb;
#endif
#if HAVE_LINK_READY
static const struct gpio_dt_spec link_ready =
	GPIO_DT_SPEC_GET(LINK_READY_NODE, gpios);
#endif
#if HAVE_ERR_LED
static const struct gpio_dt_spec err_led =
	GPIO_DT_SPEC_GET(ERR_LED_NODE, gpios);
#endif

static uint8_t rx_frame[RIMLINK_FRAME_LEN];

/* armed==1 while a transceive is submitted with a sealed frame. */
static atomic_t armed;
static uint32_t last_cs_rise_cyc;
static uint32_t boot_ready_cyc; /* first LINK_READY assertion (2-S2) */
static K_SEM_DEFINE(link_run, 0, 1);

static void err_led_set(bool on)
{
#if HAVE_ERR_LED
	(void)gpio_pin_set_dt(&err_led, on ? 1 : 0);
#else
	ARG_UNUSED(on);
#endif
}

static void link_ready_set(bool on)
{
#if HAVE_LINK_READY
	(void)gpio_pin_set_dt(&link_ready, on ? 1 : 0);
#else
	ARG_UNUSED(on);
#endif
}

#if HAVE_CS_SENSE
/*
 * CS rising edge = end of transaction (reference cableselect()).
 * Fast-path budget <= 10 us: timestamp for CS-gap statistics and
 * re-arm-miss detection only; the buffer swap itself happens at the
 * transceive boundary in the link thread, which the completed
 * transfer unblocks at this same edge.
 */
static void cs_sense_isr(const struct device *dev,
			 struct gpio_callback *cb, uint32_t pins)
{
	ARG_UNUSED(dev);
	ARG_UNUSED(cb);
	ARG_UNUSED(pins);

	uint32_t now = k_cycle_get_32();

	if (last_cs_rise_cyc != 0U) {
		uint64_t us = k_cyc_to_us_floor64(
			(uint64_t)(now - last_cs_rise_cyc));
		rimlink_note_cs_gap((uint32_t)us);
	}
	last_cs_rise_cyc = now;

	/* If the link thread has not re-armed a transfer by the time
	 * the *next* transaction ends, the previous frame repeated:
	 * that is the re-arm-miss condition (spec section 5.4). */
	if (atomic_get(&armed) == 0) {
		rimlink_note_rearm_miss();
		err_led_set(true); /* LD3-equivalent latch */
	}
}
#endif /* HAVE_CS_SENSE */

static void link_thread(void *a, void *b, void *c)
{
	ARG_UNUSED(a);
	ARG_UNUSED(b);
	ARG_UNUSED(c);

	k_sem_take(&link_run, K_FOREVER);

#if HAVE_SPI
	const struct spi_config cfg = {
		/* Mode 1 (CPOL=0, CPHA=1), MSB first - author's
		 * logic-scope finding, spec section 2.1. */
		.operation = SPI_OP_MODE_SLAVE | SPI_MODE_CPHA |
			     SPI_WORD_SET(8) | SPI_TRANSFER_MSB,
	};
	struct spi_buf txb = { .len = RIMLINK_FRAME_LEN };
	const struct spi_buf rxb = {
		.buf = rx_frame, .len = RIMLINK_FRAME_LEN };
	const struct spi_buf_set tx = { .buffers = &txb, .count = 1 };
	const struct spi_buf_set rx = { .buffers = &rxb, .count = 1 };

	while (1) {
		/* Re-arm point: swap in the newest snapshot frame so
		 * byte 0 (0xA5) is staged before the next CS fall. */
		txb.buf = (void *)rimlink_tx_acquire();

		atomic_set(&armed, 1);
		link_ready_set(true);
		if (boot_ready_cyc == 0U) {
			/* t1: first armed transfer (spec fig. 10-1). */
			boot_ready_cyc = k_cycle_get_32() | 1U;
		}

		int ret = spi_transceive(spi_dev, &cfg, &tx, &rx);

		atomic_set(&armed, 0);
		link_ready_set(false);

		if (ret >= 0) {
			/* Slave transceive returns the received byte
			 * count; < 33 means the controller deasserted
			 * early (short frame). */
			rimlink_rx_submit(rx_frame, (size_t)ret);
		} else {
			/* Transfer fault: FAULT -> recover, counters
			 * kept by the adapter via short_frame path. */
			rimlink_rx_submit(rx_frame, 0);
			k_sleep(K_MSEC(1));
		}
	}
#else
	while (1) {
		k_sleep(K_FOREVER);
	}
#endif /* HAVE_SPI */
}

K_THREAD_DEFINE(link_tid, LINK_THREAD_STACK_SIZE,
		link_thread, NULL, NULL, NULL,
		LINK_THREAD_PRIORITY, 0, 0);

int link_spi_init(void)
{
	int ret = 0;

#if HAVE_LINK_READY
	if (!gpio_is_ready_dt(&link_ready) ||
	    gpio_pin_configure_dt(&link_ready, GPIO_OUTPUT_INACTIVE) < 0) {
		return -ENODEV;
	}
#endif
#if HAVE_ERR_LED
	if (gpio_is_ready_dt(&err_led)) {
		(void)gpio_pin_configure_dt(&err_led, GPIO_OUTPUT_INACTIVE);
	}
#endif

#if HAVE_CS_SENSE
	if (!gpio_is_ready_dt(&cs_sense)) {
		return -ENODEV;
	}
	ret = gpio_pin_configure_dt(&cs_sense, GPIO_INPUT);
	if (ret < 0) {
		return ret;
	}
	/* Reference cableselect(): interrupt on CS RISING. The node is
	 * GPIO_ACTIVE_LOW (CS), so "inactive" edge = physical rising. */
	ret = gpio_pin_interrupt_configure_dt(&cs_sense,
					      GPIO_INT_EDGE_TO_INACTIVE);
	if (ret < 0) {
		return ret;
	}
	gpio_init_callback(&cs_cb, cs_sense_isr, BIT(cs_sense.pin));
	ret = gpio_add_callback(cs_sense.port, &cs_cb);
	if (ret < 0) {
		return ret;
	}
#endif /* HAVE_CS_SENSE */

#if HAVE_SPI
	if (!device_is_ready(spi_dev)) {
		return -ENODEV;
	}
#else
	ret = -ENOTSUP;
#endif

	/* Release the link thread: first frame already sealed by
	 * rimlink_init(); LINK_READY rises with the first submission
	 * (t1, boot-to-ready, spec fig. 10-1). */
	k_sem_give(&link_run);

	return ret;
}

uint32_t link_spi_boot_ready_us(void)
{
	if (boot_ready_cyc == 0U) {
		return 0;
	}
	return (uint32_t)k_cyc_to_us_floor64(boot_ready_cyc);
}
