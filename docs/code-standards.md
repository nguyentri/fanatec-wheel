# Code Standards

> Version: 2.0
> Reviewed: 2026-07-04
> Purpose: coding conventions for the rim firmware and host tooling in this repository, aligned with the released source and enforced at review.

## Document Change Log

| Version | Date | Changes |
|---|---|---|
| 2.0 | 2026-07-04 | Fixed the toolchain section to the actual stack (Zephyr 4.4.0, west, C11); replaced the reference task-rate table (motor 20 kHz / USB 1 kHz — wheel-base concerns not present here) with this firmware's measured rates; aligned the review checklist with the checks actually run; removed references to documents not in this repository. |
| 1.0 | 2026-07-02 | Initial standards (reference-architecture, forward-looking). |

## 1. Scope

Conventions for the rim firmware (`lib/`, `app/`), the base simulator (`app/sim`), and host tooling (`scripts/`). They constrain *how* code is structured and reviewed; protocol behavior itself is specified in the phase specs.

## 2. Languages and Toolchain (as used)

- Firmware **is** C11 on **Zephyr 4.4.0**, built with `west` and the GNU Arm Embedded toolchain; unit tests run as ztest suites on `native_sim`.
- Dynamic allocation **shall not** occur after initialization on any real-time path. All buffers (frames, capture ring, latency ring) are static.
- Every target **shall** build with **zero warnings** — a release gate ([release.md](./release.md)), checked in CI on every push.
- Host tooling **is** Python 3.10+ (stdlib + `pyyaml`/`pyserial` where noted); it **shall** treat device I/O as untrusted and bounded.

## 3. Real-Time Rules (this firmware's rates)

| Path | Rate / bound | Rule (enforced in source) |
|---|---:|---|
| Link SPI slave (fast path) | base-driven, ≤ 12 MHz clock | **Shall** carry no logging, allocation, blocking, or flash writes; TX by double-buffer swap; RX handed to a workqueue via msgq |
| Input tick | 1 kHz, ≤ 250 µs worst case | Budget **measured** by the latency ring (`rim stats` P99/max), never assumed |
| LED rendering | ≤ 60 Hz, change-driven | Runs in workqueue; physical push only behind the devicetree backend |
| Haptic cues | ≤ 100 ms duration, ≥ 50 ms cooldown | Bounded primitives only; frame source gated |
| Settings/health commits | shell / 60 s work item | Diag context only |
| Watchdog | 100 ms, fed at 1 kHz | Fed **only** by the input thread (`rim_wdt.c`) |

- ISRs **shall** be short and hand work to a thread or queue; the CS-sense EXTI and SPI callbacks do exactly re-arm + timestamp.
- ISR/task shared state **shall** be protected (atomics, spinlocks, msgq) — see `rimlink_link.c`, `input_svc.c` for the reference patterns.

## 4. Naming and Structure

- Identifiers **shall** match the specification names (spec IDs cited in comments, e.g. `spec 3-S1`), so a reviewer traces code to document without a translation table.
- One concern per module; pure decoders (`encoder.c`, `funky.c`, `clutch.c`, `seg7.c`, `dpad.c`, `debounce.c`, the `led_render`/`lra_gate` cores) **shall** stay free of kernel dependencies beyond `util.h` so they unit-test on the host.
- Hardware absence **shall** be handled by devicetree guards (`DT_NODE_EXISTS`), not `#ifdef` board names — the same binary configuration must run on the bench and PCB rev A.
- Units **shall** be explicit in identifiers (`cs_gap_us`, `duration_ms`, `boot_ready_cyc`).

## 5. Protocol Confinement

- Frame offsets, the CRC definition, and identity values exist **only** in `lib/rimlink` (and its deliberate host mirror `scripts/capture/frame_model.py`). A service module indexing a frame byte is a review reject.
- The adapter is versioned (`v1 legacy-spi`); protocol revisions arrive as a new adapter, not edits scattered through services.

## 6. Error Handling and Safety

- The rim node **shall** report identity, configuration, session attribution, statistics, and persisted health (`rim session|stats|health`).
- Analog-sensor faults **shall** be detected and flagged: rail windows (open/short), rate plausibility, calibration guard bands (`clutch.c`).
- Fail quiet, never ride through: stale link clears momentary inputs (50 ms) and quiets outputs/rail (200 ms); watchdog starvation resets; faults latch where the spec says so (`power_mgr_fault`).
- Code **shall not** implement bypasses of authentication, safety keys, or firmware protections.

## 7. Testing and Instrumentation

- Every pure module **shall** have host ztests; behavioral modules get suite coverage where a stable seam exists (capture ring, health).
- Timing claims **shall** be measured (latency ring, `rim boot`, capture CS-gap stats, test-point GPIOs PF3/PD15), not asserted.
- Evidence tooling (capture ring, LA toolkit, soak runner) **shall** stay in-tree and CI-smoke-tested against fixtures.

## 8. Review Checklist (as executed)

Before merge, a reviewer **shall** confirm:
(a) no protocol offsets outside `lib/rimlink`; (b) no logging/allocation/blocking/flash writes on the fast path; (c) settings/health commits only from diag context; (d) the 50 ms stale rule and armed-frame immutability untouched; (e) gated features keep their default posture (rumble source off, watchdog off on bench); plus: zero warnings on all targets, all suites green, [specs/dma-irq-budget.md](./specs/dma-irq-budget.md) current for any peripheral change, and *measurement pending* markers on any unproven hardware claim.

## Unresolved Questions

- Static-analysis tooling (`clang-tidy` profile) is not yet wired into CI; adopt when the Phase 6 portability work lands.
