/*
 * Copyright (c) 2026 Fanatec Wheel Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "output_svc.h"

LOG_MODULE_REGISTER(output_svc, CONFIG_APP_LOG_LEVEL);

/*
 * 7-segment -> ASCII decode, ported from the reference sketch
 * (spec section 2.3). Bit 7 = dot, masked before lookup; unknown
 * patterns are rendered as hex by the caller.
 * TODO(phase1): port the full table verbatim from the reference sketch.
 */
char output_svc_7seg_to_ascii(uint8_t seg)
{
	switch (seg & 0x7FU) {
	case 0x3F: return '0';
	case 0x06: return '1';
	case 0x5B: return '2';
	case 0x4F: return '3';
	case 0x66: return '4';
	case 0x6D: return 'S'; /* also '5' */
	case 0x7D: return '6';
	case 0x07: return '7';
	case 0x7F: return '8';
	case 0x6F: return '9';
	case 0x39: return 'C';
	case 0x40: return '-';
	case 0x00: return ' ';
	default:   return '\0'; /* unknown: caller prints hex */
	}
}

int output_svc_init(void)
{
	/*
	 * TODO(phase1):
	 *  - consume validated struct base_outputs from the adapter
	 *    (workqueue context, never ISR)
	 *  - change-gated decode: act only when disp[3] differs from the
	 *    previous CRC-valid value
	 *  - TM1637 refresh (>= 10 ms min interval) when CONFIG_RIM_TM1637
	 *  - accumulate leds_last / rumble_last + change counters
	 */
	LOG_INF("output service ready (TM1637: %s)",
		IS_ENABLED(CONFIG_RIM_TM1637) ? "enabled" : "disabled");
	return 0;
}
