# APP-01 clean-exit and requested-Stop readiness claim

- Date/time: 2026-10-19 23:03 CDT (America/Chicago)
- Agent: `/root` (integrator); read-only probe: `/root/app01_probe`
- Task ID: APP-01, preserve launcher readiness after a clean child exit or requested Stop
- Files changed in this claim: `HANDOFF.md`, `docs/TASKS.md`, this checkpoint; no implementation
  code has changed.

## Observed issue

`LauncherModel.processEnded` in `launcher/Sources/SoCLauncher/LauncherModel.swift` sets state to
`.idle` after a requested stop or a zero-status normal exit. `LauncherModel.canStart` accepts a
startable readiness report only if state is `.ready` or failed. The retained report can still have
`canStart == true`, but `ContentView` disables Play while idle and shows “Not ready to play.” The
launcher therefore loses readiness after a child lifecycle completes even though its checks still
pass.

## Graft and live-file evidence

- `graft callers canStart --depth all` — no indexed symbol named `canStart`; exit 1.
- Switched to `graft grep "canStart"` — exit 0; 10 hits across 7 symbols in
  `ReadinessReport.swift`, `ContentView.swift`, `LauncherModel.swift`, and
  `AssetValidatorTests.swift`; approximately 2,579 tokens saved.
- `graft callers LauncherModel.validate --depth all` — exit 0; no indexed incoming edges for the
  method; approximately 632 tokens saved.
- Live spans verified: `LauncherModel.swift` lines 14-78, `LauncherState.swift`, `ContentView.swift`
  status/button rendering, LauncherCore tests, and G4 acceptance criteria in `docs/VALIDATION.md`.
- Graft graph has the current launcher files indexed; these calls did not modify source.

## Bounded plan

Add a pure, testable LauncherCore lifecycle mapping for requested stop, successful exit and
unexpected exit. On a requested stop or successful exit, return `.ready` only if the latest
readiness report permits starting, otherwise `.idle`. Preserve `.failed("Game core exited
unexpectedly (status …)")` for an unexpected exit. Call it from the existing `processEnded` method.
Add focused tests for clean exit, requested stop, failed exit, and startability gating.

Planned implementation paths:

- `launcher/Sources/LauncherCore/LauncherLifecycle.swift`
- `launcher/Tests/LauncherCoreTests/LauncherLifecycleTests.swift`
- `launcher/Sources/SoCLauncher/LauncherModel.swift`

At claim creation the optional preference question about a testable helper had no reply. The user
confirmed “Testable LauncherCore helper (Recommended)” at 2026-10-19 23:05 CDT, so the chosen
approach is now explicit. No process ownership, core ABI, bundle or GUI redesign is included.

## Verification and limits

- Exact planned command: `swift test --package-path launcher --scratch-path local/build/launcher`.
- At claim creation no implementation attempt had run; the current failed try 1 is recorded below.
- This test can validate transition mapping only. It cannot prove GUI child-process lifecycle, G4
  acceptance, an available native game core, or G1. G1 remains NOT PASSED; G2-G6 remain NOT RUN.
- Next exact action at claim creation: implement the helper and model call site, run the focused
  Swift package suite, inspect the diff, then update TASKS/EVIDENCE/HANDOFF and publish only
  audited allowlisted paths.

## Implementation checkpoint — failed try 1

- Date/time: 2026-10-19 23:04 CDT; agent `/root`; task APP-01.
- The first version of the lifecycle outcome helper, `processEnded` mapping, and four tests passed
  `swift test --package-path launcher --scratch-path local/build/launcher` (11 tests), but that
  output file was overwritten by the next test run.
- A follow-up change added `LauncherLifecycle.canStart` and predicate coverage so the tests can
  assert that `.ready` after clean exit/Stop actually permits Play. The same exact test command
  then exited 1. Log: `local/build/launcher-lifecycle-tests-20261019.log`. Swift reported unused
  `true`/`false` switch literals and `missing return in static method expected to return 'Bool'`.
- Observation: compilation stops in `LauncherLifecycle.canStart`; no test assertion ran on this
  source version. The helper needs an explicit returned switch expression or explicit branch
  returns. This is failed implementation try 1; no escalation threshold reached.
- Next exact action: add explicit return behavior to `canStart`, then rerun
  `swift test --package-path launcher --scratch-path local/build/launcher` and preserve the log.

## User-confirmation checkpoint — 2026-10-19 23:06 CDT

- Agent: `/root`; task APP-01; implementation attempt count remains 1.
- Files changed: `HANDOFF.md`, `docs/TASKS.md`, this checkpoint. No source edits or test runs in
  this documentation checkpoint.
- Exact evidence: user reply to the pending design question was “Testable LauncherCore helper
  (Recommended)”. Result: the previously assumed approach is confirmed.
- Observation: this settles the design preference only. It does not establish lifecycle test or G4
  acceptance results.
- Remaining risk: `LauncherLifecycle.canStart` still has the recorded missing-return compile error;
  no tests ran on that source version.
- Next exact action: add explicit return behavior to `canStart`, then rerun
  `swift test --package-path launcher --scratch-path local/build/launcher` and preserve the log.
- Status: ACTIVE; first implementation try failed to compile; no escalation threshold reached.

## Implementation checkpoint — correction passed

- Date/time: 2026-10-19 23:07 CDT; agent `/root`; task APP-01; failed try 1 corrected.
- Files changed: `launcher/Sources/LauncherCore/LauncherLifecycle.swift`,
  `launcher/Sources/SoCLauncher/LauncherModel.swift`,
  `launcher/Tests/LauncherCoreTests/LauncherLifecycleTests.swift`; generated `docs/FOLDERS.md`;
  updated `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, and this checkpoint.
- Exact command: `swift test --package-path launcher --scratch-path local/build/launcher`;
  exit 0, all 12 tests passed. Log:
  `local/validation/20261019T2307-root-app01-lifecycle-try1/swift-test.log`.
- Additional checks: `python3 scripts/project_index.py` completed (4,123 folders indexed),
  `graft check` passed the wiring freshness check, and `git diff --check` exited 0.
- Observation: explicit returns fixed the sole compile error. Five focused lifecycle cases pass.
  Inference remains limited to pure transition behavior; no GUI/process lifecycle was run.
- Remaining risks: G4 matrix, Finder launch, a real native-core ABI/bundle, and native game start
  remain unverified. G1 is NOT PASSED; G2-G6 are NOT RUN.
- Next exact action: finish publication audits, commit and push this allowlisted checkpoint, then
  select a next bounded independent task. APP-01 remains ACTIVE; failed implementation tries: 1.
