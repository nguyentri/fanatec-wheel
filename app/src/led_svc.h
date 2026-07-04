/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_SRC_LED_SVC_H_
#define APP_SRC_LED_SVC_H_

#include <stdbool.h>
#include <stdint.h>

/**
 * LED service (spec 4-S1): consumes the validated 16-bit
 * leds field + disp bytes from output_svc, renders 15 RGB rev LEDs
 * and 8 flag LEDs. Change-driven at <= 60 Hz; link stale > 200 ms
 * drops to the quiet state (spec 4.1 isolation posture).
 */

#define LED_STRIP_LEN 15
#define LED_FLAGS 8

struct led_frame {
	uint8_t rgb[LED_STRIP_LEN][3];
	uint8_t flags; /* bit k = flag LED k */
};

/**
 * Pure renderer (unit-tested): leds bits 0-8 are 9 logical rev
 * positions interpolated across the 15-LED strip with a green->red
 * ramp; bits 9-15 map to flag LEDs 0-6. Flag LED 7 is reserved for a
 * disp-derived rule - real-base capture data pending (question
 * register #1), held at 0 until then.
 */
void led_render(uint16_t leds, const uint8_t disp[3], struct led_frame *f);

struct led_stats {
	uint32_t frames_rendered;
	uint32_t frames_pushed;   /* reached a physical backend */
	uint32_t rate_limited;
	uint32_t quiet_entries;
	bool quiet;
};

int led_svc_init(void);
/** Submit from output_svc rx context (workqueue; never the ISR). */
void led_svc_submit(uint16_t leds, const uint8_t disp[3]);
void led_svc_stats_get(struct led_stats *s);

#endif /* APP_SRC_LED_SVC_H_ */
