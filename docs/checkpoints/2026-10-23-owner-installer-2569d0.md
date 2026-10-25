# Checkpoint — 0x2569d0 frame/ABI mapped and the registry-slot block lifted

- Local: 2026-10-23 08:1x (America/Chicago)
- Agents: static-RE worker; integrator
- Task: `CPU-01-OWNER-INSTALLER-2569D0-01`
- Gate: **G1 PARTIAL — mapping complete for the registry block; the rest is not lifted**
- Evidence: E567

## Extent and frame

- Body `0x2569d0..0x259b1c` (ret) = **12,621 B**, plus a 5 B guard-fail tail to `0x259b22`
  (12,626 B), 14 B nop pad, next function `0x259b30`. sha256 `4ce6b5d8…e471` through the ret,
  `6ef4eff8…a898` including the tail.
- Prologue `55 48 89 e5 41 57 41 56 41 55 41 54 53 48 81 ec 18 11 00 00`: five saved registers and
  a **verified `sub rsp,0x1118`** frame (`0x1140` below `rbp`, `rsp % 16 == 0` at calls).
- Canary `[rbp-0x30]` from `0x15fe518`, compare `0x259b05`, fail → `0xf8f3a0`. 540 distinct
  `[rbp-N]` slots, deepest `rbp-0x1134`.
- ABI: `rdi` = app object (vptr inferred `0x15c8cd8`), `rsi` = an argument only consumed by
  `call [this+0x48]` at `0x256a0d`, return `void`.
- 2,142 instructions, **87 blocks all reachable**, no jump table/indirect jump; 369 calls
  (359 direct to 73 targets, 10 indirect); 38 `je`, 4 `jne`.

## Registry block `0x256f98..0x257021`

```
256f98  mov edi,0x60 ; call 0x17070          ; allocate 0x60
256fa5  mov rdi,r12  ; call 0x1bdb10         ; construct container
256fad  lea r13,[0x2ee68c8]
256fb4  rbx = [r13] ; cmp rbx,r12 ; je skip
256fc5  call 0x1bede0                         ; destroy previous container
256fcd  call 0x170b0                          ; free previous storage
256fd2  [r13] = r12                           ; *(0x2ee68c8) = new container
256fd6  vmovq xmm0,[0x15ff080]                ; slot 4 = 0x25a4d0
256fe5  descriptor at rbp-0x80 = {ctx=app, invoke=0x25a4d0, adjust=0, fn=0}
257015  call 0x1bde30                         ; insert into list-3
257021  call [0x159c908+0x18] = 0x19f0        ; bare ret (temp destroy)
```

The other three callback installs in the same function are
`0x256edc → 0x42390 {app,0x259b30}`, `0x256f18 → 0x116540([0x2edf140]) {*(0x2dfd4b8),0x2662b0}` and
`0x256f52 → 0x116040(…, edx=1) {…,0x267770}`; they are not registry list-3 delegates.

## Ordering

`0x531d60` → `0x255130` at `0x531dda` → `[vt+0x40]` = `0x533100` → `0x2569d0`; back in Init,
`0x1290` runs at `0x531e03`. The container therefore exists before `0x1290` dereferences it (the
E522 caveat is closed), although the original code has no null guard. `0x2dfcbc8` is **not**
referenced by `0x2569d0`.

## Native work landed

`native/core/registry_slot.{hpp,cpp}` models `0x256f98..0x256fd2`: create the replacement
`RegistryOwner` first, tear the previous one down through the lifted `container_dtor_1bede0`, then
replace the slot. `native/core/registry_owner.{hpp,cpp}` models the list-3 registration and the
byte-derived FIFO dispatch; `app_info_install` supplies the two native delegate adapters. Fixtures
`registry_slot` and `registry_owner` assert the replace order, freed-node counts, the stored node key
`{receiver, native adapter, adjust=0, fn=0}` and the five-entry install.

## Verification

- Private evidence: `local/re/evidence/owner-installer-2569d0/` (`report.md` 671 lines,
  `summary.json`, 87-block/73-target/10-indirect/42-branch/36-rip-ref/19-store tables).
- Full-suite and sweep counts are in the round verification checkpoint.

## Limits

The remaining 19 constructor bodies and their virtual destructors, the `[this+0x48]`/`[this+0x58]`
bodies (`0xb111e0`/`0xb11210`), the `0x25a4d0` walker, gate/mode runtime values and the `0xac5260`
property semantics are **not lifted**. `0x2569d0` itself is not a native function; only its
registry block is modelled. **G1 PARTIAL.**
