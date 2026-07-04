/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 *
 * Rim-Link adapter v1 (legacy-spi) - public interface.
 *
 * Isolates every observed protocol constant of the legacy 33-byte SPI
 * exchange (docs/phase1-software-spec.md sections 2 and 5), ported from
 * lshachar/Arduino_Fanatec_Wheel (`Community implementation`). All
 * protocol facts are `observed`, re-verified on real hardware in Phase 2.
 * No module outside lib/rimlink may reference frame offsets.
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

/** Number of defined logical button bits (spec section 2.4). */
#define RIMLINK_BUTTON_BITS 22U

/** Rim identities (frame byte 1). */
enum rim_id {
	RIM_ID_BMW_GT2 = 0x01,
	RIM_ID_CLUBSPORT_FORMULA = 0x02,
	RIM_ID_PORSCHE_918_RSR = 0x03, /* Phase 1 default (reference parity) */
	RIM_ID_UNIHUB = 0x04,
};

/*
 * Logical button bit numbers (1-based, matching the reference sketch's
 * tail comment): bit 1 = LSB of byte A (offset 2) .. bit 22 in byte C.
 */
enum rim_button_bit {
	RIM_BIT_DPAD_UP = 1,
	RIM_BIT_DPAD_LEFT = 2,
	RIM_BIT_DPAD_RIGHT = 3,
	RIM_BIT_DPAD_DOWN = 4,
	RIM_BIT_BUTTON_11 = 5,
	RIM_BIT_BUTTON_3 = 6,
	RIM_BIT_BUTTON_6 = 7,
	RIM_BIT_BUTTON_4 = 8,
	RIM_BIT_RIGHT_PADDLE = 9,
	RIM_BIT_BUTTON_2 = 10,
	RIM_BIT_BUTTON_8 = 11,
	RIM_BIT_LEFT_PADDLE = 12,
	RIM_BIT_BUTTON_1 = 13,
	RIM_BIT_BUTTON_5 = 14,
	RIM_BIT_BUTTON_9 = 15,
	RIM_BIT_BUTTON_10 = 16,
	RIM_BIT_BUTTON_21_CM = 17,
	RIM_BIT_DPAD_BUTTON = 18,
	RIM_BIT_JOYSTICK_BUTTON = 19,
	RIM_BIT_BUTTON_7 = 20,
	RIM_BIT_BUTTON_27_CM = 21,
	RIM_BIT_MENU = 22,
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
	uint32_t buttons;   /* logical bitset: BIT(n-1) = button bit n */
	uint8_t axis_x;     /* 0x00 neutral in Phase 1 */
	uint8_t axis_y;     /* 0x00 neutral in Phase 1 */
	int8_t encoder;     /* int8 delta; 0 in Phase 1 (no encoder) */
	uint32_t timestamp; /* cycle-counter timestamp of acquisition */
};

/** Validated base outputs delivered via the RX callback. */
struct base_outputs {
	uint8_t disp[3];   /* raw 7-segment characters, bit 7 = dot */
	uint16_t leds;     /* LED bitfield - counted/logged only in Phase 1 */
	uint8_t rumble[2]; /* rumble channels - counted/logged only */
};

/** Link statistics (spec sections 5.1 / 5.4). */
struct rimlink_stats {
	uint32_t txn_count;   /* completed 33-byte transactions */
	uint32_t crc_err_rx;  /* RX CRC mismatches */
	uint32_t rearm_miss;  /* CS fell while no fresh re-arm completed */
	uint32_t short_frame; /* CS rose before 33 bytes */
	uint32_t overrun;     /* RX hand-off queue full, oldest dropped */
	uint32_t stale_clear; /* snapshots >50 ms old cleared (safety rule) */
	uint32_t cs_gap_min_us;
	uint32_t cs_gap_max_us;
};

/** Adapter configuration. */
struct rimlink_cfg {
	/**
	 * Validated-output callback: invoked from the system workqueue
	 * (never ISR) for every CRC-valid base -> rim frame.
	 */
	void (*rx_cb)(const struct base_outputs *out);
};

/* ------------------------------------------------------------------ */
/* Pure protocol functions (host-native testable, no kernel state)     */
/* ------------------------------------------------------------------ */

/**
 * CRC-8 over @p len bytes of @p data.
 *
 * Table ported verbatim from the reference sketch (polynomial 0x131
 * per its comment; equivalently reflected poly 0x31 / table form 0x8C),
 * init 0xFF, no final XOR. Verified vector: A5 03 00x30 -> 0x5A.
 */
uint8_t rimlink_crc8(const uint8_t *data, size_t len);

/**
 * Build and seal a rim -> base TX frame from a logical snapshot using
 * identity @p id: header, buttons A/B/C, axes, encoder, zeroed reserved
 * fields, CRC-8 in byte 32.
 */
void rimlink_frame_build_id(uint8_t frame[RIMLINK_FRAME_LEN],
			    const struct rim_inputs *in, enum rim_id id);

/** As rimlink_frame_build_id() with the currently configured identity. */
void rimlink_frame_build(uint8_t frame[RIMLINK_FRAME_LEN],
			 const struct rim_inputs *in);

/**
 * Validate a base -> rim frame (CRC over bytes 0-31 vs byte 32) and
 * decode the output fields into @p out (may be NULL).
 *
 * @return true when the CRC matches.
 */
bool rimlink_frame_validate(const uint8_t frame[RIMLINK_FRAME_LEN],
			    struct base_outputs *out);

/**
 * Set/clear logical button bit @p bit (1..22) in @p buttons - the
 * bitset analogue of the reference sketch's buttonBitChange().
 */
void rimlink_button_bit_change(uint32_t *buttons, uint8_t bit, bool on);

/** Set rim identity; takes effect at the next frame build. */
void rimlink_identity_set(enum rim_id id);

/** Get the currently configured rim identity. */
enum rim_id rimlink_identity_get(void);

/* ------------------------------------------------------------------ */
/* Link-state functions (double buffer, stats - kernel required)       */
/* ------------------------------------------------------------------ */

/** Initialize adapter state; seals the identity frame in both buffers. */
int rimlink_init(const struct rimlink_cfg *cfg);

/**
 * Publish a fresh input snapshot: builds and seals the staging TX
 * frame and marks it fresh (spec section 5.3). Any context except ISR.
 */
void rimlink_snapshot_publish(const struct rim_inputs *in);

/**
 * Re-arm point (CS rising / transaction boundary): swap staging ->
 * armed if fresh, apply the >50 ms stale-input clearing rule
 * (spec section 5.4), and return the armed, immutable 33-byte frame
 * for the next transaction. ISR-safe.
 */
const uint8_t *rimlink_tx_acquire(void);

/**
 * Submit a completed RX transfer of @p len bytes. Short lengths count
 * short_frame and are discarded; full frames are CRC-validated
 * (crc_err_rx on mismatch) and valid outputs are queued to the
 * workqueue callback (overrun counted when the queue is full).
 * Call from thread context (the link thread).
 */
void rimlink_rx_submit(const uint8_t *frame, size_t len);

/** Record a CS-gap measurement in microseconds. ISR-safe. */
void rimlink_note_cs_gap(uint32_t gap_us);

/** Count a re-arm miss (CS fell before re-arm completed). ISR-safe. */
void rimlink_note_rearm_miss(void);

/** Copy current statistics into @p s. */
void rimlink_stats_get(struct rimlink_stats *s);

/** Reset all counters. */
void rimlink_stats_reset(void);

/** Copy the last armed TX / last received RX frames (shell mosi/miso). */
void rimlink_frames_snapshot(uint8_t tx[RIMLINK_FRAME_LEN],
			     uint8_t rx[RIMLINK_FRAME_LEN]);

#ifdef __cplusplus
}
#endif

#endif /* LIB_RIMLINK_RIMLINK_H_ */
