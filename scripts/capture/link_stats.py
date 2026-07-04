#!/usr/bin/env python3
"""Extract clock rate, cadence, and CS-gap statistics from an LA
byte-level export (spec 2-S4). Feeds the simulator base-twin profile.

Input: same CSV as la_decode.py. Output: YAML-ish profile block ready
for app/sim/profiles/base_twin.yaml.
"""
import csv
import statistics as st
import sys


def main(path):
    with open(path, newline="") as f:
        rows = [r for r in csv.reader(f)]
    if rows and not rows[0][0].replace(".", "").replace("-", "").isdigit():
        rows = rows[1:]

    txn_starts, txn_ends, byte_gaps = [], [], []
    prev_t, cur_start = None, None
    for row in rows:
        boundary = not row or all(c.strip() in ("", "---") for c in row)
        if boundary:
            if prev_t is not None and cur_start is not None:
                txn_starts.append(cur_start)
                txn_ends.append(prev_t)
            cur_start = None
            continue
        t = float(row[0])
        if cur_start is None:
            cur_start = t
        elif prev_t is not None:
            byte_gaps.append(t - prev_t)
        prev_t = t
    if cur_start is not None:
        txn_starts.append(cur_start)
        txn_ends.append(prev_t)

    if byte_gaps:
        byte_period = st.median(byte_gaps)
        print(f"# measured byte period {byte_period*1e6:.2f} us")
        print(f"clock_hz: {int(8 / byte_period)}")
    if len(txn_starts) >= 2:
        cad = [b - a for a, b in zip(txn_starts, txn_starts[1:])]
        gaps = [s - e for e, s in zip(txn_ends, txn_starts[1:])]
        print(f"cadence_us: {int(st.median(cad) * 1e6)}")
        print(f"cadence_jitter_us: {int(st.pstdev(cad) * 1e6)}")
        print(f"cs_gap_min_us: {int(min(gaps) * 1e6)}")
        print(f"cs_gap_max_us: {int(max(gaps) * 1e6)}")


if __name__ == "__main__":
    main(sys.argv[1])
