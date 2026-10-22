# Checkpoint: APP-01-ASSET-VALIDATOR-BROKEN-SYMLINK-MISSING-FIXTURE-01 — fixture verified

- Time / timezone: 2026-10-22 02:28 CDT (America/Chicago)
- Agent and integrator: implementation `/root/next_fixture_scout_3`; integrator `/root` (Codex
  desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`).
- Task / gate / status: `APP-01-ASSET-VALIDATOR-BROKEN-SYMLINK-MISSING-FIXTURE-01`, child of
  `APP-01-ASSET-VALIDATOR-REPORT-01`; `FIXTURE_VERIFIED`, one successful implementation attempt,
  zero failed tries.
- Prerequisite evidence / input and source hashes: baseline test SHA256
  `02d5fc902015a47940d7ade7d71929ec79a7d6ef146ffcc45d9d39258fd3587c`; unchanged
  `launcher/Sources/LauncherCore/AssetValidator.swift` SHA256
  `234f62920654226fd2d6239d7f8530802bc52fc9a26d38a698dab8838bbd203e`; unchanged
  `launcher/Package.swift` SHA256
  `3ac370e6dae8491cb4abc0b694c9fd6f041f959b0c2a736e93f0c94ddda1f368`; final test SHA256
  `256f444c59b195d678778708d19bc4dafa18ad3592d85100486a58951e8a5246`.
- Owned / changed files: only `launcher/Tests/LauncherCoreTests/AssetValidationReportTests.swift`;
  added `danglingSymlinkNamedLikeArchiveIsReportedAsMissing()`.
- Hypothesis and expected result: a symlink named for required archive `pak0.psarc`, pointing to a
  nonexistent target, is reported as missing. Assertions require exactly
  `problems == ["pak0.psarc not found"]` and `missingArchives == ["pak0.psarc"]`.
- Exact command, CWD and nonsecret environment: CWD `/Volumes/Extra/SoC Mac Native`. Worker:
  `swift test --package-path launcher --scratch-path local/build/app01-asset-report-dangling-symlink-worker-20261022 --filter AssetValidationReportTests`.
  Root independent replay:
  `swift test --package-path launcher --scratch-path local/build/app01-asset-report-dangling-symlink-root-20261022 --filter AssetValidationReportTests`.
  Both use fresh SwiftPM scratch directories.
- Exit / observed result: worker and root commands exit 0; each reports 8 tests in one suite passed.
  Worker and root `git diff --check` exit 0.
- Log, capture and REA evidence paths/IDs: worker log
  `local/validation/20261022-root-fixture-baselines/app01-asset-report-dangling-symlink-worker.log`
  SHA256 `d72efaf4273fc6e98aca0088f385cf30c6ec77b7c6a9f6b13450fe049526ced9`; root replay log
  `local/validation/20261022-root-fixture-baselines/app01-asset-report-dangling-symlink-root-replay.log`
  SHA256 `09e99813821b5ef6ebad5af137e899aaea05735bcfe4fbcaec7eb7a67a6e1b75`; Trail query log
  `local/validation/20261022-root-fixture-baselines/app01-asset-report-dangling-symlink-root-trail.log`
  SHA256 `172cb60924de1b798d31aacb258a8419247d52b32a5628488dce9c5256298532`; Trail resolves the
  new test method at `AssetValidationReportTests.swift:L110-L123`.
- Observation versus inference / coverage limitations: the synthetic dangling symlink is reported
  missing, and the two expected arrays match exactly. `AssetValidator.swift` is unchanged. This
  establishes temporary-filesystem classification only, not PSARC validity, real asset readiness,
  launcher runtime, boot, audio, or gameplay.
- Cumulative failed tries and distinct hypotheses: one successful implementation hypothesis;
  zero failed tries.
- Remaining risks / blockers: no blocker to this fixture. Real game assets and all native-port
  runtime gates remain unverified.
- Project-index / Trail checks: `python3 scripts/project_index.py` and
  `python3 scripts/project_index.py --check` pass at 28,897 folders including 448 checkpoint
  files; `trail build --check` passes at 3,618 nodes / 1,240 files. Combined log
  `local/validation/20261022-root-fixture-baselines/app01-srv01-claim-index-trail.log` SHA256
  `4290efc2b8ace98dfd871aed2bdca43cf65b63211257105b75183e331bcf4cd3`.
- Next exact action: publish this completion with the region-split metadata claim using allowlisted
  audits, then dispatch only the region-split test file.
- HANDOFF.md / TASKS / EVIDENCE / INSTALL_LOG / indexes updated: HANDOFF, TASKS, EVIDENCE, and
  `docs/FOLDERS.md` are updated. No install occurred.
