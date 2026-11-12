# Startup batch 9 (worker CPU-01-STARTUP-BATCH-9), 2026-11-02

Labels: STATIC_ANALYSIS_VERIFIED + FIXTURE_VERIFIED. Nothing executed an original RVA; no boot, audio or save claim.
Build dir `local/build/startup-b9` (owned by this worker). Not committed (workers do not commit). HANDOFF/TASKS/EVIDENCE not edited (paste text below).

## Result

Coverage 503 -> 511/573 (62 open). The batch-8 cliff **339 (0x7e2fc0) is moved**. New first unrecovered slot: **340 (0x7de7e0)** (two indirect calls, XL).

The slot-339 "indirect call" and "unmodelled `0x52a990`" survey flags were NAME-resolution misses, not missing mechanisms: `0x52a990` is the
already-native `fnv_cstr_52a990`, and the one indirect call sits in the `killBoss` console-command registration (flags 2, atexit 0x4b0), modelled natively
and ELF-pinned by `check_ctor_slot_339` (observation: the checker passes; why the survey flagged it is inference).

| Slot | RVA / bytes | Shape | Checker (pins; ELF + native mutants) |
|---|---|---|---|
| 339 | 0x7e2fc0 / 1004 | 7 fnv1a name cells, 2 NameHolder30, unbounded float (tag 0x15ce440), console command `killBoss` (flags 2, atexit 0x4b0) | check_ctor_slot_339 (93; 278 + 61) |
| 395 | 0x65af60 / 1525 | bool (tag 0x15cc588), 3 bounded floats (tag 0x15cc5a8), 4 `0x26db0` toggles, the first over FLOAT targets | check_ctor_slot_395 (122; 333 + 43) |
| 380 | 0x6d71f0 / 4668 | 11 bool, 3 bounded float (tag 0x15cc878), 12 toggles, 1 slider (foot partner IK, inference) | check_ctor_slot_380 (381; 879 + 98) |
| 375 | 0x6f1c20 / 12802 | 11 bool, 23 bounded float (tag 0x15cc9f8), 5 `0x26db0` + 16 `0x26ea0` toggles, 3 sliders, 3 groups (horse avoid, inference) | check_ctor_slot_375 (1042; 2279 + 208) |
| 382 | 0x6c8600 / 1870 | NameHashCell, bool, 4 bounded floats (tag 0x15cc7f8), 4 toggles (ancient bow, inference) | check_ctor_slot_382 (173; 439 + 60) |
| 360 | 0x7806f0 / 1815 | 4 bounded floats (tag 0x15cdf18), 4 toggles, NameHolder30, NameHashCell (sword light, inference) | check_ctor_slot_360 (157; 395 + 59) |
| 373 | 0x7081e0 / 2165 | 3 bool, 4 unbounded floats (tag 0x15ccd30, early), 5 toggles (crosshair aim, inference) | check_ctor_slot_373 (178; 449 + 61) |
| 354 | 0x79e180 / 2173 | 3 bool, 1 bounded float, 4 unbounded floats with descriptions (tag 0x15ce0b8), 1 `0x26ea0` toggle (photo-mode camera, inference) | check_ctor_slot_354 (173; 449 + 71) |

All checkers: run, `--tamper`, `--tamper elf`, `--tamper native` = 0 / 1 / 1 / 1. Test per slot: `native/tests/test_ctor_slot_N.cpp`.

New mechanisms (all small):
- `init_toggle_site_26db0_float` (395): the `0x26db0` toggle over a float target. The bounded/unbounded float class answers slot3 = 1, slot6 = 3, so the
  range narrows to {0.1, 0.5}; bool targets keep {1, 10}.
- Float functor tags admitted in `ctor_shapes_343_356.{hpp,cpp}` (`kFloatArgFnVtblSlot395/380/375/382/360/354`), each pinned to the four float bodies by its checker.
- Slots 380, 375, 382, 360, 373, 354 are produced by a generator that works from a static register tracker over the pinned ELF (a tool, not an emulator; it
  reads bytes and never executes them). Generated files are ordinary hand-readable C++; the generator is not part of the repo.

## Files

New: `native/core/ctor_slot_{339_7e2fc0,395_65af60,380_6d71f0,375_6f1c20,382_6c8600,360_7806f0,373_7081e0,354_79e180}.{hpp,cpp}`,
`native/tests/test_ctor_slot_{339,395,380,375,382,360,373,354}.cpp`, `tools/lift/check_ctor_slot_{339,395,380,375,382,360,373,354}.py`.
Edited: `ctor_shapes_343_356.{hpp,cpp}`, `startup_ctors.{hpp,cpp}`, `native/CMakeLists.txt`; pins: `test_startup_ctor_coverage.cpp` (511 / first 340 / 0x7de7e0 /
+62; pending 379, menu roots 234, atexit 747), `test_startup_ctors.cpp` `expect[]` (511 rows), `check_ctor_slots.py` (DEBUG_PAGE_BATCH_9, 62 unregistered, first 340,
index digest b4dad232...), `check_console_var_ctors.py`, `check_fctor_slots_296_297_300.py` (511 rows). Survey fix: `tools/lift/survey_startup_gaps.py`
now labels a gap that the working tree registers `WIRED_UNCOMMITTED` (it was `IN_FLIGHT`); `IN_FLIGHT` is kept for an untracked model with no row.
Regenerated: docs/STARTUP_GAP_SURVEY.md, docs/entry_map.json (2085 constants, 1406 RVAs, 469 headers), docs/G1_EVIDENCE.md, docs/FOLDERS.md (project index, 77950 folders).

## Refused / not taken (need new mechanisms; 0 failed tries on any task)

- 340 (two indirect calls, XL, the new cliff), 342 (indirect call, `0x23fd00`), 370 (53 KB with new shapes), 349 (deferred, not analysed).
- 402 (unclassified var + string var + holder), 494 (four allocating vector builders), 464 / 454, 566 (0x25ab0 twin), 390 / 495 / 398 / 493, 570 / 511.
- 391 (`0xf2f0`), 561 (`0xc580` / `0x3a980`), 469 (`0x27050`), 401 (`0x620e10`), 501, 508, 352: unmodelled helpers each.

## Commands and results

| Command | Result |
|---|---|
| `cmake --build local/build/startup-b9 -j 12` | exit 0, 0 warnings |
| `ctest --test-dir local/build/startup-b9 -j 12` | 595/595 passed |
| `tools/lift/check_ctor_slot_{...}.py` + 3 tamper probes each | 0/1/1/1 each |
| `check_ctor_slots`, `check_console_var_ctors`, `check_fctor_slots_296_297_300`, `check_evidence_grades`, `gen_entry_map.py --check`, `g1_evidence_pack.py --check`, `check_g1_evidence_pack.py` | OK |
| `SOTC_NATIVE_BUILD=local/build/startup-b9 tools/lift/run_all_checkers.sh` | 804 passed, 0 skipped, 0 failed (exit 0) |
| `python3 -I scripts/audit_publish_paths.py --all` | OK (4110 paths; new untracked files not yet staged) |
| `python3 scripts/project_index.py` | written (77950 folders) |

## Risks

- Stack-guard compares are hooks, as in every earlier slot; `0xf590` releases are exact only because views are borrowed.
- Name readings (foot IK, horse avoid, ancient bow, sword light, crosshair aim, photo mode) are inference from strings; no variable consumer is lifted.
- The toggle-over-float range {0.1, 0.5} follows the float class slot3/slot6 answers; it is pinned by the bodies the checkers hash, not by a run.
- The tracker/generator is scratch tooling and not committed; regenerating needs it again, but every output is pinned by its checker, so a hand edit is caught.
- Other workers add concurrent slots; counts were re-measured from the fixture, not assumed. The survey line "HEAD 460/573" compares with the last commit, not with batch 8.

## Paste-ready text

HANDOFF: "2026-11-02 worker startup batch 9: coverage 503->511/573 (slots 339, 395, 380, 375, 382, 360, 373, 354 wired, each with a tie checker and
--tamper probes). Cliff 339 -> 340 (0x7de7e0, two indirect calls). The 339 survey flags were name misses (0x52a990 is native fnv_cstr_52a990). New:
0x26db0 toggle over float targets (395). Pending 379, menu roots 234, atexit 747. 595/595 ctest, run_all_checkers 804/0/0, count pins and docs regenerated;
survey now labels wired-uncommitted slots WIRED_UNCOMMITTED. STATIC_ANALYSIS_VERIFIED + FIXTURE_VERIFIED; no boot/audio/save claim. Not committed.
Refused: 340, 342, 370, 349, 402, 494. Next: 349, 402 (string var), then the unmodelled-helper slots 391, 561, 469, 401."

TASKS: "CPU-01-AOT-CFG-SCALE-01: startup coverage 511/573 (batch 9 done; 62 open, first 340). Status in progress, attempts 0 failed."

EVIDENCE (re-check the tail first; last at write time was E1166; use E1170+): E1170 slot 339; E1171 slot 395 (+ `0x26db0` float-target toggle);
E1172 slot 380; E1173 slot 375; E1174 slot 382; E1175 slot 360; E1176 slot 373; E1177 slot 354; E1178 batch 9 totals (coverage 511/573, pending 379,
roots 234, atexit 747, 595/595 ctest, 804 checkers). Each: `STATIC_ANALYSIS_VERIFIED + FIXTURE_VERIFIED`, `tools/lift/check_ctor_slot_N.py` (mutants as in the table).
