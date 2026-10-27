# Startup constructor slot 306 semantics (CPU-01-STARTUP-SLOT-306-SEMANTICS-01)

Date: 2026-10-25. Static, read-only analysis; no original code was executed. Evidence is private under
`local/re/evidence/startup-slot-306-semantics-20261025/` (not published). Status: **STATIC_ANALYSIS_VERIFIED**
(symbolic walk of the straight-line body from the pinned ELF), not a native module and not a gate result.

Pinned ELF `local/re/eboot.elf` SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`.
Body `[0xa633b0, 0xa654bd)`, 1568 instructions, listing from the earlier indirect-edge survey
(`docs/checkpoints/2026-10-25-slot306-indirect-edges.md`). The walk executed 1567 of 1568 instructions (the
unexecuted one is the cold `__stack_chk_fail` call at `0xa654b8`), recorded 208 call events plus the `ret`
(209 calls in the listing = 198 direct + 11 indirect), and found 0 stores it could not attribute.

Labels: **OBS** = read from the pinned bytes or the simulated event stream; **INF** = inference.

## Result

Slot 306 is the `sndx_` (sound-engine debug) console-variable block. The body is one straight-line
sequence (the only branches are the stack protector and three skippable `je`; no loops). It constructs
**46 global objects**: 1 pointer cell, 3 ranged int vars,
5 ranged float vars, 11 bool vars, 3 console commands and
23 debug-menu items
(3 groups, 4 separators, 12 toggles,
3 float sliders, 1 command item). Per-object status:
RESOLVED 35, PARTIAL 11, UNKNOWN 0. Every object has a known global,
helper, argument values, registration call and atexit record (`__cxa_atexit` `0xf8f390`, dso `0x1604218`);
PARTIAL means a helper or destructor body is not yet lifted, never a missing argument (reasons below).

## Table (all 46 objects, in construction order)

`Global` is the object address (RVA). `Name` is the registered console name read from the ELF; menu items
have no console name (their strings are the parent path and caption). `Args` are the observed values.

| # | Site | Helper | Global | Name | Args | atexit dtor | Status |
|---|------|--------|--------|------|------|-------------|--------|
| 0 | 0xa633ef | inline store | 0x32f8ff0 | - | qword 0 | 0xa5be70 | RESOLVED |
| 1 | 0xa63497 | 0xa624a0 | 0x32f8ff8 | sndx_listenerOrientation | def 0, min 0, max 1, flags 1 | 0xa5bea0 | RESOLVED |
| 2 | 0xa63567 | 0xa624a0 | 0x32f90d0 | sndx_listenerPosition | def 0, min 0, max 1, flags 1 | 0xa5bea0 | RESOLVED |
| 3 | 0xa63662 | 0xc300 | 0x32f91a8 | sndx_listenerPlayerOffset | def 0 (0x0), min 0, max 100, flags 1 | 0x20f0 | RESOLVED |
| 4 | 0xa63725 | 0x20af0 + inline | 0x32f9280 | sndx_drawListenerPosition | def false, value false, flags 1, desc "" | 0x20b0 | RESOLVED |
| 5 | 0xa6380f | 0x20af0 + inline | 0x32f9348 | sndx_showEnvironmentInfo | def false, value false, flags 1, desc "" | 0x20b0 | RESOLVED |
| 6 | 0xa638f3 | 0x20af0 + inline | 0x32f9410 | sndx_showWorldAmbientInfo | def false, value false, flags 1, desc "" | 0x20b0 | RESOLVED |
| 7 | 0xa639d2 | 0x20af0 + inline | 0x32f94d8 | sndx_showTouchVolumes | def false, value false, flags 1, desc "" | 0x20b0 | RESOLVED |
| 8 | 0xa63ab1 | 0x20af0 + inline | 0x32f95a0 | sndx_showDuckingInfo | def false, value false, flags 1, desc "" | 0x20b0 | RESOLVED |
| 9 | 0xa63b90 | 0x20af0 + inline | 0x32f9668 | sndx_showGlobalInstVars | def false, value false, flags 1, desc "" | 0x20b0 | RESOLVED |
| 10 | 0xa63c6f | 0x20af0 + inline | 0x32f9730 | sndx_onlyUseLastCore | def false, value false, flags 1, desc "" | 0x20b0 | RESOLVED |
| 11 | 0xa63dc3 | 0xc300 | 0x32f9810 | sndx_masterVolume | def 1 (0x3f800000), min 0, max 1, flags 1 | 0x20f0 | RESOLVED |
| 12 | 0xa63e8b | 0x20af0 + inline | 0x32f98e8 | sndx_attachSndxDebugger | def false, value false, flags 1, desc "" | 0x20b0 | RESOLVED |
| 13 | 0xa63f8a | 0x20af0 + inline | 0x32f99b0 | sndx_showBanks | def false, value false, flags 1, desc "" | 0x20b0 | RESOLVED |
| 14 | 0xa640c7 | 0xa62860 | 0x32f9a78 | sndx_showActive | def 0, min 0, max 2, flags 1 | 0xa5bee0 | RESOLVED |
| 15 | 0xa64158 | 0x20af0 + inline | 0x32f9b50 | sndx_showActive3d | def false, value false, flags 1, desc "" | 0x20b0 | RESOLVED |
| 16 | 0xa64257 | 0x20af0 + inline | 0x32f9c18 | sndx_showSystemInfo | def false, value false, flags 1, desc "" | 0x20b0 | RESOLVED |
| 17 | 0xa643ad | 0xc300 | 0x32f9ce0 | sndx_debugGlobalAlpha | def 1 (0x3f800000), min 0, max 1, flags 1 | 0x20f0 | RESOLVED |
| 18 | 0xa644c9 | 0xc300 | 0x32f9db8 | sndx_debugLingerTime | def 0 (0x0), min 0, max 10, flags 1 | 0x20f0 | RESOLVED |
| 19 | 0xa645ec | 0xc300 | 0x32f9e90 | sndx_debugLingerFadeTime | def 0.5 (0x3f000000), min 0, max 10, flags 1 | 0x20f0 | RESOLVED |
| 20 | 0xa646bc | 0x20af0 + inline | 0x32f9f68 | sndx_reloadData | flags 2, handler 0xa5bf20 | 0x4b0 | PARTIAL |
| 21 | 0xa647b7 | 0x20af0 + inline | 0x32fa008 | sndx_categoryMuteAll | flags 2, handler 0xa5bf50 | 0x4b0 | PARTIAL |
| 22 | 0xa648a3 | 0x20af0 + inline | 0x32fa0a8 | sndx_categoryFilterAll | flags 2, handler 0xa5c020 | 0x4b0 | PARTIAL |
| 23 | 0xa64986 | 0x26c10 | 0x32fa148 | - | parent "", caption "Sound", flags 0x30 | 0x272d0 | PARTIAL |
| 24 | 0xa64a1a | 0x27050 | 0x32fa1f0 | - | parent "Sound", caption "Master Volume", range 0.01..0.1, target 0x32f9810, flags 0x30 | 0x272d0 | RESOLVED |
| 25 | 0xa64a93 | 0x26c10 | 0x32fa298 | - | parent "Sound", caption "Category Mute", flags 0x30 | 0x272d0 | PARTIAL |
| 26 | 0xa64af1 | 0x26b60 | 0x32fa340 | - | parent "Sound", flags 0x30 | 0x272d0 | PARTIAL |
| 27 | 0xa64b66 | 0x26ea0 | 0x32fa3e8 | - | parent "Sound", caption "Show System Info", target 0x32f9c18, flags 0x30 | 0x272d0 | RESOLVED |
| 28 | 0xa64bf8 | 0x26ea0 | 0x32fa490 | - | parent "Sound", caption "Show Banks", target 0x32f99b0, flags 0x30 | 0x272d0 | RESOLVED |
| 29 | 0xa64c7c | 0x26ea0 | 0x32fa538 | - | parent "Sound", caption "Show Environment Info", target 0x32f9348, flags 0x30 | 0x272d0 | RESOLVED |
| 30 | 0xa64d00 | 0x26ea0 | 0x32fa5e0 | - | parent "Sound", caption "Show World Ambient Info", target 0x32f9410, flags 0x30 | 0x272d0 | RESOLVED |
| 31 | 0xa64d84 | 0x26ea0 | 0x32fa688 | - | parent "Sound", caption "Draw Touch Volumes", target 0x32f94d8, flags 0x30 | 0x272d0 | RESOLVED |
| 32 | 0xa64e08 | 0x26ea0 | 0x32fa730 | - | parent "Sound", caption "Show Ducking Info", target 0x32f95a0, flags 0x30 | 0x272d0 | RESOLVED |
| 33 | 0xa64e8c | 0x26ea0 | 0x32fa7d8 | - | parent "Sound", caption "Show Global Inst Vars", target 0x32f9668, flags 0x30 | 0x272d0 | RESOLVED |
| 34 | 0xa64eea | 0x26b60 | 0x32fa880 | - | parent "Sound", flags 0x30 | 0x272d0 | PARTIAL |
| 35 | 0xa64f5f | 0x26ea0 | 0x32fa928 | - | parent "Sound", caption "Show Active", target 0x32f9a78, flags 0x30 | 0x272d0 | RESOLVED |
| 36 | 0xa64fea | 0x26ea0 | 0x32fa9d0 | - | parent "Sound", caption "Show Active 3d", target 0x32f9b50, flags 0x30 | 0x272d0 | RESOLVED |
| 37 | 0xa65066 | 0x26c10 | 0x32faa78 | - | parent "Sound", caption "Category Filters", flags 0x30 | 0x272d0 | PARTIAL |
| 38 | 0xa650fa | 0x27050 | 0x32fab20 | - | parent "Sound", caption "Inactive Linger Time", range 0.1..0.5, target 0x32f9db8, flags 0x30 | 0x272d0 | RESOLVED |
| 39 | 0xa65152 | 0x26b60 | 0x32fabc8 | - | parent "Sound", flags 0x30 | 0x272d0 | PARTIAL |
| 40 | 0xa651cb | 0x26ea0 | 0x32fac70 | - | parent "Sound", caption "Draw Listener Position", target 0x32f9280, flags 0x30 | 0x272d0 | RESOLVED |
| 41 | 0xa65256 | 0x26ea0 | 0x32fad18 | - | parent "Sound", caption "Listener Orientation", target 0x32f8ff8, flags 0x30 | 0x272d0 | RESOLVED |
| 42 | 0xa652da | 0x26ea0 | 0x32fadc0 | - | parent "Sound", caption "Listener Position", target 0x32f90d0, flags 0x30 | 0x272d0 | RESOLVED |
| 43 | 0xa6536e | 0x27050 | 0x32fae68 | - | parent "Sound", caption "Player Position Offset", range 0.1..1, target 0x32f91a8, flags 0x30 | 0x272d0 | RESOLVED |
| 44 | 0xa653c6 | 0x26b60 | 0x32faf10 | - | parent "Sound", flags 0x30 | 0x272d0 | PARTIAL |
| 45 | 0xa65466 | 0x26f90 | 0x32fafb8 | - | parent "Sound", caption "Reload All Sound Data", target 0x2dfcbd0, cmd "sndx_reloadData", flags 0x30 | 0x272d0 | PARTIAL |

Status reasons:

- RESOLVED root cell, ranged ints, floats, bools: shapes match the native `concmd_vars` / `ctor_slot_301`
  patterns; int vars `0xa624a0` and `0xa62860` take (default, min, max) **by pointer** (OBS) and differ only in
  vtable/functor constants (OBS, normalized disassembly diff); both finish by calling `0x20d60` themselves.
- RESOLVED menu toggles (12) and float sliders (3): the native `debug_menu_item` module covers `0x26ea0`/`0x27050`.
  Slider range = the two float args stored at `+0x98`/`+0x9c` (OBS: `xmm0` -> `+0x98`, `xmm1` -> `+0x9c`); for the
  Master Volume slider those are 0.01 and 0.1 (bit patterns `0x3c23d70a`, `0x3dcccccd`). Whether the pair is a
  min/max or min/step is INF-open (the native module already names them as an explicit range).
- PARTIAL menu groups (`0x26c10`, 3), separators (`0x26b60`, 4) and command item (`0x26f90`, 1): ctor bodies read, but no
  native module exists for these three shapes.
- PARTIAL console commands (3): construction fully recovered (below); the callback bodies, invoker thunk `0x1990`
  and the non-null command-manager path of `0x20d80` are not lifted. At startup the manager cell is null, so
  registration only appends the node to the pending list (OBS from `concmd_base`).

## Per-shape side-effect contract

Each object follows one fixed template (OBS, uniform across all instances of the shape). Strings are built as
borrowed views `{cstr, strlen, 0, 0}` on stack temporaries (three names go through `0xf2f0`, the rest are inline stores);
`0xf590` is the shared-string release. Order within a shape:

1. **Root cell** `0x32f8ff0`: store qword 0; `__cxa_atexit(0xa5be70, 0x32f8ff0, dso)`.
2. **Ranged int var** (3): `0xf730()` yields the shared empty string (the description); build the name view;
   `0xa624a0` (or `0xa62860`) `(obj, &name, &default, &min, &max, flags=1)` with the description on the stack; the
   helper stores vptr/functor, value and default and (INF: no `0x20d60` call event follows in the body, and the 16 `0x20d60` events are exactly the 5 floats and 11 bools) registers the object itself; then the argument functor is
   destroyed through its vtable (`[tag + 0x18]`, tag `0x15d1fa8` for the two listener vars, `0x15d2080` for
   `sndx_showActive`); `0xf590(&name)`; `__cxa_atexit(dtor, obj, dso)` with dtor `0xa5bea0` (listener vars) or
   `0xa5bee0` (`sndx_showActive`).
3. **Ranged float var** (5): `0xc300(obj, &name, &valueCell(obj+0xd0), &min, &max, flags=1)` with a stack functor of
   tag `0x15d2040` and an empty description view; then `0xf590(&description view)`, the functor destroy through
   `[0x15d2040 + 0x18]`, direct `0x20d60(obj)`, `0xf590(&name)`, `__cxa_atexit(0x20f0, obj, dso)`. Default (`+0x68`)
   and initial value (`+0xd0`) are stored in the object (bit patterns are in the table; `sndx_masterVolume` and `sndx_debugGlobalAlpha` default 1.0,
   `sndx_debugLingerFadeTime` 0.5, `sndx_listenerPlayerOffset` and `sndx_debugLingerTime` 0.0).
4. **Bool var** (11): `0x20af0(obj, &name, &desc(empty), flags=1)`; inline stores: vptr `0x15940b0` then `0x1594218`,
   `+0x60` = address of own value byte (`obj+0xc0`), `+0x68` = 0 (default), `+0xb0` = tag `0x15d2060`, `+0xc0` = 0
   (value); `0xf590(&desc)`; direct `0x20d60(obj)`; `0xf590(&name)`; `__cxa_atexit(0x20b0, obj, dso)`. All 11 have
   default false (OBS from the store stream).
5. **Console command** (3): `0xf2f0` builds the name view; `0x20af0(obj, &name, &desc, flags=2)`; inline stores: vptr
   `0x1594dd0`, delegate pointer `+0x60` = `obj+0x60` (its own inline storage), `+0x68` = invoker thunk `0x1990`, `+0x78` =
   callback RVA, `+0x70/+0x80/+0x88/+0x90` = 0; registration is the **indirect** call `[0x1594dc0 + 0x20]` = `0x20d60`;
   `0xf590(&desc)`; `0xf590(&name)`; `__cxa_atexit(0x4b0, obj, dso)`.
6. **Menu item** (23): build the parent-path view and caption view; call the ctor (`0x26c10`, `0x26b60`, `0x26ea0`,
   `0x27050`, `0x26f90`) with flags `0x30`; each ctor tail-jumps into `0x268b0`, which inserts the item into the list rooted
   at `0x2dfd4c0` because the parent cell `[0x2dfd4b8]` is null at startup (the root guard `0x2dfd4e8` and its
   atexit `0x27280` were done by slot 299); `0xf590(&caption)` (not for separators); `0xf590(&path)`;
   `__cxa_atexit(0x272d0, obj, dso)`.

Strings, targets and defaults per object are in the table; menu targets are the console-variable objects above
(for example Master Volume -> `0x32f9810`, Show Banks -> `0x32f99b0`) and the command item targets `0x2dfcbd0` with command
name `sndx_reloadData`.

## Exact ordered sequence

Construction order is the table order (`#` 0 to 45), each object's side effects being contiguous (the shapes above).
In summary: root cell; `sndx_listenerOrientation`, `sndx_listenerPosition` (ints); `sndx_listenerPlayerOffset` (float);
`sndx_drawListenerPosition` to `sndx_onlyUseLastCore` (6 bools); `sndx_masterVolume`; `sndx_attachSndxDebugger`,
`sndx_showBanks`; `sndx_showActive` (int); `sndx_showActive3d`, `sndx_showSystemInfo`; `sndx_debugGlobalAlpha`,
`sndx_debugLingerTime`, `sndx_debugLingerFadeTime` (floats); the three commands `sndx_reloadData`,
`sndx_categoryMuteAll`, `sndx_categoryFilterAll`; then the 23 menu items (the "Sound" group first, then its children).
The registration order of console objects is therefore the table order restricted to #1..#22; the menu order is #23..#45.
The pending list order and FIFO drain follow from `concmd_base` (not re-derived).

## Does slot 306 depend on anything outside already-native helpers?

Yes (INF from the native tree plus OBS of the callees). Already native: `0x20af0`, `0x20d60`/`0x20d80`, `0xc300`,
the inline bool shape, `0x26ea0`, `0x27050`, `0xf730` and the `SharedStr` basics, `0xf590` as a model, `0xf8f390` as the host atexit.
Not yet native (required to reproduce slot 306): the int-var ctors `0xa624a0`/`0xa62860` and their workers
(`0xa62660`/`0xa62a20`, same shape), the command-object shape (vptr `0x1594dd0`, thunk `0x1990`), the menu ctors
`0x26c10`, `0x26b60`, `0x26f90`, and the `0x268b0` kind-1/kind-3 branches. No call target in the body is a Sony-module import other than the `__cxa_atexit` PLT (`0xf8f390`) and the
cold `__stack_chk_fail`; the bodies read so far do not touch audio. The body itself performs no load of mutable engine
state (OBS from the event stream); the sound-engine pointer cell is stored as null.

## Helper bodies still unread or only partly read

- Command callbacks `0xa5bf20`, `0xa5bf50`, `0xa5c020` and their callees (`0xa5f380`, `0x1e4c0`, `0xba40`); they run only when a command is invoked, not at startup.
- Destructors: `0x4b0` (command), `0xa5bea0`/`0xa5bee0` (int vars, partly read), `0xa5cb30`, `0x20b0`/`0x20f0`/`0x272d0` (pinned, not executed), `0xa5be70` (read).
- `0x27470` (menu parent insert) and `0x3e7e0` are not reached with flags `0x30` (INF from flag arithmetic).
- `0x26ea0` virtual slots 3 and 6 for the int-var targets (the bool targets are `0xb890`/`0xb930`).
- `0x20d80` non-null manager path and invoker thunk `0x1990`.

## Evidence

Private directory `local/re/evidence/startup-slot-306-semantics-20261025/` (listing, simulator, event stream, table, helper disassemblies; the latter contain game bytes and are never published).
SHA-256: `simulate_slot306.py` `152ca38828ce52f362e48ed461da77e697cfe3a2cc778aa5db065e37c7324f6f`; `sim_events.json` `5554ecb10be5489f82064545dc5acbc5f2753d748c05b9abfabb1bca83b6ea50`; `slot306_objects.json` `b0715f3bbd1cf73a74517fb88d8e50cd44e2dea49987599a100090ceb50a57de`.
Commands: `python3 -I -B simulate_slot306.py` (exit 0), `python3 -I -B build_table.py` (exit 0), `python3 -I -B make_doc.py` (exit 0), all with no network and no execution of original code.

Not done: no native module, no test, no edit of tracked files, no gate claim.
