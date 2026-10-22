# Completion: RE-01-UNWRAP-IDENTITY-SIDECAR-SYMLINK-ALIAS-FIXTURE-01

- Local time: 2026-10-22 01:29 CDT (America/Chicago).
- Agent / integrator: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`).
- Task: `RE-01-UNWRAP-IDENTITY-SIDECAR-SYMLINK-ALIAS-FIXTURE-01`, child of
  `RE-01-UNWRAP-INPUT-SIDECAR-COLLISION-01`; status `FIXTURE_VERIFIED`.
- Claim: [published claim checkpoint](2026-10-22-root-re01-sidecar-symlink-claim.md), commit
  `6cd08912d27d88f3bf4a4c9690043678c70e8ab3`.
- Implementation file: `scripts/test_project_tools.py` only. Added
  `ToolChecks.test_identity_sidecar_symlink_collision_does_not_overwrite_source`; production
  `scripts/unwrap_self.py` is unchanged.
- Final test SHA-256: `b496646d6da958ddf1bd8f0cf165d073d4b56d3b08ad96d60d7ee60562ca4f02`. The unchanged
  wrapper SHA-256 is `165333d29aaab4fb2529a796bdaa796574402245e65579b85ecabee3784fc0d8`.
- Assertions: create a valid synthetic plaintext SELF at `input.self`, create
  `out.identity.json` as a symlink to that input, then call `unwrap(input.self, out.elf)`. Assert
  exact error text `identity sidecar would overwrite source: {source}`, byte-for-byte input
  preservation, and no ELF destination. This covers a symlink alias distinct from the direct
  pathname collision already tested by E132.
- Exact baseline/final command and CWD: `PYTHONDONTWRITEBYTECODE=1 python3 -I
  scripts/test_project_tools.py`, CWD `/Volumes/Extra/SoC Mac Native`. Baseline exited 0 with 11
  tests; worker and root final runs exited 0 with 12. Baseline, worker, and root logs are
  `local/validation/20261022-root-fixture-baselines/re01-sidecar-symlink-baseline.log`,
  `.../re01-sidecar-symlink-worker.log`, and `.../re01-sidecar-symlink-final.log`. SHA-256 values
  are recorded in E272/E274.
- `git diff --check` exited 0. `trail grep 'test_identity_sidecar_symlink_collision_does_not_overwrite_source'
  --in scripts/` resolves the method at `scripts/test_project_tools.py:L119-L144`. Trail wiring
  graph is in sync at 3,616 nodes / 1,240 files; deep meaning layer remains unbuilt.
- `python3 scripts/project_index.py` and `python3 scripts/project_index.py --check` exited 0;
  24,080 folders are indexed/current and `docs/checkpoints` is 437. Output is retained in
  `local/validation/20261022-root-fixture-baselines/re01-sidecar-symlink-complete-work.log`,
  SHA-256 `56c3cfce7a7d5cb0626b95c0fa4e2cc256a8b56df55392536d9c1906e597457c`.
- Observation versus inference: the synthetic symlink resolves to the source and the live guard
  rejects it before output creation; source bytes remain unchanged. This does not establish
  behavior for original game files, ELF execution, or concurrent filesystem mutation.
- Attempt count: one successful test-only implementation attempt; 0 failed tries.
- Claim publication: commit `6cd08912d27d88f3bf4a4c9690043678c70e8ab3` was pushed and fetch-verified
  at 0/0 divergence. Staged/full audits accepted 5/1,707 paths. See E273.
- Completion commit `0aee49f185e8162fda021f74fc4b48dc742ffd35` was pushed to `origin/main`. Staged
  audit accepted 6 paths and full audit accepted 1,708. Fresh fetch/divergence returned 0/0 and
  post-push full audit accepted 1,708. Publication output is retained in
  `local/validation/20261022-root-fixture-baselines/re01-sidecar-symlink-complete-publication.log`,
  SHA-256 `8d1bd393a6f8f77ad66b19dd339f63381ce7ee58017b11e1ecb5e3eee11a7f0c`.
- Existing user-owned `native/api/CMakeLists.txt` and `native/api/host/` remain untouched and outside
  this task.
- Remaining risks: local symlink support is verified in this environment; no game files or runtime
  are used. No project acceptance gate changes.
- Next exact action: validate the read-only candidate
  `SAV-01-NATIVE-STREAM-FILE-SYMLINK-DESTINATION-FIXTURE-01` against its live parent, spec, and
  target, then claim it if still distinct.
