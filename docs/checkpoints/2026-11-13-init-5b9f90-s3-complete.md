# `0x5b9f90` S3 complete: S3.3–S3.6 execute (9,274 of 46,881 B)

Task `CPU-01-INIT-5B9F90-SLICES-01`, evidence E1434. Continuation of commit `d39985c2` on branch
`cursor/init-hash-551140-6aa93e8`. Everything below is measured from the pinned ELF
(`local/re/eboot.elf`, sha256 `d0723afb…5861d3`) by `tools/lift/slice_5b9f90.py` and re-derived by
`tools/lift/check_init_5b9f90_slices.py`.

## Resumed exactly where the handover said

Resume offset `0x5bb4be` = S3.3 "Shadow Cheat". The page is straight-line item clusters; every tuple
was recovered from the bytes before any code was written:

| page | range | bytes | items |
|---|---|---|---|
| S3.3 "Shadow Cheat" | `0x5bb4be..0x5bbd2e` | 2,160 | 8 toggles, 5 commands, 4 float ranges, 18 destroys, 24 releases |
| S3.4 "Shadow Trophy" | `0x5bbd2e..0x5bc377` | 1,609 | 4 guarded bool variables + named items, 5 destroys, 10 releases |
| S3.5 "Shadow Render" | `0x5bc377..0x5bc3c7` | 80 | group only |
| S3.6 "MLAA" | `0x5bc3c7..0x5bc3d4` | 13 | group only |

- **S3.3 toggles:** `No_Damage`/`0x3265a20`, `Unlimited_Grip`/`0x324c408`,
  `Max_Attack_Damage`/`0x3265ae8`, `Unlock All Gallery Items`/`0x2fed798`, `Give All Items`/
  `0x2fed0b8`, `Equip All Items`/`0x2fed298`, `Unequip All Items`/`0x2fed338`,
  `Enable Debug Fly`/`0x325dd78`. The item label is the page's **parent** view `"CP10"`, not the
  caption — the body passes `%r14` (the parent view at `0x1fe8(%rsp)`) to every item.
- **S3.3 commands:** `Set Game Completed`→`"1"`/`0x2fed3d8`, `Clear Game Completed`→`"0"`,
  `Set Game Hard Mode`→`"1"`/`0x2fed478`, `Set Game Normal Mode`→`"0"`,
  `Set Game Easy Mode`→`"1"`/`0x2fed518`.
- **S3.3 floats:** `30Hz Acceleration Scalar`/`0x300fea8`, `30Hz Acceleration Scalar Easy Mode`/
  `0x300ff80`, `60Hz Acceleration Scalar`/`0x3010058`, `60Hz Acceleration Scalar Easy Mode`/
  `0x3010130`; every site loads `0x12662fc` (0.01f) into `xmm0` and `0x1266300` (0.1f) into `xmm1`.
- **S3.4 variables:** `0x2fed838` "Show Remaining Fruit Trees" (guard `0x2fed8f8`), `0x2fed900`
  "Log Gex Tails Cut" (`0x2fed9c0`), `0x2fed9c8` "Debug Horse Stunts" (`0x2feda88`),
  `0x2feda90` "Show Visited Save Points" (`0x2fedb50`). Each block: guard acquire `0xf8f3d0` →
  `0x20af0` base ctor (name view + empty description) → vptr `0x1594348`, value ref, default,
  functor tag `0x15cb1d8` → `__cxa_atexit(0xacf0)` → `0x20d60` → guard release → `0x26db0` item →
  two view releases. This is a **different ConCmd class** from the slot-299/301 bool variable
  (`0x1594218`/`0x20b0`), so `init_bool_var_static` was not reused; the model performs the same
  sequence explicitly.
- **Destroy blocks:** S3.3 destroys its 18 items (reverse order) plus the group and releases the
  parent view (`0x5bbcca`); S3.4 destroys 5 and releases its parent. 23 destroys in total.

## Native model

`native/core/init_5b9f90_slices.{hpp,cpp}` now has a uniform item table
(`Init5b9f90ItemKind`: command / toggle / float_item / named_bool_var) and a page table with item
and destroy counts. The existing models do the work: `init_menu_group_26c10`,
`init_menu_command_26f90`, `init_debug_menu_item_26ea0`, `init_debug_menu_item_27050`,
`init_debug_menu_item_26db0`, `init_concmd_base_20af0`, `register_concmd_20d60`,
`register_debug_menu_item_268b0`.

Host-owned inputs, each named in the header: 8 toggle bool stand-ins (the global variables are
constructed by unlifted engine init; the target identity is the measured bool-variable pair
slot3 = 1 / slot6 = 0), 4 `ConCmdBoolVar` objects with their value and guard bytes, the menu root,
the ConCmd registry, the atexit log, the item storage (64 slots) and the canary cell.

Execution totals: **7 groups, 60 menu items, 60 menu registrations, 4 ConCmd registrations,
106 releases, 23 destroys, 7 slice records, 9,274 B (19.8 %)**. Execution stops at `0x5bc3d4`.

## The two page builders

- `0x2170` "Shadow Misc": scanned `0x2170..0x9000`, **no `ret`** and no `jmp`; 4,565 instructions
  and 825 direct calls in that window (170 × `0x26ea0`, 50 × `0x27050`, 18 × `0x26b60`,
  254 × `0x272d0`, 258 × `0xf590`). The extent and page decomposition are not closed, so S1 stays
  **contract**; claiming bytes would be a guess.
- `0x5cf140` "Shadow Credits": **5,526 B** (`0x5cf140..0x5d06d6`, single `ret` at `0x5d06d6`),
  932 instructions, 132 direct calls, **no backward branch**. Shape: group + one command +
  separators + 2 bool variables + 10 `0xc300` bounded floats each with a `0x27120` slider.
  Every callee has a native model, so the body is liftable in principle, but the linear scanner does
  not recover its stack-slot views or the xmm pairs at the ten `c300`/`27120` blocks (the recovered
  arg dump is visibly wrong: `?` labels, shifted call indices). No byte-pinned tuple table exists,
  so S2 stays **contract** — the precise missing piece is a pre-call view/constant tracer for this
  body, not host data and not an unmodelled callee.

## Verification

- `cmake -S native -B local/build/orch-r22` → 0; `cmake --build … -j 12` → 0.
- `ctest --test-dir local/build/orch-r22 -j 12` → **704/704** (`test_init_5b9f90_slices` extended to
  7 groups / 60 items / 106 releases / 23 destroys / 5 atexit records).
- `python3 -I tools/lift/check_init_5b9f90_slices.py` → OK, `executed=9274B items=53`.
- `sotc_init_window` → exit 0, stderr 0 B, `ok=true`, `post_chain_complete=true`,
  `post_call_count=24`, `post_calls_executed=23`, `post_calls_blocked=1`,
  `post_calls_complete=false`, boss `0xA7`, resume `0x5bc3d4`.
- `tools/lift/run_all_checkers.sh` → **914 passed / 0 skipped / 0 failed**, exit 0.
- `tools/lift/gen_entry_map.py` → 3,028 constants → 1,942 RVAs / 584 headers (455 duplicates);
  `scripts/project_index.py` → 114,663; publish audit `--all` → 0; attribution → OK.

No boot, audio, save/reload or Metal claim: G1 **PARTIAL**, G2/G5/G6 **NOT RUN**.
