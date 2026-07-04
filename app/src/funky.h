/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_SRC_FUNKY_H_
#define APP_SRC_FUNKY_H_

#include <stdbool.h>
#include <stdint.h>

/**
 * Funky-switch / 7-way decoder (spec 3-S2): four direction contacts +
 * push, direct GPIO. Directions and push debounce independently;
 * opposite directions simultaneously = wiring/contact fault, reported
 * as a diag flag and NOT as an input.
 */

enum funky_dir {
	FUNKY_IDLE = 0,
	FUNKY_UP,
	FUNKY_DOWN,
	FUNKY_LEFT,
	FUNKY_RIGHT,
};

struct funky {
	uint8_t dir_raw_prev;   /* previous raw direction mask */
	uint8_t dir_stable;     /* debounced direction mask */
	bool push_raw_prev;
	bool push_stable;
	bool fault;             /* opposite-chord fault latched this sample */
	uint32_t fault_count;
};

/** Direction mask bits. */
#define FUNKY_M_UP BIT(0)
#define FUNKY_M_DOWN BIT(1)
#define FUNKY_M_LEFT BIT(2)
#define FUNKY_M_RIGHT BIT(3)

void funky_init(struct funky *f);

/**
 * Feed one sample: @p dir_mask of FUNKY_M_* raw contacts, @p push raw.
 * 2-sample debounce on each, independently (spec 3-S2).
 */
void funky_feed(struct funky *f, uint8_t dir_mask, bool push);

/** Current debounced direction (chords resolve to FUNKY_IDLE + fault). */
enum funky_dir funky_dir_get(const struct funky *f);

static inline bool funky_push_get(const struct funky *f)
{
	return f->push_stable;
}

#endif /* APP_SRC_FUNKY_H_ */
