#!/usr/bin/env python3
"""Donor-wheel capture report: disp/leds/rumble field activity
(roadmap section 11.4, spec 2-S4). Answers: does the base populate
each output field for the chosen identity? Feeds the 4-S2 lra_svc
enable decision and the 4-S1 LED map confirmation.

Input: la_decode.py JSON-lines. Output: per-field activity summary.
"""
import json
import sys

from frame_model import parse_frame


def main(path):
    disp_vals, led_bits, rumble_seen, total, valid = set(), 0, set(), 0, 0
    for line in open(path):
        rec = json.loads(line)
        total += 1
        if not rec["mosi_crc_ok"]:
            continue
        valid += 1
        f = parse_frame(bytes.fromhex(rec["mosi_hex"]))
        disp_vals.add(tuple(f["disp"]))
        led_bits |= f["leds"]
        if any(f["rumble"]):
            rumble_seen.add(tuple(f["rumble"]))

    print(f"transactions: {total} (crc-valid {valid})")
    print(f"disp: {len(disp_vals)} distinct values"
          f" -> {'ACTIVE' if len(disp_vals) > 1 else 'static'}")
    print(f"leds: bits ever set = 0x{led_bits:04X}"
          f" -> {'ACTIVE' if led_bits else 'silent'}")
    print(f"rumble: {'ACTIVE ' + str(sorted(rumble_seen)[:8]) if rumble_seen else 'silent (lra_svc stays disabled per spec 4-S2)'}")


if __name__ == "__main__":
    main(sys.argv[1])
