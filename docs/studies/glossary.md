# Fanatec Sim-Racing Customer Glossary

> Research date: 2026-07-02
> Audience: customer support, sales, developers, technical writers, and new sim racers
> Scope: current and commonly encountered Fanatec product, compatibility, setup, tuning, peripheral, and troubleshooting language

This glossary standardizes words used when discussing Fanatec products with customers. It is unofficial and does not replace the product page, Quick Guide, manual, or Fanatec Support instructions for a specific product and firmware version.

## Contents

- [How to Use This Glossary](#how-to-use-this-glossary)
- [Customer Intake Vocabulary](#customer-intake-vocabulary)
- [Brand, Product, and Bundle Terms](#brand-product-and-bundle-terms)
- [Fanatec DD Wheel-Base Model Families](#fanatec-dd-wheel-base-model-families)
- [Platform and Compatibility Terms](#platform-and-compatibility-terms)
- [Wheel Base, Torque, and Mounting Terms](#wheel-base-torque-and-mounting-terms)
- [Steering Wheel and Control Terms](#steering-wheel-and-control-terms)
- [Pedal, Shifter, and Handbrake Terms](#pedal-shifter-and-handbrake-terms)
- [Software, Firmware, and Setup Terms](#software-firmware-and-setup-terms)
- [Tuning Menu Abbreviations](#tuning-menu-abbreviations)
- [General FFB and Sim-Racing Terms](#general-ffb-and-sim-racing-terms)
- [Troubleshooting Terms](#troubleshooting-terms)
- [Abbreviation Quick Index](#abbreviation-quick-index)
- [References](#references)

## How to Use This Glossary

### Evidence labels

| Label | Meaning |
|---|---|
| Official | Fanatec product, support, Explorer, or manual language. |
| Standard | General USB, control-system, electrical, or motorsport usage. |
| Common | Widely used sim-racing language; exact implementation can vary by product or game. |
| Legacy | Older Fanatec product/software language that customers may still use. |

Current-product claims age quickly. The [ecosystem source register](./references.md) records review dates and known conflicts. In particular, a January 2026 community guide predates the current ClubSport DD/DD+ torque update and Podium DD flagship positioning.

### Communication rules

- Say **Fanatec**, not “Fantec.”
- Ask for the exact product name, platform, connection path, Fanatec App version, and firmware versions before diagnosing.
- Separate **steering wheel** from **wheel base**. Customers often call either one “the wheel.”
- Say **FFB strength** for a setting and **torque in Nm** for physical output. They are related but not interchangeable.
- Do not promise compatibility from connector shape alone. Product, platform, firmware, quick-release generation, and connection path all matter.
- Treat “Xbox Ready” and “PlayStation Ready” as conditional labels, not the same as licensed compatibility.
- Verify current product-specific QR2 torque support. QR2 Lite behavior has changed for selected wheels through firmware updates. [F4]

## Customer Intake Vocabulary

Collect these facts first. This prevents most terminology-driven support errors.

| Ask Customer For | What It Means | Useful Example |
|---|---|---|
| Platform | Host running the game | Windows PC, Xbox Series X\|S, PS5 |
| Wheel base | Motor and USB hub unit | CSL DD, ClubSport DD+, Podium DD1 |
| Steering wheel or hub | Detachable control assembly | Formula V2.5X, CSL Universal Hub V2 |
| Quick release | Mechanical/electrical wheel-to-base coupling | QR1, QR2 Lite, QR2 Pro |
| Peripherals | Pedals, shifter, handbrake, or static paddles | CSL Pedals, ClubSport Shifter SQ V1.5 |
| Connection path | How each peripheral reaches the host | Pedals to base by RJ12; base to PC by USB |
| Software versions | Host package and device firmware | Fanatec App plus base/wheel firmware versions |
| Mode | Current platform or compatibility mode | PC mode, Xbox mode, compatibility mode |
| Symptom | Observable failure, not assumed cause | “Base is detected but there is no FFB” |
| Trigger | Action after which issue began | Firmware update, QR swap, game update |

## Brand, Product, and Bundle Terms

| Term | Definition and Customer Explanation | Evidence |
|---|---|---|
| Fanatec | Sim-racing hardware brand. Use the exact capitalization. | Official |
| Ecosystem | Interoperable set of bases, wheels, pedals, shifters, handbrakes, quick releases, software, and mounting accessories. It does not mean every combination supports every platform or feature. | Official [F1] |
| CSL | CSL stands for ClubSport Light. Fanatec product tier classified as the entry-level series. It is a family name, not a connector or compatibility standard. | Official [F19] |
| CSL Elite | Product-family name used for selected wheels and pedals. “Elite” does not define platform compatibility. | Official |
| ClubSport / CS | Mid-to-high product tier and common abbreviated prefix. Products such as ClubSport DD and ClubSport Shifter remain distinct. | Official |
| Podium / P | Premium product tier used for high-performance bases, hubs, modules, and accessories. | Official |
| Ready2Race / R2R | Bundle intended to contain the major components needed to start driving. Confirm platform support, mounting, and optional upgrades on its page. | Official |
| Racing Wheel | May mean a complete base-and-wheel package. Ask whether the customer means the system or only the detachable steering wheel. | Official/Common |
| Wheel Base | Fixed unit containing the steering motor, shaft, control electronics, USB interface, and peripheral ports. It generates FFB. | Official |
| Steering Wheel | Detachable driver control assembly with grips/rim, buttons, paddles, and often LEDs/display. It does not generate main steering torque. | Official |
| Wheel Rim / Rim | Part held by the driver. In modular systems it bolts to a hub. Customers also use “rim” for the entire steering wheel; clarify. | Official/Common |
| Hub | Interface between rim and base, usually carrying controls, paddles, electronics, and quick-release mounting. | Official [F3] |
| Universal Hub | Modular Fanatec hub accepting compatible rims. Bolt pattern, diameter, weight, controls, QR, and platform support still require checks. | Official [F3] |
| Button Module | Add-on control/display assembly mounted to a compatible hub or rim. | Official |
| Paddle Module | Add-on rear control assembly containing shift paddles and, depending on model, analogue clutch paddles. | Official |
| V1 / V2 / V2.5 | Hardware revision in a product name. Do not assume accessories or firmware are interchangeable across revisions. | Official |
| Discontinued / Legacy Product | Product outside the current range but possibly still supported by legacy drivers, firmware, manuals, or spare parts. | Legacy |

## Fanatec DD Wheel-Base Model Families

`DD` means **Direct Drive**, but customers may use “DD” for the drive architecture, the ClubSport DD, or the current Podium DD product. Always request the complete model name.

| Model | Product Position | Advertised Torque | Platform Summary | Main Distinction |
|---|---|---:|---|---|
| CSL DD | Entry-level direct drive | 5 Nm; 8 Nm with supported Boost Kit | Windows PC; Xbox with an Xbox-licensed wheel or hub; not PlayStation licensed | Compact, lower-cost entry into the Fanatec DD ecosystem. [F13] |
| Podium Wheel Base DD1 | Previous-generation Podium flagship | Up to 20 Nm peak | Standard model: Windows PC; Xbox with an Xbox-licensed wheel or hub | Earlier high-torque Podium base. A separate historical PlayStation-licensed DD1 bundle/model existed, so check the exact SKU. [F14] |
| Podium Wheel Base DD2 | Previous-generation Podium flagship | Up to 25 Nm peak | Windows PC; Xbox with an Xbox-licensed wheel or hub | Stronger DD1-era model; Podium Kill Switch supplied as standard. [F14] |
| ClubSport DD | Current ClubSport direct drive | 15 Nm holding torque with current firmware | Windows PC; Xbox with an Xbox-licensed wheel or hub; not PlayStation licensed | Current-generation QR2 and FullForce platform without PlayStation licensing. [F15] |
| ClubSport DD+ | Current ClubSport direct drive | 18 Nm holding torque with current firmware | Windows PC and PlayStation; Xbox with an Xbox-licensed wheel or hub | Stronger ClubSport model with PlayStation licensing. [F15] |
| Podium DD | Current Podium flagship introduced in 2026 | 25 Nm holding torque; up to 33 Nm peak overshoot | Check the current regional product page and attached wheel/hub for platform support | Successor to DD1/DD2, based on the newer ClubSport DD architecture with FullForce. [F16] |

### Naming and comparison cautions

- **CSL DD** is a complete model name: `CSL` is the product tier and `DD` means Direct Drive.
- **DD1** and **DD2** are model identifiers in the previous Podium Wheel Base generation; “2” identifies the higher-output variant, not “Direct Drive version 2.”
- **DD / DD+** usually means **ClubSport DD / ClubSport DD+** in customer shorthand. The `+` identifies the stronger, PlayStation-licensed ClubSport model.
- **Podium DD** without a number is a separate 2026 product. Do not confuse it with DD1, DD2, or generic direct drive.
- DD1/DD2 figures are published as **peak torque**. ClubSport DD/DD+ and Podium DD use **holding torque** in current product claims. These numbers are not directly equivalent.
- ClubSport DD and DD+ originally shipped with advertised holding torque of 12 Nm and 15 Nm. Fanatec firmware V1.4.2.3 or later raised them to 15 Nm and 18 Nm in May 2026 without hardware changes. Update through the latest Fanatec App. [F15]
- Available torque can still be limited by the attached steering wheel, quick release, firmware, or Low Torque Mode. [F4]

## Platform and Compatibility Terms

| Term | Definition and Customer Explanation | Evidence |
|---|---|---|
| Platform | PC or console environment on which the game runs. Support can depend on licensing hardware and peripheral connection path. | Official [F2] |
| PC Compatible | Product works on supported Windows PCs through an approved connection. It does not promise support in every game or non-Windows OS. | Official [F2] |
| Xbox Compatible / Xbox Licensed | Steering wheel or hub contains Xbox licensing hardware. Attached to a compatible Fanatec base, it enables the connected system on Xbox. | Official [F2] |
| Xbox Ready | Conditional label: product becomes usable on Xbox when combined with an Xbox-licensed Fanatec steering wheel or hub. | Official [F2] |
| PlayStation Compatible / PS Licensed | Fanatec PlayStation licensing is in the wheel base. A licensed base enables supported attached components on PlayStation. | Official [F2] |
| PlayStation Ready / PS Ready | Conditional label: component can work on PlayStation when connected through a PlayStation-licensed Fanatec base. | Official [F2] |
| Cross-Platform Setup | Xbox-licensed wheel/hub plus PlayStation-licensed base. This can support PC and both console families; verify every product page. | Official [F2] |
| Console Compatibility | Requires licensed hardware and a supported game. Peripherals normally connect through the base rather than separate console USB ports. | Official [F2][F8] |
| Game Support | Game implements the relevant axes, buttons, FFB, displays, LEDs, or telemetry. Hardware compatibility does not guarantee every feature. | Official [F7] |
| Native Mode | Device identifies in its intended current mode. Exact name and behavior vary by base and platform. | Official/Common |
| Compatibility Mode / CSW Mode | Base emulates or identifies similarly to an older base so games without native support may work. Features can differ. | Official/Legacy |
| PC Mode | Wheel-base mode intended for Windows PC. LED color and mode-switch procedure are product-specific. | Official |
| Xbox Mode | State used with supported hardware on Xbox. Xbox licensing remains in the wheel or licensed hub. | Official |
| PlayStation Mode | State of a PlayStation-licensed base on a supported PlayStation console. Exact indication varies. | Official |
| Standalone | Peripheral connects directly to PC, usually by USB or ClubSport USB Adapter. Standalone USB normally does not work on consoles. | Official [F2][F8] |
| Base-Connected | Peripheral connects to the base, which aggregates it into the base’s host/console connection. | Official [F2] |

## Wheel Base, Torque, and Mounting Terms

| Term / Abbreviation | Definition and Customer Explanation | Evidence |
|---|---|---|
| DD — Direct Drive | Motor shaft drives the steering shaft directly, without belt or gear reduction. DD describes architecture, not automatically torque or quality. | Official/Common |
| Belt Drive | Motor transfers force through belts and pulleys. Common on older wheel bases. | Common/Legacy |
| FFB — Force Feedback | Motor-generated steering force based on game commands and base settings. | Official [F5] |
| Torque | Rotational force at the steering shaft, usually stated in newton-metres. | Standard |
| Nm — Newton-metre | SI unit of torque. More Nm means higher possible torque, not automatically more detail or realism. | Standard |
| Peak Torque | Highest short-duration torque under specified conditions. Do not compare it directly with another product’s holding torque. | Common |
| Holding / Sustained Torque | Torque maintained longer under thermal and electrical limits. Vendor definitions and test conditions can differ. | Common |
| High Torque Mode | State allowing more than the low-torque limit when the wheel/QR combination is approved. Never bypass detection or warnings. | Official [F4] |
| Low Torque Mode | Safety-limited mode for a wheel or QR not approved for full base torque. Current guidance describes 8 Nm on listed high-output bases. | Official [F4] |
| QR — Quick Release | Mechanical/electrical coupling attaching the steering wheel to the base. Both sides and generations must match. | Official [F4] |
| QR1 | First-generation Fanatec QR family. QR1 and QR2 do not mate. Fanatec states QR1 is discontinued, though legacy hardware and model-specific conversion paths remain. | Official [F4][F17] |
| QR1 Lite | Composite wheel-side QR1 with product-specific torque limits. It is not equivalent to QR2 Lite. | Official [F4] |
| QR2 | Current QR family with separate Base-Side and Wheel-Side components. | Official [F4] |
| QR2 Base-Side | QR2 component fitted to the base shaft. Type-C, Type-F, or Type-M is base-specific. | Official [F4] |
| QR2 Wheel-Side | Component fitted to wheel/hub. It mates with official QR2 Base-Side variants, subject to product and torque approval. | Official [F4] |
| QR2 Lite Wheel-Side | Composite QR2 wheel-side. High-torque support is wheel- and firmware-specific; selected products gained full-torque approval. | Official [F4] |
| QR2 Pro Wheel-Side | Premium motorsport-oriented metal variant. “Pro” does not imply universal platform support. | Official [F4] |
| Type-C / Type-F / Type-M | Base-specific QR2 Base-Side variants: Type-C for CSL DD/GT DD Pro, Type-F for ClubSport DD, Type-M for Podium in current guidance. | Official [F4] |
| Shaft | Rotating output member of the wheel base. The Base-Side QR mounts to it. | Standard |
| Flex | Unwanted elastic movement in cockpit, mount, rim, or QR. It can reduce perceived detail without an electronic fault. | Common |
| Play / Backlash | Free movement before load transfers. Ask where: QR, shaft, hub, rim, pedals, or cockpit. | Common |
| Hard Mount | Bolting equipment directly to a cockpit/plate using specified points. Use approved bolt size and depth. | Official |
| Table Clamp | Accessory securing base or shifter to a desk. Desk strength and torque limits matter. | Official |
| Cockpit / Rig | Structural frame supporting seat, base, pedals, and accessories. Rigidity and adjustment affect comfort and FFB perception. | Common |
| Boost Kit | Higher-power supply for supported bases that enables the advertised higher torque configuration. It is not a generic overclock. | Official |
| FullForce | Fanatec high-frequency FFB protocol/effect layer on supported hardware and games. As of June 2026, current official guidance includes CSL DD and GT DD Pro as well as ClubSport and current Podium DD families; game support remains required. | Official [F18] |

## Steering Wheel and Control Terms

| Term / Abbreviation | Definition and Customer Explanation | Evidence |
|---|---|---|
| D-pad | Four-direction digital control, often with center press, used for menus or Tuning Menu navigation. | Common |
| FunkySwitch | Fanatec multi-function control combining directional movement, rotation, and push on supported wheels. | Official [F5] |
| Rotary Encoder | Knob producing step-up/step-down inputs when turned. It usually does not report a fixed absolute position. | Standard |
| Thumb Encoder | Rotary encoder positioned for thumb operation while gripping the wheel. | Common |
| MPS — Multi-Position Switch | Multi-position control exposed as encoder, pulse, constant, or game-selected behavior. | Official [F5] |
| Button Cluster / Island | Group of buttons mounted together, often adjustable on a Universal Hub. | Official [F3] |
| Button Caps | Removable labels matching platform/game functions. Labels do not change electronic compatibility. | Official |
| Magnetic Shifter Paddle | Paddle with magnetic return/detent action, normally reporting a digital shift input. | Common |
| Analogue Paddle | Variable-axis paddle configurable for clutch, handbrake, brake/throttle, or extra axes on supported wheels. | Official [F5] |
| Dual Clutch | Two analogue paddles used together for race starts; one can hold the configured bite point. | Official [F5] |
| CbP — Clutch Bite Point | Configured clutch engagement percentage for repeatable starts with dual analogue paddles. | Official [F5] |
| RevLEDs | RPM/rev indicators controlled by game telemetry or supported software. They do not prove FFB is working. | Official [F7] |
| FlagLEDs | Multi-color LEDs for race flags, pit/limiter, and other supported status indications. | Official [F7] |
| RevStripe | Product-specific illuminated rev strip. Behavior depends on wheel, game, platform, and software. | Official [F7] |
| OLED / LCD | Display technologies showing speed, gear, tuning, telemetry, or status. Content support varies. | Standard/Official |
| Telemetry | Game-exported data such as RPM, speed, gear, flags, or fuel. It drives displays/LEDs and is separate from FFB. | Common |
| Static Shifter Paddles | Fixed paddles attached to base/cockpit rather than rotating with the wheel. Port behavior is base-specific. | Official [F8] |

## Pedal, Shifter, and Handbrake Terms

| Term / Abbreviation | Definition and Customer Explanation | Evidence |
|---|---|---|
| Pedal Set | Throttle, brake, and optional clutch assembly. Connection and USB capability vary by model and installed kits. | Official |
| Throttle / Accelerator | Pedal axis commanding engine power, usually position-sensed. | Common |
| Brake Pedal | Pedal axis commanding braking. It may measure travel, force, or a combination. | Common |
| Clutch Pedal | Pedal axis used for manual clutch control. | Common |
| Load Cell / LC | Force sensor commonly used in brake pedals; measures applied force rather than position. | Official [F6] |
| Hall Sensor | Non-contact magnetic sensor normally measuring pedal or lever position. | Official [F6] |
| Potentiometer / Pot | Contact-based variable resistor measuring position. Wear or contamination can cause noisy readings. | Standard/Common |
| Hydraulic-Equipped Pedal | Uses fluid-based resistance/pressure behavior. Confirm whether the exact model senses pressure, force, or travel. | Official [F6] |
| Elastomer | Compressible polymer element tuning pedal resistance/travel, especially around a load-cell brake. | Official [F6] |
| Preload | Initial compression or force before normal pedal movement. It is not the maximum brake force. | Common |
| Pedal Travel | Physical distance or angle through which a pedal moves. | Standard |
| BRF — Brake Force | Fanatec setting for physical load-cell force corresponding to maximum brake input. It is not brake bias. | Official [F5] |
| Brake Bias | In-game setting distributing braking between front/rear axles; unrelated to BRF calibration. | Common |
| Dead Zone | Input range ignored near start/end of an axis. Too much reduces usable resolution. | Common |
| Calibration | Maps physical minimum, maximum, center, or gear positions to logical values. | Official [F8] |
| H-Pattern | Manual shifter layout with a physical gate position for each gear. | Official [F8] |
| SQ — Sequential | Forward/back shifter actions request next/previous gear. “SQ” identifies sequential capability. | Official [F8] |
| Reverse Inhibitor | Mechanism preventing accidental reverse, such as pressing the lever down first. | Official [F8] |
| Shifter 1 Port | Supports H-pattern and sequential modes for ClubSport Shifter SQ V1.5 in current guidance. | Official [F8] |
| Shifter 2 Port | Supports sequential input; current guidance also identifies Static Shifter Paddles use. | Official [F8] |
| Handbrake | Usually an analogue lever axis for rally/drift input. It is not a digital button unless mapped that way. | Official/Common |
| ClubSport USB Adapter | Adapter allowing selected peripherals to work standalone on PC. Adapter mode must match the peripheral. | Official [F9] |
| RJ12 | Modular connector used for Fanatec peripherals. Connector name does not define protocol or guarantee compatibility. | Official/Common [F9] |
| PS/2 Connector | Mini-DIN used on some legacy base/shifter paths. It does not imply PC keyboard/mouse compatibility. | Official/Legacy [F8] |
| Simultaneous USB and Base Connection | Potentially unsafe/unsupported for some pedals. Follow the exact manual; do not use both unless explicitly permitted. | Product-specific official guidance |

## Software, Firmware, and Setup Terms

| Term | Definition and Customer Explanation | Evidence |
|---|---|---|
| Fanatec App | Current Windows software for supported setup, testing, tuning, LED/display configuration, and firmware management. | Official [F4][F8] |
| Fanatec Control Panel / Wheel Property Page | Legacy Windows configuration interface in older drivers/support material. Ask which package the customer sees. | Legacy [F5] |
| FanaLab | Legacy/companion PC tuning and telemetry-profile tool in older setups. Verify version and active profile before diagnosis. | Legacy |
| Driver | Host software enabling Windows communication with hardware. Driver and firmware versions are different values. | Official [F10] |
| Firmware / FW | Software stored in base, motor controller, wheel, pedals, or adapter. One setup can have several firmware components. | Official [F10] |
| Firmware Manager | Update/recovery workflow for checking and flashing supported firmware. | Official [F8][F10] |
| Bootloader | Minimal device code used to start/recover updates. Update mode does not necessarily mean permanent damage. | Standard |
| Firmware Update | Replaces device firmware. Keep power/cables stable and follow prompts. | Official [F10] |
| Manual Firmware Update | Advanced workflow selecting firmware/component manually. Use only when official instructions require it. | Official [F8] |
| Adapter Mode | ClubSport USB Adapter firmware/personality selected for its attached peripheral. | Official [F8][F9] |
| Device Detection | Host software recognizes the base/components. Detection is separate from game mapping and FFB. | Common |
| Center Calibration | Stores physical straight-ahead as logical center. It does not fix mechanical misalignment. | Official |
| Shifter Calibration | Teaches H-pattern gear positions; may be requested after firmware updates. | Official [F8] |
| Pedal Calibration | Stores pedal minimum/maximum or force range. Modes are product/software-specific. | Official |
| Tuning Menu | Base settings accessible through wheel controls and PC software. It does not replace in-game setup. | Official [F5] |
| Standard Tuning Menu | Simplified view exposing core parameters and fewer profiles. | Official [F5] |
| Advanced Tuning Menu | Full view exposing additional parameters and multiple custom slots. | Official [F5] |
| A SET — Auto Setup | Lets a supported game control tuning values; documented defaults apply when unused. | Official [F5] |
| C SET — Custom Setup | User-adjustable profile in Standard Tuning Menu on supported current bases. | Official [F5] |
| S_1–S_5 / SET 1–5 | User-stored Advanced Tuning Menu profiles. Availability depends on base/firmware. | Official [F5] |
| Profile | Saved group of values. State its owner: base, Fanatec App, FanaLab, or game. | Official/Common |
| Factory Settings | Manufacturer baseline. Resetting profiles is not firmware downgrade or driver uninstall. | Official |

## Tuning Menu Abbreviations

Exact ranges and availability vary by base, wheel, pedals, firmware, and menu mode. Check the current product manual/support page before recommending values. [F5]

| Abbreviation | Expansion | Practical Explanation |
|---|---|---|
| SEN | Sensitivity | Steering rotation in degrees, or automatic game/driver control. Not USB polling sensitivity. |
| FF / FFB | Force Feedback | Base maximum FFB strength. Final torque also depends on game output and modifiers. |
| FFS | Force Feedback Scaling | Selects linear (`LIN`) or peak (`PEA`) behavior. Not the same as FF strength. |
| LIN | Linear | Preserves a more linear request-to-torque relationship, potentially with reduced maximum output. |
| PEA | Peak | Allows peak output behavior on supported bases. |
| NDP | Natural Damper | Adds speed-dependent resistance, controlling movement and oscillation. Too much feels slow. |
| NFR | Natural Friction | Adds relatively constant resistance independent of game detail. Too much masks detail and adds fatigue. [F11] |
| NIN | Natural Inertia | Simulates additional steering mass/inertia, often useful with lighter wheels. |
| INT | FFB Interpolation | Smooths coarse/noisy game FFB. Higher values reduce harshness but can reduce immediacy. |
| FEI | Force Effect Intensity | Changes intensity/sharpness of game force effects. Not the main torque limit. |
| FOR | Force | Scales game constant-force effects. Keep baseline unless game-specific guidance requires change. |
| SPR | Spring | Scales game-requested spring effects. It does not always create automatic centering. |
| DPR | Damper | Scales game-requested damper effects. It differs from base-generated NDP. |
| BRF | Brake Force | Sets load-cell force required for maximum brake input on supported pedals. |
| BLI | Brake Level Indicator | Threshold for supported pedal/wheel vibration; game-controlled behavior can differ. |
| SHO | Shock / Vibration Strength | Controls supported vibration motors, not the base’s main FFB motor. |
| MPS | Multi-Position Switch Function | Chooses how supported multi-position switches report inputs. |
| AUTO | Automatic / Game Specific | Lets game/software select behavior. Meaning depends on the parent setting. |
| ENC | Encoder | MPS sends one input clockwise and another counter-clockwise. |
| CONST | Constant | MPS holds a distinct button state for each position. |
| PULSE | Pulse | MPS sends a brief position-specific input when moved. |
| AP | Analogue Paddles | Selects supported analogue-paddle function. |
| CbP | Clutch Bite Point | Both paddles cooperate as clutch controls for repeatable starts. |
| CH | Clutch / Handbrake | One analogue paddle is clutch; the other is handbrake. |
| bt / BT | Brake / Throttle | Analogue paddles become brake and throttle. |
| AnA | Mappable Analogue Axes | Exposes paddles as additional axes where supported. |

## General FFB and Sim-Racing Terms

| Term / Abbreviation | Definition and Customer Explanation | Evidence |
|---|---|---|
| Axis | Continuous input such as steering, throttle, brake, clutch, or handbrake. | USB/Common |
| Button Mapping | Assignment from physical control to game command. It does not change licensing or firmware. | Common |
| Clipping | FFB reaches configured maximum, so stronger forces lose distinction. Lower/rebalance in-game gain to preserve detail. | Common |
| Damping | Resistance mainly related to motion speed. It stabilizes movement; too much hides fast detail. | Control-system/Common |
| Friction | Resistance opposing movement, including slow movement. In Fanatec tuning, see NFR. | Control-system/Common |
| Inertia | Resistance to rotational-speed change due to effective mass. In tuning, see NIN. | Control-system/Common |
| Oscillation | Unwanted back-and-forth wheel motion. Aggressive FFB, latency, low damping, or game behavior can contribute. | Official/Common [F12] |
| Gain | FFB strength multiplier in game/software. High gain can cause clipping. | Common |
| Minimum Force | Game setting boosting weak center FFB. Excess can cause oscillation on DD bases. | Common [F12] |
| Linearity | How proportionally output torque follows requested force. More linear does not mean stronger. | Common |
| Latency | Delay across game, USB, firmware, motor, and driver response. | Standard |
| Polling / Report Rate | Frequency at which input reports are exchanged. It is not complete end-to-end latency. | USB/Common |
| USB HID | Universal Serial Bus Human Interface Device class for controls such as axes/buttons. | Standard [S1] |
| USB PID | USB Physical Interface Device model for FFB effects; unrelated to product identification numbers. | Standard [S2] |
| RPM | Revolutions per minute. Engine RPM telemetry commonly drives RevLEDs. | Standard |
| Understeer | Front tires lack grip for requested turn. FFB representation depends on game physics/design. | Motorsport/Common |
| Oversteer | Rear tires lose lateral grip relative to front, rotating the car more than requested. | Motorsport/Common |
| Road Effects | Texture, bump, kerb, or vibration effects; physics-derived or canned depending on game. | Common |
| Soft Lock | Software force near the simulated car’s steering limit, not a physical shaft stop. | Common |
| Steering Lock / Rotation | Total steering angle in degrees. Coordinate Fanatec SEN and game steering lock. | Common |
| Sim Rig | Complete installation: cockpit, seat, controls, display/VR, PC/console, and accessories. | Common |
| Telemetry Dashboard | Display driven by game data, often through Fanatec App or SimHub. | Common |

## Troubleshooting Terms

| Term | Definition and Customer-Safe Wording |
|---|---|
| Not Detected | Host does not recognize device. Check power, USB path, mode, cable, port, and Device Manager before assuming firmware failure. |
| Detected but Not Working in Game | Driver sees device, but game mapping/mode/support may be missing. Separate input, FFB, LEDs, and telemetry tests. |
| No FFB | Inputs may work while motor force does not. Check game FFB, mode, FF setting, firmware, torque/safety state, and game focus. |
| FFB Loss | FFB worked then stopped. Record trigger, temperature, USB events, session state, and whether inputs remain. |
| Wheel Disconnect | Detachable wheel loses communication while base stays powered. Check seating, QR pairing, approved contact inspection, firmware, and play. |
| Mis-shift | H-pattern gear reads incorrectly. Confirm Shifter 1, H mode, cable, and calibration. |
| Input Jitter / Spiking | Axis changes without intended movement. Causes can include sensor noise/wear, grounding, cable, calibration, or mechanics. |
| EMI — Electromagnetic Interference | Electrical noise coupled into USB/sensors/cables. Reproduce against routing, grounding, load, and alternate ports. |
| Ground Loop | Unwanted grounded-device current path causing noise/disconnects. Never remove protective earth as a workaround. |
| Power Cycle | Controlled shutdown, wait, restart. It is not unplugging during firmware flash. |
| Re-enumeration | USB device disconnects and appears again, possibly in another mode/identifier. |
| Firmware Mismatch | Components run unintended version combinations. Use current official package guidance. |
| Recovery Mode | Bootloader/update state used to restore firmware. Follow exact-model official steps. |
| Bricked | Informal claim that device is unusable after update. Check recovery, detection, and power before declaring permanent damage. |
| RMA — Return Merchandise Authorization | Approved return/repair process. Supply serial, purchase proof, versions, logs, evidence, and reproduction steps. |
| DOA — Dead on Arrival | Informal first-setup failure label. Complete minimum safe checks before using it in a case. |
| Reproduction Steps | Short sequence reliably causing issue, including state, connections, mode, versions, game, and result. |
| Expected Result | Behavior supported by current manual/product documentation. |
| Actual Result | What customer directly observes, separate from assumed cause. |
| Workaround | Temporary safe method restoring use without claiming root cause is fixed. |
| Root Cause | Verified reason for failure. Symptom, correlation, or reboot success alone is not proof. |

## Abbreviation Quick Index

| Abbreviation | Expansion | Abbreviation | Expansion |
|---|---|---|---|
| AnA | Mappable Analogue Axes | AP | Analogue Paddles |
| BLI | Brake Level Indicator | BRF | Brake Force |
| BT / bt | Brake / Throttle | CbP | Clutch Bite Point |
| CH | Clutch / Handbrake | CONST | Constant |
| CS | ClubSport | DD | Direct Drive |
| DOA | Dead on Arrival | DPR | Damper |
| EMI | Electromagnetic Interference | ENC | Encoder |
| FEI | Force Effect Intensity | FF / FFB | Force Feedback |
| FFS | Force Feedback Scaling | FW | Firmware |
| HID | Human Interface Device | INT | FFB Interpolation |
| LC | Load Cell | LIN | Linear |
| MPS | Multi-Position Switch | NDP | Natural Damper |
| NFR | Natural Friction | NIN | Natural Inertia |
| Nm | Newton-metre | OLED | Organic Light-Emitting Diode |
| PEA | Peak | PID | Physical Interface Device |
| PS | PlayStation | QR | Quick Release |
| R2R | Ready2Race | RMA | Return Merchandise Authorization |
| RPM | Revolutions per minute | SEN | Sensitivity |
| SHO | Shock / Vibration Strength | SPR | Spring |
| SQ | Sequential | USB | Universal Serial Bus |

## Recommended Customer Language

| Avoid | Prefer | Reason |
|---|---|---|
| “Your wheel is incompatible.” | “Confirm the exact base, wheel/hub, QR, platform, and connection path.” | “Wheel” is ambiguous; compatibility is combination-specific. |
| “Turn the force to 8 Nm.” | “Set documented FFB value; base/QR determines available torque.” | Percent settings and physical torque differ. |
| “Xbox compatibility is in the base.” | “Xbox licensing is in the Fanatec steering wheel or licensed hub.” | Prevents wrong purchases. [F2] |
| “PlayStation compatibility is in the wheel.” | “PlayStation licensing is in the Fanatec wheel base.” | Prevents wrong purchases. [F2] |
| “QR2 Lite always limits torque.” | “QR2 Lite support depends on exact wheel and current firmware approval.” | Selected products changed. [F4] |
| “The LEDs are broken.” | “Do LEDs pass App test, and does this game/platform provide telemetry?” | Separates hardware from game support. [F7] |
| “The update bricked it.” | “Device is not detected after update; check recovery/bootloader state.” | Avoids asserting permanent damage. |

## References

### Official Fanatec sources

- **[F1]** [Fanatec ecosystem diagram](https://help.fanatec.com/hc/de/articles/43786297099281-Fanatec-Ecosystem-Diagramm) — category relationships.
- **[F2]** [Platform compatibility explained](https://www.fanatec.com/us-en/platforms) — compatibility labels and licensing locations.
- **[F3]** [Fanatec steering wheel hubs comparison](https://www.fanatec.com/jp/en/explorer/products/steering-wheel/fanatec-hubs-a-comparison/) — hub purpose and distinctions.
- **[F4]** [QR2 Lite torque update](https://www.fanatec.com/eu/en/explorer/products/steering-wheel/qr2-lite-torque-limit-lifted/) and [QR2 conversion guidance](https://help.fanatec.com/hc/en-us/articles/30011253510289-Which-products-can-be-converted-to-QR2) — QR variants and current cautions.
- **[F5]** [Tuning Menu parameters](https://help.fanatec.com/hc/en-us/articles/43901256649233-In-the-Tuning-Menu-of-your-wheel-base-you-can-adjust-a-variety-of-parameters) — setting names and modes.
- **[F6]** [Load cell, Hall, and hydraulic-equipped pedals](https://www.fanatec.com/us/en/explorer/products/pedals/difference-load-cell-hall-sensor-and-hydraulic-pedals/) — pedal sensors.
- **[F7]** [RevLED and FlagLED activation](https://help.fanatec.com/hc/en-us/articles/30312122625553-How-do-I-activate-the-RevLEDs-or-flag-LEDs-on-my-wheel) and [FlagLED guide](https://www.fanatec.com/au/en/explorer/products/steering-wheel/understanding-fanatec-steering-wheel-flagleds/) — game/platform dependencies.
- **[F8]** [ClubSport Shifter SQ V1.5 guide](https://www.fanatec.com/us/en/explorer/products/shifters/guide-to-fanatecs-clubsport-shifter-sq-v15/) and [shifter-port guidance](https://help.fanatec.com/hc/en-us/articles/45597346898449-Which-shifter-port-should-I-use-on-my-Fanatec-wheel-base) — modes, calibration, ports.
- **[F9]** [ClubSport USB Adapter guide](https://www.fanatec.com/ca/en/explorer/products/handbrakes/guide-to-fanatecs-clubsport-usb-adapter/) — standalone use and adapter mode.
- **[F10]** [Driver and Firmware Instructions](https://assets.fanatec.com/fanatec-pwa/image/upload/downloads-prod/pdfs/Driver-Firmware-Instructions-Manual-EN_Web_02_MO.pdf) — update terminology.
- **[F11]** [Natural Friction explained](https://www.fanatec.com/us/en/explorer/products/racing-wheels-wheel-bases/nfr-natural-friction-tuning-menu/) — NFR behavior.
- **[F12]** [Wheel oscillation guidance](https://help.fanatec.com/hc/en-us/articles/30312108300177-Why-is-my-steering-wheel-oscillating-or-shaking) — oscillation and settings.
- **[F13]** [CSL DD Wheel Base guide](https://www.fanatec.com/au/en/explorer/products/racing-wheels-wheel-bases/fanatec-csl-dd-wheel-base/) — 5 Nm and 8 Nm configurations.
- **[F14]** [Podium DD1 and DD2 comparison](https://www.fanatec.com/us/en/explorer/products/racing-wheels-wheel-bases/podium-dd1-vs-dd2-differences/) — previous Podium generation, peak torque, features, and platform support.
- **[F15]** [ClubSport DD and DD+ torque update](https://www.fanatec.com/us/en/explorer/products/racing-wheels-wheel-bases/more-torque-same-hardware/) and [ClubSport DD+ product page](https://www.fanatec.com/us/en/p/wheel-bases/cs_dd%2B_us/clubsport-dd-plus-eu) — current 15 Nm and 18 Nm holding-torque figures and firmware requirement.
- **[F16]** [Podium DD, DD1, and DD2 comparison](https://www.fanatec.com/ca/en/explorer/products/racing-wheels-wheel-bases/fanatec-podium-dd-vs-dd1-vs-dd2-key-differences/) — current Podium DD architecture, holding torque, and peak overshoot.
- **[F17]** [Fanatec Steering Wheel FAQ](https://help.fanatec.com/hc/en-us/articles/43802514108433-Steering-Wheel-FAQ) — QR2 default and QR1 discontinuation date.
- **[F18]** [FullForce arrives on CSL DD and Gran Turismo DD Pro](https://www.fanatec.com/us/en/explorer/products/racing-wheels-wheel-bases/fullforce-arrives-on-csl-dd-and-gran-turismo-dd-pro/) — June 2026 FullForce expansion and current-generation architecture context.
- **[F19]** [What does CSL mean?](https://help.fanatec.com/hc/de/articles/30312787274641-What-does-CSL-mean) — Official definition of the CSL abbreviation as ClubSport Light and its entry-level classification.

### Standards sources

- **[S1]** [USB-IF HID specifications and tools](https://www.usb.org/hid) — USB input terminology.
- **[S2]** [USB PID Class 1.0](https://www.usb.org/sites/default/files/documents/pid1_01.pdf) — USB FFB model.

### Supplemental community reading

These aid onboarding but are not authoritative for current compatibility or safety decisions.

- [OC Racing: Fanatec ecosystem explained](https://ocracing.com/guides/fanatec-ecosystem-explained-for-dummies/)
- [Sim Racing Setups: Fanatec ecosystem explained](https://simracingsetup.com/product-guides/fanatec-ecosystem-explained/)
- [Internal developer study index](./README.md)
- [Ecosystem source register and currency notes](./references.md)

## Actionable Next Steps

1. Use this vocabulary in support scripts, ticket templates, FAQs, and specifications.
2. Keep SKU compatibility matrices separate; a glossary should not duplicate fast-changing compatibility data.
3. Recheck official compatibility, QR, App, and Tuning Menu sources before major releases.
4. Record customer wording as aliases only after mapping it to an exact component term.
