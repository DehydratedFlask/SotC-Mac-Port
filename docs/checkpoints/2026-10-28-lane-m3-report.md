# Lane m3 report: startup constructor slots 426 436 439 440 441 456 457 458 459 461 (ten slots)

Date: 2026-10-28. Lane: m3. Source ELF: `local/re/eboot.elf`, sha256
`d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3` (read-only, verified by every checker).
Nothing was loaded, relocated or executed from the original; vtable pointers, functor tags, handler tags and
destructor RVAs are identity tags (destructors are recorded in `StartupAtexitLog`, never run). No git add/commit/tag/push.
No existing file was edited; `native/CMakeLists.txt` is untouched (the lane fragment `native/lanes/m3.cmake` is picked up
by its existing lane glob).

## Status

All ten slots are recovered: STATIC_ANALYSIS_VERIFIED (ELF byte-oracle checkers, every body fully CFG-walked with no
unresolved edge in any slot body) + FIXTURE_VERIFIED (native fixtures). No slot was skipped. Registration into
`StartupState` is NOT done (integrator task, wiring below). Boot/audio/gameplay are not claimed.

| Slot | RVA | Size | What it constructs | Checker | Test (ctest) |
|---|---|---|---|---|---|
| 426 | 0x53fb50 | 336 | bool var `showGameStateValues` (0/0, tag 0x15c8e98) + zeroed 16-byte static, atexit 0x539a00 | `check_ctor_slot_426.py` | `ctor_slot_426` |
| 436 | 0x50dbe0 | 410 | bool var `shadow_showDOFStats` (0/0, tag 0x15c6498) + 0x26db0 toggle under `Depth of Field` | `check_ctor_slot_436.py` | `ctor_slot_436` |
| 439 | 0x4f7b70 | 410 | bool var `r_waterWetHelperDebugDraw` (0/0, tag 0x15c4a50) + 0x26db0 toggle under `Water` | `check_ctor_slot_439.py` | `ctor_slot_439` |
| 440 | 0x4f0c60 | 362 | command `shadow_GallerySkipToNextAudioTrack` (handler tag 0x4eeaf0) + four `0xa2ddc0` hashes to dwords at 0x2efc170 | `check_ctor_slot_440.py` | `ctor_slot_440` |
| 441 | 0x4e68c0 | 410 | bool var `cp10_debugSwordLight` (0/0, tag 0x15c2c20) + 0x26db0 toggle under `Shadow Player` | `check_ctor_slot_441.py` | `ctor_slot_441` |
| 456 | 0x471950 | 323 | bool var `wwise_postSounds` (1/1, tag 0x15bc748) | `check_ctor_slot_456.py` | `ctor_slot_456` |
| 457 | 0x45c440 | 323 | bool var `showCollisionMeshTests` (0/0, tag 0x15bad48) | `check_ctor_slot_457.py` | `ctor_slot_457` |
| 458 | 0x45ace0 | 323 | bool var `showFootDrivenMotion` (0/0, tag 0x15bab98) | `check_ctor_slot_458.py` | `ctor_slot_458` |
| 459 | 0x43f460 | 299 | bool var `SM_printAnimPhase` (0/0, tag 0x15b70d0) | `check_ctor_slot_459.py` | `ctor_slot_459` |
| 461 | 0x419480 | 323 | bool var `drawCameraControllerInfo` (1/1, tag 0x15b5198) | `check_ctor_slot_461.py` | `ctor_slot_461` |

## Observations (all pinned by bytes in the checkers)

- Every bool variable is the already-native inline sequence of `init_bool_var_static`: `0x20af0(obj, name, desc="", flags 1)`,
  vptr stores (0x15940b0, then 0x1594218), `value_ref = obj+0xc0`, zero cells, functor tag at `+0xb0`, `f590(desc)`,
  value byte at `+0xc0` then default byte at `+0x68`, `0x20d60`, `f590(name)`, `__cxa_atexit(0x20b0, obj, dso)`.
  The 323 B family (456/457/458/461) addresses the object rip-relative (no base register), which changes only the encoding
  (323 B vs the 299 B base-register form of 348/459); no store differs. 459 is byte-for-byte the 348 shape (299 B).
  All five bool functor tags are valid bool functor vtables (four slots = 0xc2a0/0xc2c0/0xc2e0/0xc2f0, relocs checked).
- 436/439/441 (410 B): the same bool variable followed by the slot-344 toggle sequence: `0x26db0(item, &parent_view, var, 0x30)`
  (caption = the variable name, tail-jumps to 0x268b0), `f590(parent)`, `__cxa_atexit(0x272d0, item, dso)`. The three
  bodies differ only by object addresses, name, parent string and functor tag.
- 426: after the bool var and its atexit, one `vmovups xmm0(0) -> [0x2eff438]` and `__cxa_atexit(0x539a00, 0x2eff438, dso)`.
  0x539a00 (83 B, sha `aff9c9eb...1f1d`) reads a control-block pointer at `cell+8`, `lock xadd` decrements its use count
  and on the last reference calls control-block virtual slots `+0x10` / `+0x18`.
- 440: a console command built exactly like the slot 306/343 commands (`0xf2f0` name view, 0x41-byte description,
  flags 2, delegate fn cell 0x15fe5e0, `call [0x1594dc0+0x20]`, `f590` x2, atexit 0x4b0), then four calls of `0xa2ddc0`
  (`mov esi,0x811c9dc5; jmp 0x162020`, both body hashes pinned) on `UI_OPTION_GALLERY_AUDIO_PLAYLIST_{NONE,LOOP_SINGLE,
  LOOP_ALL,RANDOM}`, results stored to `dword [0x2efc170 + 0/4/8/0xc]` (0x2efc170 = command object + 0xa0).

## Inference only (not proven)

- 426: the 16-byte cell is an empty shared-pointer-like `{ptr, ctl}`; the destructor's indirect calls are unreachable for
  an empty cell and are NOT modelled. The destructor is hash-pinned and recorded, never run. The cell has no identified consumer.
- 440: the four dwords are string ids of the gallery playlist options; their consumer is not identified. The handler body at
  0x4eeaf0 is NOT recovered (identity tag only, as for 0x7d6420 / 0x7d2c20). The native hash uses the documented
  ASCII-`toupper` policy for the imported `_Getptoupper` (fnv_path_cstr_162020.hpp); that is a project policy, not a byte
  fact. The test expectations equal an independent reference FNV with the same fold.
- The bool/command names are debug console variables (names suggest debug draw/stat toggles); no gameplay meaning is claimed.
- Every `f590` release in these slots is on a borrowed view (ctl == nullptr), so it is an exact no-op; an owned view
  makes the existing helpers report `ok == false`.

## Commands and results (2026-10-28)

```sh
cmake -S native -B local/build/lane-m3 -DSOTC_LANES=m3                                       # exit 0
cmake --build local/build/lane-m3 -j 4 --target test_ctor_slot_{426,436,439,440,441,456,457,458,459,461}   # exit 0
ctest --test-dir local/build/lane-m3 -R "ctor_slot_(426|436|439|440|441|456|457|458|459|461)"   # 10/10 passed
python3 -I tools/lift/check_ctor_slot_<N>.py           # N in the ten slots: exit 0 each
python3 -I tools/lift/check_ctor_slot_<N>.py --tamper  # flips one body byte in memory: exit 1 each (10/10)
```

Mutation self-test results (ELF mutants + native mutants, all rejected): 426 99+18; 436 135+15; 439 135+15; 440 92+27;
441 135+15; 456 113+12; 457 113+12; 458 113+12; 459 87+12; 461 113+12. `tools/lift/run_all_checkers.sh`, the
full-tree build, `audit_publish_paths.py` and `project_index.py` were NOT RUN in this lane (integrator).

## Files created

- `native/core/ctor_slot_<N>_<rva>.{hpp,cpp}` for the ten slots (no new shared helper was needed: the pair cell of 426 and
  the hash dwords of 440 live in their own slot modules; everything else reuses `ctor_shapes_343_356`, `concmd_vars`,
  `fconcmd_object_slot306`, `debug_menu_item`, `fnv_path_cstr_162020`)
- `native/tests/test_ctor_slot_<N>.cpp` (ten; reuse `tests/l4_test_util.hpp`)
- `tools/lift/check_ctor_slot_<N>.py` (ten; reuse `l4_slot_lib.py`, extra callee hashes registered per checker)
- `native/lanes/m3.cmake`, `docs/checkpoints/2026-10-28-lane-m3-report.md`

## Gaps

- Not registered in `StartupState` / the constructor table (integrator).
- Handler body 0x4eeaf0, the 426 cell consumer and the 440 hash consumers are not recovered.

## Integrator wiring

Includes (startup_ctors.hpp):

```cpp
#include "ctor_slot_426_53fb50.hpp"
#include "ctor_slot_436_50dbe0.hpp"
#include "ctor_slot_439_4f7b70.hpp"
#include "ctor_slot_440_4f0c60.hpp"
#include "ctor_slot_441_4e68c0.hpp"
#include "ctor_slot_456_471950.hpp"
#include "ctor_slot_457_45c440.hpp"
#include "ctor_slot_458_45ace0.hpp"
#include "ctor_slot_459_43f460.hpp"
#include "ctor_slot_461_419480.hpp"
```

`StartupState` members:

```cpp
    Slot426Objects slot426{}; Slot426Result slot426_result{};
    Slot436Objects slot436{}; Slot436Result slot436_result{};
    Slot439Objects slot439{}; Slot439Result slot439_result{};
    Slot440Objects slot440{}; Slot440Result slot440_result{};
    Slot441Objects slot441{}; Slot441Result slot441_result{};
    Slot456Objects slot456{}; Slot456Result slot456_result{};
    Slot457Objects slot457{}; Slot457Result slot457_result{};
    Slot458Objects slot458{}; Slot458Result slot458_result{};
    Slot459Objects slot459{}; Slot459Result slot459_result{};
    Slot461Objects slot461{}; Slot461Result slot461_result{};
```

Registration rows (startup_ctors.cpp; slot index = constructor-table index, word `e.u64(0x2DD70E0 - 8*N) == rva`, verified
for all ten):

```cpp
        {0x53fb50, 426, [](StartupState& s) noexcept { s.slot426_result = init_slot426_53fb50(s.slot426, s.concmd); if (!s.slot426_result.ok) s.init_failed = true; }},
        {0x50dbe0, 436, [](StartupState& s) noexcept { s.slot436_result = init_slot436_50dbe0(s.slot436, s.concmd, s.debug_menu); if (!s.slot436_result.ok) s.init_failed = true; }},
        {0x4f7b70, 439, [](StartupState& s) noexcept { s.slot439_result = init_slot439_4f7b70(s.slot439, s.concmd, s.debug_menu); if (!s.slot439_result.ok) s.init_failed = true; }},
        {0x4f0c60, 440, [](StartupState& s) noexcept { s.slot440_result = init_slot440_4f0c60(s.slot440, s.concmd); if (!s.slot440_result.ok) s.init_failed = true; }},
        {0x4e68c0, 441, [](StartupState& s) noexcept { s.slot441_result = init_slot441_4e68c0(s.slot441, s.concmd, s.debug_menu); if (!s.slot441_result.ok) s.init_failed = true; }},
        {0x471950, 456, [](StartupState& s) noexcept { s.slot456_result = init_slot456_471950(s.slot456, s.concmd); if (!s.slot456_result.ok) s.init_failed = true; }},
        {0x45c440, 457, [](StartupState& s) noexcept { s.slot457_result = init_slot457_45c440(s.slot457, s.concmd); if (!s.slot457_result.ok) s.init_failed = true; }},
        {0x45ace0, 458, [](StartupState& s) noexcept { s.slot458_result = init_slot458_45ace0(s.slot458, s.concmd); if (!s.slot458_result.ok) s.init_failed = true; }},
        {0x43f460, 459, [](StartupState& s) noexcept { s.slot459_result = init_slot459_43f460(s.slot459, s.concmd); if (!s.slot459_result.ok) s.init_failed = true; }},
        {0x419480, 461, [](StartupState& s) noexcept { s.slot461_result = init_slot461_419480(s.slot461, s.concmd); if (!s.slot461_result.ok) s.init_failed = true; }},
```
