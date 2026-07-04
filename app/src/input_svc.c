/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 *
 * Input service (spec section 6), porting the reference readButtons():
 * six active-low buttons with internal pull-ups scanned at 1 kHz with
 * 2-sample debounce, analog D-pad ladder decoded through calibrated
 * windows with hysteresis, all composed into an immutable
 * struct rim_inputs snapshot and published to the adapter.
 *
 * Default physical mapping mirrors the reference configuration:
 * buttonsBits[] = { 8, 5, 13, 11, 9, 12 } (spec section 2.4),
 * reconfigurable at run time via shell.
 */

#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/util.h>
#include <string.h>

#if DT_NODE_EXISTS(DT_PATH(zephyr_user)) && \
	DT_NODE_HAS_PROP(DT_PATH(zephyr_user), io_channels)
#define HAVE_DPAD_ADC 1
#include <zephyr/drivers/adc.h>
#else
#define HAVE_DPAD_ADC 0
#endif

#include <lib/rimlink/rimlink.h>

#include "input_svc.h"
#include "debounce.h"
#include "dpad.h"

LOG_MODULE_REGISTER(input_svc, CONFIG_APP_LOG_LEVEL);

#define INPUT_THREAD_STACK_SIZE 1536
#define INPUT_THREAD_PRIORITY -1 /* cooperative high (spec section 3.1) */

/* The six DUT buttons (hardware spec section 5.2), via devicetree. */
#define BTN_SPEC(node_id) GPIO_DT_SPEC_GET(node_id, gpios),
static const struct gpio_dt_spec btn_specs[] = {
#if DT_NODE_EXISTS(DT_NODELABEL(btn1))
	GPIO_DT_SPEC_GET(DT_NODELABEL(btn1), gpios),
	GPIO_DT_SPEC_GET(DT_NODELABEL(btn2), gpios),
	GPIO_DT_SPEC_GET(DT_NODELABEL(btn3), gpios),
	GPIO_DT_SPEC_GET(DT_NODELABEL(btn4), gpios),
	GPIO_DT_SPEC_GET(DT_NODELABEL(btn5), gpios),
	GPIO_DT_SPEC_GET(DT_NODELABEL(btn6), gpios),
#endif
};

#define SNAP_TICK_NODE DT_NODELABEL(snap_tick)
#define HAVE_SNAP_TICK DT_NODE_EXISTS(SNAP_TICK_NODE)
#if HAVE_SNAP_TICK
static const struct gpio_dt_spec snap_tick =
	GPIO_DT_SPEC_GET(SNAP_TICK_NODE, gpios);
#endif

#if HAVE_DPAD_ADC
static const struct adc_dt_spec dpad_adc =
	ADC_DT_SPEC_GET_BY_IDX(DT_PATH(zephyr_user), 0);
#endif

/* Reference buttonsBits[] = { 8, 5, 13, 11, 9, 12 }. */
static uint8_t button_map[INPUT_NUM_BUTTONS] = { 8, 5, 13, 11, 9, 12 };
static struct debounce btn_db[INPUT_NUM_BUTTONS];
static struct dpad_decoder dpad;
static uint8_t dpad_prev_pos;
static atomic_t manual_bits;

/* Latency instrumentation: ring of publish-path durations (cycles). */
#define LAT_RING 128
static uint32_t lat_ring[LAT_RING];
static uint32_t lat_idx;
static uint32_t lat_max_cyc;

static K_SEM_DEFINE(tick_sem, 0, 1);
static struct k_timer tick_timer;

static void tick_handler(struct k_timer *t)
{
	ARG_UNUSED(t);
	k_sem_give(&tick_sem);
}

static void latency_note(uint32_t cyc)
{
	lat_ring[lat_idx++ % LAT_RING] = cyc;
	if (cyc > lat_max_cyc) {
		lat_max_cyc = cyc;
	}
}

void input_svc_latency_get(uint32_t *p99_us, uint32_t *max_us)
{
	uint32_t copy[LAT_RING];
	unsigned int n = MIN(lat_idx, (uint32_t)LAT_RING);

	memcpy(copy, lat_ring, sizeof(copy));

	/* insertion sort - 128 entries, shell context only */
	for (unsigned int i = 1; i < n; i++) {
		uint32_t v = copy[i];
		unsigned int j = i;

		while (j > 0 && copy[j - 1] > v) {
			copy[j] = copy[j - 1];
			j--;
		}
		copy[j] = v;
	}

	uint32_t p99_cyc = (n > 0) ? copy[(n * 99U) / 100U] : 0;

	*p99_us = (uint32_t)k_cyc_to_us_floor64(p99_cyc);
	*max_us = (uint32_t)k_cyc_to_us_floor64(lat_max_cyc);
}

uint8_t input_svc_map_get(unsigned int idx)
{
	return (idx < INPUT_NUM_BUTTONS) ? button_map[idx] : 0;
}

int input_svc_map_set(unsigned int idx, uint8_t bit)
{
	if (idx >= INPUT_NUM_BUTTONS ||
	    bit < 1 || bit > RIMLINK_BUTTON_BITS) {
		return -EINVAL;
	}
	button_map[idx] = bit;
	return 0;
}

void input_svc_manual_bit(uint8_t bit, bool on)
{
	if (bit < 1 || bit > 24) {
		return;
	}
	if (on) {
		atomic_or(&manual_bits, 1UL << (bit - 1));
	} else {
		atomic_and(&manual_bits, ~(1UL << (bit - 1)));
	}
}

void input_svc_manual_toggle(uint8_t bit)
{
	if (bit < 1 || bit > 24) {
		return;
	}
	atomic_xor(&manual_bits, 1UL << (bit - 1));
}

uint32_t input_svc_manual_get(void)
{
	return (uint32_t)atomic_get(&manual_bits);
}

int input_svc_dpad_cal(const uint16_t thr[7], uint16_t hyst)
{
	for (int i = 1; i < DPAD_THRESHOLDS; i++) {
		if (thr[i] >= thr[i - 1]) {
			return -EINVAL; /* must be strictly descending */
		}
	}
	dpad_init(&dpad, thr, hyst);
	return 0;
}

uint8_t input_svc_dpad_pos(void)
{
	return dpad.cur;
}

#if HAVE_DPAD_ADC
static int dpad_sample(uint16_t *raw)
{
	int16_t buf;
	struct adc_sequence seq = {
		.buffer = &buf,
		.buffer_size = sizeof(buf),
	};

	(void)adc_sequence_init_dt(&dpad_adc, &seq);

	int ret = adc_read_dt(&dpad_adc, &seq);

	if (ret < 0) {
		return ret;
	}
	*raw = (buf < 0) ? 0U : (uint16_t)buf;
	return 0;
}
#endif

static void input_thread(void *a, void *b, void *c)
{
	ARG_UNUSED(a);
	ARG_UNUSED(b);
	ARG_UNUSED(c);

	struct rim_inputs snap = {0};
	uint32_t prev_buttons = UINT32_MAX;

	while (1) {
		k_sem_take(&tick_sem, K_FOREVER);

		uint32_t t0 = k_cycle_get_32();
		uint32_t buttons = 0;

		/* Physical buttons: reference readButtons(). Active-low
		 * handled by devicetree flags; gpio_pin_get_dt() == 1
		 * means pressed. */
		for (unsigned int i = 0; i < ARRAY_SIZE(btn_specs); i++) {
			bool raw = gpio_pin_get_dt(&btn_specs[i]) == 1;
			bool pressed = debounce_feed(&btn_db[i], raw);

			rimlink_button_bit_change(&buttons, button_map[i],
						  pressed);
		}

		/* Analog D-pad ladder (reference thresholds + hysteresis). */
#if HAVE_DPAD_ADC
		uint16_t raw;

		if (dpad_sample(&raw) == 0) {
			uint8_t pos = dpad_decode(&dpad, raw);

			if (pos != dpad_prev_pos) {
				LOG_DBG("Dpad button: %u", pos);
			}
			dpad_prev_pos = pos;
		}
#endif
		dpad_apply_bits(&buttons, dpad_prev_pos, true);

		/* Shell override bits (reference serial command feature). */
		buttons |= input_svc_manual_get();

		/* Compose + publish the immutable snapshot; axes and
		 * encoder stay neutral in Phase 1 (spec section 2.2). */
		snap.buttons = buttons;
		snap.axis_x = 0x00;
		snap.axis_y = 0x00;
		snap.encoder = 0;
		snap.timestamp = t0;

		if (buttons != prev_buttons) {
			rimlink_snapshot_publish(&snap);
			prev_buttons = buttons;
		} else {
			/* Republish periodically to satisfy the 50 ms
			 * freshness rule while inputs are steady. */
			static uint32_t hold;

			if (++hold >= 20U) {
				rimlink_snapshot_publish(&snap);
				hold = 0;
			}
		}

#if HAVE_SNAP_TICK
		(void)gpio_pin_toggle_dt(&snap_tick);
#endif
		latency_note(k_cycle_get_32() - t0);
	}
}

K_THREAD_DEFINE(input_tid, INPUT_THREAD_STACK_SIZE,
		input_thread, NULL, NULL, NULL,
		INPUT_THREAD_PRIORITY, 0, 0);

int input_svc_init(void)
{
	int ret;

	for (unsigned int i = 0; i < ARRAY_SIZE(btn_specs); i++) {
		if (!gpio_is_ready_dt(&btn_specs[i])) {
			return -ENODEV;
		}
		/* Internal pull-ups + active-low come from devicetree
		 * flags (reference INPUT_PULLUP). */
		ret = gpio_pin_configure_dt(&btn_specs[i], GPIO_INPUT);
		if (ret < 0) {
			return ret;
		}
	}

#if HAVE_SNAP_TICK
	if (!gpio_is_ready_dt(&snap_tick) ||
	    gpio_pin_configure_dt(&snap_tick, GPIO_OUTPUT_INACTIVE) < 0) {
		return -ENODEV;
	}
#endif

#if HAVE_DPAD_ADC
	if (!adc_is_ready_dt(&dpad_adc)) {
		return -ENODEV;
	}
	ret = adc_channel_setup_dt(&dpad_adc);
	if (ret < 0) {
		return ret;
	}
#endif

	dpad_init(&dpad, NULL, 0);

	k_timer_init(&tick_timer, tick_handler, NULL);
	k_timer_start(&tick_timer, K_MSEC(1), K_MSEC(1));

	LOG_INF("1 kHz acquisition running (%u buttons, dpad %s)",
		(unsigned int)ARRAY_SIZE(btn_specs),
		HAVE_DPAD_ADC ? "adc" : "off");
	return 0;
}
