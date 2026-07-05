# Sim Racing Developer Tools and References

This document lists practical tools and reference sources for developers studying or prototyping sim-racing peripherals. It is a starting point, not a product approval list.

## 1. Standards and Interface References

| Need | Reference | Use |
|---|---|---|
| USB input devices | [USB-IF HID specifications and tools](https://www.usb.org/hid) | HID descriptors, usages, report model, descriptor authoring tools. |
| Force feedback devices | [USB-IF PID Class 1.0](https://www.usb.org/sites/default/files/documents/pid1_01.pdf) | Physical Interface Device reports for force-feedback wheels and haptic devices. |
| Linux input and FF APIs | [Linux force-feedback documentation](https://www.kernel.org/doc/html/latest/input/ff.html) | Host-side effect upload/playback concepts. |
| HIDRAW access | [Linux HIDRAW documentation](https://docs.kernel.org/hid/hidraw.html) | Direct HID descriptor/report access for tools and compatibility layers. |

## 2. Ecosystem and Compatibility References

| Need | Reference | Use |
|---|---|---|
| Source confidence and dates | [Fanatec ecosystem source register](./references.md) | Check whether a claim is official, current community context, or stale buyer-guide material. |
| Customer terminology | [Fanatec customer glossary](./glossary.md) | Use consistent component, platform, QR, tuning, and troubleshooting language. |
| Current wheel-base tiers | [Fanatec Wheel Bases FAQ](https://help.fanatec.com/hc/en-us/articles/43766204938257-Wheel-Bases-A-FAQ) | Verify current CSL, ClubSport, and Podium positioning and connection constraints. |
| Platform licensing | [Fanatec platform compatibility](https://www.fanatec.com/us-en/platforms) | Verify Xbox wheel/hub and PlayStation base ownership. |
| QR generations | [Fanatec QR2 conversion guidance](https://help.fanatec.com/hc/en-us/articles/30011253510289-Which-products-can-be-converted-to-QR2) | Check Base-Side/Wheel-Side generation, upgrade path, and model-specific restrictions. |

## 3. Public Sim-Racing Software

| Tool | Source | Developer Use |
|---|---|---|
| OpenFFBoard | [OpenFFBoard wiki](https://github.com/Ultrawipf/OpenFFBoard/wiki/) | Study modular FFB firmware concepts, motor drivers, encoders, and HID/PID integration. |
| hid-fanatecff | [gotzl/hid-fanatecff](https://github.com/gotzl/hid-fanatecff) | Study Linux-side Fanatec HID/FFB translation, device IDs, LED/display sysfs separation, and HIDRAW behavior. |
| hid-fanatecff-tools | [gotzl/hid-fanatecff-tools](https://github.com/gotzl/hid-fanatecff-tools) | Study game-telemetry bridge patterns for LEDs, displays, and tuning values. |
| SimHub | [SimHub wiki](https://github.com/SHWotever/SimHub/wiki) | Study telemetry dashboards, Arduino displays, LEDs, button boxes, and serial-device integration. |

## 4. Firmware and Hardware Bring-Up Tools

| Tool Class | Examples | Use |
|---|---|---|
| Logic analyzer | Saleae-class analyzer, sigrok/PulseView | Validate SPI/UART/CAN timing, QR transaction boundaries, and boot-to-response deadlines. |
| Oscilloscope | 2+ channel DSO | Verify rails, reset, PWM timing, encoder signals, current-sense timing, and QR backfeed. |
| USB analyzer/software | Wireshark USBPcap, Linux `usbmon`, hid-tools | Inspect enumeration, descriptors, reports, and host/device timing. |
| Firmware debug | SWD/JTAG, RTT, semihosting disabled in real-time paths | Debug startup, state machines, and diagnostics without disturbing critical link timing. |
| HIL fixtures | Protocol simulator, current-limited power fixture, dummy motor/load | Verify fault handling before commercial hardware or full-energy operation. |

## 5. Validation Checklist by Subsystem

| Subsystem | Minimum Tooling |
|---|---|
| Wheel base | Oscilloscope, logic analyzer, current-limited supply, USB trace, E-stop/fault injection fixture. |
| Steering rim | Logic analyzer, QR pinout fixture, rail/backfeed test, input bounce test, display/LED stress test. |
| Pedals | DMM, ADC capture, known weights/force fixture, USB HID trace, calibration persistence test. |
| Shifter/handbrake | GPIO/ADC capture, debounce timing trace, impossible-state injection. |
| Dashboard/button box | USB/serial trace, SimHub profile, display refresh stress test. |
| Cockpit | Deflection measurement under wheel torque and brake load, fastener torque audit, resonance/tactile-transducer isolation check. |

## 6. Source Handling Rules

- Use official standards for protocol claims.
- Use manufacturer manuals for public setup, update, safety, and connector behavior.
- Use GitHub/community projects for demonstrated public implementations only.
- Record generation boundaries. A legacy Fanatec SPI rim emulator is not proof for ClubSport DD/DD+ behavior.
- Link the exact source used; do not cite a repository search result as evidence for a specific claim.
- Date community buyer guides. Recheck torque, availability, QR, platform, and firmware claims against current manufacturer support before reuse.

## Question Register (Resolved and Open)

Reviewed 2026-07-05.

### Resolved

- **Which USB/HID descriptor inspection tool should be standardized once source code exists?**
  **Engineering recommendation (verified public tools).** Standardize on a small, cross-platform set rather than one tool: for descriptor decoding use the **USB-IF HID Descriptor Tool** or an online HID report-descriptor decoder; for live capture/decode use **Wireshark with USBPcap** (Windows) or **usbmon** (Linux); for raw HID read/write and quick FFB report testing use **hidapitester** or Linux `usbhid-dump` + `evtest`; and validate axis/button/FFB behavior with the OS joystick/HID test panel. On Linux specifically, the `hid-fanatecff` driver plus `evtest`/`fftest` is the practical FFB-verification path. Pick the descriptor decoder as the primary standard and keep the capture/raw-HID tools as the supporting toolchain.
