/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_SRC_INPUT_SVC_H_
#define APP_SRC_INPUT_SVC_H_

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Input service v2 (Phase 3, spec s.3): 1 kHz full-fabric
 * acquisition - 6 debounced buttons, analog D-pad, 4 quadrature
 * encoders, funky switch, 2 Hall clutch channels with dual-clutch
 * logic - composed into the unchanged adapter snapshot (buttons/
 * axes/encoder byte populated per spec 3-S7).
 */

#define INPUT_NUM_BUTTONS 6

int input_svc_init(void);

/** Button map access (persisted via rim/map, spec 3-S5). */
uint8_t input_svc_map_get(unsigned int idx);
int input_svc_map_set(unsigned int idx, uint8_t bit);

/** Manual bit override register (shell), OR-ed into every snapshot. */
void input_svc_manual_bit(uint8_t bit, bool on);
void input_svc_manual_toggle(uint8_t bit);
uint32_t input_svc_manual_get(void);

/** Publish-path latency: P99/max us (spec 3-S6 budget, <=250 us). */
void input_svc_latency_get(uint32_t *p99_us, uint32_t *max_us);

/** D-pad calibration (persisted via rim/cal). */
int input_svc_dpad_cal(const uint16_t thr[7], uint16_t hyst);
uint8_t input_svc_dpad_pos(void);

/** Extended v2 state for diagnostics (spec s.3.1 payload). */
struct input_v2_state {
	int8_t enc_delta[4];     /* last consumed deltas */
	uint32_t enc_illegal[4]; /* illegal-transition counters */
	uint8_t funky_dir;       /* enum funky_dir */
	bool funky_push;
	uint32_t funky_faults;
	uint8_t clutch[2];       /* calibrated 0..255 */
	uint8_t clutch_combined;
	uint8_t clutch_diag[2];  /* CLUTCH_DIAG_* flags */
};
void input_svc_v2_state_get(struct input_v2_state *s);

/** Live clutch mode/bite control (settings-backed, spec 3-S4). */
int input_svc_clutch_mode_set(uint8_t mode, uint8_t bite);

#endif /* APP_SRC_INPUT_SVC_H_ */
