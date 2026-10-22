# Claim checkpoint: APP-01-AUDIO-READINESS-GATE-FIXTURE-01

- Time / timezone: 2026-10-22 01:00 CDT (America/Chicago).
- Agent / integrator: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`).
- Task / parent / status: `APP-01-AUDIO-READINESS-GATE-FIXTURE-01`, child of `APP-01`; `ACTIVE`;
  0 failed implementation tries, no implementation attempt yet.
- Source/spec review: `launcher/README.md` documents injected `HostChecks` and the readiness report;
  `docs/VALIDATION.md` G4 requires honest launcher readiness; `ROADMAP.md` identifies the native
  launcher gate; `docs/INDEX.md` establishes live source as implementation truth. `LauncherModel`
  and `ReadinessReport` both block start unless all readiness rows are ready. Existing
  `hostChecksStillGatePlay` only isolates Metal; existing default no-start case is caused by an
  unready core, so neither isolates missing audio.
- Baseline and source hashes: `launcher/Tests/LauncherCoreTests/LauncherModelTests.swift`
  SHA-256 `e8d429a647414c269108e5baeb49765df36a0f968d0d453a0b80c4055095c5c1`; unchanged
  `launcher/Sources/LauncherCore/LauncherModel.swift` SHA-256
  `ae6ce2e1b33a83c2baae76efd63a3f2beacc0a62202c6a62ab9bb96644be6d51` and
  `launcher/Sources/LauncherCore/ReadinessReport.swift` SHA-256
  `170b03a7c24aadb3c799f364998ba81948438404e88eef3b07200ec210982e98`.
- Owned scope: one test method in `launcher/Tests/LauncherCoreTests/LauncherModelTests.swift`;
  root retains ownership of shared docs. Production Swift sources, native/api user changes, and
  other task files are outside the scope.
- Hypothesis / expected result: with assets, native core, and Metal ready but audio set to
  `.missing("No default audio output device")`, `model.report.canStart` is false, the audio row
  exposes the missing reason, presentation identifies Audio output as the blocker, `startCore()`
  leaves the model idle, and `ScriptedCoreClient.count("start")` remains zero.
- Exact baseline command, CWD and environment: from `/Volumes/Extra/SoC Mac Native`,
  `swift test --package-path launcher --scratch-path local/build/launcher --filter LauncherModelValidationTests`.
- Exit / observed baseline: exit 0; 8 tests in 1 suite pass. Log
  `local/validation/20261022-root-fixture-baselines/app01-audio-readiness-baseline.log` SHA-256
  `7d5c203fc6b89374e9379e38156b575f785266c9dcc130dfcb4f8e5e9cf98a6d`. Toolchain reported Swift
  6.4.0.34.1, target arm64-apple-macosx27.0.0; Xcode 27.1 (27A9275).
- Observation / inference and limits: baseline confirms current model validation tests pass. The
  task will add a synthetic injected-host-check regression only; it will not exercise a physical
  audio device, CoreAudio output, audible content, actual game core, or gameplay.
- Failed tries / remaining risks: 0 failed implementation attempts. No alternate implementation
  approach is required; existing `makeModel` accepts an injected audio status and the scripted
  client counts `start()` calls.
- Exact next action: implement one isolated test in the owned test file, rerun the same focused
  command, inspect the diff, refresh project index and Trail, then record the completion checkpoint.
- `HANDOFF.md`, `docs/TASKS.md`, and `docs/EVIDENCE.md` contain this claim; `INSTALL_LOG.md` is
  unchanged. No code edit has started.
