# Checkpoint: APP-01-ASSET-VALIDATOR-BROKEN-SYMLINK-MISSING-FIXTURE-01 — claimed

- Time / timezone: 2026-10-22 02:20 CDT (America/Chicago)
- Agent and integrator: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`);
  planned implementation owner `/root/next_fixture_scout_3`.
- Task / gate / status: `APP-01-ASSET-VALIDATOR-BROKEN-SYMLINK-MISSING-FIXTURE-01`, child of
  `APP-01-ASSET-VALIDATOR-REPORT-01`; `ACTIVE`, implementation not started, zero failed tries.
- Prerequisite evidence / input and source hashes: baseline
  `launcher/Tests/LauncherCoreTests/AssetValidationReportTests.swift` SHA256
  `02d5fc902015a47940d7ade7d71929ec79a7d6ef146ffcc45d9d39258fd3587c`; unchanged
  `launcher/Sources/LauncherCore/AssetValidator.swift` SHA256
  `234f62920654226fd2d6239d7f8530802bc52fc9a26d38a698dab8838bbd203e`; unchanged
  `launcher/Package.swift` SHA256
  `3ac370e6dae8491cb4abc0b694c9fd6f041f959b0c2a736e93f0c94ddda1f368`.
- Owned / changed files: one synthetic test in
  `launcher/Tests/LauncherCoreTests/AssetValidationReportTests.swift`; no production change.
  Create a dangling symlink at required archive `pak0.psarc`, whose target path does not exist.
- Hypothesis and expected result: `FileManager.fileExists(atPath:isDirectory:)` treats the dangling
  archive symlink as absent. `AssetValidator.report` should return exactly
  `problems == ["pak0.psarc not found"]` and `missingArchives == ["pak0.psarc"]`.
- Exact command, CWD and nonsecret environment: CWD `/Volumes/Extra/SoC Mac Native`.
  `swift test --package-path launcher --scratch-path /tmp/sotc-root-app01-assetreport-baseline-20261022 --filter AssetValidationReportTests`.
- Exit / observed result: exit 0; 7 `AssetValidationReportTests` pass in a fresh SwiftPM scratch
  directory. This is the baseline before the proposed assertion.
- Log, capture and REA evidence paths/IDs: root baseline log
  `local/validation/20261022-root-fixture-baselines/app01-asset-report-dangling-symlink-root-baseline.log`,
  SHA256 `755d14973a7a7ecbd24519301c4b564d06d32ee9d3c89a2e44c54e635ff780bf`; Graft query log
  `local/validation/20261022-root-fixture-baselines/app01-asset-report-dangling-symlink-root-query.log`
  (hash recorded in E287).
- Observation versus inference / coverage limitations: existing tests cover an absent archive path
  and a symlink to a directory, but not a dangling symlink. The proposed test isolates the missing
  target case. This establishes only a synthetic temporary-filesystem classification, not archive
  integrity, game asset readiness, or runtime behavior.
- Cumulative failed tries and distinct hypotheses: zero implementation attempts and zero failed
  tries.
- Remaining risks / blockers: no blocker to the narrow fixture; `AssetValidator.swift` is not to
  change.
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
- Next exact action: finish publishing the verification record, then delegate only
  `AssetValidationReportTests.swift` to `/root/next_fixture_scout_3` for implementation and focused
  verification.
- HANDOFF.md / TASKS / EVIDENCE / INSTALL_LOG / indexes updated: HANDOFF, TASKS, EVIDENCE, and
  `docs/FOLDERS.md` are updated; `python3 scripts/project_index.py` and its check report 26,499
  folders including 446 checkpoint files; `trail build --check` reports 3,617 nodes / 1,240 files.
  Combined index/Trail log is
  `local/validation/20261022-root-fixture-baselines/root-fixture-pair-index-trail.log`, SHA256
  `512a3855aedd451db0bcbfa8a5ee3b3fcd76cc1a76f9591e793bf93bd6757d15`. No install occurred.
