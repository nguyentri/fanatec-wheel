/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_SRC_DIAG_SVC_H_
#define APP_SRC_DIAG_SVC_H_

/**
 * @brief Diagnostics: counters, shell, timing test points.
 *
 * Spec sections 3 and 4: shell commands `rim mosi`, `rim miso`,
 * `rim disp`, `rim btn set/clr <bit>`, `rim id <n>`, `rim stats` -
 * reproducing the reference serial-command feature so base menu
 * sequences can be exercised without physical buttons.
 */

int diag_svc_init(void);

#endif /* APP_SRC_DIAG_SVC_H_ */
