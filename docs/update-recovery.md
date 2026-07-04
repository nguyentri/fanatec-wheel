# Update & Recovery (Phase 5, spec 5-S1 / 5-S2)

## Verified boot chain

MCUboot (128 KiB `boot_partition`) verifies an ED25519 signature over
the app image before jumping. Build the chained pair with:

    west build -b fk723m1_zgt6 app --sysbuild

Artifacts: `mcuboot/zephyr/zephyr.bin` (flash at 0x08000000) and
`app/zephyr/zephyr.signed.bin` (slot0, 0x08020000).

### Keys

Development builds sign with MCUboot's bundled `root-ed25519.pem` -
**never ship this key**. Production: generate a per-product key
(`imgtool keygen -k rim-release.pem -t ed25519`), store it in the CI
secret store, and set `SB_CONFIG_BOOT_SIGNATURE_KEY_FILE`. The public
key is compiled into the bootloader; rotating it requires reflashing
the boot partition over SWD.

## Dual-slot update flow

1. New image is written to slot1 (`image-1`, 0x08060000) over the
   service interface and marked pending (`imgtool`/mcumgr semantics).
2. MCUboot swap-using-move exchanges the slots on the next boot.
3. The app must confirm the image after its self-test (link armed +
   first valid transaction); an unconfirmed image is swapped back
   automatically - a failed update can not brick the rim (spec 5-S2).

Interrupted-update torture test (power cut during swap):
scripts/soak/soak_runner.py hooks + bench procedure - **measurement
pending** (requires hardware power control).

## Boot deadline with the loader

`rim boot` reports boot-to-ready including the MCUboot stage.
Re-measure against the base first-poll deadline and record >= 2x
margin - **measurement pending**. Mitigation ladder if the margin
fails (question register #3): shrink MCUboot (disable revert
logging), move to direct-XIP, or hold QR detect until ready.

## Last-resort recovery

SWD service pads (PCB rev A: TC2030 footprint) reach the bootloader
and both slots regardless of app state. Bench board: onboard ST-Link
header. `west flash` restores a known-good chained pair.
