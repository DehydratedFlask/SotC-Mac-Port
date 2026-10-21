# Round 67 fixtures complete

**Checkpoint:** 2026-10-21 15:36 CDT (America/Chicago)
**Integrator:** `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`)
**Task status:** Five claimed synthetic fixtures pass worker and root checks; each has 1 successful
implementation attempt and 0 failed tries.
**Files changed:** Five claimed test files, `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`,
`docs/FOLDERS.md`, and this checkpoint. Existing user-owned `native/api/CMakeLists.txt` and
untracked `native/api/host/` were not changed by this round and remain unstaged.
**Acceptance limit:** All checks are unit/helper/client fixtures only. G0 remains PARTIAL; G1–G6
remain NOT RUN. No game executable, cold boot, actual game audio, real save/reload, or Metal gameplay
is demonstrated.

## Verified fixture results

### E199 — launcher core queries with no selected asset folder

- Changed test `launcher/Tests/LauncherCoreTests/LauncherModelTests.swift`, SHA-256
  `51f1f531998316a91cc52a86f4695a43285d277eed3d19b35299abdf6742d0cc`. Existing test now asserts
  `validate == 0`, `coverage == 1`, and `status == 1` after validation with a nil directory.
- Unchanged production `launcher/Sources/LauncherCore/LauncherModel.swift`, SHA-256
  `ae6ce2e1b33a83c2baae76efd63a3f2beacc0a62202c6a62ab9bb96644be6d51`.
- Exact command: `swift test --package-path launcher --scratch-path local/build/launcher --filter LauncherModelValidationTests`.
- Worker and root each pass 8 tests in 1 suite. Worker log
  `local/validation/round67-launcher-no-folder-core-queries/attempt-1.log`, SHA-256
  `54bd7085315224ab176880da4949a1632368b1964946d8625ad76599c2929f30`; root replay
  `local/validation/root-round66-scout/round67-app-model-root-replay.log`, SHA-256
  `60b4990a0fe03e8985ae2e5a1d609d1646f5ee8f6df05e6ea6907c7fc75557ea`.
- Limit: scripted query counts only; no native-core behavior.

### E200 — launcher `canStart` transient-state gates

- Changed test `launcher/Tests/LauncherCoreTests/LauncherLifecycleTests.swift`, SHA-256
  `f4cacf8fd80c73842184d8f44888e83565d86c8b8f5f0cec8b28f87b70eb1ee3`. Added false expectations
  for `.validating`, `.starting`, and `.stopping` with readiness true.
- Unchanged production `launcher/Sources/LauncherCore/LauncherLifecycle.swift`, SHA-256
  `358e78ae1de70957fd01100e9969916ab4c6c81a0e52365484251087d8c248c4`.
- Exact command: `swift test --package-path launcher --scratch-path local/build/launcher --filter LauncherLifecycleTests`.
- Worker and root each pass 11 tests across 2 suites. Worker log
  `local/validation/round67-lifecycle-canstart-transitions/attempt-1.log`, SHA-256
  `e63353699625140e5c91065219f6ca5c8365ca1b3eb908f32fb8647ae5e66845`; root replay
  `local/validation/root-round66-scout/round67-app-lifecycle-root-replay.log`, SHA-256
  `2fa27bc0c3e197bd41dce1c16e814e06fd316780f1cdb4b19194b25a43df82b8`.
- Limit: pure helper only; no SwiftUI button interaction or live process lifecycle.

### E201 — zero-byte synthetic manifest member

- Changed test `scripts/test_archive_overlap_map.py`, SHA-256
  `fc472fb7a1fb9b1bd7f0d635cc6b200ccbdf870c149a6ffc0bb09fd9756d6ebe`. The JSONL record with
  `size: 0` parses to `{"empty.bin": 0}`.
- Unchanged production `scripts/audit_archive_overlap_map.py`, SHA-256
  `25889f7ff85db9154ba209bed4913b1bb9e1a627e8a984ef7c11580b24e982d1`.
- Exact command: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_archive_overlap_map.py`.
- Worker and root each pass 24 tests. Worker log
  `local/validation/data02-manifest-zero-byte-member-size/attempt-1.log`, SHA-256
  `c0346c60690daa52884ca1114c4cae2bc20614b9cc61f89502b71ae3d4e8ed8b`; root replay
  `local/validation/root-round66-scout/round67-data-root-replay.log`, the same SHA-256
  `c0346c60690daa52884ca1114c4cae2bc20614b9cc61f89502b71ae3d4e8ed8b`.
- Limit: synthetic JSONL parsing only; no real manifest or downstream asset acceptance.

### E202 — direct zero-size host-map helper check

- Changed test `native/tests/test_host_map.cpp`, SHA-256
  `90124f68bc8a57aa35eb8ed0c9800da37bf09fb81bd9416c1ea25b44edc35afd`. Added the claimed direct
  `map_anonymous_aligned(0, 0x100000u)` rejection assertion.
- Unchanged production `native/core/host_map.cpp`, SHA-256
  `0ce60c6af1e2d4f9f91f5882326bf516f20dd5e58f936f99b0efc67cbb582f72`; unchanged header
  `native/core/host_map.hpp`, SHA-256
  `283c3b8f0c98ccb48cb756e9e16ba4fd54c98babc1ea60b942f2cc2276c30bbe`.
- Exact commands: `cmake --build local/build/native --target test_host_map` and
  `ctest --test-dir local/build/native -R '^host_map$'`.
- Worker and root target builds exit 0; focused CTest passes 1/1. Worker build log
  `local/validation/round67-scout-host-map-zero-size/build-attempt-1.log`, SHA-256
  `0524da2e8cc2458ec343a44bc4b73f53179647b2df39031b43c2b170235a77a1`; worker CTest log
  `local/validation/round67-scout-host-map-zero-size/attempt-1.log`, SHA-256
  `d84681f951406b360a27f32c8aa141b3a7c7d73a32a4049a7576ce9545a86f84`. Root build log
  `local/validation/root-round66-scout/round67-host-map-build-root-replay.log`, SHA-256
  `c6a4b5ca13ce84a970f60a6231a0c43038e207bf62d80ce0e8e2926e5a0a94d4`; root CTest log
  `local/validation/root-round66-scout/round67-host-map-ctest-root-replay.log`, SHA-256
  `cfd32f1c8272b8585ab8934c8b381aa76d4f6030f396633ece07ec86df8cbe05`.
- Limit: synthetic helper behavior only; no allocator integration or game-runtime claim.

### E203 — identify each stale project-index output

- Changed test `scripts/test_project_tools.py`, SHA-256
  `986821a09679e2080ee653d9911ef8c032b61f1b3cc43f1af18fe31bcbc594f4`. Separate temporary roots
  alter only one generated output each and assert the exact stale-output error.
- Unchanged production `scripts/project_index.py`, SHA-256
  `394ec07b93f1120e4fe2f5459cc0663672ad0e2165ab87afaa568bacdf40b9d9`.
- Exact command: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_project_tools.py`; 9 tests pass.
  Root log `local/validation/root-round66-scout/round67-project-tools-attempt-1.log`, SHA-256
  `542478c5e228cffe15731b293460e1c4bc97eab58d811a31f3775a96f09a3ee3`.
- Limit: check-mode error identification with temporary files only; no live project-index consumer.

## Repository checks

- `trail build --follow-nested-repos --only-dir scripts --only-dir tools --only-dir reference --only-dir native --only-dir launcher --only-dir tests` exited 0; graph now has 3,566 nodes and 4,824 edges. Log `local/validation/root-round66-scout/round67-trail-build.log`, SHA-256 `c453efbc9faa2b25747bc071f68801488ba3b3288f7bd2009aca8e6f769a70a5`.
- `trail build --check` exited 0; the wiring graph is in sync. The optional deep meaning layer remains unbuilt. Log `local/validation/root-round66-scout/round67-trail-check.log`, SHA-256 `0ec7755d52776f1355230d9cca6db9396c02eb77d44c27b4102624de605e0041`.
- `python3 scripts/project_index.py` exited 0 and wrote the indexes: 14,462 folders indexed. Log `local/validation/root-round66-scout/round67-completion-project-index.log`, SHA-256 `46655ba58e53060f219cbb3fa420b672d6074832d97952f4a8cbd6d47f5bd44f`. `python3 scripts/project_index.py --check` exited 0 with “14,462 folders indexed; current”. Log `local/validation/root-round66-scout/round67-completion-project-index-check.log`, SHA-256 `97db2b2f2954142fdf8de1fb2d55239f3cb17ec1cef92b9a89afff123534250b`.
- No install, dependency, asset, runtime, or game-gate facts changed.

## Publication state

- The five fixture changes and completion records were committed as `393d26e96e46ed1691928dc23855c7095d15487a` (`test: add Round67 launcher, archive, map, and index fixtures`) and pushed to `origin/main`. Push log `local/validation/root-round66-scout/round67-completion-push.log`, SHA-256 `3ebda6595d988d82d5b4714d152cd5cd4c20f8908041ba1f14836fc4e1e71523`. Post-push state log `local/validation/root-round66-scout/round67-completion-publish-state.log`, SHA-256 `7978964921216cee26dce84d7fa47b95936d588047451bbb1a6ad24aabc51bd1`, verified `HEAD == origin/main == 393d26e96e46ed1691928dc23855c7095d15487a` and showed only the pre-existing user-owned `native/api/CMakeLists.txt` modification and untracked `native/api/host/` outside the commit.
- The fixture commit contained exactly 10 allowlisted paths. Staged audit passed for 10 paths (log SHA-256 `80d1e6191da0abb83c5858979daecc58a882a0b0fb8e63a3b060562621f7cf9a`); full audit passed for 1,639 paths (log SHA-256 `1403dc002af1b8e55f7312d91c75947b44d16f258f1a536854f6fe1822712b6e`). `git diff --cached --check` passed with empty output (log SHA-256 `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`).
- A post-push documentation sync updates this checkpoint with the final index/audit evidence. Its first project-index check failed with `Stale indexes: local/indexes/folders.json` after private publication logs were created. The combined log `local/validation/root-round66-scout/round67-sync-index-check.log` preserves that failure, the refresh, and subsequent passing checks at 14,462 folders; SHA-256 `5408017d8505cc1de9e690dfc946f84c6cd88a4cb12e71e65fc12aea4c4ab131`.
- The sync commit is limited to `HANDOFF.md` and this checkpoint. Existing user-owned `native/api/CMakeLists.txt` and untracked `native/api/host/` remain untouched and unstaged. Staged/full audits and post-fetch state for this documentation sync will be added below before its push.
- Exact next action: complete the documentation-sync audits, fetch and compare `origin/main`, then commit and push the two reviewed documentation paths. After that, scout the next bounded batch from the live repository; G0 remains PARTIAL and G1–G6 remain NOT RUN.
