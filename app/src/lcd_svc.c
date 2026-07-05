/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 *
 * Race-display LCD service (spec 4-S1).
 *
 * Model layer: the pure lcd_render() turns the validated `leds` field
 * into 15 rev-bar segment colors + 8 flag bits (unit-tested on the
 * host, no kernel dependencies).
 *
 * View layer: an LVGL race screen - title bar, 0..10 rev scale over a
 * 15-segment bar, the current gear (decoded display text) inside an
 * arc with an "RPM" caption, lap/tire placeholder panels (the legacy
 * frame carries no telemetry), a Menu/Down bar, and 8 flag squares.
 * Built only when CONFIG_LVGL is enabled and the devicetree provides
 * a `chosen zephyr,display` node; otherwise counters-only.
 *
 * All rendering happens in the system workqueue at <= 60 Hz; the
 * LVGL timer pump is a self-rescheduling work item. The link fast
 * path is never touched.
 */

#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/logging/log.h>
#include <string.h>

#include "lcd_svc.h"
#include "seg7.h"

#if defined(CONFIG_LVGL) && DT_HAS_CHOSEN(zephyr_display)
#define HAVE_LCD 1
#include <lvgl.h>
#include <zephyr/drivers/display.h>
static const struct device *const lcd_dev =
	DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
#else
#define HAVE_LCD 0
#endif

LOG_MODULE_REGISTER(lcd_svc, CONFIG_APP_LOG_LEVEL);

#define LCD_MIN_PERIOD_MS 17   /* <= 60 Hz (spec 4-S1) */
#define LCD_QUIET_MS 200       /* stale link -> quiet (spec 4.1) */
#define LVGL_PUMP_MS 20        /* lv_timer_handler cadence */

static struct {
	struct k_spinlock lock;
	uint16_t leds;
	uint8_t disp[3];
	bool dirty;
	int64_t last_push_ms;
	struct lcd_stats stats;
	struct k_work_delayable render_work;
	struct k_work_delayable quiet_work;
} lsvc;

void lcd_render(uint16_t leds, const uint8_t disp[3], struct lcd_frame *f)
{
	ARG_UNUSED(disp); /* flag 7 rule: capture data pending */

	memset(f, 0, sizeof(*f));

	/* Rev bar: 9 logical positions -> 15 segments. Logical position
	 * i centers on segment i*14/8 (fixed point x8); neighbours
	 * within one segment pitch get the linear remainder. */
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
		if (next_lvl > 0 && lo + 1 < LCD_REV_SEGMENTS) {
			f->rgb[lo + 1][0] = MAX(f->rgb[lo + 1][0],
						(uint8_t)((r * next_lvl) / 255));
			f->rgb[lo + 1][1] = MAX(f->rgb[lo + 1][1],
						(uint8_t)((g * next_lvl) / 255));
		}
	}

	/* Flag indicators 0-6 <- leds bits 9-15. Flag 7 reserved. */
	f->flags = (uint8_t)((leds >> 9) & 0x7FU);
}

#if HAVE_LCD

/* ---- LVGL race screen (view layer) ---- */

static struct {
	bool built;
	lv_obj_t *seg[LCD_REV_SEGMENTS];   /* rev-bar segments */
	lv_obj_t *flag[LCD_FLAGS];         /* flag squares */
	lv_obj_t *gear;                    /* big center text */
	lv_obj_t *arc;                     /* gear ring */
	lv_obj_t *stale;                   /* quiet-state banner */
	struct k_work_delayable pump_work; /* lv_timer_handler */
} ui;

static void lvgl_pump(struct k_work *work)
{
	ARG_UNUSED(work);
	(void)lv_timer_handler();
	k_work_reschedule(&ui.pump_work, K_MSEC(LVGL_PUMP_MS));
}

static lv_obj_t *panel_label(lv_obj_t *parent, const char *txt,
			     lv_color_t color, lv_align_t align,
			     int32_t x, int32_t y)
{
	lv_obj_t *l = lv_label_create(parent);

	lv_label_set_text(l, txt);
	lv_obj_set_style_text_color(l, color, 0);
	lv_obj_align(l, align, x, y);
	return l;
}

/* Build the static screen once (workqueue context, post-kernel). */
static void ui_build(void)
{
	lv_obj_t *scr = lv_screen_active();
	const int32_t w = lv_obj_get_width(scr) ?
		lv_obj_get_width(scr) : 320;
	const lv_color_t dim = lv_color_hex(0x202020);
	const lv_color_t grn = lv_color_hex(0x30d030);

	lv_obj_set_style_bg_color(scr, lv_color_black(), 0);
	lv_obj_set_style_text_color(scr, lv_color_white(), 0);

	/* Title bar. */
	panel_label(scr, "Race Display", lv_color_white(),
		    LV_ALIGN_TOP_MID, 0, 2);

	/* Rev scale 0..10 over the segment bar. */
	const int32_t bar_y = 22, bar_h = 16;
	const int32_t seg_w = (w - 10) / LCD_REV_SEGMENTS;

	for (int t = 0; t <= 10; t++) {
		char num[3];

		snprintf(num, sizeof(num), "%d", t);
		panel_label(scr, num, lv_color_white(), LV_ALIGN_TOP_LEFT,
			    5 + (t * (w - 10 - 10)) / 10, bar_y + bar_h + 2);
	}

	for (int i = 0; i < LCD_REV_SEGMENTS; i++) {
		ui.seg[i] = lv_obj_create(scr);
		lv_obj_remove_style_all(ui.seg[i]);
		lv_obj_set_size(ui.seg[i], seg_w - 2, bar_h);
		lv_obj_set_pos(ui.seg[i], 5 + i * seg_w, bar_y);
		lv_obj_set_style_bg_opa(ui.seg[i], LV_OPA_COVER, 0);
		lv_obj_set_style_bg_color(ui.seg[i], dim, 0);
	}

	/* Center gear ring + big gear text + RPM caption. */
	ui.arc = lv_arc_create(scr);
	lv_obj_set_size(ui.arc, 96, 96);
	lv_obj_align(ui.arc, LV_ALIGN_CENTER, 0, 14);
	lv_arc_set_bg_angles(ui.arc, 120, 60); /* open at the bottom */
	lv_arc_set_value(ui.arc, 0);
	lv_obj_remove_style(ui.arc, NULL, LV_PART_KNOB);
	lv_obj_clear_flag(ui.arc, LV_OBJ_FLAG_CLICKABLE);

	ui.gear = lv_label_create(scr);
	lv_label_set_text(ui.gear, "-");
#ifdef CONFIG_LV_FONT_MONTSERRAT_48
	lv_obj_set_style_text_font(ui.gear, &lv_font_montserrat_48, 0);
#endif
	lv_obj_align(ui.gear, LV_ALIGN_CENTER, 0, 6);
	panel_label(scr, "RPM", lv_color_white(), LV_ALIGN_CENTER, 0, 42);

	/* Lap / Lat / tire panels: PLACEHOLDERS - the legacy frame
	 * carries no telemetry (spec question register). */
	panel_label(scr, "Lap  --.---", lv_color_white(),
		    LV_ALIGN_LEFT_MID, 6, -18);
	panel_label(scr, "Lap  --.---", lv_color_white(),
		    LV_ALIGN_LEFT_MID, 6, 0);
	panel_label(scr, "Lat  --.---", lv_color_white(),
		    LV_ALIGN_RIGHT_MID, -6, -18);
	panel_label(scr, "TIRE --\xC2\xB0  --\xC2\xB0", grn,
		    LV_ALIGN_BOTTOM_LEFT, 6, -22);
	panel_label(scr, "TIRE --\xC2\xB0  --\xC2\xB0", grn,
		    LV_ALIGN_BOTTOM_RIGHT, -6, -22);

	/* Bottom bar: Menu/Down caption + flag squares. */
	panel_label(scr, "Menu /Down", lv_color_white(),
		    LV_ALIGN_BOTTOM_MID, 0, -4);
	for (int k = 0; k < LCD_FLAGS; k++) {
		ui.flag[k] = lv_obj_create(scr);
		lv_obj_remove_style_all(ui.flag[k]);
		lv_obj_set_size(ui.flag[k], 12, 8);
		lv_obj_align(ui.flag[k], LV_ALIGN_BOTTOM_LEFT,
			     6 + k * 15, -4);
		lv_obj_set_style_bg_opa(ui.flag[k], LV_OPA_COVER, 0);
		lv_obj_set_style_bg_color(ui.flag[k], dim, 0);
	}

	/* Quiet-state banner, hidden while the link is alive. */
	ui.stale = panel_label(scr, "LINK STALE", lv_color_hex(0xd0d000),
			       LV_ALIGN_TOP_MID, 0, 40);
	lv_obj_add_flag(ui.stale, LV_OBJ_FLAG_HIDDEN);

	(void)display_blanking_off(lcd_dev);
	k_work_init_delayable(&ui.pump_work, lvgl_pump);
	k_work_reschedule(&ui.pump_work, K_MSEC(LVGL_PUMP_MS));
	ui.built = true;
}

static void frame_push(const struct lcd_frame *f, const uint8_t disp[3],
		       bool quiet)
{
	if (!ui.built) {
		if (!device_is_ready(lcd_dev)) {
			return;
		}
		ui_build();
	}

	for (int i = 0; i < LCD_REV_SEGMENTS; i++) {
		lv_color_t c = lv_color_make(f->rgb[i][0], f->rgb[i][1],
					     f->rgb[i][2]);

		if ((f->rgb[i][0] | f->rgb[i][1] | f->rgb[i][2]) == 0U) {
			c = lv_color_hex(0x202020);
		}
		lv_obj_set_style_bg_color(ui.seg[i], c, 0);
	}

	for (int k = 0; k < LCD_FLAGS; k++) {
		lv_obj_set_style_bg_color(ui.flag[k],
					  (f->flags & BIT(k)) ?
					  lv_color_hex(0xd0d000) :
					  lv_color_hex(0x202020), 0);
	}

	/* Gear: first decoded display character; '-' when blank. */
	char c = seg7_to_ascii(disp[0]);
	char gear[2] = { (c != '\0' && c != ' ') ? c : '-', '\0' };

	lv_label_set_text(ui.gear, gear);

	if (quiet) {
		lv_obj_clear_flag(ui.stale, LV_OBJ_FLAG_HIDDEN);
	} else {
		lv_obj_add_flag(ui.stale, LV_OBJ_FLAG_HIDDEN);
	}

	lsvc.stats.frames_pushed++;
}

#else /* !HAVE_LCD */

static void frame_push(const struct lcd_frame *f, const uint8_t disp[3],
		       bool quiet)
{
	ARG_UNUSED(f);
	ARG_UNUSED(disp);
	ARG_UNUSED(quiet);
}

#endif /* HAVE_LCD */

static void render_handler(struct k_work *work)
{
	ARG_UNUSED(work);

	struct lcd_frame f;
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

	lcd_render(leds, disp, &f);
	lsvc.stats.frames_rendered++;
	lsvc.stats.quiet = false;
	frame_push(&f, disp, false);
	lsvc.last_push_ms = k_uptime_get();
}

static void quiet_handler(struct k_work *work)
{
	ARG_UNUSED(work);

	/* Link stale (spec 4.1): everything dark, no residual state. */
	struct lcd_frame f = {0};
	const uint8_t disp_blank[3] = {0};

	frame_push(&f, disp_blank, true);
	lsvc.stats.quiet_entries++;
	lsvc.stats.quiet = true;
}

void lcd_svc_submit(uint16_t leds, const uint8_t disp[3])
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
	k_work_reschedule(&lsvc.quiet_work, K_MSEC(LCD_QUIET_MS));

	if (!changed) {
		return; /* change-driven (spec 4-S1) */
	}

	int64_t since = k_uptime_get() - lsvc.last_push_ms;

	if (since >= LCD_MIN_PERIOD_MS) {
		k_work_reschedule(&lsvc.render_work, K_NO_WAIT);
	} else {
		lsvc.stats.rate_limited++;
		k_work_reschedule(&lsvc.render_work,
				  K_MSEC(LCD_MIN_PERIOD_MS - since));
	}
}

void lcd_svc_stats_get(struct lcd_stats *s)
{
	*s = lsvc.stats;
}

int lcd_svc_init(void)
{
	k_work_init_delayable(&lsvc.render_work, render_handler);
	k_work_init_delayable(&lsvc.quiet_work, quiet_handler);
	LOG_INF("lcd_svc: %s", HAVE_LCD ?
		"LVGL race display" :
		"panel absent (counters only, panel pending)");
	return 0;
}
