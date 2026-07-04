/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 *
 * Hall clutch pipeline + dual-clutch logic (spec 3-S3 / 3-S4).
 */

#include <zephyr/sys/util.h>

#include "clutch.h"

#define PADDLE_ACTIVE_THR 32 /* 0..255: paddle considered pulled */

void clutch_chan_init(struct clutch_chan *c, const struct clutch_cal *cal)
{
	*c = (struct clutch_chan){0};
	c->cal = *cal;
	c->iir_shift = 2;      /* y += (x-y)/4 */
	c->rate_limit = 64;    /* max out step per 1 ms sample */
	c->rail_lo = 64;       /* raw below = open circuit (12-bit) */
	c->rail_hi = 4030;     /* raw above = short to rail */
}

static uint16_t median3(uint16_t a, uint16_t b, uint16_t c)
{
	uint16_t lo = MIN(a, MIN(b, c));
	uint16_t hi = MAX(a, MAX(b, c));

	return (uint16_t)(a + b + c - lo - hi);
}

uint8_t clutch_chan_feed(struct clutch_chan *c, uint16_t raw)
{
	/* Plausibility: out-of-rail windows (open/short, spec 3-S3). */
	c->diag &= (uint8_t)~(CLUTCH_DIAG_OPEN | CLUTCH_DIAG_SHORT);
	if (raw < c->rail_lo) {
		c->diag |= CLUTCH_DIAG_OPEN;
	} else if (raw > c->rail_hi) {
		c->diag |= CLUTCH_DIAG_SHORT;
	}

	/* Median-of-3 spike rejection. */
	c->med[c->med_i] = raw;
	c->med_i = (uint8_t)((c->med_i + 1U) % 3U);

	uint16_t m = median3(c->med[0], c->med[1], c->med[2]);

	/* IIR low-pass. */
	c->iir = (uint16_t)(c->iir + ((int32_t)m - c->iir) /
			    (1 << c->iir_shift));

	/* Calibration with guard bands (spec 3-S3). */
	uint16_t lo = c->cal.min + c->cal.guard;
	uint16_t hi = (c->cal.max > c->cal.guard) ?
		      (uint16_t)(c->cal.max - c->cal.guard) : 0U;
	uint32_t out255;

	if (hi <= lo) {
		out255 = 0; /* invalid calibration -> safe zero */
	} else if (c->iir <= lo) {
		out255 = 0;
	} else if (c->iir >= hi) {
		out255 = 255;
	} else {
		out255 = ((uint32_t)(c->iir - lo) * 255U) / (hi - lo);
	}

	/* Deadzone. */
	if (out255 <= c->cal.deadzone) {
		out255 = 0;
	}

	/* Rate-of-change plausibility: flag, then slew-limit. */
	int32_t step = (int32_t)out255 - c->out_prev;

	c->diag &= (uint8_t)~CLUTCH_DIAG_RATE;
	if (step > c->rate_limit || step < -(int32_t)c->rate_limit) {
		c->diag |= CLUTCH_DIAG_RATE;
		out255 = (step > 0) ? c->out_prev + c->rate_limit
				    : c->out_prev - c->rate_limit;
	}

	c->out_prev = (uint8_t)out255;
	c->out = (uint8_t)out255;
	return c->out;
}

void dual_clutch_init(struct dual_clutch *d, enum clutch_mode mode,
		      uint8_t bite_point)
{
	*d = (struct dual_clutch){ .mode = mode, .bite_point = bite_point };
}

uint8_t dual_clutch_combine(struct dual_clutch *d, uint8_t c0, uint8_t c1)
{
	switch (d->mode) {
	case CLUTCH_MODE_AXES:
		return c0; /* axes published separately in the snapshot */
	case CLUTCH_MODE_MAPPED:
		return 0;  /* paddles feed buttons via threshold */
	case CLUTCH_MODE_BITE:
	default:
		break;
	}

	bool p0 = c0 > PADDLE_ACTIVE_THR;
	bool p1 = c1 > PADDLE_ACTIVE_THR;

	if (!p0 && !p1) {
		d->launch = false;
		return 0;
	}

	if (p0 != p1) {
		/* Single paddle: direct value; it becomes the master. */
		d->master = p0 ? 0U : 1U;
		d->launch = false;
		return p0 ? c0 : c1;
	}

	/* Both pulled: launch armed - output follows the deeper paddle,
	 * and the master releasing to (or below) the bite point holds
	 * output at the bite point until the second paddle releases. */
	d->launch = true;

	uint8_t master_val = (d->master == 0U) ? c0 : c1;

	return MAX(master_val, d->bite_point);
}
