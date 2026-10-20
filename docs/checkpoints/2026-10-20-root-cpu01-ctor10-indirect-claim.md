# CPU-01-CTOR10-INDIRECT-01 — slot-10 computed-call dataflow claim

- Date/time: 2026-10-20 00:19 CDT (America/Chicago).
- Integrator/owner: `/root`; assigned analyst: `/root/srv01_page_map_19660`.
- Task ID: CPU-01-CTOR10-INDIRECT-01; parents CPU-01-CTOR10-ANALYSIS-01 and
  CPU-01-CTOR10-INIT-HELPER-01.
- Files changed at claim: `HANDOFF.md`, `docs/TASKS.md`, `docs/FOLDERS.md`, and this checkpoint.
  The required `python3 scripts/project_index.py` refresh exited 0 and reported 4,129 folders;
  it updated the checkpoint count in `docs/FOLDERS.md` from 26 to 27. No native/source
  implementation or private evidence was changed.
- Failed tries: 0.

## Prerequisite and bounded scope

E43 records an unresolved computed call at `0xf09efb` in constructor slot 10 (`0xf09e60`),
encoded as `call qword ptr [RAX+0xc0]`. E46 independently verified that, for the observed entry
arguments, the pre-call zero store and helper write ranges leave both guard fields zero and the
first branch skips this call. This follow-up asks only what dataflow supplies the call target on a
call-reaching path, and whether available pinned data can identify its target. Keep the earlier
observed path separate from any hypothetical path. Do not claim reachability from a constructor
runner or assign a helper/service contract from names or neighboring comments.

Target input: `local/re/eboot.elf`, x86-64 ELF SHA-256
`d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`, image base 0. Prior
hash-bound evidence is E43/E46 and their private dossiers. If REA is needed, use the installed
REA 6.7.0 / Ghidra 12.1.4 CLI, a separate snapshot
`local/re/rea-f09efb-snapshot.json`, and private output under
`local/re/evidence/rea-f09efb/`; never change the baseline `local/re/rea-snapshot.json` or either
prior task snapshot.

Use Ghidra/REA results and independent raw-byte/disassembly/data-reference checks to bound:

1. the branch conditions leading to `0xf09efb`;
2. the definition(s) of `RAX` on a call-reaching path and the pointer field at `+0xc0`;
3. any static initialization, constructor-table, or relocation evidence that uniquely determines
   the target.

If no unique static target is supported, retain an unresolved edge and state exactly what input or
runtime evidence would be needed. A skipped call on the previously verified zero-guard path is not
proof that all paths skip it.

## Acceptance and limits

- Deliver a private hash/address-bound note with exact commands, result/log paths, REA Evidence ID
  if run, Ghidra range/hash, raw instructions, data references, and caller-local target provenance.
- Independently verify any instruction/range/target claim against the unchanged pinned ELF; root
  reviews the report before any shared task status changes.
- No source edits, game execution, install/download, emulator, ABI claim, constructor reachability
  claim, or G1 promotion.

## Claim checkpoint

- Exact next action: inspect the existing E43/E46 disassembly and byte verifier first; only if they
  do not expose the call-reaching RAX/data source, run one REA/Ghidra analysis into the distinct
  snapshot and private folder above. Independently check all target bytes/references, then send the
  bounded report to root. Root owns `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, index refresh,
  audits, commit, and push.
- Graft lookup `graft grep "f09efb" --in native/` exited 0 with no native indexed occurrences;
  this does not inspect the binary. The exact reference query
  `graft ask "Locate the pinned Shadow of the Colossus source's game-audio bank and stream readers, decode entry points, and sound event callsites. Distinguish file identity/header parsing from actual decoding or playback, and identify a bounded next AUD-01 task." --source --in reference/ShadowOfTheColossus-PC/`
  exited 1 (`nothing indexed under` that subtree). The exact documented rebuild
  `graft build --follow-nested-repos --only-dir scripts --only-dir tools --only-dir native --only-dir launcher --only-dir reference`
  exited 0 but replayed only 528 cached root files and did not index that nested repository.
  `graft check` still exited 0 for the current wiring graph, demonstrating that this check does not
  prove the reference subtree is in scope. This missing scope is recorded in HANDOFF; raw source
  search is allowed only for that genuinely unindexed subtree.
- Status: DISPATCHED; 0 failed analysis hypotheses/tries. The existing slot-10 call/guard dossier
  is context, not a claim that this new target question is solved.

## Dispatch checkpoint

- Date/time: 2026-10-20 00:21 CDT (America/Chicago); agent ID `/root`.
- Task ID: CPU-01-CTOR10-INDIRECT-01; files changed: `HANDOFF.md` and this checkpoint.
- Exact action: pushed claim commit `4cfc51b` to `origin/main`, then sent
  `collaboration.followup_task` to `/root/srv01_page_map_19660`, directing it to reuse E43/E46,
  create the specified separate REA snapshot only if needed, independently verify raw bytes and
  target provenance, keep unsupported target unresolved, and return evidence without editing shared
  docs or implementation.
- Result: dispatch accepted; task is in progress. No new command log or analysis evidence yet.
- Observation: the worker had completed the previous page-map task and was reused; no task result is
  inferred from the dispatch.
- Remaining risk: call-reaching `RAX` provenance and unique-target question remain unresolved.
- Next exact action: review the worker report and independently rerun the specified byte/reference
  checks before changing shared evidence/status.
- Failed tries: 0; status: IN_PROGRESS.
