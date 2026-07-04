/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 *
 * Base-side simulator (spec section 8): clocks 33-byte Mode-1
 * transactions at a shell-configurable rate/cadence, seals MOSI
 * frames with CRC-8, verifies MISO CRC and header 0xA5, runs the
 * fault-injection set {truncate at byte N, corrupt CRC, extra clocks,
 * cadence jitter}, encodes shell-entered text into disp[3], and
 * streams per-second statistics over its console. Simulator and DUT
 * statistics must reconcile to zero unexplained discrepancy.
 */

#include <stdlib.h>
#include <string.h>

#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/logging/log.h>
#include <zephyr/random/random.h>
#include <zephyr/shell/shell.h>

#include <lib/rimlink/rimlink.h>

#include "../../src/seg7.h"
#include "sim_profile.h"

LOG_MODULE_REGISTER(sim, LOG_LEVEL_INF);

#define SPI_NODE DT_NODELABEL(spi1)
#define HAVE_SPI DT_NODE_HAS_STATUS(SPI_NODE, okay)

/* Base-twin profile (spec 2-S6): defaults generated from
 * profiles/base_twin.yaml; runtime-overridable via `sim profile`. */
#define SIM_CS_SETUP_US SIM_PROFILE_CS_SETUP_US
#define SIM_SPI_HZ SIM_PROFILE_CLOCK_HZ
#define SIM_DEFAULT_CADENCE_US SIM_PROFILE_CADENCE_US

enum sim_fault {
	FAULT_NONE = 0,
	FAULT_CRC,      /* corrupt MOSI CRC */
	FAULT_TRUNC,    /* truncate transaction at byte N */
	FAULT_EXTRA,    /* clock extra bytes after byte 33 */
	FAULT_JITTER,   /* random cadence jitter 0..N ms */
};

static struct {
	bool running;
	uint32_t cadence_us;
	uint32_t jitter_us;
	uint32_t clock_hz;
	uint32_t flush_bytes;
	enum sim_fault fault;
	uint32_t fault_n;     /* every Nth transaction / parameter */
	uint32_t txn;
	uint32_t miso_crc_err;
	uint32_t first_byte_err;
	uint32_t faults_injected;
	uint8_t mosi[RIMLINK_FRAME_LEN + 8]; /* + flush/extra-clock room */
	uint8_t miso[RIMLINK_FRAME_LEN + 8];
	struct k_spinlock lock;
} sim = {
	.running = true,
	.cadence_us = SIM_DEFAULT_CADENCE_US,
	.jitter_us = SIM_PROFILE_CADENCE_JITTER_US,
	.clock_hz = SIM_SPI_HZ,
	.flush_bytes = SIM_PROFILE_FLUSH_BYTES,
	.fault_n = 100,
};

#if HAVE_SPI
static const struct device *const spi_dev = DEVICE_DT_GET(SPI_NODE);

static struct spi_config spi_cfg = {
	.frequency = SIM_SPI_HZ,
	/* Controller, Mode 1 (CPOL=0/CPHA=1), MSB first (spec 2.1). */
	.operation = SPI_OP_MODE_MASTER | SPI_MODE_CPHA |
		     SPI_WORD_SET(8) | SPI_TRANSFER_MSB,
	.cs = {
		.gpio = GPIO_DT_SPEC_GET(SPI_NODE, cs_gpios),
		.delay = SIM_CS_SETUP_US,
	},
};
#endif

static void seal_mosi(void)
{
	sim.mosi[0] = RIMLINK_HEADER; /* recorded-not-interpreted on rim */
	sim.mosi[1] = RIM_ID_PORSCHE_918_RSR;
	sim.mosi[RIMLINK_OFF_CRC] =
		rimlink_crc8(sim.mosi, RIMLINK_OFF_CRC);
}

static void run_transaction(void)
{
#if HAVE_SPI
	uint8_t mosi_txn[RIMLINK_FRAME_LEN + 8];
	size_t len = RIMLINK_FRAME_LEN;
	bool inject = (sim.fault != FAULT_NONE) && (sim.fault_n != 0U) &&
		      ((sim.txn + 1U) % sim.fault_n == 0U);

	k_spinlock_key_t key = k_spin_lock(&sim.lock);

	memcpy(mosi_txn, sim.mosi, sizeof(sim.mosi));
	k_spin_unlock(&sim.lock, key);

	if (inject) {
		switch (sim.fault) {
		case FAULT_CRC:
			mosi_txn[RIMLINK_OFF_CRC] ^= 0xFFU;
			break;
		case FAULT_TRUNC:
			/* Deassert CS after N bytes (N < 33). */
			len = MAX(1U, MIN(sim.fault_n % RIMLINK_FRAME_LEN,
					  RIMLINK_FRAME_LEN - 1U));
			len = MIN(len, RIMLINK_FRAME_LEN - 1U);
			break;
		case FAULT_EXTRA:
			/* Extra clocks after byte 33 within CS. */
			len = RIMLINK_FRAME_LEN + 1U;
			break;
		default:
			break;
		}
		sim.faults_injected++;
	}

	/* Genuine-base flush emulation (2-S6 / roadmap 11.5): extra
	 * bytes clocked within the same CS after byte 33. */
	if (len == RIMLINK_FRAME_LEN && sim.flush_bytes > 0U) {
		len += MIN(sim.flush_bytes, 8U);
	}

	spi_cfg.frequency = sim.clock_hz;

	const struct spi_buf txb = { .buf = mosi_txn, .len = len };
	struct spi_buf rxb = { .buf = sim.miso, .len = len };
	const struct spi_buf_set tx = { .buffers = &txb, .count = 1 };
	const struct spi_buf_set rx = { .buffers = &rxb, .count = 1 };

	int ret = spi_transceive(spi_dev, &spi_cfg, &tx, &rx);

	if (ret < 0) {
		return;
	}

	sim.txn++;

	if (len < RIMLINK_FRAME_LEN) {
		return; /* truncated on purpose: no MISO verdict */
	}

	/* Verify MISO: header 0xA5 and CRC over bytes 0-31 (spec s.8). */
	if (sim.miso[0] != RIMLINK_HEADER) {
		sim.first_byte_err++;
	}
	if (rimlink_crc8(sim.miso, RIMLINK_OFF_CRC) !=
	    sim.miso[RIMLINK_OFF_CRC]) {
		sim.miso_crc_err++;
	}
#endif /* HAVE_SPI */
}

static void sim_thread(void *a, void *b, void *c)
{
	ARG_UNUSED(a);
	ARG_UNUSED(b);
	ARG_UNUSED(c);

	int64_t last_report = k_uptime_get();
	uint32_t last_txn = 0;

	while (1) {
		if (!sim.running) {
			k_sleep(K_MSEC(50));
			continue;
		}

		run_transaction();

		uint32_t sleep_us = sim.cadence_us;

		if (sim.jitter_us > 0U) {
			sleep_us += sys_rand32_get() % sim.jitter_us;
		}
		if (sim.fault == FAULT_JITTER && sim.fault_n != 0U) {
			sleep_us += (sys_rand32_get() % sim.fault_n) * 1000U;
			sim.faults_injected++;
		}
		k_sleep(K_USEC(MAX(100U, sleep_us)));

		int64_t now = k_uptime_get();

		if ((now - last_report) >= 1000) {
			LOG_INF("txn/s=%u total=%u miso_crc_err=%u "
				"first_byte_err=%u injected=%u",
				sim.txn - last_txn, sim.txn,
				sim.miso_crc_err, sim.first_byte_err,
				sim.faults_injected);
			last_txn = sim.txn;
			last_report = now;
		}
	}
}

K_THREAD_DEFINE(sim_tid, 2048, sim_thread, NULL, NULL, NULL, 5, 0, 0);

/* ---------------- shell ---------------- */

static int cmd_sim_start(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);
	sim.running = true;
	shell_print(sh, "running");
	return 0;
}

static int cmd_sim_stop(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);
	sim.running = false;
	shell_print(sh, "stopped");
	return 0;
}

static int cmd_sim_rate(const struct shell *sh, size_t argc, char **argv)
{
	if (argc == 2) {
		int us = atoi(argv[1]);

		if (us < 100) {
			shell_error(sh, "cadence >= 100 us");
			return -EINVAL;
		}
		sim.cadence_us = (uint32_t)us;
	}
	shell_print(sh, "cadence: %u us", sim.cadence_us);
	return 0;
}

/* sim profile [clock_hz cadence_us jitter_us flush] - base twin (2-S6). */
static int cmd_sim_profile(const struct shell *sh, size_t argc, char **argv)
{
	if (argc == 5) {
		uint32_t hz = (uint32_t)strtoul(argv[1], NULL, 0);

		if (hz < 100000U || hz > 12000000U) {
			shell_error(sh, "clock 100k..12M (schema cap)");
			return -EINVAL;
		}
		sim.clock_hz = hz;
		sim.cadence_us = (uint32_t)strtoul(argv[2], NULL, 0);
		sim.jitter_us = (uint32_t)strtoul(argv[3], NULL, 0);
		sim.flush_bytes = (uint32_t)strtoul(argv[4], NULL, 0);
	}
	shell_print(sh, "profile %s: clock=%u Hz cadence=%u us jitter=%u us "
		    "cs_setup=%u us flush=%u B", SIM_PROFILE_NAME,
		    sim.clock_hz, sim.cadence_us, sim.jitter_us,
		    (unsigned int)SIM_CS_SETUP_US, sim.flush_bytes);
	return 0;
}

/* sim disp <txt>: encode up to 3 chars into disp[3] (spec section 8). */
static int cmd_sim_disp(const struct shell *sh, size_t argc, char **argv)
{
	const char *txt = (argc == 2) ? argv[1] : "";
	k_spinlock_key_t key = k_spin_lock(&sim.lock);

	for (int i = 0; i < 3; i++) {
		sim.mosi[RIMLINK_OFF_DISP + i] =
			(i < (int)strlen(txt)) ? seg7_from_ascii(txt[i])
					       : 0x00U;
	}
	seal_mosi();
	k_spin_unlock(&sim.lock, key);
	shell_print(sh, "disp set to \"%.3s\"", txt);
	return 0;
}

/* sim leds <hex16> / sim rumble <b0> <b1>: exercise output fields. */
static int cmd_sim_leds(const struct shell *sh, size_t argc, char **argv)
{
	uint16_t v = (uint16_t)strtoul(argv[1], NULL, 0);
	k_spinlock_key_t key = k_spin_lock(&sim.lock);

	sim.mosi[RIMLINK_OFF_LEDS] = (uint8_t)v;
	sim.mosi[RIMLINK_OFF_LEDS + 1] = (uint8_t)(v >> 8);
	seal_mosi();
	k_spin_unlock(&sim.lock, key);
	shell_print(sh, "leds = 0x%04X", v);
	ARG_UNUSED(argc);
	return 0;
}

static int cmd_sim_rumble(const struct shell *sh, size_t argc, char **argv)
{
	k_spinlock_key_t key = k_spin_lock(&sim.lock);

	sim.mosi[RIMLINK_OFF_RUMBLE] = (uint8_t)strtoul(argv[1], NULL, 0);
	sim.mosi[RIMLINK_OFF_RUMBLE + 1] = (uint8_t)strtoul(argv[2], NULL, 0);
	seal_mosi();
	k_spin_unlock(&sim.lock, key);
	shell_print(sh, "rumble = %02X %02X", sim.mosi[RIMLINK_OFF_RUMBLE],
		    sim.mosi[RIMLINK_OFF_RUMBLE + 1]);
	ARG_UNUSED(argc);
	return 0;
}

/* sim fault none|crc|trunc|extra|jitter [N] */
static int cmd_sim_fault(const struct shell *sh, size_t argc, char **argv)
{
	if (strcmp(argv[1], "none") == 0) {
		sim.fault = FAULT_NONE;
	} else if (strcmp(argv[1], "crc") == 0) {
		sim.fault = FAULT_CRC;
	} else if (strcmp(argv[1], "trunc") == 0) {
		sim.fault = FAULT_TRUNC;
	} else if (strcmp(argv[1], "extra") == 0) {
		sim.fault = FAULT_EXTRA;
	} else if (strcmp(argv[1], "jitter") == 0) {
		sim.fault = FAULT_JITTER;
	} else {
		shell_error(sh, "none|crc|trunc|extra|jitter [N]");
		return -EINVAL;
	}
	if (argc == 3) {
		sim.fault_n = (uint32_t)atoi(argv[2]);
	}
	shell_print(sh, "fault mode %d, N=%u", sim.fault, sim.fault_n);
	return 0;
}

static int cmd_sim_stats(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);
	shell_print(sh, "txn:            %u", sim.txn);
	shell_print(sh, "miso_crc_err:   %u", sim.miso_crc_err);
	shell_print(sh, "first_byte_err: %u", sim.first_byte_err);
	shell_print(sh, "faults_injected:%u", sim.faults_injected);
	return 0;
}

static int cmd_sim_reset(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);
	sim.txn = sim.miso_crc_err = sim.first_byte_err =
		sim.faults_injected = 0;
	shell_print(sh, "counters reset");
	return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_sim,
	SHELL_CMD(start, NULL, "start clocking transactions", cmd_sim_start),
	SHELL_CMD(stop, NULL, "stop", cmd_sim_stop),
	SHELL_CMD_ARG(rate, NULL, "[us] get/set cadence", cmd_sim_rate, 1, 1),
	SHELL_CMD_ARG(profile, NULL, "[hz cad_us jit_us flush] base twin", cmd_sim_profile, 1, 4),
	SHELL_CMD_ARG(disp, NULL, "<txt> encode 3 chars to disp[3]", cmd_sim_disp, 1, 1),
	SHELL_CMD_ARG(leds, NULL, "<hex16> set LED bitfield", cmd_sim_leds, 2, 0),
	SHELL_CMD_ARG(rumble, NULL, "<b0> <b1> set rumble bytes", cmd_sim_rumble, 3, 0),
	SHELL_CMD_ARG(fault, NULL, "none|crc|trunc|extra|jitter [N]", cmd_sim_fault, 2, 1),
	SHELL_CMD(stats, NULL, "per-run statistics", cmd_sim_stats),
	SHELL_CMD(reset, NULL, "reset counters", cmd_sim_reset),
	SHELL_SUBCMD_SET_END
);
SHELL_CMD_REGISTER(sim, &sub_sim, "Base-side simulator control", NULL);

int main(void)
{
	printk("Rim-Link base simulator, twin profile '%s' "
	       "(%u Hz, %u us cadence, flush %u)\n",
	       SIM_PROFILE_NAME, sim.clock_hz, sim.cadence_us,
	       sim.flush_bytes);

#if !HAVE_SPI
	LOG_ERR("spi1 not enabled in devicetree");
#else
	if (!device_is_ready(spi_dev)) {
		LOG_ERR("SPI device not ready");
	}
#endif
	seal_mosi();
	return 0;
}
