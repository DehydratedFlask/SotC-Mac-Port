# Checkpoint: APP-01-A11Y-LABELS-01 — accessibility copy for readiness

- Time / timezone: 2026-10-21 00:20 CDT (America/Chicago).
- Agent and integrator: worker `app01-launcher` (subagent of the orchestrator session); integrator
  still to merge this record into `HANDOFF.md` / `docs/TASKS.md`.
- Task / gate / status: `APP-01-A11Y-LABELS-01` = `FIXTURE_VERIFIED` (pure-function unit tests only).
  No gate advanced: G1 NOT PASSED, G4 NOT PASSED, G0 PARTIAL.
- Prerequisite evidence / input and source hashes: baseline launcher suite was 19 tests / 3 suites
  on the same scratch path before any edit (`swift test --package-path launcher --scratch-path
  local/build/launcher-agent-c`). Existing `ReadinessItem`/`ReadinessReport`/`ReadinessRow` had no
  accessibility helper — `ReadinessRow` carried its own inline `detail` copy, nothing else existed.
- Owned / changed files:
  - new `launcher/Sources/LauncherCore/ReadinessAccessibility.swift`
    sha256 `7e3578376896e03cf63a41ef050956774498426959f7c90b9d9654b351fbe734`
  - new `launcher/Tests/LauncherCoreTests/ReadinessAccessibilityTests.swift`
    sha256 `b25594380c80c78c10f8a12bfe7870703fc0d7f19d34345bf50b7a4f64866f54`
  - `launcher/Sources/SoCLauncher/ReadinessRow.swift` (two lines: row now supplies
    `.accessibilityLabel`/`.accessibilityValue` from the tested copy)
    sha256 `5300b19e91bfce81502fd6db63a94dd9df4c780144c462b43db5dc6188a1502c`
  - `launcher/README.md` (documented stable strings + role table)
- Hypothesis and expected result: an exhaustive, pure label/value table can express every
  `Kind × Status` combination without a catch-all, and `.notBuilt` can pass the underlying message
  through verbatim so VoiceOver never hears a softer state than the row. Expected: 5 new tests,
  existing 19 still pass.
- Exact command, CWD and nonsecret environment: CWD `/Volumes/Extra/SoC Mac Native`;
  `swift build --package-path launcher --scratch-path local/build/launcher-agent-c` then
  `swift test --package-path launcher --scratch-path local/build/launcher-agent-c`.
  Apple Swift 6.4 (swiftlang-6.4.0.34.1), target `arm64-apple-macosx27.0.0`, macOS 27.0.1, arm64.
- Exit / observed result: build exit 0 (`Build complete! (0.64 sec)`, zero warnings).
  `swift test` exit 0: **42 tests in 7 suites passed**, including
  `Suite ReadinessAccessibilityTests passed after 0.001 seconds` (5 tests).
  Logs: `local/build/launcher-agent-c/build-final.log`,
  `local/build/launcher-agent-c/test-final.log` (private `local/` scope).
- Negative control (required): temporarily changed `.notBuilt` value from `detail` to
  `"Not built: \(detail)"` in `ReadinessAccessibility.swift`;
  `swift test ... --filter ReadinessAccessibilityTests` then exited non-zero with
  `Test run with 5 tests in 1 suite failed ... with 5 issues`
  (`Expectation failed: copy.value == expectedValue(for: status)`). Restored from
  `local/build/launcher-agent-c/negctl/ReadinessAccessibility.swift.bak`; `diff -q` clean.
- Observation versus inference / coverage limitations: the 12-combination table, the distinct-label
  check and the native-core NOT BUILT containment check are observed passing assertions. Nothing was
  observed about real VoiceOver output, focus order, rotation of the rotor, localization, or how
  SwiftUI merges `.accessibilityElement(children: .combine)` with an explicit label/value at runtime
  — that wiring is compiled but **not** UI-verified. No VoiceOver/accessibility-inspector session ran.
- Cumulative failed tries and distinct hypotheses: 0 implementation tries (both full runs passed).
  One negative control deliberately failed and was restored.
- Remaining risks / blockers: strings are English-only and frozen by tests; changing them later
  needs test+README updates. The label duplicates `Kind.rawValue` by design (test asserts equality),
  so renaming a visible label must also update the accessibility switch. Real VoiceOver behaviour,
  the `.app` bundle and sandbox entitlements remain unverified.
- Next exact action: integrator merges this record; a later runtime accessibility audit should open
  the launcher under VoiceOver and compare spoken output with `ReadinessAccessibility` (the earlier
  `APP-01-VOICEOVER-READINESS-01` attempt is still `WAITING_FOR_DEPENDENCY`, not this task).
- HANDOFF.md / TASKS / EVIDENCE / INSTALL_LOG / indexes updated: **not by this worker** (shared docs
  are integrator-owned). No dependency or install change. `docs/checkpoints/` gained this file only.
