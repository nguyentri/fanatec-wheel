/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_SRC_ENCODER_H_
#define APP_SRC_ENCODER_H_

#include <stdint.h>

/**
 * Quadrature encoder decoder (spec 3-S1): full transition-table
 * decode - no interrupt-per-edge counting reaches the logic. Sampled
 * at the 1 kHz input tick; detents accumulate into a saturating int8
 * delta consumed atomically at snapshot compose.
 */

#define ENCODER_CHANNELS 4

struct encoder {
	uint8_t prev_ab;     /* previous 2-bit A/B state */
	int16_t accum;       /* sub-detent transition accumulator */
	int16_t delta;       /* detents since last consume (saturated) */
	uint32_t illegal;    /* illegal (double-step) transitions */
	uint8_t detent_div;  /* transitions per detent (typ. 4) */
};

/** Initialize with the current A/B level and detent divider (0 -> 4). */
void encoder_init(struct encoder *e, uint8_t ab, uint8_t detent_div);

/** Feed one A/B sample (bit0 = A, bit1 = B). */
void encoder_feed(struct encoder *e, uint8_t ab);

/** Consume the accumulated detent delta (saturating int8), reset it. */
int8_t encoder_consume(struct encoder *e);

#endif /* APP_SRC_ENCODER_H_ */
