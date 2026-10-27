# APP-01 pending-request shutdown fixture claim

Local date/time: 2026-10-27 00:42 CDT (America/Chicago). Integrator: `/root`.

Task: `APP-01-PROCESS-CLIENT-SHUTDOWN-PENDING-REQUEST-01`.
Parent: `APP-01-LAUNCHER-PROCESS-CLIENT-01`, already **FIXTURE_VERIFIED** (E613).

## Live contract and scope

The required Trail query exited 0:

```sh
trail ask "How does ProcessCoreClient shutdown handle in-flight and queued requests, and what tests cover pending requests?" --source --in launcher/
```

The live implementation at `launcher/Sources/LauncherCore/ProcessCoreClient.swift:184-219`
serializes shutdown on its owning queue, marks the client shut down, calls `failPending` with
`.badState`, then writes a new `quit` request and waits for the child to exit. The live
`failPending` implementation at lines 353-360 clears and resumes in-flight and queued continuations.
Existing tests at `launcher/Tests/LauncherCoreTests/ProcessCoreClientTests.swift:352-372` cover
shutdown after a completed request and a request submitted after shutdown, but do not cover an
outstanding request.

The bounded change is test-only in `launcher/Tests/LauncherCoreTests/ProcessCoreClientTests.swift`.
A temporary shell fake will log a `status` request without answering, wait for `quit`, log it, and
exit. The test will wait until the fake has received `status`, call `shutdown()`, then assert the
pending call fails with `.badState` and the log contains `status` followed by `quit`. It will not
assume queued-request behavior or change production code/protocol.

## Acceptance and limits

Run the focused `ProcessCoreClientTests` suite and the full launcher suite. The shell fake is a
fixture only; do not claim a real-core run, Finder launch, game boot/audio/save/reload, or Metal gate.
The principal risk is synchronization: the test must wait for the fake to log `status` before
shutting down, without a long sleep or relying on command scheduling order.

Status: **ACTIVE**, claimed by `/root`; attempt count 0, failed hypotheses 0. No implementation
change has been made. Next exact action: commit and push this claim with the current documentation
checkpoint; only then implement the test in the bounded file scope.
