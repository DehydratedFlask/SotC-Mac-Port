# Round 67 fixture claims

**Checkpoint:** 2026-10-21 15:21 CDT (America/Chicago)
**Integrator:** `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`)
**Task status:** Five tasks claimed; 0 failed tries and 0 implementation attempts. Baselines only.
**Files changed:** `HANDOFF.md`, `docs/TASKS.md`, `docs/FOLDERS.md`, this checkpoint.
**Exact commands:** The five focused baseline commands are below. Project index commands were
`python3 scripts/project_index.py` and `python3 scripts/project_index.py --check`; source graph
command was `trail build --check`.
**Result/log evidence:** Five independent focused baselines exit successfully; project index writes
and checks current at 14,461 folders; trail reports the 3,564-node / 4,820-edge wiring graph in
sync. Logs are under `local/validation/root-round66-scout/`. No source or test file is edited.
**Observations:** Parent task statuses and behavior were verified in Graft and then against live
files. Candidate IDs have no prior row in `docs/TASKS.md` or `docs/EVIDENCE.md`. First index check
exited 1 with `Stale indexes: local/indexes/folders.json` because validation log files were created
after the generator ran. The same file-count change reproduced at
`local/validation/root-round66-scout/round67-claim-index-stale-reproduction.log`; index regeneration
then restored a clean check. The graph's optional deep meaning tier remains unbuilt. Existing
`native/api/CMakeLists.txt` and untracked `native/api/host/` are pre-existing user-owned work and
remain outside the claims.
**Inference:** These focused assertions should pin existing deterministic behavior; each result is
limited to its synthetic helper or test client. They establish no game behavior or acceptance gate.
**Remaining risks:** Baselines do not exercise a native core, game assets, launcher UI, real archive
manifests, runtime mapping, or generated-index use by downstream consumers.
**Current next exact action:** Root adds the claimed project-index fixture to
`scripts/test_project_tools.py`, then reviews and replays worker changes. The claim and dispatch
history follows below.

## Claim publication preflight

- `python3 scripts/project_index.py` exited 0 and wrote 14,461 folders to `docs/FOLDERS.md` and the
  private JSON index. `python3 scripts/project_index.py --check` exited 0 and reports current.
  Logs: `round67-claim-project-index.log` SHA-256
  `0bf7924ad18fc75b028850bb5b62c77fd27472d8199d07fa13b1142402e9a0f7`, and
  `round67-claim-project-index-check.log` SHA-256
  `9f6e98a64c5aa9718de40f66c0ae6c07604e5579604d60ddedbc3c14ed842973`.
- The stale-index reproducer check exited 1 with exactly
  `Stale indexes: local/indexes/folders.json`; log `round67-claim-index-stale-reproduction.log`,
  SHA-256 `31dad00ffd5208458ee89a329fea70b0fa13494e525ddd8a7e837c7614a2266b`.
- `trail build --check` exited 0: wiring graph in sync, 3,564 nodes / 4,820 edges; optional deep
  meaning layer remains unbuilt. Log `round67-claim-trail-check.log`, SHA-256
  `f832a0d276e12f7e5663965b3b000260b63aacce0cdce1afd66d67c0700b012d`.
- Before staging, `git status --short --branch` showed only these claim docs plus the pre-existing
  user-owned `native/api/CMakeLists.txt` and `native/api/host/`. `git diff --check` exited 0.

## Pre-push claim audit

- The first staged `git diff --cached --check` exited 2 because the newly added checkpoint used
  trailing Markdown hard-break spaces. The affected lines were stripped, then both
  `git diff --cached --check` and `git diff --check` exited 0. Failed log
  `round67-claim-diff-check-failed.log` SHA-256
  `11ce99d37a6c02f42e8b63716d66187031cf55e2d5c9e2e0b144f3f9664df45d`; passing combined log
  `round67-claim-diff-check.log` SHA-256
  `7b95a935833431420b42941a093b4235d46bba6d9c4aaf7ad4e8b88db4881440`.
- `python3 -I scripts/audit_publish_paths.py` exited 0 and accepted exactly 4 staged paths. Log
  `round67-claim-audit-staged.log`, SHA-256
  `00b9726e1edfd4c9f744e58cd0ed4d8e08e24ba311d1dadf8e78e2db2b2de7d5`.
- `python3 -I scripts/audit_publish_paths.py --all` exited 0 and accepted 1,638 repository paths.
  Log `round67-claim-audit-all.log`, SHA-256
  `4b8486fcdac766f93191f4c7ca4a86dd5ee714344aa28e02b1f68602ed6e5bf6`.
- `git status --short --branch` captured exactly the four staged claim-doc paths; the modified
  `native/api/CMakeLists.txt` and untracked `native/api/host/` remain unstaged. Log
  `round67-claim-git-status.log`, SHA-256
  `f8615d10fae7218b3a67165d2e806d7c0c7ce47d53abe577d37b5ad3ca58283c`.
- `git fetch origin` exited 0. `git rev-list --left-right --count origin/main...HEAD` returned
  `0 0`, so the pre-claim commit is aligned with `origin/main`. Fetch log
  `round67-claim-fetch.log` is empty (SHA-256
  `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`).

## Claim commit and publication

- The allowlisted claim files were committed as
  `ca9e20fedad3fa2e8d73091bb7a06ec78f7f2621` (`docs: claim Round67 fixture tasks`) and pushed to
  `origin/main`. Push log `local/validation/root-round66-scout/round67-claim-push.log`, SHA-256
  `9fc55f37222b69d2cb1a20bd0a36d5b3a9eb14d3324af5dcacb2171329b18ede`.
- After push, `HEAD == origin/main == ca9e20fedad3fa2e8d73091bb7a06ec78f7f2621`. `git status`
  contains only the pre-existing user-owned `native/api/CMakeLists.txt` and untracked
  `native/api/host/`, neither staged nor published.
- At publication time, the next action was to send the implementation prompts to the three listed
  workers. That dispatch is recorded below. Root's current next action is implementing and checking
  the project-index test.

## Worker dispatch

- **Checkpoint:** 2026-10-21 15:27 CDT (America/Chicago), `/root`.
- The claim set and publication record are pushed. Three implementation assignments were sent:
  - `/root/launcher_review`: only `LauncherModelTests.swift` and `LauncherLifecycleTests.swift`,
    each with its claim-scoped assertions and corresponding `swift test` filter.
  - `/root/review_host_proto`: only `scripts/test_archive_overlap_map.py`, with the claimed direct
    zero-byte manifest parse and focused Python test command.
  - `/root/slot23_raw_xrefs`: only `native/tests/test_host_map.cpp`, with the claimed direct
    zero-size helper check, target build, and focused CTest command.
- All three were instructed not to change production/shared documentation or commit, and to return
  exact results, log hashes, changed test hashes, and unchanged production hashes. No implementation
  result has been reviewed yet; task attempt counts remain 0.
- Root owns only `scripts/test_project_tools.py` for the fifth claimed fixture, plus shared docs and
  review. **Next exact action:** add separate temporary-root cases identifying stale `docs/FOLDERS.md`
  and stale `local/indexes/folders.json`, then run
  `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_project_tools.py`.

## Worker attempt results reported; root review pending

**Checkpoint:** 2026-10-21 15:28 CDT (America/Chicago), `/root`. The following are worker reports,
not yet root-verified results. Each task has one successful worker attempt and zero failed tries.

- `/root/launcher_review` reports only the two claimed launcher test files changed. The no-folder
  query fixture passes 8 tests; log
  `local/validation/round67-launcher-no-folder-core-queries/attempt-1.log`, SHA-256
  `54bd7085315224ab176880da4949a1632368b1964946d8625ad76599c2929f30`. The lifecycle fixture
  passes 11 tests across 2 suites; log
  `local/validation/round67-lifecycle-canstart-transitions/attempt-1.log`, SHA-256
  `e63353699625140e5c91065219f6ca5c8365ca1b3eb908f32fb8647ae5e66845`. Test hashes are
  `LauncherModelTests.swift` `51f1f531998316a91cc52a86f4695a43285d277eed3d19b35299abdf6742d0cc`
  and `LauncherLifecycleTests.swift` `f4cacf8fd80c73842184d8f44888e83565d86c8b8f5f0cec8b28f87b70eb1ee3`.
  Production hashes reportedly remain at claim values.
- `/root/review_host_proto` reports only `scripts/test_archive_overlap_map.py` changed; the direct
  zero-byte member parse passes 24 tests. Log
  `local/validation/data02-manifest-zero-byte-member-size/attempt-1.log`, SHA-256
  `c0346c60690daa52884ca1114c4cae2bc20614b9cc61f89502b71ae3d4e8ed8b`. Test SHA-256
  `fc472fb7a1fb9b1bd7f0d635cc6b200ccbdf870c149a6ffc0bb09fd9756d6ebe`; parser hash reportedly
  unchanged at the claim value.
- `/root/slot23_raw_xrefs` reports only `native/tests/test_host_map.cpp` changed; target build exits
  0 and focused CTest passes 1/1. Build log
  `local/validation/round67-scout-host-map-zero-size/build-attempt-1.log`, SHA-256
  `0524da2e8cc2458ec343a44bc4b73f53179647b2df39031b43c2b170235a77a1`; CTest log
  `local/validation/round67-scout-host-map-zero-size/attempt-1.log`, SHA-256
  `d84681f951406b360a27f32c8aa141b3a7c7d73a32a4049a7576ce9545a86f84`. Test SHA-256
  `90124f68bc8a57aa35eb8ed0c9800da37bf09fb81bd9416c1ea25b44edc35afd`; production and header
  hashes reportedly remain at claim values.

All worker claims remain `ACTIVE` until root verifies exact diffs and independently replays each
focused command. Their coverage remains synthetic/test-client-only.

## Dispatch-record formatting check

- A staged whitespace check during this update exited 2 for two trailing spaces in the checkpoint
  sentence `history follows below.`. The formatting was stripped; the staged and worktree checks
  now both exit 0. Failed log `local/validation/root-round66-scout/round67-claim-diff-check-dispatch-failed.log`,
  SHA-256 `3fa3bc24fee5064480d2fb1232befc4723ef12e8360fc4a774ac3fcfcd701d3d`; passing log
  `round67-claim-diff-check.log`, SHA-256
  `7b95a935833431420b42941a093b4235d46bba6d9c4aaf7ad4e8b88db4881440`.

## Claims

### APP-01-LAUNCHER-NO-FOLDER-CORE-QUERY-FIXTURE-01

- **Parent:** `APP-01`, `ACTIVE`.
- **Owner / only implementation file:** `/root/launcher_review`; `launcher/Tests/LauncherCoreTests/LauncherModelTests.swift`.
- **Assertion:** Extend `noFolderChosenNeverAsksTheCoreToValidate()` to require exactly one `coverage()` and one `status()` scripted-client query after `await model.validate()`, while retaining its zero-validation assertion.
- **Baseline command/result:** `swift test --package-path launcher --scratch-path local/build/launcher --filter LauncherModelValidationTests`; exit 0, 8 tests. Root log `local/validation/root-round66-scout/root-round67-app-model-baseline.log`, SHA-256 `c97bbb86ec0a04a0aa07f87c9bb6dc6fd8fee431ac50771629c27a6f06427950`. Scout log `local/validation/round67-launcher-no-folder-core-queries/baseline-1.log`, SHA-256 `602dccd619774e38491ff0d33015d27b11e1820ec327e01f21fc1f588edfb2a9`.
- **Claim-time hashes:** Test `d8242785effd9b52355aba366d558b5b41f37839218c9d536cf62d29a9e387cb`; production `launcher/Sources/LauncherCore/LauncherModel.swift` `ae6ce2e1b33a83c2baae76efd63a3f2beacc0a62202c6a62ab9bb96644be6d51`.
- **Observation/limit:** Live `gather` skips only validation when the directory is nil; coverage and status calls are unconditional. This pins scripted call counts only.
- **Attempt:** 0 failed; 0 implementation attempts.

### APP-01-LIFECYCLE-CANSTART-TRANSIENT-STATE-GATES-FIXTURE-01

- **Parent:** `APP-01-LIFECYCLE-UX-01`, `FIXTURE_VERIFIED`; overarching `APP-01` is `ACTIVE`.
- **Owner / only implementation file:** `/root/launcher_review`; `launcher/Tests/LauncherCoreTests/LauncherLifecycleTests.swift`.
- **Assertion:** Add false `LauncherLifecycle.canStart(readinessAllowsStart: true, from:)` expectations for `.validating`, `.starting`, and `.stopping`.
- **Baseline command/result:** `swift test --package-path launcher --scratch-path local/build/launcher --filter LauncherLifecycleTests`; exit 0, 11 tests across 2 suites. Root log `local/validation/root-round66-scout/root-round67-app-lifecycle-baseline.log`, SHA-256 `3347314dde8b45095b1a6e4530bab2a66caeb2375a521605ceb989f8c2c1776e`. Scout log `local/validation/round67-lifecycle-canstart-transitions/baseline-1.log`, SHA-256 `dac97b4fe800c97830ff94f190fc6c39f1c6818c9bdec44c3c4e6d44e4225233`.
- **Claim-time hashes:** Test `623f8ad4d8d452f1dc00f1516d06174f4be28d7a6072ad1145c55170b154b557`; production `launcher/Sources/LauncherCore/LauncherLifecycle.swift` `358e78ae1de70957fd01100e9969916ab4c6c81a0e52365484251087d8c248c4`.
- **Observation/limit:** The pure helper excludes SwiftUI button interaction and runtime process lifecycle behavior.
- **Attempt:** 0 failed; 0 implementation attempts.

### DATA-02-MANIFEST-ZERO-BYTE-MEMBER-SIZE-FIXTURE-01

- **Parent:** `DATA-02-ARCHIVE-OVERLAP-MAP-01`, `ARTIFACT_VERIFIED`.
- **Owner / only implementation file:** `/root/review_host_proto`; `scripts/test_archive_overlap_map.py`.
- **Assertion:** Add a `ParseManifestTests` record `b'{"path":"empty.bin","size":0}\n'` and assert `{"empty.bin": 0}`.
- **Baseline command/result:** `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_archive_overlap_map.py`; exit 0, 23 tests. Root log `local/validation/root-round66-scout/root-round67-data-baseline.log`, SHA-256 `c836e74252ccf0a8e9c16af8b4b10a20f7de8317734985df83dc620cadc8d267`. Scout log `local/validation/round67-data02-manifest-boundary-baseline.log`, SHA-256 `0e97cc4540d0caadb9e31a693798a7a5a96333809d2500758a85472c298659a1`.
- **Claim-time hashes:** Test `b9444f8f8da78cb387d5f390c0dbb0f5a00c45426eb94fe585d8253cf0a00ac4`; production `scripts/audit_archive_overlap_map.py` `25889f7ff85db9154ba209bed4913b1bb9e1a627e8a984ef7c11580b24e982d1`.
- **Observation/limit:** Parser rejects booleans and negative values but accepts zero; existing valid fixture uses a positive size. Synthetic parsing only, not a claim about real game manifests or downstream acceptance.
- **Attempt:** 0 failed; 0 implementation attempts.

### SRV-01-HOST-MAP-ZERO-SIZE-HELPER-FIXTURE-01

- **Parent:** `SRV-01`, `ACTIVE`.
- **Owner / only implementation file:** `/root/slot23_raw_xrefs`; `native/tests/test_host_map.cpp`.
- **Assertion:** Add `CHECK(!map_anonymous_aligned(0, 0x100000u).has_value());` to exercise the helper's zero-size guard directly.
- **Baseline commands/results:** `cmake --build local/build/native --target test_host_map` exits 0; `ctest --test-dir local/build/native -R '^host_map$'` passes 1/1. Root logs `local/validation/round67-scout-host-map-zero-size/root-build-baseline.log` SHA-256 `c6a4b5ca13ce84a970f60a6231a0c43038e207bf62d80ce0e8e2926e5a0a94d4`, and `root-ctest-baseline.log` SHA-256 `50c4307e2702737affb26b07026016663a300e6e324d4a6a81cae7f6819c269d`. Scout logs are `build-baseline.log` (same build hash) and `ctest-baseline.log` SHA-256 `b579355abbc96298fb9f55b71b7e3c1ff3c0538be37d610549ded4fba5b04bb8`.
- **Claim-time hashes:** Test `6a66502c1db4c23beac5aa5b2ba10b9cc0565d668a1e314ffe26e90e30884270`; production `native/core/host_map.cpp` `0ce60c6af1e2d4f9f91f5882326bf516f20dd5e58f936f99b0efc67cbb582f72`; header `native/core/host_map.hpp` `283c3b8f0c98ccb48cb756e9e16ba4fd54c98babc1ea60b942f2cc2276c30bbe`.
- **Observation/limit:** Existing empty request returns from `map_region` before calling this helper; this directly covers the helper branch. Synthetic helper only, no allocator or game-runtime claim.
- **Attempt:** 0 failed; 0 implementation attempts.

### DOC-FOLDER-INDEX-STALE-OUTPUT-IDENTIFICATION-01

- **Parent:** `DOC-FOLDER-INDEX-CHECK-READONLY-01`, `FIXTURE_VERIFIED`.
- **Owner / only implementation file:** `/root`; `scripts/test_project_tools.py`.
- **Assertion:** In separate temporary roots, generate the index, alter exactly one output (`docs/FOLDERS.md` or `local/indexes/folders.json`), call `generate(root, check=True)`, and assert the exact stale error names only that modified output.
- **Baseline command/result:** `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_project_tools.py`; exit 0, 8 tests. Root log `local/validation/root-round66-scout/root-round67-project-tools-baseline.log`, SHA-256 `d1737db24da81b3c3a0b18702ce65bfadfb6524254e681f8bc84ee0e82703fbb`.
- **Claim-time hashes:** Test `88f846bd064404579c56f4c5a127b4c232de093ca6ee260fbe08d37ae82e04db`; production `scripts/project_index.py` `394ec07b93f1120e4fe2f5459cc0663672ad0e2165ab87afaa568bacdf40b9d9`.
- **Observation/limit:** Existing check-mode coverage rejects both outputs missing together and reports a newly added project folder generically. New fixture is scoped to independent error identification in a temporary root; it does not exercise a user's live project index.
- **Attempt:** 0 failed; 0 implementation attempts.
