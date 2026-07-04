# Documentation Index

> Version: 2.0
> Reviewed: 2026-07-04

## Document Change Log

| Version | Date | Changes |
|---|---|---|
| 2.0 | 2026-07-04 | Corrected the index to the released repository: added the implemented-firmware docs (phase specs, consolidated specs, compat, release/update, budget), moved the study table to an external-research note (those documents live in the research base, not this firmware repo), and removed links to files not present here. |
| 1.0 | 2026-07-02 | Initial index of the research base. |

This directory documents the **implemented rim firmware** (Zephyr, FK723M1-ZGT6). Start with the codebase summary, then the specs relevant to your task.

## Project Documents

| Topic | Document |
|---|---|
| Codebase summary (start here) | [codebase-summary.md](./codebase-summary.md) |
| System architecture | [system-architecture.md](./system-architecture.md) |
| Roadmap and status | [development-roadmap.md](./development-roadmap.md) |
| Project overview / PDR | [project-overview-pdr.md](./project-overview-pdr.md) |
| Code standards + review checklist | [code-standards.md](./code-standards.md) |
| Project changelog | [project-changelog.md](./project-changelog.md) |
| Release definition | [release.md](./release.md) |
| Update & recovery (MCUboot) | [update-recovery.md](./update-recovery.md) |
| Spec-enhancement conventions | [skills/spec-update.md](./skills/spec-update.md) |

## Specifications

| Topic | Document | Authority |
|---|---|---|
| Long-form roadmap + system spec | [fanatec-wheel-roadmap-and-system-spec.md](./fanatec-wheel-roadmap-and-system-spec.md) | normative |
| Phase 1 software / hardware | [phase1-software-spec.md](./phase1-software-spec.md) · [phase1-hardware-spec.md](./phase1-hardware-spec.md) | normative |
| Phases 2–6 software / hardware | [phases2-6-software-spec.md](./phases2-6-software-spec.md) · [phases2-6-hardware-spec.md](./phases2-6-hardware-spec.md) | normative |
| Consolidated software spec | [specs/steering_wheel_sw_spec.md](./specs/steering_wheel_sw_spec.md) | summary (phase specs win) |
| Consolidated hardware spec | [specs/steering_wheel_hw_spec.md](./specs/steering_wheel_hw_spec.md) | summary |
| Pin mapping (Implemented/Planned/Reserved) | [specs/steering_wheel_pin_mapping.csv](./specs/steering_wheel_pin_mapping.csv) | implemented rows match the overlay |
| DMA / IRQ budget (living) | [specs/dma-irq-budget.md](./specs/dma-irq-budget.md) | living |
| Compatibility matrix | [compat/matrix.md](./compat/matrix.md) (rendered from [matrix.yaml](./compat/matrix.yaml)) | evidence-backed |

Board photos / renders: [specs/fk723m1_zgt6.webp](./specs/fk723m1_zgt6.webp), [specs/steering_wheel_pbc_3d.png](./specs/steering_wheel_pbc_3d.png), [specs/steering_wheel_production.png](./specs/steering_wheel_production.png).

## External Research Base

The subsystem study documents (ecosystem, wheel base, rim, pedals, add-ons, accessories, cockpits, motion, tactile, telemetry, protocols, tools, repositories, glossary) live in the **research repository**, not in this firmware tree. Their source-confidence rules carry over here:

| Source Type | Use As | Do Not Use As |
|---|---|---|
| Standards bodies | Protocol/interface requirements | Vendor-specific implementation proof |
| Manufacturer manuals/support | Public product behavior and safety constraints | Internal schematic/firmware proof |
| Public open-source projects | Demonstrated community implementation evidence | Official vendor specification |
| Patents and academic papers | Architecture context | Product requirement without verification |
| Forum/community notes | Discovery input | Standalone technical authority |

## Unresolved Questions

- None.
