# Checkpoint 2026-11-02: startup batch 6 (14 helper-callee-backed slots)

Agent: startup-ctor lane worker (Sonnet 5.5) under the orchestrator. Task: `CPU-01-AOT-CFG-SCALE-01` startup-table
coverage, batch 6. Labels: STATIC_ANALYSIS_VERIFIED (ELF byte pins, offline, the private ELF is only read) and
FIXTURE_VERIFIED (native tests). No original RVA was executed. No commit, tag or push was made. HANDOFF.md,
docs/TASKS.md and docs/EVIDENCE.md were NOT edited (paste-ready text below, evidence ids E1130..E1144; the EVIDENCE
tail was E1122 when this batch finished).

## Result

- Startup constructor coverage 477 -> 491 / 573 (+14 rows). Remaining rows 82 (was 96). First unrecovered row
  unchanged: index 336 (0x7ec4c0). HEAD is 460 (batches 2-6 are uncommitted in the tree).
- All 14 candidates were wired; none was refused. Every functor-family call site is proven per slot: the slot's caller
  functor tag is admitted by the helper wrapper (`helper_c580_ranged_int`, `helper_3a980_unbounded_int`,
  `helper_6aa00_unbounded_float`) and the native slot requires `HelperRefusal::none`; a tag outside the family is
  refused with nothing written (each slot's `section_refusal` / `section_tag_refusal`). The `callq *0x18(%rax)` site
  after each wrapper is the bare-ret destroy of the family (bodies 0xc770 / 0xc4f0 pinned by every checker), so no new
  functor mechanism was needed.
- Corrected claim: the task brief called 0x27050 "unmodelled". It is native (`init_debug_menu_item_27050`, tied by
  `check_ctor_slot_361`, `467` and `check_fctor_slots_298_299_301`); slots 369 and 420 reuse it and re-pin its 208 B
  body hash, the `kind = 2` store and the tail `jmp 0x268b0`.
- No three-try escalation (failed tries: 0). `docs/escalations/` untouched.

## Slots

| idx | RVA | size | body sha | pins | mutants (ELF + native) | shape |
|---|---|---|---|---|---|---|
| 366 | 0x744ac0 | 695 | 2aec5eb2 | 62 | 192 + 26 | 2 bool vars (tag 0x15cd200, values 1/0) + two 0x26db0 toggles under `Shadow IK` |
| 411 | 0x591710 | 692 | b7deef05 | 60 | 188 + 26 | 2 bool vars (tag 0x15cb168, values 1/0) + two 0x26db0 toggles under `Shadow Camera` |
| 430 | 0x529df0 | 539 | ec873821 | 42 | 157 + 36 | ranged int `wanderForceCorruptionLevel` (-1..5, 0xc580, tag 0x15c8450) + cell atexit 0x523ba0 + toggle |
| 504 | 0x23f4d0 | 562 | 46dc6495 | 44 | 177 + 32 | unbounded float `uiOverrideSafeFrame` (0x6aa00, tag 0x159ba68) + bool `uiShowSafeFrame` |
| 555 | 0x527f0 | 585 | 190562fd | 45 | 189 + 35 | strings `smtp_server` / `smtp_from` (0x257a0) + unbounded int `smtp_server_port` = 25 (0x3a980, tag 0x1595e60) |
| 397 | 0x64d760 | 1036 | a7fc2085 | 79 | 221 + 41 | four unbounded floats (player acceleration scalars) |
| 442 | 0x4e3760 | 779 | 1a787228 | 63 | 219 + 39 | bool + float + bool (grip-fill test variables) |
| 450 | 0x4af4b0 | 808 | 3cbbb9b3 | 62 | 185 + 33 | three unbounded floats (UI logo / main-menu timings) |
| 460 | 0x41c340 | 857 | 47a71a6b | 85 | 258 + 33 | bool `drawWaypointNode` + two unbounded floats |
| 514 | 0x1c4c50 | 739 | 9ffe136f | 85 | 238 + 26 | 2 bool vars (tag 0x1598f50, values 0/1) + two 0x26db0 toggles under `Camera` |
| 455 | 0x47bfa0 | 817 | 3fe3a873 | 73 | 216 + 33 | 2 bool vars (tag 0x15bddb8, values 1/0) + 0x26c10 group `Loose Attachment` + two 0x26db0 toggles |
| 488 | 0x2ba5d0 | 1028 | 06002a8a | 93 | 252 + 39 | ranged int `SM_showActiveState` (0..2) + 0x30 B holder (atexit 0x2b5050) + unbounded ints `SM_showActiveStateMinTime` / `MaxTime` |
| 369 | 0x734190 | 898 | 985ad228 | 86 | 278 + 42 | bool + 0x26ea0 toggle + unbounded float (0x6aa00, tag 0x15cd180) + 0x27050 slider (0.01..0.1) |
| 420 | 0x56f6e0 | 1006 | 6e93305b | 83 | 289 + 49 | bool + ranged int (0xc580, 0..0x1f, tag 0x15caf38) + 0x26b60 separator + 0x26ea0 toggle + 0x27050 slider + 8 B cell (atexit 0x56ef80) |

Every checker's `--tamper`, `--tamper elf` and `--tamper native` probes exit 1 (verified for all 14).

### New mechanisms

- `native/core/helper_int_var_sites.{hpp,cpp}`: `helper_init_ranged_int_var_site`, `helper_init_unbounded_int_var_site`,
  `helper_init_unbounded_float_var_site` (`HelperIntSiteSpec`, `HelperFloatSiteSpec`): one call site = wrapper (tag
  admission, `HelperRefusal`) + bare-ret functor destroy + description/name `0xf590` releases + `0x20d60` registration +
  `__cxa_atexit` identity record. `tools/lift/batch6_lib.py` carries the shared checker data.
- Slot 420 is the only slot that needs an allocator: the `0x26b60` separator builds the text `"MenuSeparator"` through the
  native `0xf1a0`, whose allocator (0x12820 / 0x12af0) is not lifted. `default_slot420_hooks()` installs the documented
  malloc/free stand-in (same convention as slots 306 / 329); `Slot420Objects` returns the storage in its destructor; with
  the hook cleared the fragment reports `ok == false` at the separator and builds nothing after it (tested).
- Slot 488 holds a 0x30 B inline holder (class 0x15a0a38 + 0x10, borrowed view `SM_debugIdentifier`, zero tail) with
  atexit 0x2b5050; slot 420 holds an 8 B zero cell with atexit 0x56ef80 (pinned 36 B body: frees a non-null pointer via
  0x170b0, so a zero cell is inert). Both destructor RVAs are IDENTITY TAGS recorded in the atexit log, never run; the
  0x15a0a38 class is not claimed.

## Startup state (coverage test)

slots 477 -> 491, `slots.size() + 82 == kOriginalCtorCount`, pending 212 -> 246, menu roots 136 -> 151, atexit_log
467 -> 519 (the menu-root record already existed, so 0x268b0 adds none). The coverage test also asserts per-slot fields
for 366, 411, 430, 504, 555, 397, 442, 450, 460, 514, 455, 488, 369, 420.

## Files

New (native): `native/core/helper_int_var_sites.{hpp,cpp}`, `native/core/ctor_slot_<N>_<rva>.{hpp,cpp}` and
`native/tests/test_ctor_slot_<N>.cpp` for the 14 slots (369, 420 and 455, 488, 514 written this segment).
New (tools): `tools/lift/check_ctor_slot_<N>.py` for the 14 slots, `tools/lift/batch6_lib.py`.
Modified: `native/CMakeLists.txt`, `native/core/startup_ctors.{hpp,cpp}` (14 rows in original-index order, objects and
results), `native/tests/test_startup_ctors.cpp` (expect[]), `native/tests/test_startup_ctor_coverage.cpp`,
`tools/lift/check_ctor_slots.py` (DEBUG_PAGE_BATCH_6 has 14 members, digest
`0c1b60a8afc23ac9b1cf11defb484ae0fb0670cdaa7cc826c044ba8f396c5687`, 82 unregistered),
`tools/lift/check_console_var_ctors.py` (COVERAGE_TOKENS 491 / 82), `tools/lift/check_fctor_slots_296_297_300.py`
(491 rows). Regenerated: `docs/entry_map.json` (2014 constants, 1340 distinct RVAs, 449 headers),
`docs/G1_EVIDENCE.md`, `docs/STARTUP_GAP_SURVEY.md` (HEAD 460, working tree 491, 82 open), `docs/FOLDERS.md`
(project_index). `check_evidence_grades.py` needed no change.

## Commands and results (build dir `local/build/startup-b6`, owned by this worker)

| Command | Result |
|---|---|
| `cmake -S native -B local/build/startup-b6` then `cmake --build local/build/startup-b6 -j 12` | exit 0, 0 warnings |
| `ctest --test-dir local/build/startup-b6 -j 12 --output-on-failure` | 575/575 passed |
| `python3 -I tools/lift/check_ctor_slot_<N>.py` (14 slots, full mutation self-test) | all exit 0 |
| same checkers with `--tamper elf` / `--tamper native` (369, 420 re-run at the end) | exit 1 |
| `check_ctor_slots.py`, `check_console_var_ctors.py`, `check_fctor_slots_296_297_300.py`, `check_evidence_grades.py`, `gen_entry_map.py --check`, `g1_evidence_pack.py --check`, `check_g1_evidence_pack.py`, `survey_startup_gaps.py --write-doc` | all exit 0 |
| `SOTC_NATIVE_BUILD=local/build/startup-b6 tools/lift/run_all_checkers.sh` | 783 passed, 0 skipped, 0 failed (exit 0) |
| `python3 -I scripts/audit_publish_paths.py --all` | OK (4110 paths) |
| `python3 scripts/project_index.py` | written (74091 folders) |

## Risks

- The `__stack_chk_guard` compare in every body is a hook (not modelled), as in all earlier slots.
- Every `0xf590` release is exact only because the views are borrowed (`control == nullptr`); a later change to owned
  views would invalidate the "inert release" claim.
- The 0x523ba0 cell destructor (430), the 0x2b5050 holder destructor (488) and the 0x56ef80 cell destructor (420) are
  recorded identity tags; their bodies and the 0x15a0a38 class are not modelled beyond the pinned stores.
- Slot 420's separator allocation uses a malloc stand-in, not the engine allocator; any consumer that cares about the
  allocator identity must replace `Slot420Hooks`.
- Readings of the names (wander bone physics, sound occlusion, safe frame, SMTP, waypoint draw, ...) are inference from the
  strings; no consumer of any of these variables or menu items is lifted.
- Slider ranges: 369 {0.01, 0.1}, 420 {1.0, 1.0} (min == max, taken from .rodata 0x101e978 exactly as the body loads it).
- Gate: no boot, audio or save claim; startup coverage is a static/fixture claim only.

## Paste-ready text

HANDOFF: "2026-11-02 worker startup batch 6: coverage 477->491/573 (14 slots wired: 366, 411, 430, 504, 555, 397, 442, 450,
460, 514, 455, 488, 369, 420; helper wrappers admit each slot's functor tag, HelperRefusal::none required; 0x27050 is native
and reused; slot 420's separator uses a documented malloc allocator stand-in). Pending 246, menu roots 151, atexit 519.
Cliff unchanged at 336 (0x7ec4c0), 82 remain. 575/575 ctest, 14 new ELF-tie checkers with --tamper probes, all count pins
and docs regenerated. STATIC_ANALYSIS_VERIFIED + FIXTURE_VERIFIED; no boot/audio/save claim. Not committed. Next: batch 7
from the 82 open rows (survey: 69 debug-page template slots were open before this batch; re-run survey_startup_gaps.py)."

TASKS: "CPU-01-AOT-CFG-SCALE-01: startup coverage 491/573 (batch 6 done; 82 open, first 336). Status in progress, attempts 0
failed."

EVIDENCE rows (E1130..E1144), one per slot, `| E11xx | 2026-11-02 | Slot N (0xRVA, B bytes): <shape> | STATIC_ANALYSIS_VERIFIED +
FIXTURE_VERIFIED | tools/lift/check_ctor_slot_N.py (pins, mutants as in the table above) |`:
E1130 366, E1131 411, E1132 430, E1133 504, E1134 555, E1135 397, E1136 442, E1137 450, E1138 460, E1139 514, E1140 455,
E1141 488, E1142 369, E1143 420; plus E1144 batch 6 totals (coverage 491/573, pending 246, roots 151, atexit 519,
575/575 ctest).
