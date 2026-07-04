# Project Overview and Product Requirements

> Version: 2.0
> Reviewed: 2026-07-04

## Document Change Log

| Version | Date | Changes |
|---|---|---|
| 2.0 | 2026-07-04 | Updated from research-repository framing to the implemented firmware project; acceptance criteria mapped to their realized mechanisms. |
| 1.0 | 2026-07-02 | Initial overview (research consolidation). |

## Overview

This repository implements clean-room firmware for an advanced Fanatec-compatible steering wheel rim on Zephyr RTOS (FK723M1-ZGT6 / STM32H723ZGT6), together with a base simulator, host evidence tooling, and tests. It grew out of a public-knowledge research base and keeps that provenance discipline: protocol behavior is derived from public community implementations and bench evidence, never proprietary assets.

## Objectives

- Implement the rim as a well-behaved SPI peripheral honoring the legacy 33-byte link, with the full GT input fabric and locally rendered outputs.
- Keep every observed protocol constant confined to one versioned adapter (`lib/rimlink`) so higher layers survive protocol revisions.
- Produce auditable evidence for compatibility claims: capture ring, session attribution, host toolkit, and the compatibility matrix.
- Ship with product-grade hardening: verified boot, safe update, health telemetry, and a defined release process.

## Requirements (realized mechanisms)

- Research and code **shall** distinguish verified facts from inference → spec IDs in source comments; *measurement pending* markers on unproven claims; compat matrix rows cite capture references.
- The link fast path **shall** carry no logging, allocation, blocking, or flash writes → review-gated invariant ([code-standards.md](./code-standards.md) §8).
- Outputs **shall** fail quiet: stale link (> 50 ms momentary clear, > 200 ms output quiet/rail-off) defaults to the safe state.
- Implementations **shall** avoid proprietary code and confidential data, and **shall not** bypass authentication or safety mechanisms.
- Configuration **shall** persist in versioned, migratable storage with a field write-lock.

## Non-Goals

- Redistributing vendor firmware or proprietary assets.
- Claiming official compatibility, certification, or vendor endorsement.
- Driving force feedback or any torque path — the rim is an input/display peripheral; torque belongs to the base.

## Acceptance Criteria (status)

- A new contributor can locate everything from [README.md](./README.md) → maintained index. ✅
- Code has an explicit architecture, test strategy, and safety boundary → phase specs + 12 unit suites + review checklist. ✅
- Compatibility claims are evidence-backed → toolkit + matrix pipeline in place; first real-base row **pending hardware**.
- A failed update cannot brick the device → MCUboot dual-slot with revert; power-cut torture test **pending hardware**.

## References

- [Documentation index](./README.md) · [Development roadmap](./development-roadmap.md) · [Codebase summary](./codebase-summary.md)
