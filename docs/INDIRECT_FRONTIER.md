# Indirect-call frontier of the `0x531d60` closure (CPU-01-AOT-CFG-SCALE-01)

Every indirect `call`/`jmp` inside the transitive direct-call closure of the appmain init entry
`0x531d60`, classified by the instruction window before the call. Static only; nothing was
executed, and **no target is guessed**.

- Tool: `tools/lift/indirect_frontier.py` (reuses the FDE loader, adjacency and closure from
  `cfg_census.py`).
- Publishable summary: [`indirect_frontier.json`](indirect_frontier.json) (counts + resolved rows,
  no instruction text). Full per-site rows stay private in `local/validation/indirect-frontier/`.
- Checker: `tools/lift/check_indirect_frontier.py` regenerates the summary and diffs it; it skips
  cleanly when the private disassembly is absent.

Inputs (pinned): ELF `d0723afb…61d3`, disassembly `9c196254…107e83`, FDE ranges `f507ab6c…22e8`.

## Numbers

| Metric | Value |
|---|---:|
| Closure functions from `0x531d60` | **608** |
| Frontier functions (≥1 indirect site) | **105** |
| Indirect sites in closure | **499** |
| — indirect `call` sites | 452 |
| — indirect `jmp` sites | 47 |
| Sites whose target is resolved from the image | **1** |
| Sites whose target stays unknown | 498 |

The 608 / 105 pair matches `cfg_census.py --seed 0x531d60`; this card only adds the classification
that the census deliberately refused to make.

## Classification

| Class | Sites | Meaning |
|---|---:|---|
| `vtable-slot` | 283 | `call *0xNN(%reg)` where the window loads `%reg` from `0xMM(object)` — slot `0xNN` of the object's vtable at `+0xMM` |
| `first-slot` | 96 | `call *(%reg)` — slot 0 of the object's vtable |
| `register-offset` | 45 | `call *0xNN(%reg)` with no vtable load in the 4-instruction window |
| `indirect-jump` | 47 | `jmp *…` inside the closure (dispatcher / tail-call shapes) |
| `register` | 15 | `call *%reg`, emitters and callbacks |
| `member-pointer` | 2 | `call *%reg` where the window loads a member pointer into `%reg` |

## What is resolvable

Twelve sites carry a RIP-relative global base in the window; those are the only ones whose object
identity is even statically named. Of them:

- **1 resolves end to end.** `0x28471` (`call *0x20(%rax)` in a closure function) takes `%rax` from
  `lea 0x1594dc0` (file-backed vtable), so slot `0x20` reads as **`0x20d60`**. Independent
  corroboration: the dynamic relocations contain exactly one row at `0x1594de0` (the same slot),
  type 8, symbol 0, addend `0x20d60`. `0x20d60` is also an FDE function start, so it is a concrete
  native entry candidate once its ABI is checked.
- **3 have a readable vtable pointer but an unreadable slot** (`0x2ee2d00`, `0x2ee3b10`,
  `0x2eff450`): the pointer resolves into memory that is not file-backed, and none of the three slot
  addresses (`0x2ee2d28`, `0x2ee3b78`, `0x2eff458`) has a dynamic relocation row. Those vtables are
  filled at runtime, so their targets are unknowable from the image.
- **8 point at BSS globals** (`0x2dfb500`, `0x2dfb600`, `0x2dfc700`, `0x2dfc800`, `0x2efe318`,
  `0x2eff450`, …): the qword is zero in the image and only exists after construction at runtime.

## What stays unknown, and why

- **487 sites have no static base at all.** The object pointer comes from a function argument, a
  returned value or a heap walk, so no file-backed chain exists to follow. They are genuinely
  dynamic; resolving them needs runtime construction or a type/ownership proof per call site, not a
  better window.
- The one resolved site is 0.2 % of the frontier. That is the honest scale: the closure's indirect
  calls are dominated by runtime-initialised objects, which matches the earlier findings for this
  engine (`_Root`, the camera `D`, the app-info registry all register their vtables at runtime).
- A longer instruction window would reclassify some `register-offset`/`register` sites, but it would
  not create a file-backed object chain where none exists.

## Next steps

1. Add the single resolved target (`0x20d60`) to the entry map and check its ABI before calling it.
2. The three "vtable readable, slot not" sites are already answered: no dynamic relocation targets
   their slot addresses, so the tables are runtime-filled. Do not re-open them without new evidence.
3. Everything else needs either runtime capture or per-site type proofs; record it as such rather
   than widening the classifier.

## Limits

- Classifications describe call shape, not callee identity.
- One seed (`0x531d60`); other entry points will have different frontiers.
- The window is 4 instructions; a load older than that is invisible.
- Disassembly-derived (objdump text), not decoder-derived; a site objdump merged or split could be
  mis-grouped.
