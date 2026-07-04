/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_SRC_INPUT_SVC_H_
#define APP_SRC_INPUT_SVC_H_

/**
 * @brief Input service: 1 kHz scan, debounce, D-pad decode, snapshot.
 *
 * Spec section 6: read the six buttons (active-low, pull-ups), 2 ms
 * debounce (two equal samples), decode the D-pad ADC ladder with
 * hysteresis, compose an immutable struct rim_inputs, timestamp it,
 * publish to the adapter, toggle SNAPSHOT_TICK. Budget <= 100 us/tick.
 */

int input_svc_init(void);

#endif /* APP_SRC_INPUT_SVC_H_ */
