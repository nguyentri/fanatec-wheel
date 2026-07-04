/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 *
 * Capture ring implementation (spec 2-S1 / 2-M2).
 *
 * Fast-path contract: rimlink_cap_record() performs one relaxed
 * atomic load and returns when the ring is disarmed or frozen; when
 * armed it copies 33 bytes + metadata into preallocated RAM. No
 * logging, no allocation, no blocking.
 */

#include <string.h>

#include <zephyr/kernel.h>
#include <zephyr/sys/atomic.h>

#include "rimlink_capture.h"

static struct rimlink_cap_entry ring[RIMLINK_CAP_DEPTH];
static atomic_t armed;             /* recording enabled */
static atomic_t frozen;            /* post-mortem freeze (2-M2) */
static atomic_t freeze_on_anomaly = ATOMIC_INIT(1);
static atomic_t wr;                /* total entries written */

void rimlink_cap_start(void)
{
	atomic_set(&frozen, 0);
	atomic_set(&armed, 1);
}

void rimlink_cap_stop(void)
{
	atomic_set(&armed, 0);
}

void rimlink_cap_freeze(void)
{
	atomic_set(&frozen, 1);
}

void rimlink_cap_freeze_on_anomaly(bool en)
{
	atomic_set(&freeze_on_anomaly, en ? 1 : 0);
}

void rimlink_cap_note_anomaly(void)
{
	if (atomic_get(&armed) == 1 &&
	    atomic_get(&freeze_on_anomaly) == 1) {
		atomic_set(&frozen, 1);
	}
}

void rimlink_cap_record(enum rimlink_cap_dir dir, const uint8_t *frame,
			bool crc_ok, uint32_t cs_gap_us)
{
	if (atomic_get(&armed) != 1 || atomic_get(&frozen) == 1) {
		return;
	}

	uint32_t n = (uint32_t)atomic_inc(&wr);
	struct rimlink_cap_entry *e = &ring[n % RIMLINK_CAP_DEPTH];

	e->timestamp = k_cycle_get_32();
	e->dir = (uint8_t)dir;
	e->crc_ok = crc_ok ? 1U : 0U;
	e->cs_gap_us = (cs_gap_us > UINT16_MAX) ? UINT16_MAX
						: (uint16_t)cs_gap_us;
	memcpy(e->frame, frame, RIMLINK_FRAME_LEN);
}

void rimlink_cap_status_get(struct rimlink_cap_status *s)
{
	s->armed = atomic_get(&armed) == 1;
	s->frozen = atomic_get(&frozen) == 1;
	s->freeze_on_anomaly = atomic_get(&freeze_on_anomaly) == 1;
	s->recorded = (uint32_t)atomic_get(&wr);
	s->depth = RIMLINK_CAP_DEPTH;
}

int rimlink_cap_read(uint32_t idx, struct rimlink_cap_entry *e)
{
	uint32_t total = (uint32_t)atomic_get(&wr);
	uint32_t avail = MIN(total, (uint32_t)RIMLINK_CAP_DEPTH);

	if (idx >= avail) {
		return -ERANGE;
	}

	/* idx 0 = oldest still in the ring. */
	uint32_t first = total - avail;

	*e = ring[(first + idx) % RIMLINK_CAP_DEPTH];
	return 0;
}
