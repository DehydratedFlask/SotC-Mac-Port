# Startup slot 316 characterization claim

Local date/time: 2026-10-27 00:02 CDT (America/Chicago). Agent: `/root`.

Task: `CPU-01-STARTUP-SLOT-316-CHARACTERIZATION-01`.

## Bounded objective

Verify constructor-table index **316** / pinned RVA **`0xa33280`** against the live source and
`local/re/eboot.elf`; then run the existing read-only slot analyzer. Record the body span/hash,
reachable instruction and branch inventory, direct-call sites/targets, RIP-relative references,
and indirect-call sites. The live row is `{316, 0xa33280}` at
`native/core/ctor_original_table.cpp:330`; pinned ELF SHA-256 is
`d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`.

This pass is analyzer-only. Do not infer call semantics, CFG completeness, or native-lift
feasibility. Do not edit shared source/docs. Keep artifacts private under
`local/validation/orch-20261027/slot316/`. Do not run the original executable or use any emulator,
guest interpreter/JIT, or Sony runtime. Registration remains behind the earlier startup gaps at
indices 309, 312, and 314.

## Status and next action

Status is **ACTIVE / preliminary static analysis**; failed semantic hypotheses: **0**. After this
claim is published, `/root/leaf_rows_review` should verify the live table/ELF and run
`python3 -I tools/lift/analyze_slot_struct.py 0xa33280 --label slot316`. Return exact commands,
private artifact paths, hashes, and limitations. Do not call REA/Ghidra in this task.
