/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_SRC_DPAD_H_
#define APP_SRC_DPAD_H_

#include <stdbool.h>
#include <stdint.h>

/**
 * Analog D-pad ladder decode (reference readButtons() analog section):
 * seven descending thresholds partition the ADC range into eight
 * positions:
 *   0 idle, 1 right, 2 left, 3 left+right (center-cal), 4 down,
 *   5 up, 6 up+down (menu), 7 D-pad button.
 */
#define DPAD_POSITIONS 8
#define DPAD_THRESHOLDS 7

struct dpad_decoder {
	/* Descending thresholds, raw ADC counts (calibration table). */
	uint16_t thr[DPAD_THRESHOLDS];
	/* Hysteresis band around each threshold, raw counts. */
	uint16_t hyst;
	uint8_t cur; /* current decoded position */
};

/** Initialize with a calibration table (or NULL for defaults) */
void dpad_init(struct dpad_decoder *d, const uint16_t *thr, uint16_t hyst);

/**
 * Feed one raw sample; returns the (possibly unchanged) position.
 * Position changes only when the sample clears the boundary by more
 * than the hysteresis band - readings inside a band keep the previous
 * position (spec section 6: "windows with hysteresis").
 */
uint8_t dpad_decode(struct dpad_decoder *d, uint16_t raw);

/**
 * Map a D-pad position to logical button-bit changes (reference
 * dPadBitChange()): applies @p on to the bits belonging to @p pos in
 * @p buttons.
 */
void dpad_apply_bits(uint32_t *buttons, uint8_t pos, bool on);

#endif /* APP_SRC_DPAD_H_ */
