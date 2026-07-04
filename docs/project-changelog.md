# Project Changelog

> Version: 2.0
> Reviewed: 2026-07-04
> Purpose: human-facing history of the project as a whole, reverse-chronological. For line-by-line document edits, see each document's own change log.

## Document Change Log

| Version | Date | Changes |
|---|---|---|
| 2.0 | 2026-07-04 | Recorded the implementation milestones (Phase 1 baseline, Phases 2–5 delivery) and this documentation restructure; adopted semantic firmware versions per the release definition. |
| 1.0 | 2026-07-02 | Created the project changelog and recorded the documentation baseline. |

## Versioning Scheme

Date-based project entries; firmware releases use semantic versions (`MAJOR.MINOR.PATCH`) from the `VERSION` file, tagged `vX.Y.Z` per [release.md](./release.md).

## [Unreleased]

- Phase 6 tracks (portability bake-off, display module) — not started.
- Open hardware gates: real-base captures, full-fabric P99, 24 h soak, interrupted-update test, PCB rev A ([development-roadmap.md](./development-roadmap.md) §3).

## 2026-07-04 — Documentation set corrected to released source

- Restructured `docs/`: consolidated hardware/software specs, pin mapping, and the DMA/IRQ budget under `specs/`; spec-enhancement conventions under `skills/`.
- Corrected the pin mapping to the released overlay (clutch ADC PA0/PA1, D-pad ladder PA3, encoder/funky/button pins), moved planned I2C1 to valid pins (PB8/PB9), and flagged the BTN1/PF10 QSPI-CLK caveat.
- Corrected the software spec: IWDG watchdog deviation, LRA source disabled by default, devicetree-guarded LED backend; added the implemented settings, instrumentation, verified-boot, and health features.
- Rewrote the codebase summary and roadmap from "documentation-only / proposed" to the actual tree and phase status.

## 2026-07-04 — Phases 2–5 firmware delivered (git `c6974ea`)

- Phase 2: 256-entry link capture ring with freeze-on-anomaly, `CONFIG_RIM_FASTBOOT` boot ordering + boot-to-ready measurement, session attribution header, host capture toolkit with fixtures, compatibility-matrix pipeline, simulator base-twin profile (≤ 12 MHz, flush emulation).
- Phase 3: input fabric v2 — quadrature transition-table decoders ×4, funky switch with chord-fault, Hall clutch pipeline, dual-clutch modes, NVS settings schema v2, frame axis/encoder enablement.
- Phase 4: LED renderer (15 RGB + 8 flags, quiet state), bounded haptic cue service (rumble source disabled pending captures), output-rail power manager, IWDG watchdog, DMA/IRQ budget, pin-registry generator.
- Phase 5: MCUboot verified boot (ED25519/tinycrypt sysbuild, dual 256 KiB slots), persisted health counters, soak automation, config lock, release definition + CI release job.
- Verification: 28 host unit tests across 12 suites green; five build targets at zero warnings.

## 2026-07-04 — Phase 1 baseline (git `d755ac7`)

- `rimlink` legacy-SPI adapter (frame build/parse, CRC-8 verified against the reference sketch), SPI1 slave link with CS-sense re-arm and LINK_READY, input/output/diagnostic services, TM1637 display option, base simulator with fault injection, 14 unit tests, CI workflow.

## 2026-07-02 — Documentation baseline

- Established the documentation layout, uniform versioning/change-log conventions, and the study/reference base that seeded the specifications.

## Unresolved Questions

- None. This file gains a dated entry at each milestone.
