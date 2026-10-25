# Checkpoint — list destructor `0x1bdeb0` lifted (0x58-byte flag nodes)

- Local: 2026-10-23 (America/Chicago)
- Agent: integrator
- Task: `CPU-01-LIST-DTOR-1BDEB0-NATIVE-01`
- Gate: **G1 PARTIAL — teardown exact; releases/free are hooks**
- Evidence: E559

## What was built

`native/core/list_dtor_1bdeb0.{hpp,cpp}` + `native/tests/test_list_dtor_1bdeb0.cpp` (338th test)
+ `tools/lift/check_flist_dtor_1bdeb0.py`. This is the teardown invoked on `*(0x2ee68c8)` — the same
singleton `0x1290` registers into — so it now runs natively over the *correct* node type that E558
identified.

## OBS (checker-verified)

Body `0x1bdeb0..0x1bdf28` = 120 B, sha256 `7d64b9d3…ba35`, `ret` at `0x1bdf27`. The loop tests
`dword [node+0x50]`, decrements `dword [head+0x10]`, and calls `0xf590` twice
(`0x1bdefc`, `0x1bdf05`) plus `0x170c0` (`0x1bdf12`) per erased node. Node type: 0x58 B with
prev at `+0x00`, next at `+0x08`, two 0x20 B callables at `+0x10`/`+0x30` (control pointer at
`+0x18`), dword flag at `+0x50`.

## Model

The walk starts at `sentinel.prev`, skips nodes whose flag is non-zero (the original does), and for
each eligible node: advances first, recovers the node as `prev->next`, unlinks, releases `+0x30`
then `+0x10` through `shared_release_f590`, frees with tag 2, and decrements the count. A missing
free hook fails before touching the list; a release that cannot complete stops the walk without
pretending the node was freed.

The fixture covers: mixed flags (middle node skipped and left linked), erase order (walk starts at
the tail, so `n3` goes first), two releases before each free, count decrement, skipped node
callables with untouched control blocks, empty list, and the missing-hook failure.

## NOT modelled

Bodies of `0xf590`/`0x170c0` (hooks), the `0x2ee68c8` singleton construction, and the meaning of the
flag (flagged nodes are deliberately skipped by the original).

## Verification

```
python3 -I tools/lift/check_flist_dtor_1bdeb0.py -> OK
ctest --test-dir local/build/orch-r18           -> 100% tests passed out of 338
tools/lift/run_all_checkers.sh                  -> 418 passed, 20 skipped, 0 failed
```

## Next exact action

Lift `0x1bdb50` (the 0x58-node inserter: allocate 0x58, copy two callables with indirect
`lock incq` acquires, write the flag, link) so this list can be built natively as well as torn down.
