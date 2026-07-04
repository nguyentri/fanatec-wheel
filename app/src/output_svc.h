/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_SRC_OUTPUT_SVC_H_
#define APP_SRC_OUTPUT_SVC_H_

/**
 * @brief Output service: display decode, TM1637 mirror, change filter.
 *
 * Spec section 7: on each CRC-valid RX frame whose disp[3] changed,
 * decode 7-segment characters to ASCII (shell `rim disp`) and mirror
 * to the TM1637 when CONFIG_RIM_TM1637=y. LED/rumble fields are
 * accumulated into counters only (Phase 4 evidence collection).
 */

int output_svc_init(void);

/** Decode one 7-segment pattern to ASCII; unknown patterns -> '\0'. */
char output_svc_7seg_to_ascii(uint8_t seg);

#endif /* APP_SRC_OUTPUT_SVC_H_ */
