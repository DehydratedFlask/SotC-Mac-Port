# Round 64 bounded fixture claims

Checkpoint: 2026-10-21 14:37 CDT (America/Chicago)
Integrator: `/root`
Task: three independent test-only fixture claims
Files changed at claim: `docs/TASKS.md`, `HANDOFF.md`, `INSTALL_LOG.md`, this checkpoint; the
project index is regenerated below. No test or production source is edited.

## Candidate claims

Root reviewed the three read-only scout candidates against current live files and the source graph,
checked the task/evidence registries for duplicate IDs, and ran each focused baseline. Digests below
bind the reviewed claim-time state.

| Task / owner | Single file and exact assertion | Parent / focused command | Claim-time digests |
|---|---|---|---|
| `APP-01-CORECLIENT-RUNNING-WITHOUT-ENGINE-FAILURE-COPY-FIXTURE-01` / `/root/launcher_review` | Add to `runningWithoutAnEngineIsRejectedAsFailure` a case-pattern assertion that `.failed` carries exactly `Core reported running without an engine`; avoid imposing enum-wide `Equatable`. | `APP-01-CORE-ABI-01`; `swift test --package-path launcher --scratch-path local/build/launcher --filter CoreClientTests` | Test `launcher/Tests/LauncherCoreTests/CoreClientTests.swift`: `964d5de91efb269b85c899236bb2aa2c3afb660dcbd95b763bdf605cc1109f7e`. Production `launcher/Sources/LauncherCore/CoreClient.swift`: `9f2b214486f8c3ed81ead6a237f86e494c52ccd89a33c15a9fed60de533b73f5`. |
| `DATA-02-MANIFEST-NO-TRAILING-NEWLINE-FIXTURE-01` / `/root/review_host_proto` | Add a `ParseManifestTests` case with one valid JSON object encoded as JSONL and no final newline; assert parsed path and size. Parser implementation remains untouched. | `DATA-02-ARCHIVE-OVERLAP-MAP-01`; `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_archive_overlap_map.py` | Test `scripts/test_archive_overlap_map.py`: `9a7319a4ae369c6a4df1995a7fdfa15ec04596ba27ce1d8dd12cdc545664c881`. Production `scripts/audit_archive_overlap_map.py`: `25889f7ff85db9154ba209bed4913b1bb9e1a627e8a984ef7c11580b24e982d1`. |
| `SRV-01-HOST-MAP-ALIGNMENT-JUST-OVER-1M-FIXTURE-01` / `/root/slot23_raw_xrefs` | Add `CHECK(reserve_alignment(0x100001u) == 0x200000u);` to the existing host-map alignment assertions. Helper and mapping implementation remain untouched. | `SRV-01`; `cmake --build local/build/native --target test_host_map`; `ctest --test-dir local/build/native -R '^host_map$'` | Test `native/tests/test_host_map.cpp`: `3b7b7593d1f0b29c8cd83a245219985d5cc510bb7ffd51fb259918405ea03cbf`. Header `native/core/host_map.hpp`: `283c3b8f0c98ccb48cb756e9e16ba4fd54c98babc1ea60b942f2cc2276c30bbe`. Implementation `native/core/host_map.cpp`: `0ce60c6af1e2d4f9f91f5882326bf516f20dd5e58f936f99b0efc67cbb582f72`. |

Only these three existing test files are delegated. Workers must not edit shared documentation,
production sources, build configuration, or commit. Root remains the sole integrator for shared docs,
review, independent replay, evidence, and publication. All tasks are `ACTIVE` before implementation
prompts. Failed attempts: 0 for each.

## Baseline runs and evidence

| Task | Result | Log and SHA-256 |
|---|---|---|
| APP | Exit 0; 22 tests across 2 suites; CoreClient mapping baseline clean. | `local/validation/root-round64-scout/app-baseline.log` — `a3be66b1d893a7af02948cd15ba14771b7eec48c39a95bb2c15999d8e99fd1fc` |
| DATA | Exit 0; 22 tests; parser suite baseline clean. | `local/validation/root-round64-scout/data-baseline.log` — `9b3b3ffefd77b5f6906ceaccac2d1d336fbdabf745aedd76725d3a2408c40012` |
| SRV build | Exit 0; `test_host_map` target built. | `local/validation/root-round64-scout/srv-build-baseline.log` — `c6a4b5ca13ce84a970f60a6231a0c43038e207bf62d80ce0e8e2926e5a0a94d4` |
| SRV test | Exit 0; focused CTest 1/1. | `local/validation/root-round64-scout/srv-ctest-baseline.log` — `e7f8ea9f30abb150422b4ce014d3a1afa33d590825c60c8416aaf809ca5ed2c4` |

Environment facts are recorded in `INSTALL_LOG.md`. No new dependency was installed, no
configuration changed, and build artifacts remain under `local/build/`. Baseline logs are private
under `local/validation/` and are not publishable.

## Repository baseline and limits

- At checkpoint start, `HEAD == origin/main == 992014187039f46eecb164719e9c4a1a55e1565f`.
- Preexisting user-owned `native/api/CMakeLists.txt` is modified and `native/api/host/` is untracked.
  Neither is claimed or staged.
- The focused suites validate synthetic launcher state mapping, JSONL parsing, and host alignment
  arithmetic only. They do not prove real asset behavior, memory mapping, game execution, boot,
  audio, saving, or Metal rendering. G0 remains PARTIAL; G1–G6 remain NOT RUN.
- Root's `trail build --check` passed at 3,560 nodes / 4,811 edges; only the wiring graph is current
  and the optional meaning layer remains unbuilt. Log
  `local/validation/root-round64-scout/trail-check.log`, SHA-256
  `890fc0002bc6638e16b968bc203bf870ba8a44b8b061d966624708a28d306a4a5`.

## Claim publication checkpoint

`python3 scripts/project_index.py` exited 0 and indexed 14,437 folders; log
`local/validation/root-round64-scout/project-index.log`, SHA-256
`7fa0579f463474d6f5b4e0903258a2fa8128bda7b3d9bf9b8f4f18b95fe1ba5c`. Then
`python3 scripts/project_index.py --check` exited 0 and reported current; log
`local/validation/root-round64-scout/project-index-check.log`, SHA-256
`167cc599d17d3d700959dd0fd55b102102b74b54411b96b37b8d9fccda4a0a33`.

`git fetch origin main` exited 0 and fetched the branch. Log
`local/validation/root-round64-scout/remote-ref.log`, SHA-256
`58e4bb2ebd9bd713bf3332d4cad1251aeb0b51fb26a8b06f30b3aaa431df79b3`. After fetch,
`HEAD == origin/main == 992014187039f46eecb164719e9c4a1a55e1565f`.

The first `git diff --cached --check` exited 2 because the new checkpoint's three metadata lines
used trailing spaces for Markdown line breaks. This was a documentation formatting issue, not a
fixture or source failure. The spaces were removed and the check then passed. The first output hash
was `b78eee8ae2cea5c78375cca20513b4b16320c80b428638e998d2bf7bb97c0dcf`; the successful empty
output hash is `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855` at
`local/validation/root-round64-scout/diff-check.log`. `git diff --check` also exited 0.

`python3 -I scripts/audit_publish_paths.py` exited 0 and accepted exactly 5 staged paths; log
`local/validation/root-round64-scout/audit-staged.log`, SHA-256
`4269219eea1e8d254f7666b2d08ad6a527f864bb48e65dd77a47dac1fc9f45c4`. The `--all` audit exited 0
and accepted 1,632 paths; log `local/validation/root-round64-scout/audit-all.log`, SHA-256
`3ffd562af1fed574c90c8016c13b92d5a4f2e4a04b5513fd2b6d97aff5116d97`. The exact staged paths are
`HANDOFF.md`, `INSTALL_LOG.md`, `docs/FOLDERS.md`, `docs/TASKS.md`, and this checkpoint. The
preexisting `native/api/CMakeLists.txt` modification and untracked `native/api/host/` remain
unstaged and excluded.

At 2026-10-21 14:39 CDT, commit
`218ce2134fa8e1af05c6e5e6e419cb30ceaa7310` (`docs: claim round 64 fixture tasks`) was created and
pushed to `origin main`. The commit command exited 0; its captured log before the later checkpoint
update was SHA-256 `0634e1bec7bb6ceb083291f6ac0fa21a9ef6a1f8637802b142446c47ddfd6198`. The push
exited 0; log `local/validation/root-round64-scout/push.log`, SHA-256
`5ec1fa737dba74a71d51b23fc57a97dc65216ba7681523d767a6fed4d289d3d5`. After push,
`HEAD == origin/main == 218ce2134fa8e1af05c6e5e6e419cb30ceaa7310`. Only the five audited allowlisted
paths were included. User-owned `native/api/CMakeLists.txt` and untracked `native/api/host/` remain
untouched and excluded.

The claim is published and the workers have been dispatched. Root verification and final publication
are recorded below and in the linked completion checkpoint.

At 2026-10-21 14:40 CDT, after recording the first publication, root reran
`python3 scripts/project_index.py` and `python3 scripts/project_index.py --check`. Both exited 0;
14,437 folders were indexed and the result is current. Logs are
`local/validation/root-round64-scout/project-index-final.log` (SHA-256
`7fa0579f463474d6f5b4e0903258a2fa8128bda7b3d9bf9b8f4f18b95fe1ba5c`) and
`local/validation/root-round64-scout/project-index-check-final.log` (SHA-256
`167cc599d17d3d700959dd0fd55b102102b74b54411b96b37b8d9fccda4a0a33`). The generated folder
index remains unchanged. This updates the shared HANDOFF and claim checkpoint after the first
publication; the claim itself was already pushed before any worker prompt.

## Worker dispatch

At 2026-10-21 14:40 CDT, after claim commit `218ce2134fa8e1af05c6e5e6e419cb30ceaa7310` was
confirmed on `origin/main`, root dispatched the three test-only tasks:

- `/root/launcher_review` owns only `launcher/Tests/LauncherCoreTests/CoreClientTests.swift` for
  APP-01; acceptance: `swift test --package-path launcher --scratch-path local/build/launcher
  --filter CoreClientTests`.
- `/root/review_host_proto` owns only `scripts/test_archive_overlap_map.py` for DATA-02; acceptance:
  `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_archive_overlap_map.py`.
- `/root/slot23_raw_xrefs` owns only `native/tests/test_host_map.cpp` for SRV-01; acceptance:
  `cmake --build local/build/native --target test_host_map`, then
  `ctest --test-dir local/build/native -R '^host_map$'`.

Prompts prohibit production/shared-doc edits and commits. Each worker reported one passing attempt
and 0 failures. Root reviewed the exact one-file diffs, verified the production files remain at
claim-time hashes, independently replayed each focused command, and refreshed the source graph.
E190–E192 and acceptance limits are in
[the Round 64 completion checkpoint](2026-10-21-root-round64-fixtures-complete.md). All three tasks
are `FIXTURE_VERIFIED`; this does not advance a game runtime gate.
