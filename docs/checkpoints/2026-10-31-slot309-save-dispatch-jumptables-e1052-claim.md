# E1052 — slot-309 save-dispatch jump-table route claim

**Claim checkpoint:** 2026-10-31 02:00 CDT, root. **Task:**
`CPU-01-SLOT309-SAVE-DISPATCH-JUMPTABLES-01`. **Status:** ACTIVE. **Attempts:** 0.

E911 allows the active slot-309 characterization to resume after a principal reassessment. The
current user request provides that reassessment. E1020 left computed jumps `0xa4869b` and
`0xa4afba` unresolved on the save-dispatch route. A read-only scout identified candidate
PIC-relative tables at `0x12c3280` and `0x12c3324`, with reported lengths of 17 and 19 entries, and
candidate case routes toward `0xa4a600` and `0xa4a940`. These are leads only; root has not yet
verified their encodings, target ranges, or source digest linkage.

## Bounded scope

- Verify the exact table bytes and signed-relative target calculations in pinned ELF
  `local/re/eboot.elf`, SHA-256
  `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`.
- Cross-check dispatcher/case instructions with REA/Ghidra and record tool profile, image base,
  source mapping, evidence IDs, coverage, and unresolved indirect control flow.
- Trace only case blocks that reach existing save-consumer paths `0xa4a600` and `0xa4a940`.
- Stop if no concrete producer/consumer/order edge to slot-309 initializer `0xa505e0` is proved.
  Slot 309 remains unregistered; no source/runtime change or game-gate claim is in scope.

## Ownership and next action

Root owns `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/INDEX.md`, generated
`docs/FOLDERS.md`, this claim/result checkpoint, and private evidence under
`local/validation/slot309-save-dispatch-e1052/`. No files in `reference/` will be changed.

**Next exact action:** verify the saved Ghidra project identity/lock state, then independently
decode and byte-check the two tables and selected case blocks against the pinned ELF. Attempts: 0.
