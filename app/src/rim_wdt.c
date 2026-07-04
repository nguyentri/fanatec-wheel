/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 *
 * Watchdog service (spec 4-S6).
 */

#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/logging/log.h>

#include "rim_wdt.h"

LOG_MODULE_REGISTER(rim_wdt, CONFIG_APP_LOG_LEVEL);

static uint32_t reset_cause_flags;

#if defined(CONFIG_RIM_WATCHDOG) && DT_NODE_EXISTS(DT_ALIAS(watchdog0))
#define HAVE_WDT 1
#include <zephyr/drivers/watchdog.h>
static const struct device *const wdt_dev =
	DEVICE_DT_GET(DT_ALIAS(watchdog0));
static int wdt_chan = -1;
#else
#define HAVE_WDT 0
#endif

#ifdef CONFIG_HWINFO
#include <zephyr/drivers/hwinfo.h>
#endif

void rim_wdt_feed(void)
{
#if HAVE_WDT
	if (wdt_chan >= 0) {
		(void)wdt_feed(wdt_dev, wdt_chan);
	}
#endif
}

uint32_t rim_wdt_reset_cause(void)
{
	return reset_cause_flags;
}

int rim_wdt_init(void)
{
#ifdef CONFIG_HWINFO
	(void)hwinfo_get_reset_cause(&reset_cause_flags);
	(void)hwinfo_clear_reset_cause();
	if (reset_cause_flags & RESET_WATCHDOG) {
		LOG_WRN("previous reset: WATCHDOG");
	}
#endif

#if HAVE_WDT
	const struct wdt_timeout_cfg cfg = {
		/* IWDG: lower window bound must be 0 (deviation from
		 * the windowed requirement, PCB rev A pending). */
		.window = { .min = 0, .max = 100 },
		.flags = WDT_FLAG_RESET_SOC,
	};

	if (!device_is_ready(wdt_dev)) {
		return -ENODEV;
	}
	wdt_chan = wdt_timeout_install(wdt_dev, &cfg);
	if (wdt_chan < 0) {
		return wdt_chan;
	}

	int ret = wdt_setup(wdt_dev, 0);

	if (ret == 0) {
		LOG_INF("watchdog armed: 100 ms, fed by input thread");
	}
	return ret;
#else
	LOG_INF("watchdog disabled (CONFIG_RIM_WATCHDOG)");
	return 0;
#endif
}
