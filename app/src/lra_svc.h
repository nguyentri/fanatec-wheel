/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_SRC_LRA_SVC_H_
#define APP_SRC_LRA_SVC_H_

#include <stdbool.h>
#include <stdint.h>

/**
 * LRA haptics service (spec 4-S2): two channels driven only
 * through bounded cue primitives - duration-capped, cooldown-enforced,
 * never raw pass-through. The rumble[2] frame source stays DISABLED
 * until real-base captures show the base populating those bytes for
 * this identity (field_activity.py report - measurement pending).
 */

#define LRA_CHANNELS 2
#define LRA_MAX_DURATION_MS 100
#define LRA_COOLDOWN_MS 50

/** Per-channel cue gate state (pure, unit-tested). */
struct lra_gate {
	int64_t busy_until_ms;  /* cue active until */
	int64_t ready_at_ms;    /* cooldown elapsed at */
};

/**
 * Pure gate (spec 4-S2 bounds): clamps @p duration_ms to
 * LRA_MAX_DURATION_MS, rejects cues inside the cooldown window.
 * @return granted duration in ms, or 0 when blocked.
 */
uint32_t lra_gate_request(struct lra_gate *g, int64_t now_ms,
			  uint32_t duration_ms);

struct lra_stats {
	uint32_t requested;
	uint32_t played;
	uint32_t capped;
	uint32_t blocked;
	bool source_enabled;
};

int lra_svc_init(void);
/** Bounded cue entry point (shell/diag + gated rumble source). */
int lra_svc_cue(unsigned int chan, uint8_t level, uint32_t duration_ms);
/** Frame rumble source gate (default off, spec 4-S2). */
void lra_svc_source_enable(bool en);
void lra_svc_rumble(const uint8_t rumble[2]);
void lra_svc_stats_get(struct lra_stats *s);

#endif /* APP_SRC_LRA_SVC_H_ */
