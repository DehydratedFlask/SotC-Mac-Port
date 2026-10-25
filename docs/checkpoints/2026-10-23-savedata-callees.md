# Checkpoint — `CPU-01-SAVEDATA-CALLEES-ANALYSIS-01`: `0x54b250` + `0xa54920` fully characterised

- Local: 2026-10-23 (America/Chicago)
- Agent: `subagent-savedata-callees` (static analysis worker, non-integrator)
- Task: `CPU-01-SAVEDATA-CALLEES-ANALYSIS-01` (attempt 1)
- Gate: **G1 PARTIAL — static characterisation only, no code changes**
- Prereq evidence: ELF `d0723afb…` (E537/E538), prior `2026-10-22-init-tail-53299d.md`,
  `2026-10-23-init-32f79a8-2eff450-provenance.md`.
- Evidence (private): `local/validation/savedata-callees-20261023/` — `manifest.json`, `report.md`,
  `listing_54b250.s`, `listing_a54920.s`, `*.bin`/`*.elf`, `tools_elfwrap.py`.

Method: `scripts/ps4_dynlink.py` byte reads from `local/re/eboot.elf` + llvm-objdump on elfwrap
wrappers; imports resolved thunk→GOT→relocation→NID→symbol. Nothing executed. No edits to `native/`,
`scripts/`, or any HANDOFF/TASKS/EVIDENCE/VALIDATION/INDEX/ROADMAP. Extracted bytes treated as data.

## OBS (byte-verified)

### Extents / sha256

| Symbol | Extent | Size | sha256 |
| --- | --- | --- | --- |
| `0x54b250` body | `0x54b250..0x54b2c7` | 119 B | `40814086…d39f` |
| `0x54b250` aligned | `0x54b250..0x54b2d0` | 128 B | `96fbf6e7…de2d` |
| `0xa54920` | `0xa54920..0xa55050` | 1840 B | `27c76ceb…7810` (matches E537) |

Vtable `0x15c9830` slot list (all RELATIVE, base=0): `0x5450c0, 0x54b250, 0x545310, 0x54b2d0,
0x54b350, 0x54b3d0, 0x54b450, 0x54b460, 0x5459e0, 0x545d30, 0x548d30, 0x54b0b0, 0x54b0e0,
0x545040, 0x545080`. Slots 1/3/4/5 (`0x54b250`/`0x54b2d0`/`0x54b350`/`0x54b3d0`) are identical
query wrappers differing only in inner engine (`0xa56560`/`0xa56900`/`0xa56ae0`/`0xa56c30`).

### CFG

- `0x54b250`: 1 conditional branch (canary), 0 loops, single ret + cold `__stack_chk_fail` tail.
- `0xa54920`: 12 conditional branches (null/grow/canary), **0 loops** (3 register blocks unrolled),
  single ret (`0xa55048`, returns `r13`) + cold `__stack_chk_fail` tail.

### Import inventory (site → thunk → NID → symbol → library)

**Neither function reaches any `sceSaveData*`/`sceSaveDataDialog*` import, directly or via its
direct callees.** Only libc/libkernel:

| Function | Via | Thunk | Symbol | Library |
| --- | --- | --- | --- | --- |
| 0x54b250 | direct | 0xf8f3a0 | `__stack_chk_fail` | libkernel |
| 0xa54920 | direct | 0xf8f3a0 | `__stack_chk_fail` | libkernel |
| 0xa54920 | 0x20d60 | 0xf8f500 | `strcasecmp` | libc |
| 0xa54920 | 0x272d0 | 0xf8f3d0/0xf8f390/0xf8f3e0/0xf8f3b0/0xf8f3c0 | `__cxa_guard_acquire`/`__cxa_atexit`/`__cxa_guard_release`/`strlen`/`vsnprintf` | libc |

All `sceSaveData*` call sites are in `0xa47xxx..0xa4dxxx` (disjoint subsystem). Full table in
`report.md` §3.

### Direct/indirect calls

- `0x54b250`: direct `0xa56560` (query engine `(this,0,2,&struct,0,0)`) + `__stack_chk_fail`;
  indirect `[0x15c9920+0x18]` = slot 3 = `0x538200` = **bare `ret` (no-op)**.
- `0xa54920`: direct = allocators `0x17070/0x17090/0x170b0`, `0x10a90` (snprintf), `0xf590`
  (std::string dtor), `0x3a980` (ConVar value obj), `0x20d60` (global-list register), `0x26ea0`
  (ConVar build: default 1.0f / max 10.0f), `0x272d0` (ConVar commit, static-init + `__cxa_atexit`),
  `0xa55050` (sibling descriptor ctor), `0xa57c10` (list dtor), `0xa586a0` (vector grow). Indirect:
  `[0x15d1d80+0x18]`×3 = `0xc770` = **bare `ret` (no-op)**; `[obj+8]`×3 = replaced-object dtor
  (null on first init); six descriptor copy vtable calls via `0x15c8d98`→accessors.

### Struct writes / registration

- `0x54b250`: stack-only result struct (`[rsp+0x40]=0x15c9920`). No heap/global/atexit writes.
- `0xa54920`: 3 sub-counter objects (`[obj]=vtable 0x1594c08`, `[obj+0xd0]=[obj+0x68]=max_count or 0`)
  → parent links `[parent+0x40/0x48/0x50]`; 3 ConVars (name@+0x50, value-obj@+0x90, 1.0f@+0x98,
  10.0f@+0x9c); descriptor `[rbx]=name_struct,[+0x10]=list,[+0x18]=0,[+0x20]=-1`; parent vector
  append (grow via `0xa586a0` + `new[] 0x100`). Global list register via `0x20d60` (head
  `0x2dfcbc8`); atexit via `0x272d0` (`__cxa_guard_acquire`/`__cxa_atexit` @ `0x2dfd4e8`).

## INF (graded)

- **High** — `0x54b250` is a **read-only save-data statistics query** wrapper: forwards
  `(this,index=0,category=2)` to `0xa56560`, then fires a provably no-op callback (`0x538200`).
  No Sony I/O, no registration, no global write.
- **High** — `0xa54920` is **dominated by ConVar/console (statistics) registration**, NOT save-data
  transaction setup. Strings `save_%s_{max_count,saves_used,mem_used}` + `"Save System"` /
  `"%s Max Count"` / `"%s Saves Used"` / `"%s Save Mem Used (MB)"`, ConVar float bounds (1.0/10.0),
  static-init+atexit commit, global-list registration all point to telemetry, and **zero**
  `sceSaveData*` reachable. E537's ConVar inference is **confirmed**; the "save-data transaction"
  reading is **rejected** for this symbol.
- **High** — the real save-data boundary is the separate `0xa47xxx..0xa4dxxx` subsystem (11
  `sceSaveData` + 9 `sceSaveDataDialog` imports). Neither subject belongs to it.
- **Medium** — `0x20d60`'s global list (head `0x2dfcbc8`) is the store that `0x54b250`'s engine
  `0xa56560` walks; registration (ctor) and query (method) are thereby linked in memory but neither
  touches the PS4 filesystem.

## Boundary recommendation

| Symbol | Recommendation | Reason | NOT-modelled boundary |
| --- | --- | --- | --- |
| `0x54b250` | **implement natively** (co-lift `0xa56560` engine) | 128 B pure read-only, only `__stack_chk_fail` import, no-op dispatch | reads in-memory statistics; does **not** reach PS4 save FS / `sceSaveData*` |
| `0xa54920` | **model as explicit non-fake "save-data statistics counter registry" contract** (alt: native lift) | effect fully characterizable (`{name,max_count,description}` register + descriptor append); no Sony semantics to fake | zero save-data transaction; real `sceSaveData*` boundary is `0xa47xxx..0xa4dxxx` |

Do **not** label `0xa54920` "the save-data hook" — it contains no I/O; that misattributes the
architecture.

### Real save-data entry points (caller scan into `0xa47000..0xa4e000`)

| Caller | Target | Note |
| --- | --- | --- |
| `0x25bbd5` | `0xa477a0` | early-init save-data entry |
| `0x25be33` | `0xa4d400` | early-init (adjacent `sceSaveDataInitialize3` @ `0xa4d295`) |
| `0x534c70` | `0xa47020` | Init-region save-data entry |
| `0x534c7d` | `0xa477d0` | Init-region save-data entry |
| `0x5329a9` | `0xa473b0` | capture-enable setter (E537), NOT save-data |

Neither subject calls into this region. The honest save-data service-contract boundary is at
`0xa47020`/`0xa477a0`/`0xa477d0`/`0xa4d400` (called directly from Init-adjacent code), independent
of the statistics registrar `0xa54920` and query `0x54b250`.

## Exact next action

1. Lift `0x54b250` + its query-engine cluster (`0xa56560`, and optionally siblings
   `0xa56900`/`0xa56ae0`/`0xa56c30` + `0xa56560` callees `0xa558f0`/`0xa56340`/`0xa56080`) as a
   native read-only statistics-query unit (`CPU-01-SAVEDATA-QUERY-NATIVE-01`); wire `__stack_chk_fail`
   natively and drop the no-op `0x538200` dispatch.
2. Model `0xa54920` as an explicit statistics-registry contract at the three Init call sites
   (`0x532a07`/`0x532a62`/`0x532ae2`), with a real in-memory counter registry consumed by the query
   path; do not treat as a Sony service hook.
3. Separately, characterise/lift the actual `0xa47xxx..0xa4dxxx` save-data subsystem (11
   `sceSaveData` + 9 `sceSaveDataDialog`) — that is the genuine save-data service-contract boundary.

## Remaining unknowns

- `0xa55050` sibling ctor (only shallowly read; recurses at `0xa5548d`; libc/allocator imports only).
- Runtime vtable of `parent` singleton `[r13]` and of descriptor object (BSS, not resolved).
- `category=2` semantics in `0x54b250` (which statistics field the query selects).

## Rows for `docs/INDEX.md` (integrator action — not edited here)

```
| [Savedata callees 54b250/a54920 (2026-10-23)](checkpoints/2026-10-23-savedata-callees.md) | 0x54b250=128B read-only stats query (engine 0xa56560, no-op dispatch 0x538200); 0xa54920=1840B ConVar/statistics registrar (zero sceSaveData*). Real save-data = 0xa47xxx..0xa4dxxx. E539; G1 PARTIAL |
```

Evidence row **E539**: ELF `d0723afb…`; `0x54b250` body 119 B `40814086…` / aligned 128 B
`96fbf6e7…`; `0xa54920` 1840 B `27c76ceb…`; both have **zero** `sceSaveData*` imports (only
`__stack_chk_fail`/libc); `0x54b250` indirect `[0x15c9920+0x18]`→`0x538200` (no-op), `0xa54920`
indirect `[0x15d1d80+0x18]`→`0xc770` (no-op); all 20 `sceSaveData*` imports sit in `0xa47xxx..0xa4dxxx`.
G1 PARTIAL.
