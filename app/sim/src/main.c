/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 *
 * Base-side simulator (spec section 8): clocks 33-byte Mode-1
 * transactions at a shell-configurable rate, seals MOSI frames with
 * CRC-8, verifies MISO CRC + header 0xA5, runs the fault-injection set
 * {truncate at byte N, corrupt CRC, extra clocks, min CS gap, cadence
 * jitter}, encodes shell text into disp[3], and streams per-second
 * statistics. Simulator and DUT counters must reconcile to zero
 * unexplained discrepancy.
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <lib/rimlink/rimlink.h>

LOG_MODULE_REGISTER(sim, LOG_LEVEL_INF);

#define SIM_CADENCE_MS 2 /* default 2 ms cadence (Gate G1 soak rate) */

static uint8_t mosi[RIMLINK_FRAME_LEN];
static uint32_t txn_count;
static uint32_t miso_crc_err;
static uint32_t first_byte_err;

static void seal_mosi(void)
{
	/* header/id recorded-not-interpreted on the rim side; use the
	 * same constants for easy scope correlation. */
	mosi[0] = RIMLINK_HEADER;
	mosi[1] = RIM_ID_PORSCHE_918_RSR;
	/* disp[3] at 2..4, leds at 5..6, rumble at 7..8: shell-driven.
	 * TODO(phase1): `sim disp <txt>` 7-seg encoder. */
	mosi[RIMLINK_OFF_CRC] = rimlink_crc8(mosi, RIMLINK_OFF_CRC);
}

int main(void)
{
	printk("Rim-Link Phase 1 base simulator\n");

	seal_mosi();

	while (1) {
		/*
		 * TODO(phase1):
		 *  - SPI controller transceive: Mode 1, MSB first,
		 *    CS-assert-to-first-clock >= 5 us, base-side flush
		 *    emulation (spec section 2.1)
		 *  - verify MISO: header 0xA5 (first_byte_err++),
		 *    CRC byte 32 (miso_crc_err++)
		 *  - fault injectors, each >= 1000 events
		 *  - per-second stats line: txn / crc err / first-byte err
		 */
		txn_count++;
		k_sleep(K_MSEC(SIM_CADENCE_MS));

		if ((txn_count % (1000U / SIM_CADENCE_MS)) == 0U) {
			LOG_INF("txn=%u miso_crc_err=%u first_byte_err=%u",
				txn_count, miso_crc_err, first_byte_err);
		}
	}

	return 0;
}
