# Checkpoint: SRV-01-REGION-SPLIT-OVERFLOW-FIXTURE-01 — complete

- Local date/time: 2026-10-21 12:29 CDT.
- Agent ID: `/root` (integrator), with implementation by `/root/slot23_raw_xrefs`.
- Task ID: `SRV-01-REGION-SPLIT-OVERFLOW-FIXTURE-01`.
- Status: `FIXTURE_VERIFIED`; failed tries: 0.
- Files changed for implementation: only `native/tests/test_region_split.cpp`.
  Production `native/core/region_split.cpp` is unchanged.
- Verification commands and results:
  - Worker `ctest --test-dir local/build/native -R '^region_split$'` — exit 0; 1/1.
    Log `local/validation/srv01-region-split-overflow/attempt-1.log`, SHA256
    `ef864521d624816678d14ba30c8a4993b9eb676e903dd2ba14486b67ebf7dcf3`.
  - Root `cmake --build local/build/native --target test_region_split` — exit 0; target rebuilt.
    Log `local/validation/srv01-region-split-overflow/root-build.log`, SHA256
    `8afdf348bbda2cd741eda1e7216d82fc246741da3b539ee41c7522696b30af26`.
  - Root `ctest --test-dir local/build/native -R '^region_split$'` — exit 0; 1/1.
    Log `local/validation/srv01-region-split-overflow/root-replay.log`, SHA256
    `8c06ed69c4bf3b8468c63c4baba86cc3581bb211cc6dcb6bd3c77b0b0e44ab1a`.
  - `git diff --check` — exit 0.
  - Test source SHA256: `7d773a5581e549c40c99cfaf466707245e3e196cce8aa6662459ebaf0667eac9`.
  - Unchanged implementation SHA256: `aeead9c2a91b49283e0924d1bfba5ac98bb56aee50a1cc1f320a6e6924d0021f`.
- Observation: a synthetic node starting at `UINT64_MAX - 3`, size 16, with lead 8 is rejected;
  both raw node arrays remain byte-for-byte unchanged.
- Inference limit: malformed range arithmetic fixture only; no allocator runtime or game behavior
  is established.
- Remaining risks: real service behavior and all game acceptance gates remain open.
- Next exact action: publish this fixture with the launcher-tone and decoder completions.
