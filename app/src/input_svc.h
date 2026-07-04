/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_SRC_INPUT_SVC_H_
#define APP_SRC_INPUT_SVC_H_

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Input service (spec section 6): 1 kHz scan, 2-sample
 * debounce, analog D-pad ladder decode with hysteresis, immutable
 * snapshot compose + publish, SNAPSHOT_TICK toggle, latency tracking.
 */

#define INPUT_NUM_BUTTONS 6

int input_svc_init(void);

/** Get/set the physical-button -> logical-bit map (shell `rim map`). */
uint8_t input_svc_map_get(unsigned int idx);
int input_svc_map_set(unsigned int idx, uint8_t bit);

/** Manual bit override register (shell `rim btn set/clr`), OR-ed with
 *  physical inputs into every snapshot - reproduces the reference
 *  serial command feature so base menu sequences can be driven
 *  without hardware buttons. */
void input_svc_manual_bit(uint8_t bit, bool on);
void input_svc_manual_toggle(uint8_t bit);
uint32_t input_svc_manual_get(void);

/** Latency figures: publish-path P99/max in microseconds. */
void input_svc_latency_get(uint32_t *p99_us, uint32_t *max_us);

/** D-pad calibration access (shell `rim dpad`). */
int input_svc_dpad_cal(const uint16_t thr[7], uint16_t hyst);
uint8_t input_svc_dpad_pos(void);

#endif /* APP_SRC_INPUT_SVC_H_ */
