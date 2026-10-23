# Checkpoint — the insert's "copy/move/destroy" is three 20-byte helpers (analysis)

- Local: 2026-10-23 04:25 CDT (America/Chicago)
- Agent: integrator (static analysis of an already-lifted function)
- Task: `CPU-01-INSERT-CALLABLE-HELPERS-ANALYSIS-01`
- Gate: **G1 PARTIAL — analysis only; no new native code**
- Evidence: E548

## Why

`insert_1beeb0` (E531) leaves one part unmodelled: a five-call sequence through
`descriptor[0x40]`, the node's vtable and two stack temporaries, historically described as
"copy/move/destroy vcalls". That phrase hides how small the work actually is.

## OBS (byte-verified)

- `descriptor[0x40]` is the **same table** as the node's vtable: `0x1594060` and `0x1598ce0` both
  hold `{0x19a0, 0x19c0, 0x19e0, 0x19f0}` (re-read from the ELF; four `R_X86_64_RELATIVE` slots).
- The four targets are trivial:
  - `0x19a0..0x19b4` (20 B): `mov rax,[rsi+0x10]; lea rcx,[0x1594060]; mov [rdi+0x10],rax; mov [rdi+0x20],rcx; ret` — copy value, set vtable.
  - `0x19c0..0x19d4` (20 B): `lea rax,[0x1594060]; mov [rdi+0x20],rax; mov rax,[rsi+0x10]; mov [rdi+0x10],rax; ret` — set vtable first, then copy value.
  - `0x19e0..0x19e9` (9 B): `mov rax,[rsi+0x10]; mov [rdi+0x10],rax; ret` — copy value only.
  - `0x19f0`: one-byte `ret`.
- Call sites inside `0x1beeb0` (from the listing):
  `0x1bef44 call *0x8(%rax)` with `rax = descriptor[0x40]`, `rdi = temp1 (rsp-0x88)`,
  `rsi = &descriptor[0x20]`;
  `0x1bef55`, `0x1bef68`, `0x1bef74`, `0x1bef7a/0x1bef8d` then chain the temporaries and the node
  storage through slot 0 and finally `*0x18(%rax)` (slot 3, the `ret`).
- Node offsets line up with the helpers' `+0x10`/`+0x20` fields when the destination is
  `node + 0x30`: value at `node+0x40`, vtable at `node+0x50` — exactly the two fields the insert
  already writes (0 and `0x1598ce0`).

## INF

The descriptor's `+0x20` context is a **type-erased callable** with shared copy/move/destroy
helpers (a small-buffer `std::function`-shaped object). The insert copy-constructs that callable
into `node+0x30` using the same table, which is why `node[0x50]` is initialised to `0x1598ce0`.

## Consequence for the lift

This is not a mystery sequence: the follow-up is to model a `CallableStorage { value; vtable; }`
with the three trivial helpers and drive the five call sites through it. The open question is
only *which* word of `descriptor[0x20..0x40)` is the source value (`[rsi+0x10]` with
`rsi = &descriptor[0x20]` implies `descriptor+0x30`), and whether the two temporaries add an
observable effect beyond the final `node+0x30` contents.

## Next exact action

Lift `0x19a0`/`0x19c0`/`0x19e0` as a `callable_storage` module, then replace the unmodelled
comment in `insert_1beeb0` with the real sequence and assert `node+0x30/+0x40/+0x50` contents.
