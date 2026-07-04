/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 *
 * Host-native ztest suite for the rimlink adapter (spec section 11):
 * CRC-8 reference vector + 200-random-vector equivalence, frame
 * build/parse round-trip, button bit-map (all 22 bits), double-buffer
 * swap semantics and the 50 ms stale-input clearing rule.
 */

#include <string.h>

#include <zephyr/ztest.h>
#include <zephyr/random/random.h>

#include <lib/rimlink/rimlink.h>

/* Independent bitwise reference: reflected poly 0x31 -> 0x8C. */
static uint8_t ref_crc8(const uint8_t *d, size_t len)
{
	uint8_t crc = 0xFF;

	while (len--) {
		crc ^= *d++;
		for (int i = 0; i < 8; i++) {
			crc = (crc & 1U) ? (crc >> 1) ^ 0x8CU : crc >> 1;
		}
	}
	return crc;
}

ZTEST(rimlink, test_crc8_reference_vector)
{
	/* Spec section 2.1: A5 03 00x30 -> 0x5A (verified vector). */
	uint8_t vec[32] = { 0xA5, 0x03 };

	zassert_equal(rimlink_crc8(vec, sizeof(vec)), 0x5A,
		      "reference vector failed");
}

ZTEST(rimlink, test_neutral_identity_frame_seal)
{
	/* The neutral id=0x03 frame must seal to 0x5A. Note: the
	 * reference returnData initializer ends in 0xFC, but that is a
	 * dead placeholder - calcOutgoingCrc() overwrites byte 32 on
	 * every loop pass before anything is transmitted (0xFC matches
	 * no valid identity; verified against the ported table). */
	uint8_t frame[RIMLINK_FRAME_LEN];
	struct rim_inputs neutral = {0};

	rimlink_identity_set(RIM_ID_PORSCHE_918_RSR);
	rimlink_frame_build(frame, &neutral);
	zassert_equal(frame[RIMLINK_OFF_CRC], 0x5A,
		      "sealed neutral frame CRC");
}

ZTEST(rimlink, test_crc8_random_vectors)
{
	/* 200 random-vector equivalence against the ported table. */
	uint8_t buf[32];

	for (int n = 0; n < 200; n++) {
		sys_rand_get(buf, sizeof(buf));
		zassert_equal(rimlink_crc8(buf, sizeof(buf)),
			      ref_crc8(buf, sizeof(buf)),
			      "random vector %d mismatch", n);
	}
}

ZTEST(rimlink, test_button_bitmap_all_22_bits)
{
	/* Every logical bit 1..22 must land at the reference position:
	 * bit 1 = LSB of byte A (offset 2) .. bit 22 = bit 5 of byte C. */
	for (uint8_t bit = 1; bit <= RIMLINK_BUTTON_BITS; bit++) {
		uint8_t frame[RIMLINK_FRAME_LEN];
		struct rim_inputs in = {0};

		rimlink_button_bit_change(&in.buttons, bit, true);
		rimlink_frame_build(frame, &in);

		unsigned int byte = 2U + (bit - 1U) / 8U;
		uint8_t mask = 1U << ((bit - 1U) % 8U);

		for (unsigned int off = 2; off <= 4; off++) {
			uint8_t expect = (off == byte) ? mask : 0x00;

			zassert_equal(frame[off], expect,
				      "bit %u: frame[%u]=0x%02X", bit, off,
				      frame[off]);
		}

		rimlink_button_bit_change(&in.buttons, bit, false);
		zassert_equal(in.buttons, 0, "bit %u clear", bit);
	}
}

ZTEST(rimlink, test_frame_build_layout)
{
	uint8_t frame[RIMLINK_FRAME_LEN];
	struct rim_inputs in = {
		.buttons = BIT(RIM_BIT_DPAD_UP - 1) |
			   BIT(RIM_BIT_BUTTON_1 - 1),
		.encoder = -3,
	};

	rimlink_identity_set(RIM_ID_PORSCHE_918_RSR);
	rimlink_frame_build(frame, &in);

	zassert_equal(frame[0], 0xA5, "header");
	zassert_equal(frame[1], 0x03, "identity");
	zassert_equal(frame[2], 0x01, "byte A: bit 1 = LSB");
	zassert_equal(frame[3], 0x10, "byte B: bit 13");
	zassert_equal(frame[4], 0x00, "byte C clear");
	zassert_equal(frame[7], (uint8_t)-3, "encoder int8");
	zassert_equal(frame[31], 0x00, "fwvers");
	zassert_equal(frame[32], rimlink_crc8(frame, 32), "sealed CRC");

	/* Identity switch takes effect on next build. */
	rimlink_identity_set(RIM_ID_CLUBSPORT_FORMULA);
	rimlink_frame_build(frame, &in);
	zassert_equal(frame[1], 0x02, "identity switched");
	rimlink_identity_set(RIM_ID_PORSCHE_918_RSR);
}

ZTEST(rimlink, test_frame_validate_roundtrip)
{
	uint8_t frame[RIMLINK_FRAME_LEN] = {0};
	struct base_outputs out;

	frame[2] = 0x6D; /* 'S' */
	frame[3] = 0x3F; /* '0' */
	frame[4] = 0x39; /* 'C' */
	frame[5] = 0x34;
	frame[6] = 0x12; /* leds = 0x1234, little-endian in frame */
	frame[7] = 0xAA;
	frame[8] = 0x55;
	frame[32] = rimlink_crc8(frame, 32);

	zassert_true(rimlink_frame_validate(frame, &out), "valid frame");
	zassert_equal(out.disp[0], 0x6D, "disp0");
	zassert_equal(out.leds, 0x1234, "leds LE");
	zassert_equal(out.rumble[1], 0x55, "rumble1");

	frame[10] ^= 0xFF; /* corrupt */
	zassert_false(rimlink_frame_validate(frame, &out),
		      "CRC mismatch must be rejected");
}

/* ---- adapter link-state: double buffer + stale rule ---- */

static struct base_outputs last_out;
static int rx_calls;

static void test_rx_cb(const struct base_outputs *out)
{
	last_out = *out;
	rx_calls++;
}

ZTEST(rimlink_link, test_swap_and_stale_rule)
{
	const struct rimlink_cfg cfg = { .rx_cb = test_rx_cb };
	struct rim_inputs in = {0};

	zassert_ok(rimlink_init(&cfg));

	/* Initial armed frame: neutral identity frame. */
	const uint8_t *f0 = rimlink_tx_acquire();

	zassert_equal(f0[0], 0xA5, "identity frame armed");
	zassert_equal(f0[2] | f0[3] | f0[4], 0, "buttons neutral");

	/* Publish a snapshot -> next acquire returns the swapped buffer
	 * with the button set. */
	rimlink_button_bit_change(&in.buttons, RIM_BIT_MENU, true);
	rimlink_snapshot_publish(&in);

	const uint8_t *f1 = rimlink_tx_acquire();

	zassert_not_equal((uintptr_t)f0, (uintptr_t)f1, "buffer swapped");
	zassert_equal(f1[4], 0x20, "bit 22 in byte C");

	/* No further publish: same (previous) frame re-armed. */
	zassert_equal((uintptr_t)rimlink_tx_acquire(), (uintptr_t)f1,
		      "re-arm previous frame when not fresh");

	/* Stale rule: after >50 ms without a snapshot, momentary bits
	 * are cleared in the next sealed frame (spec section 5.4). */
	k_sleep(K_MSEC(60));

	const uint8_t *f2 = rimlink_tx_acquire();

	zassert_equal(f2[2] | f2[3] | f2[4], 0, "stale bits cleared");

	struct rimlink_stats s;

	rimlink_stats_get(&s);
	zassert_equal(s.stale_clear, 1, "stale_clear counted");
}

ZTEST(rimlink_link, test_rx_paths)
{
	const struct rimlink_cfg cfg = { .rx_cb = test_rx_cb };
	uint8_t frame[RIMLINK_FRAME_LEN] = {0};
	struct rimlink_stats s;

	zassert_ok(rimlink_init(&cfg));
	rimlink_stats_reset();
	rx_calls = 0;

	/* Short frame: counted, no callback. */
	rimlink_rx_submit(frame, 10);
	rimlink_stats_get(&s);
	zassert_equal(s.short_frame, 1, "short_frame");
	zassert_equal(s.txn_count, 0, "short not a txn");

	/* CRC error: counted, no callback. */
	frame[32] = 0x00;
	frame[5] = 0x01;
	rimlink_rx_submit(frame, RIMLINK_FRAME_LEN);
	rimlink_stats_get(&s);
	zassert_equal(s.crc_err_rx, 1, "crc_err_rx");
	zassert_equal(s.txn_count, 1, "txn counted");

	/* Valid frame: callback from workqueue with decoded fields. */
	frame[2] = 0x50; /* 'r' */
	frame[5] = 0xCD;
	frame[6] = 0xAB;
	frame[32] = rimlink_crc8(frame, 32);
	rimlink_rx_submit(frame, RIMLINK_FRAME_LEN);
	k_sleep(K_MSEC(10)); /* let the system workqueue run */

	zassert_equal(rx_calls, 1, "rx callback invoked");
	zassert_equal(last_out.leds, 0xABCD, "leds decoded");
	zassert_equal(last_out.disp[0], 0x50, "disp decoded");
}

ZTEST_SUITE(rimlink, NULL, NULL, NULL, NULL, NULL);
ZTEST_SUITE(rimlink_link, NULL, NULL, NULL, NULL, NULL);
