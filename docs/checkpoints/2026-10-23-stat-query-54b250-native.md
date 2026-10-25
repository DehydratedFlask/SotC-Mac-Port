# Checkpoint — native lift of stat-query wrapper `0x54b250`

- Local: 2026-10-23 04:05 CDT (America/Chicago)
- Agent: orchestrator (integrator)
- Task: `CPU-01-STAT-QUERY-54B250-NATIVE-01`
- Gate: **G1 PARTIAL — wrapper is native; the engine is a contract**
- Evidence: E547

## What was built

`native/core/stat_query_54b250.{hpp,cpp}` + `native/tests/test_stat_query_54b250.cpp` (333rd test)
+ `tools/lift/check_fstat_query_54b250.py`, wired into batch E so Init's tail dispatch
(`0x532b09`) executes the lifted wrapper when a `StatsEngine54b250` contract is supplied.

## OBS (checker-verified)

Body `0x54b250..0x54b2c7` = **119 B**, sha256 `40814086ed16f396…d39f`. Sequence: `and rsp,-0x20`,
`sub rsp,0x60`; guard load `0x54b25f → 0x15fe518`; traits lea `0x54b26a → 0x15c9920`; zero the
0x60 B descriptor through `+0x20`, `+0x30 = 0`, `+0x40 = 0x15c9920`, `+0x48 = 0`;
`call 0xa56560(this, esi, edx, &descriptor, 0, 0)`; then `mov rax,[rsp+0x40]`,
`mov rdi,[rsp+0x20]`, `call *0x18(%rax)` → traits slot 3 = `0x538200`, a bare `ret`; canary
compare; epilogue/`ret`. Return type `void` — the query result stays in the discarded descriptor.

## Model

The wrapper is executed natively: descriptor fields are built and exposed, the engine is called
through an explicit contract with the exact Init call shape `(this, 0, 2, &descriptor, 0, 0)`, the
provable no-op dispatch is counted, and a missing **or failing** engine makes `ok == false` rather
than silently succeeding.

## NOT modelled

The engine body `0xa56560`; the descriptor's internal layout beyond the fields this function
writes; whether the discarded result matters to any caller; the canary fail path.

## Verification

```
cmake -S native -B local/build/orch-r13 -G Ninja  -> exit 0
cmake --build local/build/orch-r13                -> exit 0
ctest --test-dir local/build/orch-r13             -> 100% tests passed out of 333
python3 -I tools/lift/check_fstat_query_54b250.py -> OK (sha, engine/traits/guard, slot 3 ret)
tools/lift/run_all_checkers.sh                    -> 413 passed, 20 skipped, 0 failed
```

## Next exact action

Lift `0xa56560` (0xd8-byte frame, vtable dispatch) or contract it with visible status, and then the
owner singletons from `0x533100`/`0x2569d0` so the tail objects exist natively.
