# Claim checkpoint: APP-01 Metal readiness and SRV-01 exact-fit write boundary

- Local date/time: 2026-10-22 05:57 CDT (America/Chicago). Agent / integrator: `/root` (Codex
  desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`).
- Task IDs: `APP-01-METAL-READINESS-GATE-FIXTURE-01` (parent `APP-01`) and
  `SRV-01-EXACT-FIT-FINALIZE-WRITE-BOUNDARY-FIXTURE-01` (parent `SRV-01`). Both are `ACTIVE`, with
  zero implementation attempts and zero failed tries.
- Files changed at claim: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, generated
  `docs/FOLDERS.md`, the next-action lines in the SAV-01 and SRV-01 completion checkpoints, and this
  claim checkpoint. No implementation source/test file has been edited yet.
- Files claimed for independent implementation: one existing test method in
  `launcher/Tests/LauncherCoreTests/LauncherModelTests.swift`, and one synthetic case in
  `native/tests/test_exact_fit_finalize.cpp`. Shared documentation and publication remain owned by
  `/root`; no production source, game input, reference clone, or user API file is in scope.
- `APP-01-METAL-READINESS-GATE-FIXTURE-01`: current test source SHA256
  `c06fc9379fcebba7d04303e2ae76591ae1894f6f6daf726500eca4f326a0d363`; unchanged production
  `LauncherModel.swift` and `ReadinessReport.swift` hashes are
  `ae6ce2e1b33a83c2baae76efd63a3f2beacc0a62202c6a62ab9bb96644be6d51` and
  `170b03a7c24aadb3c799f364998ba81948438404e88eef3b07200ec210982e98`. Graft and Trail point to
  `hostChecksStillGatePlay`: it injects missing Metal and currently checks idle state, status copy,
  and native-core readiness, but omits `!canStart` and the blocked start-call count. The adjacent
  audio case already pins those latter effects. Hypothesis: with Metal missing and other checks
  ready, `canStart` is false, the Metal row carries the injected missing reason, and `startCore()`
  leaves the model idle without invoking scripted `start()`.
- Exact APP-01 command, from repository root after implementation:
  `swift test --package-path launcher --scratch-path local/build/native-root-app01-metal-readiness-20261022 --filter LauncherModelValidationTests`.
  Expected result: the focused validation suite passes; the test remains synthetic and does not
  exercise a physical Metal device or renderer.
- `SRV-01-EXACT-FIT-FINALIZE-WRITE-BOUNDARY-FIXTURE-01`: baseline test file SHA256
  `acdc2d8f9621799db22cd9b2bcc5c7564b16b5141dad63a6a2ed6083e7ef8c69`; unchanged helper/header
  SHA256 `a49d202388d520c945b744ca6bd3a2696b12165b57c51b0477f9008b5ef5d0e4` /
  `16a32b55aa533a3f682ed0ea7a4f248128987503b3c6e2f9d86f06d0de8c4a5a`; `native/CMakeLists.txt`
  already registers `test_exact_fit_finalize` and CTest `exact_fit_finalize`. Existing assertions
  check selected fields and `start`/`capacity`, but not every byte. Hypothesis: after seeding a
  `RegionNode` with a poison pattern and running `exact_fit_finalize`, the documented writes at
  pointer, sentinel, size, flag A, and flag B offsets occur while every other byte remains unchanged.
- Exact SRV-01 commands, from repository root:
  `cmake -S native -B local/build/native-root-srv01-exact-fit-write-boundary-20261022 -G Ninja -DCMAKE_BUILD_TYPE=Debug`;
  `cmake --build local/build/native-root-srv01-exact-fit-write-boundary-20261022 --target test_exact_fit_finalize`;
  `ctest --test-dir local/build/native-root-srv01-exact-fit-write-boundary-20261022 --output-on-failure -R '^exact_fit_finalize$'`.
  Expected result: configure/build succeed and focused CTest passes 1/1. This synthetic helper byte
  boundary does not establish allocator reachability or game behavior.
- Publication baseline: preceding SAV-01 completion commit
  `2b1097220c0dfd7a1312e171c7cbde49f54a7603` is pushed and fetch-verified at 0/0 divergence; E349
  records its staged/full path audits and post-push status.
- Baseline evidence: source hash command output is pinned in
  `local/validation/20261022-root-parallel-claims/baseline-source-hashes.log` (SHA256
  `029a57ae1e4ebce96b68b21690d53bb512e346431f1da391014e9c52ce30537c`). Graft/Trail result
  excerpts are in `graft-app-metal-query.txt`, `trail-app-metal-query.txt`,
  `graft-srv-exact-fit-query.txt`, and `trail-srv-exact-fit-query.txt`, SHA256
  `7dbe2eff5459e6b1474776c66d4d0e7311e8a768fb74b2da6295ff4e587795d7`,
  `094a6578ac1e51cbcbc703aafc7adda1b4936898d326abe8d6bee7cf08a508dc`,
  `28f9e06ba9e2893851d439d1ac0b64b9c5d40046cd09a38d801514976ee13229`, and
  `992a1d74de9be14b966dc512efe17a6c02602f35508bf9868b2eeaa1a4a95895`. Candidate IDs were absent
  before this checkpoint was created, confirmed by:
  `rg -n "APP-01-METAL-READINESS-GATE-FIXTURE-01|SRV-01-EXACT-FIT-FINALIZE-WRITE-BOUNDARY-FIXTURE-01" HANDOFF.md docs/TASKS.md docs/EVIDENCE.md docs/checkpoints` (exit 1, no matches).
- Project-index first check exited 1 with `Stale indexes: local/indexes/folders.json`; after
  precreating the remaining logs, regeneration and check both exit 0 at 47,204 folders.
  Initial/final check and generation log SHA256
  `31dad00ffd5208458ee89a329fea70b0fa13494e525ddd8a7e837c7614a2266b` /
  `52c954047cb92ac6dd75251f806596cae559be5f3cea5f8738cbdfb0d0813fb7` /
  `d967be38f2c1ae0c2845c23831dc41f0bbbdd79d120d48105db93bc3f1999b89`. Exact commands:
  `python3 scripts/project_index.py`, `python3 scripts/project_index.py --check`,
  `trail build --check`, and `git diff --check`; final generation/check and the latter two exit 0.
  Trail wiring graph is current at 3,618 nodes / 1,240 files, with meaning tier unbuilt. Their log SHA256 values are
  `f7231c65a9ccdc9f1ec3de943e3fe9b3a2be977fdf687c86e771d1c9c416a2dc` and
  `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`.
- Observation versus inference: source and ledger show both proposed test gaps; the candidate
  fixtures are independent and have disjoint test files. No implementation result is claimed.
- Remaining risks: no APP-01 physical Metal device/renderer and no SRV-01 allocator/game runtime
  behavior are covered. Native game acceptance remains G0 PARTIAL; G1–G6 NOT RUN.
- Next exact action: dispatch each bounded test edit to a worker in its own file; receive test logs
  and diff; integrator records both results and reruns targeted verification before completion.
