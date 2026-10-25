# Checkpoint — native lift of the `0x21cd0` bump-allocator insert (B path)

- Local: 2026-10-23 01:35 CDT (America/Chicago)
- Agent: orchestrator round-7 worker subagent (non-Claude model); integrator replayed the build,
  the byte checker and the 329-test suite.
- Task: `CPU-01-INSERT-21CD0-NATIVE-01`
- Gate: **G1 PARTIAL — fixture-level model; nothing executed against the game**
- Evidence: E533

## What was built

`native/core/insert_21cd0.{hpp,cpp}` + `native/tests/test_insert_21cd0.cpp` (329th test) +
`tools/lift/check_f21cd0.py`. `init_1290`'s B insert now executes this body when a real container
is supplied (`container_b`, hook fallback retained), symmetric with the A path from E532.

## OBS (integrator replay)

Body `0x21cd0..0x21d9c` = **204 B**, fn sha256 `2fb96a5555881520…`; `ret` at `0x21d96`, guard
call `0x21d97 → 0xf8f3a0`, 4 B nop pad to the next function. Rel32 targets re-derived from the
bytes: `0x21d27 → 0x3b390` (begin thunk), `0x21d73 → 0x23210` (grow), `0x21d7b → 0x3b3c0` (end
thunk). Guard cell `0x15fe518` is read, not written.

Effect order: own frame and canary; build a 0x18 B stack aggregate of assert metadata (delegate
signature pointer, source-path pointer, exception code `0xea`); begin thunk on `owner+0x58`;
capacity check `owner[0x40]` (bump position) vs `owner[0x48]` (end); fast path zeroes the 0x20 B
slot's first 16 B, copies `descriptor[0x08..0x18)` with a `vmovdqu` pair, copies `descriptor[0x00]`
and `descriptor[0x18]`, bumps the pointer by 0x20; grow path calls `0x23210(owner+0x38, &B)`;
`fn != 0` self-links the slot; end thunk on `owner+0x58`; canary compare.

This is a **different container** from `0x1beeb0`: 0x20 B flat entry in a bump-allocated buffer
with begin/end locking, not a 0x60 B intrusive-list node with `operator new`.

## Subsystem identification (INF from the assert metadata)

The assert metadata names the `BPE` core `ConVar` translation unit and a
`ConCmdManager::RegisterGetCmdsForEditorCallback` delegate signature. `0x21cd0` is therefore the
console-variable/command registration insert used by the same singleton whose vtable field the
E530 analysis attributed to a console-variable object. This is debug/console infrastructure that
Init still executes unconditionally; it is not a gameplay subsystem.

## NOT modelled

The begin/end thunks `0x3b390`/`0x3b3c0` and the grow function `0x23210` (both remain hooks,
fail-visible); the canary fail path; the owner's construction (`0x533100`/`0x2569d0`, not
statically orderable against Init); the assert format strings are not reproduced in native code.

## Verification (integrator replay)

```
cmake -S native -B local/build/orch-r6 -G Ninja   -> exit 0
cmake --build local/build/orch-r6                 -> exit 0
ctest --test-dir local/build/orch-r6              -> 100% tests passed out of 329
python3 -I tools/lift/check_f21cd0.py             -> OK
python3 -I tools/lift/check_f1290.py              -> OK
python3 -I tools/lift/check_finsert_1beeb0.py     -> OK
python3 -I tools/lift/check_ctor_slots.py         -> 155 pairs
```

## Next exact action

Lift the grow function `0x23210` so the B path stops using a hook, or build the owning container
constructor family (`0x1bdb10`/`0x1bdae0`/`0x1bdeb0`) so both inserts can run against a natively
created owner.
