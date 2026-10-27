# APP-01 concurrent child-client request correlation fixture complete

Local date/time: 2026-10-27 00:36 CDT (America/Chicago). Integrator: `/root`.

Task: `APP-01-PROCESS-CLIENT-CONCURRENT-REQUESTS-01`.

## Change and evidence

Only `launcher/Tests/LauncherCoreTests/ProcessCoreClientTests.swift` changed (SHA-256
`a63408869b7b248382d72b8bdf521b525097195e3e34a9873900e24f7bdff71b`). The test helper can delay
the temporary shell fake's responses; the new test calls `status()` and `coverage()` concurrently,
checks each method's type-specific result, and verifies exactly two logged commands with distinct
IDs `1` and `2` without assuming which command is first. No production source or protocol changed.

Root verification used these exact commands:

```sh
swift test --package-path launcher --scratch-path local/build/launcher --filter ProcessCoreClientTests > local/validation/orch-20261027/app01-client-concurrent/root-focused.log 2>&1
swift test --package-path launcher --scratch-path local/build/launcher > local/validation/orch-20261027/app01-client-concurrent/root-full.log 2>&1
```

Both exited 0. The focused run passed **21 tests in 1 suite**; the full launcher run passed
**154 tests in 20 suites**. The opt-in `realCoreAnswersTheProtocolAndRefusesStart()` test was
skipped because the real-core integration setting was absent. Root logs are retained privately;
their SHA-256 values are `a8c8e9828c2f98259989b1f93090b9dcfcfca6e32f9ae25bef6952f423afe353`
(focused) and `fc20bd7e228b11e1b47601e0ee61c9066d6a070e859c7572b7cda68c8797b941` (full).
The independent worker's runs also passed 21/1 and 154/20; its logs are retained under the same
private directory.

One initial worker compile attempt failed at the test's Swift key path (`expected expression path in
Swift key path`); the typo was corrected before the passing focused and full runs. The failed log is
`local/validation/orch-20261027/app01-client-concurrent/focused-compile-failure-1.log` (SHA-256
`291bbf377b38461488c27697f78581eca0b041da6bdaf4d8defe901f6cf708a3`). Count: one transient
compile failure corrected, one successful implementation, zero failed semantic hypotheses.

Status: **FIXTURE_VERIFIED** for this test-only contract. It establishes protocol behavior against
a synthetic shell child, not a real game core. G4 and all native-game gates remain PARTIAL/NOT RUN;
the skipped real-core protocol test does not imply boot, audio, or save/reload.

## Publication checkpoint

`python3 scripts/project_index.py` exited 0 and indexed 33,051 folders. `trail build --check`
exited 0 with the wiring graph in sync; `trail grep
"concurrentStatusAndCoverageRequestsKeepResponsesPaired"` returned the new method at
`launcher/Tests/LauncherCoreTests/ProcessCoreClientTests.swift:209-235`.
`git diff --check`, `python3 -I scripts/audit_publish_paths.py --all` (2,979 paths),
`python3 -I scripts/check_commit_attribution.py --identity`, and the unpublished-range default
attribution audit all exited 0. `python3 -I scripts/check_commit_attribution.py --all` still exits 1
only for the previously documented non-Claude `f6604a86e60e` `orchestrator@local` author and
committer. Staged audits, commit, push, and a post-push remote rescan remain the next actions.
