/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_SRC_TM1637_H_
#define APP_SRC_TM1637_H_

#include <stdint.h>

/**
 * Minimal bit-banged TM1637 4-digit driver (3.3 V module, hardware
 * spec section 5.2), enabled by CONFIG_RIM_TM1637. Protocol per the
 * TM1637 datasheet / the reference project's bundled library.
 */

int tm1637_init(void);

/** Write four raw segment bytes (digit 0..3), auto-address mode. */
int tm1637_set_segments(const uint8_t seg[4]);

/** Set brightness 0..7 and display on/off. */
int tm1637_brightness(uint8_t level, bool on);

#endif /* APP_SRC_TM1637_H_ */
