/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_SRC_RIM_SETTINGS_H_
#define APP_SRC_RIM_SETTINGS_H_

#include <stdbool.h>
#include <stdint.h>

#include "clutch.h"

/**
 * Settings schema v2 (spec 3-S5): namespaces rim/cal, rim/map,
 * rim/mode subtrees persisted via Zephyr settings (NVS). Commits happen
 * only from diag/shell context - never the fast path. The schema is
 * versioned; a migration stub runs when a stored version differs.
 */

#define RIM_SETTINGS_SCHEMA_VERSION 2

struct rim_settings {
	uint8_t schema;
	/* rim/cal subtree */
	struct clutch_cal clutch_cal[CLUTCH_CHANNELS];
	uint16_t dpad_thr[7];
	uint16_t dpad_hyst;
	/* rim/map subtree */
	uint8_t button_map[6];
	uint8_t encoder_sel;    /* which encoder feeds the frame byte */
	uint8_t axis_map;       /* 0: clutch->axes, 1: combined->x */
	/* rim/mode subtree */
	uint8_t clutch_mode;    /* enum clutch_mode */
	uint8_t bite_point;
	/* rim/lock (spec 5-S5) */
	bool locked;
};

int rim_settings_init(void);
struct rim_settings *rim_settings_get(void);

/** Persist one subtree ("cal", "map", "mode", "lock"). Diag context
 *  only. Returns -EACCES when the config lock is engaged (5-S5)
 *  except for the lock subtree itself. */
int rim_settings_commit(const char *subtree);

#endif /* APP_SRC_RIM_SETTINGS_H_ */
