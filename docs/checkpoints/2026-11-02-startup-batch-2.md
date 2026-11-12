# Checkpoint 2026-11-02: startup batch 2 (slots 374, 448, 507, 539, 545)

Agent: worker (Sonnet 5.5) under the orchestrator. Task: `CPU-01-AOT-CFG-SCALE-01` startup-table coverage.
Labels: STATIC_ANALYSIS_VERIFIED (ELF byte pins) + FIXTURE_VERIFIED (native tests). Nothing was run
from the game; guest bytes were never executed. No commit, tag or push was made.

## Result

- Startup constructor coverage 460 -> 465 / 573 (+5 rows: 374, 448, 507, 539, 545).
- First unrecovered row unchanged: index 336 (0x7ec4c0). Remaining rows 108 (was 113).
- Red default build fixed without suppressing warnings (explicit empty `hashed_names` spans in the
  four committed per-slot Script initializers and their adapters: 361, 379, 467, 536).
- Slot 518 (0x1a57d0) was surveyed and designed but NOT implemented (see Refusals/Pending).

## Files changed

- Fix: `native/core/ctor_slot_{361_777390,379_6d8d20,467_3ca110,536_1342d0}.{hpp,cpp}`,
  `tools/lift/check_ctor_slot_536.py`.
- Registration: `native/core/startup_ctors.{hpp,cpp}` (StartupState members + five rows; slot 545 is
  passed `kNoCookie = 0` for both cookies, see Hooks).
- Slot 374 (peer-authored files, verified here, unchanged): `native/core/ctor_slot_374_701230.{hpp,cpp}`,
  `native/tests/test_ctor_slot_374.cpp`, `tools/lift/check_ctor_slot_374.py`, stanza at the end of
  `native/CMakeLists.txt`.
- Pins: `native/tests/test_startup_ctor_coverage.cpp` (465 / +108 / cliff 336, batch-2 block, pending 175,
  menu roots 123, atexit_log 412), `native/tests/test_startup_ctors.cpp` (expect[] 465),
  `tools/lift/check_ctor_slots.py` (DEBUG_PAGE_BATCH_2, 108 unregistered),
  `tools/lift/check_console_var_ctors.py` (COVERAGE_TOKENS), `tools/lift/check_fctor_slots_296_297_300.py`
  (465 rows), `tools/lift/check_ctor_slot_545.py` (the stale "slot 545 remains absent" assertion became
  a positive "row registered with kNoCookie" assertion), `docs/entry_map.json` (1910 constants, 1245
  distinct RVAs, 422 headers), `docs/STARTUP_GAP_SURVEY.md`, `docs/G1_EVIDENCE.md` (regenerated).
- `tools/lift/debug_page_batch_gen.py` (untracked peer WIP): one syntax error fixed, otherwise not used,
  not relied on, unfinished (no main).

## Commands and results (build dir `local/build/startup-b2`)

| Command | Result |
|---|---|
| `cmake --build local/build/startup-b2 -j 12` | exit 0 |
| `ctest --test-dir local/build/startup-b2 -j 12 --output-on-failure` | 545/545 passed |
| `check_ctor_slot_{361,374,379,448,467,507,536,539,545}.py` (one at a time) | all exit 0 |
| `check_ctor_slots.py`, `check_console_var_ctors.py`, `check_fctor_slots_296_297_300.py`, `check_entry_map.py` | all exit 0 |
| `SOTC_NATIVE_BUILD=local/build/startup-b2 tools/lift/run_all_checkers.sh` | 753 passed, 0 failed (after regenerating G1 evidence and the gap survey) |
| `python3 -I scripts/audit_publish_paths.py --all` | OK (4110 paths) |

## Modelled versus hooked

- Slots 374/361/467/379/536: bounded float variables via `debug_page_var_item` (0xc300 class 0x15942b0,
  slider 0x27050 / toggle 0x26ea0 items, atexit dtors 0x20f0/0x272d0). Slot 374 adds the leading
  `{name, fnv1a(name)}` cell (0x52a990 -> 0x161fb0). Only hook: the stack-guard (null means
  `canary_checked == false`).
- Slots 448/507/539: bool console variables on lane M6 shapes, ELF-pinned (132/68/153 operand pins).
- Slot 545: 32-byte constant source + 64-byte object prefix, matrix helper 0xab860 byte-verified,
  one atexit identity, two conditional tag-2 releases. Hook: both cookies are `kNoCookie = 0` (the
  host-supplied cookie value is a runtime input, not recovered).

## Refusals (callee unmodelled, no stub added)

0xc580 (slot 430); 0x6aa00 + 0x20af0 bool rows (504); 0x26db0 (366, 411); 0x257a0 / 0x3a980 (555);
0x17070 / 0x3b310 / 0x26e340 / 0x26e770 in leaves 409, 414, 494; 418 is 4,726 B with 179 hash calls.

## Pending: slot 518 (0x1a57d0, 527 B)

Fits the model's callee set but needs a slot-specific tail beyond the shared template: one float row
(`vgambientSoundSpacing`, len 0x15, lo 1.0f, hi 20.0f, value 7.0f, tag 0x1598a50, sites 0x1a58c1,
0x1a58dd, 0x1a58f7, 0x1a5915) plus a static-float block derived from an 8.5f cache cell fed from
0x16048a0 (mulss/addss/divss chain at 0x1a593e..0x1a59b9). Body sha256
`172ed60c075ac81fd92073a19a808c8d7a6a256fcffd26976fab7b145bd54829`, FDE 0x1358330 reclen 36. Next
exact action: verify that the 0x1598a50 relocations resolve to the float tag; if not, refuse. No 518
files exist. Failed tries for 518: 0.

## Risks

- Slot 545 state is self-linked and must not be copied (`StartupState` is never copied).
- The writer of 0x16048a0 (two refs, `local/validation/startup_batch2/refs_16048a0.txt`) is runtime-only
  by inference.
- Rip-operand pin lists are not exhaustive; derived pins (pending 175, roots 123, atexit_log 412) were
  validated by the tests, not by an independent derivation.
- `debug_page_batch_lib.py` MODEL_ORDER does not yet contain the hashed-name step (374 only).
- Gate: no boot, audio or save claim; STARTUP coverage is a static/fixture claim.

## Paste-ready text

HANDOFF: "2026-11-02 worker startup batch 2: coverage 460->465/573 (374, 448, 507, 539, 545 registered),
cliff 336/0x7ec4c0 unchanged, 108 remain; red default build fixed (explicit hashed_names); build exit 0,
ctest 545/545, run_all_checkers 753/0, publish audit OK. 518 documented pending. Record:
docs/checkpoints/2026-11-02-startup-batch-2.md. STATIC_ANALYSIS_VERIFIED/FIXTURE_VERIFIED."

TASKS: `CPU-01-AOT-CFG-SCALE-01` startup coverage 465/573; next: slot 518, then M-tier debug-page slots.

EVIDENCE: "| E1098 | 2026-11-02 | Startup batch 2: slots 374, 448, 507, 539, 545 registered, coverage 465/573,
cliff unchanged at 336 (0x7ec4c0). ELF-pinned checkers 374/448/507/539/545 with mutation self-tests; counts
pinned in coverage test, expect[], check_ctor_slots, check_console_var_ctors, check_fctor, entry_map.
ctest 545/545, run_all_checkers 753/0. Hook: slot 545 kNoCookie, stack guard. STATIC_ANALYSIS_VERIFIED /
FIXTURE_VERIFIED. Record: docs/checkpoints/2026-11-02-startup-batch-2.md. |"
