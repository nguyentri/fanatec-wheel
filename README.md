# Fanatec Steering Wheel — Zephyr Rim-Link Firmware

Clean-room firmware for an advanced Fanatec-compatible steering-wheel **rim**,
built on **Zephyr 4.4.0** for the **STM32H723ZGT6**. The rim presents itself to
a Fanatec wheel base as an SPI peripheral over the quick-release link, acquires
the full GT input fabric, renders its own LEDs and haptics, and hardens the
product with verified boot, persisted health telemetry, and a watchdog — while
keeping the base's force feedback untouched.

Protocol behavior is derived from public community implementations and bench
evidence (a port of `Arduino_Fanatec_Wheel`), never from proprietary assets. The
rim produces **no torque**; it is an input/display peripheral.

**Target board:** [FK723M1-ZGT6](https://docs.zephyrproject.org/latest/boards/fanke/fk723m1_zgt6/doc/index.html)
(STM32H723ZGT6, upstream Zephyr board `fk723m1_zgt6`).

---

## Highlights

- **Legacy rim link** — 33-byte SPI-peripheral frames sealed with CRC-8, identity
  presentation, TX double-buffering with a freshness swap, a 50 ms stale-input
  safety clear, and a validated RX path handed off to a workqueue. All protocol
  constants live in exactly one versioned adapter (`lib/rimlink`).
- **Full input fabric @ 1 kHz** — debounced buttons, an analog D-pad ladder, four
  quadrature encoders (transition-table decode, illegal-transition counters), a
  funky switch with independent debounce and chord-fault detection, and two Hall
  clutch channels (median-of-3 -> IIR -> calibration -> deadzone -> plausibility)
  with dual-clutch bite-point / two-axis / mappable modes. Worst-case compose
  time is measured, not assumed (`rim stats`).
- **Local output rendering** — the 16-bit `leds` field drives an **LVGL race
  screen** on a digital LCD: a 0..10-scaled 15-segment rev bar, the current gear
  inside an arc (decoded from the display bytes), and 8 flag indicators
  (change-driven <= 60 Hz, quiet-state banner after 200 ms of link silence), and bounded
  haptic cues (duration cap + cooldown; the frame rumble source is disabled by
  default pending real-base evidence). Both back-ends are devicetree-guarded, so
  the same binary runs on the bench board (counters-only) and on custom hardware.
- **Configuration & persistence** — a versioned settings schema (NVS) for
  calibration, mapping, and clutch mode, with a field write-lock. Commits happen
  only from the diagnostic context, never the fast path.
- **Instrumentation & evidence** — a 256-entry transaction capture ring that
  freezes on anomaly, a boot-to-ready measurement, a session-attribution header
  (firmware, git hash, identity, config, base-under-test), and a host toolkit
  that reconciles a device capture against a logic-analyzer export.
- **Product hardening** — MCUboot verified boot (ED25519, dual-slot swap with
  automatic revert), persisted health counters, an IWDG watchdog fed only by the
  input thread, a soak runner, and a defined release process.
- **Base simulator** — a second-board application that drives the link from a
  configurable base-twin profile (clock, cadence, jitter, flush behavior) with
  fault injectors, so the rim is exercised end-to-end without hardware.

---

## Repository layout

```
lib/rimlink/          Link adapter v1 "legacy-spi": frame build/parse, CRC-8,
                      identity, TX double-buffer, RX queue, stats, capture ring.
                      The ONLY place protocol offsets and constants appear.
app/                  Rim firmware (device under test)
  src/
    main.c            Boot ordering: fast path first, services deferred
    link_spi.c        SPI1 slave glue, CS-sense EXTI re-arm, LINK_READY
    input_svc.c       1 kHz acquisition -> immutable snapshot
    encoder|funky|clutch|debounce|dpad|seg7|tm1637.c   pure decoders + helpers
    output_svc.c      validated-frame fan-out + display decode
    lcd_svc.c         LVGL race screen: rev bar, gear-in-arc, flags, quiet state
    lra_svc.c         bounded haptic cue service (source gated)
    power_mgr.c       output-rail sequencing state machine
    rim_wdt.c         IWDG fed by the input thread
    rim_settings.c    settings schema (NVS) + config lock
    health.c          persisted health counters
    diag_svc.c        `rim ...` shell, session header
  boards/...overlay   pins, ADC channels, flash partitions
  sysbuild.conf +     MCUboot verified-boot configuration (signed image)
    sysbuild/
  prj.conf / Kconfig  RIM_FASTBOOT, RIM_WATCHDOG, RIM_TM1637, ...
app/sim/              Base-side simulator + base-twin profile (`sim` shell)
scripts/
  capture/            LA decode, device-ring diff, link stats, field activity,
                      matrix renderer, and fixtures (host toolkit)
  soak/soak_runner.py thresholded dual-console soak with capture-freeze
  pin_registry.py     overlay -> pin table for schematic cross-checks
drivers/              Reusable out-of-tree driver-class templates (blink,
                      example_sensor) + bindings under dts/bindings/
tests/unit/rimlink    adapter, capture, link, and health suites (native_sim)
tests/unit/app_logic  decoder + LED/LRA pure-logic suites (native_sim)
tests/func/example_sensor   on-target functional build
docs/                 System roadmap + hardware/software specifications
west.yml              Zephyr v4.4.0 manifest (hal_stm32, cmsis_6, picolibc,
                      segger, mcuboot, lvgl)
```

---

## Getting started

```shell
# one-time
pip install west
west init -l .          # if .west/ is not present
west update             # fetches Zephyr and modules into 3rd_party/
west packages pip --install
```

### Build

```shell
# Rim firmware (device under test)
west build -p -b fk723m1_zgt6 app

# Rim firmware with the verified-boot chain (MCUboot + ED25519-signed image)
pip install cbor2
west build -p -b fk723m1_zgt6 app --sysbuild
#   -> build/mcuboot/zephyr/zephyr.bin    (flash at 0x08000000)
#   -> build/app/zephyr/zephyr.signed.bin (slot0 at 0x08020000)

# Optional variants
west build -p -b fk723m1_zgt6 app -- -DCONFIG_RIM_TM1637=y   # TM1637 display mirror
west build -p -b fk723m1_zgt6 app -- -DCONFIG_RIM_WATCHDOG=y # IWDG enabled

# Base-side simulator (second board)
west build -p -b fk723m1_zgt6 app/sim

# Functional example (custom driver class + out-of-tree sensor driver)
west build -p -b fk723m1_zgt6 tests/func/example_sensor
```

### Flash

```shell
west flash    # on-board ST-Link / OpenOCD per the upstream board docs
```

### Test

```shell
# Host-native unit suites (native_sim)
west build -p -b native_sim/native/64 tests/unit/rimlink && ./build/zephyr/zephyr.exe
west build -p -b native_sim/native/64 tests/unit/app_logic && ./build/zephyr/zephyr.exe
# or the whole set:
west twister -T tests/unit --integration

# Host toolkit smoke test (from a device capture / LA export)
cd scripts/capture
python3 la_decode.py fixtures/sample_la.csv > /tmp/la.jsonl
python3 field_activity.py /tmp/la.jsonl
python3 ring_diff.py fixtures/sample_ring_dump.txt /tmp/la.jsonl
```

The CRC-8 reference vector `A5 03` + 30 zero bytes -> `0x5A` is asserted by the
adapter suite, alongside a 200-vector equivalence check against the reference
table.

---

## Diagnostic shell

The rim exposes a `rim` command tree over the Zephyr shell; the simulator exposes
`sim`.

| Command | Purpose |
|---|---|
| `rim mosi` / `rim miso` / `rim disp` | Inspect the last base->rim frame, the armed rim->base frame, and the decoded display text |
| `rim btn <set\|clr\|toggle> <1..22>` | Force a logical button bit for testing |
| `rim map [idx bit]` / `rim id [1..4]` | Show or set the button map and rim identity |
| `rim dpad [t1..t7 hyst]` | Show or calibrate the D-pad ladder |
| `rim input` | Dump the v2 fabric state (encoders, funky, clutches) |
| `rim clutch [mode bite]` | Show or set the dual-clutch mode and bite point |
| `rim save <cal\|map\|mode>` | Persist a settings subtree (rejected while locked) |
| `rim lock <on\|off\|status>` | Engage or release the configuration write-lock |
| `rim session [base <str>]` | Emit the session header; tag the base under test |
| `rim boot` | Report boot-to-ready (us from reset to first armed transfer) |
| `rim cap <start\|stop\|freeze\|dump\|status\|auto>` | Drive the transaction capture ring |
| `rim stats` / `rim reset` | Link statistics (CRC/short/overrun/re-arm, CS-gap, latency P99/max) |
| `rim health` | Persisted counters (power cycles, transactions, errors, watchdog resets) |
| `sim start\|stop\|rate\|profile\|fault\|disp\|leds` | Drive and perturb the base simulator |

---

## Host tooling

- `scripts/capture/` — turn a logic-analyzer SPI export into the frame model,
  diff it against a device `rim cap dump`, extract clock/cadence/CS-gap
  statistics to feed the simulator's base-twin profile, and report per-field
  output activity from a donor wheel. See `scripts/capture/README.md`.
- `scripts/soak/soak_runner.py` — drive the rim and simulator consoles for hours,
  enforce zero-tolerance thresholds, and freeze the capture ring on the first
  anomaly. Runs offline against recorded logs with `--replay`.
- `scripts/pin_registry.py` — generate a pin table from the board overlay for
  schematic cross-checks; reports conflicts.

---

## Documentation

- [`docs/fanatec-wheel-roadmap-and-system-spec.md`](docs/fanatec-wheel-roadmap-and-system-spec.md)
  — system architecture and long-form roadmap.
- [`docs/phase1-software-spec.md`](docs/phase1-software-spec.md) ·
  [`docs/phase1-hardware-spec.md`](docs/phase1-hardware-spec.md) — the link
  adapter, core services, and simulator.
- [`docs/phases2-6-software-spec.md`](docs/phases2-6-software-spec.md) ·
  [`docs/phases2-6-hardware-spec.md`](docs/phases2-6-hardware-spec.md) — the
  instrumentation, input fabric, output rendering, and hardening work.

The specification documents use a stable requirement-ID scheme (`2-S1`, `3-S3`,
`5-S5`, ...) that source comments cite, so any module can be traced back to the
requirement it satisfies.

---

## Status

The firmware is feature-complete on the bench board and builds clean (zero
warnings) across all targets: the rim application, the TM1637 and watchdog
variants, the MCUboot-signed sysbuild pair, the simulator, and the functional
example. The host unit suites (adapter, capture, link, health, encoder, funky,
clutch, LED, LRA, debounce, D-pad, seg7) all pass on `native_sim`.

Items that require a real base or custom hardware are wired and instrumented but
marked **measurement pending** in the code and specs: real-base link captures and
the boot-margin measurement, the full-fabric latency report, the 24-hour output
isolation soak, and the interrupted-update torture test. Devicetree guards keep
the LCD panel (`chosen zephyr,display` + `CONFIG_LVGL`), haptic driver,
output-rail load switch, and windowed-watchdog part as drop-in points on a future
PCB without code changes. Note: the LVGL-enabled image (~420 KiB) still exceeds
the expanded 384 KiB MCUboot slot — prune LVGL widgets/fonts or revise the
partition plan further before signing that variant (the bench image is unaffected
at ~106 KiB).

---

## License

Apache-2.0. See [`LICENSE`](LICENSE). This project redistributes no vendor
firmware or proprietary assets and claims no official compatibility,
certification, or endorsement.
