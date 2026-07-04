/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 *
 * 7-segment <-> ASCII tables, ported verbatim from
 * lshachar/Arduino_Fanatec_Wheel refreshAlphanumericDisplays().
 */

#include <zephyr/sys/util.h>

#include "seg7.h"

struct seg7_map {
	uint8_t seg;
	char ch;
};

static const struct seg7_map map[] = {
	{ 0x3F, '0' }, { 0x06, '1' }, { 0x5B, '2' }, { 0x4F, '3' },
	{ 0x66, '4' }, { 0x6D, 'S' }, /* also '5' in the reference */
	{ 0x7D, '6' }, { 0x07, '7' }, { 0x7F, '8' }, { 0x6F, '9' },
	{ 0x39, 'C' }, { 0x38, 'L' }, { 0x08, '_' }, { 0x79, 'E' },
	{ 0x54, 'n' }, { 0x71, 'F' }, { 0x76, 'H' }, { 0x5C, 'o' },
	{ 0x77, 'A' }, { 0x7C, 'b' }, { 0x5E, 'd' }, { 0x50, 'r' },
	{ 0x30, 'I' }, { 0x73, 'P' }, { 0x78, 't' }, { 0x40, '-' },
	{ 0x3E, 'u' },
	{ 0x2A, '*' }, /* floating 'o' (top square) - no ASCII glyph */
	{ 0x00, ' ' },
};

char seg7_to_ascii(uint8_t seg)
{
	uint8_t p = seg & 0x7FU; /* remove the dot bit */

	for (unsigned int i = 0; i < ARRAY_SIZE(map); i++) {
		if (map[i].seg == p) {
			return map[i].ch;
		}
	}
	return '\0';
}

uint8_t seg7_from_ascii(char c)
{
	/* '5' shares the 'S' glyph in the reference decode. */
	if (c == '5') {
		c = 'S';
	}
	for (unsigned int i = 0; i < ARRAY_SIZE(map); i++) {
		if (map[i].ch == c) {
			return map[i].seg;
		}
	}
	return 0x00U;
}
