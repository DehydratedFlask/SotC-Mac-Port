# 2026-11-09 — E1427 checker-contract repair

**Task:** `AUD-01-INIT-POST-CALL-CHECKERS-01`
**Agent:** Buffy
**Status:** PASSED / ARTIFACT_VERIFIED — full sweep and final checks green
**Depends on:** E1427; `CPU-01-INIT-POST-CALLS-01`

## Problem

The first full sweep after E1427 reported **897 passed, 0 skipped, 11 failed**:

- `check_entry_map.py`: `docs/entry_map.json` stale after the E1427 native constants.
- Ten `check_f531d60_post_base_*` call-site checkers: `FAIL omission`, because each still
  required the literal `NOT modelled` line that E1426 removed when those bodies became native.

The failures were real gate failures. Re-adding `NOT modelled` would have restored a false claim.

## Change

- Regenerated `docs/entry_map.json` from the live headers:
  **2,929 constants → 1,861 distinct guest RVAs in 576 headers (443 duplicates)**.
- Updated each affected checker to require the *current* claim instead of stale omission text:
  - `1290`: `real init_1290 body` + `E1426`
  - `499f80`: `run_init_499f80_body` + `E1034`
  - `56db70`, `56fad0`, `5b3cf0`, `5b3d00`: `native empty` + `E1426`
  - `56dcb0`: `132 B body is native` + `E1426`
  - `5c64c0`: `291 B body is native` + `E1426`
  - `7f7220`: `54 B body is native` + `E1426`
  - `f90310`: `Sony PLT body is not lifted` + `E1426`
- Updated the checker docstrings and success output so they no longer print “body NOT claimed”.
- No native source or test behavior changed.

## Verification

- Targeted set: **11/11 exit 0** (`check_entry_map` plus the ten repaired checkers), log
  `local/validation/2026-11-09-post-calls-checker-fix.log`.
- Full sweep: **908 passed, 0 skipped, 0 failed**, exit 0
  (`local/validation/2026-11-09-checker-sweep2.log`).
- Final CTest before this docs-only repair: **702/702**, exit 0.
- Project index **114,651 folders**; publish audit **OK (4,110 paths)**; attribution **OK**;
  `git diff --check` clean; **0 staged**.

## Remaining risk

The repaired assertions still pin the ELF fragment and the actual current native-body claim; they
do not prove runtime behavior beyond E1427. No native source or test changed, so the **702/702**
CTest result remains the last native-code verification.

## Next exact action

Take the next blocked callee only after re-deriving its full nested chain; do not treat the
checker repair as callee closure or game-runtime progress.
