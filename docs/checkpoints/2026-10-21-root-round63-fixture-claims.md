# Checkpoint: Round 63 fixture claims

- Local date/time: 2026-10-21 14:23 CDT (America/Chicago).
- Agent ID: `/root` (integrator); read-only scouts `/root/launcher_review`,
  `/root/review_host_proto`, and `/root/slot23_raw_xrefs`.
- Base commit: `136af1da4d9b03c33b6e4ded81a3f38acd745f0a`; `origin/main` matched before claims.
- Three synthetic test-only tasks have unique IDs and passing pre-edit baselines. Failed fixture
  attempts: 0 for all tasks. No implementation prompts have been sent.

## Candidate discovery and root verification

- Root ran `trail ask "Find three small, independent, not-yet-covered synthetic fixture gaps in
  existing SoC Mac Native tests: one launcher Swift model/readiness boundary, one PSARC/audio parser
  error or edge boundary, and one native region/allocator boundary. Return exact file and
  symbol/method spans, likely assertion, and why existing tests do not cover it. Do not propose
  production changes." --source`; it exited 0 and reported approximately 14,204 tokens saved.
- The launcher scout found that `CoreClientMapping.launcherState(for:failureMessage:)` uses the
  explicit override for `.failed`, while `everyCoreStateHasADefinedLauncherState` checks only the
  case and `failureTextCoversTheErrorsTheLauncherCanSee` checks only the fallback. Root verified the
  live mapping/test and found no matching task or evidence ID.
- The parser scout found that separate fixtures cover compressed and stored blocks but not both in
  sequence within one member. Root verified `Psarc.read`'s `csize == 0` stored branch, the zlib
  branch, and the existing fixture builder's per-block encoding; the selected `b'A' * 16` block
  compresses and `bytes(range(16))` uses stored marker 0 at block size 16. Root found no matching
  task or evidence ID.
- The native scout found that `try_carve_fit` accepts when `free_size - lead == req_size`, while
  existing cases test larger capacity, insufficient capacity, and exact whole-node alignment. Root
  verified the live helper/tests and found no matching task or evidence ID.
- All candidate files are distinct. Existing user-owned `native/api/CMakeLists.txt` and untracked
  `native/api/host/` remain outside task scope.

## APP-01-CORECLIENT-LAUNCHER-STATE-FAILURE-MESSAGE-OVERRIDE-FIXTURE-01

- Owner `/root/launcher_review`; may edit only
  `launcher/Tests/LauncherCoreTests/CoreClientTests.swift`.
- Add an assertion using the local `status` helper in `everyCoreStateHasADefinedLauncherState`:
  `#expect(CoreClientMapping.launcherState(for: status(.failed, error: .engineNotImplemented), failureMessage: "synthetic status detail") == .failed("synthetic status detail"))`.
- Acceptance command: `swift test --package-path launcher --scratch-path local/build/launcher --filter CoreClientTests`.
- Worker baseline exited 0: 22 tests across `CoreClientTests` (13) and `NativeCoreClientTests` (9).
  Log `local/validation/round63-coreclient-failure-override-scout/baseline.log`, SHA-256
  `5ff2bce9f95c0e532dd119d7affbd640682ee0d4311fc5147425a5b9c3421a22`.
- Root replay exited 0: the same 22 tests passed. Log
  `local/validation/root-round63-scout/root-app-baseline.log`, SHA-256
  `49413e616e5b3dde811d33af119bbefc2a2f81906713144e36992e4459bf1cc2`.
- Test SHA-256 `38b3df894e0865af53cb5af8727d9c65f8445185228141459ea7e70068d8b84e`; unchanged
  production `launcher/Sources/LauncherCore/CoreClient.swift` SHA-256
  `9f2b214486f8c3ed81ead6a237f86e494c52ccd89a33c15a9fed60de533b73f5`.
- Limit: pure Swift status-message selection only; no running core, launcher UI, or game behavior.

## DATA-02-PSARC-MIXED-COMPRESSED-STORED-MEMBER-FIXTURE-01

- Owner `/root/review_host_proto`; may edit only `scripts/test_psarc.py`.
- At block size 16, build a synthetic member with payload `b'A' * 16 + bytes(range(16))`; assert
  its first member block table entry is nonzero (compressed), the next is zero (stored), and
  `archive.read(1)` returns the exact 32-byte payload.
- Acceptance command: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_psarc.py`.
- Scout baseline exited 0: 33 tests passed. Log
  `local/validation/root-round63-scout/psarc-baseline.log`, SHA-256
  `435857b0702c990ceacd6438fe10a4e08fa6450046f64bf1e056a465d3f8987f`.
- Root replay exited 0: 33 tests passed. Log
  `local/validation/root-round63-scout/root-psarc-baseline.log`, SHA-256
  `364619f073e382e37208a01f4c1ce42e1275432e9c5b5af387ca168370f311b1`.
- Test SHA-256 `b8cfdd95dbf7d61fe6a67a0834da6dd4ef797c501212b1abd2cbd45bca2d8313`; unchanged
  production `scripts/psarc.py` SHA-256
  `39f2c748db013637aba8144205dc60b7e5b1eb49ee0f641960783a1e2c140485`.
- Limit: synthetic PSARC block encoding and reads only; no live archive or game behavior claim.

## SRV-01-FREE-LIST-CARVE-EXACT-REMAINDER-FIT-FIXTURE-01

- Owner `/root/slot23_raw_xrefs`; may edit only `native/tests/test_free_list_carve.cpp`.
- Add the synthetic case where `free_start=0x20001000`, `free_size=0x1f000`, `req_size=0x10000`,
  and `alignment=0x10000`; assert fit, `!exact_node`, `lead == 0xf000`,
  `allocated_start == 0x20010000`, and `allocated_size == 0x10000`.
- Acceptance commands: `cmake --build local/build/native --target test_free_list_carve`, then
  `ctest --test-dir local/build/native -R '^free_list_carve$'`.
- Worker build/CTest both exited 0; CTest passed 1/1. Logs
  `local/validation/round63-srv01-free-list-carve-equal-capacity-scout/baseline-build.log` and
  `baseline-ctest.log`, SHA-256
  `864e6f0bdf507d73c1178c9078acbe53c24725d2383621db31416c2de9c14544` and
  `db4436ff416a8c4da13cc1a559b1fbdf77089d0d55886b15318b24789ea9df72`.
- Root build/CTest both exited 0; CTest passed 1/1. Logs
  `local/validation/root-round63-scout/root-srv-build-baseline.log` and
  `root-srv-ctest-baseline.log`, SHA-256
  `864e6f0bdf507d73c1178c9078acbe53c24725d2383621db31416c2de9c14544` and
  `3e68d0037317133fde230ae08fb257e72e357a0ff6f445f77dfbc03a148bf9f9`.
- Test SHA-256 `5780461241ed1b0ba2cb1b8338052729f72149f164d97647111185ad58c93e6f`; unchanged
  production `native/core/free_list_carve.cpp` SHA-256
  `a5d0d660cb49b73499c27b42724afc6d57a8735a01ce07017d11599b3e8b1630`.
- Limit: synthetic carve arithmetic only; no allocator integration, runtime, or game behavior.

## Shared boundary and next action

- Root found all three task IDs absent from `docs/TASKS.md` and `docs/EVIDENCE.md`; all production
  implementations and test files match the hashes recorded above at claim time.
- At claim time, no test edits had been made. Worker edits and first-run outcomes after the published
  claim are recorded in the Round 63 dispatch checkpoint. No dependency, install, or game-file facts
  changed. Game gates remain G0 PARTIAL and G1–G6 NOT RUN.
- `python3 scripts/project_index.py` exited 0 at 14,430 folders; `docs/FOLDERS.md` records 362
  checkpoint files. `python3 scripts/project_index.py --check` exited 0 (`current`). Logs
  `local/validation/root-round63-claims/project-index.log` and `project-index-check.log`, SHA-256
  `27bfe59536b592f44ab1fb75c2e762500a8fe6d525063031e6aba1cf9aff66d7` and
  `3464168f3254d3d661428dd06754ac05202df81abbf5cafa87e7fbc10a7324db`.
- Staged path audit accepted 4 documentation/index paths; `--all` accepted 1,629. Logs
  `local/validation/root-round63-claims/audit-staged.log` and `audit-all.log`, SHA-256
  `538d687a0f16bdc2f5201d438a2f453cd3eb923fb5c6479dae0c79546672d279` and
  `3f99d0a41a7108b79a7492ee671d3efefba4fd98436fc0e3551192a020984885`. Staged and working
  `git diff --check` passed. User-owned `native/api` changes remain unstaged.
- Claim commit `3a79b5318beb437011384ca6c250c5f56b12ead9` was pushed to `origin/main` before
  prompts; push exited 0 (`136af1d..3a79b53 main -> main`). Push log
  `local/validation/root-round63-claims/push-claims.log`, SHA-256
  `9ac94a0929fcf694f749680b1a93f761736a99497aaabeb959b59e04fdd4417d`; `git rev-parse HEAD
  origin/main` matched at the claim commit, recorded in `publication-state.log`, SHA-256
  `009237c394683aedd262a2d22db3a1f964cd5af54f9ace71df27380705292b91`.
- At 14:24 CDT, one-file implementation prompts were sent to the three owners. The worker first-run
  results and unreviewed diffs are recorded in the Round 63 dispatch checkpoint. Task status remains
  `ACTIVE` until root replay, with 0 failed attempts.
- Exact claim-time next action was to publish the claims and send prompts; both are recorded above.
