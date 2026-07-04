/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_SRC_SEG7_H_
#define APP_SRC_SEG7_H_

#include <stdint.h>

/**
 * Decode one 7-segment pattern (bit 7 = dot, masked internally) to
 * ASCII. Table ported verbatim from the reference sketch's
 * refreshAlphanumericDisplays(). Unknown patterns return '\0' and the
 * caller renders them as hex (reference printHex behavior).
 */
char seg7_to_ascii(uint8_t seg);

/**
 * Encode an ASCII character to its 7-segment pattern (dot bit clear),
 * the inverse table - used by the simulator's display-text injection.
 * Unsupported characters return 0x00 (blank).
 */
uint8_t seg7_from_ascii(char c);

#endif /* APP_SRC_SEG7_H_ */
