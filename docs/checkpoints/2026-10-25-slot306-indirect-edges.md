# Startup slot 306 -- indirect-call resolution and 196/198 direct-call reconciliation

Task `CPU-01-STARTUP-SLOT-306-INDIRECT-EDGES-01`, read-only static analysis by a worker. This note records
addresses, hashes, offsets and mnemonic-level facts only; no game bytes. Private evidence:
`local/re/evidence/startup-slot-306-indirect-20261025/` (manifest `SHA256SUMS`, SHA-256
`b0cf795325428dc7cba80ac1bdfd6d86ded26f8ff400177ddd58250bfa0972b1`). Nothing was executed, installed or
downloaded; the survey evidence directory was not modified; no REA/Ghidra session was opened (the
existing Ghidra export from the survey was used as an independent cross-check). The worker did not
edit `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `INSTALL_LOG.md`, `native/` or `launcher/`.
Labels: **OBS** = observed in pinned bytes/tool output; **INF** = inference.

## Identity (OBS)

- Pinned ELF `local/re/eboot.elf`, SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`
  (rechecked). Body `[0xa633b0,0xa654bd)` 8,461 B. A fresh `llvm-objdump -d` of `[0xa633b0,0xa654c0)` is
  byte-identical (SHA-256 `9dac2a5a...8165`) to the survey's listing: 1,568 instructions in the body, 198
  direct `call` sites, 11 indirect `call` sites, zero indirect `jmp`.

## The 11 indirect calls

All 11 are resolved. Each is a read of a `.data.rel.ro` slot (phdr 1, vaddr `0x1594000`) whose RELATIVE
relocation addend equals the raw file word (checked with `scripts/ps4_dynlink.py` `parse`), and each result
agrees with the survey's Ghidra export (`COMPUTED_CALL` target plus `READ` slot reference, 11/11).

| # | Call site | Operand | Where the target comes from | Slot | Target | Target body |
|---|-----------|---------|-----------------------------|------|--------|-------------|
| 1 | `0xa634b0` | `call [rax+0x18]` | rax = functor tag at `[rsp+0xa0]`, stored `0xa6344f` from r14 = `lea 0x15d1fa8` (`0xa63406`); rdi = `rsp+0x80` | `0x15d1fc0` | `0xa62490` | bare `ret` |
| 2 | `0xa63580` | `call [rax+0x18]` | same tag `0x15d1fa8` (store `0xa63524`) | `0x15d1fc0` | `0xa62490` | bare `ret` |
| 3 | `0xa6367e` | `call [rax+0x18]` | tag `0x15d2040` (rcx = lea `0xa635c3`, store `0xa6360c`) | `0x15d2058` | `0xc4f0` | bare `ret` |
| 4 | `0xa63de4` | `call [rax+0x18]` | tag `0x15d2040` (lea `0xa63d73`, store `0xa63d86`) | `0x15d2058` | `0xc4f0` | bare `ret` |
| 5 | `0xa640db` | `call [rax+0x18]` | tag `0x15d2080` (lea `0xa64073`, store `0xa64080`) | `0x15d2098` | `0xa62850` | bare `ret` |
| 6 | `0xa643ce` | `call [rax+0x18]` | tag `0x15d2040` (r15 = lea `0xa64310`, store `0xa6436a`) | `0x15d2058` | `0xc4f0` | bare `ret` |
| 7 | `0xa644ed` | `call [rax+0x18]` | tag `0x15d2040` (rdx = lea `0xa64437`, store `0xa6448a`) | `0x15d2058` | `0xc4f0` | bare `ret` |
| 8 | `0xa6460b` | `call [rax+0x18]` | tag `0x15d2040` (rcx = lea `0xa64554`, store `0xa645ab`) | `0x15d2058` | `0xc4f0` | bare `ret` |
| 9 | `0xa64725` | `call r14` | r14 = `[rax+0x20]` loaded `0xa64700`, rax = `lea 0x1594dc0` (`0xa646c1`); rdi = global `0x32f9f68` | `0x1594de0` | `0x20d60` | ConCmd register (guard byte `[rdi+0x18]`, then tail into `0x20d80`) |
| 10 | `0xa64811` | `call r14` | same r14 (callee-saved, not redefined); rdi = global `0x32fa008` | `0x1594de0` | `0x20d60` | same |
| 11 | `0xa648fd` | `call r14` | same r14; rdi = global `0x32fa0a8` | `0x1594de0` | `0x20d60` | same |

Status: sites 1-8 and 9-11 are **RESOLVED (STATIC_ANALYSIS_VERIFIED)**; none is UNKNOWN.

Observations behind the table:

- Sites 1-8 (OBS): the stack functor is a type-erased small callable at `rsp+0x80` (buffer, payload
  `+0x10`, tag `+0x20` at `rsp+0xa0`, aux `+0x28`). Each site is the compiler-emitted destroy of a
  temporary: reload tag, `rdi = rsp+0x80`, `call [tag+0x18]`. The four functor tables `0x15d1fa8`,
  `0x15d2040`, `0x15d2060`, `0x15d2080` are 4-slot tables (copy-construct, clone, move-payload, destroy);
  slot 3 of the three tables used here is a single `ret`. This matches the already recorded
  functor-destroy pattern in `native/core/concmd_vars.hpp` (`0xb0e240` bare `ret`). `0x15d2040` has the same
  four bodies as the table at `0x1594318` (its copy bodies store `0x1594318`, not `0x15d2040`); this does
  not affect slot 3. `0x15d2060` is not used by an indirect call (it is only stored into a global).
- Each tag has exactly two mentions of `[rsp+0xa0]` in the span (one store, one reload), the tag register
  is not redefined between its `lea` and the store, and the 0x50 B argument block that contains the functor is passed by pointer to
  `0xa624a0`, `0xa62860` or `0xc300` (stack argument) which only read it (bodies of `0xc300` and the
  `0xa624a0` wrapper plus worker `0xa62660` read; the clone slot writes only its destination). That
  read-only callee behaviour is **OBS for `0xc300`, `0xa624a0`, `0xa62660`; INF for `0xa62860`/`0xa62a20`**
  (same instruction shape, not read end to end).
- Sites 9-11 (OBS): same shape as the previously resolved `call [0x1594dc0+0x20]` in
  `native/core/owner_ctor_14db00.hpp`: vptr `0x1594dd0` entry 2 is `0x20d60`, the same function the body
  also calls directly 16 times. These three therefore add no new callee. INF: they register three more
  console-command globals (`0x32f9f68`, `0x32fa008`, `0x32fa0a8`), each preceded by a `0x20af0` base
  constructor call and an atexit registration of `0x4b0`.
- Callee/this context for sites 1-8 (OBS): preceding constructor call and global object are
  `0xa624a0` on `0x32f8ff8` and `0x32f90d0`; `0xc300` on `0x32f91a8`, `0x32f9810`, `0x32f9ce0`,
  `0x32f9db8`, `0x32f9e90`; `0xa62860` on `0x32f9a78`. (INF: `0xc300` is the known ranged-float variable
  constructor; `0xa624a0`/`0xa62860` are sibling instantiations with the same prototype.)

Residual (INF/UNKNOWN): the value of the tag at the call is the stored one only if the callees do not
write through the stack argument; `0xa62860`'s worker was not read in full. Semantics of the
ConVar/ConCmd objects themselves (names, defaults) are not covered here.

## 196 vs 198 direct calls

- Sets (OBS): LLVM linear decode = 198; prior Ghidra export = 198; they are identical sets. The
  reachability walker `tools/lift/analyze_slot_struct.py` (survey `body/analyze-slot306.json`) = 196. The
  walker has no extra call that the others lack.
- The two differing sites (OBS): `0xa637ae -> 0xf8f390` (`__cxa_atexit`; registers `0x20b0` for global
  `0x32f9280`) and `0xa643ad -> 0xc300` (constructor of the float variable global `0x32f9ce0`, i.e. the
  call that precedes indirect site 6). They are present in LLVM and in Ghidra and absent from the walker.
- Cause (OBS, replayed with the walker's own `build_report` under a redirected scratch directory):
  the walker disassembles in 0x400-byte windows (`decode_from` limit). Windows begin at `0xa633b0` and then
  at each next boundary (`0xa637b0`, `0xa63bb0`, `0xa63fb0`, `0xa643b0`, `0xa647b0`, `0xa64bb0`,
  `0xa64fb0`, `0xa653b0`). The 5-byte calls at `0xa637ae` and `0xa643ad` straddle the boundaries `0xa637b0`
  and `0xa643b0`; the window tail yields a 1-byte `<unknown>` row, the walker falls through by one byte
  and decodes a few misaligned instructions before resynchronising. Net effect: 1,582 walker-visited
  addresses = 1,568 true instructions - 3 skipped real instructions (`0xa637b3` lea, `0xa643b2`,
  `0xa643b6`) + 17 spurious addresses (five of them `<unknown>` rows). Only the two calls above are lost
  from `direct_calls`; no other real instruction is missed, and the walker's indirect list (11) is
  unaffected. INF: the walker's `reachable_instructions` (1,582), reference lists and any footprint derived
  from them should be treated as slightly contaminated; the `0xa637b3` RIP-relative `lea` of
  `0x32f9348` is absent from it. The LLVM/Ghidra 198 is the authoritative direct-call count.
- Recommendation (not done, outside scope): fix `instr_at` to retry on `<unknown>` rows or to overlap
  windows; do not edit `tools/lift` in this task.

## Commands (all exit 0; see private `commands.txt`)

`shasum -a 256 local/re/eboot.elf`; `xcrun llvm-objdump -d --start-address=0xa633b0 --stop-address=0xa654c0`;
`xcrun llvm-objdump -d --no-show-raw-insn` over the callee and table ranges named above;
`python3 -I -B replay_reachability.py`; `python3 -I -B resolve_indirect.py`; `trail grep` lookups.
Failed/blocked attempts (zsh `===` expansion, zsh word-split loop, a refused `rm`, two parser fixes) are
listed in `commands.txt`; none was retried a third time.

## Private evidence hashes (SHA-256)

- `resolve_indirect.json` `b7699cfb66bc04c84f7e8c16ef885eed446fd83ed06913181fb1147f85540c51`
- `resolve_indirect.py` `f39375421edfbc11eae4da0e08e6c248b868d57637c687c7e4d377517fbea53c`
- `replay_reachability.json` `fdc07bd23e29ed068db0bc7a13fe32bc968e8f603eaf8d12fcd518901d44ea99`
- `replay_reachability.py` `2b86d8e8b1c4662f8c6940700fbc70746d303dcec8a1687bbc78ae9757758dac`
- `llvm-objdump-slot306.dis` `9dac2a5a8ecc42b61e9169baf6575bfdc8abaf717c610db7a869c499dc2b8165`
- `callee-a624a0.dis` `350a012df5b94d1165ce37b8720e6b314961dd6a16a41089126e1e8ec7bdf992`; the other callee
  listings and the full list are in `SHA256SUMS` (42 files, including scratch copies of decode windows,
  which are private game bytes and must never be published).

## Status

Slot 306 indirect edges: **STATIC_ANALYSIS_VERIFIED** (11/11 resolved to three distinct targets:
bare-`ret` functor destroys `0xa62490`, `0xc4f0`, `0xa62850`, and `0x20d60`). Direct-call discrepancy:
**explained**. The constructor semantic contract, native implementation, and G1 status are unchanged
(PARTIAL). Suggested follow-up for the integrator: record the two walker-omitted calls explicitly in any
slot-306 native checker (`0xa643ad` is a real `0xc300` construction) and do not rely on the walker's
reference inventory for this body.
