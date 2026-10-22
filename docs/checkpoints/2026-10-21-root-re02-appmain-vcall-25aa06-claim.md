# Checkpoint: RE-02-APPMAIN-VCALL-25AA06-01 — claim

- Time / timezone: 2026-10-21 22:20 CDT (America/Chicago).
- Agent and integrator: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`).
- Task / gate / status: resolve or explicitly verify unresolved the one indirect app-main call at
  `0x25aa06`; task `ACTIVE`, parent RE-02 active; no G1 claim.
- Prerequisite evidence / inputs: RE-01; `RE-02-APPMAIN-25A8D0-01` completed with REA Evidence ID
  `ev_167b8234fc082d63d7a17a8ea4ee218ff0b8559c2397945ae599ba0c657b72ba` and exact ELF SHA-256
  `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`. Existing body evidence
  records `0x25aa06` as `call qword ptr [RAX+0x20]`; the reaching object, vptr, and slot target are
  still unresolved.
- Owned / changed files: integrator-only `HANDOFF.md`, `docs/TASKS.md`, this checkpoint, and
  generated `docs/FOLDERS.md`; worker scope after publication is a separate private evidence
  directory and new `tools/lift/verify_appmain_vcall_25aa06.py` only. Do not modify `reference/` or
  the completed app-main dossier. No native lift is authorized by this claim.
- Hypothesis and expected result: a unique file-backed reaching object/vptr and `+0x20` slot may
  identify one static callee. If the bytes do not establish a unique mapping, retain the call as
  unresolved and identify the missing evidence. No target or C++ contract is assumed.
- Exact command, CWD and nonsecret environment: from `/Volumes/Extra/SoC Mac Native`,
  `python3 -I local/re/evidence/re02-appmain-25a8d0/verify.py`.
- Exit / observed result: exit 0. It reproduced the completed parent verifier output exactly; the
  baseline output SHA-256 is
  `173a22dab0846535c1947627942087e98fe1a09c535b5db0615d6bdfa9b356cd`; stderr is empty with SHA-256
  `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`. Private copies are under
  `local/re/evidence/re02-appmain-vcall-25aa06-01/parent-baseline.*`.
- Graft command: `trail ask "How is the existing byte-bound RE-02 app-main verifier organized, and
  what extension point fits a separate unresolved indirect call at 0x25aa06?" --source --in
  tools/`. It returned existing byte-bound analysis patterns and the source-extension area; saved
  approximately 8,673 tokens. This informs the verifier location only; it does not identify the
  binary callee.
- Duplicate check: `rg -n 'RE-02-APPMAIN-VCALL-25AA06-01|APP-01-FINDER-BUNDLE-01'
  docs/TASKS.md docs/EVIDENCE.md HANDOFF.md` returned no matches before the claim.
- Observation versus inference / coverage limitations: the parent report binds the instruction
  bytes and body to the pinned ELF. That does not establish the RAX value at this instruction,
  object construction, class identity, slot contents, call reachability, runtime behavior, or
  native-call semantics. Prior verifier mapping failures belong to the completed parent task and
  are not counted as failed hypotheses on this new callsite task.
- Cumulative failed tries and distinct hypotheses: 0 on this task. The completed parent task has
  three verifier-only mapping failures and principal reassessment; see its completion checkpoint.
- Remaining risks: RAX may be dynamically populated or its slot may be outside file-backed data;
  Ghidra partial callee coverage and an unresolved dynamic dispatch are possible. Stop rather than
  infer a function from strings or neighboring addresses.
- Claim publication: staged and full path audits passed for 4 and 1,660 paths. Commit
  `866e7bf648976bf7f0f83943884c9df65d66922b` (`docs: claim app-main vcall analysis`) was pushed to
  `origin/main`. Post-push `git fetch origin main`, `git rev-parse HEAD origin/main`, and
  `git rev-list --left-right --count HEAD...origin/main` confirm matching tips and 0/0 divergence.
  Private log `local/checkpoints/round78-re02-callsite-claim-postpush.log` SHA-256
  `bfd7aecbaae70345740d2858e5dcba5c0eaa721ddabe08fa00a0172a2dba750b`.
- Index checks: `python3 scripts/project_index.py` exited 0 and reported 19,269 folders;
  `python3 scripts/project_index.py --check` reports current. `trail build --check` reports the
  3,580-node wiring graph in sync; its optional meaning layer remains unbuilt. `git diff --check`
  passed. A later first `python3 scripts/project_index.py --check` reported stale
  `local/indexes/folders.json` after creation of the private evidence directory; regeneration
  reported 19,269 folders and the immediate check then passed. This was a private index count
  update, not a source/task failure. The recovery record is
  `local/checkpoints/round78-re02-index-recovery.log`, SHA-256
  `2f4277881f03e3b76fc41d5827731af6eb23cecc19a8aeee87dd96862a1e3782`. Staged/full publication
  audits at the claim accepted 4/1,660 paths. Before the publication-record sync, staged/full
  audits accepted 2/1,660 paths; logs `local/checkpoints/round78-re02-claim-sync-staged-audit.log`
  (`fcb26e41bea7f46348480b2cdeabecbfbb40526c82808d673cd0fcc55c5d0075`) and
  `local/checkpoints/round78-re02-claim-sync-full-audit.log`
  (`731980e81a08531778550a6b4dcd0dec95ddc65e32417cee5ad1a78c5510c410`). The sync `trail build
  --check` result is in `local/checkpoints/round78-re02-claim-sync-trail-check.log`
  (`c4d4773a1c7beda75e8c787228e9162b855526c58e5e95e811191e7c74b30015`); wiring graph current at
  3,580 nodes, deep meaning layer not built.
- Publication-record sync commit `414a0434e96885b5f028e9b2c7e9f18730955b3e` was pushed to
  `origin/main`. Post-push fetch confirms `HEAD == origin/main`, divergence 0/0; log
  `local/checkpoints/round78-re02-claim-sync-postpush.log` SHA-256
  `4e7d5e77d98fb4c48e615b108f703daacb772c84a424047a393ef25879df4e9b`. Only the pre-existing
  `native/api/CMakeLists.txt` modification and untracked `native/api/host/` remain in the worktree;
  neither was staged or published.
- Next exact action: build an independent byte/mapping trace for only `0x25aa06`; create the
  verifier-backed report; root will replay it and decide whether this static edge is unique or
  remains unresolved. Only then consider a follow-on native function task.
- HANDOFF.md / TASKS / EVIDENCE / INSTALL_LOG / indexes updated: HANDOFF and TASKS updated;
  EVIDENCE and INSTALL_LOG unchanged because this is a claim with no new verified artifact or
  dependency; regenerate and check project indexes before publication.
