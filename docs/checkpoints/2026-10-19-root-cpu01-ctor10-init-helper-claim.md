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

## Completion checkpoint — 2026-10-19 23:59 CDT

- Agent: `/root/cpu_ctor10_init_helper`; integrator: `/root`.
- Task ID: CPU-01-CTOR10-INIT-HELPER-01, parent CPU-01-CTOR10-ANALYSIS-01 / CPU-01.
- Files changed: worker changed no repository-tracked files. Root updated `HANDOFF.md`,
  `docs/TASKS.md`, `docs/EVIDENCE.md`, and this checkpoint; no native/launcher source or baseline
  REA snapshot was changed. Private evidence remains under ignored `local/re/evidence/` and `local/re/`.
- Exact REA command and environment are retained in
  `local/re/evidence/rea-e1eda0-analysis.command.txt`; it invoked REA 6.7.0 with Ghidra 12.1.4 and
  exited 0. Input ELF SHA-256 is
  `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`, image base 0; Evidence ID
  `ev_9b111492162042c8bdb212bc83c30eda2370d34b6d4bb2cec357d83bff0d437d`. Helper bytes cover
  `0xe1eda0..0xe1eddd` (62 bytes), SHA-256
  `d51c6f9bf6d0197e16ed629e6f6c0f2e450511aa3c16f1a2c475b60352f1f559`. Full result:
  `local/re/evidence/rea-e1eda0-summary.md`.
- Independent root check: `PYTHONDONTWRITEBYTECODE=1 python3 -I local/re/evidence/rea-e1eda0-verify.py`
  redirected to `rea-e1eda0-root-verification.json` exited 0 with empty stderr. Its output compared
  byte-for-byte equal to worker `rea-e1eda0-verification.json` (`cmp -s`, exit 0; both SHA-256
  `85a4fc9472fa7b6c8067c04224f57700b7140bb0d75049d1e564ac7268420bc6`). Root reconfirmed the ELF,
  baseline snapshot, and target snapshot hashes; baseline remained
  `d80232c4fe22d7e75654eade8db44af093507a94723f80dd5ce67605dd382354`.
- Observation versus inference: raw caller/helper byte checks show the call arguments and write
  spans; the pre-call YMM zero store covers both guard fields, and helper writes do not overlap them.
  The first `je` therefore skips `0xf09efb` on this function-entry path with these arguments. This
  does not show whether the constructor is reachable or runs, and leaves the computed target unknown.
- Failed attempts: 0 failed REA invocations and 0 failed analysis hypotheses. One first checker
  shell wrapper failed after verifier output because zsh reserves `status`; the corrected wrapper
  passed. The first attempt record is retained in
  `local/re/evidence/rea-e1eda0-verifier-try1.command.txt`.
- Remaining risks: constructor reachability/execution, computed target, helper/service ABI, native
  startup, and all game acceptance gates remain open. No game, build, or runtime constructor was run.
- Next exact action: if continuing this area, claim a separate bounded task for the computed call or
  remaining helper contract; do not infer reachability from this branch result. G1 remains NOT PASSED.
- Status: STATIC_ANALYSIS_VERIFIED; 0 failed analysis hypotheses; parent CPU-01 remains ACTIVE.
