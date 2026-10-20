# RE-02-PHDR8-SOURCE-AUDIT-01 — claim for program-header type `0x61000000`

- Time: 2026-10-20 01:25 CDT (America/Chicago).
- Agent / integrator: `/root`.
- Task: `RE-02-PHDR8-SOURCE-AUDIT-01`; parent RE-02; dependency app-main task is STATIC_ANALYSIS_VERIFIED.
- Status: ACTIVE; failed tries: 0; G1 remains NOT PASSED.
- Files changed: documentation only (`HANDOFF.md`, `docs/TASKS.md`, this checkpoint, and regenerated `docs/FOLDERS.md`).

## Input and observed header

Pinned plaintext ELF `local/re/eboot.elf` SHA-256:
`d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`.
The app-main verifier records PHDR #8 as raw `p_type=0x61000000`, `p_offset=0x2ddb100`,
`p_vaddr=0`, `p_filesz=0x1055ab0`, `p_memsz=0`, `p_flags=0x4`, `p_align=0x10`. It creates an
alternate file-backed view of VMA `0x25a8d0..0x25accc` at file offset `0x30359d0`, while the
executable bytes are validated through PT_LOAD #0 at `0x25e8d0`. The Sony/platform meaning and
runtime loader use of PHDR #8 are unknown.

The pinned `reference/shadPS4/` repository identity in `docs/INDEX.md` is
`0ebea15c24253b60f514fc37a348dabc8d0bcb90`. Graft query:

`graft ask "Which named PS4/Sony program-header types are handled in the pinned shadPS4 source, especially numeric type 0x61000000, and where are their semantics stated?" --source --in reference/shadPS4/`

exited `1` with “nothing indexed under `reference/shadPS4/` (or any path prefix).” This is an
index-scope limitation, not evidence that the source lacks a definition. Verify the live pin and
source tree before reading it; do not modify/fetch the reference checkout.

## Scope and acceptance

Search public first-party Sony/PlayStation technical sources for the exact number or an attributable
name and semantics. A pinned emulator/source implementation may be recorded as a comparison, but
must not be presented as authoritative Sony documentation. Record source identities, exact queries,
dates, result limits, and citations. If no attributable first-party mapping is available, conclude
only that the bounded search found none and retain `UNCLASSIFIED` for meaning/runtime relevance.
No game execution, firmware acquisition, credentialed documentation, binaries in shared docs, or
reference checkout changes. Keep report/logs private under
`local/re/evidence/re02-phdr8-source-audit/` and report exact commands to root for shared-doc updates.

Exact next action: dispatch a read-only source audit to a separate worker after this claim is
published; root integrates shared docs and evidence.
