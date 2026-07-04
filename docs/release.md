# Release Definition (Phase 5, spec 5-S6)

A release is a tag `vX.Y.Z` on a green main. The CI release job
produces, from one tag:

1. `zephyr.signed.bin` + `mcuboot.bin` (sysbuild, release key)
2. `zephyr.bin` (bench/dev, unsigned, direct 0x08000000)
3. Compatibility matrix snapshot (`docs/compat/matrix.md` at the tag)
4. Test evidence: unit-suite logs + build warnings count (must be 0)
5. `session`: git hash + VERSION embedded in the image (2-S3)

## Checklist (gate, in order)

- [ ] All unit suites green (`tests/unit/rimlink`, `tests/unit/app_logic`)
- [ ] All targets build with zero warnings (app, app+TM1637, sim, sysbuild)
- [ ] docs/specs/dma-irq-budget.md current for any peripheral change
- [ ] Compat matrix has a row for every base/fw combination claimed
- [ ] Soak: 24 h dual-console run PASS at current thresholds
      (scripts/soak/soak_runner.py) - hardware pending on bench
- [ ] VERSION bumped; CHANGELOG entry; tag signed
- [ ] Interrupted-update test on hardware for any MCUboot change
      (docs/update-recovery.md) - hardware pending on bench
