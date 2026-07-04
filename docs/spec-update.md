---
name: spec-update
description: >
  Conventions, formatting rules, and diagram standards for enhancing embedded systems and
  power-electronics software specifications. Use this skill whenever the user asks to update,
  improve, clean up, rewrite, or normalise a specification document (.md, .docx, pasted text).
  Trigger on phrases like "update my spec", "clean up my spec", "improve this spec",
  "apply spec conventions", "normalise my spec", or any request to enhance a technical
  specification document. Read this skill before touching the document.
---

# Spec Enhancement Conventions

Reference guide for formatting, language, diagrams, and structure when updating a specification.
**Do not invent requirements. Preserve all original technical intent.**

---

## 1. Document Structure — Section Order

Reorder sections to follow a **Basic → Advanced** progression based on the document's own domain.
The general pattern is:

- Context and scope first (summary, definitions, architecture overview)
- Hardware and physical behaviour before software behaviour
- Theory before implementation detail
- Normal operation before fault and edge-case handling
- Performance targets, testing, and appendices last

Merge or omit sections that do not apply. Split sections that cover more than one distinct topic.
Add a short introductory paragraph at the start of each major section explaining its purpose and
what the reader needs to know before reading it.

---

## 2. Normative Language

| Word | When to use |
|------|-------------|
| **shall** | Every mandatory requirement — must be atomic, unambiguous, testable |
| **should** | Recommendation or best practice |
| **may** | Permitted but not required |
| **is / are** | Descriptive / informative text only |

**Requirement quality rules:**

- **Atomic** — one testable condition per sentence.
- **Unambiguous** — no "as appropriate", "if necessary", "reasonable", "normally".
- **Traceable** — use §N.M cross-references, not vague "see above".

Separate normative requirements from informative explanations. Place the informative rationale
*before* the normative block, or use a `> **Informative:**` callout. Never mix rationale sentences
into a normative bullet list.

---

## 3. Diagram Conventions — ASCII → Mermaid

Replace every ASCII or text-box diagram with a Mermaid diagram. Choose type by content:

| Content | Mermaid type |
|---------|-------------|
| System block diagram, data flow, signal routing | `graph TD` or `graph LR` |
| State machine | `stateDiagram-v2` |
| Timing / sequencing / ISR call chain | `sequenceDiagram` |
| Simple pipeline / conversion chain | `graph LR` |

**Rules:**

- Place a `**Figure N-M: Descriptive Title**` caption on the line immediately before every fenced
  diagram block.
- Keep node labels short (≤ 6 words); put detail in surrounding prose.
- Delete the ASCII original once replaced — never leave both.
- If a diagram cannot be faithfully converted (e.g., analogue circuit schematic), replace it with:
  `*See hardware schematic: <filename>.*`

**Example caption format:**

```
**Figure 3-1: System Architecture Overview**

` ``mermaid
graph TD
  ...
` ``
```

---

## 4. Pseudocode and Algorithm Descriptions

Remove implementation-level constructs (loop syntax, variable declarations, temporary storage,
operator syntax) from normative sections. Express behaviour using one of these forms:

**Interface Table** — for function inputs and outputs:

| Element | Direction | Type | Description |
|---------|-----------|------|-------------|
| `signal_name` | Input | float | What it represents |
| `output_name` | Output | float | What the function writes |

**Step Table** — for initialization sequences and procedures:

| Step | Action | Notes / Constraint |
|------|--------|--------------------|
| 1 | What shall happen | Dependency or precondition |

**Condition Table** — for protection and fault-detection logic:

| Condition | Trigger | Action |
|-----------|---------|--------|
| `|signal| > LIMIT` | Overcurrent | Set fault flag; call shutdown function |

If the original pseudocode section is already labelled "Informative", it may be retained as a
structured interface skeleton — but add `> **This section is informative.**` and remove C-specific
syntax.

---

## 5. Markdown Cleanup

**Headings:** `#` = document title, `##` = major section, `###` = subsection, `####` = sub-subsection.
No skipped levels. Fix any non-sequential numbering (e.g., "§9A" → "§9.9").

**Tables:** Consistent column headers, aligned pipe separators, no ragged pipes.

**Lists in prose:** Write inline as "x, y, and z" unless there are 4+ items or items are long and
parallel — then use a bullet list.

**Bold:** Use for signal names, state names, and key terms on first use. Avoid bold for general
emphasis.

**Remove:**
- Encoding artefacts (e.g., garbled Unicode, `\xab` sequences)
- Trailing whitespace and consecutive blank lines (max one blank line between elements)
- Inconsistent punctuation (mixed dash styles, mixed quote styles)
- HTML tags — use standard Markdown equivalents
- Exotic Markdown extensions that do not survive Word/PDF export

---

## 6. Word / PDF Export Readiness

Optimise Markdown so it converts cleanly to Word or PDF:

- No raw HTML tags (`<br>`, `<div>`, `<span>`, etc.).
- No exotic extensions (custom directives, non-standard callout syntax).
- Use standard tables, simple bullet/numbered lists, inline bold/italic only.
- Every figure and table has a label/caption.
- Code blocks use standard triple-backtick fences with a language hint (` ```c `, ` ```mermaid `).

---

## 7. Readability and Pedagogy

- Add a short introductory paragraph at the start of each major section.
- Summarise complex algorithms in plain language before the detailed description.
- Group related requirements together under a single subsection heading.
- State assumptions, constraints, and numerical limits explicitly — not buried mid-paragraph.
- Use `> **Note:**` blockquotes for key warnings or architectural constraints.

---

## 8. Evidence, Citations, and Link Hygiene

Use this section when enhancing research or specification documents that contain externally verifiable claims.

### 8.1 Source Classes

| Source Class | Use For | Required Label |
|---|---|---|
| Standards body | Protocol, descriptor, electrical, safety, or compliance baseline | `Official standard` |
| Manufacturer manual/support | Public product behavior, setup, calibration, update, and warnings | `Manufacturer public docs` |
| Public open-source repository | Demonstrated implementation pattern or compatibility report | `Community implementation` |
| Patent/academic paper | Architecture background and historical context | `Reference context` |
| Forum/wiki/community note | Discovery or hypothesis only | `Community observation` |

### 8.2 Citation Rules

- Link material product claims to the exact source used.
- Prefer stable manuals, standards pages, and repository permalinks over search-result pages.
- Do not cite community reverse engineering as an official vendor specification.
- State confidence where source class affects certainty: `verified`, `observed`, `inferred`, or `unknown`.
- Keep source lists near the relevant subsystem section or in a final `References` section.
- Validate relative Markdown links after edits.

### 8.3 Link Text Rules

- Use descriptive link text, not raw URLs, except when documenting a URL itself.
- Keep internal links relative.
- Do not rename underscore-named legacy documents unless a dedicated migration updates every reference.

---

## 9. Optional Checks (flag but do not fix without asking)

- **Missing sections** — flag as `> **Suggested addition:** <section name> — currently absent.`
- **Ambiguous requirements** — flag inline as `[AMBIGUOUS: <reason>]`
- **Unverifiable requirements** — flag inline as `[TBD: needs acceptance criterion]`
- **Significant restructuring** — propose an updated Table of Contents at the end of the response.

---

## 10. Output and Change Log

- Bump the version number in the document header table.
- Add a row to the Document Change Log listing each category of change applied.
- When `docs/CN0XX-1_OBC_DCDC_Specification.md` is updated, append the change details to `docs/spec-updates.md`.
- Do **not** invent new requirements.
- Do **not** alter numerical values, signal names, or stated thresholds unless clearly typographical.
