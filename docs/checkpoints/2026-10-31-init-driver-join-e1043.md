# Init driver join 1b/1c — E1043

- **Local time / agent:** 2026-10-31 01:03 CDT, `/root`
- **Task:** `CPU-01-INIT-DRIVER-JOIN-01`
- **Status:** `COMPLETE / FIXTURE_VERIFIED`; failed semantic hypotheses: 0. One stale fixture expectation failed during the earlier iteration and was corrected.
- **Files changed for this task:** `native/core/appmain_init_ctor_98a90_slice.cpp`, `native/core/appmain_init_ctor_98a90_slice.hpp`, `native/core/init_531d60_post_base_alt_5327bd.cpp`, `native/core/init_531d60_post_base_alt_5327bd.hpp`, `native/tests/test_appmain_init_ctor_98a90_slice.cpp`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/INDEX.md`, `HANDOFF.md`, and this checkpoint.

## Result

Both alternate outcomes join at `0x532904`. The error preamble can run the native `0x91210` model when configured, and continue-C does not replay that preamble. Continue-C to E and continue-C to D to E carry the same `r12b`; continue-B derives `bl ^ 1` only when EAX is known. Unknown branch inputs stop before the dependent work. `r13` is forwarded from alternate entry to C/E. Calls behind unknown alternate predicates are suppressed and explicitly covered by callback counters.

## Verification

Commands run from the repository root:

```sh
cmake --build local/build/orch-r22 --target test_appmain_init_ctor_98a90_slice -j 12
ctest --test-dir local/build/orch-r22 -R '^appmain_init_ctor_98a90_slice$' --output-on-failure
python3 tools/lift/check_f531d60_post_base_alt_5327bd.py
python3 tools/lift/check_f531d60_post_base_continue_c.py
python3 tools/lift/check_f531d60_post_base_continue_c_after_merge.py
```

All five commands passed after the final callback-guard assertions. CTest passed 1/1; all three byte checkers reported `OK`. Captured output is in `local/validation/driver-join-e1043f/` (`build.log`, `ctest.log`, and the three checker logs). The first stale empty-alt expectation is recorded under `local/validation/driver-join-e1043c/`; it was corrected and did not represent a failed semantic hypothesis.

## Evidence boundary and remaining risks

- **Observed:** fixture result flags and hook counters; pinned byte checkers accept the covered ranges and branch targets.
- **Inference:** routing and predicate-knownness reflect the recovered CFG for these modeled blocks.
- The alternate route selector still depends on host-provided gate hooks. No executable or Sony module is loaded. No actual game boot, audio, save/reload, or Metal gameplay path was exercised. Full CTest and full lift-checker sweep were **NOT RUN** in this checkpoint. G0–G6 do not advance.
- The independent CritSec review found two concurrency defects in the existing E1034 model; see active task `RE-02-CRITSEC-CONTENTION-01` and the current top of `HANDOFF.md`.

## Next exact action

Review the claimed CritSec sidecar/lease repair, then run the bounded focused test and CritSec checker. Task status is complete; attempt count is 0 under the three-try semantic-hypothesis rule.
