# Checkpoint: Round 62 fixture claims

- Local date/time: 2026-10-21 14:14 CDT (America/Chicago).
- Agent ID: `/root` (integrator); assigned owners after publication: `/root/launcher_review`,
  `/root/review_host_proto`, and `/root/slot23_raw_xrefs`.
- Base commit: `f3bfa078a0b4b5b9d4ea04147355ef998ba95cc4`; `origin/main` matched before claims.
- Three independent test-only tasks have unique IDs and passing pre-edit baselines. Failed fixture
  attempts: 0 for all tasks. No implementation prompts were sent before this claim checkpoint.

## Candidate discovery and root verification

- Root ran `trail ask "Identify small isolated synthetic fixture gaps in the current launcher Swift
  model, audio-header classifier, and native region helpers; name exact source/test files and
  boundary behaviors suitable for separate test-only tasks" --source`. It returned candidate spans
  in launcher model/lifecycle code and `scripts/test_audio_bank_header.py`; these were candidate
  areas, not proof of missing coverage.
- The launcher scout proposed an existing-directory executable-path case. Root verified the live
  branch in `SystemChecks.nativeCore(executable:)` and the absence of that specific case in
  `AssetValidatorTests.swift`. Task ID was checked against both `docs/TASKS.md` and `docs/EVIDENCE.md`.
- The audio scout proposed a PSARC read error after a unique path match. Root used
  `trail grep "cannot read PSARC entry"`, which found the wrapper in `scripts/audio_bank_header.py`
  at line 147, then verified the live cleanup `defer` and the existing success, summary, identity,
  zero-match, and duplicate-match fixtures. Task ID was checked against both registries.
- The native scout proposed the second 64 KiB block rounding boundary. Root used
  `trail grep "align_size_up_64k"`; live helper and existing tests show coverage for 1 byte, exact
  64 KiB, and the 0x6400000 aligned heap size, but not a non-aligned value above 64 KiB. Task ID was
  checked against both registries.
- The Graft wiring graph check exited 0 at 3,557 nodes; the optional meaning layer remains unbuilt.
  Log `local/validation/root-round62-scout/trail-check.log`, SHA-256
  `5dd1786553d23e50de436a9292a17ca6f8a6749d89368567d3d68306242c8859`.

## APP-01-SYSTEM-CHECKS-CORE-DIRECTORY-PATH-FIXTURE-01

- Owner `/root/launcher_review`; may edit only `launcher/Tests/LauncherCoreTests/AssetValidatorTests.swift`.
- Add a temporary directory named `SoCGameCore`, pass its URL to
  `SystemChecks.nativeCore(executable:)`, and assert the exact `.notBuilt` message:
  `NOT BUILT: SoCGameCore is a directory, not an executable. Play disabled until a native SoCGameCore exists`.
- Acceptance command: `swift test --package-path launcher --scratch-path local/build/launcher --filter AssetValidatorTests`.
- Root baseline exited 0: 8 tests in 1 suite. Log
  `local/validation/root-round62-scout/app-baseline.log`, SHA-256
  `d5ee19404da842009e06a2f69ea6a9813104bcff02594271f4c93db96c31d15e`.
- Test SHA-256 `77ee2181a7e778fae3fa69e314c9daf63efe0b19ddb86623a91d56d02139feab`; unchanged
  production `launcher/Sources/LauncherCore/SystemChecks.swift` SHA-256
  `cf3d6ce5dd5663b10291c5c53169aecb2ef02bc7912960e29af2789e60a3b0d8`.
- Limit: synthetic filesystem classification/copy only; no executable validity or runtime behavior.

## AUD-01-XFF-PSARC-READ-FAILURE-CLEANUP-FIXTURE-01

- Owner `/root/review_host_proto`; may edit only `scripts/test_audio_bank_header.py`.
- Patch the synthetic reader's `read(1)` to raise `abh.PsarcError("synthetic read failure")`; assert
  exact wrapping as `cannot read PSARC entry 1 (sound/fixture.hd): synthetic read failure` and that
  the reader closes.
- Acceptance command: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_audio_bank_header.py`.
- Root baseline exited 0: 19 tests. Log `local/validation/root-round62-scout/aud-baseline.log`,
  SHA-256 `d604d5b4a307d14f9b407f9dc4ec3c155113ee4ba72b98ed71fc618e7f6974db`.
- Test SHA-256 `e06ae817af095c524175eb8681521807ad3e7e65ec936016ee546d970e25915d`; unchanged
  production `scripts/audio_bank_header.py` SHA-256
  `bdad7d192b0ecdb929daf5147a139e7545fd5c546f01ed4ffb4005f2cea9f9c0`.
- Limit: synthetic error wrapping and reader cleanup only; no real PSARC or audio decoding claim.

## SRV-01-ALLOCATOR-SIZE-ALIGN-SECOND-64K-BOUNDARY-FIXTURE-01

- Owner `/root/slot23_raw_xrefs`; may edit only `native/tests/test_allocator_region.cpp`.
- Add the boundary assertion `CHECK(align_size_up_64k(0x10001ull) == 0x20000ull);`.
- Acceptance commands: `cmake --build local/build/native --target test_allocator_region`, then
  `ctest --test-dir local/build/native -R '^allocator_region$'`.
- Root baseline build exited 0; log `local/validation/root-round62-scout/srv-build-baseline.log`,
  SHA-256 `40d4c8c5d57a58d1b3e380e966efa8c1c6a87b1bcbb7f7f81bfb16abb4efc181`. CTest exited 0,
  1/1 passed; log `local/validation/root-round62-scout/srv-ctest-baseline.log`, SHA-256
  `db3f8595523db6fc5b31eb9aefa526d405f1831c9339c52335ac5a5fa830669c`.
- Test SHA-256 `405d9ae57520f2f0fa7abb93733236220d3828d32dd95c478fd4a621d27c5a76`; unchanged
  production `native/core/allocator_region.hpp` SHA-256
  `6bc5fc4091d6a5ce8411d0beb0020fa07e5e93d2846c9169d7081d9a03c3696a`.
- Limit: synthetic integer rounding only; no allocator integration or runtime behavior.

## Shared boundary and next action

- All task files are distinct. Baselines were run at commit `f3bfa07`; no code changes exist in the
  three claimed files. Existing user-owned `native/api/CMakeLists.txt` and untracked
  `native/api/host/` remain excluded.
- `python3 scripts/project_index.py` exited 0 at 14,422 folders; `docs/FOLDERS.md` lists 360
  checkpoints. Log `local/validation/root-round62-scout/project-index.log`, SHA-256
  `34122898ccd11d56cfe396e70ee6611d3b3ccbe2f57944826028cd2509ee3aae`. The follow-up
  `python3 scripts/project_index.py --check` exited 0 with `14422 folders indexed; current`; log
  `local/validation/root-round62-scout/project-index-check.log`, SHA-256
  `29e3191870dfdca0e137dbad94b6d20e4e3a918164f8877984fac35ad71159d0`.
- Before claim publication, `python3 -I scripts/audit_publish_paths.py` accepted 5 documentation
  paths; log `local/validation/root-round62-scout/audit-staged.log`, SHA-256
  `af6d2238fe1393f9240c96be614b654d8c26405bb60fa7863844e14f01ea0da3`. The `--all` audit accepted
  1,627 paths; log `local/validation/root-round62-scout/audit-all.log`, SHA-256
  `2223343937e29d32840709ba62f249313597ccf82638e583d06a9cb4d7a2b09e`. Cached and working
  `git diff --check` both exited 0. Existing user-owned `native/api` files are unstaged.
- Game gates remain G0 PARTIAL and G1–G6 NOT RUN. No dependency or install facts changed.
- Exact next action: stage the documentation update, commit and push `origin main`; send each worker
  its single-file prompt only after the claim checkpoint is published.
