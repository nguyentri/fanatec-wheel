/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 *
 * Bit-banged TM1637 driver: 2-wire (CLK/DIO open-drain-style) with
 * start/stop conditions and per-byte ACK, ~100 kHz, LSB first.
 */

#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>

#include "tm1637.h"

#define TM1637_NODE DT_NODELABEL(tm1637)

#define CMD_DATA_AUTO 0x40U /* data command: auto address increment */
#define CMD_ADDR_BASE 0xC0U /* address command: digit 0 */
#define CMD_DISP_CTRL 0x80U /* display control: 0x88 | brightness */

#define TM_HALF_PERIOD_US 5

static const struct gpio_dt_spec clk =
	GPIO_DT_SPEC_GET(TM1637_NODE, clk_gpios);
static const struct gpio_dt_spec dio =
	GPIO_DT_SPEC_GET(TM1637_NODE, dio_gpios);

static void tm_delay(void)
{
	k_busy_wait(TM_HALF_PERIOD_US);
}

static void tm_start(void)
{
	gpio_pin_set_dt(&dio, 1);
	gpio_pin_set_dt(&clk, 1);
	tm_delay();
	gpio_pin_set_dt(&dio, 0);
	tm_delay();
	gpio_pin_set_dt(&clk, 0);
}

static void tm_stop(void)
{
	gpio_pin_set_dt(&clk, 0);
	gpio_pin_set_dt(&dio, 0);
	tm_delay();
	gpio_pin_set_dt(&clk, 1);
	tm_delay();
	gpio_pin_set_dt(&dio, 1);
	tm_delay();
}

static void tm_write_byte(uint8_t b)
{
	for (int i = 0; i < 8; i++) { /* LSB first */
		gpio_pin_set_dt(&clk, 0);
		gpio_pin_set_dt(&dio, (b >> i) & 1U);
		tm_delay();
		gpio_pin_set_dt(&clk, 1);
		tm_delay();
	}
	/* ACK slot: release DIO for one clock. */
	gpio_pin_set_dt(&clk, 0);
	gpio_pin_set_dt(&dio, 1);
	tm_delay();
	gpio_pin_set_dt(&clk, 1);
	tm_delay();
	gpio_pin_set_dt(&clk, 0);
}

int tm1637_brightness(uint8_t level, bool on)
{
	tm_start();
	tm_write_byte(CMD_DISP_CTRL | (on ? 0x08U : 0x00U) | (level & 7U));
	tm_stop();
	return 0;
}

int tm1637_set_segments(const uint8_t seg[4])
{
	tm_start();
	tm_write_byte(CMD_DATA_AUTO);
	tm_stop();

	tm_start();
	tm_write_byte(CMD_ADDR_BASE);
	for (int i = 0; i < 4; i++) {
		tm_write_byte(seg[i]);
	}
	tm_stop();
	return 0;
}

int tm1637_init(void)
{
	if (!gpio_is_ready_dt(&clk) || !gpio_is_ready_dt(&dio)) {
		return -ENODEV;
	}
	if (gpio_pin_configure_dt(&clk, GPIO_OUTPUT_HIGH) < 0 ||
	    gpio_pin_configure_dt(&dio, GPIO_OUTPUT_HIGH) < 0) {
		return -EIO;
	}
	return tm1637_brightness(5, true); /* reference TM_BRIGHTNESS 5 */
}
