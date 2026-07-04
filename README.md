# Fanatec Steering Wheel — Zephyr Rim-Link Firmware

Zephyr 4.4.0 workspace for the Phase 1 rim-side prototype: a port of the
`Arduino_Fanatec_Wheel` behavior (legacy 33-byte SPI exchange, CRC-8,
identity, button snapshots) behind the versioned `rimlink` adapter, plus
the base-side simulator. Specifications live in [`docs/`](docs/).

**Target board:** [FK723M1-ZGT6](https://docs.zephyrproject.org/latest/boards/fanke/fk723m1_zgt6/doc/index.html)
(STM32H723ZGT6, upstream Zephyr board `fk723m1_zgt6`).

## Repository layout

```
app/                 Rim firmware (DUT): main + link_spi / input_svc /
                     output_svc / diag_svc templates (phase1 spec section 3)
app/sim/             Base-side simulator application (spec section 8)
lib/rimlink/         Adapter v1 legacy-spi: frames, CRC-8, identity
                     (only place protocol constants may appear)
drivers/blink/       Custom driver class template (reusable)
drivers/example_sensor/  Out-of-tree sensor driver template (reusable)
dts/bindings/        Bindings for the kept drivers
boards/              Workspace board root (common helpers; no custom boards)
tests/unit/rimlink/  Host-native ztest: CRC vectors, frame round-trip
tests/func/example_sensor/  Reference functional build (fk723m1_zgt6)
docs/                Roadmap + Phase 1..6 hardware/software specifications
scripts/             Reusable west/build helper scripts
west.yml             Zephyr v4.4.0 manifest (hal_stm32, cmsis_6, picolibc, segger)
```

## Getting started

```shell
# one-time
pip install west
west init -l .          # if .west/ not present
west update
west packages pip --install   # or: pip install -r 3rd_party/zephyr/zephyr/scripts/requirements-base.txt
```

### Build

```shell
# Rim firmware (DUT)
west build -p -b fk723m1_zgt6 app

# Base-side simulator (second board)
west build -p -b fk723m1_zgt6 app/sim

# Functional example (custom driver class + out-of-tree sensor driver)
west build -p -b fk723m1_zgt6 tests/func/example_sensor
```

### Test

```shell
# Host-native unit tests (CRC-8 reference vector A5 03 00x30 -> 0x5A, etc.)
west build -p -b native_sim/native/64 tests/unit/rimlink
./build/zephyr/zephyr.exe
# or: west twister -T tests/unit --integration
```

### Flash

```shell
west flash    # ST-Link / OpenOCD per the upstream board docs
```

## Phase 1 scope

Implemented as templates with `TODO(phase1)` markers, per
`docs/phase1-software-spec.md`:

- `lib/rimlink` — CRC-8 (implemented + ztest-verified), frame
  build/seal/validate (implemented), stats/state machine (TODO)
- `app/src/link_spi.c` — SPI slave transceive thread; CS-EXTI re-arm,
  double-buffer swap, DMA + nocache buffers (TODO)
- `app/src/input_svc.c` — 1 kHz tick skeleton; debounce, D-pad ladder,
  snapshot publish (TODO)
- `app/src/output_svc.c` — 7-seg decode (partial table), TM1637 mirror (TODO)
- `app/src/diag_svc.c` — `rim id` / `rim stats` shell; mosi/miso/btn (TODO)

## Phase 1 implementation status

Implemented (Phase 1 complete, pending hardware bring-up):

- `lib/rimlink` - adapter v1 (`legacy-spi`): verbatim reference CRC-8 table
  (verified vector `A5 03 00x30 -> 0x5A` + 200-random-vector equivalence),
  33-byte frame build/validate, 22-bit logical button map, TX double buffer
  with freshness swap, 50 ms stale-input clearing, RX validation queue with
  workqueue callback, full statistics (txn/crc/short/rearm/overrun/cs-gap).
- `app` (DUT rim firmware): SPI1 slave Mode 1 link thread with CS-sense EXTI
  re-arm and LINK_READY/err-LED indicators; 1 kHz input service (6 debounced
  buttons + analog D-pad ladder with reference thresholds and hysteresis,
  SNAPSHOT_TICK, publish-latency P99/max); output service with the verbatim
  7-segment decode table, change/CRC gating, LED/rumble evidence counters
  and optional TM1637 mirror (`CONFIG_RIM_TM1637`, `rAcE` boot banner);
  `rim` shell (mosi/miso/disp/btn/map/id/dpad/stats/reset) reproducing the
  reference serial commands.
- `app/sim` (base-side simulator): Mode-1 controller at 500 kHz / 2 ms
  cadence with software CS, MOSI CRC sealing, MISO header+CRC verification,
  fault injectors (crc / trunc / extra clocks / jitter), display-text
  encoding and per-second statistics via the `sim` shell.
- Unit tests (`native_sim/native/64`): 5 suites, 14 tests - CRC vectors and
  equivalence, frame layout/round-trip, all-22-bit map, buffer swap + stale
  rule, RX paths, 7-seg decode/encode, debounce, D-pad windows/hysteresis.

Deferred with `TODO(phase2-perf)` markers: SPI DMA + non-cacheable link
buffers, and the LL-level transfer fallback (only if measured re-arm latency
misses Gate G1, software spec section 10).
