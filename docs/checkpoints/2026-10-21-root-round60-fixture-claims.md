# Checkpoint: Round 60 synthetic fixture claims

- Local date/time: 2026-10-21 13:46 CDT (America/Chicago).
- Agent ID: `/root` (integrator), with scout owners `/root/launcher_review`, `/root/review_host_proto`,
  and `/root/slot23_raw_xrefs`.
- Round 59's three verified fixture tests were published in `7f9ce03`; its recorded DATA-02
  fixture-construction failure is in `0653bce`. Both commits are on `origin/main` before these
  claims.
- Claim task IDs:
  - `APP-01-ASSET-VALIDATOR-SYMLINKED-DIRECTORY-REJECTION-FIXTURE-01`
  - `AUD-01-CLASSIFIER-LZMA-OFFSET4-FIXTURE-01`
  - `SRV-01-REGION-LIST-UNCOUNTED-UNLINK-FIXTURE-01`
- At claim, all three target test files were clean and task IDs/evidence entries were absent.
  Production files remain unchanged. Each worker owns only the listed test file; root owns shared
  documentation and independent replay. User-owned `native/api/CMakeLists.txt` and
  `native/api/host/` remain excluded.

## APP-01 symlinked directory

- Owned file: `launcher/Tests/LauncherCoreTests/AssetValidationReportTests.swift`.
- Branch: create a required archive path that is a symlink to a directory; `AssetValidator.report`
  should classify it as `"pak0.psarc not found"` and list `pak0.psarc` in `missingArchives`.
- Existing test covers a real directory named like an archive, but no symlink case. The source guard
  checks `fileExists(atPath:isDirectory:)` and rejects `isDirectory == true`.
- Claim-time test SHA-256 `4b87c4181ebbdb07b01daba131a8a85d6bb0b1f83188e776654751b63a8739a4`;
  source SHA-256 `234f62920654226fd2d6239d7f8530802bc52fc9a26d38a698dab8838bbd203e`.
- Command: `swift test --package-path launcher --scratch-path local/build/launcher --filter AssetValidationReportTests`.
- Owner: `/root/launcher_review`. Zero failed tries. Synthetic filesystem validation only; no
  app/UI/runtime, VoiceOver, or game claim.

## AUD-01 LZMA at offset 4

- Owned file: `scripts/test_audio_headers.py`.
- Branch: prefix `lzma.compress(b"A" * 64)` with four bytes; assert `decompress_probe` reports
  `lzma_offset4.ok` and `out_len == 64`.
- Existing tests cover zlib and raw DEFLATE at offsets 0 and 2, but no LZMA offset. The classifier's
  offset 4 branch is already implemented and unchanged.
- Claim-time test SHA-256 `6ebf195b01cb8f01938cc847de9381d50c87f7042e4334a3133aba019a96d0e1`;
  source SHA-256 `624a51336859bab8aef2e7e7880b16d1282c9e883f459de05c8e3c1c4320cd77`.
- Command: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_audio_headers.py`.
- Owner: `/root/review_host_proto`. Zero failed tries. Synthetic standard-library decompression
  only; no game asset or codec claim.

## SRV-01 unlink without a counter

- Owned file: `native/tests/test_region_list.cpp`.
- Branch: build a synthetic A-B-C chain, call `list_unlink(b)` with the default null counter, then
  assert A links to C, C links back to A, and B's next pointer self-loops. Existing unlink fixtures
  pass a counter pointer.
- Claim-time test SHA-256 `2c1ba1bce804ebb38adc5f6e221eb9febaf98e62cd2ea34be15ded8f46e3db49`;
  source SHA-256 `a042ec5af910f31579c43b1897d6d35e5e535c25da0b031f32e7973f1c8e3bda`.
- Commands: `cmake --build local/build/native --target test_region_list` and
  `ctest --test-dir local/build/native -R '^region_list$'`.
- Owner: `/root/slot23_raw_xrefs`. Zero failed tries. Synthetic intrusive-list behavior only; no
  allocator integration or runtime claim.

- Task status: all three ACTIVE; verification NOT RUN; failed-attempt count 0 for each.
- `python3 scripts/project_index.py` exited 0 and indexed 14,411 folders; `docs/checkpoints` now
  lists 353 entries. Log `local/validation/root-round60-claims/project-index.log`, SHA-256
  `aa66db4a28246681bb51bd2b3d670bd0b926ece1103d0c04c13fc9c721637a0f`.
- `trail build --check` exited 0; wiring graph is in sync at 3,553 nodes / 4,795 edges. Optional
  deep layer is not built. Log `local/validation/root-round60-claims/trail-check.log`, SHA-256
  `edd294377abf28693b2f71cccac3065592d98729c855c751af91b8e78c547973`.
- Pre-push staged path audit accepted 4 paths; log `local/validation/root-round60-claims/audit-staged.log`,
  SHA-256 `66dcb0ed79d51d0c4723f832fe70826bba65da6d409fdafb485c83f0d05780af`. Full path audit
  accepted 1,620 paths; log `local/validation/root-round60-claims/audit-all.log`, SHA-256
  `c7b0fce976f748eed85efacb3c12fe606c524b4aabcd05dae7afc7a0d1de1a89`. Cached and working diff
  checks exited 0. Only the four claim/documentation paths are staged; user `native/api` work is
  unstaged.
- Exact next action: commit and push the allowlisted claim paths, then dispatch the three workers.
