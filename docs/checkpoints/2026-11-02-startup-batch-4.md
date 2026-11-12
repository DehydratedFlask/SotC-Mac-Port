# Checkpoint 2026-11-02: startup batch 4 (slots 550, 512, 443, 368, 338)

Agent: startup-ctor lane worker (Sonnet 5.5) under the orchestrator. Task: `CPU-01-AOT-CFG-SCALE-01` startup-table
coverage. Labels: STATIC_ANALYSIS_VERIFIED (ELF byte pins, offline, the private ELF is only read) and
FIXTURE_VERIFIED (native tests). No original RVA was executed. No commit, tag or push was made. HANDOFF.md,
docs/TASKS.md and docs/EVIDENCE.md were NOT edited (paste-ready text below, evidence ids E1104..E1106; the EVIDENCE
tail was E1103 when this batch started).

## Result

- Startup constructor coverage 471 -> 476 / 573 (+5 rows: 550, 512, 443, 368, 338). Remaining rows 97 (was 102).
  First unrecovered row unchanged: index 336 (0x7ec4c0). HEAD is 460 (batches 2-4 are still uncommitted in the tree).
- Five slots modelled, not six: the suggested list (442, 450, 514, 455, 570, 460, 369, 420, 488, 397) was NOT taken
  because the orchestrator excluded every slot that needs the helper callees 0x26db0 / 0xc580 / 0x6aa00 (another
  worker is adding those models). See "Skipped" for which slots those are.
- No three-try escalation: every slot passed on its first modelling attempt (failed tries: 0 for all five).
  `docs/escalations/` untouched.

## Slots (all value == default; every callee was already modelled)

| idx | RVA | B | tag | shape | pins | mutants (ELF + native) |
|---|---|--:|---|---|--:|---|
| 550 | 0x91290 | 706 | 0x1597680 | var `r_showRenderContextMemoryStats` (0/0), toggles "RenderDebug" + "MemoryMisc", then a `NameHolder30` for `r_threadRObjectNewAndFree` (vptr 0x15976b0, atexit 0x8ef00) | 92 | 270 + 42 |
| 512 | 0x1d5e80 | 902 | 0x1599288 | var (1/1), group "Static Touch Manager" (empty parent), toggle, var (0/0), toggle | 96 | 264 + 40 |
| 443 | 0x4de370 | 936 | 0x15c2400 | 2 vars (0/0), group (parent "CP10", caption "Scripted Events"), 2 toggles | 120 | 314 + 41 |
| 368 | 0x73df50 | 1317 | 0x15cd1a0 | 3 vars (1/1, 1/1, 0/0), 3 toggles ("Shadow IK"), then 18 `movl $imm32, [rip]` static-cell stores | 155 | 384 + 49 |
| 338 | 0x7e6c90 | 2025 | 0x15ce460 | 9 bool vars (0/0 each), no menu items | 196 | 462 + 57 |

- Shared pieces only: `lane_m6::init_bool_var_literal` (0x20af0 / 0x20d60 / 0xf590 / `__cxa_atexit`),
  `lane_m6::init_toggle_site_target` (0x26ea0), `family_group_item_26c10` (0x26c10 group item: parent view = `label`,
  caption = `text` and `empty`, kind = kMenuKindGroup = 1, target nullptr, flags 0x30) and the NameHolder30 holder from slot 539.
- Toggles target bool variables (class slot3 = 1, slot6 = 0), so the `0x26ea0` `slot3 == 1 && slot6 == 3` narrowing does
  not hold and the range stays {1.0, 10.0} (same as slot 449). The first menu item registered in a standalone test also
  records the root destructor 0x27280 before 0x272d0; in the full startup state the root already exists.
- Slot 368 tail: 18 literal dword stores to static cells (`Slot368StaticStore{rva, bits}` table, `statics[]` array in the
  result). These cells have no lifted consumer yet; the stores are pinned and applied in body order.
- Slot 550 holder: NameHolder30 (SharedStr view at +8, tail 0 at +0x28, vptr 0x15976b0 = 0x15976a0 + 0x10, atexit
  0x8ef00). The dtor 0x8ef00 is 23 B, sha 32799b7b..., vtables 0x15976b0 / 0x1597658; the checker pins all of it.
- Views are borrowed (`control == nullptr`), so every `0xf590` release is an exact no-op. The `__stack_chk_guard`
  compare is a hook; the ELF side pins the `jne` and `__stack_chk_fail` PLT call.

## Files

New (native): `native/core/ctor_slot_{550_91290,512_1d5e80,443_4de370,368_73df50,338_7e6c90}.{hpp,cpp}`,
`native/tests/test_ctor_slot_{550,512,443,368,338}.cpp`.
New (tools): `tools/lift/check_ctor_slot_{550,512,443,368,338}.py`. The sources, tests and checkers were generated from
the disassembly by a scratchpad generator (not committed) and then run/reviewed; the generated files are the artifact.
Modified: `tools/lift/bool_page_lib.py` (`x_bp_holder_vtables` extra, `EXTRAS["bp_holder_vtables"]`),
`native/CMakeLists.txt` (batch-4 stanza: sources, five test executables, five `add_test`),
`native/core/startup_ctors.{hpp,cpp}` (five rows in original-index order, `Slot{338,368,443,512,550}Objects` +
`...Result` members), `native/tests/test_startup_ctor_coverage.cpp` (476 / 97 / cliff 336; batch-4 block; pending
192 -> 209, menu roots 125 -> 136, atexit_log 431 -> 460), `native/tests/test_startup_ctors.cpp` (expect[]),
`tools/lift/check_ctor_slots.py` (DEBUG_PAGE_BATCH_4 = {338, 368, 443, 512, 550}, digest
`e7c4f284596c09e26cbb21aeb4ffcf3895764ffcf1e5a8aaee7070df96160321`, 97 unregistered),
`tools/lift/check_console_var_ctors.py` (COVERAGE_TOKENS 476 / 97), `tools/lift/check_fctor_slots_296_297_300.py`
(476 rows). Regenerated: `docs/entry_map.json` (1944 constants, 1271 distinct RVAs, 434 headers),
`docs/G1_EVIDENCE.md` (registered 476), `docs/STARTUP_GAP_SURVEY.md` (HEAD 460, working tree 476, 97 open).
`check_evidence_grades.py` needed no change.

## Commands and results (build dir `local/build/startup-b4`, owned by this worker)

| Command | Result |
|---|---|
| `cmake -S native -B local/build/startup-b4` then `cmake --build local/build/startup-b4 -j 12` | exit 0, 0 warnings |
| `ctest --test-dir local/build/startup-b4 -j 12 --output-on-failure` | 560/560 passed |
| `python3 -I tools/lift/check_ctor_slot_{550,512,443,368,338}.py` (full mutation self-test) | all exit 0 |
| same checkers with `--tamper`, `--tamper elf`, `--tamper native` | all exit 1 |
| `check_ctor_slots.py`, `check_console_var_ctors.py`, `check_fctor_slots_296_297_300.py`, `check_evidence_grades.py`, `check_entry_map.py`, `g1_evidence_pack.py --check` | all exit 0 |
| `SOTC_NATIVE_BUILD=local/build/startup-b4 tools/lift/run_all_checkers.sh` | 764 passed, 0 skipped, 0 failed (exit 0) |
| `python3 -I scripts/audit_publish_paths.py --all` | OK (4110 paths) |
| `python3 scripts/project_index.py` | written (71609 folders) |

## Skipped / refused (not taken in this batch)

- Taken out of the suggested order because they need the helper callees the orchestrator excluded (0x26db0 / 0xc580 /
  0x6aa00 are being modelled by a concurrent worker): 442, 450, 514, 455, 570, 460, 369, 420, 488, 397. Per the survey
  these are the M-tier slots with functor slot-3 sites or 0x26db0 / 0x6aa00 rows; they are NOT claimed refused for a
  missing mechanism beyond that, and should be revisited once those models land.
- Refused for a genuinely new mechanism (this lane): 570 (functor slot-3 sites), 388, 391.
- Earlier-batch refusals still stand: 430 (0xc580 row), 504 (0x6aa00 + bool rows), 366 / 411 (0x26db0), 555
  (0x257a0 / 0x3a980), leaves 409 / 414 / 494, 418 (4,726 B with 179 hash calls).
- 437 (borrowed-string array) was not taken; optional.

## Risks

- Slot 368: the 18 static dword cells have no lifted consumer; the model stores literals (ELF-pinned), nothing reads them.
- Counts (pending 209, roots 136, atexit_log 460) were measured by the tests, not derived independently; the pre-run
  predictions (+17 vars, +11 menu objects, +29 atexit records) matched.
- Rip-operand/immediate pin lists were generated from the disassembly and validated by the mutation suite, not
  independently derived; register moves, vector zeroing and stack-view field stores are covered only by the whole-body hash.
- Stack-canary compare is a hook, as in every other bool slot. Value/default are the ELF literals; no runtime path that
  changes them is modelled.
- The generator lives only in the scratchpad; the generated files are the reviewed artifact.
- Gate: no boot, audio or save claim; startup coverage is a static/fixture claim.

## Paste-ready text

HANDOFF: "2026-11-02 worker startup batch 4: coverage 471->476/573 (debug-page slots 550, 512, 443, 368, 338), cliff
336/0x7ec4c0 unchanged, 97 remain; build exit 0, ctest 560/560, run_all_checkers 764/0, publish audit OK. 442/450/
514/455/570/460/369/420/488/397 left for the helper-callee models (0x26db0/0xc580/0x6aa00). No escalations. Record:
docs/checkpoints/2026-11-02-startup-batch-4.md. STATIC_ANALYSIS_VERIFIED/FIXTURE_VERIFIED."

TASKS: `CPU-01-AOT-CFG-SCALE-01` startup coverage 476/573; next: slots blocked on 0x26db0 / 0xc580 / 0x6aa00 once the
concurrent callee models land (442, 450, 514, 455, 460, 369, 420, 488, 397), then remaining 0-helper debug pages.

EVIDENCE:
"| E1104 | 2026-11-02 | Slots 550/512 (0x91290, 0x1d5e80): bool var + 0x26ea0 toggles (+ 0x26c10 group item for 512, NameHolder30 `r_threadRObjectNewAndFree` with dtor 0x8ef00 for 550) built only from modelled callees. Toggles on bool vars keep range {1,10}. Checkers 92/96 pins, 312/304 mutants rejected. STATIC_ANALYSIS_VERIFIED / FIXTURE_VERIFIED. |"
"| E1105 | 2026-11-02 | Slots 443/368/338 (0x4de370, 0x73df50, 0x7e6c90): 443 two vars + group + two toggles; 368 three vars + three toggles + 18 literal static dword stores (no lifted consumer); 338 nine bool vars. Checkers 120/155/196 pins, 355/433/519 mutants rejected. STATIC_ANALYSIS_VERIFIED / FIXTURE_VERIFIED. |"
"| E1106 | 2026-11-02 | Startup batch 4 totals: coverage 476/573, cliff 336 (0x7ec4c0), 97 remain. pins: coverage test (pending 209, roots 136, atexit_log 460), expect[], check_ctor_slots (digest e7c4f284...), check_console_var_ctors, check_fctor, entry_map (1944 consts), G1 evidence, gap survey. Build exit 0, ctest 560/560, run_all_checkers 764/0, publish audit OK. Record: docs/checkpoints/2026-11-02-startup-batch-4.md. |"
