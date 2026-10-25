# Checkpoint: `CPU-01-INIT-32F79A8-ANALYSIS-01` — both provenance gaps resolved

- Time / timezone: 2026-10-23 (America/Chicago)
- Agent: subagent (static analysis worker, non-integrator)
- Task: `CPU-01-INIT-32F79A8-ANALYSIS-01` (attempt 1)
- Gate: **G1 PARTIAL** (static characterisation only, no code changes)
- Prerequisite evidence: ELF `d0723afb…` (E537), prior checkpoint `2026-10-22-init-tail-53299d.md`

## OBS (byte-verified)

### `0x32f79a8` — 3 register-indirect store sites found

Full scan of all 5.9M text instructions via llvm-objdump. No RIP-relative stores; no relocation entries targeting the address. Found by register-indirect dataflow tracking from 50 LEA sites:

| # | Store vaddr | File offset | Bytes | Value | Containing function |
|---|---|---|---|---|---|
| 1 | `0x2569b3` | `0x25a9b3` | `4d 89 37` | `movq %r14,(%r15)` → object ptr | `0x256970` GetInstance (0x210-byte alloc + `call 0xa46c70`) |
| 2 | `0x2557b5` | `0x2597b5` | `49 c7 07 00 00 00 00` | `movq $0,(%r15)` → NULL | `0x255780` teardown/release |
| 3 | `0xa47c73` | `0xa4bc73` | `48 c7 06 00 00 00 00` | `movq $0,(%rsi)` → NULL | `0xa479d0` init-sub + atexit registration |

Object: 0x210-byte singleton, ctor `0xa46c70`, dtor `0xa46d10`. Read at `0x5329a6` passes value as `this` to `0xa473b0`, which **ignores rdi**.

### `0x2eff450` — vtable resolved, slot-1 = `0x54b250`

2 register-indirect stores:

| # | Store vaddr | File offset | Bytes | Value | Containing function |
|---|---|---|---|---|---|
| 1 | `0x5323a6` | `0x5363a6` | `49 89 1e` | `movq %rbx,(%r14)` → 0x460-byte obj | Init `0x531d60` cell-install (E482) |
| 2 | `0x54b9fb` | `0x54f9fb` | `48 c7 06 00 00 00 00` | `movq $0,(%rsi)` → NULL | atexit-registration region |

Object vtable pointer at `object[0]` = `0x15c9830` (stored by `0x544fe0` at `0x54500f`).

Vtable `0x15c9830` (rodata, 15 R_X86_64_RELATIVE entries, all file-backed):

| Slot | File value (=final, base=0) | Function |
|---|---|---|
| 0 | `0x5450c0` | first vfn |
| **1** | **`0x54b250`** | **← slot-1 target at `0x532b09`** |
| 2 | `0x545310` | |
| 3-12 | `0x54b2d0` … `0x54b0e0` | |
| 13 | `0x545040` | destructor (deleting) |
| 14 | `0x545080` | destructor (`call *0x70` at `0x5323a3`) |

At `0x532b09`: `call [rax+0x8]` = `call *(0x15c9838)` = `call 0x54b250(*(0x2eff450), 0, 2)`.

Object identity: name struct `0x2efd9f8` = `"save_%s_saves_used"` (ctor `0x544fe0` → `0xa55f70`). 0x460-byte save-data tracking object (one of three in the `0x5329ae` block).

## INF (graded)

- **High** — Previous "no .text store" finding for `0x32f79a8` was caused by the scan being RIP-relative-only. All three stores are register-indirect (`lea rXX,[cell]; mov [rXX],val`), invisible to a pure RIP-relative byte-pattern scan.
- **High** — `0x32f79a8` is a lazily-initialized singleton (GetInstance pattern at `0x256970`). Its NULL state is a valid runtime state; `0xa473b0` ignores `this`.
- **High** — `0x2eff450`'s slot-1 target `0x54b250` is a link-time constant in rodata, confirmed by RELATIVE relocation and byte read. It is **resolvable statically**.
- **Medium** — Both cells are "singleton pointer" cells (the game uses this pattern extensively). Each has an atexit-registered NULL + destructor registration.
- **Medium** — `0x54b250` is likely a save-data query method: it takes `(this, index=0, category=2)`, calls `0xa56560` with a stack struct and dispatches through `0x15c9920`.

## Remaining unknowable statically

1. Exact C++ class names/types (no debug info, stripped binary).
2. Runtime constructor ordering (whether `0x256970` runs before Init's `0x5329a6`).
3. `0x54b250` deep callee semantics (`0xa56560`, `0x15c9920` dispatch).

## Coverage / limitations

- Dataflow tracker (`find_stores.py`) tracks one LEA register at a time through simple copy/clobber patterns within one function. It may miss stores through registers re-derived via arithmetic (e.g., `add r15, 0x10; mov (r15), val`). However, the dominant patterns are captured — all stores found are through the direct LEA destination with disp=0.
- Only the executable segment (vaddr `0..0x1593978`) was searched for stores. Constructor `.init_array` entries were not walked (cost: ~2 min for full ctor audit, but the stores are already found in normal code paths).
- `0x32f79a8`'s object vtable (from `0xa46c70`) was not read; it is straightforward to do with the same method.

## Exact next action

1. Lift `0xa473b0` + the 17 B `0x53299d` prefix as `CPU-01-INIT-A473B0-NATIVE-01` (already next in E537), now with provenance: `*0x32f79a8` as `this` is dead (which the lift's hook can express as nullable/ignored).
2. For `0x2eff450`: slot-1 = `0x54b250` is now a **known link-time constant**. The lift of `0x532b09` can emit a direct `call 0x54b250` stub instead of an unresolved hook. Slot-14 (`0x545080`) is also resolved as a destructor.
3. Optional: read `0x32f79a8`'s object vtable (`0xa46c70`'s stored vtable) for completeness.

## Rows for `docs/INDEX.md` (integrator action)

```
| [Init 32f79a8/2eff450 provenance (2026-10-23)](checkpoints/2026-10-23-init-32f79a8-2eff450-provenance.md) | 0x32f79a8: 3 reg-indirect stores (GetInstance + NULL×2), 0x210-byte singleton. 0x2eff450: vtable 0x15c9830, slot-1=0x54b250 (resolved). E538; G1 PARTIAL |
```

And evidence row **E538**: ELF `d0723afb…`; `0x32f79a8` writer sites: `0x2569b3` (`4d 89 37`), `0x2557b5` (`49 c7 07 00 00 00 00`), `0xa47c73` (`48 c7 06 00 00 00 00`). `0x2eff450` object vtable `0x15c9830` (15 entries, all RELATIVE `R_X86_64_RELATIVE`), slot-1 = `0x54b250`, slot-14 dtor = `0x545080`. Init store `0x5323a6` (`49 89 1e`). G1 PARTIAL.

## Private evidence

`local/validation/init-32f79a8-20261022/` — manifest.json, report.md, full_disasm.txt, find_stores.py
