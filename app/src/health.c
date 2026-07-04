/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 *
 * Health counters (spec 5-S3).
 *
 * Sampling model: a 60 s diag-context work item folds the rimlink
 * stats deltas into the persisted totals and commits. The commit
 * volume is one small settings record per minute - NVS wear at this
 * rate is decades on the 256 KiB storage partition.
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/settings/settings.h>

#include <lib/rimlink/rimlink.h>

#include "health.h"
#include "rim_wdt.h"

#ifdef CONFIG_HWINFO
#include <zephyr/drivers/hwinfo.h>
#define WDT_CAUSE RESET_WATCHDOG
#else
#define WDT_CAUSE 0
#endif

LOG_MODULE_REGISTER(health, CONFIG_APP_LOG_LEVEL);

#define HEALTH_COMMIT_PERIOD K_SECONDS(60)

static struct health_counters hc;
static struct rimlink_stats last;
static struct k_work_delayable commit_work;

static int health_set(const char *key, size_t len,
		      settings_read_cb read_cb, void *cb_arg)
{
	if (strcmp(key, "counters") == 0 && len == sizeof(hc) &&
	    read_cb(cb_arg, &hc, len) >= 0) {
		return 0;
	}
	return -ENOENT;
}

SETTINGS_STATIC_HANDLER_DEFINE(rim_health, "rimh", NULL, health_set,
			       NULL, NULL);

static void fold_stats(void)
{
	struct rimlink_stats now;

	rimlink_stats_get(&now);
	hc.txn_total += now.txn_count - last.txn_count;
	hc.err_total += (now.crc_err_rx - last.crc_err_rx) +
			(now.short_frame - last.short_frame) +
			(now.overrun - last.overrun);
	last = now;
}

int health_commit(void)
{
	fold_stats();
	return settings_save_one("rimh/counters", &hc, sizeof(hc));
}

static void commit_handler(struct k_work *work)
{
	ARG_UNUSED(work);
	(void)health_commit();
	k_work_reschedule(&commit_work, HEALTH_COMMIT_PERIOD);
}

void health_get(struct health_counters *h)
{
	fold_stats();
	*h = hc;
}

void health_note_mate(void)
{
	hc.mate_events++;
}

int health_init(void)
{
	/* Loaded by rim_settings_init()'s settings_load_subtree? No -
	 * separate subtree: load it here (diag context, boot path). */
	int ret = settings_load_subtree("rimh");

	hc.power_cycles++;
	if (rim_wdt_reset_cause() & WDT_CAUSE) {
		hc.wdt_resets++;
	}

	k_work_init_delayable(&commit_work, commit_handler);
	k_work_reschedule(&commit_work, HEALTH_COMMIT_PERIOD);
	LOG_INF("health: boot #%u, wdt resets %u", hc.power_cycles,
		hc.wdt_resets);
	return ret;
}
