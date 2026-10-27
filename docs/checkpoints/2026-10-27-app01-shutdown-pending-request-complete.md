# APP-01 pending-request shutdown fixture complete

Local date/time: 2026-10-27 00:50 CDT (America/Chicago). Integrator: `/root`.

Task: `APP-01-PROCESS-CLIENT-SHUTDOWN-PENDING-REQUEST-01`.

## Change and acceptance

Only `launcher/Tests/LauncherCoreTests/ProcessCoreClientTests.swift` changed for this task; its
SHA-256 is `7cd34460f8f4d8f3be1591f5b0bcc641564f4193b824476bf71bb8fa2ce6a216`. The added
`shutdownFailsPendingStatusBeforeSendingQuit()` uses a temporary shell child that logs and withholds
the status response until shutdown sends quit. The test waits for the status command, calls
`shutdown()`, then verifies the pending call fails with `.badState` and the wire command order is
exactly `status`, `quit`. No production source or protocol changed.

Root ran these exact commands; both exited 0:

```sh
swift test --package-path launcher --scratch-path local/build/launcher --filter ProcessCoreClientTests > local/validation/orch-20261027/app01-shutdown-pending/root-focused.log 2>&1
swift test --package-path launcher --scratch-path local/build/launcher > local/validation/orch-20261027/app01-shutdown-pending/root-full.log 2>&1
```

The focused run passed **22 tests in 1 suite**; the full launcher suite passed **155 tests in 20
suites**. The opt-in `realCoreAnswersTheProtocolAndRefusesStart()` test was skipped because its
integration setting was absent. Root log SHA-256 values: focused
`f2093c68c6eac3dff42ed542043ce7c5efa911384625238f2a6661ba7a1bbcbc`; full
`276f9af7c50ee595b6dc518a0516b72d650363c686979904efade06cc6ac2476`. The independent worker also
passed 22/1 and 155/20, using these exact commands from `launcher/`:

```sh
mkdir -p ../local/validation/orch-20261027/app01-shutdown-pending && swift test --filter ProcessCoreClientTests > ../local/validation/orch-20261027/app01-shutdown-pending/focused.log 2>&1
swift test > ../local/validation/orch-20261027/app01-shutdown-pending/full-launcher.log 2>&1
```

Both worker commands exited 0. Worker log SHA-256 values: focused
`5865f886c5cb7c83b8e537f76b42ea10dd10d744bfeccd1b69f9898589c8ae3c`; full
`cd01b2d625acc963c4f7cb6c63eecdf9f53676db5ee86be4324857a6edb5769f`.

Trail first reported the changed test file and one new method as stale; `trail build` exited 0
(9,874 nodes, 13,458 edges, 1,608 cards), the subsequent `trail build --check` exited 0, and
`trail grep "shutdownFailsPendingStatusBeforeSendingQuit"` returned the new method at
`launcher/Tests/LauncherCoreTests/ProcessCoreClientTests.swift:364-401`.

Status: **FIXTURE_VERIFIED** for the synthetic child-client shutdown contract. The test and parent
source do not establish real-core behavior, Finder launch, game boot/audio/save/reload, or Metal.
Those acceptance gates remain PARTIAL/NOT RUN. Attempts: one successful implementation, zero failed
semantic hypotheses.

## Attribution scan

The most recent fetched-ref scan after `3d9f58b165579012bc1b6c8ae06621e2b1278f41` found zero
Claude/Anthropic matches across 9 branch heads and 2,363 unique reachable commits (E739). Its
private JSON SHA-256 is
`f347c047c29805fb70238b4d66c2d1552a03cfe9ee444dbdd433bea96beeb6bc`; it predates this completion
checkpoint.

Next exact action: update the task/evidence/index/handoff docs, regenerate the folder index, run
publication gates, commit and push only allowlisted files as the owner, then fetch and rescan.

Pre-commit publication gates: staged `python3 -I scripts/audit_publish_paths.py` passed all 9
allowlisted paths; `git diff --cached --check` and `git diff --check` exited 0. The all-path audit
passed 2,986 paths; private log `local/validation/orch-20261027/app01-shutdown-completion-all-paths.log`
SHA-256 `b27712f7ac1fd3233b24a3b618a066df93d5d055e9e5a8e7af51d513fee0791d`. Identity-only and
unpublished-range attribution checks exited 0. The `--all` audit reports only the separate
historical `f6604a86e60e` `orchestrator@local` author and committer, not a Claude/Anthropic identity.
