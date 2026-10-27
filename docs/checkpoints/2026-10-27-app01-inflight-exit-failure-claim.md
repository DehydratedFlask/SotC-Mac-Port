# APP-01 in-flight request child-exit fixture claim

Local date/time: 2026-10-27 00:56 CDT (America/Chicago). Integrator: `/root`.

Task: `APP-01-PROCESS-CLIENT-INFLIGHT-EXIT-FAILURE-01`.
Parent: `APP-01-LAUNCHER-PROCESS-CLIENT-01`, **FIXTURE_VERIFIED** (E613).

## Live contract and scope

Required Trail query exited 0:

```sh
trail ask "What happens to an in-flight ProcessCoreClient request when the child exits before replying, and which tests cover early exit and subsequent requests?" --source --in launcher/
```

The live `handleEndOfOutput()` path at `launcher/Sources/LauncherCore/ProcessCoreClient.swift:261-266`
creates an `.io` failure and calls `abort()`, which calls `failPending()` and resumes the in-flight
request at lines 340-360. Existing `earlyExitFailsWithIO()` at
`launcher/Tests/LauncherCoreTests/ProcessCoreClientTests.swift:430-442` starts a child that exits
immediately, without proving it first received the request. `exitAfterOneAnswerFailsTheNextRequest()`
at lines 444-458 covers a request after a prior response.

Bounded test-only scope: `launcher/Tests/LauncherCoreTests/ProcessCoreClientTests.swift`. A temporary
shell child will read and log a status request, then exit nonzero without replying. The test will
assert the in-flight call receives `.io`, the failure is not the response-timeout message, and the
child log contains exactly the status request. Do not assert on a specific EOF/exit message because
the two notifications can be queued in either order. Do not claim queued-request draining: the fake
cannot observe whether a second request reached the internal client queue.

## Acceptance and limits

Run focused `ProcessCoreClientTests` and the full launcher suite. This fixture does not invoke the
game core, and it does not establish game boot/audio/save/reload, Finder launch, or Metal behavior.
The child must not send `quit`; its exit is the transport failure under test.

Status: **ACTIVE**, claimed by `/root`; attempt count 0, failed hypotheses 0. No implementation
change has been made. Next exact action: publish this claim with the pending E742 scan checkpoint;
only then edit the bounded test file.
