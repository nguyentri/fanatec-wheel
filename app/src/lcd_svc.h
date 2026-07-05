/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_SRC_LCD_SVC_H_
#define APP_SRC_LCD_SVC_H_

#include <stdbool.h>
#include <stdint.h>

/**
 * Race-display LCD service (spec 4-S1): consumes the validated 16-bit
 * `leds` field + disp bytes from output_svc and renders a race screen
 * on a digital LCD - a 15-segment rev bar with a 0..10 scale, the
 * current gear (from the decoded display bytes) inside an arc, and
 * 8 flag indicators. Change-driven at <= 60 Hz; link stale > 200 ms
 * drops to the quiet state (spec 4.1 isolation posture).
 *
 * The protocol field keeps its historical name (`leds`); the physical
 * output is a digital LCD rendered with LVGL when the devicetree
 * provides a `chosen zephyr,display` node and CONFIG_LVGL is enabled.
 * On boards without a panel the renderer runs to counters only.
 *
 * Lap/telemetry panels on the screen are placeholders: the legacy
 * 33-byte frame carries no lap or tire data (telemetry source
 * pending; see the software spec question register).
 */

#define LCD_REV_SEGMENTS 15 /* rev-bar segments */
#define LCD_FLAGS 8         /* flag indicators */

struct lcd_frame {
	uint8_t rgb[LCD_REV_SEGMENTS][3];
	uint8_t flags; /* bit k = flag indicator k */
};

/**
 * Pure renderer (unit-tested): leds bits 0-8 are 9 logical rev
 * positions interpolated across the 15-segment bar with a green->red
 * ramp; bits 9-15 map to flag indicators 0-6. Flag 7 is reserved for
 * a disp-derived rule - real-base capture data pending (question
 * register #1), held at 0 until then.
 */
void lcd_render(uint16_t leds, const uint8_t disp[3], struct lcd_frame *f);

struct lcd_stats {
	uint32_t frames_rendered;
	uint32_t frames_pushed;   /* reached the LCD panel */
	uint32_t rate_limited;
	uint32_t quiet_entries;
	bool quiet;
};

int lcd_svc_init(void);
/** Submit from output_svc rx context (workqueue; never the ISR). */
void lcd_svc_submit(uint16_t leds, const uint8_t disp[3]);
void lcd_svc_stats_get(struct lcd_stats *s);

#endif /* APP_SRC_LCD_SVC_H_ */
