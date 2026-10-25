# E28 zero-request checker parity

**Date:** 2026-10-25 02:56 CDT
**Integrator:** Codex `/root`
**Task:** `SRV-01-F19E00-ZERO-REQUEST-PEAK-UPDATE-FIXTURE-01`
**Status:** candidate verified; PR publication pending

## Change

The existing `arena_accounting` fixture emits the `zero_request_repairs_peak` case, but
`tools/lift/check_f19e00_accounting.py` did not include it in its expected-case table. A fresh build
therefore made the checker reject the fixture's case set. The checker now independently pins the
known inputs `(requested=0, mode=0, +0x80=0x100, +0x88=0xff, +0x90=0x123)` and validates the output
against the E28 instruction-derived qword model. `docs/TASKS.md` and the existing E368 evidence now
record this checker parity.

## Verification

| Command | Result | Log |
|---|---|---|
| `cmake -S native -B local/build/e28-checker` | PASSED | `local/validation/srv01-f19e00-checker-20261025/configure.log` |
| `cmake --build local/build/e28-checker --target test_arena_accounting -j 12` | PASSED | `local/validation/srv01-f19e00-checker-20261025/build.log` |
| `ctest --test-dir local/build/e28-checker -R '^arena_accounting$' --output-on-failure` | PASSED, 1/1 | `local/validation/srv01-f19e00-checker-20261025/ctest.log` |
| `python3 -I tools/lift/check_f19e00_accounting.py --elf '/Volumes/Extra/SoC Mac Native/local/re/eboot.elf' --fixture local/build/e28-checker/test_arena_accounting` | PASSED; exact ELF bytes checked, fixture output checked, ELF not executed | `local/validation/srv01-f19e00-checker-20261025/checker.log` |
| `python3 -I scripts/audit_publish_paths.py --all` | PASSED, 2,755 paths | terminal output |
| `python3 scripts/project_index.py` | PARTIAL; reported 842 folders in this temporary worktree, producing an incomplete `docs/FOLDERS.md`; restored that generated file from current main | terminal output |
| `git diff --check origin/main` | PASSED | empty output |
| `trail build --check` | NOT RUN: this temporary worktree has no Trail graph (`NO GRAPH`) | terminal output |

The first checker invocations used default local paths absent from this isolated worktree and failed
to locate `local/re/eboot.elf` / `local/build/native/test_arena_accounting`. No code check failed. The
successful run points read-only at the existing private ELF and the fresh local fixture. No source
binary or private evidence is staged. The temporary worktree's project index did not match the
tracked current-main inventory; its generated `docs/FOLDERS.md` was restored. Regenerate and verify
the index from the full root checkout before final handoff.

## Limits and next action

This is parity for a synthetic accounting fixture and the static checker only. It makes no allocator,
game-runtime, or G1 claim. Next: run attribution and staged-path audits, commit under Johnny Wang,
publish a focused PR, refresh `main` before merge, and record the merge SHA.
