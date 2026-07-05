# Fanatec & Sim Racing Open-Source Repositories

This document is a curated discovery map for public projects relevant to Fanatec-compatible and sim-racing hardware research. These repositories are useful for architecture lessons and interoperability hypotheses. They are not official Fanatec specifications unless the repository is owned by the vendor.

## How To Use These Repositories

| Evidence Type | What It Can Support | Required Caution |
|---|---|---|
| README/project docs | Project goals, supported hardware, build method | May be stale or incomplete |
| Source code | Observed framing, timing, descriptors, and hardware assumptions | May target only one device generation |
| Schematics/pinouts | Community electrical hypotheses | Must be verified against approved docs or measured safely |
| Issues/discussions | Failure modes and compatibility reports | Treat as anecdotal until reproduced |
| Releases/binaries | Reproducible build/deployment path | Do not redistribute proprietary firmware or unknown binaries |

## Developer Reading Order

1. Read [sim_racing_research.md](./sim_racing_research.md) for subsystem boundaries.
2. Read [wheel_rim.md](./wheel_rim.md) before wheel-emulator projects.
3. Read [pedals.md](./pedals.md) before pedal-controller or pedal-proxy projects.
4. Read [add_ons.md](./add_ons.md) before shifter/handbrake projects.
5. Use [tools.md](./tools.md) to validate USB, HID, timing, and electrical assumptions.

## Wheel Emulators & DIY Steering Wheels

* **[lshachar/Arduino_Fanatec_Wheel](https://github.com/lshachar/Arduino_Fanatec_Wheel)**
  A do-it-yourself project allowing you to build a custom steering wheel that communicates with Fanatec wheel bases over SPI. It supports buttons, D-pads, and alphanumeric displays, spoofing the wheelbase into enabling Force Feedback.

* **[StuyoP/Fanatec-Wheel-Barebone-Emulator](https://github.com/StuyoP/Fanatec-Wheel-Barebone-Emulator)**
  A barebone emulator for Fanatec wheelbases utilizing an ATmega328p chip natively running at 3.3V. It allows you to create DIY steering wheels with full button, display, and LED support without the need for logic level shifters.

* **[Alexbox364/F_Interface_AL](https://github.com/Alexbox364/F_Interface_AL)**
  A versatile hardware and software platform for building DIY custom steering wheels. It communicates directly with Fanatec wheelbases via the SPI protocol and can support up to 24 push buttons, rotary encoders, TM1637 displays, and OLED screens.

## Pedal Emulators & Controller Replacements

* **[jssting/ArduinoTec-Pedals](https://github.com/jssting/ArduinoTec-Pedals)**
  An Arduino Leonardo/Pro Micro project meant to replace the OEM controller board of Fanatec ClubSport Pedals (CSP) V1/V2. Perfect for repairing older pedals when the main PCB breaks, allowing you to interface the original load cells and Hall effect sensors via standard USB.

* **[GeekyDeaks/fanatec-pedal-emulator](https://github.com/GeekyDeaks/fanatec-pedal-emulator)**
  A tool that allows you to take third-party USB pedals (like Heusinkveld) and proxy them through a Fanatec wheelbase via the RJ12 port, making them usable on consoles like PS4/PS5 or Xbox.

* **[adamcrawley/fanatec-pedal-emulator-vrs](https://github.com/adamcrawley/fanatec-pedal-emulator-vrs)**
  A modified version of the emulator above, specifically geared toward making VRS DirectForce Pro Pedals compatible with a Fanatec wheelbase.

## Shifters, Motion & Force Feedback

* **[StuyoP/Universal-Shifter-Interface-for-Fanatec](https://github.com/StuyoP/Universal-Shifter-Interface-for-Fanatec)**
  An interface that allows you to connect any switch-based H-pattern or sequential shifter directly to a Fanatec wheelbase using its internal RJ12 protocol.

* **[Ultrawipf/OpenFFBoard](https://github.com/Ultrawipf/OpenFFBoard)**
  An overarching universal open-source force feedback interface platform for highly compatible DIY simulation devices, particularly direct-drive steering wheels. It supports various motor drivers (like TMC4671, ODrive, VESC) and encoders.

* **[vnmsimulation/VNM_MOTION_CONTROLLER](https://github.com/vnmsimulation/VNM_MOTION_CONTROLLER)**
  STM32F401RCT-based firmware and configurator app from VNM Simulation to build DIY hardware including wheelbases, pedals, and motion rigs.

## Hardware Reference & Pinouts

* **[FendtXerion3800/Fanatec-Pinout](https://github.com/FendtXerion3800/Fanatec-Pinout)**
  A useful hardware reference repository documenting the pinouts for Fanatec equipment (such as RJ12 sockets for pedals, shifters, and wheel quick releases). This is crucial for anyone building DIY conversion cables or custom adapters.

## Drivers & Software Interfaces

* **[gotzl/hid-fanatecff](https://github.com/gotzl/hid-fanatecff)**
  A highly regarded Linux kernel driver module providing full force feedback (FFB) support for Fanatec wheel bases. Essential for anyone wanting to sim race on Linux or SteamOS platforms.

## General Repository Searches

If you want to keep exploring the latest community creations across GitHub and GitLab, here are a few direct search queries:
* [GitHub Search: "fanatec"](https://github.com/search?q=%20fanatec&type=repositories)
* [GitHub Search: "racing wheel base"](https://github.com/search?q=racing+wheel+base+&type=repositories)
* [GitLab Search: "fanatec"](https://gitlab.com/search?search=fanatec)
* [GitLab Search: "sim racing"](https://gitlab.com/search?search=sim+racing)

## Reference Anchors

- [USB-IF HID specifications and tools](https://www.usb.org/hid) — baseline for USB HID reports and usages.
- [USB-IF PID Class 1.0](https://www.usb.org/sites/default/files/documents/pid1_01.pdf) — baseline for HID force-feedback/PID model.
- [Fanatec Podium DD1 manual](https://assets.fanatec.com/fanatec-pwa/image/upload/downloads-prod/pdfs/P-WB-DD1-Manual-EN_web.pdf) — public manufacturer behavior for one high-end base family.
- [SimHub wiki](https://github.com/SHWotever/SimHub/wiki) — public telemetry/dashboard/button-box ecosystem.
- [OpenFFBoard wiki](https://github.com/Ultrawipf/OpenFFBoard/wiki/) — public modular FFB wheelbase ecosystem.

## Open Questions for Developers to Self-Investigate

Reviewed 2026-07-05. Process question — the answer is produced by the reader's verification work, not by lookup.

- **Which repositories should be promoted into a formal compatibility matrix after bench verification?**
  *How to investigate:* treat every repo here as **discovery input / community evidence**, never as an official spec. Promote a project into [`compatibility-matrix.md`](./compatibility-matrix.md) only after (a) confirming the exact hardware/firmware it was validated against, (b) reproducing its behavior on the bench, and (c) recording the result with date and versions. Prioritize projects that already publish concrete, checkable details — e.g. `gotzl/hid-fanatecff` (device USB IDs, extended controls) and `GeekyDeaks/fanatec-pedal-emulator` (RJ12/UART pinout) — since these are the easiest to verify. Re-verify on firmware or product changes.
