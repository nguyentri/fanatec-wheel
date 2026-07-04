#!/usr/bin/env python3
"""Render docs/compat/matrix.yaml to a markdown table (spec 2-S5)."""
import sys

import yaml  # pip install pyyaml

COLS = ["base_model", "base_fw", "qr_gen", "rim_identity", "result",
        "capture_refs", "date"]


def main(src, dst):
    rows = yaml.safe_load(open(src))["matrix"]
    with open(dst, "w") as f:
        f.write("# Compatibility Matrix (generated - do not edit)\n\n")
        f.write("| " + " | ".join(COLS) + " |\n")
        f.write("|" + "---|" * len(COLS) + "\n")
        for r in rows:
            f.write("| " + " | ".join(
                ", ".join(r[c]) if isinstance(r[c], list) else str(r[c])
                for c in COLS) + " |\n")
    print(f"rendered {len(rows)} rows -> {dst}")


if __name__ == "__main__":
    main(sys.argv[1] if len(sys.argv) > 1 else "docs/compat/matrix.yaml",
         sys.argv[2] if len(sys.argv) > 2 else "docs/compat/matrix.md")
