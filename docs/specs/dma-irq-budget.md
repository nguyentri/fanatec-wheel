# DMA / IRQ Budget (Phase 4, spec 4-S5 - living document)

Rule (spec s.4): the link path owns the highest priority; nothing may
add latency to link SPI. Order: **link > ADC scan > LED stream > rest**.

## Interrupt priorities (NVIC, lower = higher priority)

| Source | Prio | Owner | Notes |
|---|---|---|---|
| SPI1 (link slave) | 0 | link_spi | never preempted by app IRQs |
| EXTI PG14 (CS sense) | 0 | link_spi | CS-gap timestamping |
| DMA for SPI1 RX/TX | 0 | HAL | paired with SPI1 |
| ADC1 (dpad + clutch) | 4 | input_svc | 1 kHz polled reads today; DMA circular planned on PCB rev A |
| LED strip SPI/DMA | 8 | led_svc | <= 60 Hz, absent on bench board |
| I2C (LRA) | 8 | lra_svc | bounded cues only, absent on bench board |
| IWDG | n/a | rim_wdt | no interrupt (reset source) |
| USART (shell/log) | 12 | diag | lowest of the peripherals |

## DMA stream allocation (STM32H723 DMA1/DMA2)

| Stream | Assignment | Status |
|---|---|---|
| DMA1 S0/S1 | SPI1 RX/TX (link) | active (Zephyr SPI driver) |
| DMA1 S2 | ADC1 circular scan | planned - PCB rev A (measurement pending) |
| DMA1 S3 | LED strip SPI TX | planned - PCB rev A |
| others | free | - |

Deviations: IWDG lower window bound is 0 (true windowed WDT is a part
selection item for PCB rev A, spec 4-S6 note in rim_wdt.h).

Update this table with every peripheral addition; the code review
checklist (docs/release.md) gates on it being current.
