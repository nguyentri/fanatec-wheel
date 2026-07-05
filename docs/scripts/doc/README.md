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
Remove-Item docs\studies\assets\*.png -ErrorAction SilentlyContinue

# Step 1 — render diagrams + generate raw DOCX
python docs\scripts\doc\md_to_pandoc.py docs\studies\en\add_ons.md `
    --reference-doc docs\scripts\doc\my_style.docx `
    --keep-heading-numbers --svg-dpi 300

# Step 2 — post-process XML → final DOCX
python docs\scripts\doc\docx_postprocess.py docs\studies\en\add_ons.docx `
    -r docs\scripts\doc\my_style.docx `
    -o docs\studies\en\add_ons_final.docx -v
```

Output: `docs\studies\en\add_ons_final.docx`

## Incremental re-run (Markdown/table changes only, PNGs unchanged)

```powershell
python docs\scripts\doc\md_to_pandoc.py docs\studies\en\add_ons.md `
    --reference-doc docs\scripts\doc\my_style.docx `
    --keep-heading-numbers --skip-mermaid --skip-svg-convert

python docs\scripts\doc\docx_postprocess.py docs\studies\en\add_ons.docx `
    -r docs\scripts\doc\my_style.docx `
    -o docs\studies\en\add_ons_final.docx -v
```

```powershell
# First run (renders all diagrams from scratch)
python docs\scripts\doc\md_to_pandoc.py docs\studies\en\add_ons.md --reference-doc docs\scripts\doc\my_style.docx --keep-heading-numbers

# Incremental re-runs (PNGs cached, only text/table processing + pandoc re-runs)
python docs\scripts\doc\md_to_pandoc.py docs\studies\en\add_ons.md --reference-doc docs\scripts\doc\my_style.docx --keep-heading-numbers --skip-svg-convert

# Post-process
python docs\scripts\doc\docx_postprocess.py docs\studies\en\add_ons.docx -r docs\scripts\doc\my_style.docx -o docs\studies\en\add_ons_final.docx -v
```