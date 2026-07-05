#!/usr/bin/env python3
"""
docx_postprocess.py  —  XML surgery on pandoc-generated .docx files
=====================================================================
Plugs into the pipeline:

  Markdown
    |  md_to_pandoc.py        (preprocess + pandoc)
  raw.docx
    |  docx_postprocess.py    (this script)
  final.docx

Fixes applied (all optional, all on by default):
  1. cover_page       — style first H1 as cover-heading; page-break after cover table
  2. page_breaks      — <w:pageBreakBefore/> on every Heading2 (skip first)
  3. table_width      — expand every body table to full content width
  4. table_style      — shade header row; apply table-heading / table-body styles
  5. image_centre     — centre paragraphs that contain an inline drawing
  6. toc_heading      — restyle the TOC heading to match the reference doc
  7. toc_hyperlinks   — add \\h flag to TOC field so entries are clickable
  8. caption_style    — restyle bold **Figure N-M:** paragraphs as Caption
  9. header_footer    — copy settings.xml from the reference doc so
                        evenAndOddHeaders and titlePg are properly enabled

Usage
-----
  python docx_postprocess.py INPUT.docx                   # -> INPUT_final.docx
  python docx_postprocess.py INPUT.docx -o OUTPUT.docx
  python docx_postprocess.py INPUT.docx -r my_style.docx  # reference doc for settings
  python docx_postprocess.py INPUT.docx --skip table_width image_centre
  python docx_postprocess.py INPUT.docx --header-fill 1F4D78 --header-text FFFFFF
  python docx_postprocess.py INPUT.docx -v

Requirements
------------
  lxml     pip install lxml
"""

from __future__ import annotations

import argparse
import copy
import re
import sys
import zipfile
from collections.abc import Collection
from pathlib import Path
from typing import Optional

from lxml import etree

# ── XML namespace map ─────────────────────────────────────────────────────────

NS = {
    "w":   "http://schemas.openxmlformats.org/wordprocessingml/2006/main",
    "r":   "http://schemas.openxmlformats.org/officeDocument/2006/relationships",
    "wp":  "http://schemas.openxmlformats.org/drawingml/2006/wordprocessingDrawing",
    "a":   "http://schemas.openxmlformats.org/drawingml/2006/main",
    "mc":  "http://schemas.openxmlformats.org/markup-compatibility/2006",
    "w14": "http://schemas.microsoft.com/office/word/2010/wordml",
}

W = NS["w"]


def wtag(local: str) -> str:
    return f"{{{W}}}{local}"


def wval(el: etree._Element, local: str) -> Optional[str]:
    return el.get(wtag(local))


# ── Low-level XML helpers ─────────────────────────────────────────────────────

def get_or_create(parent: etree._Element, tag: str) -> etree._Element:
    """Return the first child matching *tag*, creating it if absent.

    The new element is appended (not prepended) so that callers which need a
    specific position within the parent can control insertion order themselves.
    Prepending to index 0 violated the OOXML schema ordering for <w:pPr>
    (required: pStyle → pageBreakBefore → numPr → spacing → ind → jc → rPr).
    """
    child = parent.find(tag, NS)
    if child is None:
        child = etree.SubElement(parent, tag)
    return child


def ensure_pPr(para: etree._Element) -> etree._Element:
    pPr = para.find(wtag("pPr"))
    if pPr is None:
        pPr = etree.Element(wtag("pPr"))
        para.insert(0, pPr)
    return pPr


def ensure_tcPr(tc: etree._Element) -> etree._Element:
    tcPr = tc.find(wtag("tcPr"))
    if tcPr is None:
        tcPr = etree.Element(wtag("tcPr"))
        tc.insert(0, tcPr)
    return tcPr


def get_pStyle(para: etree._Element) -> Optional[str]:
    pPr = para.find(wtag("pPr"))
    if pPr is None:
        return None
    pStyle = pPr.find(wtag("pStyle"))
    if pStyle is None:
        return None
    return pStyle.get(wtag("val"))


def set_pStyle(para: etree._Element, style_id: str) -> None:
    pPr = ensure_pPr(para)
    pStyle = pPr.find(wtag("pStyle"))
    if pStyle is None:
        pStyle = etree.Element(wtag("pStyle"))
        pPr.insert(0, pStyle)
    pStyle.set(wtag("val"), style_id)


def has_drawing(para: etree._Element) -> bool:
    return para.find(f".//{wtag('drawing')}") is not None


def paragraph_text(para: etree._Element) -> str:
    return "".join(t.text or "" for t in para.iter(wtag("t")))


def resolve_input_docx(input_path: Path) -> Path:
    """
    Accept either a .docx file directly or a Markdown source file whose
    pandoc-generated .docx sits beside it.
    """
    suffix = input_path.suffix.lower()

    if suffix == ".docx":
        return input_path

    if suffix in {".md", ".markdown"}:
        candidate = input_path.with_suffix(".docx")
        if candidate.is_file():
            return candidate
        raise FileNotFoundError(
            f"No generated DOCX found beside Markdown source: {candidate}"
        )

    raise ValueError(
        "Input must be a .docx file, or a .md/.markdown file with a sibling .docx"
    )


# ── Page geometry ─────────────────────────────────────────────────────────────

def _int(el: etree._Element, attr: str, default: int = 0) -> int:
    v = el.get(wtag(attr))
    return int(v) if v is not None else default


def read_page_geometry(body: etree._Element) -> dict:
    sectPr = body.find(wtag("sectPr"))
    if sectPr is None:
        return {"w": 11906, "h": 16838,
                "top": 1440, "bottom": 1440, "left": 1440, "right": 1440,
                "content_w": 9026, "content_h": 13958}

    pgSz  = sectPr.find(wtag("pgSz"))
    pgMar = sectPr.find(wtag("pgMar"))

    w = _int(pgSz, "w",  11906) if pgSz  is not None else 11906
    h = _int(pgSz, "h",  16838) if pgSz  is not None else 16838
    left   = _int(pgMar, "left",   1440) if pgMar is not None else 1440
    right  = _int(pgMar, "right",  1440) if pgMar is not None else 1440
    top    = _int(pgMar, "top",    1440) if pgMar is not None else 1440
    bottom = _int(pgMar, "bottom", 1440) if pgMar is not None else 1440

    return {
        "w": w, "h": h,
        "top": top, "bottom": bottom, "left": left, "right": right,
        "content_w": w - left - right,
        "content_h": h - top - bottom,
    }


# ── DocxPostProcessor ─────────────────────────────────────────────────────────

class DocxPostProcessor:
    """
    Load a .docx, apply XML surgery, save result.
    All fix_* methods are independent and can be called in any order.
    """

    def __init__(self, docx_path: Path, verbose: bool = False):
        self.path    = docx_path
        self.verbose = verbose

        try:
            with zipfile.ZipFile(docx_path) as z:
                self._files: dict[str, bytes] = {n: z.read(n) for n in z.namelist()}
        except zipfile.BadZipFile as exc:
            raise ValueError(
                f"Input is not a valid .docx/ZIP file: {docx_path}"
            ) from exc

        self._doc_tree = etree.fromstring(self._files["word/document.xml"])
        self._body     = self._doc_tree.find(wtag("body"))
        self._geometry = read_page_geometry(self._body)
        self._log(f"Page content width: {self._geometry['content_w']} DXA "
                  f"({self._geometry['content_w'] / 1440:.2f} in)")

    def _log(self, msg: str) -> None:
        if self.verbose:
            print(f"  {msg}")

    # ── Fix 1: Cover page ─────────────────────────────────────────────────────

    def fix_cover_page(
        self,
        cover_heading_style: str = "cover-heading",
        title_style: str = "z-title",
    ) -> None:
        """
        Turn the first Heading1 paragraph into a proper cover heading, and
        insert a page break after the first table (the document-metadata table
        that immediately follows the title).

        Layout produced:
          [Cover page]  Title + metadata table
          [Page break]
          [Body]        TOC, then section 1 onwards
        """
        body_children = list(self._body)

        # --- Style the first Heading1 as cover-heading -----------------------
        first_h1 = next(
            (p for p in self._body.iter(wtag("p")) if get_pStyle(p) == "Heading1"),
            None,
        )
        if first_h1 is not None:
            set_pStyle(first_h1, cover_heading_style)
            self._log(f"fix_cover_page: restyled first H1 -> '{cover_heading_style}'")
        else:
            self._log("fix_cover_page: no Heading1 found, skipping title restyle")

        # --- Find the first table (metadata table) and insert page break ------
        first_tbl = next(
            (el for el in body_children if el.tag == wtag("tbl")),
            None,
        )
        if first_tbl is None:
            self._log("fix_cover_page: no table found for page break insertion")
            return

        # Insert a page-break paragraph immediately after the metadata table.
        # Guard against re-runs: if the element right after the table is already
        # a page-break paragraph, skip insertion to stay idempotent.
        body_list = list(self._body)
        tbl_idx   = body_list.index(first_tbl)
        next_el   = body_list[tbl_idx + 1] if tbl_idx + 1 < len(body_list) else None
        already_has_break = (
            next_el is not None
            and next_el.tag == wtag("p")
            and next_el.find(f".//{wtag('br')}") is not None
        )
        if already_has_break:
            self._log("fix_cover_page: page break already present, skipping insertion")
            return

        pb_para = etree.Element(wtag("p"))
        pb_pPr  = etree.SubElement(pb_para, wtag("pPr"))
        pb_pSt  = etree.SubElement(pb_pPr,  wtag("pStyle"))
        pb_pSt.set(wtag("val"), "Normal")
        pb_r    = etree.SubElement(pb_para, wtag("r"))
        pb_br   = etree.SubElement(pb_r,    wtag("br"))
        pb_br.set(wtag("type"), "page")
        self._body.insert(tbl_idx + 1, pb_para)
        self._log("fix_cover_page: inserted page break after metadata table")

    # ── Fix 2: Page breaks before Heading2 ───────────────────────────────────

    def fix_page_breaks(
        self,
        heading_style: str = "Heading2",
        skip_first: bool = True,
    ) -> None:
        """
        Add <w:pageBreakBefore/> to every paragraph using heading_style.

        heading_style defaults to "Heading2" because the source markdown uses
        ## for top-level sections (not # which maps to Heading1 / doc title).

        skip_first=True leaves the very first match without a page break
        (it immediately follows the TOC page, so no break needed).
        """
        paras = [
            p for p in self._body.iter(wtag("p"))
            if get_pStyle(p) == heading_style
        ]
        start = 1 if skip_first else 0
        count = 0
        for para in paras[start:]:
            pPr = ensure_pPr(para)
            if pPr.find(wtag("pageBreakBefore")) is None:
                pb = etree.Element(wtag("pageBreakBefore"))
                pStyle_el = pPr.find(wtag("pStyle"))
                insert_at = (list(pPr).index(pStyle_el) + 1
                             if pStyle_el is not None else 0)
                pPr.insert(insert_at, pb)
                count += 1
        self._log(
            f"fix_page_breaks: added pageBreakBefore to {count} "
            f"'{heading_style}' paragraphs (skipped first={skip_first})"
        )

    # ── Fix 3: Table full width ───────────────────────────────────────────────

    def fix_table_width(self) -> None:
        content_w = self._geometry["content_w"]
        count = 0

        for tbl in self._body.iter(wtag("tbl")):
            tblPr = tbl.find(wtag("tblPr"))
            if tblPr is None:
                continue
            tblStyle = tblPr.find(wtag("tblStyle"))
            if tblStyle is not None and tblStyle.get(wtag("val")) == "FigureTable":
                continue

            tblW = tblPr.find(wtag("tblW"))
            if tblW is None:
                tblW = etree.SubElement(tblPr, wtag("tblW"))
            tblW.set(wtag("w"),    str(content_w))
            tblW.set(wtag("type"), "dxa")

            jc_el = tblPr.find(wtag("jc"))
            if jc_el is not None:
                tblPr.remove(jc_el)

            tblGrid = tbl.find(wtag("tblGrid"))
            if tblGrid is not None:
                grid_cols = tblGrid.findall(wtag("gridCol"))
                n_cols = len(grid_cols)
                if n_cols > 0:
                    # Preserve original column proportions where available.
                    # If Pandoc left all widths at 0, fall back to equal split.
                    orig_widths = []
                    for col in grid_cols:
                        v = col.get(wtag("w"))
                        orig_widths.append(int(v) if v else 0)

                    total_orig = sum(orig_widths)
                    if total_orig > 0:
                        # Distribute proportionally, fix rounding on last col
                        col_widths = [
                            round(content_w * w / total_orig) for w in orig_widths
                        ]
                        col_widths[-1] = content_w - sum(col_widths[:-1])
                    else:
                        # Equal split (Pandoc provided no width hints)
                        base   = content_w // n_cols
                        remain = content_w - base * n_cols
                        col_widths = [base + (1 if i < remain else 0)
                                      for i in range(n_cols)]

                    for col, w in zip(grid_cols, col_widths):
                        col.set(wtag("w"), str(w))
                    for tr in tbl.findall(wtag("tr")):
                        for ci, tc in enumerate(tr.findall(wtag("tc"))):
                            if ci >= len(col_widths):
                                break
                            tcPr = ensure_tcPr(tc)
                            tcW  = tcPr.find(wtag("tcW"))
                            if tcW is None:
                                tcW = etree.SubElement(tcPr, wtag("tcW"))
                            tcW.set(wtag("w"),    str(col_widths[ci]))
                            tcW.set(wtag("type"), "dxa")
            count += 1

        self._log(f"fix_table_width: updated {count} table(s) to {content_w} DXA")

    # ── Fix 4: Table header shading + styles ──────────────────────────────────

    def fix_table_style(
        self,
        header_fill:  str = "2A289D",
        header_text:  str = "FFFFFF",
        body_style:   str = "table-body",
        header_style: str = "table-heading",
    ) -> None:
        tables_done = 0

        for tbl in self._body.iter(wtag("tbl")):
            tblPr = tbl.find(wtag("tblPr"))
            tblStyle_el = (tblPr.find(wtag("tblStyle")) if tblPr is not None else None)
            if (tblStyle_el is not None
                    and tblStyle_el.get(wtag("val")) == "FigureTable"):
                continue

            rows = tbl.findall(wtag("tr"))
            for row_idx, tr in enumerate(rows):
                trPr    = tr.find(wtag("trPr"))
                is_hdr  = (trPr is not None
                           and trPr.find(wtag("tblHeader")) is not None)
                is_first = row_idx == 0

                for tc in tr.findall(wtag("tc")):
                    tcPr = ensure_tcPr(tc)

                    if is_hdr or is_first:
                        shd = tcPr.find(wtag("shd"))
                        if shd is None:
                            shd = etree.SubElement(tcPr, wtag("shd"))
                        shd.set(wtag("val"),   "clear")
                        shd.set(wtag("color"), "auto")
                        shd.set(wtag("fill"),  header_fill)

                        for p in tc.findall(wtag("p")):
                            set_pStyle(p, header_style)
                            pPr = p.find(wtag("pPr"))
                            if pPr is not None:
                                jc = pPr.find(wtag("jc"))
                                if jc is not None:
                                    pPr.remove(jc)
                            for r in p.findall(wtag("r")):
                                rPr = r.find(wtag("rPr"))
                                if rPr is None:
                                    rPr = etree.Element(wtag("rPr"))
                                    r.insert(0, rPr)
                                color = rPr.find(wtag("color"))
                                if color is None:
                                    color = etree.SubElement(rPr, wtag("color"))
                                color.set(wtag("val"), header_text)
                    else:
                        for p in tc.findall(wtag("p")):
                            set_pStyle(p, body_style)
                            pPr = p.find(wtag("pPr"))
                            if pPr is not None:
                                jc = pPr.find(wtag("jc"))
                                if jc is not None:
                                    pPr.remove(jc)

            tables_done += 1

        self._log(f"fix_table_style: styled {tables_done} table(s) "
                  f"(header fill #{header_fill}, body='{body_style}')")

    # ── Fix 5: Centre image paragraphs ────────────────────────────────────────

    def fix_image_centre(self) -> None:
        count = 0
        for para in self._body.iter(wtag("p")):
            if has_drawing(para):
                pPr = ensure_pPr(para)
                jc  = pPr.find(wtag("jc"))
                if jc is None:
                    jc = etree.SubElement(pPr, wtag("jc"))
                jc.set(wtag("val"), "center")
                count += 1

        for tbl in self._body.iter(wtag("tbl")):
            tblPr = tbl.find(wtag("tblPr"))
            if tblPr is None:
                continue
            tblStyle_el = tblPr.find(wtag("tblStyle"))
            if (tblStyle_el is not None
                    and tblStyle_el.get(wtag("val")) == "FigureTable"):
                for p in tbl.iter(wtag("p")):
                    pPr = ensure_pPr(p)
                    jc  = pPr.find(wtag("jc"))
                    if jc is None:
                        jc = etree.SubElement(pPr, wtag("jc"))
                    jc.set(wtag("val"), "center")
                    count += 1

        self._log(f"fix_image_centre: centred {count} image paragraph(s)")

    # ── Fix 6: TOC heading style ──────────────────────────────────────────────

    def fix_toc_heading(self, toc_title_style: str = "toc-title") -> None:
        """Restyle TOCHeading to the reference doc's toc-title style."""
        count = 0
        for para in self._body.iter(wtag("p")):
            if get_pStyle(para) == "TOCHeading":
                set_pStyle(para, toc_title_style)
                count += 1
        self._log(f"fix_toc_heading: restyled {count} TOC heading(s) -> '{toc_title_style}'")

    # ── Fix 7: TOC hyperlinks ─────────────────────────────────────────────────

    def fix_toc_hyperlinks(self) -> None:
        """
        Ensure the Word TOC field instruction contains the \\h flag so that
        entries become clickable hyperlinks, and set dirty=true so Word
        rebuilds the TOC with correct page numbers on first open.

        Bug fixed: fldChar lives inside <w:r>, so fldChar.getparent() returns
        the *run* element, not the paragraph.  The instrText is in a *sibling*
        run, so we must go up one more level to the <w:p> before iterating.
        """
        updated = 0
        for fldChar in self._body.iter(wtag("fldChar")):
            if fldChar.get(wtag("fldCharType")) != "begin":
                continue
            # fldChar → w:r → w:p  (two levels up to reach the paragraph)
            parent_r = fldChar.getparent()
            if parent_r is None:
                continue
            parent_p = parent_r.getparent()
            if parent_p is None:
                continue
            for instr in parent_p.iter(wtag("instrText")):
                txt = instr.text or ""
                if "TOC" not in txt:
                    continue
                # Add \h flag if missing (enables hyperlinks)
                if r"\h" not in txt:
                    instr.text = txt.rstrip() + r" \h"
                # Mark dirty so Word recomputes page numbers
                fldChar.set(wtag("dirty"), "true")
                updated += 1
                break
        self._log(f"fix_toc_hyperlinks: updated {updated} TOC field(s) with \\h + dirty=true")

    # ── Fix 8: Caption style ─────────────────────────────────────────────────

    def fix_caption_style(
        self,
        caption_style: str = "Caption",
        pattern: str = r"^\*\*Figure\s+\d+|^\*\*Table\s+\d+|^Figure\s+\d+|^Table\s+\d+",
    ) -> None:
        rx  = re.compile(pattern, re.IGNORECASE)
        count = 0
        for para in self._body.iter(wtag("p")):
            txt = paragraph_text(para)
            txt_clean = txt.lstrip("*").strip()
            if rx.match(txt_clean) or rx.match(txt):
                set_pStyle(para, caption_style)
                for rPr in para.iter(wtag("rPr")):
                    for bold_tag in (wtag("b"), wtag("bCs")):
                        b = rPr.find(bold_tag)
                        if b is not None:
                            rPr.remove(b)
                count += 1
        self._log(f"fix_caption_style: restyled {count} caption paragraph(s)")

    # ── Fix 10: Remove automatic outline numbering from Heading styles ────────

    def fix_heading_numbering(
        self,
        heading_style_ids: tuple[str, ...] = (
            "Heading1", "Heading2", "Heading3", "Heading4", "Heading5",
        ),
    ) -> None:
        """Remove automatic outline numbering inherited from the reference doc's
        Heading styles.

        Root cause: ``my_style.docx`` (and many corporate templates) assign a
        multilevel decimal ``numId`` to Heading1–5 via the style definition in
        ``word/styles.xml``.  When the document body contains no ``Heading1``
        paragraph (the only ``#`` heading was restyled to ``cover-heading``),
        the level-0 counter never advances, so every ``Heading2`` renders as
        ``1.1``, ``1.2`` … instead of ``1.``, ``2.`` …  — all starting with "1.".

        The fix removes ``<w:numPr>`` from the ``<w:pPr>`` of each named
        heading style in the output ``word/styles.xml``.  It does NOT touch
        individual paragraph ``<w:pPr>`` blocks in the document body, because
        pandoc does not inject inline ``numPr`` on heading paragraphs — the
        numbering comes entirely from the style definition.
        """
        styles_bytes = self._files.get("word/styles.xml")
        if styles_bytes is None:
            self._log("fix_heading_numbering: no styles.xml found, skipping")
            return

        try:
            styles_tree = etree.fromstring(styles_bytes)
        except etree.XMLSyntaxError as exc:
            self._log(f"fix_heading_numbering: cannot parse styles.xml: {exc}")
            return

        removed = 0
        for style in styles_tree.findall(wtag("style")):
            sid = style.get(wtag("styleId"), "")
            if sid not in heading_style_ids:
                continue
            pPr = style.find(wtag("pPr"))
            if pPr is None:
                continue
            numPr = pPr.find(wtag("numPr"))
            if numPr is not None:
                pPr.remove(numPr)
                removed += 1

        if removed:
            self._files["word/styles.xml"] = etree.tostring(
                styles_tree, xml_declaration=True, encoding="UTF-8", standalone=True
            )
        self._log(
            f"fix_heading_numbering: removed numPr from {removed} heading style(s)"
        )

    # ── Fix 11: Remove auto-generated alt-text caption paragraphs ─────────────

    def fix_remove_autogen_captions(
        self,
        patterns: tuple[str, ...] = (
            r"^Mermaid diagram \d+",
            r"^Mermaid diagram \d+ \(render failed\)",
            r"^Boxed text diagram \d+",
            r"^OBC DC-DC Hardware .*",
            r"^OBC .+ PWM timing.*",
            r"^DCDC PSU phase-shift cascade.*",
            r"^OBC .+ VAC polarity transition.*",
        ),
    ) -> None:
        """Remove paragraphs that contain only pandoc-generated image alt text.

        When an image is written as ![Alt text](img.png) in Markdown, pandoc
        emits the alt text as a visible caption paragraph in the DOCX body.
        For auto-generated names like "Mermaid diagram 8" this is redundant
        where a proper Figure caption already exists.
        """
        rxs = [re.compile(p, re.IGNORECASE) for p in patterns]
        to_remove: list[etree._Element] = []

        for para in self._body.iter(wtag("p")):
            txt = paragraph_text(para).strip()
            if any(rx.fullmatch(txt) for rx in rxs):
                to_remove.append(para)

        for para in to_remove:
            parent = para.getparent()
            if parent is not None:
                parent.remove(para)

        self._log(f"fix_remove_autogen_captions: removed {len(to_remove)} paragraph(s)")

    # ── Fix 9: Header / footer settings ──────────────────────────────────────

    def fix_header_footer(self, reference_docx: Path | None = None) -> None:
        """
        Ensure the document settings enable the header and footer defined
        in the reference doc.

        Two complementary sub-fixes:
        a) Copy settings.xml from the reference doc (preserves evenAndOddHeaders,
           titlePg, and other flags that control header/footer visibility).
        b) If no reference doc is available, inject the two most common
           missing flags directly into the existing settings.xml.
        """
        if reference_docx is not None and reference_docx.is_file():
            try:
                with zipfile.ZipFile(reference_docx) as ref_zip:
                    if "word/settings.xml" in ref_zip.namelist():
                        self._files["word/settings.xml"] = ref_zip.read("word/settings.xml")
                        self._log(
                            f"fix_header_footer: copied settings.xml from {reference_docx.name}"
                        )
                        return
            except Exception as exc:
                self._log(f"fix_header_footer: could not read reference doc: {exc}")

        # Fallback: patch the existing settings.xml
        settings_bytes = self._files.get("word/settings.xml")
        if settings_bytes is None:
            self._log("fix_header_footer: no settings.xml found, skipping")
            return

        try:
            settings = etree.fromstring(settings_bytes)
        except etree.XMLSyntaxError as exc:
            self._log(f"fix_header_footer: cannot parse settings.xml: {exc}")
            return

        changed = False

        # <w:evenAndOddHeaders/> – show different headers on even/odd pages
        if settings.find(wtag("evenAndOddHeaders")) is None:
            el = etree.SubElement(settings, wtag("evenAndOddHeaders"))
            settings.insert(0, el)
            changed = True

        # <w:titlePg/> in the body sectPr – honour the "first page" header/footer
        sectPr = self._body.find(wtag("sectPr"))
        if sectPr is not None and sectPr.find(wtag("titlePg")) is None:
            title_pg = etree.Element(wtag("titlePg"))
            sectPr.insert(0, title_pg)
            changed = True

        if changed:
            self._files["word/settings.xml"] = etree.tostring(
                settings, xml_declaration=True, encoding="UTF-8", standalone=True
            )
            self._log("fix_header_footer: injected evenAndOddHeaders + titlePg")
        else:
            self._log("fix_header_footer: settings already correct, no changes")

    # ── Orchestrator ──────────────────────────────────────────────────────────

    def run(
        self,
        skip: Collection[str] | None = None,
        header_fill: str = "2A289D",
        header_text: str = "FFFFFF",
        reference_docx: Path | None = None,
    ) -> None:
        """
        Run all fixes in order. Pass names in `skip` to disable individual fixes.

        Available fix names:
          cover_page, page_breaks, table_width, table_style, image_centre,
          toc_heading, toc_hyperlinks, caption_style, header_footer,
          heading_numbering
        """
        skip = set(skip or [])

        steps = [
            ("cover_page",        lambda: self.fix_cover_page()),
            ("page_breaks",       lambda: self.fix_page_breaks()),
            ("table_width",       lambda: self.fix_table_width()),
            ("table_style",       lambda: self.fix_table_style(header_fill, header_text)),
            ("image_centre",      lambda: self.fix_image_centre()),
            ("toc_heading",       lambda: self.fix_toc_heading()),
            ("toc_hyperlinks",    lambda: self.fix_toc_hyperlinks()),
            ("caption_style",          lambda: self.fix_caption_style()),
            ("remove_autogen_captions", lambda: self.fix_remove_autogen_captions()),
            ("header_footer",           lambda: self.fix_header_footer(reference_docx)),
            ("heading_numbering", lambda: self.fix_heading_numbering()),
        ]

        for name, fn in steps:
            if name in skip:
                self._log(f"[SKIP] {name}")
                continue
            if self.verbose:
                print(f"  [{name}]")
            fn()

    # ── Serialise and save ────────────────────────────────────────────────────

    def save(self, out_path: Path) -> None:
        xml_bytes = etree.tostring(
            self._doc_tree,
            xml_declaration=True,
            encoding="UTF-8",
            standalone=True,
        )
        self._files["word/document.xml"] = xml_bytes

        with zipfile.ZipFile(out_path, "w", zipfile.ZIP_DEFLATED) as z:
            for name, data in self._files.items():
                z.writestr(name, data)

        self._log(f"Saved -> {out_path}")


# ── CLI ───────────────────────────────────────────────────────────────────────

def build_parser() -> argparse.ArgumentParser:
    p = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    p.add_argument("input", metavar="INPUT.docx",
                   help="Pandoc-generated .docx to post-process")
    p.add_argument("-o", "--output", metavar="OUTPUT.docx",
                   help="Output path (default: INPUT_final.docx)")
    p.add_argument(
        "-r", "--reference-doc", metavar="STYLE.docx",
        help="Reference .docx used for conversion (supplies settings.xml for headers/footers)",
    )
    p.add_argument(
        "--skip", nargs="+", metavar="FIX",
        choices=[
            "cover_page", "page_breaks", "table_width", "table_style",
            "image_centre", "toc_heading", "toc_hyperlinks",
            "caption_style", "remove_autogen_captions", "header_footer", "heading_numbering",
        ],
        default=[],
        help="Fix names to skip",
    )
    p.add_argument(
        "--header-fill", metavar="RRGGBB", default="2A289D",
        help="Table header row background colour (hex, no #). Default: 2A289D",
    )
    p.add_argument(
        "--header-text", metavar="RRGGBB", default="FFFFFF",
        help="Table header row text colour (hex, no #). Default: FFFFFF",
    )
    p.add_argument("-v", "--verbose", action="store_true")
    return p


def main() -> int:
    args = build_parser().parse_args()

    in_path = Path(args.input).expanduser().resolve()
    if not in_path.is_file():
        print(f"Error: not found: {in_path}", file=sys.stderr)
        return 1

    try:
        source_docx = resolve_input_docx(in_path)
    except (FileNotFoundError, ValueError) as exc:
        print(f"Error: {exc}", file=sys.stderr)
        return 1

    out_path = (
        Path(args.output).expanduser().resolve()
        if args.output
        else in_path.with_name(in_path.stem + "_final.docx")
    )

    reference_docx = (
        Path(args.reference_doc).resolve()
        if args.reference_doc else None
    )

    print(f"Input : {in_path}")
    if source_docx != in_path:
        print(f"DOCX  : {source_docx}")
    print(f"Output: {out_path}")
    if reference_docx:
        print(f"Ref   : {reference_docx}")
    print()

    try:
        proc = DocxPostProcessor(source_docx, verbose=args.verbose)
    except ValueError as exc:
        print(f"Error: {exc}", file=sys.stderr)
        return 1

    proc.run(
        skip=args.skip,
        header_fill=args.header_fill,
        header_text=args.header_text,
        reference_docx=reference_docx,
    )
    proc.save(out_path)

    print("\nDone.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
