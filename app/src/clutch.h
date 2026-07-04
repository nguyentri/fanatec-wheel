/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_SRC_CLUTCH_H_
#define APP_SRC_CLUTCH_H_

#include <stdbool.h>
#include <stdint.h>

/**
 * Hall clutch pipeline (spec 3-S3) and dual-clutch logic (3-S4).
 * Per channel: median-of-3 -> IIR -> min/max calibration with guard
 * bands -> deadzone -> 0..255; plausibility checks (rate limit,
 * open/short via out-of-rail windows) raise diag flags.
 */

#define CLUTCH_CHANNELS 2

/* Diag flags (spec 3.1 snapshot.diag). */
#define CLUTCH_DIAG_RATE BIT(0)   /* rate-of-change implausible */
#define CLUTCH_DIAG_OPEN BIT(1)   /* below open-circuit rail window */
#define CLUTCH_DIAG_SHORT BIT(2)  /* above short-to-rail window */

struct clutch_cal {
	uint16_t min;        /* raw at released, plus guard band */
	uint16_t max;        /* raw at fully pulled, minus guard band */
	uint16_t guard;      /* guard band, raw counts */
	uint8_t deadzone;    /* output deadzone, 0..255 domain */
};

struct clutch_chan {
	struct clutch_cal cal;
	uint16_t med[3];     /* median-of-3 window */
	uint8_t med_i;
	uint16_t iir;        /* filtered raw (Q0) */
	uint8_t iir_shift;   /* y += (x - y) >> shift */
	uint8_t out;         /* calibrated 0..255 */
	uint8_t out_prev;
	uint16_t rate_limit; /* max output change per sample */
	uint16_t rail_lo;    /* below = open */
	uint16_t rail_hi;    /* above = short */
	uint8_t diag;        /* CLUTCH_DIAG_* */
};

/** Dual-clutch modes (spec 3-S4), mirroring the public Fanatec paddle
 *  modes surfaced by hid-fanatecff (`Community implementation`). */
enum clutch_mode {
	CLUTCH_MODE_BITE = 0, /* bite-point combined (default) */
	CLUTCH_MODE_AXES,     /* two independent axes */
	CLUTCH_MODE_MAPPED,   /* paddles act as mappable buttons */
};

struct dual_clutch {
	enum clutch_mode mode;
	uint8_t bite_point;  /* 0..255 */
	bool launch;         /* bite-point state machine armed */
	uint8_t master;      /* which paddle went down first */
};

void clutch_chan_init(struct clutch_chan *c, const struct clutch_cal *cal);

/** Feed one raw ADC sample; returns calibrated 0..255 output. */
uint8_t clutch_chan_feed(struct clutch_chan *c, uint16_t raw);

void dual_clutch_init(struct dual_clutch *d, enum clutch_mode mode,
		      uint8_t bite_point);

/**
 * Combine the two calibrated paddles per the active mode
 * (spec 3-S4 semantics, documented in the unit tests):
 *  - BITE: first paddle pulled = master at 100 %; while the second is
 *    also pulled, releasing the master holds output at the bite point;
 *    releasing the second paddle too returns to direct master value.
 *  - AXES: returns paddle 0 (both delivered separately via snapshot).
 *  - MAPPED: analog output 0 (paddles feed buttons via threshold).
 */
uint8_t dual_clutch_combine(struct dual_clutch *d, uint8_t c0, uint8_t c1);

#endif /* APP_SRC_CLUTCH_H_ */
