#!/usr/bin/env python3
"""Soak automation (spec 5-S4 / output isolation proof).

Drives the DUT and the base-simulator consoles over serial for N
hours, polls counters, enforces thresholds, freezes the DUT capture
ring on the first anomaly, and emits a pass/fail report.

Execution requires bench hardware (two consoles) - measurement
pending. The threshold logic below is the same used by CI against
recorded console logs (--replay).

Usage:
  soak_runner.py --dut /dev/ttyACM0 --sim /dev/ttyACM1 --hours 24
  soak_runner.py --replay dut_console.log            # offline check
"""
import argparse
import json
import re
import sys
import time

THRESHOLDS = {
    "crc_err_rx": 0,       # zero tolerated during soak (spec 2.3)
    "short_frame": 0,
    "overrun": 0,
    "rearm_miss": 10,      # boot transient allowance
    "stale_clear": 0,
    "funky_faults": 0,
    "wdt_resets": 0,
}

STAT_RE = re.compile(r"(\w+)[=:]\s*(\d+)")


def parse_stats(text):
    return {k: int(v) for k, v in STAT_RE.findall(text)}


def evaluate(stats):
    failures = []
    for key, limit in THRESHOLDS.items():
        if stats.get(key, 0) > limit:
            failures.append(f"{key}={stats[key]} > {limit}")
    return failures


def run_live(dut_port, sim_port, hours, poll_s):
    import serial  # pip install pyserial

    dut = serial.Serial(dut_port, 115200, timeout=2)
    sim = serial.Serial(sim_port, 115200, timeout=2)

    def cmd(port, line):
        port.write((line + "\n").encode())
        time.sleep(0.3)
        return port.read(4096).decode(errors="replace")

    cmd(dut, "rim session")
    cmd(dut, "rim cap start")       # freeze-on-anomaly is default-on
    cmd(sim, "sim start")

    t_end = time.time() + hours * 3600
    stats = {}
    while time.time() < t_end:
        text = cmd(dut, "rim stats") + cmd(dut, "rim input") + \
               cmd(dut, "rim health")
        stats = parse_stats(text)
        failures = evaluate(stats)
        if failures:
            print(cmd(dut, "rim cap status"))  # ring auto-froze (2-M2)
            report(stats, failures)
            return 1
        time.sleep(poll_s)

    report(stats, [])
    return 0


def report(stats, failures):
    verdict = "FAIL" if failures else "PASS"
    print(json.dumps({"verdict": verdict, "failures": failures,
                      "stats": stats}, indent=2))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--dut")
    ap.add_argument("--sim")
    ap.add_argument("--hours", type=float, default=24)
    ap.add_argument("--poll", type=float, default=30)
    ap.add_argument("--replay", help="offline console log check")
    args = ap.parse_args()

    if args.replay:
        stats = parse_stats(open(args.replay).read())
        failures = evaluate(stats)
        report(stats, failures)
        return 1 if failures else 0
    if not (args.dut and args.sim):
        ap.error("--dut and --sim required (or --replay)")
    return run_live(args.dut, args.sim, args.hours, args.poll)


if __name__ == "__main__":
    sys.exit(main())
