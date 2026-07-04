/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 *
 * Analog D-pad ladder decode with hysteresis (spec section 6),
 * ported from the reference sketch's readButtons()/dPadBitChange().
 */

#include <string.h>

#include <lib/rimlink/rimlink.h>

#include "dpad.h"

/*
 * Default calibration: the reference 10-bit / 5 V thresholds
 * {933, 763, 639, 505, 299, 164, 72} scaled x4 to the 12-bit DUT ADC.
 * The bench-measured ladder values replace these via `rim dpad cal`
 * (hardware spec section 5.2 note).
 */
static const uint16_t default_thr[DPAD_THRESHOLDS] = {
	3732, 3052, 2556, 2020, 1196, 656, 288
};

#define DPAD_DEFAULT_HYST 40 /* raw counts */

void dpad_init(struct dpad_decoder *d, const uint16_t *thr, uint16_t hyst)
{
	memcpy(d->thr, (thr != NULL) ? thr : default_thr, sizeof(d->thr));
	d->hyst = (hyst != 0U) ? hyst : DPAD_DEFAULT_HYST;
	d->cur = 0;
}

static uint8_t window_of(const struct dpad_decoder *d, uint16_t raw)
{
	for (uint8_t i = 0; i < DPAD_THRESHOLDS; i++) {
		if (raw > d->thr[i]) {
			return i;
		}
	}
	return DPAD_THRESHOLDS; /* position 7: D-pad button */
}

uint8_t dpad_decode(struct dpad_decoder *d, uint16_t raw)
{
	uint8_t next = window_of(d, raw);

	if (next == d->cur) {
		return d->cur;
	}

	/* Hysteresis: only leave the current window when the sample
	 * clears the boundary between cur and next by more than hyst. */
	uint8_t boundary = (next > d->cur) ? d->cur : next;
	uint16_t thr = d->thr[boundary];
	uint16_t dist = (raw > thr) ? (uint16_t)(raw - thr)
				    : (uint16_t)(thr - raw);

	if (dist > d->hyst) {
		d->cur = next;
	}

	return d->cur;
}

void dpad_apply_bits(uint32_t *buttons, uint8_t pos, bool on)
{
	switch (pos) {
	case 0: /* idle */
		break;
	case 1: /* right */
		rimlink_button_bit_change(buttons, RIM_BIT_DPAD_RIGHT, on);
		break;
	case 2: /* left */
		rimlink_button_bit_change(buttons, RIM_BIT_DPAD_LEFT, on);
		break;
	case 3: /* left+right: calibrate wheel center */
		rimlink_button_bit_change(buttons, RIM_BIT_DPAD_BUTTON, on);
		rimlink_button_bit_change(buttons, RIM_BIT_JOYSTICK_BUTTON, on);
		break;
	case 4: /* down */
		rimlink_button_bit_change(buttons, RIM_BIT_DPAD_DOWN, on);
		break;
	case 5: /* up */
		rimlink_button_bit_change(buttons, RIM_BIT_DPAD_UP, on);
		break;
	case 6: /* up+down: menu */
		rimlink_button_bit_change(buttons, RIM_BIT_MENU, on);
		break;
	case 7: /* D-pad button */
		rimlink_button_bit_change(buttons, RIM_BIT_DPAD_BUTTON, on);
		break;
	default:
		break;
	}
}
