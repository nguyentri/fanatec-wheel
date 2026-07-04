/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 *
 * Output service (spec section 7), porting the reference
 * refreshAlphanumericDisplays(): only CRC-valid frames reach this
 * code (adapter gate), and only frames whose disp[3] differs from the
 * previous valid value cause a decode/refresh. LED and rumble fields
 * are accumulated into counters only - Phase 4 evidence collection.
 */

#include <stdio.h>
#include <string.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "output_svc.h"
#include "led_svc.h"
#include "lra_svc.h"
#include "power_mgr.h"
#include "seg7.h"
#ifdef CONFIG_RIM_TM1637
#include "tm1637.h"
#endif

LOG_MODULE_REGISTER(output_svc, CONFIG_APP_LOG_LEVEL);

/* Minimum TM1637 refresh interval (spec section 3.1). */
#define OUTPUT_MIN_REFRESH_MS 10

static struct {
	uint8_t prev_disp[3];
	char text[16];
	struct output_stats stats;
	int64_t last_refresh;
	struct k_spinlock lock;
} osvc;

static void decode_text(const uint8_t disp[3], char *text, size_t size)
{
	size_t pos = 0;

	for (int i = 0; i < 3; i++) {
		char c = seg7_to_ascii(disp[i]);

		if (c != '\0') {
			text[pos++] = c;
		} else {
			/* Unknown glyph rendered as hex (reference). */
			pos += snprintf(&text[pos], size - pos, "0x%02X",
					disp[i] & 0x7FU);
		}
		if (disp[i] & 0x80U) { /* dot bit */
			text[pos++] = '.';
		}
	}
	text[pos] = '\0';
}

void output_svc_rx(const struct base_outputs *out)
{
	k_spinlock_key_t key;
	bool disp_changed =
		memcmp(out->disp, osvc.prev_disp, sizeof(out->disp)) != 0;

	key = k_spin_lock(&osvc.lock);

	if (out->leds != osvc.stats.leds_last) {
		osvc.stats.leds_changes++;
	}
	if (memcmp(out->rumble, osvc.stats.rumble_last, 2) != 0) {
		osvc.stats.rumble_changes++;
	}
	osvc.stats.leds_last = out->leds;
	memcpy(osvc.stats.rumble_last, out->rumble, 2);

	if (disp_changed) {
		memcpy(osvc.prev_disp, out->disp, sizeof(osvc.prev_disp));
		osvc.stats.disp_changes++;
		decode_text(out->disp, osvc.text, sizeof(osvc.text));
	}
	k_spin_unlock(&osvc.lock, key);

	/* Phase 4 consumers (workqueue context, spec 4-S1/4-S2/4-S3):
	 * every frame reaching here already passed CRC validation. */
	power_mgr_note_valid_rx();
	led_svc_submit(out->leds, out->disp);
	lra_svc_rumble(out->rumble);

	if (!disp_changed) {
		return;
	}

	LOG_INF("disp: %s", osvc.text);

#ifdef CONFIG_RIM_TM1637
	int64_t now = k_uptime_get();

	if ((now - osvc.last_refresh) >= OUTPUT_MIN_REFRESH_MS) {
		/* Reference: mosiBuf[2..4] -> digits 0..2, 4th blank. */
		uint8_t seg[4] = { out->disp[0], out->disp[1],
				   out->disp[2], 0x00 };

		(void)tm1637_set_segments(seg);
		osvc.last_refresh = now;
	}
#endif
}

void output_svc_disp_text(char text[16])
{
	k_spinlock_key_t key = k_spin_lock(&osvc.lock);

	memcpy(text, osvc.text, 16);
	k_spin_unlock(&osvc.lock, key);
}

void output_svc_stats_get(struct output_stats *s)
{
	k_spinlock_key_t key = k_spin_lock(&osvc.lock);

	*s = osvc.stats;
	k_spin_unlock(&osvc.lock, key);
}

int output_svc_init(void)
{
	strcpy(osvc.text, "   ");

#ifdef CONFIG_RIM_TM1637
	int ret = tm1637_init();

	if (ret < 0) {
		LOG_ERR("TM1637 init failed (%d)", ret);
		return ret;
	}
	/* Reference boot banner: 'rAcE' for one second, then clear. */
	static const uint8_t tm_race[4] = { 0x50, 0x77, 0x58, 0x79 };

	(void)tm1637_set_segments(tm_race);
#endif

	LOG_INF("output service ready (TM1637: %s)",
		IS_ENABLED(CONFIG_RIM_TM1637) ? "enabled" : "disabled");
	return 0;
}
