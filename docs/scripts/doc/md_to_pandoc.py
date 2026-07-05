#!/usr/bin/env python3
"""
md_to_pandoc.py  —  One-shot Markdown → Pandoc pipeline
=========================================================
Steps performed automatically:
  1. Render every ```mermaid block to a PNG (via mmdc / npx)
  2. Replace each mermaid block in the Markdown with an image reference
  3. Render boxed text diagrams in fenced code blocks to PNG
  4. Convert every .svg image reference to a .png beside the source   ← NEW
  5. Rewrite .svg image references to .png
  6. Convert inconsistent pipe tables to pandoc grid tables
  7. Strip redundant heading numbers (e.g. "## 1. Title" -> "## Title")
  8. Remove manual Table of Contents section (pandoc generates a real one)
  9. Write a cleaned intermediate Markdown file
 10. Run pandoc to produce the final output document

Supported output formats (--format):
  docx  (default)  Microsoft Word
  html             Standalone HTML with embedded CSS
  pdf              PDF via wkhtmltopdf or weasyprint (must be installed)
  md               Cleaned Markdown only (skip pandoc step)

Requirements
------------
  Python 3.8+
    Pillow          pip install pillow
    cairosvg        pip install cairosvg   (SVG→PNG; or use rsvg-convert/inkscape/convert)
  pandoc          https://pandoc.org/installing.html
  mmdc  OR  npx   npm install -g @mermaid-js/mermaid-cli
  Chrome/Chromium (auto-detected) for Mermaid PNG rendering

Usage
-----
  python md_to_pandoc.py INPUT.md                        # -> INPUT.docx
  python md_to_pandoc.py INPUT.md -o report.docx
  python md_to_pandoc.py INPUT.md -f html -o report.html
  python md_to_pandoc.py INPUT.md -f pdf  -o report.pdf
  python md_to_pandoc.py INPUT.md -f md   -o clean.md    # preprocess only
  python md_to_pandoc.py INPUT.md --reference-doc custom.docx
  python md_to_pandoc.py INPUT.md --skip-mermaid
  python md_to_pandoc.py INPUT.md --skip-svg-convert     # PNGs already exist
  python md_to_pandoc.py INPUT.md --svg-dpi 200          # higher-res SVG export
  python md_to_pandoc.py INPUT.md --keep-heading-numbers
  python md_to_pandoc.py INPUT.md --keep-manual-toc
"""

from __future__ import annotations

import argparse
import json
import re
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path


# ---------------------------------------------------------------------------
# Constants
# ---------------------------------------------------------------------------

MERMAID_FENCE_RE = re.compile(
    r"(```mermaid\s*\n(.*?)\n```|~~~mermaid\s*\n(.*?)\n~~~)",
    re.IGNORECASE | re.DOTALL,
)

FENCED_BLOCK_RE = re.compile(
    r"(?P<fence>`{3,}|~{3,})(?P<info>[^\n]*)\n(?P<code>.*?)(?:\n(?P=fence))[ \t]*(?:\n|$)",
    re.DOTALL,
)

# Matches leading section numbers in headings:
#   ## 1. Title        -> ## Title
#   ### 1.1 Title      -> ### Title
#   #### 1.2.3 Title   -> #### Title
HEADING_NUM_RE = re.compile(
    r"^(#{1,6}\s+)(\d+(?:\.\d+)*\.?\s+)(.+)$",
    re.MULTILINE,
)

# Matches a manual TOC section up to (but not including) the next ## heading
MANUAL_TOC_RE = re.compile(
    r"^##\s+(?:Table\s+of\s+)?Contents?\s*\n.*?(?=^##\s|\Z)",
    re.IGNORECASE | re.MULTILINE | re.DOTALL,
)

# Chrome executable candidates (priority order)
_CHROME_CANDIDATES = [
    r"C:\Program Files\Google\Chrome\Application\chrome.exe",
    r"C:\Program Files (x86)\Google\Chrome\Application\chrome.exe",
    str(Path.home() / "AppData/Local/Google/Chrome/Application/chrome.exe"),
    r"C:\Program Files\Microsoft\Edge\Application\msedge.exe",
    r"C:\Program Files (x86)\Microsoft\Edge\Application\msedge.exe",
    str(Path.home() / "AppData/Local/Microsoft/Edge/Application/msedge.exe"),
    "/opt/google/chrome/chrome",
    "/opt/pw-browsers/chromium-1194/chrome-linux/chrome",
    "/usr/bin/google-chrome",
    "/usr/bin/google-chrome-stable",
    "/usr/bin/chromium-browser",
    "/usr/bin/chromium",
]

PANDOC_FORMATS = {
    "docx": {"ext": ".docx", "pandoc_to": "docx"},
    "html": {"ext": ".html", "pandoc_to": "html"},
    "pdf":  {"ext": ".pdf",  "pandoc_to": "pdf"},
    "md":   {"ext": ".md",   "pandoc_to": None},
}

TEXT_DIAGRAM_CHARS = set("┌┐└┘├┤┬┴┼│─═╔╗╚╝╠╣╦╩╬►◄▲▼")

_MONOSPACE_FONT_CANDIDATES = [
    r"C:\Windows\Fonts\CascadiaMono.ttf",
    r"C:\Windows\Fonts\consola.ttf",
    r"C:\Windows\Fonts\lucon.ttf",
    "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf",
    "/usr/share/fonts/truetype/liberation2/LiberationMono-Regular.ttf",
    "/usr/share/fonts/TTF/DejaVuSansMono.ttf",
    "/System/Library/Fonts/Menlo.ttc",
    "/System/Library/Fonts/SFNSMono.ttf",
]


# ---------------------------------------------------------------------------
# Chrome / Puppeteer helpers
# ---------------------------------------------------------------------------

def _resolve_executable(*names: str) -> str | None:
    """Return a concrete executable path for the first matching command name."""
    for name in names:
        found = shutil.which(name)
        if found:
            return found
    return None

def _find_chrome() -> str | None:
    """Return path to a usable Chrome/Chromium binary, or None."""
    for path in _CHROME_CANDIDATES:
        if Path(path).is_file():
            return path
    for name in (
        "chrome",
        "chrome.exe",
        "msedge",
        "msedge.exe",
        "google-chrome-stable",
        "google-chrome",
        "chromium-browser",
        "chromium",
    ):
        found = _resolve_executable(name)
        if found:
            return found
    return None


def _write_puppeteer_config(tmp_dir: Path) -> Path | None:
    """
    Write puppeteer.json pointing to the system Chrome inside tmp_dir.
    Returns the config path, or None if no Chrome binary is found.
    """
    chrome = _find_chrome()
    if chrome is None:
        return None
    cfg = tmp_dir / "puppeteer.json"
    cfg.write_text(json.dumps({
        "executablePath": chrome,
        "args": [
            "--no-sandbox",
            "--disable-setuid-sandbox",
            "--disable-dev-shm-usage",
            "--disable-gpu",
        ],
    }))
    return cfg


def _load_pillow():
    try:
        from PIL import Image, ImageDraw, ImageFont
    except ImportError as exc:
        raise RuntimeError(
            "Pillow is required to render boxed text diagrams. "
            "Install with: python -m pip install pillow"
        ) from exc
    return Image, ImageDraw, ImageFont


def _load_monospace_font(font_size: int):
    _, _, image_font = _load_pillow()

    for candidate in _MONOSPACE_FONT_CANDIDATES:
        if Path(candidate).is_file():
            try:
                return image_font.truetype(candidate, font_size)
            except OSError:
                pass

    for candidate in ("Cascadia Mono", "Consolas", "DejaVu Sans Mono", "Liberation Mono", "Menlo"):
        try:
            return image_font.truetype(candidate, font_size)
        except OSError:
            pass

    raise RuntimeError(
        "No suitable monospace font found for boxed text diagram rendering. "
        "Install Cascadia Mono, Consolas, or DejaVu Sans Mono."
    )


def _looks_like_text_diagram(info: str, code: str) -> bool:
    lang = (info.strip().split()[0].lower() if info.strip() else "")
    if lang == "mermaid":
        return False

    lines = [line.rstrip("\r") for line in code.splitlines()]
    if len(lines) < 3:
        return False

    box_char_count = sum(1 for char in code if char in TEXT_DIAGRAM_CHARS)
    structured_lines = sum(
        1 for line in lines
        if any(char in line for char in TEXT_DIAGRAM_CHARS)
    )
    return box_char_count >= 12 and structured_lines >= 3


def _render_text_diagram(code: str, out_png: Path) -> None:
    image, image_draw, _ = _load_pillow()

    font_size = 20
    padding = 24
    line_gap = 8
    lines = [line.rstrip("\r") for line in code.rstrip("\n").splitlines()]
    if not lines:
        lines = [""]

    font = _load_monospace_font(font_size)
    probe = image.new("RGB", (1, 1), "white")
    draw = image_draw.Draw(probe)

    widths: list[int] = []
    heights: list[int] = []
    for line in lines:
        left, top, right, bottom = draw.textbbox((0, 0), line, font=font)
        widths.append(right - left)
        heights.append(bottom - top)

    line_height = max(heights or [font_size]) + line_gap
    img_width = max(widths or [0]) + padding * 2
    img_height = line_height * len(lines) + padding * 2 - line_gap

    rendered = image.new("RGB", (img_width, img_height), "white")
    draw = image_draw.Draw(rendered)

    y = padding
    for line in lines:
        draw.text((padding, y), line, fill="black", font=font)
        y += line_height

    rendered.save(out_png)


# ---------------------------------------------------------------------------
# Step 1 + 2  — Mermaid -> PNG  +  block replacement
# ---------------------------------------------------------------------------

def _mermaid_cli() -> list[str]:
    """Return the mermaid CLI invocation (mmdc preferred, falls back to npx)."""
    mmdc = _resolve_executable("mmdc", "mmdc.cmd", "mmdc.exe")
    if mmdc:
        return [mmdc]

    npx = _resolve_executable("npx", "npx.cmd", "npx.exe")
    if npx:
        return [npx, "-y", "@mermaid-js/mermaid-cli"]

    raise RuntimeError(
        "Mermaid CLI not found.\n"
        "Install with:  npm install -g @mermaid-js/mermaid-cli\n"
        "or make sure 'npx' is available in PATH."
    )


def _render_mermaid(code: str, out_png: Path) -> None:
    cli = _mermaid_cli()
    with tempfile.TemporaryDirectory() as tmp:
        tmp_path = Path(tmp)
        src = tmp_path / "diagram.mmd"
        src.write_text(code, encoding="utf-8")

        cmd = [*cli, "-i", str(src), "-o", str(out_png)]

        # Inject puppeteer config so mmdc can locate the system Chrome
        puppeteer_cfg = _write_puppeteer_config(tmp_path)
        if puppeteer_cfg is not None:
            cmd += ["-p", str(puppeteer_cfg)]
        elif cli[0].lower().endswith(("npx", "npx.cmd", "npx.exe", "mmdc", "mmdc.cmd", "mmdc.exe")):
            raise RuntimeError(
                "Chrome/Chromium not found for Mermaid rendering. "
                "Install Google Chrome, Microsoft Edge, or Chromium, "
                "or rerun with --skip-mermaid if the PNGs already exist."
            )

        result = subprocess.run(cmd, capture_output=True, text=True)

    if result.returncode != 0:
        raise RuntimeError(
            f"Mermaid render failed -> {out_png.name}\n"
            f"STDOUT:\n{result.stdout}\nSTDERR:\n{result.stderr}"
        )


def process_mermaid_blocks(text: str, md_path: Path, verbose: bool) -> str:
    """
    Render all mermaid blocks to PNGs next to the source file and replace
    each fenced block with a Markdown image reference.
    """
    out_dir = md_path.parent
    stem = md_path.stem
    counter = 0

    def replace(match: re.Match) -> str:
        nonlocal counter
        counter += 1
        code = (match.group(2) or match.group(3) or "").strip()
        if not code:
            return match.group(0)

        png_name = f"{stem}_mermaid_{counter:02d}.png"
        out_png = out_dir / png_name

        if out_png.exists():
            if verbose:
                print(f"  [mermaid {counter}] {png_name} already exists, skipping render")
        else:
            if verbose:
                print(f"  [mermaid {counter}] Rendering -> {png_name}")
            try:
                _render_mermaid(code, out_png)
            except RuntimeError as exc:
                print(f"  WARNING: {exc}", file=sys.stderr)
                return (
                    f"<!-- Mermaid diagram {counter}: render failed -->\n\n"
                    f"![Mermaid diagram {counter} (render failed)]({png_name})\n"
                )

        return (
            f"<!-- mermaid diagram {counter} pre-rendered to {png_name} -->\n\n"
            f"![Mermaid diagram {counter}]({png_name})\n"
        )

    return MERMAID_FENCE_RE.sub(replace, text)


def process_text_diagram_blocks(text: str, md_path: Path, verbose: bool) -> str:
    """
    Render boxed Unicode/ASCII diagrams inside fenced code blocks to PNGs and
    replace those fenced blocks with image references.
    """
    out_dir = md_path.parent
    stem = md_path.stem
    counter = 0

    def replace(match: re.Match) -> str:
        nonlocal counter
        info = match.group("info") or ""
        code = match.group("code") or ""

        if not _looks_like_text_diagram(info, code):
            return match.group(0)

        counter += 1
        png_name = f"{stem}_textdiag_{counter:02d}.png"
        out_png = out_dir / png_name

        if verbose:
            print(f"  [textdiag {counter}] Rendering -> {png_name}")

        try:
            _render_text_diagram(code, out_png)
        except RuntimeError as exc:
            print(f"  WARNING: {exc}", file=sys.stderr)
            return match.group(0)

        return (
            f"<!-- boxed text diagram {counter} rendered to {png_name} -->\n\n"
            f"![Boxed text diagram {counter}]({png_name})\n"
        )

    return FENCED_BLOCK_RE.sub(replace, text)


# ---------------------------------------------------------------------------
# Step 4  — .svg -> .png image references
# ---------------------------------------------------------------------------

def svg_to_png_refs(text: str) -> str:
    """Rewrite .svg extensions to .png **only** inside Markdown image references.

    The original pattern ``re.sub(r'\\.svg\\b', '.png', text)`` was too broad:
    it also mutated .svg occurrences inside fenced code blocks, plain hyperlinks
    ``[label](file.svg)``, and prose text — corrupting shell examples and URLs.

    The corrected pattern anchors to the ``![alt](path)`` image syntax so that
    only actual image embeds are rewritten.
    """
    return re.sub(r'(!\[[^\]]*\]\([^)]*?)\.svg\b', r'\1.png', text)


# ---------------------------------------------------------------------------
# Step 3b  — Convert SVG assets to PNG
# ---------------------------------------------------------------------------

_SVG_REF_RE = re.compile(r'!\[[^\]]*\]\(([^)]*\.svg[^)]*)\)')


def _render_svg_via_browser(svg: Path, png: Path, dpi: int = 150, chrome: str = "") -> None:
    """Render an SVG file to PNG using a headless Chrome/Edge instance.

    Creates a minimal HTML wrapper around the SVG, exports it via the browser's
    --screenshot flag, then crops white borders using Pillow.
    """
    import xml.etree.ElementTree as ET

    # Read SVG natural dimensions (fall back to 1200x900)
    width, height = 1200, 900
    try:
        tree = ET.parse(str(svg))
        root = tree.getroot()
        # Strip namespace for attribute lookup
        vb = root.get("viewBox", "")
        w_attr = root.get("width", "")
        h_attr = root.get("height", "")
        if vb:
            parts = vb.strip().replace(",", " ").split()
            if len(parts) == 4:
                width  = max(1, int(float(parts[2])))
                height = max(1, int(float(parts[3])))
        elif w_attr and h_attr:
            width  = max(1, int(float(w_attr.rstrip("px "))))
            height = max(1, int(float(h_attr.rstrip("px "))))
    except Exception:
        pass

    # Scale up for the requested DPI (SVG native is ~96 dpi); cap to avoid browser OOM
    scale = dpi / 96.0
    win_w = min(int(width  * scale), 2400)
    win_h = min(int(height * scale), 3600)
    # Keep aspect ratio if either dimension was clamped
    original_ratio = height / max(width, 1)
    if win_w == 2400 or win_h == 3600:
        if win_w / win_h < width / max(height, 1):
            win_h = max(1, int(win_w * original_ratio))
        else:
            win_w = max(1, int(win_h / original_ratio))

    with tempfile.TemporaryDirectory() as tmp:
        tmp_path = Path(tmp)
        html_path = tmp_path / "render.html"
        shot_path = tmp_path / "screenshot.png"

        # Inline the SVG so the browser renders it at exactly the viewport size
        svg_content = svg.read_text(encoding="utf-8", errors="replace")
        html_path.write_text(
            f"""<!DOCTYPE html><html><head><meta charset='utf-8'>
<style>*{{margin:0;padding:0;background:white}}</style></head>
<body>{svg_content}</body></html>""",
            encoding="utf-8",
        )

        cmd = [
            chrome,
            "--headless=new",
            "--disable-gpu",
            "--no-sandbox",
            "--disable-dev-shm-usage",
            f"--screenshot={shot_path}",
            f"--window-size={win_w},{win_h}",
            f"file:///{html_path.as_posix()}",
        ]
        result = subprocess.run(cmd, capture_output=True, timeout=30)
        if result.returncode != 0 or not shot_path.exists():
            raise RuntimeError(
                f"Browser screenshot failed (exit {result.returncode})"
            )
        # Copy (Pillow not strictly required here, but trims if available)
        import shutil as _sh
        _sh.copy(shot_path, png)


def _is_imagemagick_convert(path: str) -> bool:
    """Return True only if the 'convert' binary is actually ImageMagick.

    On Windows, C:\\Windows\\System32\\convert.exe is a disk-conversion utility
    that has nothing to do with ImageMagick and will silently fail on SVGs.
    """
    try:
        r = subprocess.run([path, "--version"], capture_output=True, text=True, timeout=5)
        return "imagemagick" in (r.stdout + r.stderr).lower()
    except Exception:
        return False


def _find_svg_converter() -> tuple[str | None, list[str]]:
    """Return (tool_name, cmd_prefix) for SVG→PNG conversion.

    Priority order:
      1. cairosvg      — pure-Python, best quality (pip install cairosvg + cairo DLL)
      2. rsvg-convert  — librsvg CLI, fast and accurate
      3. inkscape      — full SVG support, slower
      4. magick        — ImageMagick 7 CLI (magick convert)
      5. convert       — ImageMagick 6 CLI (skipped if Windows system convert.exe)
      6. browser       — Chrome/Edge headless, no extra install required
    Returns (None, []) if nothing is found.
    """
    try:
        import cairosvg as _cs  # noqa: F401
        return "cairosvg", []
    except (ImportError, OSError, Exception):
        pass
    for cmd in ("rsvg-convert",):
        if shutil.which(cmd):
            return cmd, [cmd]
    # Inkscape — check PATH first, then well-known Windows install locations
    _inkscape_candidates = [
        shutil.which("inkscape") or "",
        r"C:\Program Files\Inkscape\bin\inkscape.exe",
        r"C:\Program Files (x86)\Inkscape\bin\inkscape.exe",
    ]
    for _ink in _inkscape_candidates:
        if _ink and Path(_ink).is_file():
            return "inkscape", [_ink]
    # ImageMagick 7 uses 'magick', ImageMagick 6 uses 'convert'
    magick = shutil.which("magick")
    if magick:
        return "convert", [magick, "convert"]
    convert = shutil.which("convert")
    if convert and _is_imagemagick_convert(convert):
        return "convert", [convert]
    # Last resort: headless Chrome/Edge (already used for Mermaid rendering)
    chrome = _find_chrome()
    if chrome:
        return "browser", [chrome]
    return None, []


def convert_svg_assets(
    text: str,
    md_path: Path,
    dpi: int = 150,
    verbose: bool = False,
) -> int:
    """Convert every .svg image reference in *text* to a .png beside the source file.

    The previous pipeline only rewrote ``.svg`` → ``.png`` in the Markdown text
    (svg_to_png_refs) but never converted the actual files, so pandoc silently
    dropped every SVG diagram.  This step closes that gap by rasterising each
    referenced SVG before the reference-rewriting step runs.

    Skips files whose .png counterpart already exists so re-runs are fast.
    Returns the count of SVGs newly converted.
    """
    tool, prefix = _find_svg_converter()
    if tool is None:
        print(
            "  WARNING: no SVG converter found — SVG images will be missing.\n"
            "           Install cairosvg:  pip install cairosvg",
            file=sys.stderr,
        )
        return 0

    src_dir = md_path.parent
    converted = 0
    seen: set[Path] = set()

    for ref in _SVG_REF_RE.findall(text):
        svg = (src_dir / ref).resolve()
        if svg in seen:
            continue
        seen.add(svg)

        if not svg.is_file():
            print(f"  WARNING: SVG not found: {svg}", file=sys.stderr)
            continue

        png = svg.with_suffix(".png")
        if png.exists():
            if verbose:
                print(f"  [svg] {svg.name} -> already exists, skipping")
            continue

        if verbose:
            print(f"  [svg] {svg.name} -> {png.name}  [{tool}]")

        try:
            if tool == "cairosvg":
                import cairosvg
                cairosvg.svg2png(url=str(svg), write_to=str(png), dpi=dpi)
            elif tool == "inkscape":
                subprocess.run(
                    [*prefix, "--export-type=png",
                     f"--export-dpi={dpi}",
                     f"--export-filename={png}", str(svg)],
                    check=True, capture_output=True,
                )
            elif tool == "rsvg-convert":
                subprocess.run(
                    [*prefix, "-d", str(dpi), "-p", str(dpi),
                     "-o", str(png), str(svg)],
                    check=True, capture_output=True,
                )
            elif tool == "browser":
                _render_svg_via_browser(svg, png, dpi=dpi, chrome=prefix[0])
            else:  # ImageMagick convert (prefix already contains full cmd)
                subprocess.run(
                    [*prefix, "-density", str(dpi), str(svg), str(png)],
                    check=True, capture_output=True,
                )
            converted += 1
        except Exception as exc:
            print(f"  WARNING: failed to convert {svg.name}: {exc}", file=sys.stderr)

    return converted


# ---------------------------------------------------------------------------
# Step 5  — Inconsistent pipe tables -> pandoc grid tables
# ---------------------------------------------------------------------------

_PIPE_SEP_RE = re.compile(r"^\|[\s\-:=|]+\|$")


def _has_leading_separator(block: list[str]) -> bool:
    """Return True when a pipe table block opens with a separator row before the header.

    The Document Change Log uses a non-standard format::

        |---------|------|---------|   ← leading separator  (pandoc can't parse this)
        | Version | Date | Changes |   ← actual header
        |---------|------|---------|   ← normal separator
        | 0.01    | …    | …       |

    Pandoc treats the leading separator as the *start* of the table and therefore
    produces no table at all.  We detect and strip the leading separator here so
    convert_tables() can normalise it.
    """
    if not block:
        return False
    return bool(_PIPE_SEP_RE.match(block[0].strip()))


def _strip_leading_separator(block: list[str]) -> list[str]:
    """Remove the leading separator row if present."""
    if _has_leading_separator(block):
        return block[1:]
    return block


def _is_inconsistent_pipe_table(block: list[str]) -> bool:
    """Return True only when *data* rows have mismatched pipe counts.

    The original implementation counted separator rows (``|---|---|``) alongside
    data rows.  Separator rows legitimately have a different number of ``|``
    characters (one per dash cell boundary plus two for the outer pipes), so
    virtually every valid table was flagged as "inconsistent" and needlessly
    converted to grid format.  Separator rows are now excluded before counting.

    Tables that open with a leading separator row are also flagged for conversion
    since pandoc cannot parse them (see _has_leading_separator).
    """
    if _has_leading_separator(block):
        return True
    data_lines = [
        line for line in block
        if line.strip() and not _PIPE_SEP_RE.match(line.strip())
    ]
    counts = [line.count("|") for line in data_lines]
    return len(set(counts)) > 1


def _pipe_table_to_grid(block: list[str]) -> list[str]:
    rows: list[list[str]] = []
    for line in block:
        stripped = line.strip()
        if re.match(r"^\|[\s\-:=|]+\|$", stripped):
            continue
        if stripped.startswith("|"):
            cells = [c.strip() for c in stripped.strip("|").split("|")]
            rows.append(cells)

    if not rows:
        return block

    num_cols = max(len(r) for r in rows)
    for row in rows:
        while len(row) < num_cols:
            row.append("")

    col_widths = [max(len(r[i]) for r in rows) for i in range(num_cols)]

    def sep(char: str) -> str:
        return "+" + "+".join(char * (w + 2) for w in col_widths) + "+"

    def fmt_row(cells: list[str]) -> str:
        padded = (cells + [""] * num_cols)[:num_cols]
        return "|" + "|".join(
            f" {padded[i].ljust(col_widths[i])} " for i in range(num_cols)
        ) + "|"

    grid = [sep("-"), fmt_row(rows[0]), sep("=")]
    for row in rows[1:]:
        grid.append(fmt_row(row))
        grid.append(sep("-"))
    return grid


def convert_tables(text: str) -> str:
    """Walk the markdown line-by-line and fix pipe tables that pandoc cannot parse.

    Two cases handled:
    - Inconsistent pipe counts across data rows → convert to grid table.
    - Leading separator row before the header → strip the spurious leading
      separator so pandoc can identify the header row correctly.
    """
    lines = text.splitlines()
    out: list[str] = []
    i = 0
    while i < len(lines):
        if lines[i].startswith("|"):
            block: list[str] = []
            while i < len(lines) and lines[i].startswith("|"):
                block.append(lines[i])
                i += 1
            if _is_inconsistent_pipe_table(block):
                # Strip any leading separator before converting to grid format
                out.extend(_pipe_table_to_grid(_strip_leading_separator(block)))
            else:
                out.extend(block)
        else:
            out.append(lines[i])
            i += 1
    return "\n".join(out)


# ---------------------------------------------------------------------------
# Step 6  — Strip redundant heading numbers
# ---------------------------------------------------------------------------

def strip_heading_numbers(text: str) -> str:
    """
    Remove leading section numbers from headings so Word's built-in outline
    numbering (from the reference doc's Heading styles) does not double up.

      ## 1. Executive Summary   ->  ## Executive Summary
      ### 1.1 Project Overview  ->  ### Project Overview
    """
    return HEADING_NUM_RE.sub(r"\1\3", text)


# ---------------------------------------------------------------------------
# Step 7  — Remove manual Table of Contents section
# ---------------------------------------------------------------------------

def remove_manual_toc(text: str) -> str:
    """
    Delete the hand-written TOC section from the Markdown.
    Pandoc --toc will insert a proper, hyperlinked Word TOC field instead.
    """
    cleaned = MANUAL_TOC_RE.sub("", text)
    # Collapse triple+ blank lines left behind
    cleaned = re.sub(r"\n{3,}", "\n\n", cleaned)
    return cleaned


# ---------------------------------------------------------------------------
# Step 8  — Write cleaned intermediate Markdown
# ---------------------------------------------------------------------------

def write_clean_md(text: str, original: Path) -> Path:
    clean_path = original.with_name(original.stem + "_clean.md")
    clean_path.write_text(text, encoding="utf-8")
    return clean_path


# ---------------------------------------------------------------------------
# Step 9  — Run pandoc
# ---------------------------------------------------------------------------

def run_pandoc(
    clean_md: Path,
    output_path: Path,
    fmt: str,
    reference_doc: Path | None,
    extra_args: list[str],
    toc: bool,
    verbose: bool,
) -> None:
    pandoc_to = PANDOC_FORMATS[fmt]["pandoc_to"]
    if pandoc_to is None:
        shutil.copy(clean_md, output_path)
        return

    cmd = [
        "pandoc",
        str(clean_md),
        "-f", "markdown+pipe_tables+grid_tables",
        "-t", pandoc_to,
        "-o", str(output_path),
        "--standalone",
        "--wrap=none",
    ]

    if fmt == "docx" and reference_doc:
        cmd += ["--reference-doc", str(reference_doc)]

    # Explicitly tell pandoc where to look for images.  Without this, pandoc
    # resolves relative paths from the process CWD, not the input file location.
    cmd += ["--resource-path", str(clean_md.parent)]

    if fmt == "html":
        cmd += ["--embed-resources", "--self-contained"]

    # Generate a proper Word TOC field with hyperlinks
    if toc and fmt == "docx":
        cmd += ["--toc", "--toc-depth=3"]

    cmd += extra_args

    if verbose:
        print("  [pandoc] " + " ".join(cmd))

    result = subprocess.run(cmd, capture_output=True, text=True)
    if result.returncode != 0:
        raise RuntimeError(
            f"pandoc failed (exit {result.returncode})\n"
            f"STDOUT:\n{result.stdout}\nSTDERR:\n{result.stderr}"
        )
    if result.stderr and verbose:
        print(result.stderr, file=sys.stderr)


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------

def build_parser() -> argparse.ArgumentParser:
    p = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    p.add_argument("input", metavar="INPUT.md", help="Source Markdown file")
    p.add_argument(
        "-o", "--output", metavar="OUTPUT",
        help="Output file path (default: INPUT with new extension)",
    )
    p.add_argument(
        "-f", "--format", metavar="FORMAT",
        choices=PANDOC_FORMATS.keys(),
        default="docx",
        help="Output format: docx (default), html, pdf, md",
    )
    p.add_argument(
        "--reference-doc", metavar="TEMPLATE.docx",
        help="Custom Word reference doc for styling (--format docx only)",
    )
    p.add_argument(
        "--skip-mermaid", action="store_true",
        help="Skip Mermaid rendering (assume PNGs already exist)",
    )
    p.add_argument(
        "--skip-svg-convert", action="store_true",
        help="Skip SVG-to-PNG conversion (assume PNGs already exist beside the source)",
    )
    p.add_argument(
        "--svg-dpi", type=int, default=150, metavar="DPI",
        help="DPI for SVG-to-PNG rasterisation (default: 150)",
    )
    p.add_argument(
        "--keep-heading-numbers", action="store_true",
        help="Keep leading section numbers in headings (default: strip them)",
    )
    p.add_argument(
        "--keep-manual-toc", action="store_true",
        help="Keep the manual TOC section (default: remove and use pandoc TOC)",
    )
    p.add_argument(
        "--no-toc", action="store_true",
        help="Skip pandoc --toc generation entirely",
    )
    p.add_argument(
        "--keep-clean-md", action="store_true",
        help="Keep the intermediate _clean.md file after conversion",
    )
    p.add_argument(
        "-v", "--verbose", action="store_true",
        help="Print each pipeline step",
    )
    # FIX: was argparse.REMAINDER which consumed our own flags.
    # Now uses nargs='*' so --skip-mermaid etc. are parsed correctly.
    p.add_argument(
        "extra", nargs="*",
        help="Extra arguments passed verbatim to pandoc",
    )
    return p


def main() -> int:
    args = build_parser().parse_args()

    md_path = Path(args.input).expanduser().resolve()
    if not md_path.is_file():
        print(f"Error: file not found: {md_path}", file=sys.stderr)
        return 1

    fmt = args.format
    ext = PANDOC_FORMATS[fmt]["ext"]
    output_path = (
        Path(args.output).expanduser().resolve()
        if args.output
        else md_path.with_suffix(ext)
    )
    reference_doc = Path(args.reference_doc).resolve() if args.reference_doc else None
    generate_toc = not args.no_toc

    print(f"Input : {md_path}")
    print(f"Output: {output_path}  (format: {fmt})")
    print()

    text = md_path.read_text(encoding="utf-8")

    # Step 1+2: Mermaid
    if args.skip_mermaid:
        print("[skip] Mermaid rendering skipped (--skip-mermaid)")
    else:
        chrome = _find_chrome()
        status = chrome if chrome else "WARNING: no Chrome found"
        print(f"[1/8] Rendering Mermaid diagrams  [{status}] ...")
        text = process_mermaid_blocks(text, md_path, verbose=args.verbose)

    # Step 2: Text diagrams -> PNG
    print("[2/8] Rendering boxed text diagrams ...")
    text = process_text_diagram_blocks(text, md_path, verbose=args.verbose)

    # Step 3: Convert SVG assets to PNG  ← NEW
    if args.skip_svg_convert:
        print("[skip] SVG-to-PNG conversion skipped (--skip-svg-convert)")
    else:
        print("[3/8] Converting SVG assets to PNG ...")
        n = convert_svg_assets(text, md_path, dpi=args.svg_dpi, verbose=args.verbose)
        if n:
            print(f"       Converted {n} SVG file(s)")

    # Step 4: Rewrite .svg refs to .png
    print("[4/8] Rewriting .svg -> .png references ...")
    text = svg_to_png_refs(text)

    # Step 5: Table normalisation
    print("[5/8] Normalising pipe tables -> grid tables ...")
    text = convert_tables(text)

    # Step 6: Strip heading numbers
    if args.keep_heading_numbers:
        print("[6/8] Keeping heading numbers (--keep-heading-numbers)")
    else:
        print("[6/8] Stripping redundant heading numbers ...")
        text = strip_heading_numbers(text)

    # Step 7: Remove manual TOC
    if args.keep_manual_toc:
        print("[7/8] Keeping manual TOC section (--keep-manual-toc)")
    else:
        print("[7/8] Removing manual TOC section ...")
        text = remove_manual_toc(text)

    # Step 8: Write cleaned Markdown
    clean_md = write_clean_md(text, md_path)
    print(f"       Intermediate file: {clean_md.name}")

    # Step 9: Pandoc
    if fmt == "md":
        shutil.copy(clean_md, output_path)
        print(f"[8/8] Copied cleaned Markdown -> {output_path}")
    else:
        toc_note = " +TOC" if (generate_toc and fmt == "docx") else ""
        print(f"[8/8] Running pandoc -> {output_path.name}{toc_note} ...")
        try:
            run_pandoc(
                clean_md, output_path, fmt,
                reference_doc, args.extra,
                toc=generate_toc,
                verbose=args.verbose,
            )
        except RuntimeError as exc:
            print(f"\nError: {exc}", file=sys.stderr)
            return 1

    if not args.keep_clean_md and fmt != "md":
        clean_md.unlink(missing_ok=True)

    print("\nDone.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
