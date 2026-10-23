# Checkpoint — shared-ownership release `0xf590` lifted

- Local: 2026-10-23 05:15 CDT (America/Chicago)
- Agent: integrator
- Task: `CPU-01-SHARED-RELEASE-F590-NATIVE-01`
- Gate: **G1 PARTIAL — refcount protocol exact; destroy/free are contracts**
- Evidence: E552

## Why this one matters

`call 0xf590` appears across almost every recovered Init slice as a hook. E522 already noted the
node's `+0x58` slot is destroyed through it. It turns out to be a **shared-ownership release**, not
an opaque destructor.

## OBS (checker-verified)

Body `0xf590..0xf5d9` = 73 B, sha256 `55eef47ed634…`. Sequence:
`mov rbx,rdi`; `mov rax,[rbx+0x18]`; `test rax,rax; je ret` (no control block → no-op);
`mov rcx,-1`; `lock xadd rcx,[rax]` (atomic decrement, rcx = previous);
`cmp rcx,1; jne ret` (still referenced);
`mov rdi,[rbx]; xor esi,esi; call 0x12af0` (destroy pointee);
`mov rdi,[rbx+0x18]; test; je ret`; `jmp 0x170b0` (free the control block).
The add side is `0x14ccb0` (already lifted as `atomic_add_ref_14ccb0`).

## Model

`shared_release_f590(holder, hooks)` performs a **real** `std::atomic` decrement (so concurrent
behaviour matches), returns `previous`, and only destroys/frees when the previous count was 1.
A missing destroy or free hook yields `ok == false` rather than claiming a release; the atomic
decrement still happens exactly as the original does, and the fixture asserts that.

## NOT modelled

Bodies of `0x12af0` (pointee destruction) and `0x170b0` (operator delete); the holder's object
layout beyond `[+0x00]` and `[+0x18]`; weak references (none observed).

## Verification

```
python3 -I tools/lift/check_fshared_release_f590.py -> OK
ctest --test-dir local/build/orch-r16              -> 100% tests passed out of 336
tools/lift/run_all_checkers.sh                     -> 416 passed, 20 skipped, 0 failed
```

## Next exact action

Wire `shared_release_f590` into the modules that currently take a `call_f590` hook (continue B/C/E,
the `0x533100` attach paths) so refcount release is executed instead of simulated, and pair it
with `0x14ccb0` for the acquire side.
