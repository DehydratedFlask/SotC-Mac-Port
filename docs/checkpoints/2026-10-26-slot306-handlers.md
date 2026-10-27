# Slot 306 console-command handlers and destructors (CPU-01-SLOT306-HANDLERS-DTORS-01)

Date: 2026-10-26. Static, read-only analysis of the pinned ELF; no original code was executed, loaded or relocated.
Status: **STATIC_ANALYSIS_VERIFIED** for the contracts below. Not a native module, not a gate result.
Private evidence (disassemblies, not published): `local/re/evidence/startup-slot-306-handlers-20261026/`
(`dis/*.dis`, `SHA256SUMS`, `manifest.json`, `extract.py`, `commands.txt`).

Pinned ELF `local/re/eboot.elf` SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`
(re-verified by `extract.py` before any read). Tool: `xcrun llvm-objdump -d` over explicit `[start,end)` extents.
Labels: **OBS** = read from the pinned bytes; **INF** = inference.

## Function hashes (SHA-256 of the raw body bytes, first 16 hex shown; full values in the private `manifest.json`)

| Function | Extent | Size | Body SHA-256 (prefix) | Role |
|---|---|---|---|---|
| `0xa5bf20` | `[0xa5bf20,0xa5bf41)` | 33 | `e77baf97d82167d5` | handler `sndx_reloadData` |
| `0xa5bf50` | `[0xa5bf50,0xa5c010)` | 192 | `f0d56f563f92bfd0` | handler `sndx_categoryMuteAll` |
| `0xa5c020` | `[0xa5c020,0xa5c0e0)` | 192 | `fa10b6c20693cecb` | handler `sndx_categoryFilterAll` |
| `0xa5be70` | `[0xa5be70,0xa5be9e)` | 46 | `944d664bcd64f875` | atexit dtor of root cell `0x32f8ff0` |
| `0xa5bea0` | `[0xa5bea0,0xa5bed6)` | 54 | `46c09fe5f28364f9` | atexit dtor, listener int vars |
| `0xa5bee0` | `[0xa5bee0,0xa5bf16)` | 54 | `4c21694d286e257b` | atexit dtor, `sndx_showActive` |
| `0xa5cb30` | `[0xa5cb30,0xa5cd82)` | 594 | `0b5c450169b29ead` | sound-engine dtor (matches native `owner_dtor_a5cb30`) |
| `0x4b0` | `[0x4b0,0x521)` | 113 | `a15bae38e545531a` | command-object dtor (matches `check_fconcmd_object_slot306.py`; not re-read) |
| `0x1990` | `[0x1990,0x199c)` | 12 | `c5eca1d65aea48ea` | delegate invoke thunk (matches existing checker) |
| `0xa5f380` | `[0xa5f380,0xa5f497)` | 279 | `2af0ccd9e83dac55` | reload callee |
| `0xba40` | `[0xba40,0xbb15)` | 213 | `d3c3ffcfe5be639b` | bool console-var setter |
| `0x1e4c0` | `[0x1e4c0,0x1e5d8)` | 280 | `a4cb3cf28cf870c1` | command-argument bool lookup |
| `0x21da0` | `[0x21da0,0x21f73)` | 467 | `792cb9b25bb7a37c` | `bpe::ConCommand::ExecuteCmd` |

Other extents dumped for the next step (hashes in the private manifest): `0xa5cd90` engine init (5775 B, builds the
category vars), `0x95580`, `0x14d2d0`, `0x989450`, `0xb096b0`, and the menu ctors `0x26b60`, `0x26c10`, `0x26f90`, `0x27050`.

## (a) Console command handlers

Common (OBS): delegate invocation is `handler(cmd, args)` through thunk `0x1990`; return is a 32-bit
`bpe::ECmdExecuteResult`. `0x21da0` treats 0 as success, 2 as "usage printed", any other non-zero as a
"failed to execute" log line. The handlers return only 0 (done) or 3 (rejected). The engine pointer is the global
cell `0x32f8ff0` (the sound-engine object built by owner ctor `0xa5c850`; torn down by `0xa5be70` -> `0xa5cb30`, then
the deallocator `0x170b0`, then the cell is cleared). The Mute/Filter handlers carry a stack-protector prologue and epilogue
(cookie pointer cell `0x15fe518`, failure call `0xf8f3a0`); that is compiler scaffolding, not behaviour.

**`0xa5bf20` sndx_reloadData** (OBS)
- Reads `[0x32f8ff0]`; if null returns 3 with no side effect.
- Else calls `0xa5f380(engine)` and returns 0. `args` is not read.
- `0xa5f380` (OBS, structure only): walks the 0x38-byte bank records at engine `+0x28..+0x30` and, for each record whose
  handle is not `-1`, calls `0x989450` (statically linked `sceSndxResourceUnregister`, not a Sony module) and
  `0x14d2d0` (`ResourceWrapper::RemoveReference`), then clears the record's loaded byte; a second pass re-references
  banks that carry a reference flag (`0x95580`); then calls `0xb096b0` (`SndxSoundTable::LoadSoundTable`) with the
  table at `+0x7270` and the table name `bpe::String` at `+0x7250`; finally clears 256 records of stride 0x40 at
  `+0x1c50` (loop bound `0x4000` bytes). Exact flag/handle offsets are in the private listing.

**`0xa5bf50` sndx_categoryMuteAll** and **`0xa5c020` sndx_categoryFilterAll** (OBS)
- The two bodies are byte-for-byte the same template; they differ only in which engine vector they walk
  (normalised diff: vector offsets, rip-relative displacements, and the early-exit branch target).
- Guard: engine cell non-null AND `(args.end - args.begin) >> 4 == 1` (args vector at `args+0x30/+0x38`, 16-byte
  entries, exactly one argument). Otherwise return 3 and touch nothing.
- Parse: `b = 0x1e4c0(args, 0, 0, 0)` (argument 0, default false, result is one byte).
- Apply: for every bool var in the engine vector, `0xba40(var, var->+0x60, &b)`; stride `0xc8`.
  Mute vector = engine `+0x8bd0..+0x8bd8`; Filter vector = engine `+0x72a8..+0x72b0`. Return 0.
- Both vectors hold 32 entries named `sndxCategory_Mute_%d` / `sndxCategory_Filter_%d`, built in the engine init
  `0xa5cd90` (OBS: format strings and the 0xc8 stride in that function).
- `0xba40(var, valueRef, &new)` (OBS): if the clamp-engaged byte `var+0x6a` is set it applies the min/max bytes
  (`+0x69`, `+0x6b`; a flag test on `+0x1c` selects the ordered-clamp variant); then, if the stored byte differs
  from the new one, it stores it through `valueRef` and invokes the change callback (`+0x70/+0x78/+0x80`, a
  this-adjusted member-function pointer with the virtual bit in `+0x78`) with `(valueRef, var, &new, &old)`. No change -> no callback.
- `0x1e4c0(args, index, default, flag)` (OBS: reads the key/value entry, compares the text against fixed tokens,
  returns the default on mismatch/absent). **Unknown (INF-open)**: whether the compare is case-sensitive; the
  repo's notes disagree on the imported NID. Native code must take this as a hook until the token table and compare
  are lifted (the token set is in the private `1e4c0_arg_bool_lookup.dis`).

## (b) Destructors

- `0xa5bea0` / `0xa5bee0` (OBS): identical bodies except one rip-relative constant. Store the var vtable
  (`0x15d1e90 + 0x10` for the listener vars, `0x15d1f08 + 0x10` for `sndx_showActive`) at `+0x00`, destroy the
  inline type-erased functor at `+0xa0` by calling slot `+0x18` of the table pointer stored at `+0xc0`, then
  tail-jump to the `ConCmdBase` destructor `0x20b50`. These run only at process exit (atexit). They are the destructors of the
  ranged int vars already modelled by `intvar_a624a0`; no new behaviour. The `+0xc0` word is the functor's own table
  pointer in this layout (the int var keeps its value elsewhere), so do not reuse the bool-var "`+0xc0` = value" reading for int vars.
- `0xa5be70` (OBS): `if (cell) { 0xa5cb30(cell); 0x170b0(cell); cell = 0; }`. This is the root-cell dtor; the native
  `owner_dtor_a5cb30` models the `0xa5cb30` half. The deallocator `0x170b0` is a hook.
- `0xa5cb30` (OBS): hash `0b5c4501...` matches the native model's pin; no new facts. Consistency obligation: the two
  vectors above and the bank/record arrays used by the reload callee must agree with the offsets that module frees.
- `0x4b0`: not re-read (covered by `tools/lift/check_fconcmd_object_slot306.py`; the hash matches).

## (c) Slider `+0x98/+0x9c`

(OBS) The slider ctor `0x27050` stores `xmm0 -> +0x98` and `xmm1 -> +0x9c`; the Master Volume slider passes
0.01 and 0.1. A search for readers of the menu list rooted at `0x2dfd4c0` found only the insert (`0x268b0`), remove and
exit-guard sites; no code in the binary reads `+0x98/+0x9c` back (negative evidence, not proof over indirect calls).
(INF) The pair is a fine/coarse step (0.01 and 0.1 on a 0..1 volume that defaults to 1.0), not a min/max. It is
construction-only data with no gameplay effect. The native `DebugMenuItem` names these `range_min`/`range_max`; the
values and offsets are right, the names are misleading. Rename in docs/comments next time that module is touched.

## PARTIAL-row resolution (rows from `2026-10-25-slot306-semantics.md`)

| Rows | Item | Result |
|---|---|---|
| 20, 21, 22 | `sndx_reloadData`, `sndx_categoryMuteAll`, `sndx_categoryFilterAll` | Handler contracts RESOLVED (above). The callees `0xa5f380`, `0x1e4c0`, `0xba40` stay named hooks; `0x1e4c0` token compare and the non-null `0x20d80` manager path remain open. `0x4b0` was already pinned. |
| slider rows (RESOLVED earlier) | `+0x98/+0x9c` | Meaning is INF step pair; no reader found. |
| 23, 25, 37 (groups), 26, 34, 39, 44 (separators), 45 (command item) | menu ctors `0x26c10`, `0x26b60`, `0x26f90`, shared `0x268b0` | NOT resolved here. Listings are dumped to the private evidence; no native module exists. |

## Remaining unknowns and risks

- `0x989450` and `0xb096b0` internals (engine/sound-table behaviour) are not lifted; only their call sites are read.
- `0x95580` is read only for its role as a per-bank re-reference; arguments beyond the bank pointer are unverified.
- `0x1e4c0` compare semantics (see above).
- Nobody consumes the debug-menu list in the binary, so menu rows have no observable effect (INF from negative search).
- Hashes cover raw body bytes only; extents come from the end-of-function inspection, with cold-tail calls included for `0x21da0`, `0x989450`, `0xb096b0`.

## Next native steps (not done by this task)

1. New module `sndx_command_handlers`: `u32 handler(cmd, args, engine_cell)` returning 0/3; guard `args.size()==1`;
   hooks `get_bool_arg` (the `0x1e4c0` semantics, compare mode explicit), `bool_var_set` (`0xba40` incl. clamp and
   callback hook), `engine_reload` (`0xa5f380` as four hooks: unregister, remove-ref, add-ref, load table).
2. `engine_category_vars`: 32 Mute and 32 Filter bool vars from the `0xa5cd90` init loop at engine `+0x8bd0` / `+0x72a8`, stride `0xc8`.
3. Wire the three handler tags in `fconcmd_object_slot306` to the new module (replace named tags only after the
   unit test covers 0, 3, null engine, argument count != 1, change/no-change callback).
4. Menu ctor modules for `0x26c10`, `0x26b60`, `0x26f90` and the kind-1/kind-3 branches of `0x268b0`.
5. Add `tools/lift/check_fconcmd_handlers_slot306.py` pinning the hashes above (full digests from the private manifest).
