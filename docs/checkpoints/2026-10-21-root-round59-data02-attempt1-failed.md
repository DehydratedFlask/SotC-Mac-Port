# Checkpoint: Round 59 DATA-02 attempt 1 failed

- Local date/time: 2026-10-21 13:38 CDT (America/Chicago).
- Agent ID: `/root` (integrator); worker `/root/review_host_proto` stopped after reporting the failure.
- Task ID: `DATA-02-PSARC-MANIFEST-NUL-PRECEDENCE-FIXTURE-01`.
- Files changed by the attempt: `scripts/test_psarc.py` only. Root's shared documentation records
  the failure in `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, and this checkpoint. Production
  parser `scripts/psarc.py` is unchanged.
- Exact command: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_psarc.py`.
- Result: exit 1. The new test `test_nul_manifest_preserves_newline_in_member_path` used one
  member path and requested `manifest_sep=b'\0'`. Joining a one-item list emits no separator, so
  the raw manifest contained no NUL. The parser selected newline splitting and raised
  `PsarcError: manifest has 2 paths for 1 members`.
- Worker log: `local/validation/data02-psarc-manifest-nul-precedence/attempt-1.log`, SHA-256
  `3b252197ce74415027f60684da4838940c86e576fb39bde6d5c3fd3851a0282a`.
- Test file SHA-256 after the failed attempt: `fc0f0cf8c6b73484ddcee2c0e5fca4b7aa740ad6b08578e95154adb3c19718a4`.
  Claim-time test SHA-256: `0a922203a17cfe7454cbf5f1f8e5787413511320d6bd4a54415c205360263dfc`.
  Parser SHA-256 is unchanged at `39f2c748db013637aba8144205dc60b7e5b1eb49ee0f641960783a1e2c140485`.
- `git diff --check` exited 0. No implementation file changed.
- `python3 scripts/project_index.py` exited 0 and indexed 14,409 folders; `docs/checkpoints` now
  lists 351 entries. Log `local/validation/root-round59-data02-failure/project-index.log`, SHA-256
  `a8d712e8af762dda25bc1f8d83ba259485ceac1a7e70b7512b6b7a5ffb446ea9`.
- `trail build --check` exited 0; wiring graph is in sync at 3,553 nodes. The optional deep layer
  is not built. Log `local/validation/root-round59-data02-failure/trail-check.log`, SHA-256
  `edd294377abf28693b2f71cccac3065592d98729c855c751af91b8e78c547973`.
- Shared documentation files changed: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`,
  `docs/FOLDERS.md`, and this checkpoint. `INSTALL_LOG.md` is unchanged because no installation,
  download, build dependency, or configuration changed.
- Pre-push staged path audit accepted 5 staged paths; log
  `local/validation/root-round59-data02-failure/audit-staged.log`, SHA-256
  `cb323932eab9c668aba9738b1d617aab4191cfdb028bfa99539ca04244c7d042`. Full publish-path audit
  accepted 1,618 paths; log `local/validation/root-round59-data02-failure/audit-all.log`, SHA-256
  `b6dd899f21ccc4b3746ed4a30712245284d09c2b96d4fdaf9689b34169b02066`. Cached and working
  `git diff --check` both exited 0. Only the five documentation paths are staged; worker test
  edits and the user's native/api work remain unstaged.
- Observation: the test failed before it could check NUL delimiter precedence because its fixture
  did not contain a NUL byte.
- Inference: this does not demonstrate a parser defect; delimiter precedence remains unverified.
- Remaining risks: the one-path fixture must not be retained as acceptance evidence. This task has
  1 failed fixture-construction hypothesis; it remains ACTIVE and below the three-failure
  escalation threshold.
- Exact next action: add a second member path so the raw manifest contains a NUL delimiter while
  the first member path still contains `\n`; rerun the exact Python command and inspect the diff.
- Task status: ACTIVE. Failed-attempt count: 1.
