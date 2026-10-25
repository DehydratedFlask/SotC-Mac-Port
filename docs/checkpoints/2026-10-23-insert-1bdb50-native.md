# Checkpoint — 0x58-node inserter `0x1bdb50` lifted

- Local: 2026-10-23 (America/Chicago)
- Agent: integrator
- Task: `CPU-01-INSERT-1BDB50-NATIVE-01`
- Gate: **G1 PARTIAL — insert exact; allocator/release bodies are contracts**
- Evidence: E560

## Why this one matters

E558 established that the singleton `*(0x2ee68c8)` holds two node types, and E559 lifted the
teardown for the 0x58-byte ones. This round lifts the **other half of that pair** — the code that
builds those nodes — so the list can now be created and destroyed natively, and the two modules are
proven to agree on one layout by a round-trip fixture.

## OBS (checker-verified)

Body `0x1bdb50..0x1bdc80` = 304 B, sha256 `add72de8…1313`, `ret` at `0x1bdc77`, stack-canary tail
at `0x1bdc78` (`call 0xf8f3a0`), canary slot `0x15fe518` loaded at `0x1bdb66`.

Register-level signature: `rdi` = sentinel, `rsi` = callable A, `rdx` = callable B, `ecx` = flag.
The body copies both 0x20 B callables to stack temporaries, takes one reference on each live control
block (`lock incq (%rax)` at `0x1bdb9e`/`0x1bdbcf`), allocates `0x58` through `0x17090`
(`movl $0x58,%edi` at `0x1bdbfc`), stores the callables at `+0x10`/`+0x30`, writes the flag with
`movl %r14d,0x50(%rax)` (`0x1bdc2c`), links `node->prev = sentinel`,
`node->next = sentinel->next`, `next->prev = node`, `sentinel->next = node`, increments
`incq [sentinel+0x10]` (`0x1bdc4d`), then **zeroes both temporaries and releases them** through
`0xf590` (`0x1bdc51`, `0x1bdc5b`).

## The subtlety worth recording

The two trailing releases are **no-ops by construction**: the temporaries are zeroed first, and
`0xf590` returns immediately on a null control pointer (E552). So the net effect is exactly one
surviving reference per callable, owned by the node — the acquire is transferred, not doubled. The
native model keeps that shape instead of deleting the releases, because deleting them would erase
the ownership hand-off from the record.

Second detail: the original increments the count with a **64-bit** `incq` (so it also bumps the
4-byte pad at `+0x14`), while `0x1bdeb0` decrements only the low dword. They agree for any count
below 2^32; the model keeps a 32-bit count and documents the difference.

## Host-safety extension (explicitly not original behaviour)

The original never checks the allocation result and would dereference null. The native model
returns a visible failure, restores both acquires through the same release path, and leaves the list
untouched. A missing allocator is rejected before any refcount is touched. Both are labelled in the
header; neither is claimed as recovered behaviour.

## Verification

```
python3 -I tools/lift/check_finsert_1bdb50.py -> OK
ctest --test-dir local/build/orch-r22         -> 100% tests passed out of 339
tools/lift/run_all_checkers.sh                -> 419 passed, 20 skipped, 0 failed
```

The fixture also runs the built list through `list_dtor_1bdeb0` (mixed flags: 7 / 0 / 9 → the
unflagged node is erased, the flagged ones survive), which is the first test that exercises both
modules against the same memory layout. It runs under ASan and caught a real fixture bug on the way
(reading a control block the release had already freed).

## Next exact action

`0x1bdb50` has no other caller in the executable segment, so the next real step is its **caller**:
find who passes `*(0x2ee68c8)` to it (same shape as the E558 scan for `0x1bdeb0`) and whether that
site is inside `0x2569d0`, the owner installer that is still the gate for real singletons.
