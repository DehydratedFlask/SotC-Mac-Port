# Checkpoint: SRV-01-REGION-LIST-UNCOUNTED-HEAD-INSERT-FIXTURE-01 — fixture verified

- Time / timezone: 2026-10-22 02:20 CDT (America/Chicago)
- Agent and integrator: implementation `/root/next_fixture_scout_2`; integrator `/root`
  (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`).
- Task / gate / status: `SRV-01-REGION-LIST-UNCOUNTED-HEAD-INSERT-FIXTURE-01`, child of `SRV-01`;
  `FIXTURE_VERIFIED`, one successful implementation attempt, zero failed tries.
- Prerequisite evidence / input and source hashes: baseline test file SHA256
  `d3c929fabde374331c9cbde725508c9b7c0896f671c0d1b180bf848f033b10e8`; unchanged
  `native/core/region_list.cpp` / `region_list.hpp` SHA256
  `a042ec5af910f31579c43b1897d6d35e5e535c25da0b031f32e7973f1c8e3bda` /
  `3f36189b27178b4ebea6135067e96c29d4839be9e255cddb6e51cd4b7f65fb69`. Final test SHA256
  `222c18d5b7e44f94044ce9c85fa50f5fe669c83c5c0b04682a195bd89823755c`.
- Owned / changed files: only `native/tests/test_region_list.cpp` for the fixture; one case inserts
  a node before an existing head while omitting the optional counter pointer.
- Hypothesis and expected result: `list_insert_at_head` should set the new list slot and links,
  arena predecessor, and owner even when its optional `alloc_count` argument is omitted. The
  synthetic assertions pass.
- Exact command, CWD and nonsecret environment: CWD `/Volumes/Extra/SoC Mac Native`.
  Worker: `cmake --build local/build/native --target test_region_list` and
  `ctest --test-dir local/build/native --output-on-failure -R '^region_list$'`.
  Root independent replay: `cmake -S native -B local/build/native-root-srv01-region-list-uncounted-head-insert -G Ninja -DCMAKE_BUILD_TYPE=Debug`, then
  `cmake --build local/build/native-root-srv01-region-list-uncounted-head-insert --target test_region_list` and
  `ctest --test-dir local/build/native-root-srv01-region-list-uncounted-head-insert --output-on-failure -R '^region_list$'`.
  Also `git diff --check` and `trail build --check`.
- Exit / observed result: all commands exited 0. Worker and fresh root build compile the target;
  worker and root focused CTest each pass `region_list` 1/1. Root configured with AppleClang
  21.0.0.21000334 and Ninja and rebuilt the target from a fresh build directory.
- Log, capture and REA evidence paths/IDs: baseline build/CTest logs SHA256
  `e9fa818ca115f2868ef295edd6b03a891bc133da5d4aa3b8a2c4f924d9b9e` /
  `3eb9e589d96a006ab4a41d324f1f862e3d3ca8e635b8ef50ae917137137c2ade`; worker build/CTest logs
  `local/validation/20261022-root-fixture-baselines/srv01-region-list-uncounted-head-insert-worker-build.log`
  / `srv01-region-list-uncounted-head-insert-worker-ctest.log`, SHA256
  `1d4ab0e570445167d222b7ff2b7f27d0056a818212276437085555125f8c5a28` /
  `d3ff54ac577346ed7f46c75eddc9c44cbb954e22b720c41d8ace708c0b85b320`;
  root configure/build/CTest logs
  `local/validation/20261022-root-fixture-baselines/srv01-region-list-uncounted-head-insert-root-configure.log`,
  `srv01-region-list-uncounted-head-insert-root-build.log`, and
  `srv01-region-list-uncounted-head-insert-root-ctest.log`, SHA256
  `f27fe4bac25d796b9bd13ef815f960ae852efb25af9c7cd8d36a810e6fba1b1c` /
  `1ed98ed9cb3a694a42cd41894c4a0a29d012b528a028684080166a1d440e3f80` /
  `661c36bb41e44f9dfcc4b4c0aeef5b51d8c417d6a9eca29750749b4e68d448f4`; Trail unique-comment query
  log SHA256 `3b5235306e4d9db95b94da74f562564a2e52834966e444e4e1ede0d498c50c99`; root `git diff --check`
  / Trail graph-check logs SHA256 `f60c6598443dc839f82abf6314275242dd93670e12326097e2fe1dccac837615` /
  `3d59c17d0b14b3a66fc4896dd10d96d07c8a6ef0d1dedbfc5e3867d14015c8dc`.
- Observation versus inference / coverage limitations: the call omits `alloc_count` and passes all
  synthetic intrusive-list assertions. The production helper is unchanged. This does not exercise
  allocator runtime, a recovered game call path, or any real game behavior.
- Cumulative failed tries and distinct hypotheses: one successful implementation hypothesis;
  zero failed tries.
- Remaining risks / blockers: no blocker to this fixture. Parent `SRV-01` remains partial; all
  real-game acceptance gates remain open.
- Publication: commit `c434246b3ec9d3656289ac08525a452c88d065ec` pushed to `origin/main`.
  Staged and full audits accepted 7/1,717 paths; fresh fetch showed 0/0 divergence; post-push
  explicit and full audits accepted 7/1,717. Pre-push staged/full and post-push explicit/full log
  hashes are `644b4d768281cc0756d4c44fcce76f71955e718c92cbe4c5d2ab06ad337c8670` /
  `d0e6a66c34674cd00c30738c208ae2ec2d76535397443c6be86d13b90e9fbc07`; identical post-push log
  hashes are recorded in E288.
- Project-index/Trail recheck: the first `python3 scripts/project_index.py --check` reported stale
  `local/indexes/folders.json`; `python3 scripts/project_index.py` regenerated it, then
  `python3 scripts/project_index.py --check` and `trail build --check` passed. Final combined log
  SHA256 is `0cee8324d2a2578771dba0d3f61f64553e0a832eb3fa8e1736c2b8e83208bea7`.
- Next exact action: finish publishing the verification record, then dispatch only
  `AssetValidationReportTests.swift` to `/root/next_fixture_scout_3`.
- HANDOFF.md / TASKS / EVIDENCE / INSTALL_LOG / indexes updated: HANDOFF, TASKS, EVIDENCE, and
  `docs/FOLDERS.md` are updated; `python3 scripts/project_index.py` and its check report 26,499
  folders including 446 checkpoint files; `trail build --check` reports 3,617 nodes / 1,240 files.
  Combined index/Trail log is
  `local/validation/20261022-root-fixture-baselines/root-fixture-pair-index-trail.log`, SHA256
  `512a3855aedd451db0bcbfa8a5ee3b3fcd76cc1a76f9591e793bf93bd6757d15`. No install occurred.
