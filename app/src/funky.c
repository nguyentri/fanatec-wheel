/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 *
 * Funky-switch decoder (spec 3-S2).
 */

#include <zephyr/sys/util.h>

#include "funky.h"

void funky_init(struct funky *f)
{
	*f = (struct funky){0};
}

void funky_feed(struct funky *f, uint8_t dir_mask, bool push)
{
	dir_mask &= 0x0FU;

	/* Directions: 2-sample debounce on the mask as a unit, so a
	 * direction change and a push never cross-couple. */
	if (dir_mask == f->dir_raw_prev && dir_mask != f->dir_stable) {
		f->dir_stable = dir_mask;
	}
	f->dir_raw_prev = dir_mask;

	/* Push: independent 2-sample debounce (spec 3-S2). */
	if (push == f->push_raw_prev && push != f->push_stable) {
		f->push_stable = push;
	}
	f->push_raw_prev = push;

	/* Opposite chords are electrically impossible on a healthy
	 * funky switch: fault, not input. */
	bool chord = ((f->dir_stable & (FUNKY_M_UP | FUNKY_M_DOWN)) ==
		      (FUNKY_M_UP | FUNKY_M_DOWN)) ||
		     ((f->dir_stable & (FUNKY_M_LEFT | FUNKY_M_RIGHT)) ==
		      (FUNKY_M_LEFT | FUNKY_M_RIGHT));

	if (chord && !f->fault) {
		f->fault_count++;
	}
	f->fault = chord;
}

enum funky_dir funky_dir_get(const struct funky *f)
{
	if (f->fault) {
		return FUNKY_IDLE;
	}
	switch (f->dir_stable) {
	case FUNKY_M_UP:
		return FUNKY_UP;
	case FUNKY_M_DOWN:
		return FUNKY_DOWN;
	case FUNKY_M_LEFT:
		return FUNKY_LEFT;
	case FUNKY_M_RIGHT:
		return FUNKY_RIGHT;
	default:
		return FUNKY_IDLE; /* idle or non-opposite diagonal */
	}
}
