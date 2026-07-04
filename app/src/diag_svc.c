/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 *
 * Diagnostics (spec sections 3/4): counters, shell, heartbeat.
 *
 * The shell reproduces the reference sketch's serial-command feature
 * ('i', 'o', 'd', A/B/C + 1..7) as structured commands, sufficient to
 * drive the documented base menu sequences (e.g. toggling bit 22
 * enters the base setup menu; center calibration via 22,6,18/19,...)
 * without physical buttons.
 */

#include <stdlib.h>

#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <zephyr/shell/shell.h>

#include <lib/rimlink/rimlink.h>

#include "diag_svc.h"
#include "input_svc.h"
#include "output_svc.h"

LOG_MODULE_REGISTER(diag_svc, CONFIG_APP_LOG_LEVEL);

/* Heartbeat on the board's user LED (LD1-equivalent). */
#define HB_NODE DT_ALIAS(led0)
#define HAVE_HB DT_NODE_EXISTS(HB_NODE)
#if HAVE_HB
static const struct gpio_dt_spec hb_led = GPIO_DT_SPEC_GET(HB_NODE, gpios);
static void hb_work_handler(struct k_work *work);
static K_WORK_DELAYABLE_DEFINE(hb_work, hb_work_handler);

static void hb_work_handler(struct k_work *work)
{
	ARG_UNUSED(work);
	(void)gpio_pin_toggle_dt(&hb_led);
	k_work_schedule(&hb_work, K_MSEC(500));
}
#endif

#ifdef CONFIG_RIM_SHELL

static void print_frame(const struct shell *sh, const char *tag,
			const uint8_t f[RIMLINK_FRAME_LEN])
{
	shell_fprintf(sh, SHELL_NORMAL, "%s:", tag);
	for (unsigned int i = 0; i < RIMLINK_FRAME_LEN; i++) {
		shell_fprintf(sh, SHELL_NORMAL, "0x%02X ", f[i]);
	}
	shell_fprintf(sh, SHELL_NORMAL, "\n");
}

/* rim mosi - reference 'i': print the last base -> rim frame. */
static int cmd_rim_mosi(const struct shell *sh, size_t argc, char **argv)
{
	uint8_t rx[RIMLINK_FRAME_LEN];

	ARG_UNUSED(argc);
	ARG_UNUSED(argv);
	rimlink_frames_snapshot(NULL, rx);
	print_frame(sh, "MOSI", rx);
	return 0;
}

/* rim miso - reference 'o': print the armed rim -> base frame. */
static int cmd_rim_miso(const struct shell *sh, size_t argc, char **argv)
{
	uint8_t tx[RIMLINK_FRAME_LEN];

	ARG_UNUSED(argc);
	ARG_UNUSED(argv);
	rimlink_frames_snapshot(tx, NULL);
	print_frame(sh, "MISO", tx);
	return 0;
}

/* rim disp - reference 'd': current alphanumeric text. */
static int cmd_rim_disp(const struct shell *sh, size_t argc, char **argv)
{
	char text[16];

	ARG_UNUSED(argc);
	ARG_UNUSED(argv);
	output_svc_disp_text(text);
	shell_print(sh, "disp: \"%s\"", text);
	return 0;
}

/* rim btn set|clr|toggle <bit 1..22> - reference A/B/C + 1..8. */
static int cmd_rim_btn(const struct shell *sh, size_t argc, char **argv)
{
	int bit = atoi(argv[2]);

	if (bit < 1 || bit > RIMLINK_BUTTON_BITS) {
		shell_error(sh, "bit must be 1..%u", RIMLINK_BUTTON_BITS);
		return -EINVAL;
	}
	if (strcmp(argv[1], "set") == 0) {
		input_svc_manual_bit(bit, true);
	} else if (strcmp(argv[1], "clr") == 0) {
		input_svc_manual_bit(bit, false);
	} else if (strcmp(argv[1], "toggle") == 0) {
		input_svc_manual_toggle(bit);
	} else {
		shell_error(sh, "use: rim btn set|clr|toggle <bit>");
		return -EINVAL;
	}
	shell_print(sh, "manual bits: 0x%06X", input_svc_manual_get());
	return 0;
}

/* rim map [idx bit] - runtime button-map reconfiguration. */
static int cmd_rim_map(const struct shell *sh, size_t argc, char **argv)
{
	if (argc == 3) {
		int idx = atoi(argv[1]);
		int bit = atoi(argv[2]);

		if (input_svc_map_set(idx, bit) < 0) {
			shell_error(sh, "idx 0..%d, bit 1..%u",
				    INPUT_NUM_BUTTONS - 1,
				    RIMLINK_BUTTON_BITS);
			return -EINVAL;
		}
	}
	shell_fprintf(sh, SHELL_NORMAL, "button map:");
	for (unsigned int i = 0; i < INPUT_NUM_BUTTONS; i++) {
		shell_fprintf(sh, SHELL_NORMAL, " %u", input_svc_map_get(i));
	}
	shell_fprintf(sh, SHELL_NORMAL, "\n");
	return 0;
}

/* rim id [1..4] */
static int cmd_rim_id(const struct shell *sh, size_t argc, char **argv)
{
	if (argc == 2) {
		int id = atoi(argv[1]);

		if (id < RIM_ID_BMW_GT2 || id > RIM_ID_UNIHUB) {
			shell_error(sh, "id must be 1..4");
			return -EINVAL;
		}
		rimlink_identity_set((enum rim_id)id);
	}
	shell_print(sh, "rim identity: 0x%02x", rimlink_identity_get());
	return 0;
}

/* rim dpad [t1..t7 hyst] */
static int cmd_rim_dpad(const struct shell *sh, size_t argc, char **argv)
{
	if (argc == 9) {
		uint16_t thr[7];

		for (int i = 0; i < 7; i++) {
			thr[i] = (uint16_t)atoi(argv[1 + i]);
		}
		if (input_svc_dpad_cal(thr, (uint16_t)atoi(argv[8])) < 0) {
			shell_error(sh, "thresholds must descend");
			return -EINVAL;
		}
	}
	shell_print(sh, "dpad position: %u", input_svc_dpad_pos());
	return 0;
}

static int cmd_rim_stats(const struct shell *sh, size_t argc, char **argv)
{
	struct rimlink_stats s;
	struct output_stats o;
	uint32_t p99, max;

	ARG_UNUSED(argc);
	ARG_UNUSED(argv);
	rimlink_stats_get(&s);
	output_svc_stats_get(&o);
	input_svc_latency_get(&p99, &max);

	shell_print(sh, "txn:          %u", s.txn_count);
	shell_print(sh, "crc_err_rx:   %u", s.crc_err_rx);
	shell_print(sh, "short_frame:  %u", s.short_frame);
	shell_print(sh, "rearm_miss:   %u", s.rearm_miss);
	shell_print(sh, "overrun:      %u", s.overrun);
	shell_print(sh, "stale_clear:  %u", s.stale_clear);
	shell_print(sh, "cs_gap us:    min %u max %u",
		    (s.cs_gap_min_us == UINT32_MAX) ? 0 : s.cs_gap_min_us,
		    s.cs_gap_max_us);
	shell_print(sh, "snap lat us:  p99 %u max %u", p99, max);
	shell_print(sh, "leds_last:    0x%04X (%u changes)",
		    o.leds_last, o.leds_changes);
	shell_print(sh, "rumble_last:  %02X %02X (%u changes)",
		    o.rumble_last[0], o.rumble_last[1], o.rumble_changes);
	shell_print(sh, "disp_changes: %u", o.disp_changes);
	return 0;
}

static int cmd_rim_stats_reset(const struct shell *sh, size_t argc,
			       char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);
	rimlink_stats_reset();
	shell_print(sh, "counters reset");
	return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_rim,
	SHELL_CMD(mosi, NULL, "print last base->rim frame ('i')", cmd_rim_mosi),
	SHELL_CMD(miso, NULL, "print armed rim->base frame ('o')", cmd_rim_miso),
	SHELL_CMD(disp, NULL, "current display text ('d')", cmd_rim_disp),
	SHELL_CMD_ARG(btn, NULL, "set|clr|toggle <bit 1..22>", cmd_rim_btn, 3, 0),
	SHELL_CMD_ARG(map, NULL, "[idx bit] show/set button map", cmd_rim_map, 1, 2),
	SHELL_CMD_ARG(id, NULL, "[1..4] get/set rim identity", cmd_rim_id, 1, 1),
	SHELL_CMD_ARG(dpad, NULL, "[t1..t7 hyst] dpad cal/status", cmd_rim_dpad, 1, 8),
	SHELL_CMD(stats, NULL, "link statistics", cmd_rim_stats),
	SHELL_CMD(reset, NULL, "reset counters", cmd_rim_stats_reset),
	SHELL_SUBCMD_SET_END
);
SHELL_CMD_REGISTER(rim, &sub_rim, "Rim-link diagnostics", NULL);

#endif /* CONFIG_RIM_SHELL */

int diag_svc_init(void)
{
#if HAVE_HB
	if (gpio_is_ready_dt(&hb_led)) {
		(void)gpio_pin_configure_dt(&hb_led, GPIO_OUTPUT_INACTIVE);
		k_work_schedule(&hb_work, K_MSEC(500));
	}
#endif
	LOG_INF("diagnostics ready");
	return 0;
}
