/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 *
 * LRA haptics service (spec 4-S2). Driver backend: DRV2605-class
 * I2C haptic driver behind the `lra0` devicetree node - absent on
 * the bench board (PCB rev A pending), so cues run the full gate
 * logic into counters.
 */

#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/logging/log.h>

#if DT_NODE_EXISTS(DT_NODELABEL(lra0))
#define HAVE_LRA 1
#include <zephyr/drivers/i2c.h>
static const struct i2c_dt_spec lra_bus =
	I2C_DT_SPEC_GET(DT_NODELABEL(lra0));
#else
#define HAVE_LRA 0
#endif

#include "lra_svc.h"

LOG_MODULE_REGISTER(lra_svc, CONFIG_APP_LOG_LEVEL);

static struct {
	struct lra_gate gate[LRA_CHANNELS];
	struct lra_stats stats;
	atomic_t source_en;
	struct k_work_delayable stop_work[LRA_CHANNELS];
} hsvc;

uint32_t lra_gate_request(struct lra_gate *g, int64_t now_ms,
			  uint32_t duration_ms)
{
	if (now_ms < g->ready_at_ms) {
		return 0; /* cooldown (spec 4-S2) */
	}
	if (duration_ms > LRA_MAX_DURATION_MS) {
		duration_ms = LRA_MAX_DURATION_MS;
	}
	if (duration_ms == 0U) {
		return 0;
	}
	g->busy_until_ms = now_ms + duration_ms;
	g->ready_at_ms = g->busy_until_ms + LRA_COOLDOWN_MS;
	return duration_ms;
}

static void lra_hw_set(unsigned int chan, uint8_t level)
{
#if HAVE_LRA
	/* DRV2605L RTP mode: 0x02 = RTP input register. */
	uint8_t cmd[2] = { 0x02, level };

	(void)i2c_write_dt(&lra_bus, cmd, sizeof(cmd));
	ARG_UNUSED(chan);
#else
	ARG_UNUSED(chan);
	ARG_UNUSED(level);
#endif
}

static void stop0(struct k_work *w) { ARG_UNUSED(w); lra_hw_set(0, 0); }
static void stop1(struct k_work *w) { ARG_UNUSED(w); lra_hw_set(1, 0); }

int lra_svc_cue(unsigned int chan, uint8_t level, uint32_t duration_ms)
{
	if (chan >= LRA_CHANNELS) {
		return -EINVAL;
	}

	hsvc.stats.requested++;
	if (duration_ms > LRA_MAX_DURATION_MS) {
		hsvc.stats.capped++;
	}

	uint32_t granted = lra_gate_request(&hsvc.gate[chan],
					    k_uptime_get(), duration_ms);

	if (granted == 0U) {
		hsvc.stats.blocked++;
		return -EBUSY;
	}

	hsvc.stats.played++;
	lra_hw_set(chan, level);
	k_work_reschedule(&hsvc.stop_work[chan], K_MSEC(granted));
	return 0;
}

void lra_svc_source_enable(bool en)
{
	atomic_set(&hsvc.source_en, en ? 1 : 0);
	hsvc.stats.source_enabled = en;
	LOG_WRN("rumble frame source %s", en ? "ENABLED" : "disabled");
}

void lra_svc_rumble(const uint8_t rumble[2])
{
	if (atomic_get(&hsvc.source_en) != 1) {
		return; /* default posture (spec 4-S2): captures pending */
	}
	for (unsigned int ch = 0; ch < LRA_CHANNELS; ch++) {
		if (rumble[ch] != 0U) {
			(void)lra_svc_cue(ch, rumble[ch],
					  LRA_MAX_DURATION_MS);
		}
	}
}

void lra_svc_stats_get(struct lra_stats *s)
{
	*s = hsvc.stats;
}

int lra_svc_init(void)
{
	k_work_init_delayable(&hsvc.stop_work[0], stop0);
	k_work_init_delayable(&hsvc.stop_work[1], stop1);
	LOG_INF("lra_svc: hw %s, rumble source disabled (spec 4-S2)",
		HAVE_LRA ? "present" : "absent (PCB rev A pending)");
	return 0;
}
