# Checkpoint: RE-02-APPMAIN-VCALL-25AC9F-01 — claim

- Time / timezone: 2026-10-21 22:25 CDT (America/Chicago).
- Agent and integrator: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`).
- Task / gate / status: trace or explicitly verify unresolved the indirect app-main call at
  `0x25ac9f`; task `ACTIVE`, parent RE-02 active; no G1 claim.
- Prerequisite evidence / inputs: RE-01; completed `RE-02-APPMAIN-25A8D0-01`; pinned ELF
  `local/re/eboot.elf` SHA-256
  `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`. The existing report
  records `0x25ac9f` as `call qword ptr [RAX+0x28]`; its reaching object, vptr, and slot target are
  unresolved.
- Owned / changed files: integrator-only `HANDOFF.md`, `docs/TASKS.md`, this checkpoint, and
  generated `docs/FOLDERS.md`; after publication, a worker may own only private evidence under
  `local/re/evidence/re02-appmain-vcall-25ac9f-01/` and the new
  `tools/lift/verify_appmain_vcall_25ac9f.py`. Do not edit `reference/`, the parent dossier, the
  `0x25aa06` task's files, or native production sources. No native lift is in scope.
- Hypothesis and expected result: the reaching object/vptr and `+0x28` slot may map uniquely to one
  file-backed callee. Otherwise preserve the unresolved edge and identify the exact missing bytes
  or dataflow. Do not infer a C++ class or semantic contract.
- Exact command, CWD and nonsecret environment: from `/Volumes/Extra/SoC Mac Native`,
  `python3 -I local/re/evidence/re02-appmain-25a8d0/verify.py`.
- Exit / observed result: exit 0. Parent verifier output is preserved at
  `local/re/evidence/re02-appmain-vcall-25ac9f-01/parent-baseline.json`, SHA-256
  `173a22dab0846535c1947627942087e98fe1a09c535b5db0615d6bdfa9b356cd`; stderr is empty, SHA-256
  `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`.
- Graft command: `trail ask "Which existing analysis helper reports indirect transfers and maps
  x86 virtual addresses to bytes in the pinned reconstructed ELF?" --source --in tools/lift/`.
  It surfaced existing byte-bound analysis helper patterns and saved approximately 11,744 tokens;
  it did not identify the dynamic callee.
- Duplicate check: `rg -n 'RE-02-APPMAIN-VCALL-25AC9F-01' docs/TASKS.md docs/EVIDENCE.md
  HANDOFF.md` returned no matches before this claim.
- Observation versus inference / coverage limitations: parent evidence binds the function bytes
  and call instruction but does not establish the value of RAX, object construction, class identity,
  slot contents, call reachability, runtime behavior, or native-call semantics. Sibling analysis of
  `0x25aa06` is active under a separate task and file scope.
- Cumulative failed tries and distinct hypotheses: 0 on this task. Parent body characterization has
  three verifier-only mapping failures; they are documented in that task and are not transferred
  to this distinct callsite analysis.
- Index and graph checks: `python3 scripts/project_index.py` reported 19,270 folders;
  `python3 scripts/project_index.py --check` reports current. `trail build --check` passes with
  3,580 wiring nodes in sync; the optional deep meaning layer is not built. `git diff --check`
  passes. Combined capture log `local/checkpoints/round78-re02-25ac9f-index-graph.log`, SHA-256
  `2a8c0b693a80959c279e5f9ffc8806f56cd0e552d40b87166e4005541f2051ae`, records regeneration
  and the successful recheck. An earlier `--check` invocation reported `Stale indexes:
  local/indexes/folders.json` (exit 1) after private evidence/log files were created; regeneration
  restored the index at 19,270 folders and the next check passed. This was a private file-count
  refresh, not a source/task failure.
- Initial claim staged/full publication audits accepted 4/1,661 paths; logs:
  `local/checkpoints/round78-re02-25ac9f-claim-staged-audit.log` SHA-256
  `c25b4ee488c6ba783d6b744e9b1a02cab79a24328846739a396fc33974c38827` and
  `local/checkpoints/round78-re02-25ac9f-full-audit.log` SHA-256
  `17200d8db78d887760aa6fcbe3f30b7bd7a24542bbe7351675fceb9268505945`. The later documentation
  sync staged/full audits accepted 2/1,661 paths; logs `local/checkpoints/round78-re02-25ac9f-sync-staged-audit.log`
  SHA-256 `006610682336ac2a4ba91cee6e2bfec0c1754467869582cfcce9fd0e35accac6` and
  `local/checkpoints/round78-re02-25ac9f-sync-full-audit.log` SHA-256
  `17200d8db78d887760aa6fcbe3f30b7bd7a24542bbe7351675fceb9268505945`.
- Claim commit `7975c5c6f1d55e0efe7dc61031f8ff6d69cfe557` (`docs: claim second app-main callsite
  analysis`) was pushed to `origin/main`; post-push fetch verifies matching tips and 0/0 divergence.
  Private log `local/checkpoints/round78-re02-25ac9f-postpush.log` SHA-256
  `21fa37a1319c7846fb5a87c47c199414fdcfa84dc459babda4145486ec17ab4a`. Only the pre-existing
  user-owned `native/api/CMakeLists.txt` and untracked `native/api/host/` remain outside the commit.
- Remaining risks: RAX or its vtable could be dynamically populated, and partial callee coverage
  may prevent a unique static mapping. Do not use strings or neighboring addresses as proof.
- The claim's documentation sync was committed as `9462efc0187ca14f417fd7a33b6cc4733b5474b8`
  (`docs: record second callsite claim publication`) and pushed to `origin/main`. `git fetch origin`
  followed by `git rev-parse HEAD`, `git rev-parse origin/main`, and
  `git rev-list --left-right --count HEAD...origin/main` returned matching tips and `0 0`.
  Private post-push log `local/checkpoints/round78-re02-25ac9f-sync-postpush.log` SHA-256
  `85fe3be51a31fa649189e33abc6998ecfa376e1a5ed44d079996d2cfe754b3a5`.
- Exact sync preflight commands: `git diff --cached --check`,
  `python3 -I scripts/audit_publish_paths.py`, and
  `python3 -I scripts/audit_publish_paths.py --all`; exit 0, 2 staged paths and 1,661 full-tree
  paths accepted. Audit logs are `local/checkpoints/round78-re02-25ac9f-sync-staged-audit.log`
  SHA-256 `006610682336ac2a4ba91cee6e2bfec0c1754467869582cfcce9fd0e35accac6` and
  `local/checkpoints/round78-re02-25ac9f-sync-full-audit.log` SHA-256
  `17200d8db78d887760aa6fcbe3f30b7bd7a24542bbe7351675fceb9268505945`.
- Worker `/root/re02_vcall_25ac9f` now owns only the private evidence directory and dedicated
  verifier named above. It is performing offline analysis. The shared REA session is closed and root
  controls any future REA operation; do not run concurrent REA/Ghidra analysis.
- Next exact action: trace the reaching object and slot offline, add a byte-bound verifier if useful,
  and preserve the unresolved edge if evidence is not unique. Root will integrate the worker
  checkpoint and publish it; no runtime/class/ABI/G1 claim is permitted.
- HANDOFF.md / TASKS / EVIDENCE / INSTALL_LOG / indexes at claim time: HANDOFF and TASKS updated;
  EVIDENCE and INSTALL_LOG unchanged because no new verified result or dependency. The folder index
  and Trail graph passed before the claim commit; no source file was changed.

## Publication sync checkpoint

- Time / timezone: 2026-10-21 22:31 CDT (America/Chicago); integrator `/root`.
- Task: `RE-02-APPMAIN-VCALL-25AC9F-01`; status remains `ACTIVE`, 0 failed tries.
- Files changed by the sync: `HANDOFF.md` and this checkpoint. Claim row and generated
  `docs/FOLDERS.md` were already published in the claim commit.
- Exact commands: `git diff --cached --check`; `python3 -I scripts/audit_publish_paths.py`;
  `python3 -I scripts/audit_publish_paths.py --all`; `git commit -m "docs: record second callsite claim publication"`;
  `git push origin main`; `git fetch origin`; `git rev-parse HEAD`; `git rev-parse origin/main`;
  `git rev-list --left-right --count HEAD...origin/main`.
- Result: all preflight commands exited 0; staged/full audits accepted 2/1,661 paths; commit
  `9462efc0187ca14f417fd7a33b6cc4733b5474b8` pushed successfully; fetched local and remote tips
  match and divergence is 0/0. Push/fetch evidence is in the private sync log cited above.
- Observation versus inference: publication and repository synchronization are confirmed. This
  says nothing about the indirect call's target or runtime behavior.
- Remaining risks: offline trace has not yet established RAX's reaching object or the `+0x28` slot.
- Next exact action: continue the worker's offline analysis and independently replay its verifier;
  preserve `UNRESOLVED` if target bytes or dataflow do not prove one unique callee.
