# APP-01 shutdown TERM escalation and post-46a759 attribution checkpoint (E773-E778)

- Local date/time: 2026-10-27 07:57 CDT (America/Chicago).
- Agent / integrator: `/root`.
- Tasks: `APP-01-PROCESS-CLIENT-SHUTDOWN-TERM-ESCALATION-01` and `PUB-01-CLAUDE-CREDIT-CLEANUP-01`.
- Status: APP-01 task **FIXTURE_VERIFIED** at E775 (one successful initial fixture, one failed readiness-marker iteration, corrected fixture passed); attribution cleanup **ARTIFACT_VERIFIED** for fetched GitHub refs under the owner's stash scope.

## E773: post-push public attribution scan

After owner commit `30399e60770fa708c64f29e14d17e3b17669a04d` was pushed, the exact command
`git fetch origin --prune && python3 -I local/validation/orch-20261026/claude-credit-cleanup-review/scan_remote_refs_attribution.py > local/validation/orch-20261027/claude-credit-cleanup-review/remote-scan-after-30399e60.json`
exited 0. The private report has SHA-256
`24e1ff658d181362ed9bc02fe0137d784190d0fefbd5b82632c9d6e6e637fdc5`.

Observed: 9 fetched branch heads, 0 remote tag refs, 2,388 unique reachable commit objects, and 0
Claude/Anthropic author, committer, or credit-line matches. The fetched GitHub refs are clean at
`origin/main == 30399e60770fa708c64f29e14d17e3b17669a04d`. The owner chose to leave the 70
stash-only matches under private `refs/stash` unchanged; they are not in the public scan scope.

## APP-01 bounded task claim

The parent process-client task is fixture-verified. Existing `shutdownSendsQuitAndIsIdempotent`
and `shutdownFailsPendingStatusBeforeSendingQuit` cover cooperative exit and pending-request failure.
The production `shutdownLocked()` also sends SIGTERM after one second when the child ignores `quit`,
but this branch has no dedicated fixture.

Claimed scope: add one test in
`launcher/Tests/LauncherCoreTests/ProcessCoreClientTests.swift` using the existing temporary shell
fake. The fake must receive `quit`, continue running, record SIGTERM through a trap, clean up its
sleeper, and exit. Assert the quit request and TERM marker after `shutdown()`. Keep the change
test-only. SIGKILL fallback, game-core shutdown, and production behavior remain outside this task.

Files changed by this claim: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`,
`docs/ATTRIBUTION.md`, `docs/INDEX.md`, `docs/FOLDERS.md`, and this checkpoint. The implementer may
edit only the test file above; root owns the handoff, evidence, test review, build files, and
publication.

## Implementation review and follow-up

The first test-only implementation used a temporary shell child that logged `quit`, stayed in a
`sleep 30`, trapped SIGTERM, killed and reaped the sleeper, and logged `sleeper-cleaned`. It passed
focused `swift test --package-path launcher --scratch-path local/build/launcher --filter
ProcessCoreClientTests` (**24 tests / 1 suite**) and the full `swift test --package-path launcher
--scratch-path local/build/launcher` suite (**157 tests / 20 suites**). The child source hash was
`bc2ad14639957410ec9140506ea9226ee7ae48a61f77ef3533fd64533fb20e21`; logs are under
`local/validation/orch-20261027/app01-shutdown-term-escalation/` (focused SHA-256
`0fa62696a0aa3f606b9cf637351cae0a2535df89ea32f8d0802a4f3a2fcdb836`; full
`bcad426414f84ea80a2f621e8a675a061e155feecd1bc3060b68ba0f27fe4224`).

Independent review approved the quit → SIGTERM → sleeper cleanup assertions and identified a
The first readiness-marker iteration failed; see E774 below. This is a failed follow-up iteration, not a semantic hypothesis failure.
The task remains ACTIVE with 1 successful initial implementation and 1 failed follow-up iteration.

The first Trail freshness check ran while the worker was editing and returned STALE for this test
file; `trail build` then indexed 9,876 nodes / 13,469 edges, `trail build --check` passed, and
`trail grep shutdownEscalatesToSIGTERMAndChildCleansItsSleeper` found the new method. The stale
check log is `local/validation/orch-20261027/app01-term-checkpoint/trail-check.log`, SHA-256
`f7155a350daa66d4038a00b49a466a652a72e187b278c030c9f6ed288cab276e`.

Next: run the focused suite against the corrected readiness ordering before another edit.
No real game process, game assets, SIGKILL path, or runtime behavior is involved.


## E774: failed readiness-marker iteration

The exact focused command `swift test --package-path launcher --scratch-path local/build/launcher --filter ProcessCoreClientTests` exited 1 with 24 tests, one suite, and 10 issues. The log is `local/validation/orch-20261027/app01-shutdown-term-escalation/focused-ready.log`, SHA-256 `4a3f3699e550bfe3c1416d3e87774f4fdaa5ce8967a9fc18c5986cab22ce2f55`.

Observed: the test awaited `childReady` before calling `shutdown()`, but the fake emitted readiness only after receiving `quit`; this formed a test-side wait cycle and caused related request-timeout failures. No production code changed. The worker moved sleeper setup, TERM-trap installation, and readiness emission ahead of the stdin loop. That unverified working tree has source SHA-256 `0f8a8aa691f80b0f6d2f10749a70c3bd7e8dfe92a19d528f7e3c080f76e63855`. Count this as one failed follow-up iteration after one successful initial implementation; the task remains ACTIVE and below the three-failure escalation cap.

Next: run the focused suite against the corrected ordering before another edit.


## E775: final fixture and repository verification

The final test-only source is `launcher/Tests/LauncherCoreTests/ProcessCoreClientTests.swift`,
SHA-256 `6f3449c6b380aeff924b0e35b87e7f7538ef0918f49fdb7b67e809e4c0a6aaff`. Independent review
approved the final version after removing the unused sleeper PID file. The bounded readiness marker
is written only after the TERM trap is installed and the sleeper is live; the test then asserts the
`quit` line and exact `TERM` plus `sleeper-cleaned` records.

| Command | Result | Log SHA-256 |
|---|---|---|
| `swift test --package-path launcher --scratch-path local/build/launcher --filter ProcessCoreClientTests` | **PASSED**, 24/24 | `db53c287b90aa9dbda17df6d71f6152d0fb5523793082703a42f0e5032ac524d` |
| `swift test --package-path launcher --scratch-path local/build/launcher` | **PASSED**, 157/157 across 20 suites | `98b6cbc33debb1b8c5771accc8e0a925768db976ad4ccee9750c34f6aed82ce1` |
| `cmake -S native -B local/build/orch-r22` | **PASSED**, exit 0 | `c9e06a32f55f3035929fbe7bc8358eaa65e4673d154f6e0d5b8ef8135dfee0e9` |
| `cmake --build local/build/orch-r22 -j 12` | **PASSED**, no work to do | `8c35b1682f73592d30612d7b0fabdb384fcfff35cb5a076cb48b85b1368aa794` |
| `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure` | **PASSED**, 431/431 | `2f54f4e917800b48f582ce12d4c68c654d286f5adbb8066bc916c47b10a9cc21` |
| `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` | **PASSED**, 516/0/0 | `f6bd4048e58d79d1b452cdba2b9c54abb1ccac1b37de894eaae8ae7b626862db` |
| `trail build` / `trail build --check` / `trail grep shutdownEscalatesToSIGTERMAndChildCleansItsSleeper` | **PASSED**, 9,876 nodes / 13,469 edges; method queryable; deep semantic layer not built | logs under `local/validation/orch-20261027/app01-term-checkpoint/final-trail/`, hashes `d70cf6d1d88ccfe67c62cb06f0ac5b91cf35cf84b919e709b05ed9d05dff0dae`, `eb64261b7e53ff1fb2a879c571528604a13c82c9e640c49bb3c0ba17e075d55c`, `fe3e3ce7b58f06d0254c6f358f63625ef2efffdf3de2a4cc32a2604638d961a0` |
| `python3 scripts/project_index.py` | **PASSED**, 33,115 folders indexed | `196a71ca9d5025cd59a5f07ca059bfc5587b74cfdfe980540e834c23d9afb905` |

The native-gate result manifest is `local/validation/orch-20261027/app01-term-checkpoint/final-native/results.json`,
SHA-256 `17a34be98f90542228a5011a20da1f43838ede9a32420351059394e55d558672`. Logs are private under
`local/validation/orch-20261027/app01-term-checkpoint/` and
`local/validation/orch-20261027/app01-shutdown-term-escalation/`.

The first readiness-marker follow-up failed in E774 and remains one failed iteration after one
successful initial fixture. The corrected ordering plus unused-file cleanup passed; no failed
semantic hypothesis or production change. The task is now **FIXTURE_VERIFIED**. No SIGKILL, real
core shutdown, game shutdown, boot, audio, save/load, or runtime behavior claim is made.

| Command | Result | Log SHA-256 |
|---|---|---|
| `python3 -I scripts/audit_publish_paths.py` | **PASSED**, 8 staged paths | `8d1257cbbafea49b1a1235683d339d564a30ef3e6140ef3eba7253722444629a` |
| `python3 -I scripts/audit_publish_paths.py --all` | **PASSED**, 3,013 paths | `2c2ed65cd259576fa6d92667e0b3ee100cd563754980efab77acf06c95b815b4` |
| `python3 -I scripts/check_commit_attribution.py` | **PASSED**, pre-commit range empty | `72d954d7873849934ae10cdf5ad4049069928f3c6b433f3e7e44743823e63f33` |
| `python3 -I scripts/check_commit_attribution.py --identity` | **PASSED**, owner identity | `888507b0410bfc1aa9548e21b4bd5bdaa271389b1515786e902bc91722c90559` |
| `python3 -I scripts/check_commit_attribution.py --all` | **FAILED** only for the known non-Claude historical `f6604a86e60e` (`orchestrator@local`) | `f67c8b2dabf442366fbc8922353feda1e6bbd31ffc26b3537d5161e8c1adad44` |

The private publication-audit manifest is `local/validation/orch-20261027/app01-term-checkpoint/publication/results.json`,
SHA-256 `ab09332e92e5a671e011131e7f4db18c2ad03444932a7c6d8be7ec6f0273eef1`. The pre-commit
range was empty; after commit, rerun that attribution check, then push `origin main`.


## E776: post-push public attribution scan

- Local date/time: 2026-10-27 08:13 CDT (America/Chicago).
- Agent / integrator: `/root`.
- Task ID: `PUB-01-CLAUDE-CREDIT-CLEANUP-01`; related statuses were `APP-01-PROCESS-CLIENT-SHUTDOWN-TERM-ESCALATION-01` and `CPU-01-STARTUP-SLOT-309-CHARACTERIZATION-01`.
- Files changed: `HANDOFF.md`, `docs/ATTRIBUTION.md`, `docs/EVIDENCE.md`, `docs/INDEX.md`, `docs/TASKS.md`, and this checkpoint.
- Exact command: `git fetch origin --prune && python3 -I local/validation/orch-20261026/claude-credit-cleanup-review/scan_remote_refs_attribution.py > local/validation/orch-20261027/claude-credit-cleanup-review/remote-scan-after-46a75900.json`.
- Exit/result and evidence: exit 0; report SHA-256 `5c18b1e7c6a24c8d2e2adbf9a449414093a86ad7a44a1e84f0dddb4b4de2dff7`.
- Observations vs inference: the scanner observed 9 fetched branch heads, 0 remote tag refs, 2,389 unique reachable commits, and zero Claude/Anthropic author, committer, or credit-line matches. This result covers fetched refs only; it does not include private stash refs.
- Remaining risks: the owner directed that 70 stash-only matches remain unchanged; broader CPU/game acceptance remains open.
- Next exact action: rerun repository gates after the final fixture cleanup, then stage only the allowlisted documentation and audit it before publication.
- Task status / attempts: PUB-01 **ARTIFACT_VERIFIED** for fetched GitHub refs, 0 failed hypotheses; APP-01 TERM fixture **FIXTURE_VERIFIED**, one failed readiness iteration and 0 failed semantic hypotheses; CPU-01 slot 309 **ACTIVE / G1 PARTIAL**, 0 failed hypotheses.

After owner commit `46a759003caba59b68a1fd421c07a401272c97fe` was pushed, the exact command
`git fetch origin --prune && python3 -I local/validation/orch-20261026/claude-credit-cleanup-review/scan_remote_refs_attribution.py > local/validation/orch-20261027/claude-credit-cleanup-review/remote-scan-after-46a75900.json`
exited 0. The report SHA-256 is
`5c18b1e7c6a24c8d2e2adbf9a449414093a86ad7a44a1e84f0dddb4b4de2dff7`.

Observed: 9 fetched branch heads, 0 remote tag refs, 2,389 unique reachable commit objects, and 0
Claude/Anthropic author, committer, or credit-line matches. `origin/main` equals
`46a759003caba59b68a1fd421c07a401272c97fe`. The 70 stash-only matches remain unchanged by owner
direction and are outside this public scan scope.

E776 `python3 scripts/project_index.py` exited 0 and reported 33,119 folders (log SHA-256
`1d0912f208c0911f65f9548cd51d724e3a09c31efe807b5d9f9a32b9e736d346`); generated build-directory
churn was excluded from the durable folder summary. The first `trail build --check` after the final
test-only cleanup returned **STALE** because the graph predated that cleanup. Its private log is
`local/validation/orch-20261027/post-46a759-checkpoint/trail-check.log`, SHA-256
`d7b50d6583882d502891d759dee07701a8cb5056c46950e3405a159940bb262a`. This is a graph freshness
failure, not a semantic hypothesis failure; the graph was rebuilt and the check passed in E777.

## E777: repository verification

- Local date/time: 2026-10-27 08:16 CDT (America/Chicago).
- Agent / integrator: `/root`.
- Task IDs: `APP-01-PROCESS-CLIENT-SHUTDOWN-TERM-ESCALATION-01` verification; `PUB-01-CLAUDE-CREDIT-CLEANUP-01` publication checkpoint; `CPU-01-STARTUP-SLOT-309-CHARACTERIZATION-01` stopping-point review.
- Files changed: `HANDOFF.md`, `docs/ATTRIBUTION.md`, `docs/EVIDENCE.md`, `docs/INDEX.md`, `docs/TASKS.md`, and this checkpoint.
- Exact commands, exits, and evidence: the command/result/log table follows. Trail ran as `trail build` (exit 0; `local/validation/orch-20261027/post-46a759-checkpoint/trail-build.log`, SHA-256 `d70cf6d1d88ccfe67c62cb06f0ac5b91cf35cf84b919e709b05ed9d05dff0dae`) and `trail build --check` (exit 0; `local/validation/orch-20261027/post-46a759-checkpoint/trail-check-after-build.log`, SHA-256 `eb64261b7e53ff1fb2a879c571528604a13c82c9e640c49bb3c0ba17e075d55c`). Project index ran as `python3 scripts/project_index.py` (exit 0; `local/validation/orch-20261027/post-46a759-checkpoint/project-index.log`, SHA-256 `1d0912f208c0911f65f9548cd51d724e3a09c31efe807b5d9f9a32b9e736d346`). Native logs are named in the private manifest below.
- Observations vs inference: CMake, 431 CTests, and 516 lift checkers passed at repository fixture/static scope. These outcomes do not establish game boot, gameplay audio, real save/reload, retail rendering, or a native-lift boundary for slot 309.
- Remaining risks: slot 309 payload ownership and `0x531d60` invocation order remain unknown; cold boot, audible game content, real save/reload, Metal validation, and forbidden-runtime absence remain unverified.
- Next exact action: stage and audit the six allowlisted docs, then commit and push the checkpoint under the owner identity.
- Task status / attempts: APP-01 TERM fixture **FIXTURE_VERIFIED**, one failed readiness iteration and 0 failed semantic hypotheses; PUB-01 **ARTIFACT_VERIFIED** for fetched refs, 0 failed hypotheses; CPU-01 slot 309 **ACTIVE / G1 PARTIAL**, 0 failed hypotheses.

| Command | Result | Log SHA-256 |
|---|---|---|
| `cmake -S native -B local/build/orch-r22` | **PASSED**, exit 0 | `c9e06a32f55f3035929fbe7bc8358eaa65e4673d154f6e0d5b8ef8135dfee0e9` |
| `cmake --build local/build/orch-r22 -j 12` | **PASSED**, no work to do | `8c35b1682f73592d30612d7b0fabdb384fcfff35cb5a076cb48b85b1368aa794` |
| `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure` | **PASSED**, 431/431 | `2550b9f3a678bf7349e2b8a659017e65b8e0631928b5bb4ab72e155f6835b3a9` |
| `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` | **PASSED**, 516/0/0 | `f6bd4048e58d79d1b452cdba2b9c54abb1ccac1b37de894eaae8ae7b626862db` |
| `trail build` and `trail build --check` | **PASSED**, 9,876 nodes / 13,469 edges; deep semantic layer not built | logs under `local/validation/orch-20261027/post-46a759-checkpoint/`, hashes `d70cf6d1d88ccfe67c62cb06f0ac5b91cf35cf84b919e709b05ed9d05dff0dae`, `eb64261b7e53ff1fb2a879c571528604a13c82c9e640c49bb3c0ba17e075d55c` |
| `python3 scripts/project_index.py` | **PASSED**, 33,119 folders indexed | `1d0912f208c0911f65f9548cd51d724e3a09c31efe807b5d9f9a32b9e736d346` |

Private native-gate manifest: `local/validation/orch-20261027/post-46a759-checkpoint/native/results.json`,
SHA-256 `fae178fa337e9065e58baf3c454a205a6f46428a73e96799a5309f62749e0fcd`.

CPU-01 slot 309 remains ACTIVE/G1 PARTIAL: the new saved-project census did not establish payload
ownership or invocation order. Preserve both `*(0x2dfd4b8)` branches, keep slot 309 unregistered,
and do not claim a native-lift boundary. Next exact action is limited to reusing existing reports and
the saved Ghidra database for a concrete `[RBP-0x38]` lifetime or `0x531d60` invocation-order clue;
no fresh binary import is authorized by this checkpoint.


## E778: final documentation publication audit

- Local date/time: 2026-10-27 08:22 CDT (America/Chicago).
- Agent / integrator: `/root`.
- Task IDs: `PUB-01-CLAUDE-CREDIT-CLEANUP-01` publication; `APP-01-PROCESS-CLIENT-SHUTDOWN-TERM-ESCALATION-01` fixture status; `CPU-01-STARTUP-SLOT-309-CHARACTERIZATION-01` open analysis.
- Files changed: `HANDOFF.md`, `docs/ATTRIBUTION.md`, `docs/EVIDENCE.md`, `docs/INDEX.md`, `docs/TASKS.md`, and this checkpoint. `docs/FOLDERS.md` was temporarily regenerated and restored to exclude ephemeral launcher build churn.
- Exact command/results: `git fetch origin --prune` exit 0 with `HEAD == origin/main == 46a759003caba59b68a1fd421c07a401272c97fe`; `git diff --check` exit 0; `python3 -I scripts/audit_publish_paths.py` exit 0, 6 paths; `python3 -I scripts/audit_publish_paths.py --all` exit 0, 3,013 paths; `python3 -I scripts/check_commit_attribution.py` exit 0 for the empty pre-commit range; `python3 -I scripts/check_commit_attribution.py --identity` exit 0; `python3 -I scripts/check_commit_attribution.py --all` exit 1 only for the non-Claude `orchestrator` / `orchestrator@local` identity on `f6604a86e60e`.
- Audit log paths and SHA-256 values: staged path `local/validation/orch-20261027/final-docs-checkpoint/staged_path.log` (`6320dc13834a84b5ea100aeeb1f4e29e604fe41f2d405da103d16ca4b8e2d66c`); full path `local/validation/orch-20261027/final-docs-checkpoint/all_path.log` (`2c2ed65cd259576fa6d92667e0b3ee100cd563754980efab77acf06c95b815b4`); attribution range `local/validation/orch-20261027/final-docs-checkpoint/attribution_range.log` (`72d954d7873849934ae10cdf5ad4049069928f3c6b433f3e7e44743823e63f33`); identity `local/validation/orch-20261027/final-docs-checkpoint/attribution_identity.log` (`888507b0410bfc1aa9548e21b4bd5bdaa271389b1515786e902bc91722c90559`); all-history `local/validation/orch-20261027/final-docs-checkpoint/attribution_all.log` (`f67c8b2dabf442366fbc8922353feda1e6bbd31ffc26b3537d5161e8c1adad44`).
- Project index commands: `python3 scripts/project_index.py` exit 0, 33,121 folders; log `local/validation/orch-20261027/final-docs-checkpoint/project-index-refresh.log`, SHA-256 `c1c39e4b3f351efdbddad9207e5b678dae83a1eac234961cba752b6d13d943be`. `python3 scripts/project_index.py --check` returned exit 1; first log `local/validation/orch-20261027/final-docs-checkpoint/project-index-check.log`, SHA-256 `2e25949d3a1a9f6380cbbbb70072bb07dadea692c222766fd2268d7ce91e664b`, reported stale `docs/FOLDERS.md` and `local/indexes/folders.json`; second log `local/validation/orch-20261027/final-docs-checkpoint/project-index-check-after-refresh.log`, SHA-256 `31dad00ffd5208458ee89a329fea70b0fa13494e525ddd8a7e837c7614a2266b`, reported stale `local/indexes/folders.json`. The check log itself was written under recursively inventoried `local/` after refresh, and the generated docs index contained ephemeral launcher build churn; no `docs/FOLDERS.md` change is included.
- Observations vs inference: only the staged six documentation paths passed the publication allowlist. The `--all` attribution failure is the pre-existing unrelated local alias, not Claude/Anthropic. E776 scanned 2,389 fetched GitHub-reachable commits; the new checkpoint commit will also be checked under the owner identity and by the post-commit range audit.
- Remaining risks: 70 private stash-only Claude matches remain by owner choice; `--all` retains the unrelated `orchestrator@local` finding; CPU-01 slot 309 and overall native-port acceptance are incomplete.
- Next exact action: commit these six allowlisted files as the owner, run post-commit checks, and push `origin main`.
- Task status / attempts: PUB-01 **ARTIFACT_VERIFIED** for fetched public refs, 0 failed hypotheses; APP-01 TERM fixture **FIXTURE_VERIFIED**, one failed readiness iteration and 0 failed semantic hypotheses; CPU-01 slot 309 **ACTIVE / G1 PARTIAL**, 0 failed hypotheses.
