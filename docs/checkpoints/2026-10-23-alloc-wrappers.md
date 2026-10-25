# Checkpoint — allocator wrappers `0x17070` / `0x170b0` / `0x170c0`

- Local: 2026-10-23 05:35 CDT (America/Chicago)
- Agent: integrator
- Task: `CPU-01-ALLOC-WRAPPERS-NATIVE-01`
- Gate: **G1 PARTIAL — wrappers exact; dispatch bodies are contracts**
- Evidence: E553

## What the bytes say

| Wrapper | Size | Bytes | Tail target |
| --- | --- | --- | --- |
| `0x17070` | 20 B | `mov esi,1; mov edx,8; mov ecx,1; jmp rel32` | `0x12820` (allocate) |
| `0x170b0` | 10 B | `mov esi,1; jmp rel32` | `0x12af0` (deallocate, tag 1) |
| `0x170c0` | 10 B | `mov esi,2; jmp rel32` | `0x12af0` (deallocate, tag 2) |

All three are pure tail wrappers: they set argument registers and jump. They explain why so many
recovered modules show `0x17070`/`0x170b0`/`0x170c0` as "allocator hooks".

## Correction to earlier wording

`0x12af0` was described in E552 as "destroy the object". It is the allocator's **tagged deallocate
dispatcher**: `0x170b0` and `0x170c0` reach the same function with tags 1 and 2, and
`shared_release_f590` calls it with tag 0. So a last-reference release frees the pointee's storage;
whatever destructor work exists happens inside the dispatcher and is not observable from these
bodies. The E552 checkpoint and the `shared_release_f590` header now say this explicitly.

## Model

`alloc_wrappers_17070.{hpp,cpp}` exposes `alloc_17070(size)`, `free_170b0(ptr)`, `free_170c0(ptr)`
over an `AllocDispatchContract` that receives the exact recovered arguments (size, tag 1, align 8,
flags 1) or the tag (1/2). A null pointer free is a no-op success — matching the dispatcher's own
null check — while a missing contract is a visible failure.

## NOT modelled

`0x12820` and `0x12af0` bodies (size-class bookkeeping, TLS state, logging); the meaning of each
tag beyond "owner kind / alignment class".

## Verification

```
python3 -I tools/lift/check_falloc_wrappers.py -> OK (bytes and tail targets)
ctest --test-dir local/build/orch-r17           -> 100% tests passed out of 337
```

## Next exact action

Wire these wrappers as the allocator seam for `insert_1beeb0` / `insert_21cd0` so their allocation
and free paths go through the recovered tag protocol instead of the current standalone seam.
