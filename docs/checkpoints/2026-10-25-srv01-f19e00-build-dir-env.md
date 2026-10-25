# E28 checker build-directory selection fix

**Date:** 2026-10-25 03:25 CDT

**Integrator:** Codex `/root`

**Task:** `SRV-01-F19E00-CHECKER-BUILD-DIR-01`

**Status:** FIXTURE_VERIFIED

## Finding and change

On the first full lift-checker run against the merged tree,
`SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` failed only
`check_f19e00_accounting.py` with a fixture-case-set mismatch. The wrapper supplied the configured
build directory, but the checker ignored it and read the older `local/build/native/test_arena_accounting`
binary, which emits five cases. The current `local/build/orch-r22/test_arena_accounting` emits all six,
including `zero_request_repairs_peak`.

`tools/lift/check_f19e00_accounting.py` now uses `SOTC_NATIVE_BUILD` to select its default fixture
directory, matching `run_all_checkers.sh`'s documented contract. An explicit `--fixture` still takes
precedence. The checker reads the pinned ELF as data and does not execute it.

## Verification

| Command | Result | Evidence |
|---|---|---|
| `cmake -S native -B local/build/orch-r22` | PASSED | `local/validation/root-merge-integration-20261025/configure.log` |
| `cmake --build local/build/orch-r22 -j 12` | PASSED | `local/validation/root-merge-integration-20261025/build.log`; initial combined build completed 448/448 steps |
| `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure` | PASSED, 397/397 | `local/validation/root-merge-integration-20261025/ctest.log` |
| `python3 -I tools/lift/check_f19e00_accounting.py --elf local/re/eboot.elf --fixture local/build/orch-r22/test_arena_accounting` | PASSED | terminal output; same fixture selected by the environment-default run below |
| `SOTC_NATIVE_BUILD=local/build/orch-r22 python3 -I tools/lift/check_f19e00_accounting.py` | PASSED | `local/validation/root-merge-integration-20261025/f19e00-targeted.log` |
| `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` | PASSED, 481 / 0 / 0 | `local/validation/root-merge-integration-20261025/lift-checkers.log` |
| `python3 -I tools/lift/check_f19e00_accounting.py --elf local/re/eboot.elf --fixture local/build/native/test_arena_accounting` | Expected failure reproducer: old binary omits `zero_request_repairs_peak` | `local/validation/root-merge-integration-20261025/stale-fixture-path-repro.log` |

Checker source SHA-256: `ab541022a56f07cd5e892388d9aec8937a434fcf24922c983c9aa57776de9ac4`.
No dependency changed. No private game payload or build output is staged.

## Limits

This fixes test-fixture selection and verifies instruction-derived accounting parity. It does not
claim allocator startup integration, original-code execution, or game behavior.
