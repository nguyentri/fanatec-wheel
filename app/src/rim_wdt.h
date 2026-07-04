/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_SRC_RIM_WDT_H_
#define APP_SRC_RIM_WDT_H_

/**
 * Watchdog (Phase 4, spec 4-S6): fed exclusively from the input
 * thread's 1 kHz tick - a hung acquisition path resets the rim.
 * IWDG on STM32H7 (window lower bound 0 - a true windowed WDT is a
 * PCB rev A / part-selection item, deviation documented in
 * docs/dma-irq-budget.md). Reset cause surfaces via hwinfo into the
 * session header and Phase 5 health counters.
 */

int rim_wdt_init(void);
void rim_wdt_feed(void);
/** hwinfo reset-cause flags captured at boot (0 if unavailable). */
uint32_t rim_wdt_reset_cause(void);

#endif /* APP_SRC_RIM_WDT_H_ */
