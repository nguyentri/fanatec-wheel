/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 *
 * Capture hooks (spec 2-S1): interface-neutral transaction
 * ring for real-base evidence gathering. Armed/drained only from
 * diag_svc; zero fast-path cost when disabled (compile-time gate
 * CONFIG_RIMLINK_CAPTURE + runtime atomic gate).
 */

#ifndef LIB_RIMLINK_RIMLINK_CAPTURE_H_
#define LIB_RIMLINK_RIMLINK_CAPTURE_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "rimlink.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Ring depth: >= 256 transactions (spec 2-S1). */
#define RIMLINK_CAP_DEPTH 256

enum rimlink_cap_dir {
	RIMLINK_CAP_TX = 0, /* rim -> base (armed MISO frame) */
	RIMLINK_CAP_RX = 1, /* base -> rim (received MOSI frame) */
};

struct rimlink_cap_entry {
	uint32_t timestamp;  /* k_cycle_get_32() at record time */
	uint8_t dir;         /* enum rimlink_cap_dir */
	uint8_t crc_ok;      /* RX: CRC verdict; TX: always 1 */
	uint16_t cs_gap_us;  /* latest CS-gap sample (saturated) */
	uint8_t frame[RIMLINK_FRAME_LEN];
};

#ifdef CONFIG_RIMLINK_CAPTURE

/** Arm the ring (diag context only). Clears freeze, keeps contents. */
void rimlink_cap_start(void);

/** Disarm the ring (diag context only). */
void rimlink_cap_stop(void);

/**
 * Freeze the ring for post-mortem (spec 2-M2): recording stops and
 * the ring stops wrapping until the next rimlink_cap_start(). Called
 * automatically on link anomalies while armed (CRC error, short
 * frame) when freeze-on-anomaly is enabled; also callable from shell.
 */
void rimlink_cap_freeze(void);

/** Enable/disable automatic freeze-on-anomaly (default: enabled). */
void rimlink_cap_freeze_on_anomaly(bool en);

/** Status snapshot for the shell. */
struct rimlink_cap_status {
	bool armed;
	bool frozen;
	bool freeze_on_anomaly;
	uint32_t recorded;  /* total entries recorded since start */
	uint32_t depth;     /* ring depth */
};
void rimlink_cap_status_get(struct rimlink_cap_status *s);

/**
 * Copy entry @p idx (0 = oldest available) into @p e.
 * @return 0 on success, -ERANGE past the end.
 */
int rimlink_cap_read(uint32_t idx, struct rimlink_cap_entry *e);

/**
 * Record hook - fast-path facing, called by the adapter only.
 * Single relaxed atomic test when disarmed.
 */
void rimlink_cap_record(enum rimlink_cap_dir dir, const uint8_t *frame,
			bool crc_ok, uint32_t cs_gap_us);

/** Anomaly hook - freezes the ring if armed + freeze-on-anomaly. */
void rimlink_cap_note_anomaly(void);

#else /* !CONFIG_RIMLINK_CAPTURE: compile-time zero cost */

static inline void rimlink_cap_record(enum rimlink_cap_dir dir,
				      const uint8_t *frame, bool crc_ok,
				      uint32_t cs_gap_us)
{
	(void)dir; (void)frame; (void)crc_ok; (void)cs_gap_us;
}
static inline void rimlink_cap_note_anomaly(void) {}

#endif /* CONFIG_RIMLINK_CAPTURE */

#ifdef __cplusplus
}
#endif

#endif /* LIB_RIMLINK_RIMLINK_CAPTURE_H_ */
