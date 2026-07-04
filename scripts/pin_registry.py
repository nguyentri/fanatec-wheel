#!/usr/bin/env python3
"""Generate a pin registry table from a devicetree overlay
(spec 4-S4): every gpios/pinctrl reference, its node, and purpose -
the single source for schematic cross-checks on PCB rev A."""
import re
import sys


def main(path):
    text = open(path).read()
    rows = []

    for m in re.finditer(
            r"(\w+):\s*\w+\s*{\s*gpios\s*=\s*<&gpio(\w)\s+(\d+)([^>]*)>",
            text):
        label, port, pin, flags = m.groups()
        rows.append((f"P{port.upper()}{pin}", label,
                     "gpio", flags.strip() or "-"))

    for m in re.finditer(r"pinctrl-0\s*=\s*<([^>]+)>", text):
        for ref in re.findall(r"&(\w+)", m.group(1)):
            pm = re.search(r"_(p[a-k]\d+)$", ref)
            if pm:
                rows.append((pm.group(1).upper(), ref, "pinctrl", "-"))

    rows.sort()
    print("| Pin | Node/Function | Kind | Flags |")
    print("|---|---|---|---|")
    dup = set()
    for pin, label, kind, flags in rows:
        mark = " **CONFLICT**" if pin in dup else ""
        dup.add(pin)
        print(f"| {pin}{mark} | {label} | {kind} | {flags} |")


if __name__ == "__main__":
    main(sys.argv[1])
