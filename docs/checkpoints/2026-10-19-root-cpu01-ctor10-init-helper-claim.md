# CPU-01-CTOR10-INIT-HELPER-01 — caller-local initializer dataflow claim

- Date/time: 2026-10-19 23:46 CDT (America/Chicago).
- Integrator: `/root`; assigned analyst: `/root/cpu_ctor10_init_helper`.
- Task ID: CPU-01-CTOR10-INIT-HELPER-01; parent CPU-01-CTOR10-ANALYSIS-01 / CPU-01.
- Files changed at claim: `HANDOFF.md`, `docs/TASKS.md`, `docs/FOLDERS.md`, and this checkpoint.
  Adding the checkpoint updated the folder inventory from 21 to 22 files under `docs/checkpoints`.
  No native or analysis source has been changed.
- Failed tries: 0.

## Prerequisite and bounded scope

E43 records that constructor slot 10 at `0xf09e60` calls local function `0xe1eda0` at site
`0xf09ec7`, then tests caller stack fields at `RSP+0xa8` and `RSP+0xa0` before an unresolved
computed call at `0xf09efb` (`call qword ptr [RAX+0xc0]`). The existing static pseudocode suggests
those fields are part of a local object initialized to zero, but the callee may modify the object.
Do not treat that decompiler impression as proof of a dead guard.

Characterize only `0xe1eda0` and its effect on these caller fields. Use the same pinned local ELF,
REA/Ghidra, and a new snapshot. Independently verify function bytes/range, callee writes through its
argument pointer, caller stack offsets and branch bytes. Determine whether the condition at
`0xf09efb` can be statically concluded from this one initializer; if not, preserve the unresolved
status. Do not run the game, change source, overwrite `local/re/rea-snapshot.json`, or claim ABI or
runtime reachability.

## Acceptance and limits

Use the pinned REA/Ghidra installation against `local/re/eboot.elf` at `0xe1eda0`, output to
`local/re/rea-e1eda0-snapshot.json` and existing `local/re/evidence/`. Record exact command/result,
Evidence ID, tool versions/profile, input hash/image base, function range/body hash, direct calls,
data refs, unresolved flows and coverage. Add a private byte/hash verifier and independent disassembly
for the caller/callee ranges. Keep the evidence private. Failed tries: 0.

This task can clarify the static branch guard only; it cannot establish constructor execution, service
semantics, native ABI compatibility, game startup, or G1. Root owns shared docs, project-index refresh,
path audits, commit and push. Current status: ACTIVE; exact next action: run the one hash-bound
REA/Ghidra inspection and verify the two caller-field effects.
