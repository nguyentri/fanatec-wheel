/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_SRC_OUTPUT_SVC_H_
#define APP_SRC_OUTPUT_SVC_H_

#include <stdint.h>

#include <lib/rimlink/rimlink.h>

/**
 * @brief Output service (spec section 7): CRC-gated + change-gated
 * display decode, optional TM1637 mirror, LED/rumble evidence counters.
 */

int output_svc_init(void);

/** Adapter RX callback: validated base outputs (workqueue context). */
void output_svc_rx(const struct base_outputs *out);

/** Current decoded display text (3 chars + NUL) - shell `rim disp`. */
void output_svc_disp_text(char text[16]);

/** Evidence counters for the Phase 4 output decision. */
struct output_stats {
	uint16_t leds_last;
	uint8_t rumble_last[2];
	uint32_t leds_changes;
	uint32_t rumble_changes;
	uint32_t disp_changes;
};
void output_svc_stats_get(struct output_stats *s);

#endif /* APP_SRC_OUTPUT_SVC_H_ */
