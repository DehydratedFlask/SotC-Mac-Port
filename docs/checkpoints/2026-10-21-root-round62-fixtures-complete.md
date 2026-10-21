# Checkpoint: Round 62 synthetic fixtures verified

- Local date/time: 2026-10-21 14:16 CDT (America/Chicago).
- Agent ID: `/root` (integrator); workers `/root/launcher_review`, `/root/review_host_proto`,
  and `/root/slot23_raw_xrefs`.
- Task IDs: `APP-01-SYSTEM-CHECKS-CORE-DIRECTORY-PATH-FIXTURE-01`,
  `AUD-01-XFF-PSARC-READ-FAILURE-CLEANUP-FIXTURE-01`, and
  `SRV-01-ALLOCATOR-SIZE-ALIGN-SECOND-64K-BOUNDARY-FIXTURE-01`.
- Base claim commit `2c30005ab434d1fceece0d13dfc451968e2c3a1a` was pushed before worker prompts.
  Each worker changed only its claimed test file; root independently reviewed and replayed all
  acceptance commands. Files changed in this completion checkpoint: `HANDOFF.md`,
  `docs/TASKS.md`, `docs/EVIDENCE.md`, this checkpoint, `docs/FOLDERS.md`,
  `launcher/Tests/LauncherCoreTests/AssetValidatorTests.swift`,
  `scripts/test_audio_bank_header.py`, and `native/tests/test_allocator_region.cpp`.

## APP-01-SYSTEM-CHECKS-CORE-DIRECTORY-PATH-FIXTURE-01

- Added one temporary-directory fixture to `AssetValidatorTests.swift`; it asserts that a directory
  named `SoCGameCore` maps to the exact `.notBuilt` message.
- Worker command `swift test --package-path launcher --scratch-path local/build/launcher --filter AssetValidatorTests`
  exited 0: 9 tests passed. Worker log `local/validation/app01-system-checks-core-directory-path/attempt-1.log`,
  SHA-256 `60c5a40320f7b5c27f06d6a064ed69ae56d6bcaa4bdcb24eac9b32b66ec5507a`.
- Root replay of the same command exited 0: 9 tests passed. Log
  `local/validation/root-round62-replay/app.log`, SHA-256
  `57ae89ed5e85e4701de52bbb977ccd9411c8ad4011719db0e2192fd8c7bc6c7a`.
- Test SHA-256 `b5d61ccad0b50131c6ebbca85d3bebd3d70884b6dbe952358bb4b59a520ee7b4`; production
  `SystemChecks.swift` is unchanged at SHA-256
  `cf3d6ce5dd5663b10291c5c53169aecb2ef02bc7912960e29af2789e60a3b0d8`.
- Observation: this pins only temporary filesystem classification and message copy. Inference: it
  does not establish that any core executable is valid or runnable. Status: `FIXTURE_VERIFIED`,
  0 failed attempts.

## AUD-01-XFF-PSARC-READ-FAILURE-CLEANUP-FIXTURE-01

- Added one synthetic reader failure fixture to `scripts/test_audio_bank_header.py`; `read(1)` raises
  `PsarcError`, the test pins the exact `AudioBankFormatError`, one read, and reader closure.
- Worker command `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_audio_bank_header.py` exited 0:
  20 tests passed. Worker log
  `local/validation/aud01-xff-psarc-read-failure-cleanup/attempt-1.log`, SHA-256
  `00486fe52cb2d4dc3c08054b47b0b5ff66513e048586a519c6349f55936e4e7f`.
- Root replay of the same command exited 0: 20 tests passed. Log
  `local/validation/root-round62-replay/aud.log`, SHA-256
  `8c415ff8f67af64cec284b21a333d46083fc9459253f9cdfdc1a96f0074786a0`.
- Test SHA-256 `90f18dd3a98578e822a3674ef3d3d71fc237b89b46eb3f350dbb3460e7cd4833`; production
  `audio_bank_header.py` is unchanged at SHA-256
  `bdad7d192b0ecdb929daf5147a139e7545fd5c546f01ed4ffb4005f2cea9f9c0`.
- Observation: a synthetic `PsarcError` is wrapped as
  `cannot read PSARC entry 1 (sound/fixture.hd): synthetic read failure`, and cleanup closes the
  reader. Inference: no real PSARC, audio decoder, or playback behavior is established. Status:
  `FIXTURE_VERIFIED`, 0 failed attempts.

## SRV-01-ALLOCATOR-SIZE-ALIGN-SECOND-64K-BOUNDARY-FIXTURE-01

- Added `CHECK(align_size_up_64k(0x10001ull) == 0x20000ull);` to
  `native/tests/test_allocator_region.cpp`.
- Worker commands `cmake --build local/build/native --target test_allocator_region` and
  `ctest --test-dir local/build/native -R '^allocator_region$'` both exited 0; CTest passed 1/1.
  Logs `local/validation/srv01-allocator-size-align-second-64k-boundary/build-attempt-1.log` and
  `attempt-1.log`, SHA-256 `012296bfcfc7dc5a3f8e3b0261ca37654d84d097e256233e37f39365cf71660f` and
  `1887560105f046ad56b88366b3f14eb1a8e52de32296e241ab1c1ae358b7b458`.
- Root replay ran the same commands; both exited 0 and CTest passed 1/1. Logs
  `local/validation/root-round62-replay/srv-build.log` and `srv-ctest.log`, SHA-256
  `40d4c8c5d57a58d1b3e380e966efa8c1c6a87b1bcbb7f7f81bfb16abb4efc181` and
  `d0988f1835655b1db5273512b7c50e48a7fe164831bb6778a2de60a9c713faaf`.
- Test SHA-256 `9f5c6ea43cc9da4c47d4ca520e1f8471eefc945ea65dbdbf9bef63c7aa4f91fa`; production
  `allocator_region.hpp` is unchanged at SHA-256
  `6bc5fc4091d6a5ce8411d0beb0020fa07e5e93d2846c9169d7081d9a03c3696a`.
- Observation: the size helper rounds this integer boundary as expected. Inference: allocator
  integration and runtime behavior remain unverified. Status: `FIXTURE_VERIFIED`, 0 failed attempts.

## Shared status and next action

- `git diff --check` passed. No dependency, install, or user game-file facts changed. Existing
  user-owned `native/api/CMakeLists.txt` and untracked `native/api/host/` were not edited or staged.
- Game gates remain G0 PARTIAL and G1–G6 NOT RUN. These fixtures do not establish cold boot,
  gameplay, authentic audio, save/reload, or Metal rendering.
- `python3 scripts/project_index.py` exited 0 at 14,426 folders; `docs/FOLDERS.md` records 361
  checkpoint files. `python3 scripts/project_index.py --check` exited 0 (`current`). Logs
  `local/validation/root-round62-replay/project-index.log` and `project-index-check.log`, SHA-256
  `4c6b24d794869e5d0ed4837f997d16bc4f99bc8f014840687863b0d9590f2edc` and
  `e311161c106ba96ee85a3db69fb443866f08538c4532a02dd0a1ad429263af37`. An intermediate check
  after audit logs were created reported stale `local/indexes/folders.json`; regeneration and the
  final check above resolved it.
- `trail build` exited 0 at 3,559 nodes / 4,808 edges; `trail build --check` exited 0. Logs
  `local/validation/root-round62-replay/trail-build.log` and `trail-check.log`, SHA-256
  `8b7af392c50f529725dff745e16bc13f67131f68f5df491b4d9a3966d2c75696` and
  `ea87a31049480db664bcbc6916e2b4441bf0cce308bea008a5c6cc73a97d1286`. Only the wiring graph is
  current; the deep meaning layer remains unbuilt. No graph files are staged.
- Staged path audit accepted 8 allowlisted paths; `--all` accepted 1,628. Logs
  `local/validation/root-round62-replay/audit-staged.log` and `audit-all.log`, SHA-256
  `8552f4ac616b2a3029965fdc9be088b54eb55c3800bb3dbb25b0a51734ce1260` and
  `322a13abb3705f2f0c788325df60bdfe47284dd5a3a9ee1b32386f77cde60faa`. Staged and working
  `git diff --check` passed. Only the three claimed tests and five documentation/index files are
  staged; user-owned `native/api` changes remain unstaged.
- Completion commit `40a08252cee4d16bec4a1ff57df6da72b3a67334` was published with `git push origin
  main`; push exited 0 (`2c30005..40a0825 main -> main`). Push log
  `local/validation/root-round62-replay/push-main.log`, SHA-256
  `af03d97be76c36f17610d49f8bcb547944e43fc3965d55bc9805284aaa9c622c`; `git rev-parse HEAD
  origin/main` matched at the completion commit, recorded in `publication-state.log`, SHA-256
  `5fb39cee8eed007120abb6138bcac9c9fb1ddc45e98f9869b0316573ed58c1fc`.
- Exact next action: publish this handoff update, then scout the next independent fixture batch.
