#!/usr/bin/env python3
"""Diff a DUT capture-ring dump against LA truth (spec 2-S4).

DUT input: `rim cap dump` console text (CAP,idx,ts,DIR,crc_ok,gap,hex).
LA input:  la_decode.py JSON-lines output.

Matches RX entries against LA MOSI frames (and TX vs MISO) by content
sequence; reports missing/extra/corrupt-mismatch counts. The result
must reconcile to zero unexplained discrepancy (spec 2.3).
"""
import json
import sys


def load_dut(path):
    tx, rx = [], []
    for line in open(path):
        if not line.startswith("CAP,"):
            continue
        _, idx, ts, d, crc_ok, gap, hexs = line.strip().split(",", 6)
        (tx if d == "TX" else rx).append(
            {"hex": hexs.lower(), "crc_ok": crc_ok == "1"})
    return tx, rx


def load_la(path):
    recs = [json.loads(l) for l in open(path)]
    return ([r["miso_hex"] for r in recs], [r["mosi_hex"] for r in recs])


def seq_diff(name, dut, la):
    dset, lset = [e["hex"] for e in dut], la
    i = j = match = 0
    while i < len(dset) and j < len(lset):
        if dset[i] == lset[j]:
            match += 1
            i += 1
            j += 1
        elif len(dset) - i > len(lset) - j:
            i += 1  # extra in DUT
        else:
            j += 1  # missing from DUT (e.g. ring wrapped)
    print(f"{name}: dut={len(dset)} la={len(lset)} matched={match} "
          f"dut_extra={len(dset)-match} la_unmatched={len(lset)-match}")
    return match


def main(dut_path, la_path):
    tx, rx = load_dut(dut_path)
    miso, mosi = load_la(la_path)
    seq_diff("RX(mosi)", rx, mosi)
    seq_diff("TX(miso)", tx, miso)


if __name__ == "__main__":
    main(sys.argv[1], sys.argv[2])
