# Claim: RE-01-UNWRAP-IDENTITY-SIDECAR-SYMLINK-ALIAS-FIXTURE-01

- Local time: 2026-10-22 01:22 CDT (America/Chicago).
- Agent / integrator: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`).
- Task / parent / status: `RE-01-UNWRAP-IDENTITY-SIDECAR-SYMLINK-ALIAS-FIXTURE-01`, child of
  `RE-01-UNWRAP-INPUT-SIDECAR-COLLISION-01`; parent is `FIXTURE_VERIFIED`; child is `ACTIVE`,
  zero failed implementation tries.
- Candidate uniqueness: no matching task ID was found in `docs/TASKS.md`, `docs/EVIDENCE.md`, or
  `docs/checkpoints/` before the claim. The parent already covers a direct pathname collision; this
  fixture covers a symlink alias that resolves to the same source file.
- Live source context: `trail grep 'samefile' --in scripts/` resolved `unwrap` at
  `scripts/unwrap_self.py:L10-L31`; line 12 checks `identity_path.exists()` and line 13 rejects
  `source.samefile(identity_path)` before reading or writing. The existing test
  `ToolChecks.test_identity_sidecar_collision_does_not_overwrite_source` is at
  `scripts/test_project_tools.py:L90-L117` and covers the direct pathname collision.
  `trail grep 'identity sidecar would overwrite source' --in scripts/` resolves both the guard and
  the existing test. Live wrapper SHA-256 is `165333d29aaab4fb2529a796bdaa796574402245e65579b85ecabee3784fc0d8`.
- Linked specification: [`docs/EXTRACTION.md`](../EXTRACTION.md), section “SELF to ELF: observed
  plaintext reconstruction,” and the verified parent completion record
  [`RE-01-UNWRAP-INPUT-SIDECAR-COLLISION-01`](2026-10-21-root-re01-unwrap-input-sidecar-collision-complete.md).
  The fixture uses only a generated plaintext SELF and makes no runtime or original-game-input
  claim.
- Baseline source hash: `scripts/test_project_tools.py` SHA-256
  `598cc65d59fc3f4461f4c348235ff928fefdf1a1ae1a1e935276511829e4cfae`. Baseline command and CWD:
  `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_project_tools.py`, CWD
  `/Volumes/Extra/SoC Mac Native`; exit 0, 11 tests passed. Log
  `local/validation/20261022-root-fixture-baselines/re01-sidecar-symlink-baseline.log`, SHA-256
  `61d98a7f3c197b1acc2cec656250c9a9d323d25a201b1c07bae437a2e3f4c695`.
- Bounded scope: one test method in `scripts/test_project_tools.py`. Construct a valid synthetic
  plaintext SELF using the existing fixture bytes; write it to a source path distinct from the
  derived sidecar; create `destination.with_suffix(".identity.json")` as a symlink to that source;
  call `unwrap(source, destination)`; assert the exact
  `identity sidecar would overwrite source: {source}` `ValueError`, unchanged source bytes, and no
  ELF output. Do not modify `scripts/unwrap_self.py` or the pinned reference checkout.
- Observation versus inference: the live guard compares filesystem identity with `Path.samefile`
  before any source read or output write. Inference: a symlink whose target is the source should
  trigger that guard even though its pathname differs. The test pins this synthetic filesystem case.
- Current checkout: `main`, HEAD `a8aeb7e2676f54da1b21d2fdba5757e2e1509c14`, matching `origin/main`.
  The only working-tree changes are pre-existing user-owned `native/api/CMakeLists.txt` and
  untracked `native/api/host/`; neither is part of this task.
- Index/graph checks: `python3 scripts/project_index.py` and
  `python3 scripts/project_index.py --check` exited 0; 24,080 folders are indexed/current and the
  `docs/checkpoints` entry is 436. `trail build --check` exited 0 with the wiring graph in sync at
  3,615 nodes / 1,240 files; the deep meaning layer remains unbuilt. Output is retained in
  `local/validation/20261022-root-fixture-baselines/re01-sidecar-symlink-work.log`, SHA-256
  `55b0fd602179d17772c4ea204f192bfeef385e52d5461e7d53d941b36882ade0`.
- `git diff --check` exited 0.
- Files changed by claim: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, this checkpoint, and
  generated `docs/FOLDERS.md`. No implementation edit has started.
- Claim publication: commit `6cd08912d27d88f3bf4a4c9690043678c70e8ab3` was pushed to `origin/main`.
  Staged audit accepted 5 paths, full audit accepted 1,707; fresh fetch/divergence returned 0/0 and
  post-push full audit accepted 1,707. Output is in
  `local/validation/20261022-root-fixture-baselines/re01-sidecar-symlink-publication.log`, SHA-256
  `7b602866ffa74d0374c95e8c386f0f23313962157d4d11c88fb0da96347aac87`.
- Remaining risks: symlink creation is supported in this local macOS filesystem; no live game file
  or runtime is read. The test does not establish concurrent filesystem-mutation behavior.
- Exact next action: after publishing this claim, `/root/next_fixture_scout_3` may edit only
  `scripts/test_project_tools.py`, add the one symlink-alias test, run the focused suite (expected
  12 tests), and report diff, output, hashes, and attempt count. Root will replay and record
  completion. Attempt count at claim: 0 implementation tries.
