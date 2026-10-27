# L1-TINY lane report: 42 tiny constructor slots (all bodies <= 100 B)

Date: 2026-10-26. Lane: L1-TINY (worker; no commit/tag/push, `HANDOFF.md`, `TASKS.md`, `EVIDENCE.md`, `INDEX.md`,
`startup_ctors.{hpp,cpp}` untouched). Task area: CPU-01 startup-constructor recovery.

## Result summary

| Group | Slots | Status | Label |
|---|---|---|---|
| A: zero static object(s) + `__cxa_atexit` | 311 425 432 471 515 522 523 525 527 528 531 533 542 546 547 549 551 564 565 (19) | new module `ctor_tiny_atexit_slots` | FIXTURE_VERIFIED + STATIC_ANALYSIS_VERIFIED (ELF byte oracle) |
| B: miscellaneous | 404 406 409 414 494 517 519 535 541 553 562 563 (12) | new module `ctor_tiny_misc_slots` | FIXTURE_VERIFIED + STATIC_ANALYSIS_VERIFIED; 409/414/494/535 PARTIAL (host hooks, see Gaps) |
| C: already lifted leaves | 315 323 341 357 378 394 412 416 496 500 560 (11) | no new code; checkers re-run green | STATIC_ANALYSIS_VERIFIED / FIXTURE_VERIFIED (pre-existing) |

31 new slots recovered natively, 11 reused. No slot skipped; no try counter above 1 (the only failed tries were tooling
errors listed under "Failed experiments"). None of the 42 is registered yet (integrator wiring below).

## Files created

- `tools/lift/tiny_slot_decode.py` (offline straight-line x86-64 effect decoder; fails on any opcode outside the
  tiny-body subset) sha256 `b41d356e...3b30`
- `native/core/ctor_tiny_atexit_slots.{hpp,cpp}`, `native/tests/test_ctor_tiny_atexit_slots.cpp`,
  `tools/lift/check_ctor_tiny_atexit_slots.py`
- `native/core/ctor_tiny_misc_slots.{hpp,cpp}`, `native/tests/test_ctor_tiny_misc_slots.cpp`,
  `tools/lift/check_ctor_tiny_misc_slots.py`
- `native/CMakeLists.txt`: two blocks APPENDED with `cat >>` (targets `test_ctor_tiny_atexit_slots`,
  `test_ctor_tiny_misc_slots`; ctest names `ctor_tiny_atexit_slots`, `ctor_tiny_misc_slots`).
- this report.

File sha256 (prefixes, as of this report): atexit.hpp `66850229`, atexit.cpp `307c92e1`, misc.hpp `897e0b31`,
misc.cpp `c94b21c5`, atexit test `0b93d3e1`, misc test `637e0769`, atexit checker `a8be055d`, misc checker `b3168b4f`,
decoder `b41d356e`.

## Method (observation versus inference)

OBSERVED (ELF `local/re/eboot.elf`, sha256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`):

- The constructor table is stored backwards: slot i is the u64 at ELF FILE offset `0x2dda750 + (306 - i) * 8`
  (a file offset, not a vaddr; the checkers read it from the raw bytes).
- `__cxa_atexit` is PLT `0xf8f390` (NID `tsvEmnenz48`), dso handle `0x1604218`; memset PLT `0xf8f4f0`
  (`8zTFvBIAIN8`); `sceKernelGetTscFrequency` PLT `0xf8f860` (`1j3S3n-tTW4`). Resolved through GOT relocations.
- Every body was decoded from bytes by `tiny_slot_decode.py` (stores, xchg, calls, tail jumps). Slot 563 contains a
  compare/branch and is pinned instruction by instruction instead.

INFERRED (labelled so in the module headers):

- A destructor RVA passed to `__cxa_atexit` is an identity tag; native code records `(dtor_rva, object)` in
  `StartupAtexitLog` and never calls it. The original `__cxa_atexit` runs destructors at exit; the native product has no
  exit-time dispatch of original RVAs.
- BSS objects are modelled by caller-owned state structs (the original addresses are identity only).
- Hook callees (`0x17070` allocator, `0x3b310` mutex init, `0x26e340`, `0x26e770`) are not lifted; the code fails closed
  (`ok == false`, `blocked` names the stage) when a hook is absent or reports failure.

## Group A (19 slots), `core/ctor_tiny_atexit_slots`

Shape: `lea` object/dtor/dso, zero `N` bytes of a static (offset 0 or 8, length 8, 16 or 24), `__cxa_atexit(dtor, obj,
dso)` as a call or tail jump. Table `kTinyAtexitRows` (19 rows, 20 atexit records; slot 425 has two records):

```
slot rva(body)   size  object(s)                       dtor(s)              zero(off,len)
311  0xa46b70    37    0x32f7630                       0xa46620             cell
425  0x54b9c0    77    0x2eff448 / 0x2eff450           0x53fcb0 / 0x53fce0  8 B each
432  0x51bc20    42    0x2efc930                       0x51b3a0             (0,24)
471  0x3600f0    43    0x2ef54d0                       0x35efb0             (8,24)
515 0x1c00e0 | 522 0x16e960 | 523 0x16cd50 | 525 0x166ef0 | 527 0x157e60 | 528 0x156fb0 | 531 0x1442c0 |
533 0x143300 | 542 0xfb3d0 | 546 0xa25f0 | 547 0x9a970 | 549 0x96650 | 551 0x8d740 | 564 0x31220 | 565 0x2fcf0
   (all 33 B, 8-byte zero + atexit; cells/dtors: 0x2ee68c8/0x1bdae0, 0x2ee3e10/0x16cda0, 0x2ee3e08/0x16b940,
    0x2ee3cd8/0x163ad0, 0x2ee3c20/0x156ff0, 0x2ee3c18/0x1561c0, 0x2ee2cf8/0x144290, 0x2ee2a50/0x143110,
    0x2edef60/0xfaf80, 0x2ed37c0/0xa1da0, 0x2ed37b0/0x994c0, 0x2ed3670/0x95620, 0x2ed33f0/0x8d620,
    0x2dfd678/0x2fd20, 0x2dfd4b8/0x26290)
```

The 15 thirty-three-byte rows also agree with the pre-existing `verify_cleanup_thunks.py` ENTRIES (cross-checked in the
checker). API: `init_tiny_atexit_slot(slot, TinyAtexitState&, StartupAtexitLog*)`; null log gives
`blocked = "atexit_log"`, unknown slot `blocked = "unknown_slot"`, both before any write.

## Group B (12 slots), `core/ctor_tiny_misc_slots`

```
404 0x5e7c10 19B  tail memset(0x30061a0, 0, 0x51f8)
406 0x5dbe10 92B  ctor_8bd90(0x30015c8); atexit(0x8bdb0,..); ctor_8bd90(0x30015f0); atexit(0x8bdb0,..)
                  ctor_8bd90: [+0x10]=0, [+0x18..0x28)=16 B rodata 0xf9ebd0 (lo 0x1d00000000, hi 0xffffffff00000000)
409 0x5b50f0 89B  zero head/size (0x2fda210, 16 B); node=0x17070(0x40); node left/parent/right=node,
                  color=1, isnil=1; head=node; atexit(0x5b3d10, 0x2fda208)
414 0x57b9b0 72B  0x3b310(0x2f35898); atexit(0x3b380,..); zero u32 +8; zero 16 B +0x10
494 0x26f7c0 84B  0x26e340(0x2eef038); atexit(0x269590,..); 0x26e770(0x2eef058); atexit(0x269610,..)
517 0x1aa510 56B  [0x2ee6720]=0; atexit(0x1aa070,..); tail 0x326a0(0x2ee6728, 0): flag byte=0,
                  TSC guard/base 0x2ea4700/0x2ea4708 (shared with 0x323d0), stamp, [+0x10]=0
519 0x19d2d0 83B  zero 24 B at 0x2ee4548; atexit(0x19a7b0,..); memset(0x2ee4568,0,0x2000); xchg [0x2ee6570],0
535 0x138e20 96B  0x3b310(0x2ee14f0); atexit(0x3b380,..); zero [0x2ee14f8+8, +0x20); atexit(0x138370, 0x2ee14f8)
541 0xfd210  52B  [0x2edef68]=fnv(0x162020)("wind", 0x811c9dc5)=0x167de847;
                  [0x2edef6c]=fnv("mesh_instance")=0xd9e6aae6
553 0x08bd40 72B  struct 0x2ec0120: [+0]=[+8]=[+0x20]=&[+0x28], [+0x10]=0x2ed1f48; atexit(0x8a620, struct)
562 0x036100 50B  [0x2ea4718]=0; atexit(0x33280,..); xchg dword [0x2ea4720],6
563 0x032c70 72B  0x323d0 inlined: cmp qword [0x2ea4700],0 / hook 0xf8f860 / base 0x2ea4708 / stamp 0x2ea4710
```

Facts worth the integrator's attention:

- 563 uses a QWORD compare on the guard (`48 83 3b 00`), identical in shape to 0x323d0, so it reuses
  `store_tsc_323d0` and the process-wide cells. (I briefly modelled a dword compare from a mis-decoded byte and reverted it
  before it reached a test; the checker pins `48833b00`.)
- 517 and 563 share the process-wide TSC cells `0x2ea4700/0x2ea4708` with 296/323/etc.; a repeated run only refreshes the
  stamp.
- `0x17070(0x40)` leaves the node's non-listed bytes unwritten (the test fills the block with 0xA5 and proves it).

## Verification (all run in this session)

Own build dir `local/build/L1` (project flags, ASan+UBSan on arm64):

```
cmake -S native -B local/build/L1                                  exit 0
cmake --build local/build/L1 -j 4 --target test_ctor_tiny_atexit_slots test_ctor_tiny_misc_slots   exit 0 (0 warnings, 0 errors)
ctest --test-dir local/build/L1 -R ctor_tiny --output-on-failure   2/2 passed (ctor_tiny_atexit_slots, ctor_tiny_misc_slots)
python3 -I tools/lift/check_ctor_tiny_atexit_slots.py              exit 0: 19 slots, 20 atexit records, 20 distinct destructors
python3 -I tools/lift/check_ctor_tiny_misc_slots.py                exit 0: 12 slots, 10 distinct destructors
SOTC_CHECK_TAMPER=1 python3 -I tools/lift/check_ctor_tiny_atexit_slots.py   exit 1 (slot 515 body bytes)
SOTC_CHECK_TAMPER=1 python3 -I tools/lift/check_ctor_tiny_misc_slots.py     exit 1 (slot 409 body bytes)
```

Mutation sanity: (1) `exchange(0)` -> `exchange(1)` in slot 519 makes `test_ctor_tiny_misc_slots` exit 1 (restored, passes);
(2) changing the 565 dtor in the atexit header makes its checker exit 1; (3) `kSlot562FlagValue = 6` -> `7` in the misc header makes
the misc checker exit 1 (restored, exit 0). Native header constants are compared against ELF-pinned literals, so a native
drift is fail-visible.

Group C re-run (exit codes): `check_fa3ade0.py` (315) 0, `check_f58bd10.py` (412) 0, `check_f5777a0.py` (416) 0,
`check_f24ebf0.py` (500) 0, `check_f3bd70.py` (560) 0; the other six (323 f842450, 341 f7dace0, 357 f78c760, 378 f6dde20,
394 f669450, 496 f268ce0) exit 1 without a native build ("test_hashed_name_ctors not built in local/build/native") and
exit 0 with `SOTC_NATIVE_BUILD=local/build/L1` after building `test_hashed_name_ctors` and
`test_guarded_static_helpers`. So they are green; the exit 1 is the missing-build guard, not a defect.

Not run (NOT RUN): the full repository ctest and `tools/lift/run_all_checkers.sh`; the publish-path and attribution audits;
`scripts/project_index.py`; trail rebuild. Those belong to the integrator pass.

## Failed experiments (none counted against a slot)

- Tooling only: CMake configure failed while another lane's appended block referenced a not-yet-written test (re-ran after
  it appeared); BSD `sed -i` needs `-i ''`; table read used `Image.read` on a file offset (switched to raw bytes); a stale
  incremental build made a restored mutation look failing (touch + rebuild); the dword-compare misreading of 563 (above).

## Gaps and risks (honest)

1. Slots 409, 414, 494, 535 depend on unlifted callees (`0x17070` allocator, `0x3b310` mutex init, `0x26e340`,
   `0x26e770`). They are PARTIAL: with null hooks they report `ok == false`, so registered production rows set `init_failed`
   (same precedent as owner_ctor). 409 has a `default_slot409_hooks()` malloc stand-in; 414/535 use `Slot414Hooks`.
2. Shared-cell aliasing: slot 515's cell (`0x2ee68c8`, the app-info registry pointer) and slot 565's cell (`0x2dfd4b8`,
   debug-menu parent) are state-owned stand-ins here; the integrator must pass the SAME storage that the existing
   `app_info_registry` / `debug_menu` modules use, or the zero will not be observed by them. The TSC cells ARE shared
   (process-wide) by construction.
3. Slot 541 embeds the strings "wind" and "mesh_instance" (also pinned to the ELF rodata) and calls the lifted
   `fnv_path_cstr_162020`; the checker recomputes both hashes independently.
4. Slot 553's external pointer (`0x2ed1f48`) is a state-owned stand-in object, identity only.
5. Destructor RVAs are tags; native exit-time destruction is not modelled for any slot.
6. Execution order relative to the other 300+ slots is the integrator's table order; these constructors have no cross-slot
   reads except the TSC cells and (via the cells in gap 2) shared pointers.

## Integrator wiring

Add to `native/core/startup_ctors.hpp` (includes first):

```cpp
#include "ctor_tiny_atexit_slots.hpp"
#include "ctor_tiny_misc_slots.hpp"
// plus the existing leaf headers if not already included:
// store_zero32.hpp hashed_name_ctors.hpp guarded_static_helpers.hpp zero_xmm_qword.hpp zero_xmm16.hpp
// zero_struct12.hpp clear_flag64.hpp
```

`StartupState` members (new):

```cpp
    TinyAtexitState tiny_atexit{};           // 19 slots x 2 objects (family A)
    Slot404Buffer slot404{};
    Slot406State slot406{};
    Slot409State slot409{};
    Slot414State slot414{};
    Slot494State slot494{};
    Slot517State slot517{};
    Slot519State slot519{};
    Slot535State slot535{};
    Slot541State slot541{};
    Slot553State slot553{};
    Slot562State slot562{};
    Slot563Stamp slot563{};
    // hooks (production stays null = fail closed; fixtures inject):
    Slot414Hooks slot414_hooks{};            // also used by 535
    Slot494Hooks slot494_hooks{};
    Slot409Hooks slot409_hooks = default_slot409_hooks();   // or {} to fail closed
    // existing-leaf state, only if not yet present:
    std::uint32_t slot315_cell = 0;
    std::array<NameHashCell, 3> slot323_cells{};  // 3 names {..} per the leaf header
    std::array<NameHashCell, 2> slot341_cells{};
    std::array<NameHashCell, 3> slot378_cells{};
    std::array<NameHashCell, 1> slot394_cells{};
    Pair128 slot357_cell{};
    std::uint8_t slot496_cell = 0;
    Xmm16 slot412_xmm{}; std::uint64_t slot412_q = 0;
    Xmm16 slot416_xmm{};
    Bytes12 slot500_cell{};
    std::atomic<std::uint64_t> slot560_cell{0};
```

(`NameHashCell` names come from the original string literals; see `hashed_name_ctors.hpp`/its checker for the exact
name arrays.)

Registration rows in `native/core/startup_ctors.cpp` (format `{rva, slot, lambda}`):

```cpp
// Group A, 19 rows: one lambda shape
{0xa46b70, 311, [](StartupState& s) noexcept { if (!init_tiny_atexit_slot(311, s.tiny_atexit, &s.concmd.atexit_log).ok) s.init_failed = true; }},
{0x54b9c0, 425, ... 425 ...}, {0x51bc20, 432, ...}, {0x3600f0, 471, ...}, {0x1c00e0, 515, ...}, {0x16e960, 522, ...},
{0x16cd50, 523, ...}, {0x166ef0, 525, ...}, {0x157e60, 527, ...}, {0x156fb0, 528, ...}, {0x1442c0, 531, ...},
{0x143300, 533, ...}, {0xfb3d0, 542, ...}, {0xa25f0, 546, ...}, {0x9a970, 547, ...}, {0x96650, 549, ...},
{0x8d740, 551, ...}, {0x31220, 564, ...}, {0x2fcf0, 565, ...},
// Group B
{0x5e7c10, 404, [](StartupState& s) noexcept { init_slot404_5e7c10(s.slot404); }},
{0x5dbe10, 406, [](StartupState& s) noexcept { if (!init_slot406_5dbe10(s.slot406, s.concmd.atexit_log).ok) s.init_failed = true; }},
{0x5b50f0, 409, [](StartupState& s) noexcept { if (!init_slot409_5b50f0(s.slot409, s.slot409_hooks, s.concmd.atexit_log).ok) s.init_failed = true; }},
{0x57b9b0, 414, [](StartupState& s) noexcept { if (!init_slot414_57b9b0(s.slot414, s.slot414_hooks, s.concmd.atexit_log).ok) s.init_failed = true; }},
{0x26f7c0, 494, [](StartupState& s) noexcept { if (!init_slot494_26f7c0(s.slot494, s.slot494_hooks, s.concmd.atexit_log).ok) s.init_failed = true; }},
{0x1aa510, 517, [](StartupState& s) noexcept { if (!init_slot517_1aa510(s.slot517, s.concmd.atexit_log).ok) s.init_failed = true; }},
{0x19d2d0, 519, [](StartupState& s) noexcept { if (!init_slot519_19d2d0(s.slot519, s.concmd.atexit_log).ok) s.init_failed = true; }},
{0x138e20, 535, [](StartupState& s) noexcept { if (!init_slot535_138e20(s.slot535, s.slot414_hooks, s.concmd.atexit_log).ok) s.init_failed = true; }},
{0xfd210, 541, [](StartupState& s) noexcept { init_slot541_fd210(s.slot541); }},
{0x8bd40, 553, [](StartupState& s) noexcept { if (!init_slot553_8bd40(s.slot553, s.concmd.atexit_log).ok) s.init_failed = true; }},
{0x36100, 562, [](StartupState& s) noexcept { if (!init_slot562_36100(s.slot562, s.concmd.atexit_log).ok) s.init_failed = true; }},
{0x32c70, 563, [](StartupState& s) noexcept { init_slot563_32c70(s.slot563); }},
// Group C (existing leaves; confirm exact name arrays in each leaf header/test)
{0xa3ade0, 315, [](StartupState& s) noexcept { store_zero32(s.slot315_cell); }},
{0x842450, 323, [](StartupState& s) noexcept { init_ctor_842450(s.slot323_cells, kNames842450); }},
{0x7dace0, 341, [](StartupState& s) noexcept { init_ctor_7dace0(s.slot341_cells, kNames7dace0); }},
{0x78c760, 357, [](StartupState& s) noexcept { init_ctor_78c760(s.slot357_cell); }},
{0x6dde20, 378, [](StartupState& s) noexcept { init_ctor_6dde20(s.slot378_cells, kNames6dde20); }},
{0x669450, 394, [](StartupState& s) noexcept { init_ctor_669450(s.slot394_cells, kNames669450); }},
{0x58bd10, 412, [](StartupState& s) noexcept { zero_xmm_qword(s.slot412_xmm, s.slot412_q); }},
{0x5777a0, 416, [](StartupState& s) noexcept { zero_xmm16(s.slot416_xmm); }},
{0x268ce0, 496, [](StartupState& s) noexcept { init_ctor_268ce0(s.slot496_cell); }},
{0x24ebf0, 500, [](StartupState& s) noexcept { zero_struct12(s.slot500_cell); }},
{0x3bd70, 560, [](StartupState& s) noexcept { clear_flag64(s.slot560_cell); }},
```

Notes for the integrator:

- `s.concmd.atexit_log` is the `StartupAtexitLog` already used by the slot-30x modules; for family A pass its address
  (`&s.concmd.atexit_log`); every other function takes it by reference. Group A and 517/519/535/553/562 all append to the
  same log: 20 records from group A and 12 from group B (406:2, 409:1, 414:1, 494:2, 517:1, 519:1, 535:2, 553:1, 562:1), 32 in total.
- 515 and 565: pass the real shared storage instead of the state-owned stand-in (gap 2) once the owners exist.
- Hook-dependent rows (409, 414, 494, 535) leave `init_failed == true` while their hooks are null. If that is
  unacceptable at this stage, register them only in fixtures.
- Verification after wiring: re-run the six hashed/guarded leaf checkers with `SOTC_NATIVE_BUILD` pointing at the build
  directory, and `tools/lift/run_all_checkers.sh` (globs `check_*.py`, so both new checkers are included).
