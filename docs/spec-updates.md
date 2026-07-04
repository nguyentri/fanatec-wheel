# Specification Update Log

> Version: 1.0
> Reviewed: 2026-07-02
> Maintained per [spec-update.md](./spec-update.md) §10. This file aggregates the change categories applied to every document during each enhancement/review pass. Per-document detail also lives in each document's own Document Change Log; project-level milestones live in [project-changelog.md](./project-changelog.md).

## Document Change Log

| Version | Date | Changes |
|---|---|---|
| 1.0 | 2026-07-02 | Recreated this aggregate log (it was referenced by project-changelog.md, codebase-summary.md, and spec-update.md §10 but was absent from the reviewed archive) and recorded the 2026-07-02 re-review pass. |

## 2026-07-02 — Re-Review Pass

A full re-review of the uploaded `docs.zip`. The technical content was already mature and accurate; this pass fixed structural and consistency defects only. No requirement, signal name, numeric value, or stated threshold was altered.

### Environment note (reachability)

Vendor and standards links (help/www/assets.fanatec.com, usb.org, Microsoft, Apple) were **not reachable** and are verified by citation only. Community repositories on github.com / raw.githubusercontent.com are reachable and previously verified.

### Structural fixes

| Finding | Resolution |
|---|---|
| `docs/compatibility-matrix.md` duplicated `docs/study/compatibility-matrix.md` byte-for-byte, and its relative links (to accessories/pedals/repos/tools/wheel_rim) were all broken because those files live under `study/`. | Removed the stray top-level duplicate; the canonical copy is `docs/study/compatibility-matrix.md`. |
| `spec-updates.md` was referenced by `project-changelog.md`, `codebase-summary.md`, and `spec-update.md` §10 but absent. | Recreated this file. |
| `spec-update.md` §10 carried a stale legacy reference to `docs/CN0XX-1_OBC_DCDC_Specification.md`. | Generalized to "a specification document under `docs/`". |
| `communication-protocols.md` existed but was not linked from either index. | Added to the README study table and the study reading path + dependency map. |

### Index wiring

| Document | Version (before → after) | Change |
|---|---|---|
| README.md | (none) → 1.0 | Added version + change log; added five subsystem docs (motion, tactile, telemetry, communication-protocols, compatibility-matrix) to the Study table; added spec-updates, project-overview-pdr, and the engineering journal to the Project table. |
| study/README.md | (none) → 1.1 | Added version + change log; added five subsystem docs to the reading path and dependency map; captioned the dependency map (Figure 1-1). |

### Normalization applied

- Version header + Document Change Log added to documents that lacked them: accessories, cockpits, pedals, references, repos, tools, wheel_rim, glossary, system-architecture, project-overview-pdr, and both knowledge bases (VI localized). Added change logs and version bumps to add_ons (1.0.0 → 1.1.0), Template (0.1 → 0.2), and spec-update (→ 1.1).
- Line endings normalized to LF and trailing whitespace stripped across the set (CRLF was present in spec-update.md and add_ons.md; trailing whitespace on eight study files).
- Figure captions added to previously-uncaptioned diagrams in system-architecture.md, Template.md (Figure 1-1, 2-1), and study/README.md (Figure 1-1).

### Deferred (unchanged by design)

- The two knowledge bases received light treatment: version/change-log and line-ending normalization only. Their inline teaching diagrams remain intentionally uncaptioned.
- The engineering journal under `journals/` is a dated log entry and is left unversioned by design.

## Unresolved Questions

- If the project intends to drop the aggregate-log convention in favor of `project-changelog.md` alone, remove the `spec-updates.md` references in spec-update.md §10, project-changelog.md, and codebase-summary.md instead of maintaining this file.
