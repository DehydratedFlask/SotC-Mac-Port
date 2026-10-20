# RE-02-DYNLINK-01 — synthetic dynamic-link parser fixture claim

- Date/time: 2026-10-19 23:16 CDT (America/Chicago).
- Integrator: `/root`; implementation owner after publication: `/root/re02_triage`.
- Task ID: RE-02-DYNLINK-01; parent RE-02.
- Files changed in claim: `HANDOFF.md`, `docs/TASKS.md`, and this checkpoint. No parser or test
  implementation changed at claim time.
- Failed tries: 0.

## Prerequisite evidence and bounded scope

The read-only triage found six existing tests for NID hashing, identifier widths, thunk lookup,
constructor walking, and bounds checks, but no end-to-end fixture that drives `parse()` with a
dynamic symbol and `JMPREL` relocation. Graft API navigation used
`graft skeleton scripts/ps4_dynlink.py`; the parser source remains a read-only dependency.

The selected private ELF identity is SHA-256
`d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`, size 65,214,308 bytes,
entry RVA `0xf120`. Existing manifests list 49 modules, 51 libraries, 588 relocated symbols, and
590 symbol-table entries. These values are context for the fixture, not assertions about runtime
loader behavior.

The worker owns only `scripts/test_ps4_dynlink.py`. Build one synthetic ELF containing a module /
library declaration, one imported symbol, and one `JMPREL` relocation. Assert the parser preserves
module/library encodings, symbol NID, relocation table/type, symbol index, target, and signed addend.
If this fixture reveals a concrete parser defect, report it to `/root` without changing
`scripts/ps4_dynlink.py` or widening the task.

## Acceptance and limits

Exact acceptance command: `python3 -I scripts/test_ps4_dynlink.py`.

A passing fixture demonstrates parsing of its synthetic bytes only. It does not establish runtime
symbol resolution, import ABI, addend semantics, or loader behavior. Failed implementation tries
remain 0. Next exact action: publish this claim, then assign the one owned test file and request the
exact command result and private log path.
