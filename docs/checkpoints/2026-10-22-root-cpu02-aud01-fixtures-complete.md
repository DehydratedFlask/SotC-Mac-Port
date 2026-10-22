# Checkpoint: CPU-02 maximum Tagged31 retag and AUD-01 summary-digest fixtures

- Local date/time: 2026-10-22 00:10 CDT (America/Chicago)
- Integrator: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`)
- Tasks: `CPU-02-TAGGED31-RETAG-MAX-OFFSET-FIXTURE-01` and
  `AUD-01-XFF-ARCHIVE-SUMMARY-SHA256-MISMATCH-FIXTURE-01`.
- Status: both `FIXTURE_VERIFIED`; one successful implementation attempt and 0 failed tries each.
- Changed source/docs: `native/tests/test_addr.cpp`, `scripts/test_audio_bank_header.py`,
  `docs/ASSETS.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `HANDOFF.md`, this checkpoint, and the
  generated `docs/FOLDERS.md`. Production implementations are unchanged.
- Ownership: `/root/next_bounded_scout` implemented only `native/tests/test_addr.cpp`; root
  implemented only `scripts/test_audio_bank_header.py` and corrected the ASSETS count. Root
  integrated shared docs.

## CPU-02 fixture

- Baseline source SHA-256: `native/tests/test_addr.cpp`
  `793bcfa273a218f055a0d6c22637f2590e7a52252175dc46060ab5b7522dffb9`.
- Final source SHA-256: `0e8a0f58ecfa605688c5be66c95b0c17a5528f512a906eeebc2622be4f5ff908`.
- Unchanged production hashes: `native/core/addr.cpp`
  `aca5e57700c4e6247cd4f15135fb0e0b686d1c62398486650a4963a5daa35de8` and
  `native/core/addr.hpp` `3a0f10f433f57b80b7baad26b2be947c832de315efd101a16c81c4fc175463a7`.
- Exact check: `cmake --build local/build/native --target test_addr`, then
  `ctest --test-dir local/build/native -R '^addr$' --output-on-failure`.
  Baseline, worker and root target builds all exit 0; baseline, worker and root focused CTests all
  exit 0 and pass 1/1. Logs are in `local/validation/20261022-root-fixture-baselines/`:
  `cpu02-test-addr-build.log`, `cpu02-addr-ctest.log`,
  `cpu02-test-addr-build-worker.log`, `cpu02-addr-ctest-worker.log`,
  `cpu02-test-addr-root.log`, and `cpu02-addr-ctest-root.log`.
- The test constructs a full 2 GiB synthetic `HeapWindow`, retags untagged offset `0x7fffffff` to
  `0xffffffff`, and decodes it to `base + 0x7fffffff == limit - 1`.

## AUD-01 fixture

- Baseline source SHA-256: `scripts/test_audio_bank_header.py`
  `90f18dd3a98578e822a3674ef3d3d71fc237b89b46eb3f350dbb3460e7cd4833`.
- Final test SHA-256: `4162d392a35d6154a02de207348483bfa233f5339645785b5f323b569c4ae7f5`.
- Unchanged parser SHA-256: `scripts/audio_bank_header.py`
  `bdad7d192b0ecdb929daf5147a139e7545fd5c546f01ed4ffb4005f2cea9f9c0`.
- Exact check: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_audio_bank_header.py`.
  Baseline exits 0 with 20 tests; root replay exits 0 with 21 tests. Logs:
  `local/validation/20261022-root-fixture-baselines/aud01-audio-bank-header.log` and
  `local/validation/20261022-root-fixture-baselines/aud01-audio-bank-header-root.log`.
- The new synthetic fixture leaves the summary byte count correct, replaces only its digest, checks
  the exact `archive summary SHA-256 ... != live ...` error, and asserts that `Psarc` was not
  constructed. `docs/ASSETS.md` now reports 21 methods (was 18).

## Indexes, evidence, boundaries

- Trail initially returned `STALE` while these source files were being edited, naming
  `native/tests/test_addr.cpp` and `scripts/test_audio_bank_header.py`. After the worker and root
  edits were complete, `trail build` exited 0 and `trail build --check` exited 0: 3,609 nodes / 1,240
  files in sync. `trail grep "max_retag_offset"` resolves the new native assertions, and
  `trail grep "test_inspect_members_rejects_stale_archive_summary_sha256_before_open"` resolves the
  new Python method. Final check log:
  `local/validation/20261022-root-fixture-baselines/trail-build-check-root.log`.
- One `project_index.py --check` first found `local/indexes/folders.json` stale because the check's
  own log was created after generation and changed the private directory's file count. Pre-creating
  both project-index log paths before regeneration resolved this ordering issue. The final generator
  and check both exit 0 at 24,078 folders; logs are
  `local/validation/20261022-root-fixture-baselines/project-index-generate.log` and
  `local/validation/20261022-root-fixture-baselines/project-index-check.log`.
- Evidence rows E253 and E254 record source hashes, test logs, observed behavior and limits. These
  fixtures concern synthetic helper and sidecar validation only. They do not validate game pointers,
  real archive inputs, codec behavior, playback, a game runtime, or any G0–G6 gate.
- `git diff --cached --check` exits 0 (empty-output log SHA-256
  `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`). The staged publication
  audit accepts 8 allowlisted paths and the full audit accepts 1,690 paths. Logs:
  `local/validation/20261022-root-fixture-baselines/completion-staged-audit.log` SHA-256
  `601b710d16d64caffe5c9c329c8d46b36727fc286a831faea4b9ed5509b6595f` and
  `local/validation/20261022-root-fixture-baselines/completion-all-audit.log` SHA-256
  `f04004c590e010a5778fa3106b5b30c75a6b6e7a861fbcba136f957cb6bbdbbc`.
- Pre-push `git fetch origin` exits 0; `git rev-list --left-right --count origin/main...HEAD` reports
  `0 0`. Logs are `completion-pre-fetch.log` (empty output) and `completion-pre-divergence.log`
  SHA-256 `4b14c7b5549e560a68e9c4daaae3fdb8a28fe32d0247050325e7c5a3b2b3271c` in the same private
  validation directory.
- Remaining risks: no production changes were made. The task facts are local fixtures only; no
  completion claim is made for native boot, audio, saving, Metal gameplay or performance.
- Exact next action: stage only the allowlisted source, docs and checkpoint paths; run staged and
  full publication audits, commit and push, then fetch and verify 0/0 divergence. Preserve the
  pre-existing user-owned `native/api/CMakeLists.txt` and `native/api/host/` changes unstaged.
