/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 *
 * Input service v2.
 *
 * The 1 kHz tick acquires the full input fabric and composes one
 * immutable snapshot (spec 3-S6 budget: <= 250 us worst case,
 * measured by the latency ring, reported via `rim stats`):
 *   - 6 buttons, 2-sample debounce (v1, spec P1 s.6)
 *   - analog D-pad ladder, windows + hysteresis (v1)
 *   - 4 quadrature encoders via transition table (3-S1)
 *   - funky switch, independent debounce + chord fault (3-S2)
 *   - 2 Hall clutch channels + dual-clutch combine (3-S3/3-S4)
 * Frame enablement (3-S7): axis_x/axis_y and the encoder byte are
 * populated per the rim/map settings; buttons keep the 22-bit map.
 */

#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/util.h>
#include <string.h>

#if DT_NODE_EXISTS(DT_PATH(zephyr_user)) && \
	DT_NODE_HAS_PROP(DT_PATH(zephyr_user), io_channels)
#define ADC_CHANNEL_COUNT DT_PROP_LEN(DT_PATH(zephyr_user), io_channels)
#include <zephyr/drivers/adc.h>
#else
#define ADC_CHANNEL_COUNT 0
#endif
#define HAVE_DPAD_ADC (ADC_CHANNEL_COUNT >= 1)
#define HAVE_CLUTCH_ADC (ADC_CHANNEL_COUNT >= 3)

#include <lib/rimlink/rimlink.h>

#include "input_svc.h"
#include "rim_settings.h"
#include "debounce.h"
#include "dpad.h"
#include "encoder.h"
#include "funky.h"
#include "clutch.h"
#include "rim_wdt.h"

LOG_MODULE_REGISTER(input_svc, CONFIG_APP_LOG_LEVEL);

#define INPUT_THREAD_STACK_SIZE 2048
#define INPUT_THREAD_PRIORITY -1 /* cooperative high (spec P1 s.3.1) */

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

/* Encoders (3-S1): A/B pairs. */
#define HAVE_ENCODERS DT_NODE_EXISTS(DT_NODELABEL(enc0_a))
#if HAVE_ENCODERS
static const struct gpio_dt_spec enc_specs[ENCODER_CHANNELS][2] = {
	{ GPIO_DT_SPEC_GET(DT_NODELABEL(enc0_a), gpios),
	  GPIO_DT_SPEC_GET(DT_NODELABEL(enc0_b), gpios) },
	{ GPIO_DT_SPEC_GET(DT_NODELABEL(enc1_a), gpios),
	  GPIO_DT_SPEC_GET(DT_NODELABEL(enc1_b), gpios) },
	{ GPIO_DT_SPEC_GET(DT_NODELABEL(enc2_a), gpios),
	  GPIO_DT_SPEC_GET(DT_NODELABEL(enc2_b), gpios) },
	{ GPIO_DT_SPEC_GET(DT_NODELABEL(enc3_a), gpios),
	  GPIO_DT_SPEC_GET(DT_NODELABEL(enc3_b), gpios) },
};
#endif

/* Funky switch (3-S2). */
#define HAVE_FUNKY DT_NODE_EXISTS(DT_NODELABEL(fk_up))
#if HAVE_FUNKY
static const struct gpio_dt_spec fk_specs[5] = {
	GPIO_DT_SPEC_GET(DT_NODELABEL(fk_up), gpios),
	GPIO_DT_SPEC_GET(DT_NODELABEL(fk_down), gpios),
	GPIO_DT_SPEC_GET(DT_NODELABEL(fk_left), gpios),
	GPIO_DT_SPEC_GET(DT_NODELABEL(fk_right), gpios),
	GPIO_DT_SPEC_GET(DT_NODELABEL(fk_push), gpios),
};
#endif

#define SNAP_TICK_NODE DT_NODELABEL(snap_tick)
#define HAVE_SNAP_TICK DT_NODE_EXISTS(SNAP_TICK_NODE)
#if HAVE_SNAP_TICK
static const struct gpio_dt_spec snap_tick =
	GPIO_DT_SPEC_GET(SNAP_TICK_NODE, gpios);
#endif

#if ADC_CHANNEL_COUNT > 0
#define ADC_SPEC(node_id, prop, idx) ADC_DT_SPEC_GET_BY_IDX(node_id, idx),
static const struct adc_dt_spec adc_specs[ADC_CHANNEL_COUNT] = {
	DT_FOREACH_PROP_ELEM(DT_PATH(zephyr_user), io_channels, ADC_SPEC)
};
#endif

static struct debounce btn_db[INPUT_NUM_BUTTONS];
static struct dpad_decoder dpad;
static uint8_t dpad_prev_pos;
static atomic_t manual_bits;

static struct encoder enc[ENCODER_CHANNELS];
static struct funky funky;
static struct clutch_chan clutch[CLUTCH_CHANNELS];
static struct dual_clutch dual;
static struct input_v2_state v2;
static struct k_spinlock v2_lock;

/* Latency instrumentation (3-S6 measured budget). */
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
	return (idx < INPUT_NUM_BUTTONS) ?
		rim_settings_get()->button_map[idx] : 0;
}

int input_svc_map_set(unsigned int idx, uint8_t bit)
{
	if (idx >= INPUT_NUM_BUTTONS ||
	    bit < 1 || bit > RIMLINK_BUTTON_BITS) {
		return -EINVAL;
	}
	rim_settings_get()->button_map[idx] = bit;
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
			return -EINVAL;
		}
	}
	memcpy(rim_settings_get()->dpad_thr, thr,
	       sizeof(rim_settings_get()->dpad_thr));
	rim_settings_get()->dpad_hyst = hyst;
	dpad_init(&dpad, thr, hyst);
	return 0;
}

uint8_t input_svc_dpad_pos(void)
{
	return dpad.cur;
}

void input_svc_v2_state_get(struct input_v2_state *s)
{
	k_spinlock_key_t key = k_spin_lock(&v2_lock);

	*s = v2;
	k_spin_unlock(&v2_lock, key);
}

int input_svc_clutch_mode_set(uint8_t mode, uint8_t bite)
{
	if (mode > CLUTCH_MODE_MAPPED) {
		return -EINVAL;
	}
	rim_settings_get()->clutch_mode = mode;
	rim_settings_get()->bite_point = bite;
	dual_clutch_init(&dual, (enum clutch_mode)mode, bite);
	return 0;
}

#if ADC_CHANNEL_COUNT > 0
static int adc_sample(unsigned int idx, uint16_t *raw)
{
	int16_t buf;
	struct adc_sequence seq = {
		.buffer = &buf,
		.buffer_size = sizeof(buf),
	};

	(void)adc_sequence_init_dt(&adc_specs[idx], &seq);

	int ret = adc_read_dt(&adc_specs[idx], &seq);

	if (ret < 0) {
		return ret;
	}
	*raw = (buf < 0) ? 0U : (uint16_t)buf;
	return 0;
}
#endif

/* Funky direction -> logical bits: directions ride the D-pad bits,
 * push = D-pad button (spec s.2.4 map). Chord fault -> no input. */
static void funky_apply(uint32_t *buttons)
{
#if HAVE_FUNKY
	switch (funky_dir_get(&funky)) {
	case FUNKY_UP:
		rimlink_button_bit_change(buttons, RIM_BIT_DPAD_UP, true);
		break;
	case FUNKY_DOWN:
		rimlink_button_bit_change(buttons, RIM_BIT_DPAD_DOWN, true);
		break;
	case FUNKY_LEFT:
		rimlink_button_bit_change(buttons, RIM_BIT_DPAD_LEFT, true);
		break;
	case FUNKY_RIGHT:
		rimlink_button_bit_change(buttons, RIM_BIT_DPAD_RIGHT, true);
		break;
	default:
		break;
	}
	if (funky_push_get(&funky)) {
		rimlink_button_bit_change(buttons, RIM_BIT_DPAD_BUTTON, true);
	}
#else
	ARG_UNUSED(buttons);
#endif
}

/* Dual-clutch mode-change chord (spec 3-S4): both paddles pulled +
 * menu bit held cycles the mode; simple edge detect on the chord. */
static void clutch_chord_check(uint32_t buttons)
{
	static bool chord_prev;
	bool chord = (v2.clutch[0] > 200 && v2.clutch[1] > 200 &&
		      (buttons & BIT(RIM_BIT_MENU - 1)) != 0);

	if (chord && !chord_prev) {
		uint8_t next = (rim_settings_get()->clutch_mode + 1U) % 3U;

		(void)input_svc_clutch_mode_set(
			next, rim_settings_get()->bite_point);
		LOG_INF("clutch mode -> %u (chord)", next);
	}
	chord_prev = chord;
}

static void input_thread(void *a, void *b, void *c)
{
	ARG_UNUSED(a);
	ARG_UNUSED(b);
	ARG_UNUSED(c);

	struct rim_inputs snap = {0};
	uint32_t prev_buttons = UINT32_MAX;
	uint8_t prev_axis_sig = 0;

	while (1) {
		k_sem_take(&tick_sem, K_FOREVER);

		uint32_t t0 = k_cycle_get_32();
		uint32_t buttons = 0;
		struct rim_settings *st = rim_settings_get();

		/* Buttons (v1). */
		for (unsigned int i = 0; i < ARRAY_SIZE(btn_specs); i++) {
			bool raw = gpio_pin_get_dt(&btn_specs[i]) == 1;
			bool pressed = debounce_feed(&btn_db[i], raw);

			rimlink_button_bit_change(&buttons,
						  st->button_map[i],
						  pressed);
		}

		/* D-pad ladder (v1). */
#if HAVE_DPAD_ADC
		uint16_t raw;

		if (adc_sample(0, &raw) == 0) {
			dpad_prev_pos = dpad_decode(&dpad, raw);
		}
#endif
		dpad_apply_bits(&buttons, dpad_prev_pos, true);

		/* Encoders (3-S1). */
#if HAVE_ENCODERS
		for (unsigned int i = 0; i < ENCODER_CHANNELS; i++) {
			uint8_t ab = (gpio_pin_get_dt(&enc_specs[i][0]) == 1)
				     | ((gpio_pin_get_dt(&enc_specs[i][1])
					 == 1) << 1);
			encoder_feed(&enc[i], ab);
		}
#endif

		/* Funky switch (3-S2). */
#if HAVE_FUNKY
		uint8_t fk_mask =
			((gpio_pin_get_dt(&fk_specs[0]) == 1) << 0) |
			((gpio_pin_get_dt(&fk_specs[1]) == 1) << 1) |
			((gpio_pin_get_dt(&fk_specs[2]) == 1) << 2) |
			((gpio_pin_get_dt(&fk_specs[3]) == 1) << 3);
		funky_feed(&funky, fk_mask,
			   gpio_pin_get_dt(&fk_specs[4]) == 1);
#endif
		funky_apply(&buttons);

		/* Hall clutches (3-S3) + dual-clutch (3-S4). */
		k_spinlock_key_t key = k_spin_lock(&v2_lock);
#if HAVE_CLUTCH_ADC
		for (unsigned int ch = 0; ch < CLUTCH_CHANNELS; ch++) {
			uint16_t craw;

			if (adc_sample(1 + ch, &craw) == 0) {
				v2.clutch[ch] =
					clutch_chan_feed(&clutch[ch], craw);
				v2.clutch_diag[ch] = clutch[ch].diag;
			}
		}
#endif
		v2.clutch_combined = dual_clutch_combine(
			&dual, v2.clutch[0], v2.clutch[1]);

		/* Encoder delta consume: atomic at compose (3-S1). */
		int8_t frame_enc = 0;
#if HAVE_ENCODERS
		for (unsigned int i = 0; i < ENCODER_CHANNELS; i++) {
			v2.enc_delta[i] = encoder_consume(&enc[i]);
			v2.enc_illegal[i] = enc[i].illegal;
		}
		frame_enc = v2.enc_delta[st->encoder_sel %
					 ENCODER_CHANNELS];
#endif
#if HAVE_FUNKY
		v2.funky_dir = (uint8_t)funky_dir_get(&funky);
		v2.funky_push = funky_push_get(&funky);
		v2.funky_faults = funky.fault_count;
#endif
		k_spin_unlock(&v2_lock, key);

		/* Shell overrides. */
		buttons |= input_svc_manual_get();

		clutch_chord_check(buttons);

		/* Frame enablement (3-S7): axes per rim/map/axis. */
		if (st->axis_map == 1U) {
			snap.axis_x = v2.clutch_combined;
			snap.axis_y = 0;
		} else {
			snap.axis_x = v2.clutch[0];
			snap.axis_y = v2.clutch[1];
		}
		snap.encoder = frame_enc;
		snap.buttons = buttons;
		snap.timestamp = t0;

		uint8_t axis_sig = snap.axis_x ^ snap.axis_y ^
				   (uint8_t)snap.encoder;
		bool changed = (buttons != prev_buttons) ||
			       (axis_sig != prev_axis_sig) ||
			       (frame_enc != 0);

		if (changed) {
			rimlink_snapshot_publish(&snap);
			prev_buttons = buttons;
			prev_axis_sig = axis_sig;
		} else {
			static uint32_t hold;

			if (++hold >= 20U) {
				rimlink_snapshot_publish(&snap);
				hold = 0;
			}
		}

		/* Watchdog fed here and only here (spec 4-S6). */
		rim_wdt_feed();

#if HAVE_SNAP_TICK
		(void)gpio_pin_toggle_dt(&snap_tick);
#endif
		latency_note(k_cycle_get_32() - t0);
	}
}

K_THREAD_DEFINE(input_tid, INPUT_THREAD_STACK_SIZE,
		input_thread, NULL, NULL, NULL,
		INPUT_THREAD_PRIORITY, 0, 0);

static int gpio_in_init(const struct gpio_dt_spec *spec)
{
	if (!gpio_is_ready_dt(spec)) {
		return -ENODEV;
	}
	return gpio_pin_configure_dt(spec, GPIO_INPUT);
}

int input_svc_init(void)
{
	int ret;
	struct rim_settings *st = rim_settings_get();

	for (unsigned int i = 0; i < ARRAY_SIZE(btn_specs); i++) {
		ret = gpio_in_init(&btn_specs[i]);
		if (ret < 0) {
			return ret;
		}
	}

#if HAVE_ENCODERS
	for (unsigned int i = 0; i < ENCODER_CHANNELS; i++) {
		ret = gpio_in_init(&enc_specs[i][0]);
		ret |= gpio_in_init(&enc_specs[i][1]);
		if (ret < 0) {
			return ret;
		}
		uint8_t ab = (gpio_pin_get_dt(&enc_specs[i][0]) == 1) |
			     ((gpio_pin_get_dt(&enc_specs[i][1]) == 1) << 1);
		encoder_init(&enc[i], ab, 4);
	}
#endif

#if HAVE_FUNKY
	for (unsigned int i = 0; i < ARRAY_SIZE(fk_specs); i++) {
		ret = gpio_in_init(&fk_specs[i]);
		if (ret < 0) {
			return ret;
		}
	}
	funky_init(&funky);
#endif

#if HAVE_SNAP_TICK
	if (!gpio_is_ready_dt(&snap_tick) ||
	    gpio_pin_configure_dt(&snap_tick, GPIO_OUTPUT_INACTIVE) < 0) {
		return -ENODEV;
	}
#endif

#if ADC_CHANNEL_COUNT > 0
	for (unsigned int i = 0; i < ADC_CHANNEL_COUNT; i++) {
		if (!adc_is_ready_dt(&adc_specs[i])) {
			return -ENODEV;
		}
		ret = adc_channel_setup_dt(&adc_specs[i]);
		if (ret < 0) {
			return ret;
		}
	}
#endif

	dpad_init(&dpad, st->dpad_thr, st->dpad_hyst);
	for (unsigned int ch = 0; ch < CLUTCH_CHANNELS; ch++) {
		clutch_chan_init(&clutch[ch], &st->clutch_cal[ch]);
	}
	dual_clutch_init(&dual, (enum clutch_mode)st->clutch_mode,
			 st->bite_point);

	k_timer_init(&tick_timer, tick_handler, NULL);
	k_timer_start(&tick_timer, K_MSEC(1), K_MSEC(1));

	LOG_INF("v2 fabric: %u btn, dpad %s, enc %s, funky %s, clutch %s",
		(unsigned int)ARRAY_SIZE(btn_specs),
		HAVE_DPAD_ADC ? "on" : "off",
		HAVE_ENCODERS ? "on" : "off",
		HAVE_FUNKY ? "on" : "off",
		HAVE_CLUTCH_ADC ? "on" : "off");
	return 0;
}
