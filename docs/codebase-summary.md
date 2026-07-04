# Codebase Summary

> Version: 2.0
> Reviewed: 2026-07-04
> Purpose: orient a new engineer in this repository — a Zephyr west workspace containing the rim firmware, base simulator, host tooling, and tests.

## Document Change Log

| Version | Date | Changes |
|---|---|---|
| 2.0 | 2026-07-04 | Rewritten from the "documentation-only / proposed layout" state to describe the released source tree (Phases 1–5), real build/flash/test commands, and the module-to-spec traceability that actually exists. |
| 1.0 | 2026-07-02 | Initial summary (documentation-only repository, proposed layout). |

## 1. Current State

This repository is a **west workspace** with implemented firmware. Phases 1–5 of the rim roadmap are complete on the firmware side; items requiring real hardware (real-base captures, PCB rev A bring-up, 24 h soak) are scaffolded and marked *measurement pending*. See [development-roadmap.md](./development-roadmap.md) for status and [project-changelog.md](./project-changelog.md) for history.

## 2. Source Tree (as built)

```
fanatec-wheel/                 west manifest repo (west.yml)
├── lib/rimlink/               adapter v1 "legacy-spi": 33-byte frame build/
│                              parse, CRC-8, identity, TX double-buffer,
│                              RX msgq→work, stats, Phase 2 capture ring.
│                              The ONLY place protocol offsets exist.
├── app/                       rim firmware application
│   ├── src/
│   │   ├── main.c             boot order: fast path first (RIM_FASTBOOT)
│   │   ├── link_spi.c         SPI1 slave glue, CS-sense EXTI, LINK_READY
│   │   ├── input_svc.c        v2 fabric @1 kHz: buttons, D-pad ladder,
│   │   │                      encoders, funky, clutches → snapshot
│   │   ├── encoder|funky|clutch|debounce|dpad|seg7.c   pure decoders
│   │   ├── output_svc.c       validated-frame fan-out + display decode
│   │   ├── led_svc.c          15-RGB + 8-flag renderer, quiet state
│   │   ├── lra_svc.c          bounded haptic cues (source gated off)
│   │   ├── power_mgr.c        out-rail sequencing state machine
│   │   ├── rim_wdt.c          IWDG fed by input thread
│   │   ├── rim_settings.c     settings schema v2 (NVS) + config lock
│   │   ├── health.c           persisted health counters
│   │   └── diag_svc.c         `rim …` shell, session header
│   ├── boards/fk723m1_zgt6.overlay   pins, ADC channels, flash partitions
│   ├── sysbuild.conf + sysbuild/     MCUboot verified-boot configuration
│   └── prj.conf / Kconfig            RIM_FASTBOOT, RIM_WATCHDOG, RIM_TM1637…
├── app/sim/                   base simulator (second board), twin profile
│   └── profiles/base_twin.yaml → generated header; `sim …` shell
├── scripts/
│   ├── capture/               LA decode, ring diff, link stats,
│   │                          field activity, matrix renderer, fixtures
│   ├── soak/soak_runner.py    dual-console soak with thresholds
│   └── pin_registry.py        overlay → pin table (schematic cross-check)
├── tests/unit/rimlink         adapter, capture, link, health suites
├── tests/unit/app_logic       decoder + led/lra suites (pure logic)
├── tests/func/example_sensor  on-target functional example
├── plans/260704-phases2-5/    approved implementation plan + outcome
├── docs/                      this documentation set (see README.md)
└── 3rd_party/                 west-managed: zephyr v4.4.0, hal_stm32,
                               cmsis_6, picolibc, segger, mcuboot
```

## 3. Build, Flash, Test

```sh
west update                                    # fetch 3rd_party modules
west build -b fk723m1_zgt6 app                 # bench firmware (dev, 0x08000000)
west build -b fk723m1_zgt6 app --sysbuild      # MCUboot + signed app (pip install cbor2)
west build -b fk723m1_zgt6 app/sim             # base simulator (second board)
west flash                                     # on-board ST-Link
west twister -T tests/unit --integration       # host unit suites (native_sim)
```

Zero compiler warnings is a release gate ([release.md](./release.md)). All suites and targets are exercised by `.github/workflows/build.yml`.

## 4. Where Each Spec Is Implemented

| Specification | Implementing modules |
|---|---|
| [phase1-software-spec.md](./phase1-software-spec.md) (adapter, services, simulator) | `lib/rimlink`, `app/src/{link_spi,input_svc,output_svc,diag_svc}`, `app/sim` |
| [phases2-6-software-spec.md](./phases2-6-software-spec.md) 2-S1…2-S6 | `rimlink_capture.c`, `RIM_FASTBOOT` in `main.c`/`link_spi.c`, `diag_svc.c` session, `scripts/capture/`, `docs/compat/`, `app/sim/profiles/` |
| 3-S1…3-S7 | `encoder.c`, `funky.c`, `clutch.c`, `rim_settings.c`, `input_svc.c` |
| 4-S1…4-S6 | `led_svc.c`, `lra_svc.c`, `power_mgr.c`, `rim_wdt.c`, [specs/dma-irq-budget.md](./specs/dma-irq-budget.md), `scripts/pin_registry.py` |
| 5-S1…5-S6 | `app/sysbuild*`, `health.c`, `scripts/soak/`, `rim lock` path, [release.md](./release.md), CI release job |
| Consolidated summaries | [specs/steering_wheel_sw_spec.md](./specs/steering_wheel_sw_spec.md), [specs/steering_wheel_hw_spec.md](./specs/steering_wheel_hw_spec.md) |

A reviewer traces code to spec via the spec IDs cited in source comments (e.g. `spec 3-S1`) and the review checklist in [code-standards.md](./code-standards.md) §8.

## Unresolved Questions

- The Phase 6 tracks (portability bake-off, display module) have no source yet; this document gains their tree entries when they land.
