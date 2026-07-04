# Project Changelog

> Version: 1.0
> Reviewed: 2026-07-02
> Purpose: human-facing history of the project as a whole. This is distinct from each document's own Document Change Log and from the enhancement-conventions aggregate in [spec-updates.md](./spec-updates.md).

## Document Change Log

| Version | Date | Changes |
|---|---|---|
| 1.0 | 2026-07-02 | Created the project changelog and recorded the documentation baseline. |

## Scope

This file records project-level milestones (documentation baselines, source milestones, releases) in reverse-chronological order. For line-by-line document edits, see each document's change log; for the cross-document record of formatting/convention passes, see [spec-updates.md](./spec-updates.md).

## Versioning Scheme

The project uses date-based entries. Once source is added, semantic versions (`MAJOR.MINOR.PATCH`) **should** be adopted for firmware/tooling releases and referenced here alongside the date.

## [Unreleased]

- Implementation has not started; the repository is documentation-only. See [codebase-summary.md](./codebase-summary.md) §1.
- Planned next steps are sequenced in [development-roadmap.md](./development-roadmap.md).

## 2026-07-02 — Documentation baseline

- Established the two-level `docs/` + `docs/study/` layout and reconstructed the Study Index after an upload filename collision.
- Brought every document onto a uniform versioning, change-log, and formatting baseline per [spec-update.md](./spec-update.md).
- Verified all thirteen cited community repositories against their upstream READMEs.
- Added the four project documents (`code-standards`, `codebase-summary`, `development-roadmap`, this file) and four subsystem documents (`telemetry`, `motion`, `tactile`, `compatibility-matrix`) that the set previously referenced or flagged as future work.
- Full detail of the enhancement pass is in [spec-updates.md](./spec-updates.md).

## Unresolved Questions

- None. This file will gain a new dated entry at each project milestone.
