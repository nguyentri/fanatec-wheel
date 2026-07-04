/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_SRC_POWER_MGR_H_
#define APP_SRC_POWER_MGR_H_

#include <stdbool.h>
#include <stdint.h>

/**
 * Output power manager (spec 4-S3): LED/haptic rail behind
 * a load switch, enabled only after LINK_READY + first valid
 * transaction, disabled on stale link or fault so a peripheral
 * event can never load the link supply during enumeration.
 * The load-switch GPIO lives on PCB rev A (`out_rail` node -
 * hardware pending); the state machine and counters run regardless.
 */

struct power_stats {
	bool rail_on;
	uint32_t on_events;
	uint32_t off_events;
	uint32_t stale_trips;
};

int power_mgr_init(void);
/** Called from output_svc rx context on every CRC-valid frame. */
void power_mgr_note_valid_rx(void);
/** Latch a fault: rail off until reboot (diag decision). */
void power_mgr_fault(void);
void power_mgr_stats_get(struct power_stats *s);

#endif /* APP_SRC_POWER_MGR_H_ */
