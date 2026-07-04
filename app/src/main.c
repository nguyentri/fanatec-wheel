/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 *
 * Rim firmware entry point.
 *
 * Boot order (spec fig. 10-1, spec 2-S2): fast path
 * first - adapter frames sealed, SPI armed, LINK_READY raised - then
 * all non-link services. With CONFIG_RIM_FASTBOOT the non-link init
 * is deferred to the system workqueue so main() reaches link
 * readiness with nothing else in front of it.
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <app_version.h>

#include <lib/rimlink/rimlink.h>

#include "link_spi.h"
#include "input_svc.h"
#include "output_svc.h"
#include "diag_svc.h"
#include "rim_settings.h"
#include "led_svc.h"
#include "lra_svc.h"
#include "power_mgr.h"
#include "rim_wdt.h"
#include "health.h"

LOG_MODULE_REGISTER(main, CONFIG_APP_LOG_LEVEL);

static void services_init(void)
{
	int ret;

	/* Settings load precedes acquisition (cal/map/mode consumers). */
	(void)rim_settings_init();

	ret = rim_wdt_init();
	if (ret < 0) {
		LOG_ERR("watchdog init failed (%d)", ret);
	}

	ret = led_svc_init();
	if (ret < 0) {
		LOG_ERR("led_svc init failed (%d)", ret);
	}

	ret = lra_svc_init();
	if (ret < 0) {
		LOG_ERR("lra_svc init failed (%d)", ret);
	}

	ret = power_mgr_init();
	if (ret < 0) {
		LOG_ERR("power_mgr init failed (%d)", ret);
	}

	ret = output_svc_init();
	if (ret < 0) {
		LOG_ERR("output_svc init failed (%d)", ret);
	}

	ret = input_svc_init();
	if (ret < 0) {
		LOG_ERR("input_svc init failed (%d)", ret);
	}

	(void)health_init();

	ret = diag_svc_init();
	if (ret < 0) {
		LOG_ERR("diag_svc init failed (%d)", ret);
	}

	LOG_INF("services running");
}

#ifdef CONFIG_RIM_FASTBOOT
static void deferred_init_handler(struct k_work *work)
{
	ARG_UNUSED(work);
	services_init();
}
static K_WORK_DEFINE(deferred_init_work, deferred_init_handler);
#endif

int main(void)
{
	int ret;

	/* 1. Fast path only: adapter sealed, SPI peripheral armed,
	 *    LINK_READY (t1) raised inside link_spi_init(). */
	const struct rimlink_cfg cfg = {
		.rx_cb = output_svc_rx,
	};

	(void)rimlink_init(&cfg);

	ret = link_spi_init();
	if (ret < 0) {
		LOG_ERR("link init failed (%d)", ret);
	}

	printk("Rim-Link firmware %s (identity 0x%02x, fastboot %s)\n",
	       APP_VERSION_STRING, CONFIG_RIM_IDENTITY,
	       IS_ENABLED(CONFIG_RIM_FASTBOOT) ? "on" : "off");

	/* 2. Everything else, behind link readiness. */
#ifdef CONFIG_RIM_FASTBOOT
	k_work_submit(&deferred_init_work);
#else
	services_init();
#endif

	return 0;
}
