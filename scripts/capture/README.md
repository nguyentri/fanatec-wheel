# Phase 2 Capture Toolkit (spec 2-S4)

Host-side scripts for real-base evidence gathering. Requires Python 3.10+,
`pyyaml` for the matrix renderer.

| Script | Purpose |
|---|---|
| `la_decode.py <la.csv>` | LA byte export -> frame-model JSON lines |
| `ring_diff.py <dut_dump.txt> <la.jsonl>` | DUT `rim cap dump` vs LA truth |
| `link_stats.py <la.csv>` | clock/cadence/CS-gap stats -> twin profile values |
| `field_activity.py <la.jsonl>` | disp/leds/rumble activity report (roadmap 11.4) |
| `render_matrix.py [yaml] [md]` | compat matrix YAML -> markdown |

LA CSV format: `time,mosi,miso` per byte; blank or `---` row at each CS
deassertion. `fixtures/` holds a synthetic session for smoke testing:

    python3 la_decode.py fixtures/sample_la.csv > /tmp/la.jsonl
    python3 field_activity.py /tmp/la.jsonl
    python3 link_stats.py fixtures/sample_la.csv
    python3 ring_diff.py fixtures/sample_ring_dump.txt /tmp/la.jsonl
