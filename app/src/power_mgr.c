/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 *
 * Output power manager (spec 4-S3).
 */

#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

#include "power_mgr.h"

LOG_MODULE_REGISTER(power_mgr, CONFIG_APP_LOG_LEVEL);

#define PWR_STALE_MS 200 /* aligned with the 50 ms x4 stale ladder */

#if DT_NODE_EXISTS(DT_NODELABEL(out_rail))
#define HAVE_RAIL 1
static const struct gpio_dt_spec rail =
	GPIO_DT_SPEC_GET(DT_NODELABEL(out_rail), gpios);
#else
#define HAVE_RAIL 0
#endif

static struct {
	struct power_stats stats;
	bool faulted;
	struct k_work_delayable stale_work;
} pm;

static void rail_set(bool on)
{
	if (pm.stats.rail_on == on) {
		return;
	}
	pm.stats.rail_on = on;
	if (on) {
		pm.stats.on_events++;
	} else {
		pm.stats.off_events++;
	}
#if HAVE_RAIL
	(void)gpio_pin_set_dt(&rail, on ? 1 : 0);
#endif
	LOG_INF("out rail %s", on ? "ON" : "off");
}

static void stale_handler(struct k_work *work)
{
	ARG_UNUSED(work);
	pm.stats.stale_trips++;
	rail_set(false);
}

void power_mgr_note_valid_rx(void)
{
	if (!pm.faulted) {
		rail_set(true); /* LINK_READY implied: rx follows arm */
	}
	k_work_reschedule(&pm.stale_work, K_MSEC(PWR_STALE_MS));
}

void power_mgr_fault(void)
{
	pm.faulted = true;
	rail_set(false);
}

void power_mgr_stats_get(struct power_stats *s)
{
	*s = pm.stats;
}

int power_mgr_init(void)
{
#if HAVE_RAIL
	if (!gpio_is_ready_dt(&rail) ||
	    gpio_pin_configure_dt(&rail, GPIO_OUTPUT_INACTIVE) < 0) {
		return -ENODEV;
	}
#endif
	k_work_init_delayable(&pm.stale_work, stale_handler);
	LOG_INF("power_mgr: rail gpio %s",
		HAVE_RAIL ? "present" : "absent (PCB rev A pending)");
	return 0;
}
