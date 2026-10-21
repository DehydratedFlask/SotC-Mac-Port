# Checkpoint: APP-01-ASSET-VALIDATOR-REPORT-01 — ordered, complete asset problems

- Time / timezone: 2026-10-21 00:20 CDT (America/Chicago).
- Agent and integrator: worker `app01-launcher`; integrator merges into `HANDOFF.md`/`docs/TASKS.md`.
- Task / gate / status: `APP-01-ASSET-VALIDATOR-REPORT-01` = `FIXTURE_VERIFIED` (temp-directory
  tests). No gate advanced; this is the launcher shell's local archive check, not PSARC parsing and
  not a boot/save claim.
- Prerequisite evidence / input and source hashes: baseline `launcher/Sources/LauncherCore/
  AssetValidator.swift` returned only `ReadinessItem.Status` with `problems.joined(separator: "; ")`
  and had no per-archive list. Existing `AssetValidatorTests` (8 tests) already covered ready set,
  missing archive, wrong magic, and the directory-named-like-an-archive case; those tests were left
  untouched and still pass.
- Owned / changed files:
  - `launcher/Sources/LauncherCore/AssetValidator.swift`
    sha256 `234f62920654226fd2d6239d7f8530802bc52fc9a26d38a698dab8838bbd203e`
  - new `launcher/Tests/LauncherCoreTests/AssetValidationReportTests.swift`
    sha256 `feff799812708b8098ab5b118170e2e52a3b39a59649527362cb9535d52a21cb`
  - `launcher/README.md`
- Hypothesis and expected result: extracting the ordered problem list (plus a `missingArchives`
  list) behind `report(directory:)`, with `validate(directory:)` delegating to `report(...).status`,
  can add structure without changing any `.ready`/`.missing` wording or the directory-not-a-file
  behaviour. Expected: same 8 old tests pass, 5 new tests pass.
- Change detail: `AssetValidationReport(problems:missingArchives:)` (Equatable, Sendable) plus
  `isReady` and `status`. Order is `AssetValidator.requiredArchives` order, one problem per defective
  archive, wording unchanged (`<name> not found`, `<name> is not readable`,
  `<name> is not a PSARC archive`). `missingArchives` collects only "no regular file at the expected
  path" — a directory wearing the archive name counts as missing, an existing wrong-magic file does
  not (it exists, it is just not an archive).
- Exact command, CWD and nonsecret environment: CWD `/Volumes/Extra/SoC Mac Native`;
  `swift build --package-path launcher --scratch-path local/build/launcher-agent-c`;
  `swift test --package-path launcher --scratch-path local/build/launcher-agent-c`.
  Apple Swift 6.4, target `arm64-apple-macosx27.0.0`, macOS 27.0.1, test process uid 501 (non-root).
- Exit / observed result: build exit 0 (zero warnings). `swift test` exit 0:
  **42 tests in 7 suites passed**, including `Suite AssetValidationReportTests passed after 0.007
  seconds` (5 tests) and the untouched `Suite AssetValidatorTests passed after 0.040 seconds` (8
  tests). Logs `local/build/launcher-agent-c/build-final.log`, `test-final.log`.
- New test coverage actually asserted:
  - (a) empty directory → `problems == requiredArchives.map { "\($0) not found" }` (6, in order),
    `missingArchives == requiredArchives`, `status == .missing(joined)` and equal to
    `validate(directory:)`;
  - (b) all six present with one bad magic → exactly `["pak0.psarc is not a PSARC archive"]`,
    `missingArchives` empty;
  - (c) six valid `PSAR` files → `.ready`, empty `problems`, empty `missingArchives`;
  - (d) regression: a **directory** named `cachedFiles.psarc` that itself contains a valid-PSAR file
    is still reported `cachedFiles.psarc not found` (not accepted, not traversed);
  - (e) `chmod 000` on `cachedFiles.psarc` → `["cachedFiles.psarc is not readable"]`. The test first
    proves it cannot open the file (`FileHandle` throws) and only then asserts; if the mode is
    ignored (root / exotic filesystem) it returns early rather than faking a result. Environment
    here was uid 501, so the assertion really ran and passed — **not** NOT RUN.
- Negative control (required): temporarily dropped the `!isDirectory.boolValue` term from the
  existence guard; `swift test ... --filter AssetValidationReportTests` exited non-zero with
  `Test run with 5 tests in 1 suite failed ... with 3 issues`, all in
  `directoryNamedLikeArchiveIsNotAcceptedAsAFile` (problems, missingArchives and validate equality).
  Restored from `local/build/launcher-agent-c/negctl/AssetValidator.swift.bak`; `diff -q` clean.
- Observation versus inference / coverage limitations: all five cases are observed. The validator
  still only checks presence, regular-file-ness, readability and the 4-byte `PSAR` magic; it does not
  validate TOC, block sizes or archive contents, and this checkpoint makes no claim about PSARC
  correctness (that lives in `scripts/psarc.py`, untouched). Unreadable-directory (not file) cases and
  sandbox-denied paths were not exercised.
- Cumulative failed tries and distinct hypotheses: 0 implementation tries. One deliberate
  negative-control failure, restored.
- Remaining risks / blockers: `status` is recomputed from `problems`, so any future caller that edits
  the list must go through `AssetValidationReport` rather than mutating arrays; the type is immutable
  (`let`), so this is currently safe. The six-archive list is a hard-coded assumption from the
  extracted asset layout and is not re-derived from the disc.
- Next exact action: integrator merge. Optional follow-up (unclaimed): surface `missingArchives` in
  the folder-import alert so users learn *which* archives to re-extract.
- HANDOFF.md / TASKS / EVIDENCE / INSTALL_LOG / indexes updated: **not by this worker**. No install
  or dependency change.
