# Checkpoint — native lift of the `0x1beeb0` list-3 insert body

- Local: 2026-10-23 01:05 CDT (America/Chicago)
- Agent: orchestrator round-5 worker subagent (non-Claude model); integrator replayed the build,
  found and fixed a checker length bug, and validated the suite.
- Task: `CPU-01-INSERT-1BEEB0-NATIVE-01`
- Gate: **G1 PARTIAL — fixture-level model; nothing executed against the game**
- Evidence: E531

## What was built

`native/core/insert_1beeb0.{hpp,cpp}` + `native/tests/test_insert_1beeb0.cpp` (328th test) +
`tools/lift/check_finsert_1beeb0.py`. The `0x1290` insert effect is now executable instead of an
observed hook: `insert_1beeb0(container, descriptor, allocator)`.

## OBS (checker-verified, root replay)

Body `0x1beeb0..0x1befcc` = 284 B, sha256 `9a05e72e…`; entry contract from the `0x1bde30`
remarshal: `rdi = &container[0x40]`, `rsi = same`, `rdx = &descriptor`.

Order of effects: `operator new(0x60)` via thunk `0x17090`; `node[0x50] = 0x1598ce0`;
`node[0x40] = 0`; `node[0x58] = 0`; `vmovups` 0x20 B copy to `node+0x10`; if `fn != 0`
(`descriptor[0x18]`) then `node[0x10] = &node[0x10]`; `node[0x58] = descriptor[0x48]`; list-3
push-front after the sentinel; `++container[0x50]`; canary check against `0x15fe518` with fail
edge to `0xf8f3a0`.

**Correction to E522:** `node[0x58]` receives descriptor `+0x48`, not `+0x40`; descriptor `+0x40`
is the vtable pointer used by the copy/move/destroy vcalls and is never stored into the node.

## NOT modelled

The copy/move/destroy vcall sequence through table `0x1594060` / node vtable `0x1598ce0`
(slots `0x19a0`/`0x19c0`/`0x19e0`/`0x19f0`); the canary fail path (read-only guard); the owning
constructor/destructor/cleanup `0x1bdb10`/`0x1bdae0`/`0x1bdeb0`.

## Verification (integrator replay)

```
cmake -S native -B local/build/orch-r5 -G Ninja   -> exit 0
cmake --build local/build/orch-r5                 -> exit 0
ctest --test-dir local/build/orch-r5              -> 100% tests passed out of 328
python3 -I tools/lift/check_finsert_1beeb0.py     -> OK (after fixing a 5-vs-6-byte vmovups read)
```

## Next exact action

Make `init_1290` drive this body for its `insert_1bde30` effect (a real container + allocator
seam instead of a hook), then lift `0x21cd0` and the owners created by `0x533100`/`0x2569d0`.
