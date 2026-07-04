/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 *
 * Settings schema v2 (spec 3-S5) + config lock (spec 5-S5).
 */

#include <string.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/settings/settings.h>

#include "rim_settings.h"

LOG_MODULE_REGISTER(rim_settings, CONFIG_APP_LOG_LEVEL);

static struct rim_settings rs = {
	.schema = RIM_SETTINGS_SCHEMA_VERSION,
	.clutch_cal = {
		{ .min = 400, .max = 3700, .guard = 50, .deadzone = 8 },
		{ .min = 400, .max = 3700, .guard = 50, .deadzone = 8 },
	},
	.dpad_thr = { 3732, 3052, 2556, 2020, 1196, 656, 288 },
	.dpad_hyst = 40,
	.button_map = { 8, 5, 13, 11, 9, 12 },
	.encoder_sel = 0,
	.axis_map = 0,
	.clutch_mode = CLUTCH_MODE_BITE,
	.bite_point = 128,
	.locked = false,
};

struct rim_settings *rim_settings_get(void)
{
	return &rs;
}

static void migrate(uint8_t from)
{
	/* Migration stub (spec 3-S5): schema v2 is the first persisted
	 * version; future versions translate here field by field. */
	LOG_WRN("settings schema %u -> %u: defaults kept for new fields",
		from, RIM_SETTINGS_SCHEMA_VERSION);
	rs.schema = RIM_SETTINGS_SCHEMA_VERSION;
}

#define LOAD(dst) \
	if (len == sizeof(dst) && read_cb(cb_arg, &(dst), len) >= 0) { \
		return 0; \
	}

static int rim_set(const char *key, size_t len, settings_read_cb read_cb,
		   void *cb_arg)
{
	if (strcmp(key, "schema") == 0) {
		uint8_t v;

		if (len == sizeof(v) && read_cb(cb_arg, &v, len) >= 0) {
			if (v != RIM_SETTINGS_SCHEMA_VERSION) {
				migrate(v);
			}
			return 0;
		}
	} else if (strcmp(key, "cal/clutch") == 0) {
		LOAD(rs.clutch_cal);
	} else if (strcmp(key, "cal/dpad_thr") == 0) {
		LOAD(rs.dpad_thr);
	} else if (strcmp(key, "cal/dpad_hyst") == 0) {
		LOAD(rs.dpad_hyst);
	} else if (strcmp(key, "map/buttons") == 0) {
		LOAD(rs.button_map);
	} else if (strcmp(key, "map/encoder") == 0) {
		LOAD(rs.encoder_sel);
	} else if (strcmp(key, "map/axis") == 0) {
		LOAD(rs.axis_map);
	} else if (strcmp(key, "mode/clutch") == 0) {
		LOAD(rs.clutch_mode);
	} else if (strcmp(key, "mode/bite") == 0) {
		LOAD(rs.bite_point);
	} else if (strcmp(key, "lock") == 0) {
		LOAD(rs.locked);
	}
	return -ENOENT;
}

SETTINGS_STATIC_HANDLER_DEFINE(rim, "rim", NULL, rim_set, NULL, NULL);

int rim_settings_commit(const char *subtree)
{
	int ret = 0;

	if (rs.locked && strcmp(subtree, "lock") != 0) {
		/* Config lock (spec 5-S5): field state is deliberate. */
		return -EACCES;
	}

	if (strcmp(subtree, "cal") == 0) {
		ret |= settings_save_one("rim/cal/clutch", &rs.clutch_cal,
					 sizeof(rs.clutch_cal));
		ret |= settings_save_one("rim/cal/dpad_thr", &rs.dpad_thr,
					 sizeof(rs.dpad_thr));
		ret |= settings_save_one("rim/cal/dpad_hyst", &rs.dpad_hyst,
					 sizeof(rs.dpad_hyst));
	} else if (strcmp(subtree, "map") == 0) {
		ret |= settings_save_one("rim/map/buttons", &rs.button_map,
					 sizeof(rs.button_map));
		ret |= settings_save_one("rim/map/encoder", &rs.encoder_sel,
					 sizeof(rs.encoder_sel));
		ret |= settings_save_one("rim/map/axis", &rs.axis_map,
					 sizeof(rs.axis_map));
	} else if (strcmp(subtree, "mode") == 0) {
		ret |= settings_save_one("rim/mode/clutch", &rs.clutch_mode,
					 sizeof(rs.clutch_mode));
		ret |= settings_save_one("rim/mode/bite", &rs.bite_point,
					 sizeof(rs.bite_point));
	} else if (strcmp(subtree, "lock") == 0) {
		ret |= settings_save_one("rim/lock", &rs.locked,
					 sizeof(rs.locked));
	} else {
		return -EINVAL;
	}

	ret |= settings_save_one("rim/schema", &rs.schema,
				 sizeof(rs.schema));
	return ret;
}

int rim_settings_init(void)
{
	int ret = settings_subsys_init();

	if (ret == 0) {
		ret = settings_load_subtree("rim");
	}
	if (ret != 0) {
		LOG_WRN("settings unavailable (%d): RAM defaults in use",
			ret);
	}
	return ret;
}
