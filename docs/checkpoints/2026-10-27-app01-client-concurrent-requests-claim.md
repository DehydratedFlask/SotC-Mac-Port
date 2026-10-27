# APP-01 concurrent child-client request correlation claim

Local date/time: 2026-10-27 00:28 CDT (America/Chicago). Integrator: `/root`.

Task: `APP-01-PROCESS-CLIENT-CONCURRENT-REQUESTS-01`.

## Bounded scope

Add a test-only fixture to `launcher/Tests/LauncherCoreTests/ProcessCoreClientTests.swift` that
invokes `status()` and `coverage()` concurrently on one `ProcessCoreClient` backed by the existing
temporary shell fake. Give the fake a short response delay so both async requests contend for the
client queue. Assert that each operation returns its own expected state/subsystem data and that the
fake observed exactly two distinct request IDs for the two commands. Command ordering is
nondeterministic and is not pinned.

No production source, protocol, native core, or game file is in scope. The fake is a test-only child
process; no actual game executable or real-core protocol test will be run.

## Acceptance and checks

- A new focused `ProcessCoreClientTests` case completes both concurrent requests and verifies their
  response mapping plus unique request IDs.
- The focused `ProcessCoreClientTests` suite and full launcher suite pass. Preserve a skipped
  opt-in real-core test as NOT RUN; do not imply game boot, G4 completion, or native-core readiness.
- Keep logs under `local/validation/orch-20261027/app01-client-concurrent/` and record source/test
  hashes in the completion checkpoint.

Initial status: **ACTIVE**, owner `/root`, 0 failed tries. Exact next action after this claim is
published: add the focused test-only case, then run the targeted and full launcher test commands.
