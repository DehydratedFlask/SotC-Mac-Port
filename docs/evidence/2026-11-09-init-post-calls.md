# Init post-base bodies: every outgoing call is measured

**Date:** 2026-11-09
**Task:** `CPU-01-INIT-POST-CALLS-01`
**Agent:** Buffy
**Status:** PASSED / ARTIFACT_VERIFIED (per-call trace); callee closure **false**
**Gate effect:** G1 remains **PARTIAL**. This measures the post-base call surface; it does not
close the named callee bodies and does not boot the game.

## Change

E1426 counted the calls made by `0x56dcb0`, `0x5c64c0` and `0x7f7220`, but its hooks were
no-ops. E1427 replaces that dispatch count with a per-invocation record:

- `native/core/init_post_call_trace.hpp` defines the outcome/record/log types.
- `native/core/init_post_bodies.{hpp,cpp}` requires every hook to return
  `InitPostCallOutcome`; the body records the callee RVA, caller, arguments, return/void state,
  blocker name, and the runtime-generated diagnostic verbatim.
- `native/core/init_window_runtime.cpp` supplies real native leaf bodies for the five one-byte
  `ret` targets, the 14-byte `0x5d06f0` query, and the 174-byte `0x5c91e0` data copy. The other
  seven callees return a named refusal before any host or game state is changed.
- `native/app/sotc_init_window_main.cpp` emits schema `sotc.init_window.v5` with `post_calls[]`,
  `post_call_count`, `post_calls_executed`, `post_calls_blocked` and `post_calls_complete`.
- `native/tests/test_init_post_bodies.cpp` checks the recorded order/arguments and both executed
  and blocked outcomes.

`post_chain_complete=true` means all ten post-base sites were reached and the 275-entry
checkpoint survived. It does **not** mean callee closure: `post_calls_complete=false` is emitted
because 10 of 17 invocations are named blockers.

## Exact command and observed output

```sh
SOTC_INPUTTABLE_NAMES_FIXTURE=local/fixtures/inputtable_551140_names.bin \
  ./local/build/orch-r22/sotc_init_window
```

Exit **0**, stderr **0 B**. Selected top-level values from
`local/validation/2026-11-09-init-call-trace-runtime-final.json`:

```json
{
  "schema": "sotc.init_window.v5",
  "ok": true,
  "post_chain_complete": true,
  "post_chain_blocked": "",
  "post_call_count": 17,
  "post_calls_executed": 7,
  "post_calls_blocked": 10,
  "post_calls_complete": false,
  "registry_entries": 275,
  "registry_status": 0,
  "boss_flag_after": 167,
  "post_56dcb0": {"native": true, "calls": 5, "blocked": 5, "a5cd90": 1, "946880": 2,
                   "a5d990": 1, "560b60": 1, "desc0": "0x101e7ac", "desc1": "0x30a"},
  "post_5c64c0": {"native": true, "blocked": 3, "bss_a": "0x19edac14",
                   "bss_b": "0x75a104c5", "bss_c": "0x8428e931", "cell": "0x0",
                   "calls": 9, "tail": true},
  "post_7f7220": {"native": true, "blocked": 2, "branch": false, "sets": 2,
                   "mode0": 0, "str0": "0x128b8ac", "mode1": 1, "str1": "0x128b892"},
  "first_failed_stage": ""
}
```

### The 17 records (verbatim values from that run)

| # | Caller | Callee | State | Observed args | Return | Blocker | Observed diagnostic |
|---:|---|---|---|---|---|---|---|
| 1 | `0x56dcb0` | `0xa5cd90` | blocked | `0x16f8c6ee0, 0x101e7ac, 0x30a` | `none` | `body_not_lifted` | 0xa5cd90 body absent: 1143 B/260 insns/46 direct calls; descriptor 0x101e7ac/0x30a; nested 0x9854f0,0x994dd0,0x12820,0x14c2f0 have no complete native model |
| 2 | `0x56dcb0` | `0x946880` | blocked | `0x1` | `none` | `body_not_lifted` | 0x946880 body absent: 353 B/95 insns; value=1; nested 0x98a450/0x944de0/0x9469f0/0x984700 unlifted; global lock 0x2dd0a78 has no host owner |
| 3 | `0x56dcb0` | `0x946880` | blocked | `0x0` | `none` | `body_not_lifted` | 0x946880 body absent: 353 B/95 insns; value=0; nested 0x98a450/0x944de0/0x9469f0/0x984700 unlifted; global lock 0x2dd0a78 has no host owner |
| 4 | `0x56dcb0` | `0xa5d990` | blocked | `0x16f8c6ee0, 0x16f8c6f20` | `none` | `body_not_lifted` | 0xa5d990 body absent: 2709 B/500 insns; engine_category_vars models only the 32-category loop, while 0x10a90/0x20af0/0x26ea0/0x272d0 and the engine fields remain unlifted |
| 5 | `0x56dcb0` | `0x560b60` | blocked | `none` | `none` | `body_not_lifted` | 0x560b60 body absent: 803 B/156 insns; nested 0x11bc0/0x17070/0x17090/0x3ca330/0x273890 and global 0x2f08910..0x2f08960 have no native model |
| 6 | `0x5c64c0` | `0x5b9f90` | blocked | `none` | `none` | `body_not_lifted` | 0x5b9f90 body absent: 46881 B/7685 insns; first nested call 0x26c10 at 0x5ba02c; no lifted extent for this menu/UI builder |
| 7 | `0x5c64c0` | `0x96f20` | blocked | `0x100, 0x100, 0x1, 0x0` | `none` | `nested_body_not_lifted` | 0x96f20 wrapper absent: 41 B; args 0x100,0x100,0x1,0x0; it calls 0x97a50 (2835 B, 10 args), which has no native model |
| 8 | `0x5c64c0` | `0x5ddcc0` | executed_native | `none` | `void` | — | 1-byte FDE body: ret; no register or memory effect |
| 9 | `0x5c64c0` | `0x5d57f0` | executed_native | `none` | `void` | — | 1-byte FDE body: ret; no register or memory effect |
| 10 | `0x5c64c0` | `0x5d5410` | blocked | `none` | `none` | `body_not_lifted` | 0x5d5410 body absent: 420 B/109 insns; first nested 0x8be60 (547 B) at 0x5d5421 gets rdi=0x30015c8 esi=0x1c edx=0x50 ecx=1 r8=2 r9=0; 0x8f520/0x8f1d0 are also unlifted |
| 11 | `0x5c64c0` | `0x5d5760` | executed_native | `none` | `void` | — | 1-byte FDE body: ret; no register or memory effect |
| 12 | `0x5c64c0` | `0x5d5700` | executed_native | `none` | `void` | — | 1-byte FDE body: ret; no register or memory effect |
| 13 | `0x5c64c0` | `0x5c91e0` | executed_native | `none` | `void` | — | read 0xa0-byte source, wrote 0x28-byte destination; dst[0]=0x3f900000 dst[0x20]=0x3f800000 dst[0x24]=0x3f800000; src[0x54]=0x3f90a3d7 src[0x80]=0x3f400000 src[0x84]=0x3fc00000 |
| 14 | `0x5c64c0` | `0x5cf130` | executed_native | `none` | `void` | — | 1-byte FDE body: ret; no register or memory effect |
| 15 | `0x7f7220` | `0x5d06f0` | executed_native | `none` | `0x0` | — | host parent [0x2ff59c0]+0x60 byte=0; query returns false |
| 16 | `0x7f7220` | `0x7f6f90` | blocked | `0x0, 0x128b8ac` | `none` | `body_not_lifted` | 0x7f6f90 body absent: 656 B/173 insns; mode=0 string=0x128b8ac; nested 0x57cb70/0xf902a0/0xf90390/0xf90210/0x56cdb0 unlifted |
| 17 | `0x7f7220` | `0x7f6f90` | blocked | `0x1, 0x128b892` | `none` | `body_not_lifted` | 0x7f6f90 body absent: 656 B/173 insns; mode=1 string=0x128b892; nested 0x57cb70/0xf902a0/0xf90390/0xf90210/0x56cdb0 unlifted |

The object/data addresses in records 1 and 4 are host-owned cells from this run, not game
pointers. Record 12's source is the pinned `0x2dca510` `.data` block (0xa0 B); the body performs
all 12 destination writes and the four source rewrites. Record 15 reads the host parent cell at
`[0x2ff59c0]+0x60`; the byte was 0, so `0x7f7220` took the remaster/original path.

## What this proves, and what it does not

**Observed:** all 17 invocations are enumerated with a callee RVA, caller, argument list,
executed/blocked state and either a native return/state or a blocker plus diagnostic. The five
one-byte `ret` bodies really return with no state change. `0x5c91e0` really copied and rewrote
its cells. `0x5d06f0` really indexed the host parent and returned 0. The anchored run still
reaches `0x532c1c` with 275 registry entries, status 0 and boss flag `0xA7`.

**Not observed:** `0xa5cd90`, `0x946880`, `0xa5d990`, `0x560b60`, `0x5b9f90`, `0x96f20`
(and its `0x97a50` body), `0x5d5410` and `0x7f6f90` did not execute. Their records are
refusals, not plausible returns. `engine_category_vars` models only the category loop inside
`0xa5d990`, so it was not used as a substitute for the 2,709-byte body. `0x96f20` is a 41-byte
wrapper around the unmodelled 2,835-byte `0x97a50`; it was not claimed as a standalone lift.

`post_5c64c0.cell` is now `0x0`, not E1426's host sentinel `0x5c64c0`, because `0x96f20`
refused and the body stores the observed null result. The BSS hashes are unchanged
(`0x19edac14`/`0x75a104c5`/`0x8428e931`).

## Verification

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

- Focused build exit **0**; `test_init_post_bodies` prints `init_post_bodies: ok`.
- Runtime exit **0**, stderr **0 B**; 17 records, 7 executed, 10 blocked; 275 entries/status 0.
- Full CTest **702/702**, exit **0** (`local/validation/2026-11-09-init-call-trace-ctest.log`).
- Checker sweep: **908 passed / 0 skipped / 0 failed**, exit 0; entry map regenerated
  **2,929 constants → 1,861 RVAs / 576 headers (443 duplicates)**. Publish audit:
  **OK (4,110 paths)**; project index **114,651 folders**; attribution audit **OK**;
  `git diff --check` clean; **0 staged**.
- `git diff --check` clean; **0 staged**; no commit, tag or push.
- Binary `local/build/orch-r22/sotc_init_window` is an ARM64 Mach-O executable.

## Next exact action

Lift one of the named blockers as a complete body, starting with the smallest independent one:
`0x96f20` plus its `0x97a50` callee, or `0x5d5410` plus `0x8be60`/`0x8f520`/`0x8f1d0`.
Do not relabel `engine_category_vars` as `0xa5d990`, and do not claim callee closure from the
275-entry checkpoint.
