# Checkpoint: Round 60 synthetic fixtures verified

- Local date/time: 2026-10-21 13:54 CDT (America/Chicago).
- Agent ID: `/root` (integrator); worker reports from `/root/launcher_review`,
  `/root/review_host_proto`, and `/root/slot23_raw_xrefs`.
- Claims were published in `4e5779a`; worker-dispatch checkpoint was published in `beea437`.
- Status: all three claimed tasks complete, 0 failed fixture attempts each.

## APP-01-ASSET-VALIDATOR-SYMLINKED-DIRECTORY-REJECTION-FIXTURE-01

- Changed only `launcher/Tests/LauncherCoreTests/AssetValidationReportTests.swift` for this task;
  SHA-256 `02d5fc902015a47940d7ade7d71929ec79a7d6ef146ffcc45d9d39258fd3587c`. Production file
  `launcher/Sources/LauncherCore/AssetValidator.swift` is unchanged, SHA-256
  `234f62920654226fd2d6239d7f8530802bc52fc9a26d38a698dab8838bbd203e`.
- Worker command `swift test --package-path launcher --scratch-path local/build/launcher --filter
  AssetValidationReportTests` exited 0 with 7 tests in 1 suite. Log
  `local/validation/app01-asset-validator-symlinked-directory/attempt-1.log`, SHA-256
  `596a4c2538cd8fa42623627f69c70d8a95d87da22059d6b0cbfef7018f271abe`.
- Root replay ran the same command and exited 0 with 7 tests in 1 suite. Log
  `local/validation/root-round60-replay/app01.log`, SHA-256
  `d4ad0a1bc4fa187a86a971aab1baef6705feca3a26274cf5199644227efc783b`.
- Observation: a required `pak0.psarc` symlink to a directory is rejected with
  `problems == ["pak0.psarc not found"]` and `missingArchives == ["pak0.psarc"]`. Synthetic
  temporary filesystem only; no live launcher, archive import, or game behavior.

## AUD-01-CLASSIFIER-LZMA-OFFSET4-FIXTURE-01

- Changed only `scripts/test_audio_headers.py` for this task; SHA-256
  `e013de229443cc49dd3fff09bbb743bf5eb61c4c14b90dcf34e03b2cd8f3f9f4`. Production file
  `scripts/classify_audio_headers.py` is unchanged, SHA-256
  `624a51336859bab8aef2e7e7880b16d1282c9e883f459de05c8e3c1c4320cd77`.
- Worker command `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_audio_headers.py` exited 0
  with 23 tests. Log `local/validation/aud01-classifier-lzma-offset4/attempt-1.log`, SHA-256
  `b6102bd16e5928fe3d6d717be042b7db700fa15fa3eddabd5d6a7b6641c73368`.
- Root replay ran the same command and exited 0 with 23 tests. Log
  `local/validation/root-round60-replay/aud01.log`, SHA-256
  `eb79a8348ec4cfd7967542f2c163b8237133c98cd3c9bef8d756db2aa2cfd5f1`.
- Observation: the existing LZMA probe accepts a synthetic compressed 64-byte payload after a
  four-byte prefix. Synthetic standard-library compression/decompression only; no real asset
  classification or game audio.

## SRV-01-REGION-LIST-UNCOUNTED-UNLINK-FIXTURE-01

- Changed only `native/tests/test_region_list.cpp` for this task; SHA-256
  `3ca51b59317206c2fb76c417a140e0917788094dc1e4ab7d9e417dfc7dd7193c`. Production file
  `native/core/region_list.cpp` is unchanged, SHA-256
  `a042ec5af910f31579c43b1897d6d35e5e535c25da0b031f32e7973f1c8e3bda`.
- Worker build command `cmake --build local/build/native --target test_region_list` exited 0. Log
  `local/validation/srv01-region-list-uncounted-unlink/build-attempt-1.log`, SHA-256
  `1d4ab0e570445167d222b7ff2b7f27d0056a818212276437085555125f8c5a28`.
- Worker CTest command `ctest --test-dir local/build/native -R '^region_list$'` exited 0, 1/1.
  Log `local/validation/srv01-region-list-uncounted-unlink/attempt-1.log`, SHA-256
  `a08a316431708a57ca8622ce9a6c9415fd671ff08f9e513fbbbadeb0cb6d52f4`.
- Root replay ran both commands with exit 0; focused CTest passed 1/1. Logs
  `local/validation/root-round60-replay/srv01-build.log` (SHA-256
  `e9fa818ca115f2868ef295edd6b03a891bc133965da5d4aa3b8a2c4f924d9b9e`) and
  `local/validation/root-round60-replay/srv01-ctest.log` (SHA-256
  `a4f8c7c021e2e9496ffd5daa75dab3bd6afc5e33b04c683437e57d71ca37a48f`).
- Observation: uncounted unlink of B in synthetic A-B-C leaves A-C and self-links B. This does
  not establish allocator integration or game behavior.

## Shared checks and limits

- `git diff --check` exited 0. The three production sources above match their claim-time hashes;
  worker changes are limited to the three claimed test files. Existing user-owned
  `native/api/CMakeLists.txt` and untracked `native/api/host/` remain untouched and excluded.
- `trail build` exited 0, refreshing the wiring graph to 3,555 nodes / 4,801 edges, parsing 3 files
  and replaying 1,233 from cache. Log `local/validation/root-round60-replay/trail-build.log`,
  SHA-256 `739731786b932fa322733040363c020fc011d722ea4615eaec6fa28e6aafaac7`.
- `trail build --check` exited 0; wiring graph in sync at 3,555 nodes. The optional deep concept
  layer remains unbuilt (0% meaning tier), so this claims wiring freshness only. Log
  `local/validation/root-round60-replay/trail-check.log`, SHA-256
  `baf6a6fa0154df5f35c9d48b57ad6af23e8d2bb936f9c9164f5238534e936e42`.
- `docs/TASKS.md` marks the tasks `FIXTURE_VERIFIED`; `docs/EVIDENCE.md` adds E178–E180. No
  `INSTALL_LOG.md` facts changed. Game gates remain G0 PARTIAL and G1–G6 NOT RUN; no cold boot,
  real game audio, save/reload, Metal gameplay, live launcher behavior, or real asset classification
  was tested.
- `python3 scripts/project_index.py` exited 0 and indexed 14,416 folders; `docs/FOLDERS.md` now
  records 355 checkpoint files. Log `local/validation/root-round60-replay/project-index.log`,
  SHA-256 `07954259c59a360103bc473829a9fef45d72b51c242fc87544c76506b1d3e44f`.
- Pre-push staged path audit accepted 8 paths; log
  `local/validation/root-round60-replay/audit-staged.log`, SHA-256
  `0447dc488d329d4c4b55286a29d73c02e87971a5d6618503f9c214b578758f4e`. Full audit accepted 1,622
  paths; log `local/validation/root-round60-replay/audit-all.log`, SHA-256
  `eb127a95a8c8da17da30c013461297031bea680cbeb45ada7610bf30102bd993`. Cached and working
  `git diff --check` exited 0. Only the three claimed test files and five docs paths are staged;
  user-owned `native/api` work is unstaged.
- Exact next action: fetch `origin/main`; if it has not advanced, commit the allowlisted paths and
  push `main`.
