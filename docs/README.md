# Documentation Index

> Version: 1.0
> Reviewed: 2026-07-02

## Document Change Log

| Version | Date | Changes |
|---|---|---|
| 1.0 | 2026-07-02 | Added version header and change log; added the five newer subsystem docs (motion, tactile, telemetry, communication-protocols, compatibility-matrix) to the Study table; added Project-overview/PDR, spec-updates, and the engineering journal to the Project table; validated relative links. |

This directory is the entry point for the Fanatec and sim-racing research base. Start with the study documents for subsystem knowledge, then use architecture and project docs for repository-level decisions.

## Study Documents

| Topic | Document | Use When |
|---|---|---|
| Study map | [study/README.md](./study/README.md) | Choosing a reading path through the sim-racing domain |
| Customer glossary | [study/glossary.md](./study/glossary.md) | Standardizing Fanatec terminology, abbreviations, compatibility language, and customer-support wording |
| Ecosystem overview | [study/sim_racing_research.md](./study/sim_racing_research.md) | Understanding host, wheel base, rim, pedals, accessories, safety, and firmware boundaries |
| Wheel base | [study/wheel_base.md](./study/wheel_base.md) | Designing or reviewing motor-control, FFB, USB, safety, and update architecture |
| Steering rim | [study/wheel_rim.md](./study/wheel_rim.md) | Studying QR electrical links, input scanning, display/LED output, and rim identity |
| Pedals | [study/pedals.md](./study/pedals.md) | Studying potentiometer, Hall, load-cell, ADC, USB, and RJ12 pedal paths |
| Add-ons | [study/add_ons.md](./study/add_ons.md) | Studying shifters and handbrakes |
| Accessories | [study/accessories.md](./study/accessories.md) | Studying quick releases, dashboards, telemetry displays, and button boxes |
| Cockpits | [study/cockpits.md](./study/cockpits.md) | Understanding mechanical mounting, rigidity, and load paths |
| Motion platforms | [study/motion.md](./study/motion.md) | Studying motion rigs, motion cueing, and the safety envelope |
| Tactile transducers | [study/tactile.md](./study/tactile.md) | Studying bass-shakers, isolation from FFB, and resonance |
| Telemetry software | [study/telemetry.md](./study/telemetry.md) | Studying the game -> bridge -> device telemetry pipeline |
| Communication protocols | [study/communication-protocols.md](./study/communication-protocols.md) | Understanding the full protocol stack: USB/HID/PID, OS input APIs, DFU, and tool-to-device paths |
| Compatibility matrix | [study/compatibility-matrix.md](./study/compatibility-matrix.md) | Separating USB-direct vs base-proxy, QR generation, and platform paths |
| Tools | [study/tools.md](./study/tools.md) | Finding standards, host tools, firmware tools, and validation equipment |
| Repositories | [study/repos.md](./study/repos.md) | Finding public open-source projects and knowing how to interpret them |

## Project Documents

| Topic | Document |
|---|---|
| System architecture | [system-architecture.md](./system-architecture.md) |
| Code standards | [code-standards.md](./code-standards.md) |
| Codebase summary | [codebase-summary.md](./codebase-summary.md) |
| Roadmap | [development-roadmap.md](./development-roadmap.md) |
| Changelog | [project-changelog.md](./project-changelog.md) |
| Specification update conventions | [spec-update.md](./spec-update.md) |
| Specification change log | [spec-updates.md](./spec-updates.md) |
| Project overview / PDR | [project-overview-pdr.md](./project-overview-pdr.md) |
| Engineering journal | [journals/260701-2159-repository-initialization.md](./journals/260701-2159-repository-initialization.md) |

## Source Confidence Rules

| Source Type | Use As | Do Not Use As |
|---|---|---|
| Standards bodies | Protocol/interface requirements | Vendor-specific implementation proof |
| Manufacturer manuals/support | Public product behavior and safety constraints | Internal schematic/firmware proof |
| Public open-source projects | Demonstrated community implementation evidence | Official vendor specification |
| Patents and academic papers | Architecture context | Product requirement without verification |
| Forum/community notes | Discovery input | Standalone technical authority |

## Unresolved Questions

- None.
