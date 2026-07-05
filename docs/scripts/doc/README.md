# OBC/DCDC Specification — DOCX Generation

## Prerequisites

- Python venv: `.venv\Scripts\Activate.ps1`
- pandoc: https://pandoc.org/installing.html
- Node.js + mermaid CLI: `npm install -g @mermaid-js/mermaid-cli`
- Inkscape 1.x (SVG→PNG at full resolution): `winget install Inkscape.Inkscape`

## Full build (first run or after SVG changes)

Deletes cached SVG PNGs and re-renders everything from scratch at 300 DPI.

```powershell
# Remove cached SVG-derived PNGs
Remove-Item docs\dcdc_psu_phase_shift.png, docs\obc_acdc_pwm_vac_neg.png, `
            docs\obc_acdc_pwm_vac_pos.png, docs\obc_acdc_pwm_vac_transition.png, `
            docs\obc_dcac_pwm_vac_neg.png, docs\obc_dcac_pwm_vac_pos.png, `
            docs\obc_dcac_pwm_vac_transition.png, docs\obc_dcdc_protection.png `
            -ErrorAction SilentlyContinue

# Step 1 — render diagrams + generate raw DOCX
python scripts/pandoc/md_to_pandoc.py docs/obc_dcdc_specification.md `
    --reference-doc scripts/pandoc/my_style.docx `
    --keep-heading-numbers --svg-dpi 300

# Step 2 — post-process XML → final DOCX
python scripts/pandoc/docx_postprocess.py docs/obc_dcdc_specification.docx `
    -r scripts/pandoc/my_style.docx `
    -o docs/obc_dcdc_specification_final.docx -v
```

Output: `docs/obc_dcdc_specification_final.docx`

## Incremental re-run (Markdown/table changes only, PNGs unchanged)

```powershell
python scripts/pandoc/md_to_pandoc.py docs/obc_dcdc_specification.md `
    --reference-doc scripts/pandoc/my_style.docx `
    --keep-heading-numbers --skip-mermaid --skip-svg-convert

python scripts/pandoc/docx_postprocess.py docs/obc_dcdc_specification.docx `
    -r scripts/pandoc/my_style.docx `
    -o docs/obc_dcdc_specification_final.docx -v
```

```powershell
# First run (renders all diagrams from scratch)
python scripts/pandoc/md_to_pandoc.py docs/obc_dcdc_specification.md --reference-doc scripts/pandoc/my_style.docx --keep-heading-numbers

# Incremental re-runs (PNGs cached, only text/table processing + pandoc re-runs)
python scripts/pandoc/md_to_pandoc.py docs/obc_dcdc_specification.md --reference-doc scripts/pandoc/my_style.docx --keep-heading-numbers --skip-svg-convert

# Post-process
python scripts/pandoc/docx_postprocess.py docs/obc_dcdc_specification.docx -r scripts/pandoc/my_style.docx -o docs/obc_dcdc_specification_final.docx -v

```