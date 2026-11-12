# Checkpoint 2026-11-02: startup batch 5 (slot 437)

Agent: startup-ctor lane worker (Sonnet 5.5) under the orchestrator. Task: `CPU-01-AOT-CFG-SCALE-01` startup-table
coverage. Labels: STATIC_ANALYSIS_VERIFIED (ELF byte pins, offline, the private ELF is only read) and
FIXTURE_VERIFIED (native tests). No original RVA was executed. No commit, tag or push was made. HANDOFF.md,
docs/TASKS.md and docs/EVIDENCE.md were NOT edited (paste-ready text below, evidence ids E1120..E1122; the EVIDENCE
tail was E1115 when this batch finished, the helper-callee worker owns E1107..E1115).

## Result

- Startup constructor coverage 476 -> 477 / 573 (+1 row: 437). Remaining rows 96 (was 97).
  First unrecovered row unchanged: index 336 (0x7ec4c0). HEAD is 460 (batches 2-5 are uncommitted in the tree).
- One slot modelled, not six: of the ten candidates, nine are blocked on the helper callees the orchestrator excluded
  (0x6aa00 / 0x26db0 / 0xc580, plus 0x3a980 and 0x27050) or on functor slot-3 sites. Only 437 had every callee modelled.
- No three-try escalation (failed tries: 0). `docs/escalations/` untouched.

## Slot 437 (0x5070c0, 1142 B, body sha 73cde25d...)

- Three BOOL console variables (`shared_footStepSurfaceShow`, `shared_footStepEventShow`, `shared_footLiftEventShow`,
  functor tag 0x15c59a0, value == default) built with the shared `lane_m6::init_bool_var_literal`.
- New mechanism: sixteen `bp_*` borrowed string views (`bp_front_left_foot_step` .. `bp_skid_mark_4_end`) in a
  contiguous 0x20-stride array at 0x2efc550, each built by `0xf2f0` (borrowed view `{ptr, strlen(ptr), 0, 0}`; `rsi`
  is zeroed with `xorl %esi,%esi` and ignored). After every fourth element the body registers a chunk destructor
  (0x502920 / 0x502960 / 0x5029a0 / 0x5029e0) through `__cxa_atexit` with a NULL object argument. The native model
  records `(rva, nullptr)` in `StartupAtexitLog` (it already accepted a nullable object); the destructors release
  elements 3, 2, 1, 0 with the inert `0xf590` and are fixture-only helpers, never run on the startup path.
- Pins: 232 (body, `31f6`/`4889da` at the atexit and 0xf2f0 sites, the 4 destructors, the 0xf2f0 body), plus a new
  `slot437_cells` relational extra that ties element i to cell `0x2efc550 + 0x20*i` and text i, each chunk's atexit
  to element `4k+3` and destructor k, and destructor k to exactly its four cells and release calls.
- Mutants: 543 ELF + 56 native, all rejected. `--tamper`, `--tamper elf`, `--tamper native` all exit 1.
- Startup state: pending 209 -> 212, atexit_log 460 -> 467 (3 variable records + 4 NULL-object chunk records), menu roots
  136 unchanged (no menu items on this page).

## Refused (nine candidates; analysed against the ELF body, not just the survey)

| idx | RVA-class reason |
|---|---|
| 442, 450, 460, 397, 369 | rows calling the unbounded-float helper 0x6aa00 (369 also 0x27050) |
| 514, 455 | rows calling the toggle-from-variable helper 0x26db0 |
| 420, 488 | rows calling the ranged-int helper 0xc580 (488 also 0x3a980, 420 also 0x27050) |

Those helpers are being modelled by the concurrent worker (E1107..E1115) and were excluded by instruction. Several of
these bodies also contain functor slot-3 (`callq *0x18(%rax)`) sites, which is a second reason to revisit them as a
group once the helper models are wired. Nothing here is a claim that the mechanism is impossible.

## Files

New (native): `native/core/ctor_slot_437_5070c0.{hpp,cpp}`, `native/tests/test_ctor_slot_437.cpp`.
New (tools): `tools/lift/check_ctor_slot_437.py`.
Modified: `native/CMakeLists.txt` (batch-5 stanza), `native/core/startup_ctors.{hpp,cpp}` (row 437 in original-index
order, `Slot437Objects` / `Slot437Result`), `native/tests/test_startup_ctors.cpp` (expect[]),
`native/tests/test_startup_ctor_coverage.cpp` (477 / 96 / cliff 336; pending 212, atexit_log 467, roots 136; batch-5
block), `tools/lift/check_ctor_slots.py` (DEBUG_PAGE_BATCH_5 = {437}, digest
`14a05fdfcbd468797cf9782779eda0c3e4864c0c4e4a5b2671613b31e7bc242c`, 96 unregistered),
`tools/lift/check_console_var_ctors.py` (COVERAGE_TOKENS 477 / 96), `tools/lift/check_fctor_slots_296_297_300.py`
(477 rows), `tools/lift/bool_page_lib.py` (`bp_strlen_import` extra, import checks factored into `_bp_check_imports`,
and a real `--tamper native` probe), `tools/lift/l4_slot_lib.py` (see Risks). Regenerated: `docs/entry_map.json`
(1945 constants, 1272 distinct RVAs, 435 headers), `docs/G1_EVIDENCE.md`, `docs/STARTUP_GAP_SURVEY.md` (HEAD 460,
working tree 477, 96 open), `docs/FOLDERS.md` (project_index). `check_evidence_grades.py` needed no change.

## Commands and results (build dir `local/build/startup-b5`, owned by this worker)

| Command | Result |
|---|---|
| `cmake -S native -B local/build/startup-b5` then `cmake --build local/build/startup-b5 -j 12` | exit 0, 0 warnings |
| `ctest --test-dir local/build/startup-b5 -j 12 --output-on-failure` | 561/561 passed |
| `python3 -I tools/lift/check_ctor_slot_437.py` (full mutation self-test) | exit 0 |
| same checker with `--tamper`, `--tamper elf`, `--tamper native` | all exit 1 |
| `check_ctor_slots.py`, `check_console_var_ctors.py`, `check_fctor_slots_296_297_300.py`, `check_evidence_grades.py`, `check_entry_map.py`, `g1_evidence_pack.py --check`, `survey_startup_gaps.py` | all exit 0 |
| `SOTC_NATIVE_BUILD=local/build/startup-b5 tools/lift/run_all_checkers.sh` | 769 passed, 0 skipped, 0 failed (exit 0) |
| `python3 -I scripts/audit_publish_paths.py --all` | OK (4110 paths) |
| `python3 scripts/project_index.py` | written (72846 folders) |

## Risks

- The `.bss` cells (0x2efc2f8..0x2efc750) are not file-backed; the layout is tied to the RIP operands only.
- Register moves and vector zeroing in the 1142 B body are covered only by the whole-body hash; the NULL atexit argument
  is pinned separately (`31f6` at every site).
- The reading of the sixteen names as bone points / skid-mark events is inference from the strings; no consumer of the
  view array is lifted, so the model only builds and registers them.
- Harness fix in `l4_slot_lib.mutation_self_test`: the overlay is cleared before the pristine read. Without it, 5
  call-pin mutants (0xf2f0 and the four destructors) survived spuriously because of a stale overlay. All 769 checkers
  still pass with it, so no other checker was weakened or newly failing.
- `bool_page_lib.main` now makes `--tamper native` a real native probe (previously it only tampered the ELF side).
- Stack-canary compare remains a hook, as in every other bool slot. Gate: no boot, audio or save claim; startup coverage
  is a static/fixture claim.

## Paste-ready text

HANDOFF: "2026-11-02 worker startup batch 5: coverage 476->477/573 (debug-page slot 437: 3 bool vars + 16 borrowed views in
4 NULL-object atexit chunks), cliff 336/0x7ec4c0 unchanged, 96 remain; build exit 0, ctest 561/561, run_all_checkers
769/0, publish audit OK. 442/450/460/397/369/514/455/420/488 blocked on 0x6aa00/0x26db0/0xc580 helpers (concurrent
worker). No escalations. Record: docs/checkpoints/2026-11-02-startup-batch-5.md. STATIC_ANALYSIS_VERIFIED/FIXTURE_VERIFIED."

TASKS: `CPU-01-AOT-CFG-SCALE-01` startup coverage 477/573; next: wire 442, 450, 460, 397, 369, 514, 455, 420, 488 once the
helper models (0x6aa00, 0x26db0, 0xc580, 0x3a980, 0x27050) are landed and the functor slot-3 sites are classified.

EVIDENCE:
"| E1120 | 2026-11-02 | Slot 437 (0x5070c0, 1142 B): three bool console variables plus sixteen borrowed `bp_*` views built by 0xf2f0 in four chunks, each chunk registering a NULL-object atexit destructor (0x502920/0x502960/0x5029a0/0x5029e0) that releases its four cells. 232 pins plus the `slot437_cells` relational tie; 543 ELF + 56 native mutants rejected. STATIC_ANALYSIS_VERIFIED / FIXTURE_VERIFIED. |"
"| E1121 | 2026-11-02 | Startup batch 5 totals: coverage 477/573, cliff 336 (0x7ec4c0), 96 remain; pending 212, atexit_log 467, roots 136; check_ctor_slots digest 14a05fdf...; entry_map 1945 consts; build exit 0, ctest 561/561, run_all_checkers 769/0, publish audit OK. Record: docs/checkpoints/2026-11-02-startup-batch-5.md. |"
"| E1122 | 2026-11-02 | Tooling: `bool_page_lib` gains `bp_strlen_import` (PLT 0xf8f3b0 / GOT 0x15fffc8 / NID strlen) and a real `--tamper native`; `l4_slot_lib.mutation_self_test` clears a stale overlay before the pristine read (removes 5 spurious survivors). 769 checkers still pass. Nine remaining candidates (442 450 460 397 369 514 455 420 488) are blocked on helpers 0x6aa00/0x26db0/0xc580. |"
