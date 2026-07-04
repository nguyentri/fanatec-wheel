# Project Overview and Product Requirements

## Overview

This repository consolidates public technical knowledge about Fanatec-compatible sim-racing hardware and software. It supports architecture study, interoperability research, and future clean-room prototypes.

## Objectives

- Map the sim-racing product ecosystem and component boundaries.
- Document known interfaces, protocols, timing, and safety concerns.
- Catalogue useful open-source projects and authoritative references.
- Provide an auditable foundation for future firmware or tooling work.

## Requirements

- Research shall distinguish verified facts from inference.
- Externally verifiable product claims shall cite public sources.
- Existing research shall remain accessible through the documentation index.
- Future implementations shall avoid proprietary code and confidential data.
- Safety-critical motor-control proposals shall default to torque-disabled behavior on invalid or stale input.

## Non-Goals

- Redistributing vendor firmware or proprietary assets.
- Claiming official compatibility, certification, or vendor endorsement.
- Treating community findings as guaranteed behavior across all product generations.

## Acceptance Criteria

- A new contributor can locate product, architecture, and repository research from `README.md`.
- Documentation states provenance and uncertainty for material technical claims.
- Future code has an explicit architecture, test strategy, and safety boundary before hardware activation.

## References

- [Documentation index](./README.md)
- [Development roadmap](./development-roadmap.md)

