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
  `062345ae937556d3c621c9be3ea4317787cfd3c65c9bc4b1fbf665c7b5fac973`, preserves the earlier
  stale-index check and the successful regeneration/recheck.
- Staged/full publication audits accepted 4/1,661 paths; logs:
  `local/checkpoints/round78-re02-25ac9f-staged-audit.log` SHA-256
  `c25b4ee488c6ba783d6b744e9b1a02cab79a24328846739a396fc33974c38827` and
  `local/checkpoints/round78-re02-25ac9f-full-audit.log` SHA-256
  `17200d8db78d887760aa6fcbe3f30b7bd7a24542bbe7351675fceb9268505945`.
- Remaining risks: RAX or its vtable could be dynamically populated, and partial callee coverage
  may prevent a unique static mapping. Do not use strings or neighboring addresses as proof.
- Next exact action: after this claim is published, assign an offline-only worker to trace
  `0x25ac9f`; avoid the shared REA binary session until the `0x25aa06` worker has exported and
  closed it. Independently replay its byte verifier before any status change.
- HANDOFF.md / TASKS / EVIDENCE / INSTALL_LOG / indexes updated: HANDOFF and TASKS updated;
  EVIDENCE and INSTALL_LOG unchanged because no new verified result or dependency. Refresh
  `docs/FOLDERS.md`, run index/graph checks, audit allowlisted paths, and publish the claim.
