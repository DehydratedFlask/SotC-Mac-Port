# Startup batch 8 (worker CPU-01-STARTUP-BATCH-8), 2026-11-02

Labels: STATIC_ANALYSIS_VERIFIED + FIXTURE_VERIFIED. Nothing executed an original RVA; no boot, audio or save claim.
Build dir `local/build/startup-b8` (owned by this worker). Not committed (workers do not commit). HANDOFF/TASKS/EVIDENCE not edited (paste text below).

## Result

Coverage 497 -> 503/573 (70 open). The old cliff slot 337 (0x7e81d0) is moved. New first unrecovered slot: **339 (0x7e2fc0)**.

| Slot | RVA / bytes | Shape | Checker (pins; ELF + native mutants) |
|---|---|---|---|
| 337 | 0x7e81d0 / 1849 | 2 bool vars, 5 unbounded floats (tag 0x15ce4a0), NameHolder30 `shadowDemoMode` | check_ctor_slot_337 (150; 416 + 47) |
| 351 | 0x7ae220 / 2706 | 2 bool vars (tag 0x15ce220), 7 bounded floats (tag 0x15ce240, new admitted tag), 2 `0x27120` sliders, caption `Shadow Camera` | check_ctor_slot_351 (213; 529 + 54) |
| 400 | 0x62b100 / 1624 | 5 bool vars (tag 0x15cc448) with 5 `0x26db0` toggles, interleaved | check_ctor_slot_400 (144; 359 + 32) |
| 444 | 0x4d3f10 / 1276 | 4 unbounded floats (tag 0x15c1bc8) + bool `fontShowExternalElementExtents` | check_ctor_slot_444 (101; 294 + 31) |
| 474 | 0x344850 / 1300 | 4 NameHolder30 (2x class A 0x15976b0, 2x class B 0x159e1a0), 3 bools, string var `EntityCreationContext` | check_ctor_slot_474 (110; 349 + 53) |
| 521 | 0x17bf50 / 1674 | unbounded float `fontDebugScale`, 6 bools (tag 0x1598810), 0x30 gap padding | check_ctor_slot_521 (138; 382 + 44) |

All checkers: run, `--tamper`, `--tamper elf`, `--tamper native` = 0 / 1 / 1 / 1. Test per slot: `native/tests/test_ctor_slot_N.cpp`.
New native files: `native/core/ctor_slot_{337,351,400,444,474,521}_*.{hpp,cpp}`, `native/tests/test_ctor_slot_{...}.cpp`, `tools/lift/check_ctor_slot_{...}.py`.
Edited: `ctor_shapes_343_356.{hpp,cpp}` (float functor tag 0x15ce240 admitted as `kFloatArgFnVtblSlot351`, tag-pinned in the 351 checker),
`startup_ctors.{hpp,cpp}`, `native/CMakeLists.txt`, pins: `test_startup_ctor_coverage.cpp` (503 / 339 / 0x7e2fc0 / +70; pending 301,
roots 176, atexit 608), `test_startup_ctors.cpp` expect[], `check_ctor_slots.py` (DEBUG_PAGE_BATCH_8, 70 unregistered, first 339, index digest
464ce561...), `check_console_var_ctors.py`, `check_fctor_slots_296_297_300.py` (503 rows). Regenerated: docs/entry_map.json (2063 constants, 1386 RVAs,
461 headers), docs/G1_EVIDENCE.md, docs/STARTUP_GAP_SURVEY.md, project index (76655 folders).

## Refused / not taken (need new mechanisms; 0 failed tries on any task)

- **339 (0x7e2fc0, the new cliff)**: one non-functor indirect call plus the unmodelled-caller helper `0x52a990` x7 and `0xf2f0`.
- 342 (indirect call, `0x23fd00`), 340 (two indirect calls, XL).
- 395: `0x26db0` toggles over FLOAT targets; `init_toggle_site_26db0` takes a bool target only. Needs a variant, not a stub.
- 494 (four allocating vector builders), 464 / 454, 566 (0x25ab0 twin), 390 / 495 / 398 / 493, 570 / 511.
- 349, 370, 375, 380, 402: `0x27120` is now available but each was not individually analysed in this batch beyond the program dump (380/402 were
  blocked by `0x257a0`-class callers in batch 7; the string var model exists now, so 380/402 are the next candidates). Not attempted.

## Commands and results

| Command | Result |
|---|---|
| `cmake --build local/build/startup-b8 -j 12` | exit 0, 0 warnings |
| `ctest --test-dir local/build/startup-b8 -j 12 --output-on-failure` | 587/587 passed |
| `tools/lift/check_ctor_slot_{337,351,400,444,474,521}.py` + 3 tamper probes each | 0/1/1/1 each |
| `check_ctor_slots`, `check_console_var_ctors`, `check_fctor_slots_296_297_300`, `check_evidence_grades`, `gen_entry_map.py --check`, `g1_evidence_pack.py --check`, `check_g1_evidence_pack.py` | OK |
| `SOTC_NATIVE_BUILD=local/build/startup-b8 tools/lift/run_all_checkers.sh` | 796 passed, 0 skipped, 0 failed (exit 0) |
| `python3 -I scripts/audit_publish_paths.py --all` | OK (4110 paths; new untracked files are not yet staged) |
| `python3 scripts/project_index.py` | written (76655 folders) |

## Risks

- Stack-guard compares are hooks, as in every earlier slot; `0xf590` releases are exact only because views are borrowed.
- 351 depends on the bounded-float tag admission list in `ctor_shapes_343_356.cpp` (one more tag); its checker pins the tag's four float bodies.
- Name readings (shadow camera, fonts, entity creation) are inference from strings; no variable consumer is lifted.
- `STARTUP_GAP_SURVEY` still lists 336/337/... as "IN_FLIGHT" because it matches native file names, not the row table; the row set (503) is authoritative.
- Other workers add concurrent slots; counts were re-measured from the fixture, not assumed.

## Paste-ready text

HANDOFF: "2026-11-02 worker startup batch 8: coverage 497->503/573 (slots 337, 351, 400, 444, 474, 521 wired, each with a tie checker and
--tamper probes). Cliff 337 -> 339 (0x7e2fc0, indirect call). Pending 301, menu roots 176, atexit 608. 587/587 ctest, run_all_checkers 796/0/0,
count pins and docs regenerated. STATIC_ANALYSIS_VERIFIED + FIXTURE_VERIFIED; no boot/audio/save claim. Not committed. Refused: 339, 342, 340,
395 (toggle over float), 494. Next: 380/402 (string var now modelled), 349/370/375."

TASKS: "CPU-01-AOT-CFG-SCALE-01: startup coverage 503/573 (batch 8 done; 70 open, first 339). Status in progress, attempts 0 failed."

EVIDENCE (re-check the tail first; last at write time was E1156; use E1160+): E1160 slot 337; E1161 slot 351 (+ float tag 0x15ce240);
E1162 slot 400; E1163 slot 444; E1164 slot 474; E1165 slot 521; E1166 batch 8 totals (coverage 503/573, pending 301, roots 176, atexit 608,
587/587 ctest, 796 checkers). Each: `STATIC_ANALYSIS_VERIFIED + FIXTURE_VERIFIED`, `tools/lift/check_ctor_slot_N.py` (mutants as in the table).
