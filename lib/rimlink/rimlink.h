/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 *
 * Rim-Link adapter v1 (legacy-spi) - public interface.
 *
 * Isolates every observed protocol constant of the legacy 33-byte SPI
 * exchange (see docs/phase1-software-spec.md, sections 2 and 5). All
 * protocol facts are `observed` community facts, re-verified in Phase 2.
 */

#ifndef LIB_RIMLINK_RIMLINK_H_
#define LIB_RIMLINK_RIMLINK_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Transaction length: 33 bytes per CS assertion (both directions). */
#define RIMLINK_FRAME_LEN 33U

/** Frame header constant (byte 0, rim -> base). */
#define RIMLINK_HEADER 0xA5U

/** Rim identities (frame byte 1). */
enum rim_id {
	RIM_ID_BMW_GT2 = 0x01,
	RIM_ID_CLUBSPORT_FORMULA = 0x02,
	RIM_ID_PORSCHE_918_RSR = 0x03, /* Phase 1 default (reference parity) */
	RIM_ID_UNIHUB = 0x04,
};

/** Rim -> base frame field offsets (csw_in_t, spec section 2.2). */
#define RIMLINK_OFF_HEADER 0U
#define RIMLINK_OFF_ID 1U
#define RIMLINK_OFF_BUTTONS 2U /* buttons[3]: bytes A/B/C */
#define RIMLINK_OFF_AXIS_X 5U
#define RIMLINK_OFF_AXIS_Y 6U
#define RIMLINK_OFF_ENCODER 7U
#define RIMLINK_OFF_BTN_HUB 8U /* btnHub[2] */
#define RIMLINK_OFF_BTN_PS 10U /* btnPS[2] */
#define RIMLINK_OFF_FWVERS 31U
#define RIMLINK_OFF_CRC 32U

/** Base -> rim frame field offsets (csw_out_t, spec section 2.3). */
#define RIMLINK_OFF_DISP 2U   /* disp[3]: 7-segment characters */
#define RIMLINK_OFF_LEDS 5U   /* 16-bit LED bitfield */
#define RIMLINK_OFF_RUMBLE 7U /* rumble[2] */

/** Logical input snapshot published by input_svc (spec section 5.1). */
struct rim_inputs {
	uint32_t buttons;  /* logical button bitset, bit 1..22 per spec 2.4 */
	uint8_t axis_x;    /* 0x00 neutral in Phase 1 */
	uint8_t axis_y;    /* 0x00 neutral in Phase 1 */
	int8_t encoder;    /* int8 delta; 0 in Phase 1 */
	uint32_t timestamp; /* cycle-counter timestamp of acquisition */
};

/** Validated base outputs delivered via rimlink_rx_cb (spec section 5.1). */
struct base_outputs {
	uint8_t disp[3];   /* raw 7-segment characters, bit 7 = dot */
	uint16_t leds;     /* LED bitfield - counted/logged only in Phase 1 */
	uint8_t rumble[2]; /* rumble channels - counted/logged only */
};

/** Link statistics (spec section 5.1 / 5.4). */
struct rimlink_stats {
	uint32_t txn_count;
	uint32_t crc_err_rx;
	uint32_t rearm_miss;
	uint32_t short_frame;
	uint32_t overrun;
	uint32_t cs_gap_min_us;
	uint32_t cs_gap_max_us;
};

/**
 * CRC-8 over @p len bytes of @p data.
 *
 * Exact definition (verified computationally, spec section 2.1):
 * reflected polynomial 0x31 (table form 0x8C), init 0xFF, no final XOR.
 * Test vector: A5 03 00x30 -> 0x5A.
 */
uint8_t rimlink_crc8(const uint8_t *data, size_t len);

/**
 * Build and seal the next rim -> base TX frame from a logical snapshot.
 *
 * Writes RIMLINK_FRAME_LEN bytes to @p frame: header, current identity,
 * button bytes A/B/C from the logical bitset, axes, encoder, zeroed
 * reserved fields, and CRC-8 in byte 32.
 */
void rimlink_frame_build(uint8_t frame[RIMLINK_FRAME_LEN],
			 const struct rim_inputs *in);

/**
 * Validate a received base -> rim frame (CRC over bytes 0-31 vs byte 32)
 * and decode the output fields into @p out.
 *
 * @return true when CRC matches, false otherwise (caller increments
 *         crc_err_rx and drops the frame for output purposes).
 */
bool rimlink_frame_validate(const uint8_t frame[RIMLINK_FRAME_LEN],
			    struct base_outputs *out);

/** Set rim identity; takes effect at the next frame build. */
void rimlink_identity_set(enum rim_id id);

/** Get the currently configured rim identity. */
enum rim_id rimlink_identity_get(void);

#ifdef __cplusplus
}
#endif

#endif /* LIB_RIMLINK_RIMLINK_H_ */
