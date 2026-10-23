# Checkpoint — `0x1bdeb0` decoded, but deliberately not lifted yet

- Local: 2026-10-23 05:55 CDT (America/Chicago)
- Agent: integrator (static decoding)
- Gate: **G1 PARTIAL — analysis only; no code changed**
- Evidence: E555

## Decoded loop (byte-exact)

```
1bdeb0  push rbp; mov rbp,rsp; push r15/r14/rbx; push rax
1bdeba  r14 = rdi                      ; the list head object / sentinel
1bdebd  r15 = [r14]                    ; first link
1bdec0  jmp 1bded3
1bded0  r15 = [r15]                    ; advance
1bded3  cmp r15,r14 ; je end
1bded8  cmp dword [r15+0x50],0
1bdedd  jne 1bded0                     ; skip links whose +0x50 dword is non-zero
1bdedf  r15 = [r15]                    ; advance to next
1bdee2  rbx = [r15+8]                  ; rbx = next->prev  == the link to remove
1bdee6  rax = [rbx]; rcx = [rbx+8]     ; node->next, node->prev
1bdef1  [rax+8] = rcx                  ; next->prev = prev
1bdef9  [rcx]   = rax                  ; prev->next = next
1bdefc  call 0xf590(node+0x30)         ; release the callable at +0x30
1bdf01  call 0xf590(node+0x10)         ; release the callable at +0x10
1bdf0f  call 0x170c0(node)             ; tagged free, tag 2
1bdf17  decq [r14+0x10]                ; count--
1bdf1b  jmp 1bded3
```

So it is a classic erase-while-iterating destructor: `next->prev` recovers the node to erase, the
two callables at `+0x10` and `+0x30` are released through the shared-ownership release `0xf590`
(E552), the node is freed with the tag-2 wrapper `0x170c0` (E553), and the count at `[head+0x10]`
decrements.

## Why I stopped short of lifting it

The node layout conflicts with the insert's node. Insert nodes (`0x1beeb0`) keep the callable value
at `+0x40` and its **vtable pointer** at `+0x50`, whereas this loop tests a **dword flag at
`+0x50`** and only destroys links where it is zero. Either:

1. this list holds different node types (flag-bearing), and the insert's nodes would all be skipped
   by this destructor — plausible if callable-bearing nodes are owned elsewhere; or
2. the two containers are unrelated and the shared `0x1bdb10` family name misled the earlier
   grouping.

Deciding between those changes the model completely, and implementing the wrong one would put a
plausible-looking destructor into `native/` that frees the wrong things. The next agent should
resolve the node type first (find the list-1/list-2 insert paths and what writes `+0x50` as a
dword).

## What is already safe to reuse

- `0xf590` is the shared-ownership release (E552) — the two calls here are release, not free.
- `0x170c0` is tag-2 deallocate (E553).
- `0x14ccb0` is the add side.

## Next exact action

Find the code that inserts into this list and the writer of `[node+0x50]` as a dword; only then
lift `0x1bdeb0` over an explicitly typed node. If the writer is a flag (not a vtable), the model is
simple: skip non-zero, erase, release two callables, free tag 2, decrement.
