# Board Ports

## fk723m1_zgt6 (bench bring-up - active)

Upstream Zephyr board; all Phase 1-5 firmware targets it. Pin
assignments live in `app/boards/fk723m1_zgt6.overlay`; generate the
current pin table with:

    python3 scripts/pin_registry.py app/boards/fk723m1_zgt6.overlay

## rim_pcb_a (custom PCB rev A - hardware pending)

Planned out-of-tree board dir (`boards/rim/rim_pcb_a/`) once the PCB
exists (spec 4-S4). Bring-up checklist per hardware spec s.4:

1. Copy the overlay pinmux into the board DTS; run the pin registry
   diff against the schematic net list - zero conflicts required.
2. Populate the nodes that are guarded stubs on the bench board:
   `out_rail` (load switch, spec 4-S3), the `chosen zephyr,display` LCD node + CONFIG_LVGL (spec 4-S1),
   `lra0` (DRV2605-class, spec 4-S2), and a windowed watchdog part
   if selected (spec 4-S6 deviation note).
3. Boot with the same application binary configuration; the services
   detect the nodes and leave counters-only mode automatically.
4. Re-run the Phase 2 boot-to-ready measurement and the Phase 3 scan
   budget on the real fabric (both `measurement pending`).
