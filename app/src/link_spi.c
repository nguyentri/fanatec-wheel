/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 *
 * link_spi: SPI peripheral (slave) transport for the 33-byte exchange.
 *
 * Porting map (spec section 4):
 *  - setup() SPCR config      -> spi_config SPI_OP_MODE_SLAVE | SPI_MODE_CPHA
 *  - cableselect() D2 ISR     -> cs_monitor GPIO EXTI on CS rising edge
 *  - ISR(SPI_STC_vect)        -> full-frame (DMA) transfer, validated after CS
 *
 * Implementation notes (spec section 10): slave spi_transceive blocks
 * until the controller completes the frame, so it runs in a dedicated
 * link thread; the CS-rising EXTI performs the buffer swap/resubmit;
 * short frames are terminated via the driver's abort/release path.
 */

#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/spi.h>

#include <lib/rimlink/rimlink.h>

#include "link_spi.h"

#define LINK_THREAD_STACK_SIZE 1024
#define LINK_THREAD_PRIORITY -2 /* cooperative high */

#if DT_NODE_HAS_STATUS(DT_NODELABEL(spi1), okay)
#define LINK_SPI_NODE DT_NODELABEL(spi1)
static const struct device *const link_spi_dev = DEVICE_DT_GET(LINK_SPI_NODE);
#endif

/* Double buffer: armed (owned by transfer) / staging (owned by adapter). */
static uint8_t tx_frame[2][RIMLINK_FRAME_LEN];
static uint8_t rx_frame[RIMLINK_FRAME_LEN];

/* RX hand-off queue, depth >= 4 (spec section 5.3). */
K_MSGQ_DEFINE(link_rx_msgq, RIMLINK_FRAME_LEN, 4, 4);

static void link_thread(void *a, void *b, void *c)
{
	ARG_UNUSED(a);
	ARG_UNUSED(b);
	ARG_UNUSED(c);

#if DT_NODE_HAS_STATUS(DT_NODELABEL(spi1), okay)
	const struct spi_config cfg = {
		.operation = SPI_OP_MODE_SLAVE | SPI_MODE_CPHA |
			     SPI_WORD_SET(8) | SPI_TRANSFER_MSB,
	};
	const struct spi_buf txb = {
		.buf = tx_frame[0], .len = RIMLINK_FRAME_LEN };
	const struct spi_buf rxb = {
		.buf = rx_frame, .len = RIMLINK_FRAME_LEN };
	const struct spi_buf_set tx = { .buffers = &txb, .count = 1 };
	const struct spi_buf_set rx = { .buffers = &rxb, .count = 1 };

	while (1) {
		/*
		 * TODO(phase1): full transaction state machine
		 * (COLD -> ARMED -> ACTIVE -> VALIDATE, spec fig. 5-1):
		 *  - CS-rising EXTI: abort/complete, swap staging->armed
		 *    if fresh, resubmit so byte 0 (0xA5) is staged
		 *  - short-frame / overrun / rearm-miss counters
		 *  - hand validated RX frames to link_rx_msgq
		 */
		int ret = spi_transceive(link_spi_dev, &cfg, &tx, &rx);

		if (ret >= 0) {
			(void)k_msgq_put(&link_rx_msgq, rx_frame, K_NO_WAIT);
		} else {
			k_sleep(K_MSEC(10));
		}
	}
#else
	/* No SPI instance enabled in devicetree: idle placeholder. */
	while (1) {
		k_sleep(K_FOREVER);
	}
#endif
}

K_THREAD_DEFINE(link_tid, LINK_THREAD_STACK_SIZE,
		link_thread, NULL, NULL, NULL,
		LINK_THREAD_PRIORITY, 0, 0);

int link_spi_init(void)
{
	struct rim_inputs neutral = {0};

	/* Seal the identity frame so the first transaction is valid. */
	rimlink_frame_build(tx_frame[0], &neutral);
	rimlink_frame_build(tx_frame[1], &neutral);

#if DT_NODE_HAS_STATUS(DT_NODELABEL(spi1), okay)
	if (!device_is_ready(link_spi_dev)) {
		return -ENODEV;
	}
	/* TODO(phase1): configure CS-sense EXTI (PG14 equivalent) and
	 * raise LINK_READY test point (t1, spec fig. 10-1). */
	return 0;
#else
	return -ENOTSUP;
#endif
}
