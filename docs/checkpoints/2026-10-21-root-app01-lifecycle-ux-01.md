# Checkpoint: APP-01-LIFECYCLE-UX-01 — honest lifecycle copy

- Time / timezone: 2026-10-21 00:20 CDT (America/Chicago).
- Agent and integrator: worker `app01-launcher`; integrator merges into `HANDOFF.md`/`docs/TASKS.md`.
- Task / gate / status: `APP-01-LIFECYCLE-UX-01` = `FIXTURE_VERIFIED` (pure-mapping tests). No gate
  advanced; no child process was ever launched, so this says nothing about real Stop/exit behaviour.
- Prerequisite evidence / input and source hashes: baseline `LauncherLifecycle` already had
  `classifyTermination`, `canStart`, `state(after:readinessAllowsStart:)` and
  `FolderImportErrorClassifier` (11 tests in 2 suites). Status text lived only in
  `ContentView.statusText` (inline switch) and failure text only in
  `LauncherLifecycle.state(after:)`. `LauncherLifecycle.swift` was **not** modified
  (sha256 stays `358e78ae1de70957fd01100e9969916ab4c6c81a0e52365484251087d8c248c4`).
- Owned / changed files:
  - new `launcher/Sources/LauncherCore/LauncherStatusCopy.swift`
    sha256 `63dd6b3b27eb370310a9cc84b0ed82066530173df080f1b6269169fa815067c1`
  - new `launcher/Tests/LauncherCoreTests/LauncherStatusCopyTests.swift`
    sha256 `5bc5bc46d7058cb9e757ee5d96b31243c98567d0588939801d8e0151c8f20129`
  - `launcher/README.md` (documented status strings and hint presence)
- Hypothesis and expected result: a pure `LauncherStatusCopy(status:recoveryHint:)` mapping from
  each `LauncherProcessOutcome` (3 cases) and each `LauncherState` (7 cases) can give distinct,
  non-empty wording, with hints only where recovery is warranted and never for a requested Stop.
  Expected: 7 new tests, 19 existing still pass.
- Wording decisions (policy):
  - requested stop → `Stopped at your request.`, hint `nil`; the words crash / failed / unexpected
    are absent (asserted).
  - clean exit → `Game core exited normally (status 0).`, hint `nil`.
  - unexpected exit → `Game core ended unexpectedly (status N).` plus a retry/diagnostics hint. The
    copy deliberately does **not** say "crashed" or "signal": `classifyTermination` folds signals and
    nonzero exits into `.unexpectedExit(status:)`, so naming a cause would be an invention. It also
    never says success/completed/normally (asserted).
  - states → `Not ready to play.` / `Checking files and hardware…` / `Ready to play.` /
    `Starting the game core…` / `Game core running.` / `Stopping the game core…` /
    `Failed: <message>`; only `.failed` returns a hint, and the underlying message is preserved.
  - `LauncherLifecycle.classifyTermination`, `canStart`, `state(after:)` semantics unchanged.
- Exact command, CWD and nonsecret environment: CWD `/Volumes/Extra/SoC Mac Native`;
  `swift build --package-path launcher --scratch-path local/build/launcher-agent-c`;
  `swift test --package-path launcher --scratch-path local/build/launcher-agent-c`.
  Apple Swift 6.4, target `arm64-apple-macosx27.0.0`, macOS 27.0.1.
- Exit / observed result: build exit 0 (zero warnings). `swift test` exit 0:
  **42 tests in 7 suites passed**, including `Suite LauncherStatusCopyTests passed after 0.001
  seconds` (7 tests: 3 outcomes distinct + requested-stop honesty/no-hint + clean-exit no-failure +
  unexpected-exit no-success + 7 states distinct + only-failed-has-hint + failed keeps message) and
  the untouched `Suite LauncherLifecycleTests` / `Suite FolderImportErrorClassifierTests`
  (11 tests). Logs `local/build/launcher-agent-c/build-final.log`, `test-final.log`.
- Negative control (required): temporarily gave `.requestedStop` a non-nil `recoveryHint`;
  `swift test ... --filter LauncherStatusCopyTests` exited non-zero with
  `Test run with 7 tests in 1 suite failed ... with 1 issue`
  (`Expectation failed: copy.recoveryHint == nil`). Restored from
  `local/build/launcher-agent-c/negctl/LauncherStatusCopy.swift.bak`; `diff -q` clean.
- Observation versus inference / coverage limitations: mapping completeness and wording constraints
  are observed via assertions. **Not** observed: no `SoCGameCore` exists, so no real Stop, clean exit,
  signal or nonzero exit occurred; `LauncherModel.processEnded` was not modified and still drives the
  UI, so this copy is not yet displayed anywhere. Hint usefulness for real failures is untested.
- Cumulative failed tries and distinct hypotheses: 0 implementation tries. One deliberate
  negative-control failure, restored.
- Remaining risks / blockers: two status vocabularies now exist — `ContentView.statusText` (visible)
  and this tested copy (not wired in). Wiring them is a UI change that needs its own review, and
  until then a visible/audible divergence is possible. Locale/translation and VoiceOver pronunciation
  of `…`/`—` are unverified. `unexpectedExit(status:)` cannot distinguish signal from exit code; if
  that distinction is needed, `classifyTermination` (not this copy) must gain it, which would change
  a tested public signature.
- Next exact action: integrator merge. Optional follow-up (unclaimed): replace
  `ContentView.statusText` with `LauncherStatusCopy.for(model.state)` and show the hint only when
  non-nil, with a matching UI checkpoint.
- HANDOFF.md / TASKS / EVIDENCE / INSTALL_LOG / indexes updated: **not by this worker**. No install
  or dependency change.
