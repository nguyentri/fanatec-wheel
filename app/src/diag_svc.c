/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/shell/shell.h>

#include <lib/rimlink/rimlink.h>

#include "diag_svc.h"

LOG_MODULE_REGISTER(diag_svc, CONFIG_APP_LOG_LEVEL);

#ifdef CONFIG_RIM_SHELL

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

static int cmd_rim_stats(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	/* TODO(phase1): print rimlink_stats_get() - txn_count, crc_err_rx,
	 * rearm_miss, short_frame, overrun, cs_gap min/max, snapshot P99.
	 */
	shell_print(sh, "stats: TODO (adapter state machine pending)");
	return 0;
}

/* TODO(phase1): rim mosi / rim miso / rim disp / rim btn set|clr <bit> */

SHELL_STATIC_SUBCMD_SET_CREATE(sub_rim,
	SHELL_CMD_ARG(id, NULL, "get/set rim identity (1..4)", cmd_rim_id, 1, 1),
	SHELL_CMD(stats, NULL, "link statistics", cmd_rim_stats),
	SHELL_SUBCMD_SET_END
);
SHELL_CMD_REGISTER(rim, &sub_rim, "Rim-link diagnostics", NULL);

#endif /* CONFIG_RIM_SHELL */

int diag_svc_init(void)
{
	LOG_INF("diagnostics ready");
	return 0;
}
