# Checkpoint - APP-01-LAUNCHER-CORECLIENT-01: LauncherModel routed through CoreClient

- Date: 2026-10-21 (local), agent: agent-E (Claude Sonnet 5.5), task: APP-01-LAUNCHER-CORECLIENT-01.
- Status: **PARTIAL** (model/UI wiring + unit tests done; no runtime UI/VoiceOver proof; no engine). Attempt 1, no failed tries.
- Not touched by design: `native/`, `tools/`, `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `INSTALL_LOG.md`. Nothing committed or pushed.

## Files

New (`launcher/Sources/LauncherCore/`): `LauncherModel.swift` (moved from `SoCLauncher/` and rewritten), `CoreReadiness.swift`
(`CoreReadiness`, `CoreDiagnostics`), `LauncherPresentation.swift`, `UnavailableCoreClient.swift`.
Deleted: `launcher/Sources/SoCLauncher/LauncherModel.swift`.
Edited: `launcher/Package.swift` (SoCLauncher now depends on `LauncherNativeCore`), `SoCLauncher/SoCLauncherApp.swift`
(injects `NativeCoreClient`, falls back to `UnavailableCoreClient`), `SoCLauncher/ContentView.swift` (status area from
`LauncherPresentation`, accessible label/value/hints, Diagnostics disclosure + Copy), `launcher/README.md`.
Tests: new `Tests/LauncherCoreTests/LauncherModelTests.swift` (28 tests, scripted `ScriptedCoreClient`), +1 test in
`Tests/LauncherNativeCoreTests/NativeCoreClientTests.swift` (model over the real C ABI).

## Commands and results

| Command | Result |
|---|---|
| `swift build --package-path launcher --scratch-path local/build/launcher-agent-e` | exit 0, SoCLauncher app target built |
| `swift test --package-path launcher --scratch-path local/build/launcher-agent-e` (x5) | exit 0 each time: 80 core tests (12 suites) + 9 native-core tests = **89** (60 pre-existing unchanged + 29 new); log `local/build/launcher-agent-e-test.log` |
| mutation: `startCore()` sets `.running` right after `start()` returns / on refusal | 4+ tests FAIL (`engineNotImplementedIsFailed...`, `acceptedStartWithNonRunningStatusIsNotRunning`, poll tests); reverted, suite green again |
| launch `local/build/launcher-agent-e/out/Products/Debug/SoCLauncher` for 4 s, kill | process stayed alive, stderr empty (startup smoke only; no window inspected) |

## State mapping (all asserted)

- No folder: assets row MISSING, `validate` never called. Bad assets: row = core's first problem. Throwing validate/coverage: assets `Could not check...`, native core NOT BUILT `could not be queried`.
- Engine row `.ready` only if `validation.engineReady && engineError == .ok`; otherwise NOT BUILT, Play disabled, state `.idle`.
- `start()` refused (`ENGINE_NOT_IMPLEMENTED`): `.failed`, tone `.unavailable`, no polling, never `.running`; other refusal: `.failed`, tone `.failure`.
- `start()` accepted: `.running` only if `status()` says running with `engineImplemented`. Accepted + ready/idle/validating => `.failed`; running without engine => `.failed`; `starting` => `.starting` then polling flips to `.running` only when the core says so.
- Polling: core calls recorded with `Thread.isMainThread == false`; core failing / stopping alone / unreachable while running => `.failed`; stale answers after a settled state are dropped (bug found and fixed by `requestedStopIsNotAFailure`).
- Stop: success => `.ready`; failure => `.failed("Could not stop...")`; no-op when not running.

## Honest limits

- Observed: unit tests and a build; the app process starts. NOT observed: the window, VoiceOver reading, Finder launch, bundling/signing. Accessibility copy is unit-tested strings plus SwiftUI modifiers in code.
- With the real `NativeCoreClient` Play is always disabled (engine row NOT BUILT), so the start-refusal presentation is only reachable through tests that script a client claiming readiness. `SoCGameCore` child-process client does not exist.
- Metal/audio rows still use `SystemChecks` (not the core). `SystemChecks.nativeCore` is unused by the model but kept (its tests remain).
- The SwiftPM facade is built without `sotc_core`: constructor prefix reads `NOT LINKED in this build`.
- `scripts/audit_publish_paths.py` and `scripts/project_index.py` not run (integrator, with the commit). The working tree also contains other agents' unstaged `native/` and `tools/` files.

## Next exact actions

1. Integrator: run `python3 scripts/project_index.py`, stage `launcher/` + this checkpoint only, run the path audit, commit.
2. Run the app from Xcode/Finder and record a real VoiceOver pass over status area, readiness rows and Diagnostics.
3. When `SoCGameCore` exists, add a child-process `CoreClient`; no `LauncherModel` change expected.
