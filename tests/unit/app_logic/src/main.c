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

ZTEST_SUITE(seg7, NULL, NULL, NULL, NULL, NULL);
ZTEST_SUITE(debounce, NULL, NULL, NULL, NULL, NULL);
ZTEST_SUITE(dpad, NULL, NULL, NULL, NULL, NULL);
