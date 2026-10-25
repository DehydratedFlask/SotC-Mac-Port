# Checkpoint — `0x1bdeb0` blocker resolved: two node types, not one

- Local: 2026-10-23 (America/Chicago)
- Agent: integrator
- Gate: **G1 PARTIAL — analysis only; the lift is now unblocked**
- Evidence: E558 (resolves E555, amends E557)

## How the blocker was broken

E555 stopped because `[node+0x50]` is a **dword flag** in the `0x1bdeb0` loop but the insert node keeps
its callable **vtable pointer** there. Two findings settle it:

1. **`0x1bdeb0` has exactly one caller**: `0x25dc76`, immediately after
   `lea rax,[0x2ee68c8]; rdi = *(rax)` — i.e. it clears the list of **`*(0x2ee68c8)`**, the same
   singleton `0x1290` passes as the A-insert owner (E522/E527). The call is preceded at `0x25dc4a`
   by `call 0xf590` on a stack temporary. (Found by a bulk `E8 rel32` scan of the executable
   segment; there are no `E9` tail jumps to it.)
2. **`0x1bdb50` is the inserter for that list and builds 0x58-byte nodes**:
   `movl $0x58,%edi; call 0x17090`, then `vmovups` 0x20 B to `+0x10`, `vmovups` 0x20 B to `+0x30`,
   **`movl %r14d,0x50(%rax)`** (the dword flag), then link with `node[0x00] = sentinel`,
   `node[0x08] = sentinel->next`, `old->prev = node`, `sentinel->next = node`.

So the container holds **two different node types**: 0x58-byte flag nodes (inserted by `0x1bdb50`)
and 0x60-byte callable nodes (inserted by `0x1beeb0` into `container+0x40`). The `+0x50` test in
`0x1bdeb0` discriminates them: it clears the 0x58 nodes and skips anything whose `+0x50` is a
non-zero vtable pointer.

## Corrected loop reading

`sentinel = *(0x2ee68c8)`; `r15 = sentinel->prev`; walk backwards: skip when `dword [r15+0x50] != 0`,
otherwise advance `r15 = r15->prev`, recover the node as `rbx = [r15+0x08]` (`prev->next`), unlink
(`[rax+0x08] = rcx`, `[rcx] = rax`), `call 0xf590(node+0x30)`, `call 0xf590(node+0x10)`,
`call 0x170c0(node)` (tag 2), `decq [sentinel+0x10]`.

## E557 amendment

E557 concluded the control-block count is "seeded at construction and only decremented". That
conclusion is **too strong** and is withdrawn: acquire sites do exist, but as **indirect**
`lock incq (%reg)` on the control block — 33 such sites in the executable segment, including
`0x1bdb9d` and `0x1bdbce` inside `0x1bdb50`, which increment the control blocks of the two callables
they capture. The narrower part of E557 stands: `0x14ccb0` is *not* the add side (32-bit field at
`+0x30`, wrong width and offset), and the pairing claim stays withdrawn.

## Next exact action

Lift `0x1bdeb0` over an explicitly typed 0x58-byte node (prev at `+0x00`, next at `+0x08`, payload
callables at `+0x10`/`+0x30`, dword flag at `+0x50`), releasing through `shared_release_f590` and
freeing through tag 2, with a fixture covering: empty list, all-flag-zero, mixed flags (skip
non-zero), and count decrement.
