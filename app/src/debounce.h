/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_SRC_DEBOUNCE_H_
#define APP_SRC_DEBOUNCE_H_

#include <stdbool.h>
#include <stdint.h>

/**
 * Per-button 2-sample debounce (spec section 6): the debounced state
 * changes only after two consecutive equal raw samples (2 ms at the
 * 1 kHz tick).
 */
struct debounce {
	bool state; /* debounced state */
	bool prev;  /* previous raw sample */
};

static inline bool debounce_feed(struct debounce *d, bool raw)
{
	if (raw == d->prev && raw != d->state) {
		d->state = raw;
	}
	d->prev = raw;
	return d->state;
}

#endif /* APP_SRC_DEBOUNCE_H_ */
