# Checkpoint: APP-01-DIAGNOSTICS-SUMMARY-01 — deterministic diagnostics text

- Time / timezone: 2026-10-21 00:20 CDT (America/Chicago).
- Agent and integrator: worker `app01-launcher`; integrator merges into `HANDOFF.md`/`docs/TASKS.md`.
- Task / gate / status: `APP-01-DIAGNOSTICS-SUMMARY-01` = `FIXTURE_VERIFIED` (pure-function tests).
  No gate advanced; no runtime UI or clipboard/Report-copy path exists yet.
- Prerequisite evidence / input and source hashes: baseline suite 19 tests / 3 suites. No diagnostics
  renderer existed; `ReadinessReport.playBlockedSummary` was only consumed by `ContentView.statusText`.
- Owned / changed files:
  - new `launcher/Sources/LauncherCore/DiagnosticsSummary.swift`
    sha256 `1a082785137dc58e42c5b1a47ed3790df97499be87b215f46ae731e40d85ef0c`
  - new `launcher/Tests/LauncherCoreTests/DiagnosticsSummaryTests.swift`
    sha256 `2388fad5069dcaf8dc5df1990f860d3096466ef4b2f8a9d987990f31f55d492a`
  - `launcher/README.md` (exact line formats, order, NOT REPORTED rule)
- Hypothesis and expected result: a pure renderer over `ReadinessReport` can emit four fixed-order
  subsystem lines (READY / MISSING / NOT BUILT) plus one overall Play line, and can be stricter than
  `canStart` so an empty or partial report is never called ready. Expected: 6 new tests, 19 existing
  still pass.
- Design decisions (recorded because they are policy, not incidental):
  - order is `DiagnosticsSummary.subsystemOrder = [.assets, .nativeCore, .metal, .audio]`;
  - a kind absent from the report renders `NOT REPORTED — no result for this subsystem` instead of
    being dropped (no omitted lines) or being folded into MISSING (which would merge "not checked"
    with "checked and failed", against the repo's honesty rule);
  - duplicate kinds use the first occurrence (same rule as `playBlockedSummary`), so no duplicate
    lines;
  - `isStartable` requires all four kinds present **and** `report.canStart`, because
    `ReadinessReport.canStart` is vacuously true for an empty report;
  - empty report overall line: `Play is disabled: no subsystem reported ready.`;
  - startable overall line: `Play is ready: every subsystem reported READY.`;
  - otherwise `Play is disabled: <playBlockedSummary>`, falling back to
    `Play is disabled: no result reported for <labels>.` when the summary is nil.
- Exact command, CWD and nonsecret environment: CWD `/Volumes/Extra/SoC Mac Native`;
  `swift build --package-path launcher --scratch-path local/build/launcher-agent-c`;
  `swift test --package-path launcher --scratch-path local/build/launcher-agent-c`.
  Apple Swift 6.4, target `arm64-apple-macosx27.0.0`, macOS 27.0.1.
- Exit / observed result: build exit 0 (zero warnings). `swift test` exit 0:
  **42 tests in 7 suites passed**, including `Suite DiagnosticsSummaryTests passed after 0.001
  seconds` (6 tests: fixed ordering with a mixed report, all-ready, empty, partial, duplicate/omitted
  lines, newline join + determinism). Logs `local/build/launcher-agent-c/build-final.log` and
  `test-final.log`.
- Negative control (required): temporarily swapped `subsystemOrder` to
  `[.nativeCore, .assets, .metal, .audio]`; `swift test ... --filter DiagnosticsSummaryTests` exited
  non-zero with `Test run with 6 tests in 1 suite failed ... with 3 issues` (mixed, all-ready and
  empty ordering assertions). Restored from
  `local/build/launcher-agent-c/negctl/DiagnosticsSummary.swift.bak`; `diff -q` clean.
- Observation versus inference / coverage limitations: rendering, ordering, emptiness and duplicate
  handling are observed via assertions on exact strings. Not observed: no SwiftUI view displays this
  text yet, nothing copies it to the pasteboard, and no user has read it. Line content embeds the
  verbatim NOT BUILT message, so it can be long; truncation/wrapping behaviour is untested.
  "NOT REPORTED" is a fourth word beyond the READY/MISSING/NOT BUILT set, used only for a kind the
  report omits.
- Cumulative failed tries and distinct hypotheses: 0 implementation tries. One deliberate
  negative-control failure, restored.
- Remaining risks / blockers: `isStartable` intentionally diverges from `ReadinessReport.canStart`
  for incomplete reports; if a future caller treats `canStart` as sufficient it could show Play
  enabled while this report says disabled. No Diagnostics UI, bug-report attachment, or file export
  exists. String stability is enforced by exact-equality tests.
- Next exact action: integrator merge. A later UI task can surface `DiagnosticsSummary.text(for:)`
  behind a "Copy diagnostics" button; that task must not claim this checkpoint's tests cover the UI.
- HANDOFF.md / TASKS / EVIDENCE / INSTALL_LOG / indexes updated: **not by this worker** (shared docs
  are integrator-owned). No install or dependency change.
