# Startup slot 314 preliminary static dossier

Local date/time: 2026-10-27 00:02 CDT (America/Chicago). Integrator: `/root`; analysis: `/root/trail_scope_audit`.

Task: `CPU-01-STARTUP-SLOT-314-CHARACTERIZATION-01`.

## Verified artifacts

The pinned ELF SHA-256 is
`d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`. The live original
constructor table row is `{314, 0xa41810}` at `native/core/ctor_original_table.cpp:328`.
`python3 -I tools/lift/check_ctor_slots.py` and root replay
`python3 -I tools/lift/analyze_slot_struct.py 0xa41810 --label slot314` both exited 0.

The private worker analyzer JSON SHA-256 is
`e0a89887595317021c4f8a957679d8c060555fef8047b6eded92b5e81056b7fa`; root replay has the same
SHA-256 and matching full direct-call and RIP-reference arrays. The analyzer reports span
`[0xa41810,0xa428f8)` (**4,328 bytes**), body SHA-256
`3e051b27f677728ea6b2d6dfe2bf6be74873270bb358c2eb8e1871d0300f63e0`, **779** reachable
instructions, **84** direct-call sites across 9 targets, one conditional edge
`0xa428e2 -> 0xa428f3`, **8 unresolved indirect calls**, **196** RIP-relative references (**63**
store instructions), and zero outside-image references.

Worker report: `local/validation/orch-20261026/slot314/REPORT.md`, SHA-256
`b84ee1079b3bd73014980ff1491cc9e0be47f54c313e1b127f194f7836a1dcfa`. Private analyzer and root
replay files are under `local/validation/orch-20261026/slot314/`. No source files changed.

## Limits and status

This is analyzer-only static evidence. It does not establish CFG completeness, branch outcomes,
indirect targets, callee behavior, object semantics, or native-lift feasibility. No REA/Ghidra,
executable, emulator, or runtime was used. G1 remains **PARTIAL**.

Status: **STATIC_ANALYSIS_VERIFIED** for the bounded table/analyzer characterization; failed
semantic hypotheses: **0**. All eight computed calls remain unresolved.

Exact next action: run the separately claimed slot 316 analyzer pass. Do not register or implement
slot 314 from this dossier alone.
