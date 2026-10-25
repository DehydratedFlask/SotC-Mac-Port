# Checkpoint — Init `0x531d60` four remaining spans (byte coverage complete)

- Local: 2026-10-23 03:45 CDT (America/Chicago)
- Agent: orchestrator (integrator)
- Task: `CPU-01-INIT-ENTRY-GAPS-01`
- Gate: **G1 PARTIAL — claimed bytes reach 100% of the window; semantics still partial**
- Evidence: E546

## What was added

`native/core/init_531d60_entry_gaps.{hpp,cpp}` + `native/tests/test_init_531d60_entry_gaps.cpp`
(332nd test) + `tools/lift/check_finit_entry_gaps.py`. Four spans that no other module claimed:

| Span | Range | Size | sha256 (12) | Model |
| --- | --- | --- | --- | --- |
| A entry frame | `0x531d60..0x531d84` | 36 B | `18abd8d4dcc1…` | 7 pushes, `and rsp,-0x20`, `sub rsp,0xc40`, guard load `0x15fe518`, `mov [rsp+0x20],rdi` → `saved_self` |
| B base-init call site | `0x531dcf..0x531ddf` | 16 B | `ee6d49088d55…` | reload `self` from the frame slot, marshal `rsi/r15`, `call 0x255130`, expose `base_ok` |
| C ret-nop site | `0x531df9..0x531dfe` | 5 B | `3bcaa2dea7fb…` | `call 0x56dca0` hook |
| D merge jump | `0x5327b1..0x5327b6` | 5 B | `fb89143aa639…` | `jmp 0x532904` pin |

The fixture asserts the constants, 17 units, the self round-trip into the base call, `base_ok`
visibility when the callee returns false, and that a missing callee is counted but never treated
as success.

## Honest limits

- The 0xc40 stack frame and the `and rsp,-0x20` alignment are **pins, not emulation**.
- Span B is a call-site pin; the slice already invokes `base_initialize_255130` separately, so this
  does not execute base initialization a second time.
- These four spans are pinned as a set and are **not sequenced into the slice**; the coverage tool
  counts claimed bytes, not order. `0x5327b1..0x5327b6` and `0x531df9..0x531dfe` are trivially
  ordered by the existing chain, while A/B sit in the function's entry.
- Bodies of `0x255130` (here) and `0x56dca0` remain callees.

## Verification

```
cmake -S native -B local/build/orch-r12 -G Ninja   -> exit 0
cmake --build local/build/orch-r12                 -> exit 0
ctest --test-dir local/build/orch-r12              -> 100% tests passed out of 332
python3 -I tools/lift/check_finit_entry_gaps.py    -> OK (4 span shas, call/jump targets)
python3 -I tools/lift/report_init_coverage.py      -> 3500/3500 B = 100.0%, 0 gaps
```

## Next exact action

Stop growing coverage and grow semantics: wire span B's base-initialize contract into the slice's
early phase so the entry call and the later base phase are one modelled call, or move to
`0xa56560` (the engine behind `0x54b250`'s stat query) and the owner singletons from
`0x533100`/`0x2569d0`.
