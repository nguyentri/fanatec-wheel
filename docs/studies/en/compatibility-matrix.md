# Compatibility Matrix

> Version: 1.0
> Reviewed: 2026-07-02
> Purpose: provide the structure of a pedal/wheel/QR compatibility matrix, as requested in [pedals.md](./pedals.md) §8.3 and [repos.md](./repos.md). It separates the axes of compatibility and records verification status per cell; it does **not** assert unverified product-by-product claims.

## Document Change Log

| Version | Date | Changes |
|---|---|---|
| 1.0 | 2026-07-02 | New document. Framework created per the pedal-matrix follow-up in [pedals.md](./pedals.md) §8.3 and the "promote repos into a formal compatibility matrix after bench verification" question in [repos.md](./repos.md). Cells populated only where this study base already establishes a fact; all others marked for verification. |

## 1. Purpose and Method

> [!IMPORTANT]
> Compatibility claims are exactly the kind of fact this study base declines to assert without evidence. This document is a **framework**: every entry carries a status of **Verified public**, **Community-reported**, or **Verify** (not yet established here). Populate "Verify" cells only after confirming against an official source or a bench test.

The value of a matrix is separating axes that are often conflated. This document defines those axes and populates only what the set already supports.

## 2. Axes of Compatibility

| Axis | Question it answers |
|---|---|
| Connection path | Direct USB HID, or through a wheelbase port (proxy)? |
| QR generation | QR1 vs QR2; Base-Side vs Wheel-Side variant. |
| Platform/console | PC, Xbox, PlayStation — and where the license/ownership lives. |
| Device generation | Does the peripheral or protocol depend on a base generation? |

## 3. Connection Path (USB-direct vs Base-Proxy)

This is the separation [pedals.md](./pedals.md) §8.3 explicitly asks for.

| Path | Behavior | Status |
|---|---|---|
| Direct USB HID | Pedals/peripheral enumerate as their own HID device to the host. | Verified public (see [pedals.md](./pedals.md)) |
| Wheelbase-port proxy | Pedals/peripheral connect to a base port (e.g. RJ12); the base aggregates and reports them. | Verified public (see [pedals.md](./pedals.md)) |
| Console via base | Console support is generally reached *through* a compatible base rather than direct USB. | Community-reported / Verify against official platform docs |

The Fanatec USB vendor ID observed by the Linux driver `hid-fanatecff` is `0x0EB7` (see [repos.md](./repos.md)) — useful when identifying a direct-USB device, and labeled a driver observation rather than an official statement.

## 4. QR Generation

Grounded in the QR2 conversion guidance cited in [accessories.md](./accessories.md) and the QR treatment in [wheel_rim.md](./wheel_rim.md).

| Aspect | Note | Status |
|---|---|---|
| QR1 vs QR2 | Distinct generations; not all products convert, and conversions can be model-specific. | Verified public (Fanatec QR2 conversion guidance) |
| Base-Side vs Wheel-Side | QR2 has Base-Side and Wheel-Side variants that must be matched. | Verified public (same source) |
| Torque suitability | Higher DD torque raises mechanical demands on the coupling. | Engineering inference (see [accessories.md](./accessories.md)) |
| Specific model conversion list | Which exact products convert to QR2. | Verify against the live Fanatec conversion article |

## 5. Device / Protocol Generation Boundary

The clearest established boundary in this study base, from [wheel_rim.md](./wheel_rim.md) and the `Fanatec-Wheel-Barebone-Emulator` README (verified):

| Case | Reported outcome | Status |
|---|---|---|
| Community AVR SPI rim emulators on older bases (through CSL DD, DD1/DD2) | Reported working | Community-reported |
| Community AVR SPI rim emulators on ClubSport DD / DD+ | Reported **not** working | Community-reported (upstream emulator warning) |

> [!NOTE]
> This is a correctness-critical caveat: it is a generation boundary, not a timing tweak. Any rim/base integration plan **shall** account for it and **shall** verify on the actual target base.

## 6. Platform / Console Licensing

Grounded in the platform-licensing entries in [tools.md](./tools.md); all specific ownership claims are marked for verification because the vendor pages were not reachable in this review environment (see the review report's reachability note).

| Aspect | Note | Status |
|---|---|---|
| PC | Broadest compatibility path. | Verified public |
| Xbox | Xbox support is generally tied to a licensed wheel/hub. | Verify against Fanatec platform page |
| PlayStation | PlayStation support is generally tied to a licensed base. | Verify against Fanatec platform page |

## 7. Promoting Community Repos into This Matrix

[repos.md](./repos.md) asks which repositories should be promoted into a formal compatibility matrix after bench verification. The rule: a repository's claimed device support moves from **Community-reported** to **Verified** in this matrix only after it is reproduced on the bench per [tools.md](./tools.md) §5. Until then it stays labeled community-reported, with a link to the source.

## 8. How to Extend

Add a row only with an explicit status. New "Verify" rows are welcome as a to-do list, but a cell **shall not** be marked Verified without either an official source or a reproduced bench result recorded alongside it.

## Unresolved Questions

- Which specific products, QR conversions, and community projects can be moved from "Verify" / "Community-reported" to "Verified" after bench testing and confirmation against live official sources?
