# Checkpoint: Round 61 synthetic fixtures verified

- Local date/time: 2026-10-21 14:06 CDT (America/Chicago).
- Agent ID: `/root` (integrator); workers `/root/launcher_review`, `/root/review_host_proto`, and
  `/root/slot23_raw_xrefs`.
- Claims were published in `4cbd478180cb983c52b711e8a69cca2e732ea4f6`; assignment checkpoint
  `0d4aa90` was pushed before worker edits. All tasks below have 0 failed fixture attempts.
- Status: three synthetic fixtures passed worker and independent root replay; production sources
  remain unchanged.

## APP-01-CORE-ABI-PACKED-UINT16-MAX-ROUNDTRIP-FIXTURE-01

- Changed only `launcher/Tests/LauncherCoreTests/CoreClientTests.swift`; SHA-256
  `38b3df894e0865af53cb5af8727d9c65f8445185228141459ea7e70068d8b84e`. Production
  `launcher/Sources/LauncherCore/CoreModels.swift` remains unchanged; SHA-256
  `183d681e8566cf2d8c19b2a484f6e4163869ec3eb34c231f92ed91314b824eda`.
- Worker and root ran
  `swift test --package-path launcher --scratch-path local/build/launcher --filter CoreClientTests`;
  both exited 0 with 22 tests across 2 suites (13 `CoreClientTests`, 9 `NativeCoreClientTests`).
- Worker log `local/validation/app01-core-abi-packed-uint16-max-roundtrip/attempt-1.log`, SHA-256
  `504582ad79e46e09cc8f271f29a81e4549de843c300b024a2ea4b7af3dc17a0f`; root log
  `local/validation/root-round61-replay/app01.log`, SHA-256
  `629a4662ff02bacaae88dccc39f1ebd80943d5301a906097416d6176365dc3dd`.
- Observation: `CoreABIVersion(major: .max, minor: .max)` packs to `UInt32.max`, and decoding
  `UInt32.max` returns the same value. This is synthetic Swift value-mapping coverage, not C ABI
  interoperability or native game behavior.

## AUD-01-CLASSIFIER-LZMA-TRIVIAL-OUTPUT-REJECT-FIXTURE-01

- Changed only `scripts/test_audio_headers.py`; SHA-256
  `1cf3b2c86ebcc8bbacc98c001bfe63f9310cfd2ff6d179b26c5dfe63eb524beb`. Production
  `scripts/classify_audio_headers.py` remains unchanged; SHA-256
  `624a51336859bab8aef2e7e7880b16d1282c9e883f459de05c8e3c1c4320cd77`.
- Worker and root ran `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_audio_headers.py`; both
  exited 0 with 24 tests.
- Worker log `local/validation/aud01-classifier-lzma-trivial-output-reject/attempt-1.log`, SHA-256
  `aae7e3727dd8d7de7ec76244d42ca239e07d6e37378489a36b419a4533d5d222`; root log
  `local/validation/root-round61-replay/aud01.log`, SHA-256
  `5ff0e141575529018c47d4110194351a8373646850427f96c761e6ef3f435544`.
- Observation: compressing one byte and probing that payload returns `lzma_offset0` with
  `ok: false`, `out_len: 1`, and `rejected_trivial: true`. This is synthetic standard-library
  compression/decompression only; no real asset classification or game audio is established.

## SRV-01-REGION-SPLIT-LEAD-EQUAL-SIZE-FIXTURE-01

- Changed only `native/tests/test_region_split.cpp`; SHA-256
  `929f85ae00e44b9ed5bf8d50de6aed6fed1ab25765f4da61fbf92210ca5b1141`. Production
  `native/core/region_split.cpp` remains unchanged; SHA-256
  `aeead9c2a91b49283e0924d1bfba5ac98bb56aee50a1cc1f320a6e6924d0021f`.
- Worker and root ran `cmake --build local/build/native --target test_region_split`; both exited 0.
  Worker and root then ran `ctest --test-dir local/build/native -R '^region_split$'`; both exited 0
  with 1/1 passed.
- Worker build log `local/validation/srv01-region-split-lead-equal-size/build-attempt-1.log`,
  SHA-256 `8afdf348bbda2cd741eda1e7216d82fc246741da3b539ee41c7522696b30af26`; worker CTest log
  `local/validation/srv01-region-split-lead-equal-size/attempt-1.log`, SHA-256
  `05b815fc5553375a61664646e4c713c15660ce2bfa616130dd60f40a86867a91`. Root build log
  `local/validation/root-round61-replay/srv01-build.log`, SHA-256
  `21a11a3b5ef65d6abb448e29a2eec6df2cfbe0621748eea0a75d2448d65bf0ab`; root CTest log
  `local/validation/root-round61-replay/srv01-ctest.log`, SHA-256
  `72f9a935d1a69ef02dd9a3f592ddde5aadca6e2b8363754c03aa206827fd7d47`.
- Observation: `lead == free.size()` returns false and both the initialized free node and sentinel
  new node remain byte-for-byte unchanged. Synthetic split-helper boundary only; no allocator
  integration, runtime, or game behavior is established.

## Shared checks, boundaries, and next action

- Root ran `git diff --check -- launcher/Tests/LauncherCoreTests/CoreClientTests.swift
  scripts/test_audio_headers.py native/tests/test_region_split.cpp`; exit 0.
- `trail build` exited 0 at 3,557 nodes / 4,804 edges; `trail build --check` exited 0. Log hashes
  are `74dea30c80a206fd174b2d0161ea3df3300137630a5b20013dd731addb43c0da` and
  `5dd1786553d23e50de436a9292a17ca6f8a6749d89368567d3d68306242c8859` for
  `local/validation/root-round61-replay/trail-build.log` and `trail-check.log`. The optional meaning
  layer remains unbuilt; this records wiring freshness only.
- `docs/TASKS.md` marks all three tasks `FIXTURE_VERIFIED`; `docs/EVIDENCE.md` contains E181–E183.
  No dependency or install changes were made. G0 remains PARTIAL; G1–G6 remain NOT RUN.
- `python3 scripts/project_index.py` exited 0 and indexed 14,421 folders; `docs/FOLDERS.md` records
  358 checkpoint files. Final log `local/validation/root-round61-replay/project-index-final.log`,
  SHA-256 `687196a3f1d49c767054f0fa388864e7060318841225d367aeab220d441be25d`;
  `python3 scripts/project_index.py --check` exited 0 with `14421 folders indexed; current`, log
  `local/validation/root-round61-replay/project-index-final-check.log`, SHA-256
  `599908c0a90802822c2e787976337627ed8f409599988a119af170a3d09b489e`.
- The first two post-generation `--check` runs reported `local/indexes/folders.json` stale. A
  read-only recomputation showed the inventory JSON differed only because each newly created check
  log added one file under indexed `local/validation/root-round61-replay`. Precreating the final log
  paths before generation made the final generator/check pair stable; the two earlier outcomes are
  preserved in `project-index-check.log` and `project-index-check-retry.log` (both SHA-256
  `31dad00ffd5208458ee89a329fea70b0fa13494e525ddd8a7e837c7614a2266b`).
- Existing user-owned `native/api/CMakeLists.txt` and untracked `native/api/host/` are untouched and
  excluded from publication.
- Staged path audit `python3 -I scripts/audit_publish_paths.py` accepted 10 paths; log
  `local/validation/root-round61-replay/audit-staged.log`, SHA-256
  `87939384fb7cf0d7fc346be028ac5b390f285dfd79b3969dd411719209d6628a`. Full audit
  `python3 -I scripts/audit_publish_paths.py --all` accepted 1,625 paths; log
  `local/validation/root-round61-replay/audit-all.log`, SHA-256
  `55d95c999812c05fa3081484f675761e19a96b012cf8416e99bb4a53e25c5ee3`. Cached and working
  `git diff --check` both exited 0. The staged list is exactly the three tests and seven allowlisted
  docs; user-owned `native/api` changes are unstaged.
- Completion commit `741174232d44236e7c674f92c106992d6613e0e9` (`test: verify round 61 synthetic
  fixtures`) was pushed to `origin/main`; the push updated `main` from `0d4aa90` to `7411742`.
  `git rev-parse HEAD origin/main` reports both refs at the full commit above. Push log
  `local/validation/root-round61-replay/push.log`, SHA-256
  `7b05194e02460c351cc9e30bc13b1e954e3d98c4faa0916a28834b697c347f4a`; ref log
  `local/validation/root-round61-replay/publication-state.log`, SHA-256
  `2c372581f11d407caeae59473f195567275650f9b06e9a2b94451c889eac3d2b`.
- Final working tree has only the pre-existing user-owned `native/api/CMakeLists.txt` modification
  and untracked `native/api/host/`; neither path was staged or published.
- Exact next action: use Graft to scout a new set of independent Round 62 tasks, verify candidates
  and baselines against live files, then publish bounded claims before implementation edits.
