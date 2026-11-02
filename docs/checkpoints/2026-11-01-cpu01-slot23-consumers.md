# CPU-01-SLOT23-CONSUMERS-01 — who reads the slot-23 table, and does it need the original strings

- Date / time: 2026-11-01 20:49 CST (America/Chicago).
- Agent: subagent lane for `CPU-01-SLOT23-CONSUMERS-01` (task row owned by root).
- Task: `CPU-01-SLOT23-CONSUMERS-01`, originally "Determine whether slot 23's name/hash registry
  requires its original strings".
- Status now: **COMPLETE / STATIC_ANALYSIS_VERIFIED** for the determination. The answer is yes, for
  two independent reasons, and the reader set is now closed to two accessor functions and one
  consumer body. No runtime claim.
- Attempts: 0 failed semantic hypotheses. Four byte-offset slips were made while authoring the new
  checker (slice offsets and one objdump encoding assumption) and were corrected before acceptance;
  none of them was a semantic hypothesis about the binary.
- Pinned input: `local/re/eboot.elf`, 65,214,308 B, SHA-256
  `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`, image base 0, ELF64
  x86-64, 11 program headers. Program-header mapping used below: PHDR 0 executable
  (`off 0x4000`, `vaddr 0`, `filesz 0x1593978`), PHDR 1 RELRO
  (`off 0x1598000`, `vaddr 0x1594000`, `filesz 0x6d190`), PHDR 2 writable/BSS
  (`off 0x1608000`, `vaddr 0x1604000`, `filesz 0x17d3100`, `memsz 0x2352aa4`).
- Repository base: `c3719328`; working tree also carried another worker's uncommitted
  `native/core/ctor_slot_312_a46130.*` edits, which this lane did not touch.

## Bottom line

Slot 23 (`0xe71990`) publishes 14 `{u64 name pointer, u32 hash}` records at BSS `0x3353f40`.
Exactly two functions read that table, both 26-byte leaf accessors:

| Accessor | Reads | Bound | Body |
|---|---|---|---|
| `0xe70a60` | `+0x00` qword (name pointer) | `k > 12` → 0 | 26 B, FDE `[0xe70a60,0xe70a7a)` |
| `0xe70a80` | `+0x08` dword (hash) | `k > 12` → 0 | 26 B, FDE `[0xe70a80,0xe70a9a)` |

Both are virtual members of one context class (vtable `0x15f13f0`, slots `+0x28` and `+0x30`), and
both are consumed by exactly one body, `0xe5bda0` (FDE 7,084 B, `[0xe5bda0,0xe5d94c)`), an APEX
`RenderMeshAssetAuthoring` method.

**The registry does require the original strings.** The name pointer is not decorative: the consumer
passes it to `0xe70af0`, which recomputes `djb2` over the bytes and compares that value with the
stored hash column for slots 0..12. A table whose name and hash are inconsistent still creates a
parameter, but loses the `semantic` tag. The string itself is also stored as the parameter's `name`.
The 14th record is never read by anything.

**No reader can run before slot 23.** Every reader first dereferences the APEX framework singleton
`[0x33533b8]` without a null check; its only non-zero store is inside the PhysX/APEX init path that
runs in the Init slice, downstream of the constructor walk in which slot 23 is entry 23.

## 1. Producer (re-confirmed, not re-derived)

`0xe71990` is 337 B, FDE `[0xe71990,0xe71ae1)`, body SHA-256
`3932fb85b759ae53dd1675142ffda4b449be725f241cfeefdb40a07a2beaf774` (matches
`tools/lift/check_fe71990.py`). It performs 28 stores — 14 qword name addresses and 14 dword hashes —
into `0x3353f40..0x3354018`; the last four bytes of each record are never written and the 14th hash
is the literal 0. It has **no direct call site anywhere in the image**; the only reference to its RVA
is the 573-entry table at `0x2dd70e0`, whose entry 23 is `0xe71990` (read from the ELF, see checker).

The 14 name strings live at `0x131c990..0x131caa5` in PHDR 0 (read-only, file-backed). A scan of the
whole 251 MB linear disassembly for RIP-relative targets inside that window returns **15 distinct
addresses, 14 of them referenced exactly once and only by `0xe71990`**; the 15th (`0x131caa5`) is a
different string used by `0xe70933`. So the slot-23 names have no second reader: the table is their
only path into the system.

## 2. Reader census — the table has exactly two readers

Three pieces of evidence, all reproducible from the pinned image:

1. **Text.** The literal value `0x3353f40` occurs in the disassembly in exactly three instructions:
   `0xe70a6a` (`leaq`) and `0xe70a8a` (`leaq`) are reads; `0xe719a5` (`movq`) is the constructor
   store. No other instruction forms the table address.
2. **Data.** The 4-byte and 8-byte little-endian encodings of `0x3353f40` occur **zero** times in the
   file-backed image, so there is no data pointer to the table and no relocation addend for it. A
   computed reader would have to derive the address without ever materialising it, and the RIP
   displacement in all three instructions is the only encoding present.
3. **Vtable.** `0xe70a60` and `0xe70a80` appear as data exactly twice each: once as vtable slots of
   `0x15f13f0` and once as the matching RELA addend. There is no second vtable holding either
   accessor.

The class vtable is installed by `0xe71610` (74 B), whose body `lea`s `0x15f13e0`, adds `0x10` and
stores the result:

`0x15f13f0 + 0x28 = 0xe70a60`, `+0x30 = 0xe70a80`, `+0x38 = 0xe70aa0` (the djb2 hash),
`+0x40 = 0xe70af0` (the resolver), `+0xa8 = 0xe715d0` (linear hash→row search).

`0xe71610` itself has exactly two direct call sites: `0xe5c2ed` (inside `0xe5bda0`) and `0xea95c7`
(inside `0xea7470`, an 8,812-byte sibling authoring body). `0xea7470` constructs the same context but
its decompiled body never reads the preset name/hash slots: it seeds the parameter list from a
different object (`plVar31`) and uses `0xe71500`/`0xe71310` only. So the slot-23 table has one
consumer body, not two.

## 3. The accessors (exact semantics)

```
0xe70a60:  31 c0              xor  eax,eax
           83 fe 0c           cmp  esi,0xc
           77 12              ja   0xe70a79        ; k > 12 -> return 0
           48 63 c6           movslq esi,rax
           48 8d 0d cf344e02  lea  rcx,[0x3353f40]
           48 c1 e0 04        shl  rax,4
           48 8b 04 08        mov  rax,[rax+rcx]   ; record +0x00
           c3                 ret
0xe70a80:  ... same guard / base / stride ...
           8b 44 08 08        mov  eax,[rax+rcx+8] ; record +0x08
           c3                 ret
```

Both reject index 13, so the sentinel record is unreachable through the accessors. Note also that
`name_hash` maps a zero result to 1, so a stored hash of 0 can never equal `djb2` of any string: the
14th record's zero is an *unmatchable* marker, not merely an absent value.

## 4. The consumer body `0xe5bda0`

Identity: Ghidra decompiles it with the literals
`…\APEX_1.4\framework\src\ApexRenderMeshAssetAuthoring.cpp`, `"createRenderMesh"`,
`"MeshDesc is not valid!"` and `"Format (%d) is not valid for Semantic (%s)"`. It is a virtual
method (vtable slot at VA `0x15f0408`, wrapper thunk `0xe5da00` at `0x15f0578`) with no direct call
site, so it is reached only through an object of its own class. Ghidra reports 18 blocks / 6,945 B in
a 7,084 B FDE extent.

Structure relevant to the table (two independent loops, both fed by a 0x218-byte vertex-descriptor
stride):

- Outer loop over vertex buffers; inner loop `k = 0..12` — the bound is `cmpq $0xd,%rbx`
  (`0xe5c48e`, bytes `48 83 fb 0d`) with `addq $0x28,%r15` (`0xe5c48a`). The element's format code
  (`r13d`, loaded at `0xe5c350`) is dispatched through the 13-entry relative table at `0x1319b08`
  (E981/E982).
- Valid (semantic, format) pair → `0xe5c3f5`:
  `0xe70a60(ctx,k)` → `0xe70af0(ctx,name)` → `0xe71310(ctx,index,format)`.
  Bytes: `4c89f7 89de e866460100` / `4c89f7 4889c6 e8eb460100` / `418b17 4c89f7 89c6 e8fe4e0100`.
- Invalid but non-zero format → `0xe5c425`, whose error message calls `0xe70a60(ctx,k)` a second time
  (`0xe5c441`) purely to print the semantic name in
  `"Format (%d) is not valid for Semantic (%s)"`.
- A second loop (`0xe5c4f0..0xe5c5a5`) walks 0x28-byte element rows and calls `0xe70af0` at
  `0xe5c54f`; `0xe5c5bd` registers the literal `"VERTEX_ORIGINAL_INDEX"` (loaded from `0x1319c93`)
  with format `0xd`.
- The hash path (`0xe5cd78`, `0xe5ce2a`) hashes a stored name pointer with `0xe70aa0` and looks the
  hash up with `0xe715d0`; it does not read the slot-23 table.

Call-site census in the linear disassembly: `0xe70a60` ← exactly `0xe5c3f5`, `0xe5c441`; `0xe70af0`
← exactly `0xe5c400`, `0xe5c54f`, `0xe5c5bd`; `0xe70aa0` ← `0xe5cd78`, `0xe5ce2a` (plus the virtual
`+0x38` in `0xe70af0`); `0xe715d0` ← `0xe5cd82`, `0xe5ce34` (plus the virtual `+0xa8`).

**Record 13 is never read.** The loops stop at 13, both accessors reject `k > 12`, and the resolver
performs exactly 13 preset-hash comparisons (13 occurrences of `ff 50 30` in `0xe70af0`, at
`0xe70b60`…`0xe70cb2`). The only 14th-slot registration in the body is the independently named
`"VERTEX_ORIGINAL_INDEX"` literal, which is *not* the table's 14th name.

## 5. What `0xe70af0` does with the string — why the bytes matter

`0xe70af0(ctx, name)`, 1,552 B, FDE `[0xe70af0,0xe710ff)`, body SHA-256 `5e41af6c…4f4562`:

1. `name == nullptr` → return `-1` (`0xffffffff`).
2. `h = djb2(name)` through virtual `+0x38` = `0xe70aa0` (the same function the native
   `sotc::name_hash` models: 5381, `h*33 ^ sign_extend(byte)`, 0 → 1).
3. `idx = (+0xa8)(ctx, h)` = `0xe715d0`: linear search of the `VertexFormatParameters` rows
   (`count` at `+0x60`, pointer at `+0x50`, hash at `row+0x14`, stride `0x28`) returning the row
   index or `-1`.
4. If found, return `idx`.
5. Otherwise create a parameter definition. It compares `h` against the preset hashes through
   virtual `+0x30` = `0xe70a80` in a 13-deep unrolled chain and records
   `semantic = k` on the match, or `semantic = -1` if none matches. It then sets
   `name = <the string>` via the `"name"` field (`0x131cab3`, setter `+0x100`), `id = h` via the
   `"id"` field (`0x131cac1`, setter `+0x220`), `format/access = 0`, `serialize = 1`, appends the
   definition, and returns the new row index.

Consequences, all from the bytes above:

- **The hash column decides the semantic tag.** `semantic` is `k` only when the runtime `djb2` of the
  string in slot `k` equals the stored hash for slot `k`. If the native build substituted a string
  with a different `djb2`, the parameter would still be created (keyed by its new `id`) but would be
  tagged `semantic = -1`. That silently loses the semantic classification for that slot.
- **The name column is stored into the asset.** The string is handed to the parameter setter for
  field `"name"` and is also what `"id"` is computed from. Any later reader of the authored
  `VertexFormatParameters` sees the substituted name.
- Therefore a native table must carry, per slot `k ≤ 12`, the original bytes (or, formally, any
  string whose `djb2` equals `kLeafE71990Hash[k]`) — the original 13 is the only set that satisfies
  both the hash pairing and the semantic-name requirement.
- The 14th string is behaviourally invisible: no consumer reads it, and its hash 0 can never match.

The existing private fixture already enforces exactly this contract: `load_slot23_names_fixture`
recomputes `name_hash` for records 0..12 and refuses to load on a mismatch. That is the correct
native encoding of the consumer's requirement, not an arbitrary safety check.

## 6. Can a reader run before slot 23? No.

- `0xe5bda0` and `0xea7470` both start by calling `0xe11330`, which is `return [0x33533b8];`, and
  immediately dereference the result (`(*ptr+0x130)`, `(*ptr+0x278)`, …) with no null check.
- `[0x33533b8]` is written in only two places in the whole image: non-zero at `0xe11409` inside
  `0xe11340`, and zero at `0xe14d09` (the teardown path). It is read at `0xe11320`, `0xe11327`,
  `0xe11337`, `0xe11366`, `0xe14468` — five code references total.
- `0xe11340` (create-or-return) has exactly one direct call site: `0xa3ded8` inside `0xa3da30`, whose
  decompiled head calls `FUN_00011bc0("PhysX","Engine",0)` — the PhysX engine bootstrap.
- `0xa3da30` has exactly one direct call site: `0x257034` inside `0x2569d0` (the 12,621 B owner/property
  body).
- `0x2569d0` has exactly one direct call site: `0x5331e9` inside the Init-slice function `0x533100`
  (FDE `[0x533100,0x53407a)`).
- `0x533100` and `0x531d60` are rows of the vtable at `0x15c8cd8` (entries `0x15c8d18` and
  `0x15c8cf8`), installed by the workspace bootstrap `0x536afa`/`0x537fa8` and dispatched at
  `0x25aa06`; the bootstrap `0x536730` is called straight-line from `main` at `0xf15a`, after the
  constructor walker call at `0xf14d` (E1062).

So every reader execution is causally after the APEX framework exists, and the framework is created
inside PhysX/APEX init in the Init slice, downstream of the 573-entry constructor walk. Slot 23 is
entry 23 of that walk and runs 24th. There is no static path by which a reader can see the table
before it is filled.

If the table *were* read early, the failure mode would be silent, not a crash: both accessors would
return 0, `0xe70af0(ctx, nullptr)` returns `-1`, and `0xe71310` would skip the write because
`0xffffffff` is not below the row count. The ordering requirement is therefore a correctness
requirement, not a memory-safety one.

## 7. Lifetime the consumers assume

- The table is BSS (PHDR 2: `vaddr 0x1604000`, `filesz 0x17d3100`, `memsz 0x2352aa4`; `0x3353f40` is
  beyond `filesz`, so the bytes start zeroed). It is never freed, never reset, and has no destructor
  or `__cxa_atexit` reference. The only writer is `0xe71990`, which is idempotent (it rewrites all
  28 cells).
- The name strings are read-only, file-backed PHDR 0 data. They are never freed.
- Consumers re-read the table on **every** element (`0xe70a60(k)` sits inside the per-element loop)
  and re-hash the string on **every** call. They therefore assume the pointers stay valid and
  NUL-terminated, and that the hash column keeps matching the bytes, for the whole life of the
  process. Nothing in the image invalidates either.
- The one real lifetime boundary is the framework: `0xe14d09` zeroes `[0x33533b8]` at teardown, after
  which any reader would fault. The table itself outlives it.

## 8. Native-model correspondence and one latent hazard

`native/core/startup_ctors.cpp` registers `0xe71990` at index 23 with
`init_slot23_leaf_e71990`, which loads the private fixture and fails visibly (`init_failed`, runner
blocks at `0xe71990`) when it is missing. That matches the contract established here: load-time
`djb2` verification is exactly what the consumer needs.

One latent hazard, **not a live defect**: `LeafE71990::cells` stores `storage[i].data()` pointers
into `Slot23NamesFixture::storage` (a `std::array<std::string,14>`). Every `StartupState` in the tree
is a stack or heap local that is never copied or moved (`native/api/sotc_core.cpp:80`,
`native/tests/*`), so the pointers are stable today. A future move/copy of a `StartupState` after
`init_slot23_leaf_e71990` would leave short (SSO) names pointing into the old object. `native/` is
owned by another worker in this round, so this lane did not edit it; the suggested one-line contract
comment belongs in `native/core/slot23_names_fixture.hpp` or `startup_ctors.hpp`.

## 9. What is NOT claimed

- No runtime execution, no game boot, no audio, no save/load, no Metal. Nothing here executes an
  original RVA.
- No claim that the authored asset actually renders with these semantics; only that the code path
  computes `semantic = k` from `djb2(name) == hash_k` and stores the name.
- No claim about what other code does with a `VertexFormatParameters` row after authoring.
- `0xe5bda0`'s own caller is virtual, and the Init-slice bootstrap is dispatched through a vtable
  (installed by `0x536afa`/`0x537fa8`), so the ordering argument is a static, reference-complete
  argument, not a dynamic trace.
- No claim that `0xea7470` is or is not an authoring entry point beyond "it constructs the same
  context and does not read the preset slots".

## 10. Ghidra dossiers used (private)

Produced read-only from the persistent analysed project through the documented PyGhidra entry point;
no re-import, no auto-analysis, no program saved.

| File | Contents |
|---|---|
| `local/validation/CPU-01-SLOT23-CONSUMERS-01/ghidra/slot23-consumers-dossier.txt` | `0xe5bda0` (with the APEX strings), `0xe70a60`, `0xe70a80`, `0xe70aa0`, `0xe70af0`, `0xe71310`, `0xe715d0`, `0xe11330` |
| `…/ghidra/slot23-context-dossier.txt` | `0xe71610`, `0xe714c0`, `0xe713b0`, `0xe70a20` |
| `…/ghidra/slot23-framework-bootstrap-dossier.txt` | `0xa3da30` (PhysX init), `0xea7470`, `0xea6c7e` |
| `…/ghidra/slot23-string-copy-dossier.txt` | `0xe368c0` (parameter setter helper), `0xe71500` |

Command (per dump; `JAVA_HOME`, `PATH` and `GHIDRA_INSTALL_DIR` set as in `docs/GHIDRA_PIPELINE.md`):

```sh
./local/venv-pyghidra/bin/python tools/re/ghidra/run_dump.py \
    --project-dir local/ghidra/persistent-20261020 --project-name SotC \
    --script tools/re/ghidra/dump_functions.py \
    --out local/validation/CPU-01-SLOT23-CONSUMERS-01/ghidra/<name>.txt <rvas…>
```

Each run exited 0 and printed `WROTE …`; the dossier header reports `eboot.elf`,
`x86:LE:64:default`, `image_base 0x0`, source SHA-256 `d0723afb…61d3`, 38,845 functions.

## 11. Checker

`tools/lift/check_slot23_consumers.py` — new, stdlib + `scripts/ps4_dynlink.py`, offline.

It pins: the ELF SHA-256; the 9 relevant bodies by SHA-256; both accessor encodings including their
`0x3353f40` displacements, guards and `+0x00`/`+0x08` loads; the absence of any 4- or 8-byte literal
of the table base in the image; the context vtable's `+0x28/+0x30/+0x38/+0x40/+0xa8` slots; the
`0xe70af0` virtual sites and the exactly-13 preset compares; the 8 consumer call sites plus the
`cmpq $0xd` loop bound and `0x28` stride; `0xe71990`'s table-only reachability (entry 23 of the
backward table, `-1` sentinel below entry 572); optionally the `all.dis` reference census
(3 references, exact addresses, call counts), the FDE extents, and the private fixture against the
original ELF bytes and hashes.

Results:

| Test | Result |
|---|---|
| `python3 -I tools/lift/check_slot23_consumers.py` (private inputs present) | exit 0, `OK` |
| Copy without `local/`, `local/fixtures`, `all.dis`, `fde_ranges.json` | exit 0, `SKIP` |
| Copy present but private ELF absent | exit 0, `SKIP` |
| Mutated byte inside `0xe70a60` (digest gate disabled in the copy) | exit 1, body-SHA failure |
| Mutated vtable slot `13f0+0x30` | exit 1, `0xe70a81 != 0xe70a80` |
| Mutated fixture record 5 first byte | exit 1, `fixture record 5 differs from 0x131c9e4` |

The whole-file digest gate intentionally fires before the structural pins when the real ELF is
mutated; the structural pins were exercised by disabling only that gate in a throwaway copy.

## 12. Files changed / next action

- `docs/checkpoints/2026-11-01-cpu01-slot23-consumers.md` (this file, new).
- `tools/lift/check_slot23_consumers.py` — authored by this lane and **already committed by the
  integrator** in `a0e65a3d` while this checkpoint was being written; the committed blob SHA-256
  equals the working file (`765d7648…8068b`), so nothing is left uncommitted.
- `docs/TASKS.md` — `CPU-01-SLOT23-CONSUMERS-01` row: the integrator's interim closure text (which
  cited **E1065**, the `0xa4bddb` stragglers row) is replaced with the full determination and the
  correct ledger ID **E1067**.
- `docs/EVIDENCE.md` — **E1067** added; E1063–E1066 were already used when this was written
  (E1065 = `0xa4bddb` stragglers, E1066 = vtable evidence grades), so the task prompt's "E1063" was
  stale.

Documentation defect recorded, not hidden: the integrator's `a0e65a3d`/`dbfde513` commit messages
label this result "E1065", which collides with the existing stragglers row. Commit messages are
immutable; the ledger row and the task row now both say E1067.

Not edited (by instruction or ownership): `HANDOFF.md`, `native/`, `reference/`, `tools/dev/*`,
`tools/lift/indirect_frontier.py`, `docs/entry_map.json`, `docs/FOLDERS.md`, `docs/INDEX.md`.

Next exact action: none for this task. The only follow-up worth a separate row is the native
lifetime contract comment in §8 (the `StartupState` move/copy restriction), which needs the
`native/` owner. No gate moved: G1 stays `PARTIAL`, G2–G6 `NOT RUN`.
