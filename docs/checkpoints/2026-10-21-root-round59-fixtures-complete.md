# Checkpoint: Round 59 synthetic fixtures verified

- Local date/time: 2026-10-21 13:42 CDT (America/Chicago).
- Agent ID: `/root` (integrator), with `/root/launcher_review`, `/root/review_host_proto`, and
  `/root/slot23_raw_xrefs` as single-file workers.
- Task IDs: `APP-01-PRESENTATION-ENGINE-UNAVAILABLE-ACCESSIBILITY-COPY-FIXTURE-01`,
  `DATA-02-PSARC-MANIFEST-NUL-PRECEDENCE-FIXTURE-01`, and
  `SRV-01-ALLOCATOR-ALIGNMENT-ROUND-UP-BOUNDARIES-FIXTURE-01`.
- Files changed by the tasks: `launcher/Tests/LauncherCoreTests/LauncherModelTests.swift`,
  `scripts/test_psarc.py`, and `native/tests/test_allocator_region.cpp`. Production sources remain
  unchanged: `launcher/Sources/LauncherCore/LauncherPresentation.swift` SHA-256
  `717e18a1a7452ef8be060a651714e7bf2c122c7df72c0660f919927516f52564`, `scripts/psarc.py` SHA-256
  `39f2c748db013637aba8144205dc60b7e5b1eb49ee0f641960783a1e2c140485`, and
  `native/core/allocator_region.hpp` SHA-256
  `6bc5fc4091d6a5ce8411d0beb0020fa07e5e93d2846c9169d7081d9a03c3696a`.

## APP-01 accessibility copy

- Added one exact assertion for `.failed("x")` with `.engineNotImplemented`:
  `Unavailable. Not running. Unavailable: the native game engine is not implemented yet, so the game cannot start.`
- Exact command, worker and root: `swift test --package-path launcher --scratch-path local/build/launcher --filter LauncherPresentationTests`.
- Worker: exit 0, 3 tests in 1 suite; log
  `local/validation/app01-presentation-engine-unavailable-accessibility-copy/attempt-1.log`,
  SHA-256 `1dcffec9aa7ba372b9785075471b7806240a5c17d3b4aa4df11f9d55d6a243ce`.
- Root: exit 0, 3 tests in 1 suite; log `local/validation/root-round59-replay/app01.log`,
  SHA-256 `6ae2f3ac0b33a663a6109b17dce46a281db36adfd71209d933f2916453510ab0`.
- Test file SHA-256 `d8242785effd9b52355aba366d558b5b41f37839218c9d536cf62d29a9e387cb`. Zero failed
  tries. This is synthetic presentation mapping; runtime VoiceOver and a live launcher are not
  exercised.

## DATA-02 NUL manifest precedence

- Attempt 1 used one member path and failed because a one-item `join` emits no NUL delimiter. The
  newline fallback split the path and raised `PsarcError: manifest has 2 paths for 1 members`.
  Failure log and hashes are recorded in E174 and
  `docs/checkpoints/2026-10-21-root-round59-data02-attempt1-failed.md`. This was a fixture
  construction failure; it did not demonstrate a parser defect.
- Attempt 2 used two paths, with a newline in the first, and explicitly asserted the manifest bytes
  contain NUL. Exact command: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_psarc.py`.
- Root: exit 0, 33 tests passed; log `local/validation/root-round59-replay/data02-attempt2.log`,
  SHA-256 `504b6e741680bbf1a6825df35a8304e68435bc04e78e6475fad0814afd034e7a`.
- Final test file SHA-256 `b8cfdd95dbf7d61fe6a67a0834da6dd4ef797c501212b1abd2cbd45bca2d8313`; parser hash
  is the unchanged value listed above. One failed fixture-construction hypothesis. This verifies
  only synthetic PSARC path delimiter precedence.

## SRV-01 alignment helper boundaries

- Added valid boundary assertions `align_up_64k(0xffff) == 0x10000u` and
  `align_up_1m(0xfffff) == 0x100000u`.
- Exact worker and root commands:
  `cmake --build local/build/native --target test_allocator_region` and
  `ctest --test-dir local/build/native -R '^allocator_region$'`.
- Worker build and CTest exited 0; target built and 1/1 passed. Logs:
  `local/validation/srv01-allocator-alignment-round-up-boundaries/build-attempt-1.log` SHA-256
  `012296bfcfc7dc5a3f8e3b0261ca37654d84d097e256233e37f39365cf71660f`; `attempt-1.log` SHA-256
  `5d9509c6ab529c4c693948198269b05a70951eb681bb7a79782124c2bd0332c6`.
- Root build exited 0; log `local/validation/root-round59-replay/srv01-build.log`, SHA-256
  `40d4c8c5d57a58d1b3e380e966efa8c1c6a87b1bcbb7f7f81bfb16abb4efc181`. Root CTest exited 0,
  1/1 passed; log `local/validation/root-round59-replay/srv01-ctest.log`, SHA-256
  `db3f8595523db6fc5b31eb9aefa526d405f1831c9339c52335ac5a5fa830669c`.
- Test file SHA-256 `405d9ae57520f2f0fa7abb93733236220d3828d32dd95c478fd4a621d27c5a76`. Zero failed
  tries. This covers synthetic integer helpers only, not allocator integration or runtime behavior.

## Shared checks and status

- `git diff --check` exited 0.
- `python3 scripts/project_index.py` exited 0 and indexed 14,410 folders; `docs/checkpoints` has
  352 entries. Log `local/validation/root-round59-replay/project-index.log`, SHA-256
  `bff35dfce43fadd9cf3ebcbdec9bc876b225f99a61243b5a4db2bd79cda2305c`.
- Pre-push staged path audit accepted 8 paths; log
  `local/validation/root-round59-replay/audit-staged.log`, SHA-256
  `74a236596bb29bd6044e0beeb92a24137b414ad4633d080e81454b4a0c8eab69`. Full audit accepted
  1,619 paths; log `local/validation/root-round59-replay/audit-all.log`, SHA-256
  `2c4b137269d2933cda500381adc60661e15100c9d5a95eb58c9decad2038b379`. Cached and working
  diff checks exited 0. Only the three claimed test files and allowlisted docs are staged; user-owned
  `native/api` changes are unstaged.
- `trail build --check` first reported STALE for `scripts/test_psarc.py` and its method nodes after
  the edit. `trail build` then exited 0 (3,553 nodes / 4,795 edges; cache remains ignored and
  regenerable). Follow-up `trail build --check` exited 0 with the graph in sync. Logs:
  `local/validation/root-round59-replay/trail-check-before-build.log` SHA-256
  `5882c149ebcd193ac4b42a963f1b0b30cdf47d54b4c893faf0a8fa7f85b62803`, `trail-build.log` SHA-256
  `bf52de69cee997dc4a3cbe1ac8e71ceedac4f7151559f8a31aac0040430045bf`, and
  `trail-check-after-build.log` SHA-256
  `edd294377abf28693b2f71cccac3065592d98729c855c751af91b8e78c547973`. Optional deep layer is
  not built.
- Observed results are limited to the synthetic assertions above. No inference about live app
  accessibility, real archives, allocator runtime, original game execution, audio/save/load, or
  Metal gameplay follows. G0 remains PARTIAL; G1-G6 remain NOT RUN.
- Existing user-owned `native/api/CMakeLists.txt` and untracked `native/api/host/` were not edited,
  staged, or included. No dependency/tool was installed or changed; `INSTALL_LOG.md` is unchanged.
- Exact next action: scout and claim the next bounded independent fixture tasks for Round 60 before
  dispatch. Task results: APP-01 FIXTURE_VERIFIED (0 failed tries); DATA-02 FIXTURE_VERIFIED (1
  failed fixture-construction hypothesis); SRV-01 FIXTURE_VERIFIED (0 failed tries).
