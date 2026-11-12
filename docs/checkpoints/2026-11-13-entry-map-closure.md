# 2026-11-13 — Guest entry map typed against the pinned ELF

Task `CPU-01-ENTRY-MAP-CLOSURE-01`, continuation of `CPU-01-AOT-CFG-SCALE-01` (M1: whole-program CFG
closure and a guest-address → native-entry map). Branch `cursor/init-hash-551140-6aa93e8`.

## What was missing

`docs/ENTRY_MAP.md` listed which RVAs our headers name but not what each address is; its own "next
step" was to intersect the map with the CFG census's 55,790 FDE ranges. Without that, a dispatcher
could not tell a function entry from a branch label, a PLT stub or a vtable.

## What was built

- `tools/lift/entry_closure.py` parses `.eh_frame_hdr` (0x150c08c, encodings checked) into the FDE
  universe, parses the PT_LOAD map, types every distinct RVA and intersects the `0x531d60` closure.
  It writes `docs/entry_closure.json`, `docs/ENTRY_CLOSURE.md` and the generated
  `native/core/guest_entry_table.inc`.
- `native/core/guest_entry_table.{hpp,cpp}` expose `guest_entry_find(rva)`,
  `guest_entry_counts()` and `guest_entry_kind_name()`. The table is compile-time checked: 1,957
  strictly sorted rows.
- `native/tests/test_guest_entry_table.cpp` pins the measured counts and one RVA per kind plus the
  unknown-address negative. The Init runtime JSON now carries an `entry_map` block with the counts
  and seven lookups.
- `tools/lift/check_entry_closure.py` re-derives everything from the pinned ELF and compares both
  artifacts byte-for-byte; it runs in the checker sweep.

## Measured

| kind | entries |
| --- | ---: |
| function start | **830** |
| function interior (branch label/literal) | **381** |
| PLT stub | **12** |
| data in R+X segment | **203** |
| image data (non-exec PT_LOAD) | **531** |
| outside image | **0** |
| total | **1,957** |

Target/Rva-named addresses: 115 distinct → 75 starts, 31 interior labels, 1 PLT, 8 data.
Init closure: **141 of 608 functions (23.2 %)** have an exact native entry; 467 remain unmodelled.
Runtime lookups: `0x531d60`/`0x5b9f90` function_start, `0x1353` function_interior, `0xf8f390` plt,
`0x1014a80` executable_data, `0x1594d90` image_data, `0x12345678` unknown.

## Gates

build 0; full CTest **705/705**; entry closure checker **OK**; sweep **915/0/0**; runtime exit 0,
stderr 0 B, `ok=true`; init slice totals unchanged at **46,876 / 108,325 B** with only `0x2170` and
the `0x7f8664` geometry block open.

## Limits

FDE-derived only; no ABI/thunk, no indirect-target resolution (105 sites in the closure remain
unresolved), no execution of the guest image. Cold boot, audio, save/reload and Metal validation stay
**NOT RUN**; G1 stays **PARTIAL**.
