/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 *
 * CRC-8 for the legacy rim link.
 *
 * Reflected polynomial 0x31 (table form 0x8C), init 0xFF, no final XOR
 * (docs/phase1-software-spec.md section 2.1, verified computationally).
 * The table is computed bitwise here and is byte-identical to the
 * 256-entry PROGMEM table in the reference sketch; the unit test in
 * tests/unit/rimlink asserts the reference vector and random-vector
 * equivalence.
 */

#include "rimlink.h"

static uint8_t crc8_update(uint8_t crc, uint8_t data)
{
	crc ^= data;
	for (int i = 0; i < 8; i++) {
		crc = (crc & 0x01U) ? (uint8_t)((crc >> 1) ^ 0x8CU)
				    : (uint8_t)(crc >> 1);
	}
	return crc;
}

uint8_t rimlink_crc8(const uint8_t *data, size_t len)
{
	uint8_t crc = 0xFFU;

	for (size_t i = 0; i < len; i++) {
		crc = crc8_update(crc, data[i]);
	}

	return crc;
}
