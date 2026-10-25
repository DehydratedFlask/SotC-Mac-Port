# Checkpoint — owner destructor `0x1bdae0` lifted

- Local: 2026-10-23 04:55 CDT (America/Chicago)
- Agent: integrator
- Task: `CPU-01-OWNER-DTOR-1BDAE0-NATIVE-01`
- Gate: **G1 PARTIAL — holder semantics exact; callees are contracts**
- Evidence: E551

## What was built

`native/core/owner_dtor_1bdae0.{hpp,cpp}` + `native/tests/test_owner_dtor_1bdae0.cpp` (335th test)
+ `tools/lift/check_fowner_dtor_1bdae0.py`. This is the teardown half of the singleton owners in
the `0x1bdb10`/`0x1bdae0` family, registered with `__cxa_atexit` by the installer (`0x1c00e0`).

## OBS (checker-verified)

Body `0x1bdae0..0x1bdb0e` = 46 B, sha256 `9c9bbed48a95…`, `ret` at `0x1bdb0d`.
`mov r14,rdi` → `rbx = [r14]` → `test rbx,rbx; je` (null path returns without doing anything) →
`call 0x1bede0(owned)` → `call 0x170b0(owned)` → `[r14] = 0` → epilogue. Both call targets were
re-derived from the rel32 bytes by the integrator.

## Model

`owner_dtor_1bdae0(void** slot, hooks)` reports `was_empty` for the null path, and otherwise runs
`destroy`, then `deallocate`, then clears the slot. A non-null slot with a missing hook is a
visible failure and the slot is left untouched (a missing free after a successful destroy also
leaves the slot set and `ok == false`), so nothing can look like successful teardown when it was
not.

## NOT modelled

Bodies of `0x1bede0` (object destruction) and `0x170b0` (operator delete); the `__cxa_atexit`
registration; the object's type.

## Verification

```
python3 -I tools/lift/check_fowner_dtor_1bdae0.py -> OK
ctest --test-dir local/build/orch-r15            -> 100% tests passed out of 335
tools/lift/run_all_checkers.sh                   -> 415 passed, 20 skipped, 0 failed
```

## Next exact action

The list-clearing destructor `0x1bdeb0` (120 B) walks a ring and destroys/frees nodes through
`0xf590`/`0x170c0`; lifting it completes container teardown. Otherwise the owner installer
(`0x2569d0`) or the `0xa54920` registrar.
