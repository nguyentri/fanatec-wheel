/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 *
 * Host-native ztest suite for the rimlink adapter
 * (spec section 11): CRC-8 reference vector, frame build/parse
 * round-trip, button bit-map placement.
 */

#include <string.h>

#include <zephyr/ztest.h>
#include <zephyr/random/random.h>

#include <lib/rimlink/rimlink.h>

/* Reference CRC table from the sketch: reflected poly 0x31 -> 0x8C. */
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

ZTEST(rimlink, test_frame_build_layout)
{
	uint8_t frame[RIMLINK_FRAME_LEN];
	struct rim_inputs in = {
		/* logical bits 1 (D-pad Up) and 13 (Button 1) */
		.buttons = BIT(0) | BIT(12),
	};

	rimlink_frame_build(frame, &in);

	zassert_equal(frame[0], 0xA5, "header");
	zassert_equal(frame[1], (uint8_t)rimlink_identity_get(), "identity");
	zassert_equal(frame[2], 0x01, "byte A: bit 1 = LSB");
	zassert_equal(frame[3], 0x10, "byte B: bit 13");
	zassert_equal(frame[4], 0x00, "byte C clear");
	zassert_equal(frame[32], rimlink_crc8(frame, 32), "sealed CRC");
}

ZTEST(rimlink, test_frame_validate_roundtrip)
{
	uint8_t frame[RIMLINK_FRAME_LEN] = {0};
	struct base_outputs out;

	frame[2] = 0x6D; /* 'S' */
	frame[3] = 0x3F; /* '0' */
	frame[4] = 0x39; /* 'C' */
	frame[5] = 0x34;
	frame[6] = 0x12; /* leds = 0x1234 */
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

ZTEST_SUITE(rimlink, NULL, NULL, NULL, NULL, NULL);
