/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 *
 * Phase 1 rim firmware entry point (docs/phase1-software-spec.md).
 *
 * Boot order follows spec section 10 (fast path first): the link is
 * armed and LINK_READY raised before any deferred service starts.
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <app_version.h>

#include "link_spi.h"
#include "input_svc.h"
#include "output_svc.h"
#include "diag_svc.h"

LOG_MODULE_REGISTER(main, CONFIG_APP_LOG_LEVEL);

int main(void)
{
	int ret;

	printk("Rim-Link Phase 1 firmware %s\n", APP_VERSION_STRING);

	/* 1. Fast path: SPI peripheral armed, identity frame sealed,
	 *    LINK_READY (t1) raised. Must beat the base first-poll deadline.
	 */
	ret = link_spi_init();
	if (ret < 0) {
		LOG_ERR("link init failed (%d)", ret);
		/* FAULT state: counters + LD3 per spec section 5.4 */
	}

	/* 2. Deferred init: acquisition, output decode, diagnostics. */
	ret = input_svc_init();
	if (ret < 0) {
		LOG_ERR("input_svc init failed (%d)", ret);
	}

	ret = output_svc_init();
	if (ret < 0) {
		LOG_ERR("output_svc init failed (%d)", ret);
	}

	ret = diag_svc_init();
	if (ret < 0) {
		LOG_ERR("diag_svc init failed (%d)", ret);
	}

	LOG_INF("initialized, services running");

	while (1) {
		/* Main thread idles; periodic health line comes from diag_svc. */
		k_sleep(K_SECONDS(5));
	}

	return 0;
}
