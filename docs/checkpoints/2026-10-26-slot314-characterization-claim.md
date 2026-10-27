# Startup slot 314 characterization claim

Local date/time: 2026-10-26 23:55 CDT (America/Chicago). Agent: `/root`.

Task: `CPU-01-STARTUP-SLOT-314-CHARACTERIZATION-01`.

## Bounded objective

Verify constructor-table index **314** / pinned RVA **`0xa41810`** against the live table and
`local/re/eboot.elf`; then run the existing read-only slot analyzer. Record the body span/hash,
reachable instruction and branch inventory, direct-call sites/targets, RIP-relative references,
and indirect-call sites. The live row is `{314, 0xa41810}` at
`native/core/ctor_original_table.cpp:328`; the pinned ELF digest is
`d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`.

This pass records analyzer output only. Do not infer call semantics, CFG completeness, or
native-lift feasibility from the analyzer summary. Do not make shared source/docs edits. Keep
artifacts private under `local/validation/orch-20261026/slot314/`. Do not run the original
executable or use any emulator, guest interpreter/JIT, or Sony runtime.

## Status and next action

Status is **ACTIVE / preliminary static analysis**; failed semantic hypotheses: **0**. Exact next
action: `/root/trail_scope_audit` verifies the table row and ELF digest, runs
`python3 -I tools/lift/analyze_slot_struct.py 0xa41810 --label slot314`, and returns the exact
commands, private report path, hashes, and limitations. Do not call REA/Ghidra in this task; slot
309's three startup operations all timed out at the same 330,000 ms provider deadline with no
Ghidra Evidence ID or coverage.
