# Attribution scan after `9d87ef5b`

- Local date/time: 2026-10-27 03:30 CDT (America/Chicago).
- Agent / integrator: `/root`.
- Task: `PUB-01-CLAUDE-CREDIT-CLEANUP-01`.
- Status: **ARTIFACT_VERIFIED** for fetched public refs through `9d87ef5baf13fec06979693ff07316bfe034830d`; local stash scope remains **ACTIVE** pending owner direction.
- Prior checkpoint `9d87ef5baf13fec06979693ff07316bfe034830d` used the project owner identity for author and committer.

## Public-ref result (E768)

`git fetch origin --prune` exited 0. The following command exited 0:

```sh
python3 -I local/validation/orch-20261026/claude-credit-cleanup-review/scan_remote_refs_attribution.py > local/validation/orch-20261027/claude-credit-cleanup-review/remote-scan-after-9d87ef5b.json
```

It parsed 9 fetched branch heads, 0 remote tag refs, and 2,385 unique reachable commits, including
`origin/main == 9d87ef5baf13fec06979693ff07316bfe034830d`. It found zero Claude/Anthropic author or
committer identities and zero credit-line matches. Private report:
`local/validation/orch-20261027/claude-credit-cleanup-review/remote-scan-after-9d87ef5b.json`,
SHA-256 `ed223d962e1b94450fc4076c560dd83b76f163486c2dc942973bb3d1381defe0`.

E762's all-locally-stored-ref audit found 70 matching commits reachable only from `refs/stash`;
those private recovery refs remain unchanged pending the owner's scope answer. The `main` full-history
attribution audit still identifies only unrelated historical non-Claude
`f6604a86e60e` `orchestrator@local`.

## Verification

Files changed for this checkpoint: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`,
`docs/ATTRIBUTION.md`, `docs/INDEX.md`, `docs/FOLDERS.md`, and this checkpoint.

| Command | Result | Evidence |
|---|---|---|
| `cmake -S native -B local/build/orch-r22` | **PASSED**, exit 0 | `local/validation/orch-20261027/slot309-checkpoint/e768/cmake-configure.log`, SHA-256 `c9e06a32f55f3035929fbe7bc8358eaa65e4673d154f6e0d5b8ef8135dfee0e9` |
| `cmake --build local/build/orch-r22 -j 12` | **PASSED**, exit 0 (`ninja: no work to do`) | `local/validation/orch-20261027/slot309-checkpoint/e768/cmake-build.log`, SHA-256 `8c35b1682f73592d30612d7b0fabdb384fcfff35cb5a076cb48b85b1368aa794` |
| `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure` | **PASSED**, 431/431 | `local/validation/orch-20261027/slot309-checkpoint/e768/ctest.log`, SHA-256 `afe37e3ab970f454fc12e96d04b224fa2198b68b5463cd74eeda7c6960a594b5` |
| `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` | **PASSED**, 516 passed / 0 skipped / 0 failed | `local/validation/orch-20261027/slot309-checkpoint/e768/lift-checkers.log`, SHA-256 `f6bd4048e58d79d1b452cdba2b9c54abb1ccac1b37de894eaae8ae7b626862db` |
| `trail build --check` | **PASSED** wiring graph consistency; meaning tier remains 0% with 9,875 nodes pending | `local/validation/orch-20261027/slot309-checkpoint/e768/trail-build-check.log`, SHA-256 `faf3792f3bd222b9d413eefb67ccad8b2d488029cc2ffbe5c38cb8c65e5c723f` |
| `python3 scripts/project_index.py` | **PASSED**, exit 0; 33,108 folders indexed; durable checkpoint count updated 813→814 | `local/validation/orch-20261027/slot309-checkpoint/e768/project-index.log`, SHA-256 `44748f52be6dd137a303a0d85f68197bae8fc8f51674747abd4295c3222b932c` |
| `git diff --cached --check` | **PASSED**, exit 0 | `local/validation/orch-20261027/slot309-checkpoint/e768/diff-check.log`, SHA-256 `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855` |
| `python3 -I scripts/audit_publish_paths.py` | **PASSED**, 7 staged paths | `local/validation/orch-20261027/slot309-checkpoint/e768/path-audit-staged.log`, SHA-256 `88b6b00f9cbc724c8bb893bde78c0a59721738dcb0e5adbdbaec459052cc8572` |
| `python3 -I scripts/audit_publish_paths.py --all` | **PASSED**, 3,010 paths | `local/validation/orch-20261027/slot309-checkpoint/e768/path-audit-all.log`, SHA-256 `e3005e23babcb206b85b98dea5740b05aa3419f850d3f2dee41cd2add869fc1f` |
| `python3 -I scripts/check_commit_attribution.py` | **PASSED**, exit 0 (`origin/main..HEAD`) | `local/validation/orch-20261027/slot309-checkpoint/e768/attribution-precommit.log`, SHA-256 `72d954d7873849934ae10cdf5ad4049069928f3c6b433f3e7e44743823e63f33` |
| `python3 -I scripts/check_commit_attribution.py --identity` | **PASSED**, exit 0; configured identity is owner | `local/validation/orch-20261027/slot309-checkpoint/e768/attribution-identity.log`, SHA-256 `888507b0410bfc1aa9548e21b4bd5bdaa271389b1515786e902bc91722c90559` |
| `python3 -I scripts/check_commit_attribution.py --all` | **FAILED** for historic `f6604a86e60e` only (`orchestrator <orchestrator@local>`); no Claude/Anthropic identity is reported | `local/validation/orch-20261027/slot309-checkpoint/e768/attribution-all.log`, SHA-256 `f67c8b2dabf442366fbc8922353feda1e6bbd31ffc26b3537d5161e8c1adad44` |

The wrapper used to report the index command initially attempted to assign zsh's read-only
`status` variable after the command completed. The index itself exited 0 and its complete output
is the evidence above. No source or game data changed. No game execution, boot/audio/save/load,
Metal, or forbidden-runtime claim is made by this documentation checkpoint.

## Next exact action

Finish staged/all path audits and attribution checks, commit the seven allowlisted docs under the
owner identity, push `origin main`, then fetch and scan the new remote head. For slot 309, check the
saved map for `0x536aa0`, `0x537f90`, `0x536730`, or `0xf120` and preserve both registration
branches unless new order evidence is established. Resolve the stash-scope question before
modifying `refs/stash`.
