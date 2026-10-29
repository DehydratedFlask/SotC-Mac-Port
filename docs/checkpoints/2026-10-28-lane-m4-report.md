# Lane M4 report: startup constructor slots 463 465 475 476 478 479 482 486 489 490

Date: 2026-10-28. Agent: worker lane m4. Task: CPU-01-STARTUP-SLOTS-M4 (offline native lift, L4 style).
Original ELF: `local/re/eboot.elf` sha256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3` (read-only).
Gate labels: all ten slots `FIXTURE_VERIFIED` (native fixture) + `STATIC_ANALYSIS_VERIFIED` (ELF byte oracle). Nothing is
claimed about boot, audio, saving or gameplay. No original RVA is executed; vtable/functor tags are identity values.

## Result

All ten slots recovered. None skipped. No hypothesis failed (0 failed tries per slot).

| Slot | RVA | Size | Kind | Content (observation) |
|---|---|---|---|---|
| 463 | 0x3e0040 | 410 | bool var + toggle | `cp10_playerClothDebug` (tag 0x15b1828, obj 0x2ef72b0, value 0) + 0x26db0 toggle under `Shadow Player` (item 0x2ef7378) |
| 465 | 0x3cc9e0 | 323 | bool var | `showLoadPackages`, tag 0x15b0fc8, obj 0x2ef6fe8, value 0 |
| 475 | 0x33cc60 | 285 | bool var (late release) | `uiTrackToTargetShowExtents`, 71-byte description, tag 0x15a93e8, obj 0x2ef0950, value 0 |
| 476 | 0x329650 | 323 | bool var | `r_showFogVolumes`, tag 0x15a7c50, obj 0x2ef0888, value 0 |
| 478 | 0x31eb00 | 285 | bool var (late release) | `uiTextShowExtents`, 69-byte description, tag 0x15a7260, obj 0x2ef04a0, value 0 |
| 479 | 0x31b660 | 299 | bool var | `uiWarnOnUnhandledEvents`, tag 0x15a6eb0, obj 0x2ef03d8, value 0 |
| 482 | 0x2fb2a0 | 323 | bool var | `network_WarnAboutServerObjectsOnClient`, tag 0x15a52f8, obj 0x2eefd30, value 1 |
| 486 | 0x2d5e00 | 462 | int var + string var | unbounded int `SM_debugging` (arg tag 0x15a2248, obj 0x2eefa70, 0/0) + string `SM_debugIdentifier` (0x257a0, obj 0x2eefb48, empty) |
| 489 | 0x2acc10 | 342 | int var | unbounded int `disableInput`, worker 0x3a980, arg tag 0x159fae0, obj 0x2eef6b0, 0/0 |
| 490 | 0x296d90 | 323 | bool var | `r_showLights`, tag 0x159e430, obj 0x2eef5e8, value 0 |

| Slot | Checker | Fixture |
|---|---|---|
| 463 | `tools/lift/check_ctor_slot_463.py` | `test_ctor_slot_463` (ctest `ctor_slot_463`) |
| 465 | `tools/lift/check_ctor_slot_465.py` | `test_ctor_slot_465` (ctest `ctor_slot_465`) |
| 475 | `tools/lift/check_ctor_slot_475.py` | `test_ctor_slot_475` (ctest `ctor_slot_475`) |
| 476 | `tools/lift/check_ctor_slot_476.py` | `test_ctor_slot_476` (ctest `ctor_slot_476`) |
| 478 | `tools/lift/check_ctor_slot_478.py` | `test_ctor_slot_478` (ctest `ctor_slot_478`) |
| 479 | `tools/lift/check_ctor_slot_479.py` | `test_ctor_slot_479` (ctest `ctor_slot_479`) |
| 482 | `tools/lift/check_ctor_slot_482.py` | `test_ctor_slot_482` (ctest `ctor_slot_482`) |
| 486 | `tools/lift/check_ctor_slot_486.py` | `test_ctor_slot_486` (ctest `ctor_slot_486`) |
| 489 | `tools/lift/check_ctor_slot_489.py` | `test_ctor_slot_489` (ctest `ctor_slot_489`) |
| 490 | `tools/lift/check_ctor_slot_490.py` | `test_ctor_slot_490` (ctest `ctor_slot_490`) |

## New shared helper (`native/core/lane_m4_shapes.{hpp,cpp}`, namespace `sotc::lane_m4`)

Everything else reuses existing helpers unchanged (`init_bool_var_site`, `init_toggle_site_26db0`, `init_string_var_257a0`,
`init_unbounded_int_var_worker_3a980`, `register_concmd_20d60`, `release_inert_view_f590`, `ConCmdRegistry::atexit_log`).

- `init_bool_var_late_release` (slots 475, 478): the bool-variable shape with a non-empty description and NO mid-body
  vptr store; both `0xf590` releases (description then name) come after `0x20d60`, unlike the 301/348 shape that releases the
  description before `0x20d60`. Sequence: final vptr, `0x20af0`, functor tag, final vptr, value, default, `0x20d60`,
  f590(description), f590(name), `__cxa_atexit(0x20b0)`.
- `init_int_var_site` (slots 486, 489): the unbounded int variable shape: final vptr 0x1594c08, worker `0x3a980`,
  f590(description), argument-functor destroy (`call [tag+0x18]` = bare `ret` 0xc770), value (obj+0xd0), default (obj+0x68),
  `0x20d60`, f590(name), `__cxa_atexit(0x18a60)`.
- Inference: the shared worker whitelist now directly accepts argument tags 0x159fae0 (489) and 0x15a2248 (486) because
  their reloc slots 0..3 = 0xc720/0xc740/0xc760/0xc770 are byte-pinned to the same bodies as the existing accepted tables.
  The native model passes each original tag through as an identity value and never calls the original table. Slot-specific
  checkers pin both caller reloc tables and the tests assert that the shared whitelist accepts them.

## What each slot does (observations; all pinned by bytes)

- Plain bool shape (465, 476, 479, 482, 490): `0x20af0` base ctor with flags 1, functor tag store, byte value/default
  stores, f590(description) before `0x20d60`, f590(name) after, `__cxa_atexit(0x20b0)`. 482 is the only one whose
  value/default are 1/1; the other four are 0/0.
- Late-release bool shape (475, 478): see helper above; descriptions are rodata strings (0xfda8b3, 0xfd8422).
- 463: bool var then the `0x26db0` menu toggle under parent `Shadow Player`; atexit order 0x20b0, 0x27280, 0x272d0.
- 489: one int var; 486: int var `SM_debugging` then string var `SM_debugIdentifier` via `0x257a0`, atexit order
  0x18a60 then 0x23c90. Both views of 486 string name/empty are borrowed.
- The four functor-class tags of every bool slot resolve (reloc) to the lifted bodies 0xc2a0/c2c0/c2e0/c2f0; the int tags
  to 0xc720/c740/c760/c770; the string functor class 0x1595068 to 0x25a50/70/90/a0.

## Observations versus inference

Observation (read from the pinned bytes and re-verified by each checker): constructor-table word `0x2dd70e0 - 8*idx` equals
the body RVA; whole-body sha256, extent and padding; every rip-relative operand, call/jmp target, immediate store (value and
default bytes are additionally tied to the native constants, plus the string-length immediates) and indirect call; the
referenced strings; the functor/class reloc tables; the sha256 of every shared callee body.

Inference (not provable from bytes alone):
- The names of the fields (value at +0xd0, default at +0x68, etc.) come from the existing native models of the shared
  helpers; this lane only checks that the bytes stored match them.
- The surrogate argument tag for the int worker (see above).
- Consumers of these variables (render, UI, network) are not recovered and not required for registration.
- Every `f590` release here is on a borrowed view (`control == nullptr`), an exact no-op; an owned view makes the fragment
  report `ok == false` (tested in the failure section of every fixture).

## Commands and results (2026-10-28)

```sh
cmake -S native -B local/build/lane-m4 -DSOTC_LANES=m4                                  # exit 0
cmake --build local/build/lane-m4 -j 4 --target test_ctor_slot_{463,465,475,476,478,479,482,486,489,490}   # exit 0
ctest --test-dir local/build/lane-m4 -R "ctor_slot_(463|465|475|476|478|479|482|486|489|490)"   # 10/10 passed
python3 -I tools/lift/check_ctor_slot_<N>.py            # exit 0 for all ten
python3 -I tools/lift/check_ctor_slot_<N>.py --tamper   # flips one body byte in memory: exit 1 for all ten
```

Checker self-test mutant counts (ELF + native, all rejected): 463 137+17, 465 115+14, 475 88+27, 476 115+14, 478 88+27,
479 91+14, 482 115+14, 486 149+42, 489 96+37, 490 115+14.
`tools/lift/run_all_checkers.sh` picks the checkers up through its `check_*.py` glob; it was not run in full in this lane.
Full-tree build, `audit_publish_paths.py` and `project_index.py` are NOT RUN here (integrator). The 486/489 checkers extend
`l4_slot_lib` at import time (extra callees, class/functor reloc pins, ELF-store-to-native-constant ties) without editing it.

## Files created

- `native/core/lane_m4_shapes.{hpp,cpp}`, `native/core/ctor_slot_<N>_<rva>.{hpp,cpp}` (ten slots)
- `native/tests/test_ctor_slot_<N>.cpp` (ten)
- `tools/lift/check_ctor_slot_<N>.py` (ten)
- `native/lanes/m4.cmake`, this report.
No existing file was edited.

## Gaps

- Not registered in `StartupState` / the constructor table (integrator).
- Consumers of the recovered variables are not recovered.
- The int-worker argument tag whitelist is surrogated, not extended (see inference).

## Integrator wiring

Includes (startup_ctors.hpp):

```cpp
#include "ctor_slot_463_3e0040.hpp"
#include "ctor_slot_465_3cc9e0.hpp"
#include "ctor_slot_475_33cc60.hpp"
#include "ctor_slot_476_329650.hpp"
#include "ctor_slot_478_31eb00.hpp"
#include "ctor_slot_479_31b660.hpp"
#include "ctor_slot_482_2fb2a0.hpp"
#include "ctor_slot_486_2d5e00.hpp"
#include "ctor_slot_489_2acc10.hpp"
#include "ctor_slot_490_296d90.hpp"
```

`StartupState` members:

```cpp
    Slot463Objects slot463{}; Slot463Result slot463_result{};
    Slot465Objects slot465{}; Slot465Result slot465_result{};
    Slot475Objects slot475{}; Slot475Result slot475_result{};
    Slot476Objects slot476{}; Slot476Result slot476_result{};
    Slot478Objects slot478{}; Slot478Result slot478_result{};
    Slot479Objects slot479{}; Slot479Result slot479_result{};
    Slot482Objects slot482{}; Slot482Result slot482_result{};
    Slot486Objects slot486{}; Slot486Result slot486_result{};
    Slot489Objects slot489{}; Slot489Result slot489_result{};
    Slot490Objects slot490{}; Slot490Result slot490_result{};
```

Registration rows (startup_ctors.cpp; slot index = constructor-table index, word `e.u64(0x2DD70E0 - 8*N) == rva`):

```cpp
        {0x3e0040, 463, [](StartupState& s) noexcept { s.slot463_result = init_slot463_3e0040(s.slot463, s.concmd, s.debug_menu); if (!s.slot463_result.ok) s.init_failed = true; }},
        {0x3cc9e0, 465, [](StartupState& s) noexcept { s.slot465_result = init_slot465_3cc9e0(s.slot465, s.concmd); if (!s.slot465_result.ok) s.init_failed = true; }},
        {0x33cc60, 475, [](StartupState& s) noexcept { s.slot475_result = init_slot475_33cc60(s.slot475, s.concmd); if (!s.slot475_result.ok) s.init_failed = true; }},
        {0x329650, 476, [](StartupState& s) noexcept { s.slot476_result = init_slot476_329650(s.slot476, s.concmd); if (!s.slot476_result.ok) s.init_failed = true; }},
        {0x31eb00, 478, [](StartupState& s) noexcept { s.slot478_result = init_slot478_31eb00(s.slot478, s.concmd); if (!s.slot478_result.ok) s.init_failed = true; }},
        {0x31b660, 479, [](StartupState& s) noexcept { s.slot479_result = init_slot479_31b660(s.slot479, s.concmd); if (!s.slot479_result.ok) s.init_failed = true; }},
        {0x2fb2a0, 482, [](StartupState& s) noexcept { s.slot482_result = init_slot482_2fb2a0(s.slot482, s.concmd); if (!s.slot482_result.ok) s.init_failed = true; }},
        {0x2d5e00, 486, [](StartupState& s) noexcept { s.slot486_result = init_slot486_2d5e00(s.slot486, s.concmd); if (!s.slot486_result.ok) s.init_failed = true; }},
        {0x2acc10, 489, [](StartupState& s) noexcept { s.slot489_result = init_slot489_2acc10(s.slot489, s.concmd); if (!s.slot489_result.ok) s.init_failed = true; }},
        {0x296d90, 490, [](StartupState& s) noexcept { s.slot490_result = init_slot490_296d90(s.slot490, s.concmd); if (!s.slot490_result.ok) s.init_failed = true; }},
```

CMake: this lane's fragment is `native/lanes/m4.cmake` (sources + ten test targets); merge its lines into `native/CMakeLists.txt` and drop the lane glob.
