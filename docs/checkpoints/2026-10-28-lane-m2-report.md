# Lane M2 report: startup constructor slots 389 393 403 407 413 415 419 421 (409 and 414 skipped)

Date: 2026-10-28. Worker lane: m2. Status: 8 of 10 assigned slots lifted (FIXTURE_VERIFIED plus
STATIC_ANALYSIS_VERIFIED for the byte oracle); 2 skipped with a precise reason. Nothing was committed,
staged or tagged. No existing file was edited. No original RVA, guest instruction or eboot byte is
executed by any native module; the ELF (`local/re/eboot.elf`, sha256
`d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`) is read only by the Python checkers.

## Status table

| Slot | RVA | Size | Result | What the constructor does (observed from ELF bytes) |
|---:|---|---:|---|---|
| 389 | 0x695a50 | 386 | PASSED (fixture + oracle) | Bool var `shadow_mungeFishFinCloth` (tag 0x15cc690, value/default 1, flags 1) plus `0x26db0` menu toggle under `Shadow Cloth` |
| 393 | 0x66cc00 | 299 | PASSED | Bool var `cp10_debugDrawFruit` (tag 0x15cc5c8, value/default 0) |
| 403 | 0x5f0160 | 386 | PASSED | Bool var `shadow_drawModelCharacterClasses` (tag 0x15cc078, 0/0) plus toggle under `Shadow Misc` |
| 407 | 0x5d41c0 | 374 | PASSED | Bool var `cp10_creditsUseNewList` (tag 0x15cb258, 1/1) plus zero vec3 at 0x2ff649c and 4x4 identity at 0x2ff64ac |
| 413 | 0x57c810 | 370 | PASSED | Bounded float var `shadow_deadZoneDefault` via `0xc300` (min 0, max 1.0, default bits 0x3e780347, functor tag 0x15cb148) |
| 415 | 0x578b60 | 331 | PASSED | Unbounded int var `isysLinkAllocMax` via worker `0x3a980` (flags 8, 0/0, functor tag 0x15cb128) |
| 419 | 0x56fae0 | 402 | PASSED | Five 0x28 B splash-screen records at 0x2f117b0 (immediate stores) and a tail `jmp 0x323d0` stamping the TSC at 0x2f11878 |
| 421 | 0x56da20 | 329 | PASSED | Bool var `filesystem_allowAsync` (tag 0x15cade0, 0/0, flags 8) plus a 128 B zero block at 0x2f090d0 |
| 409 | - | - | SKIPPED | see below |
| 414 | - | - | SKIPPED | see below |

## Skipped slots (precise reasons)

Both were already lifted by lane L1 (`ctor_tiny_misc_slots.hpp`, `docs/checkpoints/2026-10-26-L1-tiny-slots.md`)
as PARTIAL with fail-closed host hooks. A lane-m2 re-lift would duplicate that module and would need the
same hook, so it would not be an honest improvement.

- 409: needs `0x17070` (tail wrapper: `esi=1, edx=8, ecx=1, jmp 0x12820`). The allocator dispatcher
  `0x12820` (size-class bookkeeping, TLS state) is not lifted; `alloc_wrappers_17070.hpp` documents it as
  NOT modelled (G1 PARTIAL). Slot 409 allocates a 0x40 B node through it. L1's `Slot409Hooks::alloc_17070`
  remains the only honest seam.
- 414: needs `0x3b310`, a libkernel pthread-mutex-init wrapper calling PLT imports (0xf8f920/0xf8f930/
  0xf8f8d0/0xf8f910), with destructor `0x3b380` = `jmp 0xf8f8c0`. Those are Sony libkernel imports, so there is
  no native body to lift. L1's `Slot414Hooks::mutex_init_3b310` stays a hook; a real host mutex is a
  platform decision for the integrator, not something this lane can verify from bytes.

## Observation versus inference

Observed (checker, ELF bytes): body sha256 and extent, every call/jmp/rip/immediate operand pin, constructor
table word `0x2DD70E0 - 8*N == rva`, and the bodies of the functor-tag slots 0x15ce318 (float: 0xc4a0, 0xc4c0,
0xc4e0, 0xc4f0) and 0x15ce8c0 (int: 0xc720, 0xc740, 0xc760, 0xc770). Body hashes:

389 `db45e912...`, 393 `8ed706dc...`, 403 `21962eb4...`, 407 `c0e56e7d...`, 413 `7a2e3911...`,
415 `46b1a3bc...`, 419 `721acbce...`, 421 `05d9d351...` (full digests are printed by each checker).

Inferred (not provable from bytes alone):
- The lane tags 0x15cb148 (float) and 0x15cb128 (int) are substituted by the already allowlisted helpers'
  tags. This is output-neutral because the base constructor never stores the caller tag and the destroy slot is
  a bare `ret`; the checkers still confirm the real tag's slot bodies.
- Destructor RVAs (0x20b0, 0x272d0, 0x20f0, 0x18a60) are only recorded in `StartupAtexitLog`; they are never
  run, matching the existing L4 convention.
- Names are borrowed views (`control == nullptr`); callers keep the literals alive.

Not claimed: boot, audio, gameplay, saves, rendering. These are only constructors of debug/config variables
and static tables.

## Commands and results

```sh
cmake -S native -B local/build/lane-m2 -DSOTC_LANES=m2
cmake --build local/build/lane-m2 -j 4 --target test_ctor_slot_389 test_ctor_slot_393 test_ctor_slot_403 \
    test_ctor_slot_407 test_ctor_slot_413 test_ctor_slot_415 test_ctor_slot_419 test_ctor_slot_421
ctest --test-dir local/build/lane-m2 -R "ctor_slot_(389|393|403|407|413|415|419|421)"
for n in 389 393 403 407 413 415 419 421; do python3 -I tools/lift/check_ctor_slot_$n.py; python3 -I tools/lift/check_ctor_slot_$n.py --tamper; done
```

Results: build exit 0, no warnings; ctest 8/8 passed (100 percent). Checkers: 8/8 exit 0; every `--tamper`
exits 1. Mutation self-test (ELF mutants / native-token mutants, all rejected):
389 127/13, 393 105/11, 403 127/13, 407 133/18, 413 97/16, 415 108/16, 419 190/26, 421 119/20.

Not run by this lane: the full workspace `run_all_checkers.sh`, the publish-path audits and
`project_index.py` (integrator-owned; this lane only touches the allowlisted paths below).

## Files created

- `native/core/lane_m2_shapes.hpp/.cpp` (namespace `sotc::lane_m2`): `init_bool_var_flagged` (flags argument
  that `init_bool_var_static` hard-codes to 1), `init_float_var_site` and `init_unbounded_int_var_site` with
  functor allowlists extended by the lane tags, fail-closed on unknown functors before any write.
- `native/core/lane_m2_testkit.hpp` (test-only helpers).
- `native/core/ctor_slot_{389_695a50,393_66cc00,403_5f0160,407_5d41c0,413_57c810,415_578b60,419_56fae0,421_56da20}.hpp/.cpp`
- `native/tests/test_ctor_slot_{389,393,403,407,413,415,419,421}.cpp`
- `tools/lift/check_ctor_slot_{389,393,403,407,413,415,419,421}.py`
- `native/lanes/m2.cmake`
- this report.

## Gaps

- 409 and 414 stay on L1's hook-based modules (see above).
- Slot 419 depends on `store_tsc_323d0` and its hook struct (`StoreTsc323d0Hooks`); the default hooks are used
  when none are supplied. The record `path` fields are borrowed literals for the `%s%s.ctxr` templates.
- Slots 413 and 415 rely on the allowlist extension in `lane_m2_shapes`; merging the two lane tags into the shared
  helpers would remove the wrapper but was out of scope (no existing file edits).

## Integrator wiring

Includes (startup_ctors.hpp):

```cpp
#include "ctor_slot_389_695a50.hpp"
#include "ctor_slot_393_66cc00.hpp"
#include "ctor_slot_403_5f0160.hpp"
#include "ctor_slot_407_5d41c0.hpp"
#include "ctor_slot_413_57c810.hpp"
#include "ctor_slot_415_578b60.hpp"
#include "ctor_slot_419_56fae0.hpp"
#include "ctor_slot_421_56da20.hpp"
```

`StartupState` members:

```cpp
    Slot389Objects slot389{}; Slot389Result slot389_result{};
    Slot393Objects slot393{}; Slot393Result slot393_result{};
    Slot403Objects slot403{}; Slot403Result slot403_result{};
    Slot407Objects slot407{}; Slot407Result slot407_result{};
    Slot413Objects slot413{}; Slot413Result slot413_result{};
    Slot415Objects slot415{}; Slot415Result slot415_result{};
    Slot419Objects slot419{}; Slot419Result slot419_result{};
    Slot421Objects slot421{}; Slot421Result slot421_result{};
```

Registration rows (startup_ctors.cpp; slot index = constructor-table index, word `e.u64(0x2DD70E0 - 8*N) == rva`):

```cpp
        {0x695a50, 389, [](StartupState& s) noexcept { s.slot389_result = init_slot389_695a50(s.slot389, s.concmd, s.debug_menu); if (!s.slot389_result.ok) s.init_failed = true; }},
        {0x66cc00, 393, [](StartupState& s) noexcept { s.slot393_result = init_slot393_66cc00(s.slot393, s.concmd); if (!s.slot393_result.ok) s.init_failed = true; }},
        {0x5f0160, 403, [](StartupState& s) noexcept { s.slot403_result = init_slot403_5f0160(s.slot403, s.concmd, s.debug_menu); if (!s.slot403_result.ok) s.init_failed = true; }},
        {0x5d41c0, 407, [](StartupState& s) noexcept { s.slot407_result = init_slot407_5d41c0(s.slot407, s.concmd); if (!s.slot407_result.ok) s.init_failed = true; }},
        {0x57c810, 413, [](StartupState& s) noexcept { s.slot413_result = init_slot413_57c810(s.slot413, s.concmd); if (!s.slot413_result.ok) s.init_failed = true; }},
        {0x578b60, 415, [](StartupState& s) noexcept { s.slot415_result = init_slot415_578b60(s.slot415, s.concmd); if (!s.slot415_result.ok) s.init_failed = true; }},
        {0x56fae0, 419, [](StartupState& s) noexcept { s.slot419_result = init_slot419_56fae0(s.slot419); if (!s.slot419_result.ok) s.init_failed = true; }},
        {0x56da20, 421, [](StartupState& s) noexcept { s.slot421_result = init_slot421_56da20(s.slot421, s.concmd); if (!s.slot421_result.ok) s.init_failed = true; }},
```

CMake: `include(lanes/m2.cmake)` is already handled by the `-DSOTC_LANES=m2` switch; for the permanent merge, copy the
`target_sources` list (including `core/lane_m2_shapes.cpp`) and the eight `add_executable`/`add_test` blocks from
`native/lanes/m2.cmake` into `native/CMakeLists.txt`. Slots 409 and 414 keep their existing L1 rows.
