/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <lib/rimlink/rimlink.h>

#include "input_svc.h"

LOG_MODULE_REGISTER(input_svc, CONFIG_APP_LOG_LEVEL);

#define INPUT_TICK_HZ 1000U
#define INPUT_THREAD_STACK_SIZE 1024
#define INPUT_THREAD_PRIORITY -1 /* cooperative high */

static K_SEM_DEFINE(tick_sem, 0, 1);
static struct k_timer tick_timer;

static void tick_handler(struct k_timer *t)
{
	ARG_UNUSED(t);
	k_sem_give(&tick_sem);
}

static void input_thread(void *a, void *b, void *c)
{
	ARG_UNUSED(a);
	ARG_UNUSED(b);
	ARG_UNUSED(c);

	struct rim_inputs snap = {0};

	while (1) {
		k_sem_take(&tick_sem, K_FOREVER);

		/*
		 * TODO(phase1):
		 *  - read BTN1..BTN6 GPIOs (hardware spec section 5.2),
		 *    per-button 2-sample debounce
		 *  - map physical buttons to logical bits {8,5,13,11,9,12}
		 *    (run-time reconfigurable via shell)
		 *  - ADC D-pad ladder -> direction bits (calibrated
		 *    windows + hysteresis from settings)
		 *  - stale-input rule: momentary bits cleared if no fresh
		 *    snapshot for > 50 ms (spec section 5.4)
		 *  - toggle SNAPSHOT_TICK test point; track P99 latency
		 */
		snap.timestamp = k_cycle_get_32();

		/* Publish to adapter staging buffer.
		 * TODO(phase1): rimlink_snapshot_publish(&snap) once the
		 * transaction state machine lands in link_spi.c.
		 */
	}
}

K_THREAD_DEFINE(input_tid, INPUT_THREAD_STACK_SIZE,
		input_thread, NULL, NULL, NULL,
		INPUT_THREAD_PRIORITY, 0, 0);

int input_svc_init(void)
{
	k_timer_init(&tick_timer, tick_handler, NULL);
	k_timer_start(&tick_timer, K_MSEC(1), K_MSEC(1));

	LOG_INF("1 kHz acquisition tick started (%u Hz)", INPUT_TICK_HZ);
	return 0;
}
