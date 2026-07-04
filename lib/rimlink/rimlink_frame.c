/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 *
 * Frame build / parse for the legacy 33-byte rim link
 * (docs/phase1-software-spec.md sections 2.2, 2.3, 5).
 *
 * No module outside lib/rimlink may reference frame offsets.
 */

#include <string.h>

#include "rimlink.h"

#ifdef CONFIG_RIM_IDENTITY
#define RIMLINK_DEFAULT_IDENTITY CONFIG_RIM_IDENTITY
#else
#define RIMLINK_DEFAULT_IDENTITY RIM_ID_PORSCHE_918_RSR
#endif

static enum rim_id current_id = (enum rim_id)RIMLINK_DEFAULT_IDENTITY;

void rimlink_identity_set(enum rim_id id)
{
	current_id = id;
}

enum rim_id rimlink_identity_get(void)
{
	return current_id;
}

void rimlink_frame_build(uint8_t frame[RIMLINK_FRAME_LEN],
			 const struct rim_inputs *in)
{
	memset(frame, 0, RIMLINK_FRAME_LEN);

	frame[RIMLINK_OFF_HEADER] = RIMLINK_HEADER;
	frame[RIMLINK_OFF_ID] = (uint8_t)current_id;

	/*
	 * Logical button bitset -> button bytes A/B/C.
	 * Spec section 2.4: bit 1 = LSB of byte A (offset 2),
	 * continuing through byte C (offset 4), bits 1..22.
	 */
	frame[RIMLINK_OFF_BUTTONS + 0] = (uint8_t)(in->buttons >> 0);
	frame[RIMLINK_OFF_BUTTONS + 1] = (uint8_t)(in->buttons >> 8);
	frame[RIMLINK_OFF_BUTTONS + 2] = (uint8_t)(in->buttons >> 16);

	frame[RIMLINK_OFF_AXIS_X] = in->axis_x;
	frame[RIMLINK_OFF_AXIS_Y] = in->axis_y;
	frame[RIMLINK_OFF_ENCODER] = (uint8_t)in->encoder;

	/* btnHub, btnPS, reserved, fwvers stay 0x00 in Phase 1. */

	frame[RIMLINK_OFF_CRC] = rimlink_crc8(frame, RIMLINK_OFF_CRC);
}

bool rimlink_frame_validate(const uint8_t frame[RIMLINK_FRAME_LEN],
			    struct base_outputs *out)
{
	if (rimlink_crc8(frame, RIMLINK_OFF_CRC) != frame[RIMLINK_OFF_CRC]) {
		return false;
	}

	if (out != NULL) {
		out->disp[0] = frame[RIMLINK_OFF_DISP + 0];
		out->disp[1] = frame[RIMLINK_OFF_DISP + 1];
		out->disp[2] = frame[RIMLINK_OFF_DISP + 2];
		out->leds = (uint16_t)frame[RIMLINK_OFF_LEDS] |
			    ((uint16_t)frame[RIMLINK_OFF_LEDS + 1] << 8);
		out->rumble[0] = frame[RIMLINK_OFF_RUMBLE + 0];
		out->rumble[1] = frame[RIMLINK_OFF_RUMBLE + 1];
	}

	return true;
}
