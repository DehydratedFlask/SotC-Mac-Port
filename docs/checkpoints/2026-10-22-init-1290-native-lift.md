# Checkpoint — native lift of `0x1290` (ordinary native function)

- Local: 2026-10-23 00:20 CDT (America/Chicago)
- Agent: orchestrator (integrator-authored lift, using E516 body analysis and E522 width answer)
- Task: `CPU-01-INIT-1290-NATIVE-01`
- Gate: **G1 PARTIAL — fixture-level model; nothing executed against the game**
- Evidence: E527

## What was built

`native/core/init_1290.{hpp,cpp}` is an ordinary native function replacing the 5 B call-site stub
with the recovered effect sequence, and `native/tests/test_init_1290.cpp` is its unit fixture
(`init_1290`, 327th test). `tools/lift/check_f1290.py` re-ties every claim to
`local/re/eboot.elf`.

Recovered order, modelled exactly:

1. build descriptor A `{ctx=&self, invoke=0x1a00, 0, fn=0xb10}`, `A+0x40 = 0x1594060`,
   `A+0x48 = 0`;
2. `insert A` — original `call 0x1bde30`, owner `*(0x2ee68c8)`;
3. vcall — original `call *0x18(%rax)` through table `0x1594060`, argument **`&A + 0x20`**;
4. build descriptor B `{ctx=&self, invoke=0x1a10, 0, fn=0xbb0}`;
5. `insert B` — original `call 0x21cd0`, owner `*(0x2dfcbc8)`.

The fixture asserts the order, both descriptors field by field, that the vcall receives exactly
`&A + 0x20` (not `&A`), and that a missing subsystem hook yields `ok == false` with the sequence
stopped where the original would fault — no fake success.

## OBS (checker-verified)

Extent `0x1290..0x1353` = 195 B, fn sha256 `185f03d382259a5e…`; 12 B nop pad before, 13 B after;
`ret` at `0x134d`; direct calls `0x12f5→0x1bde30`, `0x1334→0x21cd0`, `0x134e→0xf8f3a0`;
`0x1301 ff 50 18` = `call *0x18(%rax)`; table `0x1594060` slot 3 = `0x19f0` = `c3` (bare `ret`);
guard cell `0x15fe518` read-only.

## Deliberate non-modelling

- `__stack_chk_guard` fail path: read-only, can only abort; modelling it would invent a success
  observable that does not exist.
- The two singleton owners: their allocation lives in `0x533100` → `0x2569d0` and cannot be
  ordered against Init `0x531d60` statically (E522), so the hooks receive the owner as `nullptr`
  and the caller must supply the real object.
- Bodies of `0x1bde30`, `0x21cd0`, `0xb10`, `0xbb0`; the `0x1594060` slot-3 target beyond
  "bare ret".

## Verification

```
python3 -I tools/lift/check_f1290.py        -> OK (extent/sha/calls/vcall/table/constants)
ctest --test-dir local/build/orch-r4 -R '^init_1290$'   -> pass (part of 327/327)
ctest --test-dir local/build/orch-r4                    -> 100% tests passed out of 327
```

## Next exact action

Lift `0x1beeb0` (insert body, 284 B) behind the `insert_1bde30` hook so the descriptor copy is
executed rather than observed, and give the owner singletons a native owner object created by the
`0x533100` path; only then can `init_1290` be wired into a larger startup chain.
