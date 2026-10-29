# Lane M1 report: startup constructor slots 359 364 365 376 377 383 384 385 386 387

Date: 2026-10-28. Agent: worker lane m1. Task: CPU-01-STARTUP-SLOTS-M1 (offline native lift, L4 style).
Original ELF: `local/re/eboot.elf` sha256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3` (read-only).
Gate labels: all ten slots `FIXTURE_VERIFIED` (native fixture) + `STATIC_ANALYSIS_VERIFIED` (ELF byte oracle). Nothing is
claimed about boot, audio, saving or gameplay. No original RVA is executed; vtable/functor/handler values are identity tags.

## Result

All ten slots recovered. None skipped. No hypothesis failed (0 failed tries per slot).

| Slot | RVA | Size | Kind | Content (observation) |
|---|---|---|---|---|
| 359 | 0x785f60 | 296 | console command | `giveItem`, obj 0x3255c30, handler tag 0x7836d0, 0x49-byte description |
| 364 | 0x74a8e0 | 497 | float var + slider item | unbounded float `cp10_WanderSheathCollisionSwordScalar` (obj 0x32525b0, caller functor 0x15cd240) + 0x27050 item `CP10-Anim` / `WanderSheathCollisionSwordScalar`, range 0.01..0.1 (item 0x3252688) |
| 365 | 0x747870 | 299 | bool var | `nico_freezeBoss`, tag 0x15cd220, obj 0x32524c8, value 0 |
| 376 | 0x6e4930 | 299 | bool var | `skippableEndSequence`, tag 0x15cc9b8, obj 0x32415d0, value 0 |
| 377 | 0x6de050 | 299 | bool var | `pauseGameIfControllerDisconnected`, tag 0x15cc998, obj 0x3240cd0, value 1 |
| 383 | 0x6c62b0 | 386 | bool var + toggle | `shadow_debugDrawCloth` (tag 0x15cc7d8, obj 0x3239648, value 0) + 0x26db0 toggle under `Shadow Cloth` (item 0x3239710) |
| 384 | 0x6b9260 | 299 | bool var | `shadow_debugDrawDamageBox`, tag 0x15cc7b8, obj 0x3238dc0, value 0 |
| 385 | 0x6b4cf0 | 296 | console command | `gCP10ExtractCP3SERandomTables`, obj 0x3238340, handler tag 0x6b4600, 0x25-byte description |
| 386 | 0x6b12f0 | 296 | console command | `shadow_printCurrentlyLoadedSheetData`, obj 0x3017de0, handler tag 0x6aee10, empty description |
| 387 | 0x6acf00 | 299 | bool var | `CP3FileResource_copy_mem`, tag 0x15cc748, obj 0x3017890, value 1 |

## Observations versus inference

Observation (read from the pinned bytes and re-verified by each checker): constructor-table word `0x2dd70e0 - 8*idx` equals the
body RVA; whole-body sha256 and extent; padding; every rip-relative operand, call/jmp target, immediate store (including the
string-length immediates) and indirect call; the referenced strings; the four vtable slots of every caller functor; the sha256
of every shared callee body (0x20af0, 0x20d60, 0xf590, bool/float/command destructors, functor bodies, 0x6aa00, 0x26db0, 0x27050,
0x268b0 chain, ...). The 0x27050 slider range comes from two rodata floats (0x127f9ec = 0x3c23d70a, 0x127f9f0 = 0x3dcccccd),
loaded into xmm0 (min) and xmm1 (max) at 0x74aa6e / 0x74aa27, checked as `imm` pins.

Inference (documented, not provable from bytes alone):
* Slots 365/376/377/384/387 equal the slot 348 shape and 359/385/386 the slot 343/344 command shape, from normalised
  disassembly diffs plus the pinned callee hashes; the existing native helpers (`init_bool_var_site`, `init_fconcmd_object`)
  are therefore reused unchanged.
* Slot 364: its caller functor `0x15cd240` has four vtable slots byte-pinned to float functor bodies
  0xc4a0/0xc4c0/0xc4e0/0xc4f0, matching the lifted float tables. The shared native whitelist now accepts this tag directly;
  `init_float_var_static` receives the original tag, applies the same safe native model, and stores final class vptr
  `0x1594318`. The slot-specific checker still pins the caller tag and its four table slots.
* The slot-383 toggle caption is the variable name view (0x26db0 shape), as at slot 344.

Still open (not modelled): the command handler bodies (0x7836d0, 0x6b4600, 0x6aee10), the consumers of every variable, and the
meaning of the 0x15cd220 / 0x15cc* bool functor tags beyond their byte-checked slot bodies.

## Files created (only new files; no existing file edited)

* native/core/ctor_slot_{359_785f60,364_74a8e0,365_747870,376_6e4930,377_6de050,383_6c62b0,384_6b9260,385_6b4cf0,386_6b12f0,387_6acf00}.{hpp,cpp}
* native/core/lane_m1_float_item.{hpp,cpp} (namespace `sotc::lane_m1`; used by slot 364 only)
* native/tests/test_ctor_slot_{359,364,365,376,377,383,384,385,386,387}.cpp
* tools/lift/check_ctor_slot_{359,364,365,376,377,383,384,385,386,387}.py (use `tools/lift/l4_slot_lib.py` unchanged; 364 adds the
  `debug_item_float_ctor` entry (0x27050, 0xd0 bytes, sha256 ad7ca05e...b053) to `lib.COMMON_CALLEES` at runtime)
* native/lanes/m1.cmake (the lane CMake fragment)
* docs/checkpoints/2026-10-28-lane-m1-report.md (this file)

## Commands and results (all run from the workspace root)

```sh
cmake -S native -B local/build/lane-m1 -DSOTC_LANES=m1
cmake --build local/build/lane-m1 -j 4 --target test_ctor_slot_359 test_ctor_slot_364 test_ctor_slot_365 test_ctor_slot_376 \
  test_ctor_slot_377 test_ctor_slot_383 test_ctor_slot_384 test_ctor_slot_385 test_ctor_slot_386 test_ctor_slot_387
ctest --test-dir local/build/lane-m1 -R "ctor_slot_(359|364|365|376|377|383|384|385|386|387)" --output-on-failure
for i in 359 364 365 376 377 383 384 385 386 387; do python3 -I tools/lift/check_ctor_slot_$i.py; python3 -I tools/lift/check_ctor_slot_$i.py --tamper; done
```

* Build: clean (no errors). ctest: 10/10 passed (PASSED, FIXTURE_VERIFIED).
* Checkers: 10/10 exit 0; `--tamper` exit 1 for all 10. Mutation self-tests (ELF + native mutants, all rejected):
  359: 54+14; 364: 127+26; 365: 87+13; 376: 87+13; 377: 87+13; 383: 109+15; 384: 87+13; 385: 54+14; 386: 53+13; 387: 87+13.
* NOT RUN by this lane: the full-tree build/ctest (`-DSOTC_LANES=*`), `tools/lift/run_all_checkers.sh`, the publish/attribution audits,
  `scripts/project_index.py`. The integrator owns those.

## Integrator wiring

Table slots are ascending-index rows in `native/core/startup_ctors.cpp` (see the L4 rows for 343..356). Signatures were verified
against `startup_ctors.hpp/.cpp` (`s.concmd`, `s.debug_menu`, `s.init_failed`). Slots 364 and 383 take `s.debug_menu`; the rest do not.

### 1. Includes (native/core/startup_ctors.hpp, next to the other `ctor_slot_*` includes)

```cpp
#include "ctor_slot_359_785f60.hpp"
#include "ctor_slot_364_74a8e0.hpp"
#include "ctor_slot_365_747870.hpp"
#include "ctor_slot_376_6e4930.hpp"
#include "ctor_slot_377_6de050.hpp"
#include "ctor_slot_383_6c62b0.hpp"
#include "ctor_slot_384_6b9260.hpp"
#include "ctor_slot_385_6b4cf0.hpp"
#include "ctor_slot_386_6b12f0.hpp"
#include "ctor_slot_387_6acf00.hpp"
```

### 2. StartupState members (native/core/startup_ctors.hpp)

```cpp
Slot359Objects slot359{}; Slot359Result slot359_result{};
    Slot364Objects slot364{}; Slot364Result slot364_result{};
    Slot365Objects slot365{}; Slot365Result slot365_result{};
    Slot376Objects slot376{}; Slot376Result slot376_result{};
    Slot377Objects slot377{}; Slot377Result slot377_result{};
    Slot383Objects slot383{}; Slot383Result slot383_result{};
    Slot384Objects slot384{}; Slot384Result slot384_result{};
    Slot385Objects slot385{}; Slot385Result slot385_result{};
    Slot386Objects slot386{}; Slot386Result slot386_result{};
    Slot387Objects slot387{}; Slot387Result slot387_result{};
```

### 3. CtorSlot rows (native/core/startup_ctors.cpp, at the matching index positions)

```cpp
{0x785f60, 359, [](StartupState& s) noexcept { s.slot359_result = init_slot359_785f60(s.slot359, s.concmd); if (!s.slot359_result.ok) s.init_failed = true; }},
        {0x74a8e0, 364, [](StartupState& s) noexcept { s.slot364_result = init_slot364_74a8e0(s.slot364, s.concmd, s.debug_menu); if (!s.slot364_result.ok) s.init_failed = true; }},
        {0x747870, 365, [](StartupState& s) noexcept { s.slot365_result = init_slot365_747870(s.slot365, s.concmd); if (!s.slot365_result.ok) s.init_failed = true; }},
        {0x6e4930, 376, [](StartupState& s) noexcept { s.slot376_result = init_slot376_6e4930(s.slot376, s.concmd); if (!s.slot376_result.ok) s.init_failed = true; }},
        {0x6de050, 377, [](StartupState& s) noexcept { s.slot377_result = init_slot377_6de050(s.slot377, s.concmd); if (!s.slot377_result.ok) s.init_failed = true; }},
        {0x6c62b0, 383, [](StartupState& s) noexcept { s.slot383_result = init_slot383_6c62b0(s.slot383, s.concmd, s.debug_menu); if (!s.slot383_result.ok) s.init_failed = true; }},
        {0x6b9260, 384, [](StartupState& s) noexcept { s.slot384_result = init_slot384_6b9260(s.slot384, s.concmd); if (!s.slot384_result.ok) s.init_failed = true; }},
        {0x6b4cf0, 385, [](StartupState& s) noexcept { s.slot385_result = init_slot385_6b4cf0(s.slot385, s.concmd); if (!s.slot385_result.ok) s.init_failed = true; }},
        {0x6b12f0, 386, [](StartupState& s) noexcept { s.slot386_result = init_slot386_6b12f0(s.slot386, s.concmd); if (!s.slot386_result.ok) s.init_failed = true; }},
        {0x6acf00, 387, [](StartupState& s) noexcept { s.slot387_result = init_slot387_6acf00(s.slot387, s.concmd); if (!s.slot387_result.ok) s.init_failed = true; }},
```

### 4. CMake

The fragment `native/lanes/m1.cmake` is picked up by the existing `SOTC_LANES` glob (default `*`), so no CMakeLists edit is
needed. If the integrator folds it into `native/CMakeLists.txt` instead, copy its `target_sources(sotc_core PRIVATE ...)` list
(it also contains `core/lane_m1_float_item.cpp`) and the ten `add_executable/target_link_libraries/add_test` triples.

### 5. Conflicts to watch

* Object addresses are independent of the other lanes; nothing in this lane writes shared state beyond the registries it receives.
* The direct shared whitelist integration is covered by `test_ctor_slot_364` and
  `test_ctor_shapes_343_356`; callers remain bounded to the lifted shared initializer.
