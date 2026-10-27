# APP-01 in-flight child-exit fixture complete

Local date/time: 2026-10-27 00:58 CDT (America/Chicago). Integrator: `/root`.

Task: `APP-01-PROCESS-CLIENT-INFLIGHT-EXIT-FAILURE-01`.

## Change and acceptance

Only `launcher/Tests/LauncherCoreTests/ProcessCoreClientTests.swift` changed for this task; its
SHA-256 is `5f8f52ba78915b7453f78bd42b2f9fe2cd7c1babbb6da87cc9c34f606383abfe`. The added
`exitAfterReceivingRequestButBeforeReplyFailsWithIO()` uses a temporary shell child that reads and
logs one status request, then exits 17 without replying. The in-flight call must fail with `.io`,
must not report the response-timeout message, and the child log must contain exactly one status
request. The test makes no claim about queue draining. No production source or protocol changed.

Root ran these exact commands; both exited 0:

```sh
swift test --package-path launcher --scratch-path local/build/launcher --filter ProcessCoreClientTests > local/validation/orch-20261027/app01-inflight-exit/root-focused.log 2>&1
swift test --package-path launcher --scratch-path local/build/launcher > local/validation/orch-20261027/app01-inflight-exit/root-full.log 2>&1
```

The focused run passed **23 tests in 1 suite**; the full launcher suite passed **156 tests in 20
suites**. The opt-in `realCoreAnswersTheProtocolAndRefusesStart()` test was skipped because its
integration setting was absent. Root log SHA-256 values: focused
`3bf35c5ed72702be364af8386b6de40f99340641317df251fb9d16d495934411`; full
`87089aabb2d1bfd2633c661cdb1612cddce394755a79c3bb1c91687388e3338b`.

Trail rebuilt after the test edit and `trail build --check` exited 0; the method was queryable with
`trail grep "exitAfterReceivingRequestButBeforeReplyFailsWithIO"` at test lines 444-465. The graph
check remains in sync; the deep concept layer is not built and has 9,875 pending nodes.

Status: **FIXTURE_VERIFIED** for this synthetic child-client transport contract. It does not
establish real-core behavior, Finder launch, game boot/audio/save/reload, or Metal. Those acceptance
gates remain PARTIAL/NOT RUN. Attempts: one successful implementation, zero failed semantic
hypotheses.

## Attribution scan

The fetched-ref scan after `4cb0afcd4f5dfed883858abc73980a93649f4228` found zero Claude/Anthropic
matches across 9 branch heads and 2,367 unique reachable commits (E744). Its private JSON SHA-256 is
`fdd0cd46f302ca9297a02bc8624b6d2b154b8efbe0eb20723bb9386f65427859`; it predates this completion
checkpoint. The local `--all` audit still reports the separate historical `f6604a86e60e`
`orchestrator@local` identity, not Claude/Anthropic attribution.

Pre-commit publication gates: staged `python3 -I scripts/audit_publish_paths.py` passed all 9
allowlisted paths; `python3 -I scripts/audit_publish_paths.py --all` passed 2,992 paths. Private
logs under `local/validation/orch-20261027/app01-inflight-exit/publish/`: staged-path SHA-256
`c634cf7a34ae5c90c93c55cf7968a48a1f8a6944966f8c54b0eacea2072df091`; all-path SHA-256
`c0910caa177763eb0eb0e87dcc343b527f528ce807a995a0d3676c3006a99418`. The unpublished-range
attribution audit exited 0 (log SHA-256
`72d954d7873849934ae10cdf5ad4049069928f3c6b433f3e7e44743823e63f33`); `--all` reports only the
separate historical `f6604a86e60e` `orchestrator@local` identity (log SHA-256
`f67c8b2dabf442366fbc8922353feda1e6bbd31ffc26b3537d5161e8c1adad44`). Staged and unstaged diff
checks passed. These attribution commands ran before this checkpoint commit; the new commit still
requires the post-commit unpublished-range gate.

Next exact action: commit/push only allowlisted paths with the owner identity, then fetch and rescan
all remote refs.
