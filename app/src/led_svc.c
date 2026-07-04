/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 *
 * LED service (spec 4-S1).
 *
 * Backend: an addressable strip driven by the led_strip API when the
 * devicetree provides an `led-strip` alias (DMA-fed SPI/RMT chain on
 * PCB rev A - hardware pending); otherwise render-to-counters only,
 * keeping the logic testable on the bench board.
 */

#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/logging/log.h>
#include <string.h>

#if DT_NODE_EXISTS(DT_ALIAS(led_strip))
#define HAVE_LED_STRIP 1
#include <zephyr/drivers/led_strip.h>
static const struct device *const strip_dev =
	DEVICE_DT_GET(DT_ALIAS(led_strip));
#else
#define HAVE_LED_STRIP 0
#endif

#include "led_svc.h"

LOG_MODULE_REGISTER(led_svc, CONFIG_APP_LOG_LEVEL);

#define LED_MIN_PERIOD_MS 17   /* <= 60 Hz (spec 4-S1) */
#define LED_QUIET_MS 200       /* stale link -> quiet (spec 4.1) */

static struct {
	struct k_spinlock lock;
	uint16_t leds;
	uint8_t disp[3];
	bool dirty;
	int64_t last_push_ms;
	struct led_stats stats;
	struct k_work_delayable render_work;
	struct k_work_delayable quiet_work;
} lsvc;

void led_render(uint16_t leds, const uint8_t disp[3], struct led_frame *f)
{
	ARG_UNUSED(disp); /* flag LED 7 rule: capture data pending */

	memset(f, 0, sizeof(*f));

	/* Rev bar: 9 logical positions -> 15 LEDs. Logical position i
	 * centers on LED i*14/8 (fixed point x8); neighbours within
	 * one LED pitch get the linear remainder. */
	for (int i = 0; i < 9; i++) {
		if ((leds & BIT(i)) == 0U) {
			continue;
		}

		int c_x8 = (i * 14 * 8) / 8;      /* centre * 8 */
		int lo = c_x8 / 8;
		int frac = c_x8 % 8;               /* 0..7 toward lo+1 */

		/* Green->red ramp across the bar. */
		uint8_t r = (uint8_t)((i * 255) / 8);
		uint8_t g = (uint8_t)(255U - r);

		uint8_t main_lvl = (uint8_t)(255 - (frac * 255) / 8);
		uint8_t next_lvl = (uint8_t)((frac * 255) / 8);

		f->rgb[lo][0] = MAX(f->rgb[lo][0], (uint8_t)((r * main_lvl) / 255));
		f->rgb[lo][1] = MAX(f->rgb[lo][1], (uint8_t)((g * main_lvl) / 255));
		if (next_lvl > 0 && lo + 1 < LED_STRIP_LEN) {
			f->rgb[lo + 1][0] = MAX(f->rgb[lo + 1][0],
						(uint8_t)((r * next_lvl) / 255));
			f->rgb[lo + 1][1] = MAX(f->rgb[lo + 1][1],
						(uint8_t)((g * next_lvl) / 255));
		}
	}

	/* Flag LEDs 0-6 <- leds bits 9-15. Flag 7 reserved (pending). */
	f->flags = (uint8_t)((leds >> 9) & 0x7FU);
}

static void frame_push(const struct led_frame *f)
{
#if HAVE_LED_STRIP
	struct led_rgb pixels[LED_STRIP_LEN];

	for (int i = 0; i < LED_STRIP_LEN; i++) {
		pixels[i].r = f->rgb[i][0];
		pixels[i].g = f->rgb[i][1];
		pixels[i].b = f->rgb[i][2];
	}
	if (device_is_ready(strip_dev) &&
	    led_strip_update_rgb(strip_dev, pixels, LED_STRIP_LEN) == 0) {
		lsvc.stats.frames_pushed++;
	}
#else
	ARG_UNUSED(f);
#endif
}

static void render_handler(struct k_work *work)
{
	ARG_UNUSED(work);

	struct led_frame f;
	uint16_t leds;
	uint8_t disp[3];
	k_spinlock_key_t key = k_spin_lock(&lsvc.lock);

	if (!lsvc.dirty) {
		k_spin_unlock(&lsvc.lock, key);
		return;
	}
	lsvc.dirty = false;
	leds = lsvc.leds;
	memcpy(disp, lsvc.disp, 3);
	k_spin_unlock(&lsvc.lock, key);

	led_render(leds, disp, &f);
	lsvc.stats.frames_rendered++;
	lsvc.stats.quiet = false;
	frame_push(&f);
	lsvc.last_push_ms = k_uptime_get();
}

static void quiet_handler(struct k_work *work)
{
	ARG_UNUSED(work);

	/* Link stale (spec 4.1): everything dark, no residual state. */
	struct led_frame f = {0};

	frame_push(&f);
	lsvc.stats.quiet_entries++;
	lsvc.stats.quiet = true;
}

void led_svc_submit(uint16_t leds, const uint8_t disp[3])
{
	k_spinlock_key_t key = k_spin_lock(&lsvc.lock);
	bool changed = (leds != lsvc.leds) ||
		       (memcmp(disp, lsvc.disp, 3) != 0) ||
		       lsvc.stats.quiet;

	lsvc.leds = leds;
	memcpy(lsvc.disp, disp, 3);
	if (changed) {
		lsvc.dirty = true;
	}
	k_spin_unlock(&lsvc.lock, key);

	/* Every valid frame re-arms the quiet timer. */
	k_work_reschedule(&lsvc.quiet_work, K_MSEC(LED_QUIET_MS));

	if (!changed) {
		return; /* change-driven (spec 4-S1) */
	}

	int64_t since = k_uptime_get() - lsvc.last_push_ms;

	if (since >= LED_MIN_PERIOD_MS) {
		k_work_reschedule(&lsvc.render_work, K_NO_WAIT);
	} else {
		lsvc.stats.rate_limited++;
		k_work_reschedule(&lsvc.render_work,
				  K_MSEC(LED_MIN_PERIOD_MS - since));
	}
}

void led_svc_stats_get(struct led_stats *s)
{
	*s = lsvc.stats;
}

int led_svc_init(void)
{
	k_work_init_delayable(&lsvc.render_work, render_handler);
	k_work_init_delayable(&lsvc.quiet_work, quiet_handler);
	LOG_INF("led_svc: strip %s", HAVE_LED_STRIP ?
		"present" : "absent (counters only, PCB rev A pending)");
	return 0;
}
