# CPU-01-CTOR10-ANALYSIS-01 — first unresolved constructor analysis claim

- Date/time: 2026-10-19 23:29 CDT (America/Chicago).
- Integrator: `/root`; assigned analyst: `/root/cpu_slot10_analysis`.
- Task ID: CPU-01-CTOR10-ANALYSIS-01; parent CPU-01 / RE-02.
- Files changed in claim: `HANDOFF.md`, `docs/TASKS.md`, and this checkpoint. No analysis source
  or native code has been changed.
- Failed tries: 0.

## Prerequisite evidence and bounded scope

The CPU-01-CTOR-PREFIX-01 fixture stops after ten recovered callbacks and reports original
constructor index 10, RVA `0xf09e60`, as the first unresolved slot. Its exact ELF identity is
`d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`; the 573-entry original table
starts at `0x2dd70e0` and has SHA-256
`5917cb85db190e9ef93ce5e06d5b6596d40b6c3623166e37ed7c6c3c3cd3ab28`.

Graft `grep "f09e60"` finds the null registration, test assertion, and table checker only. Earlier
static notes record a 255-byte body hash
`fde1ac223a4c084da0a1a9d04804bee8ba8e12be0ac46e1a8e98a925ac4ffcc7`, but complete call/data
semantics have not been corroborated. The task is read-only: use REA/Ghidra on the local ELF at
image base zero, save an exported evidence record and reproducible recipe, and classify direct/
indirect call and data-reference coverage. Do not implement a replacement, run the game, download
files, or modify the baseline `local/re/rea-snapshot.json`.

## Acceptance and limits

Exact REA command:

```sh
rea function local/re/eboot.elf 0xf09e60 --provider ghidra --snapshot local/re/rea-f09e60-snapshot.json --format json
```

Retain logs, Evidence ID, source digest, tool versions, image base, function range and byte hash,
coverage, direct calls/import identities, data references, and unresolved computed calls under
private `local/re/evidence/` or `local/validation/`. Independently verify the input hash and any
byte/range claims. A missing function or incomplete references are valid documented outcomes, not
permission to infer semantics. This task can identify the blocker and required contracts only; it
cannot pass G1 or replace the runtime constructor.

Root integrates the checkpoint and refreshes `docs/FOLDERS.md` after reports arrive. Before the
worker closes, root verifies the evidence path and records exact commands/results here. Current
status: ACTIVE; failed tries: 0; next action: run the bounded REA/Ghidra inspection.
