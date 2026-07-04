/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 *
 * Link-state half of the adapter: TX double buffer with freshness
 * swap, RX validation/hand-off, statistics (spec sections 5.2-5.4).
 *
 * Buffering model (fig. 5-2): two TX buffers alternate between `armed`
 * (owned by the transfer) and `staging` (owned by the adapter).
 * rimlink_snapshot_publish() writes staging and marks it fresh; the
 * re-arm point (rimlink_tx_acquire()) swaps only if fresh, otherwise
 * re-arms the previous frame. RX frames are queued (depth 4) and
 * validated/decoded on the system workqueue - never in the fast path.
 */

#include <string.h>

#include <zephyr/kernel.h>
#include <zephyr/spinlock.h>

#include "rimlink.h"

/* Stale-input safety rule: momentary bits cleared when no snapshot for
 * more than this long (spec section 5.4 / system spec). */
#define RIMLINK_STALE_MS 50

static struct {
	uint8_t buf[2][RIMLINK_FRAME_LEN];
	uint8_t armed;         /* index owned by the transfer */
	bool fresh;            /* staging sealed since last swap */
	bool stale_cleared;    /* neutral frame already substituted */
	int64_t last_publish;  /* uptime ms of last snapshot */
	uint8_t last_rx[RIMLINK_FRAME_LEN];
	struct rimlink_stats stats;
	struct rimlink_cfg cfg;
	struct k_spinlock lock;
} lnk;

K_MSGQ_DEFINE(rimlink_rx_msgq, sizeof(struct base_outputs), 4, 4);

static void rx_work_handler(struct k_work *work);
K_WORK_DEFINE(rimlink_rx_work, rx_work_handler);

static void rx_work_handler(struct k_work *work)
{
	struct base_outputs out;

	ARG_UNUSED(work);

	while (k_msgq_get(&rimlink_rx_msgq, &out, K_NO_WAIT) == 0) {
		if (lnk.cfg.rx_cb != NULL) {
			lnk.cfg.rx_cb(&out);
		}
	}
}

int rimlink_init(const struct rimlink_cfg *cfg)
{
	struct rim_inputs neutral = {0};

	if (cfg != NULL) {
		lnk.cfg = *cfg;
	}

	lnk.armed = 0;
	lnk.fresh = false;
	lnk.last_publish = k_uptime_get();
	lnk.stats.cs_gap_min_us = UINT32_MAX;

	/* Seal the identity frame so the very first transaction already
	 * carries valid header/id/CRC (reference returnData init). */
	rimlink_frame_build(lnk.buf[0], &neutral);
	rimlink_frame_build(lnk.buf[1], &neutral);

	return 0;
}

void rimlink_snapshot_publish(const struct rim_inputs *in)
{
	k_spinlock_key_t key = k_spin_lock(&lnk.lock);
	uint8_t staging = 1U - lnk.armed;

	rimlink_frame_build(lnk.buf[staging], in);
	lnk.fresh = true;
	lnk.stale_cleared = false;
	lnk.last_publish = k_uptime_get();

	k_spin_unlock(&lnk.lock, key);
}

const uint8_t *rimlink_tx_acquire(void)
{
	k_spinlock_key_t key = k_spin_lock(&lnk.lock);

	if (lnk.fresh) {
		/* Swap staging -> armed. */
		lnk.armed = 1U - lnk.armed;
		lnk.fresh = false;
	} else if (!lnk.stale_cleared &&
		   (k_uptime_get() - lnk.last_publish) > RIMLINK_STALE_MS) {
		/* Stale-input rule: clear momentary bits in the next
		 * sealed frame (spec section 5.4). */
		struct rim_inputs neutral = {0};

		rimlink_frame_build(lnk.buf[lnk.armed], &neutral);
		lnk.stale_cleared = true;
		lnk.stats.stale_clear++;
	}

	const uint8_t *frame = lnk.buf[lnk.armed];

	k_spin_unlock(&lnk.lock, key);
	return frame;
}

void rimlink_rx_submit(const uint8_t *frame, size_t len)
{
	if (len < RIMLINK_FRAME_LEN) {
		/* CS rose before 33 bytes: discard, count, resync
		 * happens by resubmission (spec section 5.4). */
		lnk.stats.short_frame++;
		return;
	}

	lnk.stats.txn_count++;
	memcpy(lnk.last_rx, frame, RIMLINK_FRAME_LEN);

	struct base_outputs out;

	if (!rimlink_frame_validate(frame, &out)) {
		/* Mismatch: counter + frame dropped for output decode;
		 * TX is unaffected (reference behavior). */
		lnk.stats.crc_err_rx++;
		return;
	}

	if (k_msgq_put(&rimlink_rx_msgq, &out, K_NO_WAIT) != 0) {
		/* Queue full: drop the oldest, keep the newest. */
		struct base_outputs discard;

		(void)k_msgq_get(&rimlink_rx_msgq, &discard, K_NO_WAIT);
		(void)k_msgq_put(&rimlink_rx_msgq, &out, K_NO_WAIT);
		lnk.stats.overrun++;
	}

	k_work_submit(&rimlink_rx_work);
}

void rimlink_note_cs_gap(uint32_t gap_us)
{
	if (gap_us < lnk.stats.cs_gap_min_us) {
		lnk.stats.cs_gap_min_us = gap_us;
	}
	if (gap_us > lnk.stats.cs_gap_max_us) {
		lnk.stats.cs_gap_max_us = gap_us;
	}
}

void rimlink_note_rearm_miss(void)
{
	lnk.stats.rearm_miss++;
}

void rimlink_stats_get(struct rimlink_stats *s)
{
	k_spinlock_key_t key = k_spin_lock(&lnk.lock);

	*s = lnk.stats;
	k_spin_unlock(&lnk.lock, key);
}

void rimlink_stats_reset(void)
{
	k_spinlock_key_t key = k_spin_lock(&lnk.lock);

	memset(&lnk.stats, 0, sizeof(lnk.stats));
	lnk.stats.cs_gap_min_us = UINT32_MAX;
	k_spin_unlock(&lnk.lock, key);
}

void rimlink_frames_snapshot(uint8_t tx[RIMLINK_FRAME_LEN],
			     uint8_t rx[RIMLINK_FRAME_LEN])
{
	k_spinlock_key_t key = k_spin_lock(&lnk.lock);

	if (tx != NULL) {
		memcpy(tx, lnk.buf[lnk.armed], RIMLINK_FRAME_LEN);
	}
	if (rx != NULL) {
		memcpy(rx, lnk.last_rx, RIMLINK_FRAME_LEN);
	}
	k_spin_unlock(&lnk.lock, key);
}
