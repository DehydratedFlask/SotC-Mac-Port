# E1057 — SAV-01 load-dispatch `0x5435fc` target origin resolved

**Date:** 2026-11-01. **Agent:** subagent (SAV-01-LOAD-DISPATCH-5435FC-01 worker). **Task:**
`SAV-01-LOAD-DISPATCH-5435FC-01`. **Status:** `STATIC_ANALYSIS_VERIFIED`; the target origin is
resolved, no runtime claim. **Failed semantic hypotheses:** 0. **Failed provider queries:** 0
(no REA/Ghidra request was issued; the E996/E998 stall path was deliberately not retried).

## Result in one line

`callq *0x10(%rax)` at `0x5435fc` targets **`0x517220`** — the `_M_dispose` slot of the
`shared_ptr` control block that `0x515770` copies out of the resource map — and the companion
`callq *0x18(%rax)` at `0x543611` targets **`0x517250`** (`_M_destroy`).

## Scope and artifacts

Read-only static analysis of the owner's pinned local ELF only. No executable code ran, no game
input was read, no product source, test, or native module changed. REA/Ghidra was not used.

| Input | Identity |
|---|---|
| `local/re/eboot.elf` | SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`, image base 0, 11 program headers |
| `local/validation/registry-analysis-agent/all.dis` | SHA-256 `9c1962541d929c167dec881a1f7093159bc5b156b71e873cd1bb3325dc107e83` |
| `local/validation/registry-analysis-agent/fde_ranges.json` | SHA-256 `f507ab6cdc67e12c07a03be2c51cf02e8fd5fb7e27d00c45f1817c15bd226dd9`, no duplicate keys |
| `scripts/ps4_dynlink.py` | program headers, `Image.read`, `parse()["relocs"]` (712,333 relocation rows) |
| `tools/lift/cfg_census.py` | `load_fde_ranges(...).find(addr) -> (start, size)` |

Private working extracts (not published): `local/validation/sav-5435fc/` holds
`fn_542880.dis`, `fn_515770.dis`, `fn_543f40.dis`, `dump.py` (window extractor) and
`survey_lookup.py` (call-site tag survey).

## Function extent

`FUN_00542880` = **`0x542880`, size `0x131c`, end `0x543b9c`** (FDE row `"0x542880": 0x131c`,
`fdes.find(0x5435fc) == (0x542880, 0x131c)`). Body SHA-256
`a2985de4bc806e87c89440718213ea0c291d0a76c36a6b9726cf0048ce7c12b9`. The body contains exactly
**two** indirect transfers, both in the same release block: `0x5435fc` and `0x543611`.

## Backward slice from `0x5435fc` (instruction by instruction)

Bytes are read from the pinned ELF, not from the text listing.

| Address | Bytes | Instruction | Note |
|---|---|---|---|
| `0x5435c8` | `48 8b 9c 24 98 01 00 00` | `movq 0x198(%rsp), %rbx` | control block enters `%rbx` |
| `0x5435d5` | `48 85 db` | `testq %rbx, %rbx` | |
| `0x5435d8` | `74 3a` | `je 0x543614` | null (map miss) skips the whole release |
| `0x5435e5` | `f0` | `lock` | |
| `0x5435e6` | `0f c1 43 08` | `xaddl %eax, 0x8(%rbx)` | `%eax = -1`; `+0x8` is the strong count |
| `0x5435ea` | `ff c8` | `decl %eax` | new count |
| `0x5435ee` | `7e 06` | `jle 0x5435f6` | release only when the strong count reaches 0 |
| `0x5435f6` | `48 8b 03` | `movq (%rbx), %rax` | **`%rax` = control-block vptr** |
| `0x5435f9` | `48 89 df` | `movq %rbx, %rdi` | `this` = control block |
| `0x5435fc` | `ff 50 10` | `callq *0x10(%rax)` | **slot `+0x10` of the control-block vtable** |
| `0x5435ff` | `f0 44 0f c1 73 0c` | `lock xaddl %r14d, 0xc(%rbx)` | `+0xc` is the weak count |
| `0x543611` | `ff 50 18` | `callq *0x18(%rax)` | second call, slot `+0x18`, same vtable |

`%rbx` is **not** an argument spill and **not** a global: a whole-function scan finds exactly one
occurrence of the literal `0x198(%rsp)` — the load above — so no instruction in `0x542880` stores
that slot directly. The slot is the second word of a 16-byte `shared_ptr` returned through an sret
pointer:

| Address | Bytes | Instruction | Note |
|---|---|---|---|
| `0x543094` | `48 8d bc 24 90 01 00 00` | `leaq 0x190(%rsp), %rdi` | sret destination |
| `0x5430bf` | `e8 ac 26 fd ff` | `callq 0x515770` | rel32 verified: `0x5430c4 - 0x2d954` |
| `0x515866` | `4d 89 27` | `movq %r12, (%r15)` | sret[0] = object pointer (hit path) |
| `0x515869` | `49 89 5f 08` | `movq %rbx, 0x8(%r15)` | **sret[1] = control block** (hit path) |
| `0x515824` | `49 c7 47 08 00 00 00 00` | `movq $0x0, 0x8(%r15)` | miss path: empty `shared_ptr` |

Every path into the release block runs `0x5430bf` first: the only entries into the region
`[0x54349e, 0x5435c8)` from outside it are `0x543180 je`, `0x543189 je` and `0x5431e5 jne`, all
downstream of that call; every other branch in the region has its source inside it, and the body
has no indirect jump. `0x5435c8` is reachable only through `0x5435c1`.

`0x515770` itself (`0x515770`, size `0x183`) is a resource-map accessor:

| Address | Bytes | Instruction | Note |
|---|---|---|---|
| `0x5157a1` | `e8 0a c8 c4 ff` | `callq 0x161fb0` | FNV-1a-form name hash (seed `0x811c9dc5` at `0x515791`, prime `0x1000193`) |
| `0x5157b2` | `48 8d 0d 77 0a 00 00` | `leaq 0x516230(%rip), %rcx` | factory, stored into descriptor[+0x18] |
| `0x5157bd` | `be 43 4c 43 54` | `movl $0x54434c43, %esi` | namespace tag `'CLCT'` |
| `0x5157c4` | `4c 89 45 a0` | `movq %r8, -0x60(%rbp)` | descriptor self pointer |
| `0x5157cd` | `48 89 4d b8` | `movq %rcx, -0x48(%rbp)` | descriptor[+0x18] = `0x516230` |
| `0x5157d4` | `e8 b7 c4 55 00` | `callq 0xa71c90` | find-or-create; rel32 verified |

The descriptor at `rbp-0x60` is `{self = &descriptor, invoke = *(0x15feda0), adjust = 0,
factory = 0x516230}`. `0x15feda0` holds `0x2193d0` with an `R_X86_64_RELATIVE` row of the same
value, and is never stored to (every reference in the disassembly is a `vmovq` load).

`0xa71c90` (`0xa71c90`, size `0x370`) is the map find-or-create. On a miss it invokes the
descriptor at `0xa71e83` (`callq *%rax`, `%rax = descriptor[+0x08]`), then allocates a 0x38-byte
node and inserts through its private helper at `0xa71f8a`. The trampoline `0x2193d0`
(`48 89 f0 … ff 50 18`) forwards to `descriptor[+0x18]`, i.e. **`0x516230`**.

The factory `0x516230` (`0x516230`, size `0xd8`) builds the resource and its control block:

| Address | Bytes | Instruction | Note |
|---|---|---|---|
| `0x51624f` | `48 8d 05 …` | `leaq 0x15c6f30(%rip), %rax` + `addq $0x10` | resource vptr = `0x15c6f40` |
| `0x5162c1` | `bf 20 00 00 00` | `movl $0x20, %edi` | control-block size |
| `0x5162d9` | `48 8d 0d 78 0c 0b 01` | `leaq 0x15c6f58(%rip), %rcx` | control-block vtable base |
| `0x5162e0` | `c7 40 08 01 00 00 00` | `movl $0x1, 0x8(%rax)` | strong = 1 |
| `0x5162eb` | `c7 40 0c 01 00 00 00` | `movl $0x1, 0xc(%rax)` | weak = 1 |
| `0x5162f5` | `48 83 c1 10` | `addq $0x10, %rcx` | address point `0x15c6f68` |
| `0x5162f9` | `48 89 08` | `movq %rcx, (%rax)` | **control-block vptr = `0x15c6f68`** |
| `0x5162fc` | `48 89 58 10` | `movq %rbx, 0x10(%rax)` | owned pointer at `+0x10` |

Therefore `%rax` at `0x5435fc` is `0x15c6f68`, and the call reads `*(0x15c6f68 + 0x10)`.

## The chain, with the image bytes that pin each link

1. `0x5435c8` loads the control block from `[rsp+0x198]`.
2. That slot is sret[1] of `0x515770` (`0x515869` / `0x515824`), called at `0x5430bf` with
   `rdi = &0x190(%rsp)` (`0x543094`).
3. `0x515770` obtains the value from the resource map: `0xa71c90` with tag `'CLCT'` and the
   name hash (`0x5157d4`), passing the factory descriptor whose `[+0x18]` is `0x516230`.
4. `0xa71c90` calls `descriptor[+0x08]` on a miss (`0xa71e83`); that slot is `*(0x15feda0) =
   0x2193d0` (`R_X86_64_RELATIVE` at `0x15feda0`, addend `0x2193d0`).
5. `0x2193d0` forwards to `descriptor[+0x18]` (`0x2193df`, `ff 50 18`) = `0x516230`.
6. `0x516230` allocates 0x20 B and installs vptr `0x15c6f68` (`0x5162d9` + `0x5162f5` +
   `0x5162f9`), strong/weak counts at `+0x8`/`+0xc`, owner at `+0x10`.
7. The vtable word at **`0x15c6f78`** (= `0x15c6f68 + 0x10`) is `0x517220`, carried by
   `R_X86_64_RELATIVE` row `target=0x15c6f78, addend=0x517220`.
8. `0x517220` is an FDE start, size `0x27`, body
   `55 48 89 e5 53 50 48 89 fb 48 8b 7b 10 48 85 ff 74 06 48 8b 07 ff 50 08 48 c7 43 10 00 00
   00 00 48 83 c4 08 5b 5d c3` (SHA-256 `073bb1e4…66ef9a`): load the owner pointer from `+0x10`,
   if non-null call the owner's vtable `+0x8` (its deleting destructor), then clear `+0x10`.

Companion site `0x543611` reads the same vtable at `+0x18` = **`0x517250`** (row
`target=0x15c6f80, addend=0x517250`; FDE start, size `0x28`, body SHA-256
`9461adfc…eefa62`), which destroys the control block and frees it.

The looked-up name is the save chunk name: the key literal at `0x1017142` is
`"SecretRoomCollectibles\0"` (22 B, `movq $0x16` length at `0x54319c`), and the same string is
used by the loader's chunk reads at `0x543385`/`0x5433f8` and by the writer's chunk write at
`0x544942`.

## Why the target is single-valued (namespace uniqueness)

The map is keyed by `(tag, name_hash)` and only ever written by the find-or-create path:

- the private insert helper `0xa721c0` (`0xa721c0`, 463 B) has exactly **one** `e8` call site in
  the image: `0xa71f8a`, inside `0xa71c90`;
- the tag immediate `be 43 4c 43 54` occurs exactly twice in executable bytes: `0x5157bd`
  (find-or-create, this accessor) and `0x516126` (the read-only sibling `0xa72000`, called from
  `0x516130`), which never inserts;
- the map header at `pool+0x102100`/`+0x102108` is referenced only from `0xa66dc0`
  (construct), `0xa671e0` (teardown), `0xa67300` (iteration) and the two accessors `0xa71c90` /
  `0xa72000`, so no other function inlines an insertion;
- the control-block vtable base `0x15c6f58` is `lea`'d exactly once in the whole image, at
  `0x5162d9`;
- `0x15feda0` (the descriptor's invoke slot) is only ever loaded, never stored, so the factory
  cannot be redirected at runtime.

So any `'CLCT'` entry that exists in that map was created by `0x516230` and carries vtable
`0x15c6f68`. Entries created by the accessor for other names are irrelevant: the factory is
hard-coded per call site, not per name.

## Verification (three independent anchors)

| Anchor | Evidence |
|---|---|
| Defining instruction bytes | `0x5435c8`, `0x5435f6`, `0x5435f9`, `0x5435fc`, `0x5430bf`, `0x543094`, `0x515869`, `0x5157b2`, `0x5157bd`, `0x5157cd`, `0x5157d4`, `0x2193df`, `0x5162d9`, `0x5162f5`, `0x5162f9`, `0x5162fc` all byte-match the pinned ELF (checker step 2/4/6) |
| Slot/global bytes + RELA | `0x15c6f78 → 0x517220`, `0x15c6f80 → 0x517250`, `0x15c6f68` address point from the factory, `0x15feda0 → 0x2193d0`; all `R_X86_64_RELATIVE` and all consistent with the raw qwords |
| Target is a function | `fdes.find(0x517220) == (0x517220, 0x27)` and `fdes.find(0x517250) == (0x517250, 0x28)`; both bodies hash-pinned |

Checker (new, published): `tools/lift/check_sav_load_dispatch.py`.

```sh
python3 -I tools/lift/check_sav_load_dispatch.py
```

Result: exit 0, `OK check_sav_load_dispatch: FUN_00542880 0x542880+0x131c; 0x5435fc call *0x10
-> 0x517220 (_M_dispose, 39 B); 0x543611 call *0x18 -> 0x517250 (_M_destroy)`. The checker skips
cleanly (exit 0, `SKIP …`) when the private image is absent, and its negative paths were exercised
with throwaway copies: a flipped vtable word fails the slot/addend comparison, and a flipped call
byte fails the loader body digest.

## Observations versus inference

Observed (byte-pinned): every address, displacement, relocation row, FDE extent and body digest
above; the 16-byte sret `shared_ptr` shape; the strong/weak counter protocol at `+0x8`/`+0xc`; the
two vtable slots `+0x10`/`+0x18`; the single inserting `'CLCT'` site; the missing `0x198(%rsp)`
store.

Inference (not proven by symbols — the image is stripped): the ABI names `_M_dispose` /
`_M_destroy` and the libstdc++-shaped `_Sp_counted_ptr` reading. They follow from the counter
layout, the dispose→destroy protocol, and the `delete this` body at `0x517250`, but the runtime
class identity is not recoverable from the image. The resolved **address** does not depend on
this naming.

Open, non-blocking for the target: whether the map entry exists in a given session (if it does
not, `0x515770` returns an empty `shared_ptr`, `0x5435d5` skips the block and the call never
executes); and the C++ identity of the 0xa8-byte resource whose vptr is `0x15c6f40`.

## What this does not claim

No runtime execution, no reachability/order claim, no save or load behaviour, no transaction
claim, no native lift, no game claim. `SAV-01` stays partial and `SAV-04` remains `NOT RUN`. The
`_M_dispose` body performs a further indirect call through the resource's own vtable `+0x8`
(`0x517232`), which resolves to `0x516a20` for the factory-created object — that is a second hop
and is not part of this task's target-origin claim.

## Next exact action

Close `SAV-01-LOAD-DISPATCH-5435FC-01` as `STATIC_ANALYSIS_VERIFIED` in `docs/TASKS.md`; the
parent `SAV-01-LOAD-PARTIAL-MUTATION-01` "two unresolved indirect calls" note (E866) is now
superseded for `FUN_00542880`, which has no indirect call left unresolved.
