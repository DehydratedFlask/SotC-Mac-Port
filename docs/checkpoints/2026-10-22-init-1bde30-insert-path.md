# Checkpoint — insert path `0x1bde30` → `0x1beeb0` (static only)

- Local: 2026-10-22 (America/Chicago)
- Agent: `subagent-init-1bde30-insert-path` (CPU-01 analysis worker; no integrator files touched)
- Task: `CPU-01-INIT-1BDE30-ANALYSIS-01` (attempt 1)
- Gate: **G1 static characterisation only** (no native code; nothing built/executed)
- Evidence: `local/validation/init-1bde30-20261022/` (private, gitignored)
  `manifest.json`, `report.md`, `listing`, `tools/analyze.py`, plus per-function `.s`/`.bin`.

Method: `scripts/ps4_dynlink.py` byte reads from `local/re/eboot.elf`
(sha256 `d0723afb…`, re-verified) + `/opt/homebrew/opt/llvm/bin/llvm-objdump` on minimal ELF
wrappers. Nothing loaded, relocated or executed. Extracted code treated as data only.

## OBS (byte-verified)

- **Extents/sizes/shas** (exact, over raw ELF): `0x1bde30` 18 B (`4a838c0c…`), `0x1bde50` 82 B
  (`98191a3c…`), `0x1bdeb0` 120 B (`7d64b9d3…`), `0x1beeb0` 284 B (`9a05e72e…`), `0x1bdb10` 56 B
  (`056d7514…`), `0x1bdae0` 46 B (`9c9bbed4…`), `0x1bdb50` 256 B (`c2085472…`).
- **`0x1bde30` = 18 B tail-call remarshal**: `mov rsi,rax; add rdi,0x40; mov rdi,rsi; mov rax,rdx;
  jmp 0x1beeb0`. Delivers `(rdi=sentinel, rsi=sentinel, rdx=&A)`. No deref, no null check.
- **Copy = 0x20 B at node `+0x10`** (the decisive evidence):
  `1bef04 vmovups (%r12),%ymm0` + `1bef0a vmovups %ymm0,0x10(%rbx)` → field map
  `node[0x10]=ctx` (then self-linked because `fn!=0`), `node[0x18]=invoke(0x1a00)`,
  `node[0x20]=0`, `node[0x28]=fn(0xb10)`. Extended slots `A+0x20/+0x40/+0x48` are **read as
  arguments** (vcall rsi, vtable lookup, swap-to-`node[0x58]`), **never bulk-copied**.
- **`+0x40` = list-3 sentinel**: ctor `0x1bdb10` builds a 0x60 B object with **three**
  self-referential lists at `+0x00`, `+0x20`, `+0x40` (counts at `+0x10/+0x30/+0x50`).
  `0x1bde30`'s `add rdi,0x40` derives `&this[0x40]`; `0x1beeb0` inserts into it, `incq this[0x50]`.
- **`0x1bde50`** iterates list-3 (`this[0x40]`), invokes node callable `+0x10/+0x18/+0x20` with
  `rsi=this`. **`0x1bdeb0`** cleans list-1 (`this[0x00]`): unlinks nodes with `[+0x50]==0`,
  destructs `+0x30`/`+0x10`, deletes, `decq this[0x10]`. Siblings, different lists.
- **Vtables**: `0x1594060` = 4 code ptrs (`0x19a0/0x19c0/0x19e0/0x19f0`) + 4 string ptrs
  (`0xf93bfc`/`0xf94329`/`0xf9432e`/`0xf94333` = `"end1"/"end2"/"end3"` + debug label), all
  `R_X86_64_RELATIVE`. Node vtable `0x1598ce0` (written at `node[0x50]`) has the **same first 4
  slots**. `0x19f0` (slot 3) is `retq` — the no-op vcall in `0x1290`.
- **Null `this`**: `0x1bde30`/`0x1beeb0` have no guard. `this==NULL → rdi==0x40`; insert
  dereferences `[0x48]` (`mov 0x8(%r15)`) and writes `[0x48]/[0x50]` (`incq 0x10(%rax)`) →
  unmapped low-memory fault. Slot `0x2ee68c8` is BSS + zeroed by static init `0x1c00e0` (which only
  registers atexit dtor `0x1bdae0`); the allocation is in `0x2569d0` (new 0x60 + ctor `0x1bdb10` +
  store), called from `0x5331e9` inside function `0x533100` — **distinct** from Init `0x531d60`.

## INF (inference, graded)

- **High** — copy is 0x20 B; descriptor A ends at `+0x20`, not `+0x40`. The 0x50 B caller span is
  `HandlerDescriptor` (0x20) + a 0x30 B registration-context tail, not one descriptor.
- **High** — `0x1594060` is a 4-slot vtable + name array, resolving the checkpoint's two readings
  in favour of "4-slot vtable + separate name array"; the "0x50 B / 8-slot descriptor" reading is wrong.
- **Medium** — descriptor is a member-function-pointer-shaped callable `{obj,invoke,offset,fn}`;
  `0x19a0/19c0/19e0` are copy/move/assign helpers, `0x19f0` a trivial destroy. Semantics unproven.
- **Low** — whether `*(0x2ee68c8)` is null when `0x1290` runs is ordering-dependent (indirect
  callers of `0x531d60`/`0x533100`), not statically provable; the insert path has no guard either way.

## Impact on `2026-10-22-init-1290-body-analysis.md`

- Blocker 2 (descriptor copy size) **settled**: 0x20 B copy; keep `HandlerDescriptor {ctx,invoke,0,fn}`.
- Blocker 1 (no native owner) **partially clarified**: owner = 3-list container (ctor `0x1bdb10`,
  alloc `0x2569d0`, free `0x1bdae0`); still unlifted.
- Proposed `HandlerEntryA` (0x50 B "descriptor") is **mis-labelled** — the tail is context, not
  descriptor. Do **not** encode it as-is into `native/`.

## Exact next action

Lift the 3-list container family (`0x1bdb10` ctor, `0x1beeb0`/`0x1bde50` list-3 ops, `0x1bdeb0`
list-1 cleanup, `0x1bdae0` dtor) as one bounded unit, **or** (cheaper) resolve the nullness
ordering by tracing who invokes `0x533100` vs `0x531d60`. Do not model the 0x50 B span as a single
descriptor.

## Rows to add (integrator action — NOT edited by this worker)

`docs/INDEX.md` checkpoint table:

```
| [Init insert path 0x1bde30 (2026-10-22)](checkpoints/2026-10-22-init-1bde30-insert-path.md) | 0x20 B descriptor copy @node+0x10; +0x40 = list-3 sentinel; vtable 4-slot + name array; no null guard; E517; G1 static |
```

Evidence ledger (`docs/EVIDENCE.md`): a matching `E517` entry summarising ELF `d0723afb…`,
fn shas (`0x1bde30` `4a838c0c…`, `0x1beeb0` `9a05e72e…`, `0x1bdb10` `056d7514…`), vtable
`0x1594060` = `{0x19a0,0x19c0,0x19e0,0x19f0}` + 4 string ptrs, copy = 0x20 B at `node[0x10]`,
gate G1 (static only, nothing executed).
