/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 *
 * Phase 1 rim firmware entry point (docs/phase1-software-spec.md).
 *
 * Boot order (spec fig. 10-1, fast path first): adapter + link armed
 * and LINK_READY raised before any deferred service starts.
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <app_version.h>

#include <lib/rimlink/rimlink.h>

#include "link_spi.h"
#include "input_svc.h"
#include "output_svc.h"
#include "diag_svc.h"

LOG_MODULE_REGISTER(main, CONFIG_APP_LOG_LEVEL);

int main(void)
{
	int ret;

	printk("Rim-Link Phase 1 firmware %s (identity 0x%02x)\n",
	       APP_VERSION_STRING, CONFIG_RIM_IDENTITY);

	/* 1. Fast path: adapter frames sealed, SPI peripheral armed,
	 *    LINK_READY (t1) raised. */
	const struct rimlink_cfg cfg = {
		.rx_cb = output_svc_rx,
	};

	(void)rimlink_init(&cfg);

	ret = link_spi_init();
	if (ret < 0) {
		LOG_ERR("link init failed (%d)", ret);
	}

	/* 2. Deferred init: acquisition, output decode, diagnostics. */
	ret = output_svc_init();
	if (ret < 0) {
		LOG_ERR("output_svc init failed (%d)", ret);
	}

	ret = input_svc_init();
	if (ret < 0) {
		LOG_ERR("input_svc init failed (%d)", ret);
	}

	ret = diag_svc_init();
	if (ret < 0) {
		LOG_ERR("diag_svc init failed (%d)", ret);
	}

	LOG_INF("initialized, services running");
	return 0;
}
