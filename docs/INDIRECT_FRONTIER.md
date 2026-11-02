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
- Later seed classes (below): the 573-row constructor table, a 40-seed entry-map sample, the
  exhaustive entry-map sweep, and the vtable-closure sweep. Their artifacts live under
  `local/validation/indirect-frontier/` (`ctor-sweep.json`, `entry-map-sweep.json`,
  `entry-map-sweep2.json`, `entry-map-exhaustive-summary.json`, `vtable-closure-sweep-summary.json`).

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
slot qword, the RELA addend, the FDE start and the entry-map presence for each row. All 33
distinct targets now have a native body model; the table grew through the sweeps recorded below.

## Sweeps after the seed closure

`vtable_resolved_entries` is the union of four sweeps, each with its own seed class:

| Sweep | Seeds | Resolved rows | Distinct targets |
|---|---:|---:|---:|
| `0x531d60` / `0x1bdf30` closures | 2 | 6 | 6 |
| Constructor table (`0x2dd5f00..0x2dd70e0`, 573 entries) | 573 | 42 | 17 |
| Entry-map sample, twice | 40 | 47 | 22 |
| **Vtable-closure sweep** | **13 containers** | **74** | **33** |

The first three rows match the recorded checkpoints. The entry-map sample reached 47 rows / 22
targets; the exhaustive entry-map sweep below added no new target on top of it, and the
vtable-closure sweep is the class that did.

### Exhaustive entry-map sweep (class exhausted)

Seed class: every key of `docs/entry_map.json` that is an FDE start and had never been used as a
sweep seed — the 573 constructor rows, the 40 sampled seeds and the two base seeds removed. The live
enumeration is **266** seeds (the checkpoint's `274` was an estimate from an earlier entry map), all
run in one batch:
`local/validation/indirect-frontier/entry-map-exhaustive-summary.json`.

Result: closure **1,432** functions, **906** indirect sites, **15** resolved rows. Four of them were
not yet in the header (`0x3133e`, `0x3134e`, `0x31360`, `0x1c00b8`), and every one reaches a target
that was already recorded. **0 new targets.** The entry-map class is therefore exhausted for target
discovery; more seeds from it only re-find the same tables.

### Vtable-closure sweep

Seed class: the FDE-start **containers of every fully tied call site that goes through a vtable the
table already trusts** and was not yet a row. This is a site-evidence class, not a name list: the
sites were enumerated image-wide (slot qword + type-8 RELA addend + FDE start), then each containing
function was used as a sweep seed so the site is provably inside the tool's closure. It is the only
class that extends the table's own evidence base rather than sampling another entry-point list.

- Artifact: `local/validation/indirect-frontier/vtable-closure-sweep-summary.json` (13 seeds,
  closure 847 functions, 347 sites, 30 resolved rows, no instruction text).
- Rows added: **27**. Ten bases were already in the table (`0x15949a0`, `0x1594ae8`, `0x1594dc0`,
  `0x15953f8`, `0x1598e10`, `0x15e1ee0`, `0x15e2028`, `0x15e2d60`, `0x15ed2a0`, `0x15f2f08`); two bases
  are new because no header row used them before the sweep (`0x1595b60`, `0x1595320`).

| Target | Slot(s) | Sites | Body | Model |
|---|---|---:|---:|---|
| **`0x13d50`** | `0x1594ae8 + 0x40` | 1 | 3 B: `xor eax,eax; ret` | `vtable_1594ae8_slot40_13d50` |
| **`0x13d60`** | `0x1594ae8 + 0x50`, `0x1595b60 + 0x50` | 2 | 9 B: sets `this+0x70 = this+0x60` | `vtable_1594ae8_slot50_13d60` |
| **`0x30e50`** | `0x1595320 + 0x10` | 1 | 5 B: `jmp 0x312a0` | `vtable_1595320_slot10_30e50` |
| **`0x488d0`** | `0x1595b60 + 0x18` | 1 | 28 B: pre-pass `0x48790`, tag-1 free | `vtable_1595b60_slot18_488d0` |
| **`0xc2c0`** | `0x15949a0 + 0x58` | 1 | 20 B: stamp `0x1594280`, copy `+0x10` | `vtable_15949a0_slot58_c2c0` |
| **`0xe19cb0`** | `0x15e2028 + 0x18` | 1 | 28 B: pre-pass `0xe1b1b0`, tag-1 free | `vtable_15e2028_slot18_e19cb0` |
| **`0xe19f70`** | `0x15e2028 + 0x38` | 1 | 197 B: name scan, max-id pick, returns found | `vtable_15e2028_slot38_e19f70` |
| **`0xe1a040`** | `0x15e2028 + 0x40` | 1 | 129 B: name + id lookup, returns 0/1 | `vtable_15e2028_slot40_e1a040` |
| **`0xe19440`** | `0x15e1ee0 + 0x10` | 1 | 143 B: three 8-byte nodes, slot-0x10 dispatch | `vtable_15e1ee0_slot10_e19440` |
| **`0xe52cb0`** | `0x15ed2a0 + 0x10` | 1 | 254 B: three-owner teardown, base dtor | `vtable_15ed2a0_slot10_e52cb0` |
| **`0xed4860`** | `0x15f2f08 + 0x88` | 1 | 5 B: `jmp 0x170b0` (tag-1 free) | `vtable_15f2f08_slot88_ed4860` |

Every row was re-verified outside the sweep tool before it was written: the qword at
`vtable + slot`, exactly one type-8 dynamic relocation at that address with the same addend, an FDE
start at the target, and the body digest. `tools/lift/check_vtable_siblings_low.py`,
`check_vtable_containers_e1.py` and `check_vtable_teardown_tail.py` pin the body bytes and sha256,
the FDE length, the slot qword, the RELA addend and the entry-map record for all 11 new targets;
`check_vtable_resolved_entries.py` re-verifies all 74 rows (33 targets).

### What stays unknown, and why

- **Rows are not reachability.** A row says the slot holds that function in the pinned image. It does
  not say the call executes, that the object is constructed, or that the path is gameplay-reachable.
- **The rest of the static vtable graph is huge.** An image-wide scan (same classifier) finds 2,249
  fully tied `vtable-slot` sites over 795 distinct targets, i.e. 74 rows is 3.3 % of the tied sites.
  The remaining targets are real, but each needs a body model before it may become a row, so they
  are a queue, not a claim.
- **Sites with no static base** come from arguments, returns or heap walks; no file-backed chain
  exists to follow. They need runtime capture or a type/ownership proof per site.
- A longer instruction window would reclassify some `register-offset`/`register` sites, but it would
  not create a file-backed object chain where none exists.

## Next steps

1. The next bounded seed class is the remaining **containers of fully tied sites**: 27 rows came from
   the 13 containers above; the image-wide scan lists 857 containers, so the next batch is a
   deliberate slice (for example the next 26 lowest-RVA containers), each target still modelled
   before its row is added.
2. Do not re-sweep the constructor table or the entry-map keys; both are exhausted for new targets.
3. Keep the "model, then row" order. A target without a fixture and checker stays out of the table.

## Limits

- Classifications describe call shape, not callee identity.
- One seed (`0x531d60`); other entry points will have different frontiers.
- The window is 4 instructions; a load older than that is invisible.
- Disassembly-derived (objdump text), not decoder-derived; a site objdump merged or split could be
  mis-grouped.
