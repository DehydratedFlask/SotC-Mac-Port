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

## Completion checkpoint — 2026-10-20 00:28 CDT

- Integrator: `/root`; analyst: `/root/srv01_page_map_19660`.
- Task ID: `CPU-01-CTOR10-INDIRECT-01`; shared files changed: `HANDOFF.md`, `docs/TASKS.md`,
  `docs/EVIDENCE.md`, and this checkpoint. Private evidence is under
  `local/re/evidence/cpu01-ctor10-indirect-01/`; no native/launcher implementation or original
  input changed. `INSTALL_LOG.md` is unchanged because there was no install or download.
- Private report SHA-256:
  `8e89a42839c9185043b1823bdeec10bdb8670f88e84a6c34f0682919fe107155`. No new REA analysis was
  needed. Existing E43/E46 IDs are
  `ev_054b2f190852ca75b5f92898208d4f8e554fbcfe6ab6416b0c018242902d7921` and
  `ev_9b111492162042c8bdb212bc83c30eda2370d34b6d4bb2cec357d83bff0d437d`.
- Root exact verifier command:
  `python3 -I local/re/evidence/cpu01-ctor10-indirect-01/verify.py > local/re/evidence/cpu01-ctor10-indirect-01/root-verification.json 2> local/re/evidence/cpu01-ctor10-indirect-01/root-verification.stderr.log`.
  Exit 0; stderr 0 bytes; output SHA-256
  `2f01ccb78d20316a42709b99af563c0c5d32edc40656c3444ae45ce114c8f0d2`, byte-identical to the
  worker's verification JSON. The ELF SHA and unchanged REA baseline/E43/E46 snapshot hashes all
  match the report.
- Root exact raw-byte command:
  `python3 -I local/re/evidence/cpu01-ctor10-indirect-01/root-raw-recheck.py > local/re/evidence/cpu01-ctor10-indirect-01/root-raw-recheck.txt 2> local/re/evidence/cpu01-ctor10-indirect-01/root-raw-recheck.stderr.log`.
  Exit 0; stderr 0 bytes; verifier SHA-256
  `0f7dea2a010bbd0653536e76ecc71d4ae86887a92455040eefe43e599353a6d4`; result SHA-256
  `14aeb4665cb2db19bb4f6736cd76cab2db3c2f1ad50aec8a1dc623fccb88f866`. It independently checks
  the complete 260-byte caller-body hash, `vxorps ymm0,ymm0,ymm0` at `0xf09e80`, the following
  zeroed guard store, the VMA-0 qword, and that the computed operand lies outside both PT_LOAD
  memory ranges.
- Observation: the function setup zeroes YMM0 before storing into the guard range; its two guard
  branches therefore skip the computed call for this byte-supported setup. E46 also establishes
  that its helper stores zero into caller `[RSP+0x10]`. Conditional on a hypothetical call-reaching
  state that retains this zero, `RAX=qword[0]` reads file-backed bytes `/libexec`, and the qword at
  `[RAX+0xc0]` has no file-backed target or relocation. The constructor-table RELATIVE relocation
  only resolves slot 10 to entry `0xf09e60`.
- Inference/limits: no unique static in-image target is supported, so `0xf09efb` remains unresolved.
  No process mappings, loader behavior, constructor reachability/execution, ABI/service contract, or
  G1 behavior are established. Analysis hypothesis failures: 0.
- Exact tool issues, from `local/re/evidence/cpu01-ctor10-indirect-01/commands.txt`: first checker run
  exited 1 on `KeyError: 'relationship_coverage'`, corrected to the top-level E43 JSON key and then
  passed; `/usr/bin/objdump -R local/re/eboot.elf` exited 1 with “not a dynamic object”; a checksum
  wrapper's unquoted `/Volumes/Extra/SoC Mac Native` path failed before running, then the direct
  checksum command from the repository root passed. None invalidated an analysis hypothesis; the
  platform parser `scripts/ps4_dynlink.py` supplied relocation site records. Its live `parse()` at
  lines 72–121 was inspected after this Graft query returned only lexical/file matches:
  `graft ask "How does scripts/ps4_dynlink.py parse PS4 dynamic relocation tables and retain individual relocation target offsets?" --source --in scripts/`.
  The query exited 0 and saved approximately 12,482 tokens; the worker's preceding Graft query saved
  approximately 12,055 tokens.
- Required indexing/graph checks: `python3 scripts/project_index.py` exited 0 and indexed 4,130
  folders (the count includes the newly created private evidence directory; generated tracked
  `docs/FOLDERS.md` had no diff). `graft check` exited 0 for the wiring graph; deep meaning remains
  0% built with 1,219 nodes pending. `git diff --check` passed before publication.
- Next exact action: keep `0xf09efb` unresolved and select the next bounded CPU-01/RE-02 startup
  question from the documented task graph; do not promote G1 or treat the constructor slot as
  runtime-reachable. Task status: `STATIC_ANALYSIS_VERIFIED`; 0 failed analysis hypotheses.
