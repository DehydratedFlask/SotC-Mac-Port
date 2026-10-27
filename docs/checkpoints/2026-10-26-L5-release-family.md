# L5-RELEASE-FAMILY: 17 startup constructor slots with the 0xf590 + __cxa_atexit shape

Date: 2026-10-26. Lane worker (parallel orchestrated run). Pinned ELF `local/re/eboot.elf`
sha256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3` (read-only, verified by the checker).
No original RVA was executed, loaded or relocated; no guest CPU. No git commit/tag/push, no edits to
HANDOFF/TASKS/EVIDENCE/INDEX or `startup_ctors.{hpp,cpp}`.

## Result

All 12 requested slots **and** all 5 optional neighbours are recovered: 371 372 405 433 470 472 481 483 484
485 487 529 (name objects) plus 434 435 438 (FNV span hashes) and 431 447 (FNV C-string hash tables).
Labels: **STATIC_ANALYSIS_VERIFIED** (every body decoded from the ELF bytes and compared row by row) and
**FIXTURE_VERIFIED** (native fixture + mutation checks). G1 stays PARTIAL (these are 17 of the remaining
slots; no runtime/boot claim).

Files created:
- `native/core/ctor_release_family_slots.{hpp,cpp}` (table-driven; `init_release_family_slot(slot, state, atexit_log, hooks)`)
- `native/tests/test_ctor_release_family_slots.cpp`
- `tools/lift/check_ctor_release_family_slots.py` (auto-picked up by `run_all_checkers.sh`)
- CMake block appended to `native/CMakeLists.txt` (`target_sources` + `test_ctor_release_family_slots` + ctest `ctor_release_family_slots`)

## Observations (from the ELF bytes)

Every body is straight-line (only the stack-protector epilogue branches), all callees are already-native:
`0xf590` (inert on a stack view), `0xf8f390` `__cxa_atexit`, `0x161f80` (`fnv_span_161f80`), `0xa2ddc0`
(`fnv_cstr_a2ddc0`), `0xf8f3a0` `__stack_chk_fail`. No other call occurs (the checker fails on any other call
target or unsupported instruction), so there are **no unresolved callees and no per-element hooks**.

1. **name_object** (371 372 405 433 470 472 481 483 484 485 487 529; 16 objects). A 0x20 B stack view
   `{text, len, 0, 0}` (ctl null) is built; the 0x30 B BSS global G gets `G+0 = vptr` (vtable symbol + 0x10),
   `G+8 = {text,len}`, `G+0x18 = 0` (16 B), `G+0x28 = 0`; then `call 0xf590` on the **stack view** (control
   null => `je ret`, exact no-op), then `__cxa_atexit(dtor, G, 0x1604218)`. 371/405/470/472 hold two such
   objects back to back (stores, release, atexit, next object). The 178 B bodies are the same template.
2. **span_hash** (434 435 438): same stack view hashed with `0x161f80(view, 0x811c9dc5)`, `f590` on the stack
   view (inert), result stored to a BSS dword. Slot 435 object 0 is the empty span (len 0 -> seed).
3. **cstr_hash** (431 447): `0xa2ddc0(text)` into consecutive BSS dwords; 431's first dword is a literal 0.
   No f590 in these two slots.

Strings (ELF text, all pinned with their `.rodata` addresses in the table): console-variable-style names
(`r_WantsHDR_Game`, `r_Wants4K_Game`, `shadowDemoMode` x2, `r_hardwareGamma(_HDR)`, `r_fullscreenBlur*`,
`arealoader_*`, `wwise_masterVolume`, `disableInput` x3, `showPathFinding`, `res_printAllFileLoads`), hash keys
`hdr_curWhiteLevel/hdr_maxWhiteLevel`, `FilmGrain`, `logo_texture`, 26 `UI_LANGUAGE_*` and 8
`UI_PHOTO_MODE_*` keys. Vtable pointers seen: 0x15976b0, 0x159bab8, 0x159bb30, 0x159e1a0, 0x159e1b8,
0x15ad1d0, 0x159e1a0; dtors: 0x8ef00, 0x2c84f0, 0x2fced0, 0x4cb9e0, 0x4cba00, 0x389800. All 56 written
globals are mapped, non-file-backed (BSS) and pairwise distinct (checked).

| slot | rva | bytes | body sha256 |
|---|---|---|---|
| 371 | 0x70cdc0 | 285 | 0fbe2360920a754f1eea6ebfa7c58720ed805e03544b81edc5f58e28fb01c415 |
| 372 | 0x70a040 | 165 | 9d5a48c8dce07de7a562daa804ced9ee3a13cdf06fe4f16c2f29f8f64f37b4c1 |
| 405 | 0x5e7790 | 309 | 144887d7fd528b437bfa0794819133229ae2c0a4bccd1ab866ecc9b88da6b985 |
| 431 | 0x523a00 | 416 | 919d6f6399f0fb79fc61fd1169340163f70f447bb9a043be606288834c2c4ac7 |
| 433 | 0x517850 | 178 | 8612830b2c332e0a3d8806c9998de6de02b0a43d88b73dbabda11588d3645163 |
| 434 | 0x511c00 | 174 | fab7347ba8c980ccef97fcb0dd28d3ad52c4d147e14aa71af0a376f70a1a6707 |
| 435 | 0x510210 | 180 | 6b88425c63cddea3b83598a2ab654eadf643fbff1996622b93738ef2784ba75a |
| 438 | 0x4fbd80 | 113 | 1b14357e0107d3a9372d1b8a6ddfa0cd7e86e8efce7808ea970631d0bd152613 |
| 447 | 0x4c3410 | 151 | 4af6818fbdd9eb9f9d8d1445a9ac76d4c7d7f9a991368810b57f28cb19b68df2 |
| 470 | 0x389a00 | 311 | 3837c476005b7606750a4c8fceb6ad49e67d4e3c1607534ede1dbc4a14dd9b7c |
| 472 | 0x3583c0 | 289 | 6ff97f6127b9804fbf40940f353ef481f2e4c384ea13f2979baafcefe7e5b406 |
| 481 | 0x2fd8f0 | 178 | fc471bd633c33c3acc25132391e2831e865227135f8ee4f31497b22c826fd14b |
| 483 | 0x2ef810 | 178 | e8d2f5782df432fc02de579b05ad29cf0a684fdf68b88bc0c668d6eeb13bb575 |
| 484 | 0x2eee10 | 178 | ade123d19353fc4141a056f7067661aaebce349a5455ea320e04a36d872cec60 |
| 485 | 0x2ee5e0 | 178 | 264ce92ea12a1371571ccb11f3661e5b320090addc3ca550b49633fadb3db7f2 |
| 487 | 0x2cd060 | 178 | 4b681275aef5edd48b1e9f50684c7cb4c640b51477268bb4c4c0045f7dc591aa |
| 529 | 0x14c030 | 178 | 0afbf025783b2e118aca9dd71c2b6f2fb75f2b46101c905f595aca78c35931a4 |

## Fail-closed contract (native)

- `f590` is issued through `ReleaseFamilyHooks::release_view` (null -> lifted `release_inert_view_f590`, which
  reports false if a control block were present). A refusal blocks the slot at stage `view_release` BEFORE
  `atexit` (`ok == false`, `blocked_step` = table step, nothing after it built). A last reference with no
  destroy/free contract can therefore never be reported as success. The release is unreachable-non-inert in
  the real family (the stack view's control is always null), but the contract is exercised by the fixture.
- `atexit` is recorded in the supplied `StartupAtexitLog`; a failed record blocks at `atexit_record` (not
  injectable in the fixture because `StartupAtexitLog::record` only fails on host OOM; mutation M2 below proves
  the record is required).
- Unknown slot -> `unknown_slot`; corrupt slot extent -> `bad_table`.
- Destructors, vtable contents and string addresses are identity tags only; the dtors are recorded, never run.

## Inference (not proven)

- The class identity of each vtable and the meaning of the stored names/hashes (console-variable-like names,
  UI/localization key hashes) is NOT analysed; no consumer is modelled.
- `StartupAtexitLog` stores `{dtor_rva, object}` only; the dso-handle `0x1604218` is a pinned constant
  (checked in the ELF for every atexit call) but not stored per record.

## Commands and results

- `cmake -S native -B local/build/L5` exit 0; `cmake --build local/build/L5 -j 4 --target test_ctor_release_family_slots` exit 0.
- `ctest --test-dir local/build/L5 -R release_family --output-on-failure`: 1/1 passed (`ctor_release_family_slots`).
- `python3 -I tools/lift/check_ctor_release_family_slots.py`: exit 0, `OK ... 17 table words + body hashes pinned; 56 derived
  steps (16 name objects, 5 span hashes, 35 cstr/const hashes) equal the native table row by row`.
- `python3 -I tools/lift/check_ctor_release_family_slots.py --tamper`: exit **1** (`native step row #0 diverges from the ELF derivation`).
- Test mutation checks (each rebuilt, test exit 1, then restored; restored files byte-identical to backup, final run exit 0):
  M1 ignore refused release; M2 drop the atexit record; M3 span seed +1; M4 table dtor 0x4cba00 -> 0x4cba01;
  M5 vptr not stored; M6 `built` set despite blocked release; M7 cstr hash replaced by the span hash (no path fold).
- Full build/ctest of the whole tree, the other lift checkers, `audit_publish_paths.py`, trail rebuild: **NOT RUN**
  (other workers edit the shared tree concurrently; the integrator runs the publication gates).

## Gaps

- `__cxa_atexit` failure injection (no seam in `StartupAtexitLog`).
- Registration in `startup_ctors.cpp`, `StartupState` members and CTest count update are the integrator's (below).
- The checker's decoder understands only the instructions present in these 17 bodies; it fails loudly otherwise.

## Integrator wiring

`native/core/startup_ctors.hpp` (include `"ctor_release_family_slots.hpp"` and add to `StartupState`):

```cpp
    ReleaseFamilyState release_family{};
    ReleaseFamilyHooks release_family_hooks{};   // null release_view -> lifted inert f590
```

`native/core/startup_ctors.cpp` rows (rva, slot index, callback; keep the table's existing ordering rules):

```cpp
        {0x70cdc0, 371, [](StartupState& s) noexcept { if (!init_release_family_slot(371, s.release_family, s.concmd.atexit_log, s.release_family_hooks).ok) s.init_failed = true; }},
        {0x70a040, 372, [](StartupState& s) noexcept { if (!init_release_family_slot(372, s.release_family, s.concmd.atexit_log, s.release_family_hooks).ok) s.init_failed = true; }},
        {0x5e7790, 405, [](StartupState& s) noexcept { if (!init_release_family_slot(405, s.release_family, s.concmd.atexit_log, s.release_family_hooks).ok) s.init_failed = true; }},
        {0x523a00, 431, [](StartupState& s) noexcept { if (!init_release_family_slot(431, s.release_family, s.concmd.atexit_log, s.release_family_hooks).ok) s.init_failed = true; }},
        {0x517850, 433, [](StartupState& s) noexcept { if (!init_release_family_slot(433, s.release_family, s.concmd.atexit_log, s.release_family_hooks).ok) s.init_failed = true; }},
        {0x511c00, 434, [](StartupState& s) noexcept { if (!init_release_family_slot(434, s.release_family, s.concmd.atexit_log, s.release_family_hooks).ok) s.init_failed = true; }},
        {0x510210, 435, [](StartupState& s) noexcept { if (!init_release_family_slot(435, s.release_family, s.concmd.atexit_log, s.release_family_hooks).ok) s.init_failed = true; }},
        {0x4fbd80, 438, [](StartupState& s) noexcept { if (!init_release_family_slot(438, s.release_family, s.concmd.atexit_log, s.release_family_hooks).ok) s.init_failed = true; }},
        {0x4c3410, 447, [](StartupState& s) noexcept { if (!init_release_family_slot(447, s.release_family, s.concmd.atexit_log, s.release_family_hooks).ok) s.init_failed = true; }},
        {0x389a00, 470, [](StartupState& s) noexcept { if (!init_release_family_slot(470, s.release_family, s.concmd.atexit_log, s.release_family_hooks).ok) s.init_failed = true; }},
        {0x3583c0, 472, [](StartupState& s) noexcept { if (!init_release_family_slot(472, s.release_family, s.concmd.atexit_log, s.release_family_hooks).ok) s.init_failed = true; }},
        {0x2fd8f0, 481, [](StartupState& s) noexcept { if (!init_release_family_slot(481, s.release_family, s.concmd.atexit_log, s.release_family_hooks).ok) s.init_failed = true; }},
        {0x2ef810, 483, [](StartupState& s) noexcept { if (!init_release_family_slot(483, s.release_family, s.concmd.atexit_log, s.release_family_hooks).ok) s.init_failed = true; }},
        {0x2eee10, 484, [](StartupState& s) noexcept { if (!init_release_family_slot(484, s.release_family, s.concmd.atexit_log, s.release_family_hooks).ok) s.init_failed = true; }},
        {0x2ee5e0, 485, [](StartupState& s) noexcept { if (!init_release_family_slot(485, s.release_family, s.concmd.atexit_log, s.release_family_hooks).ok) s.init_failed = true; }},
        {0x2cd060, 487, [](StartupState& s) noexcept { if (!init_release_family_slot(487, s.release_family, s.concmd.atexit_log, s.release_family_hooks).ok) s.init_failed = true; }},
        {0x14c030, 529, [](StartupState& s) noexcept { if (!init_release_family_slot(529, s.release_family, s.concmd.atexit_log, s.release_family_hooks).ok) s.init_failed = true; }},
```

Note: `ReleaseFamilyState` also keeps a per-slot `results` array (`s.release_family.results[release_family_slot_position(slot)]`).
The `atexit_log` used here is `ConCmdRegistry::atexit_log` (`s.concmd.atexit_log`); records are appended in row execution order.
Adding the 17 production rows raises the production row count that other checkers pin (e.g.
`check_fctor_slots_296_297_300.py` expects 318); update that pin when registering.
