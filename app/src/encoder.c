/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 *
 * Quadrature transition-table decoder (spec 3-S1).
 *
 * Index = (prev_ab << 2) | ab. Values: +1/-1 valid quarter steps,
 * 0 no movement, +2 sentinel for illegal double transitions (both
 * lines changed in one sample - counted per channel, never counted
 * as movement; zero-loss at maximum hand rate is bounded by the
 * 1 kHz sample rate >> hand-detent rate).
 */

#include "encoder.h"

#define ILLEGAL 2

/* CW = Gray sequence 00 -> 01 -> 11 -> 10 -> 00 counts positive. */
static const int8_t transition[16] = {
	 0, +1, -1, ILLEGAL,
	-1,  0, ILLEGAL, +1,
	+1, ILLEGAL,  0, -1,
	ILLEGAL, -1, +1,  0,
};

void encoder_init(struct encoder *e, uint8_t ab, uint8_t detent_div)
{
	e->prev_ab = ab & 3U;
	e->accum = 0;
	e->delta = 0;
	e->illegal = 0;
	e->detent_div = (detent_div == 0U) ? 4U : detent_div;
}

void encoder_feed(struct encoder *e, uint8_t ab)
{
	int8_t step = transition[((e->prev_ab & 3U) << 2) | (ab & 3U)];

	e->prev_ab = ab & 3U;

	if (step == ILLEGAL) {
		e->illegal++;
		return;
	}

	e->accum += step;

	/* One detent = detent_div quarter steps. */
	while (e->accum >= e->detent_div) {
		e->accum -= e->detent_div;
		if (e->delta < INT8_MAX) {
			e->delta++;
		}
	}
	while (e->accum <= -(int16_t)e->detent_div) {
		e->accum += e->detent_div;
		if (e->delta > INT8_MIN) {
			e->delta--;
		}
	}
}

int8_t encoder_consume(struct encoder *e)
{
	int8_t d = (int8_t)e->delta;

	e->delta = 0;
	return d;
}
