#!/usr/bin/env python3
"""Decode a logic-analyzer SPI export into the frame model (spec 2-S4).

Input: CSV with columns time,mosi,miso (one row per byte; blank line or
'---' row = CS deassertion / transaction boundary). This matches the
per-byte export of common LA tools (e.g. Saleae SPI analyzer CSV after
column selection).

Output: one JSON line per transaction:
{n, t0, len, mosi_crc_ok, miso_crc_ok, mosi_hex, miso_hex}
"""
import csv
import json
import sys

from frame_model import FRAME_LEN, crc8


def transactions(rows):
    cur = []
    for row in rows:
        if not row or all(c.strip() in ("", "---") for c in row):
            if cur:
                yield cur
                cur = []
            continue
        cur.append(row)
    if cur:
        yield cur


def main(path):
    with open(path, newline="") as f:
        rows = list(csv.reader(f))
    if rows and not rows[0][0].replace(".", "").replace("-", "").isdigit():
        rows = rows[1:]  # header line

    for n, txn in enumerate(transactions(rows)):
        t0 = float(txn[0][0])
        mosi = bytes(int(r[1], 0) for r in txn)
        miso = bytes(int(r[2], 0) for r in txn)
        rec = {
            "n": n, "t0": t0, "len": len(txn),
            "mosi_crc_ok": len(mosi) == FRAME_LEN
                           and crc8(mosi[:32]) == mosi[32],
            "miso_crc_ok": len(miso) == FRAME_LEN
                           and crc8(miso[:32]) == miso[32],
            "mosi_hex": mosi.hex(), "miso_hex": miso.hex(),
        }
        print(json.dumps(rec))


if __name__ == "__main__":
    main(sys.argv[1])
