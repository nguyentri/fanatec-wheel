#!/usr/bin/env python3
"""Generate sim_profile.h from a base-twin YAML profile (spec 2-S6)."""
import re
import sys

KEYS = ["clock_hz", "cadence_us", "cadence_jitter_us", "cs_setup_us",
        "flush_bytes"]


def main(src, dst):
    text = open(src).read()
    vals, name = {}, "unnamed"
    for line in text.splitlines():
        line = line.split("#")[0].strip()
        m = re.match(r"(\w+):\s*(\S+)", line)
        if not m:
            continue
        k, v = m.groups()
        if k == "profile":
            name = v
        elif k in KEYS:
            vals[k] = int(v)
    missing = [k for k in KEYS if k not in vals]
    assert not missing, f"profile missing keys: {missing}"
    assert vals["clock_hz"] <= 12_000_000, "schema cap: 12 MHz"
    with open(dst, "w") as f:
        f.write("/* Generated from %s - do not edit. */\n" % src)
        f.write("#ifndef SIM_PROFILE_H_\n#define SIM_PROFILE_H_\n")
        f.write('#define SIM_PROFILE_NAME "%s"\n' % name)
        for k in KEYS:
            f.write("#define SIM_PROFILE_%s %d\n" % (k.upper(), vals[k]))
        f.write("#endif\n")


if __name__ == "__main__":
    main(sys.argv[1], sys.argv[2])
