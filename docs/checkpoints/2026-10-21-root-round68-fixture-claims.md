# Round 68 fixture claims

**Checkpoint:** 2026-10-21 15:43 CDT (America/Chicago)
**Integrator:** `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`)
**Task status:** Three bounded test-only claims; 0 failed tries and 0 implementation attempts.
**Starting revision:** `516e9f4c6648bd7e273521cf22742e1f8fdc9920` (`HEAD == origin/main`).
**Files changed:** `HANDOFF.md`, `docs/TASKS.md`, `docs/FOLDERS.md`, this checkpoint.

## Claims and baselines

| Task | Owned file | Claimed assertion | Baseline result |
|---|---|---|---|
| `APP-01-LIFECYCLE-UNEXPECTED-EXIT-READINESS-FALSE-FIXTURE-01` | `launcher/Tests/LauncherCoreTests/LauncherLifecycleTests.swift` | Extend `unexpectedExitPreservesFailureStatus()` to assert that `.unexpectedExit(status: 17)` remains `.failed("Game core exited unexpectedly (status 17)")` when `readinessAllowsStart` is false. | `swift test --package-path launcher --scratch-path local/build/launcher --filter LauncherLifecycleTests` — exit 0; 11 tests across 2 suites. Log: `local/validation/root-round68-scout/app-baseline.log`, SHA-256 `d612ef7ac8afad9a532621581474e6e663254080b5b0ad7627cb4ac340d8600a`. |
| `RE-02-DYNLINK-SYMTABSZ-ALIGNMENT-FIXTURE-01` | `scripts/test_ps4_dynlink.py` | In the existing synthetic ELF, set `DT_SCE_SYMTABSZ` to 47 bytes and assert `ValueError("bad symbol table size")`. | `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_ps4_dynlink.py` — exit 0; 23 tests. Log: `local/validation/root-round68-scout/re-baseline.log`, SHA-256 `c0e2adca80ba475d164f837475a9dd113b21848e26c9f2fbf2c4d19eeeccc0b3`. |
| `SRV-01-HOST-MAP-UNMAP-CANONICAL-EMPTY-FIXTURE-01` | `native/tests/test_host_map.cpp` | After unmap, assert `ptr`, `size`, `raw`, and `raw_size` are canonical empty values; unmap again and assert the same state. | `cmake --build local/build/native --target test_host_map` — exit 0; `ctest --test-dir local/build/native -R '^host_map$'` — exit 0, 1/1. Logs: `native-build-baseline.log`, SHA-256 `c53e4bff512449d8f5e22122c7dd376f959b11315a651281b89258e46336091b`; `native-ctest-baseline.log`, SHA-256 `b3ead5ab2594ff840aac120dc4b7f7d4ffe3ba5037616fb840a134dde610bd4a`, both under `local/validation/root-round68-scout/`. |

The current production/test SHA-256 pairs are:

- APP: unchanged `launcher/Sources/LauncherCore/LauncherLifecycle.swift`
  `358e78ae1de70957fd01100e9969916ab4c6c81a0e52365484251087d8c248c4`; test file
  `f4cacf8fd80c73842184d8f44888e83565d86c8b8f5f0cec8b28f87b70eb1ee3`.
- RE: unchanged `scripts/ps4_dynlink.py`
  `28c53257a6f2bd58522f123098d41ca0d884222330a1bf18ce9f8935fb45eff9`; test file
  `95b407e2af82f67c3643367ea5823d7905acfb4111460e483280c6cbf2e56ddc`.
- SRV: unchanged `native/core/host_map.cpp`
  `0ce60c6af1e2d4f9f91f5882326bf516f20dd5e58f936f99b0efc67cbb582f72` and
  `native/core/host_map.hpp`
  `283c3b8f0c98ccb48cb756e9e16ba4fd54c98babc1ea60b942f2cc2276c30bbe`; test file
  `90124f68bc8a57aa35eb8ed0c9800da37bf09fb81bd9416c1ea25b44edc35afd`.

## Ownership and limits

`/root/launcher_review`, `/root/review_host_proto`, and `/root/slot23_raw_xrefs` own only their
respective test files after claim publication. Root remains the only editor of shared documentation
and is responsible for diff review, root replay, task/evidence updates, and publication. No
production code, dependency, install, or `INSTALL_LOG.md` fact changes are proposed.

These fixtures exercise pure Swift lifecycle mapping, a synthetic ELF dynamic-table guard, and
synthetic host-map cleanup state. They establish no live process, ELF loader, allocator integration,
game behavior, or acceptance gate. G0 remains PARTIAL; G1–G6 remain NOT RUN. No cold boot, actual
game audio, real save/reload, or Metal gameplay is demonstrated.

The separate PSARC invalid-UTF-8 candidate remains unclaimed while the user's preference between
normalizing to `PsarcError` and preserving `UnicodeDecodeError` is pending. Existing user-owned
`native/api/CMakeLists.txt` and untracked `native/api/host/` remain untouched and excluded.

**Observation:** At claim time, only shared documentation is modified; worker implementation has
not begun. The test and production files match the hashes above.
**Inference:** Each claimed assertion should pin already-implemented deterministic behavior; the
future passing tests will be limited to those synthetic/value-level paths.
**Remaining risks:** Baselines do not validate a live executable, PS4 module loading, real asset
data, runtime mapping, launcher UI, game boot/audio/save behavior, or Metal gameplay.

## Claim index preflight

- `python3 scripts/project_index.py` exited 0 and indexed 14,463 folders; log
  `local/validation/root-round68-scout/project-index.log`, SHA-256
  `d7e246a26edd98fbd9fd5cd8e2c85d0eca151928bd02d25836cf2aaf5cf29b76`.
- `python3 scripts/project_index.py --check` exited 0 and reports the index current; log
  `local/validation/root-round68-scout/project-index-check.log`, SHA-256
  `7fdb9addb96ef2ae7ed7113ef2ae6ac697879561966b45e9be365994df9a31a5`.
- `trail build --check` exited 0: the 3,566-node / 4,824-edge wiring graph is in sync. The
  optional deep meaning layer remains unbuilt. Log `local/validation/root-round68-scout/trail-check.log`,
  SHA-256 `c5b41a58e2d38180a4a0bf9f623999c56b13aabd238ccd1cc3ee34b96fd707fb`.
- Before staging, `git diff --check` exited 0. `git status --short --branch` showed only the
  claim documentation, plus pre-existing user-owned `native/api/CMakeLists.txt` and untracked
  `native/api/host/`; the user-owned paths remain excluded.
- The first staged whitespace check exited 2 because this checkpoint had one extra blank line at
  EOF. That line was removed; the rerun of `git diff --cached --check` and `git diff --check`
  both exited 0. Combined log retains both outcomes at
  `local/validation/root-round68-scout/claim-diff-check.log`, SHA-256
  `a55bdb77d614627305441ad202d65baab6df1a94ba2a932077d6c9d1a3f65857`.
- `python3 -I scripts/audit_publish_paths.py` exited 0 and accepted 4 staged paths. Log
  `local/validation/root-round68-scout/claim-audit-staged.log`, SHA-256
  `3c12ee6fe3da2e34a61083eb84691904aa6e18c447fc2c39f1e798698aa10b9d`.
- `python3 -I scripts/audit_publish_paths.py --all` exited 0 and accepted 1,640 repository
  paths. Log `local/validation/root-round68-scout/claim-audit-all.log`, SHA-256
  `1d287119d5662186a7aab6641f2da6e254e99ce1b6c1ae71c9fafdc5c6c184e0`.
- `git fetch origin` exited 0; both refs were
  `516e9f4c6648bd7e273521cf22742e1f8fdc9920`; `git rev-list --left-right --count HEAD...origin/main`
  returned `0 0`.

## Claim publication and dispatch

- The four allowlisted claim paths were committed as
  `7afe5fcb699137eaff4c557ba30151b5231234d0` and pushed to `origin/main` before worker edits.
  The initial `git push origin main` exited 0 and advanced `516e9f4..7afe5fc`; a subsequent
  confirmation push exited 0 with `Everything up-to-date`. Confirmation log
  `local/validation/root-round68-scout/claim-push.log`, SHA-256
  `f003da496397b96e83a1edab0fe86cd9f391566234246c3cc64fac197ef7c8f2`.
- `git fetch origin` exited 0; its log is `local/validation/root-round68-scout/claim-fetch.log`,
  SHA-256 `19eaf43821a7660ec323a87c8457bf74823beb296c39f5e01aa8a683aa50f061`. The publication
  state log `local/validation/root-round68-scout/claim-publish-state.log`, SHA-256
  `4e8ec4af8adf6a3ad722dead8d3a1006bfa859c877be5a3a0e651c5d3bdd06ac`, records
  `HEAD == origin/main == 7afe5fcb699137eaff4c557ba30151b5231234d0` and zero divergence.
- After publication, the three test-file assignments were sent to `/root/launcher_review`,
  `/root/review_host_proto`, and `/root/slot23_raw_xrefs`. No worker was authorized to edit shared
  documentation or commit. The first captured post-dispatch status showed the APP test edit in
  progress, with the RE and SRV edits not yet present; later results are in the completion
  checkpoint.

**Next exact action:** Review each worker's one-file diff, independently replay the focused checks,
record E204–E206, refresh project indexes, and publish completion.
