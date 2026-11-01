# Indirect-call frontier of the `0x531d60` closure (CPU-01-AOT-CFG-SCALE-01)

Every indirect `call`/`jmp` inside the transitive direct-call closure of the appmain init entry
`0x531d60`, classified by the instruction window before the call. Static only; nothing was
executed, and **no target is guessed**.

- Tool: `tools/lift/indirect_frontier.py` (reuses the FDE loader, adjacency and closure from
  `cfg_census.py`).
- Publishable summaries: [`indirect_frontier.json`](indirect_frontier.json) (seed `0x531d60`),
  [`indirect_frontier_1bdf30.json`](indirect_frontier_1bdf30.json) and
  [`indirect_frontier_union.json`](indirect_frontier_union.json) (both seeds). Counts and resolved
  rows only, no instruction text; full per-site rows stay private.
- Checker: `tools/lift/check_indirect_frontier.py` regenerates all three summaries and diffs them;
  it skips cleanly when the private disassembly is absent.

Inputs (pinned): ELF `d0723afb…61d3`, disassembly `9c196254…107e83`, FDE ranges `f507ab6c…22e8`.

## Numbers

| Seed | Closure functions | Frontier functions | Indirect sites | Resolved |
|---|---:|---:|---:|---:|
| `0x531d60` (appmain init) | 608 | 105 | 499 (452 calls + 47 jumps) | **5** |
| `0x1bdf30` (app-info dispatch) | 416 | 59 | 122 (102 calls + 20 jumps) | **3** |
| union | 817 | 136 | 562 (513 calls + 49 jumps) | **6** |

The per-seed closure sizes match `cfg_census.py` exactly (608 / 416 / 817); this card only adds the
classification the census deliberately refused to make. The union resolves six sites because the
two seeds overlap.

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

Resolution is a backward slice, not a fixed window: the classifier walks back to the **nearest
definition** of the call register (comment text stripped first) and then follows at most one
RIP-relative base. A candidate is published only when the slot value points inside an FDE function,
which rejects the string/padding false positives a naive window produces.

Five distinct targets resolve end to end, from six call sites. Every one is corroborated twice: the
qword in the image and a type-8 dynamic relocation with the same addend.

| Call site | Vtable | Slot | Target | RELA row |
|---|---|---|---:|---|
| `0x1254d` | `0x15949a0` | `0x88` | **`0x187c0`** | `0x1594a28` |
| `0x13620` | `0x1594ae8` | `0x28` | **`0x13d20`** | `0x1594b10` |
| `0x28471` | `0x1594dc0` | `0x20` | **`0x20d60`** | `0x1594de0` |
| `0x35165` | `0x15955c0` | `0x08` | **`0x358c0`** | `0x15955c8` |
| `0xa54b7b`, `0xa54c92` | `0x15d1d80` | `0x08` | **`0xc740`** | `0x15d1d88` |

All five are FDE function starts. They are recorded in `native/core/vtable_resolved_entries.{hpp,cpp}`
(sorted by call site, with `find_resolved_vtable_call_site`) and therefore appear in
[`entry_map.json`](entry_map.json); `tools/lift/check_vtable_resolved_entries.py` re-verifies the
slot qword, the RELA addend, the FDE start and the entry-map presence for each row. Only `0x20d60`
has a body model so far (`vtable_1594dc0_slot20_20d60`); the other four are entry records awaiting
their own lift.

## What stays unknown, and why

- **The remaining sites have no static base.** The object pointer comes from a function argument, a
  returned value or a heap walk, so no file-backed chain exists to follow. They are genuinely
  dynamic; resolving them needs runtime construction or a type/ownership proof per call site, not a
  better window.
- The one resolved site is 0.2 % of the frontier. That is the honest scale: the closure's indirect
  calls are dominated by runtime-initialised objects, which matches the earlier findings for this
  engine (`_Root`, the camera `D`, the app-info registry all register their vtables at runtime).
- A longer instruction window would reclassify some `register-offset`/`register` sites, but it would
  not create a file-backed object chain where none exists.

## Next steps

1. Lift the four resolved targets that still have no body model (`0x187c0`, `0x13d20`, `0x358c0`,
   `0xc740`) one at a time, with the same pinned-body fixture used for `0x20d60`.
2. The "vtable readable, slot not" sites are already answered: no dynamic relocation targets their
   slot addresses, so the tables are runtime-filled. Do not re-open them without new evidence.
3. Everything else needs runtime capture or per-site type proofs; do not widen the classifier to
   manufacture resolutions.

## Limits

- Classifications describe call shape, not callee identity.
- One seed (`0x531d60`); other entry points will have different frontiers.
- The window is 4 instructions; a load older than that is invisible.
- Disassembly-derived (objdump text), not decoder-derived; a site objdump merged or split could be
  mis-grouped.
