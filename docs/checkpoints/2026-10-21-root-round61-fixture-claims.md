# Checkpoint: Round 61 synthetic fixture claims

- Local date/time: 2026-10-21 13:58 CDT (America/Chicago).
- Agent ID: `/root` (integrator); read-only scouts by `/root/launcher_review`,
  `/root/review_host_proto`, and `/root/slot23_raw_xrefs`.
- Claim base: `4ede1c9d92ba324ca12eee5ca3fac7e0bda20d36`, published Round 60 completion.
- Before assigning work, all proposed one-file fixtures passed their focused baseline command.
  Source and test files match the hashes below. Each owner may modify only its named test file; root
  owns shared docs and will independently replay each command. No production source is claimed for
  modification.
- Status: three tasks claimed; each has 0 failed fixture attempts. Claim commit
  `4cbd478180cb983c52b711e8a69cca2e732ea4f6` was pushed before implementation work.

## APP-01-CORE-ABI-PACKED-UINT16-MAX-ROUNDTRIP-FIXTURE-01

- Prerequisite: `APP-01-CORE-ABI-01` (PARTIAL).
- Owner: `/root/launcher_review`. Only
  `launcher/Tests/LauncherCoreTests/CoreClientTests.swift` may change.
- Proposed assertion: `CoreABIVersion(major: .max, minor: .max).packed == UInt32.max`, and
  `CoreABIVersion(packed: UInt32.max)` round-trips to that value. This covers representation
  boundaries only; it does not prove C ABI interoperability or runtime version policy.
- Production file `launcher/Sources/LauncherCore/CoreModels.swift` SHA-256
  `183d681e8566cf2d8c19b2a484f6e4163869ec3eb34c231f92ed91314b824eda`; test file SHA-256
  `e6393c9ae18e033fad672ad720ce3325603dff38f7aeccf4b2ebeb4bc6869b12`.
- Baseline command `swift test --package-path launcher --scratch-path local/build/launcher
  --filter CoreClientTests` exited 0 with 12 tests in 1 suite. Log
  `local/validation/root-round61-baseline/app01.log`, SHA-256
  `301419ada1e6fdd909551e36b54b718693be6adfa0d4cc8229718167d1a43f73`.
- Acceptance command: `swift test --package-path launcher --scratch-path local/build/launcher
  --filter CoreClientTests`.

## AUD-01-CLASSIFIER-LZMA-TRIVIAL-OUTPUT-REJECT-FIXTURE-01

- Prerequisite: `AUD-01` (ACTIVE inventory task).
- Owner: `/root/review_host_proto`. Only `scripts/test_audio_headers.py` may change.
- Proposed assertion: compress a one-byte synthetic payload with Python `lzma`, call
  `decompress_probe`, and assert `lzma_offset0` is exactly
  `{'ok': False, 'out_len': 1, 'rejected_trivial': True}`. This pins the existing short-output
  rejection on a valid synthetic LZMA stream; it does not establish asset codec or game-audio use.
- Production file `scripts/classify_audio_headers.py` SHA-256
  `624a51336859bab8aef2e7e7880b16d1282c9e883f459de05c8e3c1c4320cd77`; test file SHA-256
  `e013de229443cc49dd3fff09bbb743bf5eb61c4c14b90dcf34e03b2cd8f3f9f4`.
- Baseline command `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_audio_headers.py` exited 0
  with 23 tests. Log `local/validation/root-round61-baseline/aud01.log`, SHA-256
  `4b1466b77048ebccccd836a54b75581fa925911c0cc0b1452c131afd59bf9c23`.
- Acceptance command: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_audio_headers.py`.
- Risk: depends on the Python standard-library `lzma` module.

## SRV-01-REGION-SPLIT-LEAD-EQUAL-SIZE-FIXTURE-01

- Prerequisite: `SRV-01` (ACTIVE platform-services task).
- Owner: `/root/slot23_raw_xrefs`. Only `native/tests/test_region_split.cpp` may change.
- Proposed assertion: initialize a synthetic free node and sentinel-filled new node, snapshot both
  raw arrays, then call `split_free_at_lead(free, neu, free.size())`; assert rejection and byte-for-
  byte preservation of both nodes. This covers positive `lead == free_size`, distinct from the
  existing zero-lead and address-overflow fixtures. It makes no allocator-runtime claim.
- Production file `native/core/region_split.cpp` SHA-256
  `aeead9c2a91b49283e0924d1bfba5ac98bb56aee50a1cc1f320a6e6924d0021f`; test file SHA-256
  `7d773a5581e549c40c99cfaf466707245e3e196cce8aa6662459ebaf0667eac9`.
- Baseline build `cmake --build local/build/native --target test_region_split` exited 0. Log
  `local/validation/root-round61-baseline/srv01-build.log`, SHA-256
  `21a11a3b5ef65d6abb448e29a2eec6df2cfbe0621748eea0a75d2448d65bf0ab`.
- Baseline CTest `ctest --test-dir local/build/native -R '^region_split$'` exited 0, 1/1. Log
  `local/validation/root-round61-baseline/srv01-ctest.log`, SHA-256
  `72f9a935d1a69ef02dd9a3f592ddde5aadca6e2b8363754c03aa206827fd7d47`.
- Acceptance commands are those same build and CTest commands.

## Shared state and next action

- Exact-ID searches in `docs/TASKS.md`, `docs/EVIDENCE.md`, and `HANDOFF.md` found no prior
  registrations for these tasks. Prior fixture candidates in these files do not overlap the exact
  boundary cases claimed here. No failed prior tries were found.
- Claim-time `git status` showed only the Round 60 publication evidence update in `HANDOFF.md` and
  its completion checkpoint, plus pre-existing user-owned `native/api/CMakeLists.txt` and
  untracked `native/api/host/`. The latter two remain untouched and excluded.
- `python3 scripts/project_index.py` exited 0 and indexed 14,417 folders; `docs/FOLDERS.md` now
  lists 356 checkpoint files. Log `local/validation/root-round61-baseline/project-index.log`,
  SHA-256 `39bf43672c493802900b5ff2ada9ee28bb1d1d50c8c4546146075a4c09be1283`.
- An initial `python3 scripts/project_index.py --check` exited 1 with output
  `Stale indexes: local/indexes/folders.json`; after rerunning the generator, the same check exited
  0 with `14417 folders indexed; current`. Final check log
  `local/validation/root-round61-baseline/project-index-check.log`, SHA-256
  `60fc1d273e24906407f8d56b6e26c8bee2f6365d03210977e57235ee035ff708`. The stale-index cause is
  not established and is unrelated to fixture behavior.
- `trail build --check` exited 0; the wiring graph remains in sync at 3,555 nodes. The deep concept
  layer is not built. Log `local/validation/root-round61-baseline/trail-check.log`, SHA-256
  `baf6a6fa0154df5f35c9d48b57ad6af23e8d2bb936f9c9164f5238534e936e42`.
- `git diff --check` exited 0.
- Before publication, the staged path audit accepted 5 paths; log
  `local/validation/root-round61-baseline/audit-staged.log`, SHA-256
  `c9c6d827f3b6ca317244f2a03374bb802020cf191fa10efe7b058937af50162c`. Full audit accepted 1,623
  paths; log `local/validation/root-round61-baseline/audit-all.log`, SHA-256
  `e232b1f79f408e3c45dfd50e12db52acbbe2ffdf9bc482f96bee56aa17db36c1`. Cached and working diff
  checks passed; only five allowlisted documentation paths are staged. Existing `native/api` user
  changes remain unstaged.
- All three claims were completed and root-replayed; see
  `docs/checkpoints/2026-10-21-root-round61-fixtures-complete.md` for final test results, evidence,
  publication checks, and the next exact action.
