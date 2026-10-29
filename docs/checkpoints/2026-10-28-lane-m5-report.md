# Lane M5 report: startup constructor slots 491 492 494 499 502 509 516 535 540 543

Date: 2026-10-28. Lane: M5. Source ELF: `local/re/eboot.elf`, sha256
`d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3` (read-only, verified by every checker).
Nothing was loaded, relocated or executed from the original; vtable pointers, functor tags and destructor RVAs
are identity tags (destructors are recorded in `StartupAtexitLog`, never run). No git add/commit/tag/push/stash
was run. No existing file was edited. Registration into `StartupState` is NOT done (integrator task).

## Status

Nine of ten slots recovered: STATIC_ANALYSIS_VERIFIED (ELF byte-oracle checkers) + FIXTURE_VERIFIED (native
fixtures). Slot 494 is SKIPPED (PARTIAL / NOT RECOVERED), reason below.

| Slot | RVA | Size | Checker | Test (ctest name) |
|---|---|---|---|---|
| 491 | 0x295f90 | 372 | `check_ctor_slot_491.py` | `test_ctor_slot_491` |
| 492 | 0x278640 | 289 | `check_ctor_slot_492.py` | `test_ctor_slot_492` |
| 494 | 0x26f7c0 | - | SKIPPED | - |
| 499 | 0x2500e0 | 323 | `check_ctor_slot_499.py` | `test_ctor_slot_499` |
| 502 | 0x249270 | 323 | `check_ctor_slot_502.py` | `test_ctor_slot_502` |
| 509 | 0x2196f0 | 431 | `check_ctor_slot_509.py` | `test_ctor_slot_509` |
| 516 | 0x1b97b0 | 177 | `check_ctor_slot_516.py` | `test_ctor_slot_516` |
| 535 | 0x138e20 | 96 | `check_ctor_slot_535.py` | `test_ctor_slot_535` |
| 540 | 0x10e390 | 488 | `check_ctor_slot_540.py` | `test_ctor_slot_540` |
| 543 | 0xfae50 | 289 | `check_ctor_slot_543.py` | `test_ctor_slot_543` |

## What each slot does (observations; all pinned by bytes)

- 491: nine zeroed pointer-sized handle cells, each followed by `__cxa_atexit(0xaaf050, &cell)`; then a 0x80 B
  object whose caption view is `"<TEXT NOT FOUND>"` (len 16), identity vptr 0x159e0f8 (= 0x159e0e8 + 0x10) and
  `__cxa_atexit(0xf7b0, &caption)` (0xf7b0 is a 73 B SharedStr destructor; the analyzer's size 30986 is wrong, the
  checker pins the measured 73 B body hash).
- 492 / 543: a 0x50 B app-info queue entry `{registered u8, name SharedStr, next, FConCmdDelegate}`: name view built by
  0xf2f0 (strlen import 0xf8f3b0), delegate `{&delegate, invoker 0x4cca0 via cell 0x15fe800, 0, handler}`, registered
  once by 0x4b560, view released by 0xf590 (no-op on a borrowed view), `__cxa_atexit(0x4b660, &entry)`.
  492 = `RequestTelemetryChannels`, handler tag 0x278300, object 0x2eef4f0.
  543 = `UpdateMaterialProperties`, handler tag 0xf9520, object 0x2edef10.
- 499: bool var `r_showDirectionalBoxLights` (tag 0x159c318, 0/0, empty description), atexit 0x20b0, object 0x2eebc58.
- 502: bool var `r_showProjectedDecals` (tag 0x159bc00, 0/0, empty description), atexit 0x20b0, object 0x2eeae10.
- 509: bool var `showBroadcastEvents` (tag 0x159b170, 0/0) at 0x2ee86b0, then the 0x26ea0 toggle (item 0x2ee8778,
  parent label `Game Manager`, text `Show Broadcast Events`), atexit sequence 0x20b0 then 0x272d0.
- 516: string var `network_PlayerName` (empty initial value) at 0x2ee6780 built by 0x257a0; init view released
  before the name view; `__cxa_atexit(0x23c90)`.
- 535: critical section shim 0x3b310 (attr-init, settype(2), mutex-init with name `CritSec`, attr-destroy) with
  atexit 0x3b380, then a 0x20 B object with bytes [+8,+0x18) and [+0x18,+0x20) zeroed (bytes +0..+7 never written)
  and atexit 0x138370.
- 540: ten particle-group records (id 0x24) at 0x2edef70 named ParticleGroup_Normal, _UltraLowRes, _BelowWater,
  _Distortion, _Fog, _WaterDisplace, _WaterDisplace_Micro, _GroundDisplace, _RainDisplace, _ScreenOverlay (written
  through 0xc1c70); one `rand()` call stored through 0x158340 into a cell; name object `showParticle_Errors`
  (len 19, vptr 0x15976b0 written last, atexit 0x8ef00); list sentinel {self,self,0} (atexit 0x101ea0).
  The class at 0x15976b0 has slot 0 = 0x91210; 0x8ef00 is only the atexit helper (23 B, hash pinned).

## Skipped: slot 494 (0x26f7c0)

`0x26f7c0` constructs a page-pool free-list allocator through `0x26e340` (4096 entries, growth path via 0x26e4d0,
helpers 0x26e5f0, 0x26e750, 0x26e5d0 and their destructors). That subsystem is not recovered; the only existing
lift (`init_slot494_26f7c0`, L1) is hook-based (PARTIAL). A faithful native lift would need the allocator
semantics, and substituting a stub would be a fake success, so it was not attempted. Next action: recover the
0x26e340 allocator (its own task), then lift 494 on top of it.

## Observations vs inference

- Observed (checkers, from bytes): body hashes and extents, padding, every rip-relative operand, call target,
  immediate store, string and string length, functor vtable slots, and the hash of every shared callee
  (0xaaf040/0xaaf050, 0xf7b0, 0xf2f0, 0x4b560 byte facts, 0x4b660, 0x4cca0 and its reloc cell 0x15fe800,
  0xc1c70, 0x158340, 0x8ef00, 0x101ea0, 0x3b310, 0x3b380, 0x138370, the string-var set 0x257a0/0x23c90).
- Inference only:
  - The null-singleton path of 0x4b560 (queue head `[0x2ea8980]` with singleton `[0x2ea8978] == nullptr`) is the
    only path modelled. The non-null singleton takes an unmodelled tree/insert_unique branch; `register_entry_4b560`
    fails closed (`ok == false`) there. Whether the singleton is null during static construction is inferred.
  - `RecursiveMutex` is a host stand-in for the Sony pthread shim: settype 2 = RECURSIVE and the `PRIO_INHERIT`
    attribute are inferences. The original ignores the init return; the native model checks it and fails closed.
  - Slot 540 `rand`: the original calls the Sony libc `rand` (import thunk 0xf90130). The native lift takes a rand
    source parameter (default `std::rand`), so the host/Sony generator difference is not reproduced; the value is
    only stored, not consumed here.
  - Handler bodies (0x278300, 0xf9520), the queue consumers, particle-group consumers and the 491 handle users are
    not recovered and not required for construction.
- Every 0xf590 release in these slots is on a borrowed view (`control == nullptr`); an owned view reports
  `ok == false`.

## Checker notes

- `tools/lift/lane_m5_check.py` registers the extra hash-pinned callees into the L4 library and runs per-slot
  `elf_extra` functions inside the same mutation harness. It deliberately does not duplicate a byte pin over a
  byte already covered by a call/rip pin: `mutation_self_test` in `l4_slot_lib.py` computes the pristine byte from
  the current overlay, so two pins on the same byte cancel each other's flip (observed once, on 0xf2ff; fixed by
  removing the redundant pin, library unchanged).
- Short `jmp` (opcode 0xeb) sites in 492/543 and the empty padding of 535 (next function starts immediately at
  0x138e80) are pinned as immediate bytes (`eb21`, `554889e5`) because the library `jmp` pin is rel32-only and an
  empty padding pin cannot be mutated.
- The name-string length immediates of 491, 492, 543 and 540 (except `showParticle_Errors`) are not pinned as
  separate immediates; the strings themselves are pinned with their terminator and the native tests assert the
  lengths.

## Commands and results (2026-10-28)

```sh
cmake -S native -B local/build/lane-m5 -DSOTC_LANES=m5                          # exit 0
cmake --build local/build/lane-m5 -j 4 --target test_ctor_slot_{491,492,499,502,509,516,535,540,543}   # exit 0
ctest --test-dir local/build/lane-m5 -R "ctor_slot_(491|492|499|502|509|516|535|540|543)$"   # 9/9 passed
python3 -I tools/lift/check_ctor_slot_<N>.py            # N in 491 492 499 502 509 516 535 540 543: exit 0 each
python3 -I tools/lift/check_ctor_slot_<N>.py --tamper   # exit 1 each (9/9)
```

Mutation self-test (all mutants rejected): 491 92 ELF + 21 native; 492 71 + 22; 499 107 + 11; 502 107 + 11;
509 124 + 15; 516 79 + 14; 535 44 + 17; 540 164 + 34; 543 71 + 22.
Not run in this lane: the full `run_all_checkers.sh`, `audit_publish_paths.py`, `project_index.py`, the full
`local/build/orch-r22` build/ctest (integrator). No boot, audio or gameplay claim is made.

## Files created

- `native/core/lane_m5_app_entry.{hpp,cpp}` (shared 0x4b560 queue entry model, namespace `sotc::lane_m5`)
- `native/core/ctor_slot_<N>_<rva>.{hpp,cpp}` for N = 491 492 499 502 509 516 535 540 543
- `native/tests/test_ctor_slot_<N>.cpp` (nine)
- `tools/lift/lane_m5_check.py`, `tools/lift/check_ctor_slot_<N>.py` (nine)
- `native/lanes/m5.cmake`
- `docs/checkpoints/2026-10-28-lane-m5-report.md`

## Integrator wiring

Include `native/lanes/m5.cmake` from the build (it adds the ten sources to `sotc_core` and nine test targets).

Includes (startup_ctors.hpp):

```cpp
#include "lane_m5_app_entry.hpp"
#include "ctor_slot_491_295f90.hpp"
#include "ctor_slot_492_278640.hpp"
#include "ctor_slot_499_2500e0.hpp"
#include "ctor_slot_502_249270.hpp"
#include "ctor_slot_509_2196f0.hpp"
#include "ctor_slot_516_1b97b0.hpp"
#include "ctor_slot_535_138e20.hpp"
#include "ctor_slot_540_10e390.hpp"
#include "ctor_slot_543_fae50.hpp"
```

`StartupState` members (`app_info_queue` is shared by 492 and 543; slot 535 holds a non-copyable mutex and slot 540
an array of ten records, so `StartupState` must stay non-copyable/in-place):

```cpp
    lane_m5::AppInfoQueue app_info_queue{};
    Slot491Objects slot491{}; bool slot491_ok{false};
    Slot492Objects slot492{}; Slot492Result slot492_result{};
    Slot499Objects slot499{}; Slot499Result slot499_result{};
    Slot502Objects slot502{}; Slot502Result slot502_result{};
    Slot509Objects slot509{}; Slot509Result slot509_result{};
    Slot516Objects slot516{}; Slot516Result slot516_result{};
    Slot535Objects slot535{}; Slot535Result slot535_result{};
    Slot540Objects slot540{}; Slot540Result slot540_result{};
    Slot543Objects slot543{}; Slot543Result slot543_result{};
```

Registration rows (startup_ctors.cpp; slot index = constructor-table index, word `e.u64(0x2DD70E0 - 8*N) == rva`).
491 and 535 take the atexit log directly; 540 uses the default rand source (`std::rand`). Slot 494 is NOT wired.

```cpp
        {0x295f90, 491, [](StartupState& s) noexcept { s.slot491_ok = init_slot491_295f90(s.slot491, s.concmd.atexit_log); if (!s.slot491_ok) s.init_failed = true; }},
        {0x278640, 492, [](StartupState& s) noexcept { s.slot492_result = init_slot492_278640(s.slot492, s.app_info_queue, s.concmd); if (!s.slot492_result.ok) s.init_failed = true; }},
        {0x2500e0, 499, [](StartupState& s) noexcept { s.slot499_result = init_slot499_2500e0(s.slot499, s.concmd); if (!s.slot499_result.ok) s.init_failed = true; }},
        {0x249270, 502, [](StartupState& s) noexcept { s.slot502_result = init_slot502_249270(s.slot502, s.concmd); if (!s.slot502_result.ok) s.init_failed = true; }},
        {0x2196f0, 509, [](StartupState& s) noexcept { s.slot509_result = init_slot509_2196f0(s.slot509, s.concmd, s.debug_menu); if (!s.slot509_result.ok) s.init_failed = true; }},
        {0x1b97b0, 516, [](StartupState& s) noexcept { s.slot516_result = init_slot516_1b97b0(s.slot516, s.concmd); if (!s.slot516_result.ok) s.init_failed = true; }},
        {0x138e20, 535, [](StartupState& s) noexcept { s.slot535_result = init_slot535_138e20(s.slot535, s.concmd.atexit_log); if (!s.slot535_result.ok) s.init_failed = true; }},
        {0x10e390, 540, [](StartupState& s) noexcept { s.slot540_result = init_slot540_10e390(s.slot540, s.concmd.atexit_log); if (!s.slot540_result.ok) s.init_failed = true; }},
        {0xfae50, 543, [](StartupState& s) noexcept { s.slot543_result = init_slot543_fae50(s.slot543, s.app_info_queue, s.concmd); if (!s.slot543_result.ok) s.init_failed = true; }},
```
