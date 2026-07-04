/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_SRC_HEALTH_H_
#define APP_SRC_HEALTH_H_

#include <stdint.h>

/**
 * Persisted health counters (Phase 5, spec 5-S3): power cycles, mate
 * events, transaction/error totals, watchdog resets, thermal
 * excursions. Accumulated in RAM at runtime; committed to settings
 * periodically from the diag workqueue context only - never the fast
 * path. `rim health` dumps them.
 */

struct health_counters {
	uint32_t power_cycles;
	uint32_t mate_events;     /* hook: QR mate detect (PCB pending) */
	uint64_t txn_total;
	uint64_t err_total;       /* crc + short + overrun accumulated */
	uint32_t wdt_resets;
	uint32_t thermal_events;  /* die-temp excursions (sensor pending) */
};

int health_init(void);
void health_get(struct health_counters *h);
/** Called by the QR mate-detect hook on PCB rev A (stub today). */
void health_note_mate(void);
/** Persist now (diag context). @return settings result. */
int health_commit(void);

#endif /* APP_SRC_HEALTH_H_ */
