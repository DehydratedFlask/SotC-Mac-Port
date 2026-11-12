# 2026-11-09 — Init post-base calls measured per invocation

**Task:** `CPU-01-INIT-POST-CALLS-01`
**Agent:** Buffy
**Status:** PASSED / ARTIFACT_VERIFIED (per-call trace); callee closure false
**Gate:** G1 **PARTIAL** (unchanged); G0/G3/G4 PARTIAL; G2/G5/G6 NOT RUN

## Files changed

- `native/core/init_post_call_trace.hpp` — outcome/record/log types.
- `native/core/init_post_bodies.{hpp,cpp}` — each hook returns an observed outcome; each call
  appends a record with callee, caller, arguments, return and diagnostic.
- `native/core/init_window_runtime.{hpp,cpp}` — real leaf bodies for five one-byte rets,
  `0x5d06f0` and `0x5c91e0`; named refusals for the other seven callees; merged `post_calls`.
- `native/app/sotc_init_window_main.cpp` — schema `sotc.init_window.v5`, `post_calls[]`,
  executed/blocked counts and `post_calls_complete`.
- `native/tests/test_init_post_bodies.cpp` — call-order/argument records and blocked outcomes.
- `docs/evidence/2026-11-09-init-post-calls.md`, this checkpoint.
- `docs/EVIDENCE.md`, `docs/TASKS.md`, `ROADMAP.md`, `HANDOFF.md`,
  `docs/VALIDATION.md`, `docs/G1_EVIDENCE.md` — records updated.

## Exact commands

```sh
cmake --build local/build/orch-r22 -j 12 --target sotc_init_window test_init_post_bodies
SOTC_INPUTTABLE_NAMES_FIXTURE=local/fixtures/inputtable_551140_names.bin \
  ./local/build/orch-r22/sotc_init_window
ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure
SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh
python3 -I scripts/audit_publish_paths.py --all
python3 -I scripts/check_commit_attribution.py
python3 scripts/project_index.py
git diff --check
```

## Result

- Build exit **0**; focused `test_init_post_bodies` prints `init_post_bodies: ok`.
- Runtime exit **0**, stderr **0 B**.
- `post_chain_complete=true`, `post_calls_complete=false`; 275 registry entries, status 0,
  boss flag `0xA7`, `first_failed_stage=""`.
- `post_call_count=17`; `post_calls_executed=7`; `post_calls_blocked=10`.
- Executed: `0x5ddcc0`, `0x5d57f0`, `0x5d5760`, `0x5d5700`, `0x5cf130` (one-byte rets);
  `0x5c91e0` (174 B copy); `0x5d06f0` (14 B query, host parent byte 0).
- Blocked: `0xa5cd90`, `0x946880` ×2, `0xa5d990`, `0x560b60`, `0x5b9f90`, `0x96f20`,
  `0x5d5410`, `0x7f6f90` ×2 — each with observed arguments and verbatim diagnostic.
- `0x5c64c0` BSS hashes unchanged (`0x19edac14`/`0x75a104c5`/`0x8428e931`); host cell is `0x0`
  because `0x96f20` refused. `0x7f7220` still selects `(0,0x128b8ac)` and `(1,0x128b892)`.
- Full CTest **702/702**, exit **0** (`local/validation/2026-11-09-init-call-trace-ctest.log`).
- Checkers: **908 passed / 0 skipped / 0 failed**, exit 0; entry map **2,929 constants →
  1,861 RVAs / 576 headers (443 duplicates)**. Project index **114,651 folders**; publish
  audit **OK (4,110 paths)**; attribution audit **OK**.
- `git diff --check` clean; **0 staged**; no commit/tag/push.

## Observations vs inference

Observed: every value above is from the real `0x531d60` runtime JSON at
`local/validation/2026-11-09-init-call-trace-runtime-final.json`, including the 17 call records.
Inferred/contract: the host cells at `0x2dca510`, `0x2ff2b10` and `[0x2ff59c0]+0x60` are
host-owned stand-ins, not game runtime state. The blocked bodies were not executed; their
diagnostics describe the observed extents and missing nested models.

## Remaining risks

- Seven callees (10 invocations) remain unexecuted; `post_calls_complete=false`.
- `0x96f20`'s wrapper is not a complete lift without `0x97a50`.
- `engine_category_vars` is only the `0xa5d990` category-loop model, not the whole body.
- `0xf90310` remains a platform service seam; `0x1290` singleton owners remain host-owned.
- No cold boot, audio, save/reload, Metal validation or forbidden-runtime claim.

## Next exact action

Lift the smallest complete blocker chain: `0x96f20` + `0x97a50`, or
`0x5d5410` + `0x8be60`/`0x8f520`/`0x8f1d0`. Do not count the 275-entry checkpoint as callee
closure.

**Attempts:** 0.
