# Code Standards

> Version: 1.0
> Reviewed: 2026-07-02
> Purpose: coding conventions for firmware and tooling implemented from this study base. Forward-looking guidance; it constrains how code *should* be written, and does not assert proprietary vendor implementation details.

## Document Change Log

| Version | Date | Changes |
|---|---|---|
| 1.0 | 2026-07-02 | Initial code standards, derived from the real-time task model in [Template.md](./Template.md), the safety model in [study/sim_racing_research.md](./study/sim_racing_research.md), and the subsystem timing in [study/wheel_base.md](./study/wheel_base.md). |

## 1. Scope

This document defines conventions for firmware (MCU C/C++) and supporting host tooling written against the architecture in this repository. It applies to the wheel base, steering rim, pedals, add-ons, accessories, and any host-side bridge. It is deliberately generic: it constrains *how* code is structured and reviewed, not *what* any proprietary Fanatec product does internally.

> [!IMPORTANT]
> These are engineering-practice conventions, not reverse-engineered facts. Where a rule references a rate or threshold, that value comes from the reference architecture in this study base and **shall** be re-confirmed against real customer requirements before production use.

## 2. Languages and Toolchain

- Firmware **shall** be written in C or C++ (freestanding profile), with C++ features limited to those that carry no hidden allocation or exception cost on the hot path.
- Dynamic allocation **shall not** occur after initialization on any real-time path (motor control, USB HID, inter-subsystem link). Pre-allocate at boot.
- Every module **should** build clean under `-Wall -Wextra -Werror` (or the toolchain equivalent) and pass a static analyzer (for example, `cppcheck` or `clang-tidy`).
- Host tooling **may** use higher-level languages (Python, C#) consistent with the ecosystem in [study/tools.md](./study/tools.md); it **shall** treat device I/O as untrusted and bounded.

## 3. Real-Time Rules

The reference task rates below are taken from [Template.md](./Template.md) and [study/wheel_base.md](./study/wheel_base.md). They set the ceiling for how long any handler may run.

| Task | Reference rate | Rule |
|---|---:|---|
| Motor control | 20 kHz | **Shall** be deterministic and interrupt/timer-driven; no logging, no allocation, no blocking calls. |
| USB HID report | 1 kHz | **Shall** complete within its frame; report assembly **should** use pre-sized buffers. |
| Inter-subsystem link (QR/base) | 500 Hz | **Shall** use bounded, length-checked frames; **shall** detect and recover from a stalled peer. |
| Pedal/analog sampling | 250 Hz | **Should** filter in fixed-point or bounded floating-point; **shall not** starve higher-rate tasks. |

- Interrupt service routines **shall** be short and **shall** hand work to a task or ring buffer rather than performing it inline.
- Shared state between an ISR and a task **shall** be protected (lock-free ring buffer, critical section, or atomic), never assumed atomic by convention.
- Watchdogs **shall** be armed on any subsystem that drives an actuator or a link; a missed deadline **shall** fail safe (see §6).

## 4. Naming and Structure

- Signal, state, and register names in code **shall** match the names used in the subsystem specs (for example, torque-arbiter states in [study/wheel_base.md](./study/wheel_base.md)), so a reviewer can trace code to document without a translation table.
- State machines **shall** name states explicitly (`STATE_BOOT`, `STATE_ARMED`, `STATE_FAULT`), and transitions **shall** be centralized, not scattered across handlers.
- One subsystem per module directory; public headers expose intent, private headers expose implementation.
- Units **shall** be explicit in identifiers or types (`torque_mNm`, `angle_millideg`), never implied.

## 5. USB and HID Conventions

- HID report descriptors **shall** follow USB-IF usages (see the HID and PID references in [study/references.md](./study/references.md)); custom usages **shall** be documented in-tree.
- Endpoint lifecycle (enumeration, suspend/resume, remote wakeup) **shall** be handled explicitly; a device **shall not** assume the host is always present.
- Vendor-specific interfaces **should** be isolated behind a compile-time flag so a base HID build remains standards-clean.

## 6. Error Handling and Safety

The safety posture is inherited from [study/sim_racing_research.md](./study/sim_racing_research.md) §3.2 and the safety rules in [study/README.md](./study/README.md).

- Every intelligent node **shall** report identity, capabilities, boot state, application state, health, and recovery status.
- Passive-sensor faults **shall** be handled: cable fault, rail out-of-bounds, range limit, and plausibility failure each **shall** drive a defined safe response.
- A fault on a torque-producing path **shall** remove torque (fail safe), **shall not** attempt to "ride through," and **shall** latch until an explicit, authenticated reset.
- Code **shall not** implement bypasses of console authentication, torque limits, safety keys, or firmware protections.

## 7. Testing and Static Analysis

- Every module **should** have unit tests for its pure logic (framing, calibration math, state transitions) runnable on the host.
- Hardware-in-the-loop checks **shall** follow the validation checklist in [study/tools.md](./study/tools.md) §5 before full-energy operation.
- Timing-critical paths **should** be measured (logic analyzer / cycle counter), not assumed to meet deadline.

## 8. Review Checklist

Before merge, a reviewer **shall** confirm: no allocation or blocking on a real-time path; deadlines measured or bounded; ISR/task sharing is protected; fault paths fail safe and latch; names trace to the subsystem spec; HID descriptors are standards-clean; and no safety-bypass logic is present.

## Unresolved Questions

- Which target MCU families and RTOS (if any) will be standardized once customer hardware is chosen? This selection will fix the concrete toolchain and the exact real-time rates above.
