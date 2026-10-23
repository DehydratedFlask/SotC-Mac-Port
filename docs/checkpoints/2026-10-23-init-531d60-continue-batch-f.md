# Checkpoint — Init `0x531d60` alternate path (batch F, 70 units, 327 B)

- Local: 2026-10-23 03:20 CDT (America/Chicago)
- Agent: orchestrator round-10 worker subagent (non-Claude model); the worker left the module and
  its test mid-edit, so the integrator reconciled the naming, converted its `SCENARIO` scaffolding
  into callable cases, fixed two checker bugs and the wrong memory operand, then replayed the build.
- Task: `CPU-01-INIT-531D60-CONTINUE-BATCH-06`
- Gate: **G1 PARTIAL — control-flow model; all callees and vtable slots are hooks**
- Evidence: E544

## What was lifted

`native/core/init_531d60_post_base_alt_5327bd.{hpp,cpp}` models Init's **alternate path**, the
target of batch A's `je 0x5327bd` (taken), plus the `0x5327b6` failure exit:

- `0x5327bd..0x532904` — **327 B, 70 units**, region sha256
  `34b848a9426ffd79d2cebc433472d3d8eba3c6b0686fdd9a8dfc5f0d4c7c0206`.
- Gate `[0x2dfce70]+0x60` byte; vtable dispatch through `[0x3305568]` at slots `+0x48`,
  `+0x28`, `+0x50`, `+0x20`; `call 0xac5260`; float compare against `0x1014c40`; container pair at
  `0x2ea5170` with head/tail compare; optional logging via `0x3e7e0`; two `call 0x4c4950`
  (flag 0 / 1) and joins `jmp 0x532904` or the 36 B `sete r14b` preamble.
- `0x5327b6 xor eax,eax; jmp 0x532c6f` — explicit "return 0" exit reachable from the
  base-initialize failure branch.

The slice runs this module when the caller supplies the taken condition; the existing
not-taken chain (batches B..E) is unchanged.

## Integrator corrections

- **Wrong memory operand**: the module and checker named the container cell `0x2efe170`; the bytes
  at `0x532848` are `48 8d 0d 21 29 97 02`, i.e. `lea rcx,[0x2ea5170]`. Renamed everywhere.
- Checker bugs: a 2-byte `call *(%rax)` was validated as if it had a displacement byte; header
  tokens were compared case-sensitively against a header that uses upper-case hex.
- The appended `SCENARIO(...)` blocks are now plain `alt_case_1..5()` functions called from
  `main()` — this test file has no Catch2 harness.
- Hook/implementation name mismatches (`cell_*` vs `gate_*`/`obj_*`/`container_*`) were reconciled
  and the container cell is typed `const void*` (it points at an inline 16-byte `{head,tail}` pair,
  not at a pointer).

## NOT modelled

Bodies of `0xac5260`, `0x3e7e0`, `0x4c4950`; the five vtable slots' targets; the values behind
`0x2dfce70`, `0x3305568`, `0x2ea5170`, `0x2efe318`, `0x2efe340`; the stack frame; and anything
past the two joins.

## Verification

```
cmake -S native -B local/build/orch-r11 -G Ninja  -> exit 0
cmake --build local/build/orch-r11                -> exit 0
ctest --test-dir local/build/orch-r11             -> 100% tests passed out of 331
tools/lift/run_all_checkers.sh                    -> 411 passed, 20 skipped (need a build dir), 0 failed
python3 -I tools/lift/report_init_coverage.py     -> 3438/3500 B = 98.23%, gaps 36+16+5+5 B
```

## Next exact action

Close the four remaining small gaps (`0x531d60..0x531d84` entry frame, `0x531dcf..0x531ddf`
base-initialize call site, `0x531df9..0x531dfe` `call 0x56dca0`, `0x5327b1..0x5327b6` je-not-taken
continuation), then wire the native `init_1290` into the slice so its inserts execute in the
modelled startup rather than only in unit tests.
