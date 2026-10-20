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

The earlier optional preference question about using a testable helper had no reply. Proceeding
with the recommended LauncherCore helper is a bounded assumption. No process ownership, core ABI,
bundle or GUI redesign is included.

## Verification and limits

- Exact planned command: `swift test --package-path launcher --scratch-path local/build/launcher`.
- No implementation attempt has run; failed tries: 0.
- This test can validate transition mapping only. It cannot prove GUI child-process lifecycle, G4
  acceptance, an available native game core, or G1. G1 remains NOT PASSED; G2-G6 remain NOT RUN.
- Next exact action: implement the helper and model call site, run the focused Swift package suite,
  inspect the diff, then update TASKS/EVIDENCE/HANDOFF and publish only audited allowlisted paths.
