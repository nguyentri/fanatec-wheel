/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 *
 * Host-native ztests for the app's pure logic (spec section 11):
 * 7-seg decode table incl. dot bit and unknown-glyph path, debounce,
 * and D-pad window/hysteresis logic with the reference bit mapping.
 */

#include <zephyr/ztest.h>

#include <lib/rimlink/rimlink.h>

#include "seg7.h"
#include "dpad.h"
#include "debounce.h"
#include "encoder.h"
#include "funky.h"
#include "clutch.h"
#include "lcd_svc.h"
#include "lra_svc.h"

ZTEST(seg7, test_decode_reference_table)
{
	/* Spot-check the verbatim reference table. */
	zassert_equal(seg7_to_ascii(0x3F), '0');
	zassert_equal(seg7_to_ascii(0x6D), 'S');
	zassert_equal(seg7_to_ascii(0x39), 'C');
	zassert_equal(seg7_to_ascii(0x40), '-');
	zassert_equal(seg7_to_ascii(0x50), 'r');
	zassert_equal(seg7_to_ascii(0x2A), '*');
	zassert_equal(seg7_to_ascii(0x00), ' ');

	/* Dot bit (0x80) is masked before lookup. */
	zassert_equal(seg7_to_ascii(0x80 | 0x3F), '0', "dot masked");

	/* Unknown glyphs -> '\0' (rendered as hex by the caller). */
	zassert_equal(seg7_to_ascii(0x01), '\0', "unknown glyph");
}

ZTEST(seg7, test_encode_roundtrip)
{
	const char *chars = "0123456789SCLEnFHoAbdrIPt-u_* ";

	for (const char *c = chars; *c != '\0'; c++) {
		uint8_t seg = seg7_from_ascii(*c);
		char back = seg7_to_ascii(seg);

		/* '5' encodes to the shared 'S' glyph. */
		char expect = (*c == '5') ? 'S' : *c;

		zassert_equal(back, expect, "roundtrip '%c'", *c);
	}
}

ZTEST(debounce, test_two_sample_rule)
{
	struct debounce d = {0};

	/* One raw sample must not change the state... */
	zassert_false(debounce_feed(&d, true), "single sample ignored");
	/* ...two consecutive equal samples must. */
	zassert_true(debounce_feed(&d, true), "2 samples change state");
	/* Glitch: a single low sample keeps the state. */
	zassert_true(debounce_feed(&d, false), "glitch filtered");
	zassert_true(debounce_feed(&d, true), "still pressed");
	/* Clean release. */
	zassert_true(debounce_feed(&d, false), "first low");
	zassert_false(debounce_feed(&d, false), "released after 2");
}

ZTEST(dpad, test_windows_reference_values)
{
	struct dpad_decoder d;

	dpad_init(&d, NULL, 0);

	/* Reference ladder values (10-bit x4), well inside windows:
	 * 1023->0 idle, 844->1 right, 682->2 left, 597->3 l+r,
	 * 414->4 down, 184->5 up, 145->6 menu, 0->7 dpad button. */
	zassert_equal(dpad_decode(&d, 4092), 0, "idle");
	zassert_equal(dpad_decode(&d, 3376), 1, "right");
	zassert_equal(dpad_decode(&d, 2728), 2, "left");
	zassert_equal(dpad_decode(&d, 2388), 3, "left+right");
	zassert_equal(dpad_decode(&d, 1656), 4, "down");
	zassert_equal(dpad_decode(&d, 736), 5, "up");
	zassert_equal(dpad_decode(&d, 580), 6, "up+down menu");
	zassert_equal(dpad_decode(&d, 0), 7, "dpad button");
}

ZTEST(dpad, test_hysteresis)
{
	struct dpad_decoder d;

	dpad_init(&d, NULL, 40);

	zassert_equal(dpad_decode(&d, 4092), 0, "start idle");

	/* Threshold idle/right is 3732: a sample just below stays in
	 * the hysteresis band -> position unchanged. */
	zassert_equal(dpad_decode(&d, 3712), 0, "inside band: hold");
	/* Clearing the band switches. */
	zassert_equal(dpad_decode(&d, 3600), 1, "beyond band: switch");
	/* Coming back just above the threshold: band again holds. */
	zassert_equal(dpad_decode(&d, 3750), 1, "inside band: hold new");
	zassert_equal(dpad_decode(&d, 3800), 0, "beyond band: back");
}

ZTEST(dpad, test_apply_bits_reference_map)
{
	uint32_t btn;

	/* pos 5 = up -> bit 1 */
	btn = 0;
	dpad_apply_bits(&btn, 5, true);
	zassert_equal(btn, BIT(RIM_BIT_DPAD_UP - 1), "up");

	/* pos 3 = left+right -> bits 18 + 19 (center calibration) */
	btn = 0;
	dpad_apply_bits(&btn, 3, true);
	zassert_equal(btn, BIT(RIM_BIT_DPAD_BUTTON - 1) |
			   BIT(RIM_BIT_JOYSTICK_BUTTON - 1), "l+r");

	/* pos 6 = up+down -> bit 22 (menu) */
	btn = 0;
	dpad_apply_bits(&btn, 6, true);
	zassert_equal(btn, BIT(RIM_BIT_MENU - 1), "menu");

	/* releasing clears */
	dpad_apply_bits(&btn, 6, false);
	zassert_equal(btn, 0, "release");
}


/* ---- quadrature encoder (spec 3-S1) ---- */

/* Gray sequence CW: 00 -> 01 -> 11 -> 10 -> 00 (one detent at div 4). */
static const uint8_t cw[] = { 0, 1, 3, 2 };

ZTEST(encoder, test_transition_table_detents)
{
	struct encoder e;

	encoder_init(&e, 0, 4);

	/* 3 full CW detents. */
	for (int d = 0; d < 3; d++) {
		for (int i = 1; i <= 4; i++) {
			encoder_feed(&e, cw[i % 4]);
		}
	}
	zassert_equal(encoder_consume(&e), 3, "3 CW detents");
	zassert_equal(encoder_consume(&e), 0, "consume resets");

	/* 2 CCW detents. */
	for (int d = 0; d < 2; d++) {
		for (int i = 3; i >= 0; i--) {
			encoder_feed(&e, cw[i]);
		}
	}
	zassert_equal(encoder_consume(&e), -2, "2 CCW detents");
	zassert_equal(e.illegal, 0, "no illegal transitions");
}

ZTEST(encoder, test_illegal_transitions_counted_not_moved)
{
	struct encoder e;

	encoder_init(&e, 0, 4);

	encoder_feed(&e, 3); /* 00 -> 11: both lines changed = illegal */
	encoder_feed(&e, 0); /* 11 -> 00: illegal again */
	zassert_equal(e.illegal, 2, "double transitions counted");
	zassert_equal(encoder_consume(&e), 0, "illegal is not movement");
}

ZTEST(encoder, test_saturating_delta)
{
	struct encoder e;

	encoder_init(&e, 0, 1); /* 1 transition per detent for speed */
	for (int i = 0; i < 200; i++) {
		encoder_feed(&e, cw[(i + 1) % 4]);
	}
	zassert_equal(encoder_consume(&e), INT8_MAX, "saturates at +127");
}

/* ---- funky switch (spec 3-S2) ---- */

ZTEST(funky, test_direction_and_push_independent_debounce)
{
	struct funky f;

	funky_init(&f);

	/* Single samples change nothing (2-sample rule). */
	funky_feed(&f, FUNKY_M_UP, false);
	zassert_equal(funky_dir_get(&f), FUNKY_IDLE, "1 sample ignored");
	funky_feed(&f, FUNKY_M_UP, false);
	zassert_equal(funky_dir_get(&f), FUNKY_UP, "debounced up");

	/* Push toggles without disturbing the direction. */
	funky_feed(&f, FUNKY_M_UP, true);
	funky_feed(&f, FUNKY_M_UP, true);
	zassert_true(funky_push_get(&f), "push debounced");
	zassert_equal(funky_dir_get(&f), FUNKY_UP, "dir unaffected");
}

ZTEST(funky, test_opposite_chord_is_fault_not_input)
{
	struct funky f;

	funky_init(&f);
	funky_feed(&f, FUNKY_M_UP | FUNKY_M_DOWN, false);
	funky_feed(&f, FUNKY_M_UP | FUNKY_M_DOWN, false);
	zassert_equal(funky_dir_get(&f), FUNKY_IDLE, "chord -> no input");
	zassert_true(f.fault, "fault raised");
	zassert_equal(f.fault_count, 1, "fault counted once per episode");

	funky_feed(&f, 0, false);
	funky_feed(&f, 0, false);
	zassert_false(f.fault, "fault clears with contacts");
}

/* ---- Hall clutch pipeline (spec 3-S3) ---- */

static struct clutch_cal test_cal = {
	.min = 400, .max = 3700, .guard = 50, .deadzone = 8,
};

static uint8_t settle(struct clutch_chan *c, uint16_t raw, int n)
{
	uint8_t out = 0;

	for (int i = 0; i < n; i++) {
		out = clutch_chan_feed(c, raw);
	}
	return out;
}

ZTEST(clutch, test_calibrated_range_and_deadzone)
{
	struct clutch_chan c;

	clutch_chan_init(&c, &test_cal);

	zassert_equal(settle(&c, 400, 40), 0, "released -> 0");
	zassert_equal(settle(&c, 3700, 60), 255, "pulled -> 255");

	uint8_t mid = settle(&c, 2025, 60); /* ~half travel */

	zassert_within(mid, 128, 12, "midpoint ~128 (got %u)", mid);

	/* Deadzone: just above the guard band maps below deadzone. */
	zassert_equal(settle(&c, 460, 60), 0, "deadzone -> 0");
}

ZTEST(clutch, test_median_rejects_single_spike)
{
	struct clutch_chan c;

	clutch_chan_init(&c, &test_cal);
	settle(&c, 400, 40);

	/* One wild spike between steady samples must not move the
	 * output by more than the IIR of a single median-filtered
	 * (i.e. unchanged) sample: output stays at 0. */
	clutch_chan_feed(&c, 400);
	clutch_chan_feed(&c, 3700); /* spike */
	uint8_t out = clutch_chan_feed(&c, 400);

	zassert_equal(out, 0, "median-of-3 rejects spike (got %u)", out);
}

ZTEST(clutch, test_plausibility_flags)
{
	struct clutch_chan c;

	clutch_chan_init(&c, &test_cal);

	(void)clutch_chan_feed(&c, 10);
	zassert_true(c.diag & CLUTCH_DIAG_OPEN, "open-circuit window");
	(void)clutch_chan_feed(&c, 4090);
	zassert_true(c.diag & CLUTCH_DIAG_SHORT, "short-to-rail window");
	zassert_false(c.diag & CLUTCH_DIAG_OPEN, "open cleared");

	/* Rate limit: a full-travel step in one sample is implausible;
	 * flagged and slew-limited. */
	clutch_chan_init(&c, &test_cal);
	settle(&c, 400, 40);
	settle(&c, 3700, 3); /* let median+IIR ramp hard */
	zassert_true(c.diag & CLUTCH_DIAG_RATE ||
		     c.out <= 3 * c.rate_limit, "rate limited");
}

/* ---- dual-clutch logic (spec 3-S4) ---- */

ZTEST(clutch, test_dual_clutch_bite_point_launch)
{
	struct dual_clutch d;

	dual_clutch_init(&d, CLUTCH_MODE_BITE, 100);

	zassert_equal(dual_clutch_combine(&d, 0, 0), 0, "idle");
	zassert_equal(dual_clutch_combine(&d, 255, 0), 255,
		      "single paddle direct (paddle 0 = master)");

	/* Second paddle joins: launch armed, master still deep. */
	zassert_equal(dual_clutch_combine(&d, 255, 255), 255, "both deep");

	/* Master releases toward zero while second held: output holds
	 * at the bite point (launch control). */
	zassert_equal(dual_clutch_combine(&d, 40, 255), 100,
		      "master released -> bite point held");

	/* Second paddle releases too: back to direct master value. */
	zassert_equal(dual_clutch_combine(&d, 40, 0), 40,
		      "launch done -> direct");
}

ZTEST(clutch, test_dual_clutch_other_modes)
{
	struct dual_clutch d;

	dual_clutch_init(&d, CLUTCH_MODE_AXES, 100);
	zassert_equal(dual_clutch_combine(&d, 77, 200), 77,
		      "axes mode: combined = paddle 0, both via snapshot");

	dual_clutch_init(&d, CLUTCH_MODE_MAPPED, 100);
	zassert_equal(dual_clutch_combine(&d, 255, 255), 0,
		      "mapped mode: no analog output");
}


/* ---- LCD rev-bar renderer (spec 4-S1, pure) ---- */

static bool lcd_lit(const struct lcd_frame *f, int i)
{
	return f->rgb[i][0] || f->rgb[i][1] || f->rgb[i][2];
}

ZTEST(lcd, test_render_interpolation_and_flags)
{
	struct lcd_frame f;
	const uint8_t disp[3] = {0};

	/* No bits: dark bar, no flags. */
	lcd_render(0, disp, &f);
	for (int i = 0; i < LCD_REV_SEGMENTS; i++) {
		zassert_false(lcd_lit(&f, i), "dark at %d", i);
	}
	zassert_equal(f.flags, 0, "no flags");

	/* Bit 0 -> LED 0 lit green, no red (start of ramp). */
	lcd_render(BIT(0), disp, &f);
	zassert_true(f.rgb[0][1] > 0, "LED0 green");
	zassert_equal(f.rgb[0][0], 0, "LED0 no red at position 0");

	/* Bit 8 (last logical) -> LED 14 lit red, no green. */
	lcd_render(BIT(8), disp, &f);
	zassert_true(f.rgb[14][0] > 0, "LED14 red");
	zassert_equal(f.rgb[14][1], 0, "LED14 no green at position 8");

	/* Mid position lands between LEDs: both neighbours lit. */
	lcd_render(BIT(3), disp, &f); /* centre 5.25 */
	zassert_true(lcd_lit(&f, 5), "LED5 lit");
	zassert_true(lcd_lit(&f, 6), "LED6 spill");

	/* All 9 bits: full bar coverage. */
	lcd_render(0x01FF, disp, &f);
	int lit = 0;

	for (int i = 0; i < LCD_REV_SEGMENTS; i++) {
		lit += lcd_lit(&f, i) ? 1 : 0;
	}
	zassert_true(lit >= 14, "full bar (%d lit)", lit);

	/* Flags: bits 9-15 -> flag LEDs 0-6; flag 7 reserved. */
	lcd_render((uint16_t)(BIT(9) | BIT(15)), disp, &f);
	zassert_equal(f.flags, BIT(0) | BIT(6), "flag map");
	zassert_false(f.flags & BIT(7), "flag 7 reserved (pending)");
}

/* ---- LRA cue gate (spec 4-S2, pure) ---- */

ZTEST(lra, test_gate_caps_duration_and_enforces_cooldown)
{
	struct lra_gate g = {0};

	/* Over-long cue capped at LRA_MAX_DURATION_MS. */
	zassert_equal(lra_gate_request(&g, 1000, 5000),
		      LRA_MAX_DURATION_MS, "duration cap");

	/* Inside cue + cooldown window: blocked. */
	zassert_equal(lra_gate_request(&g, 1050, 20), 0, "busy blocked");
	zassert_equal(lra_gate_request(&g, 1000 + LRA_MAX_DURATION_MS +
				       LRA_COOLDOWN_MS - 1, 20), 0,
		      "cooldown blocked");

	/* After cooldown: granted, zero-length never granted. */
	int64_t t = 1000 + LRA_MAX_DURATION_MS + LRA_COOLDOWN_MS;

	zassert_equal(lra_gate_request(&g, t, 20), 20, "granted after");
	zassert_equal(lra_gate_request(&g, t + 200, 0), 0, "zero cue");
}

ZTEST_SUITE(lcd, NULL, NULL, NULL, NULL, NULL);
ZTEST_SUITE(lra, NULL, NULL, NULL, NULL, NULL);
ZTEST_SUITE(encoder, NULL, NULL, NULL, NULL, NULL);
ZTEST_SUITE(funky, NULL, NULL, NULL, NULL, NULL);
ZTEST_SUITE(clutch, NULL, NULL, NULL, NULL, NULL);
ZTEST_SUITE(seg7, NULL, NULL, NULL, NULL, NULL);
ZTEST_SUITE(debounce, NULL, NULL, NULL, NULL, NULL);
ZTEST_SUITE(dpad, NULL, NULL, NULL, NULL, NULL);
