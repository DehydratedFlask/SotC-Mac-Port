# Checkpoint 2026-11-02: startup batch 3 (slots 518, 410, 362, 477, 423, 449)

Agent: startup-ctor lane worker (Sonnet 5.5) under the orchestrator. Task: `CPU-01-AOT-CFG-SCALE-01` startup-table
coverage. Labels: STATIC_ANALYSIS_VERIFIED (ELF byte pins, offline, the private ELF is only read) and
FIXTURE_VERIFIED (native tests). No original RVA was executed. No commit, tag or push was made. HANDOFF.md,
docs/TASKS.md and docs/EVIDENCE.md were NOT edited (paste-ready text below, evidence ids E1101..E1103).

## Result

- Startup constructor coverage 465 -> 471 / 573 (+6 rows: 518, 410, 362, 477, 423, 449). Remaining rows 102 (was 108).
  First unrecovered row unchanged: index 336 (0x7ec4c0). HEAD is 460 (batch 2 is still uncommitted in the tree).
- Slot 518 (0x1a57d0): the four `0x1598a50 + 8*i` relocations DO resolve to the float bodies
  0xc4a0 / 0xc4c0 / 0xc4e0 / 0xc4f0, so the slot was modelled, not refused.
- Five bool debug-page slots modelled and registered: 410 (3 bool vars), 362 (3 vars + 1 `{name, fnv1a}` record),
  477 (4 vars with non-empty descriptions), 423 (4 vars + 2 records), 449 (2 vars + 2 `0x26ea0` toggles on them).
- No three-try escalation: every slot passed on the first modelling attempt (failed tries: 0 for all six).
  `docs/escalations/` untouched.

## Slot 518 (0x1a57d0, 527 B) - static-float tail

- One bounded float variable `vgAmbientSoundSpacing` (name length 0x15, lo 1.0f, hi 20.0f, value/default 7.0f,
  caller tag 0x1598a50 proven equivalent to the float functor class, sites 0x1a58c1 / 0x1a58dd / 0x1a58f7 / 0x1a5915),
  then a tail that stores four float pairs and derives three floats from an 8.5f cache cell:
  `spacing = f32(cache) * 0.5f; pitch = (spacing + spacing) * 0.866f (0x3f5db22d); inverse = 1.0f / pitch`
  (results `0x40880000`, `0x40eb8d50`, `0x3e0b1c8d` for the 8.5f default; cells 0x2ee66c0..0x2ee66ec in .bss).
- Model: `native/core/ctor_slot_518_1a57d0.{hpp,cpp}`; `apply_ambient_spacing_tail` is a bounded helper that takes the
  cache cell explicitly (default = the ELF's 8.5f, `0x41080000` at 0x16048a0) and applies the tail iff the variable
  was built and its atexit recorded. Each float op is rounded once (build uses `-ffp-contract=off`).
- Checker `tools/lift/check_ctor_slot_518.py` (base `debug_page_batch_lib`, plus an independent Python float32 model
  of the tail, the tail immediates, a .bss check of the cells and a check that the cache cell holds 8.5f).
  Mutation self-test and `--tamper elf|native` exit 1 as required.

## Slots 410 / 362 / 477 / 423 / 449 - bool debug pages

All five reuse only already-modelled callees: 0x20af0 / 0x20d60 / 0xf590 / `__cxa_atexit` (via
`lane_m6::init_bool_var_literal`), 0x26ea0 (via `lane_m6::init_toggle_site_target` with `debug_menu_target_for`) and
0x52a990 -> 0x161fb0 (`fnv_cstr_52a990`). Per slot the generated model is a straight statement list in body order.

| idx | RVA | B | tag | shape | pins | mutants (ELF + native) |
|---|---|--:|---|---|--:|---|
| 410 | 0x5ac580 | 735 | 0x15cb188 | 3 bool vars (1/1, 0/0, 0/0), empty descriptions | 82 | 221 + 26 |
| 362 | 0x76b8d0 | 760 | 0x15cd9f8 | 3 bool vars 0/0, then 1 record at 0x3253868 | 89 | 238 + 32 |
| 477 | 0x3270d0 | 918 | 0x15a77b0 | 4 bool vars 0/0 with 22..85-byte descriptions | 96 | 259 + 39 |
| 423 | 0x55f080 | 1000 | 0x15ca5a8 | 4 bool vars 0/0, then 2 records (0x2f06388, 0x2f06398) | 113 | 290 + 42 |
| 449 | 0x4b0200 | 776 | 0x15c0798 | var, toggle(var0), var, toggle(var1) | 87 | 244 + 33 |

- Statement orders are byte-read from the bodies: 410/362/423/449 use BOOLC, REL, REG, REL, ATEXIT per variable;
  477 uses BOOLC, REG, REL, REL, ATEXIT (both releases moved after registration). All `0xf590` calls are releases of
  stack views (`control == nullptr`), i.e. exact no-ops, so the reordering at 477 is unobservable.
- 449: the toggles target bool variables, whose class answers slot3 = 1 and slot6 = 0 (0xb890 / 0xb930), so the
  `slot3 == 1 && slot6 == 3` narrowing of 0x26ea0 does NOT hold and the item range stays {1.0f, 10.0f}. The test asserts
  this and the checker pins both the class bodies and the 0x26ea0 gate bytes. The first item also records the menu
  root destructor 0x27280 when the root is not yet initialised (standalone test); in the full startup state the root
  already exists, so 449 adds exactly 4 atexit records.
- Records (362, 423): `{name pointer at +0, fnv1a(name) at +8}`, +0xc never written; the `mov [cell], name` and
  `mov [cell+8], eax` instructions and the 0x52a990 jump (`be c59d1c81` + `jmp 0x161fb0`) are pinned.
- The bool functor tag is stored at +0xb0 and not validated by the model; the checker pins its four relocations
  (0xc2a0 / 0xc2c0 / 0xc2e0 / 0xc2f0) and the class tables.
- Hook (not modelled): the `__stack_chk_guard` compare, as in every other lane-M6 bool slot. The ELF side pins the
  `jne` and the `__stack_chk_fail` PLT call.

## Files

New (native): `native/core/ctor_slot_{518_1a57d0,410_5ac580,362_76b8d0,477_3270d0,423_55f080,449_4b0200}.{hpp,cpp}`,
`native/tests/test_ctor_slot_{518,410,362,477,423,449}.cpp`.
New (tools): `tools/lift/check_ctor_slot_{518,410,362,477,423,449}.py`, `tools/lift/bool_page_lib.py` (shared extras:
PLT import NIDs of `__cxa_atexit` / `__stack_chk_fail` and the bool object/class vtable identities; re-exports the
lane-M6 driver). The 410/362/477/423/449 sources, tests and checkers were generated from the disassembly by a
scratchpad generator (not committed) and then reviewed; the generated files are the artifact.
Modified: `native/CMakeLists.txt` (two stanzas at the end), `native/core/startup_ctors.{hpp,cpp}` (six rows, state),
`native/core/debug_page_var_item.{hpp,cpp}` (`kArgFnVtblSlot518`), `native/tests/l4_test_util.hpp`
(`check_bool_with_description`), `native/tests/test_startup_ctor_coverage.cpp` (471 / 102 / cliff 336; batch-3 block;
pending 176 -> 192, atexit_log 413 -> 431, menu roots 123 -> 125), `native/tests/test_startup_ctors.cpp` (expect[]),
`tools/lift/check_ctor_slots.py` (DEBUG_PAGE_BATCH_3 = {362, 410, 423, 449, 477, 518}, digest
`40c7694552f152fa4aec578f3e98e4b8e3df12b00efa4ce864bf8ecc02e6eca8`, 102 unregistered),
`tools/lift/check_console_var_ctors.py` (COVERAGE_TOKENS 471 / 102), `tools/lift/check_fctor_slots_296_297_300.py`
(471 rows). Regenerated: `docs/entry_map.json` (1930 constants, 1263 distinct RVAs, 428 headers),
`docs/G1_EVIDENCE.md` (registered 471), `docs/STARTUP_GAP_SURVEY.md` (HEAD 460, working tree 471, 102 open).
`check_evidence_grades.py` needed no change.

## Commands and results (build dir `local/build/startup-b3`, owned by this worker)

| Command | Result |
|---|---|
| `cmake -S native -B local/build/startup-b3` then `cmake --build local/build/startup-b3 -j 12` | exit 0 |
| `ctest --test-dir local/build/startup-b3 -j 12 --output-on-failure` | 551/551 passed |
| `python3 -I tools/lift/check_ctor_slot_{518,410,362,477,423,449}.py` (full mutation self-test) | all exit 0 |
| same checkers with `--tamper` | all exit 1 (518 also `--tamper elf` and `--tamper native`) |
| `check_ctor_slots.py`, `check_console_var_ctors.py`, `check_fctor_slots_296_297_300.py`, `check_evidence_grades.py`, `check_entry_map.py`, `g1_evidence_pack.py --check` | all exit 0 |
| `SOTC_NATIVE_BUILD=local/build/startup-b3 tools/lift/run_all_checkers.sh` | 759 passed, 0 skipped, 0 failed |
| `python3 -I scripts/audit_publish_paths.py --all` | OK (4110 paths) |
| `python3 scripts/project_index.py` | written (69167 folders) |

## Skipped / refused (not taken in this batch)

The task cap was five extra slots. Skipped, with reasons:

- Slots needing an unmodelled callee (per the survey `unmodelled helpers` column >= 1, the survey's own definition):
  464, 506, 493, 390, 566, 398, 454, 392 (one each), 524, 510 (three). Earlier-batch refusals still stand: 430 (0xc580
  row), 504 (0x6aa00 + bool rows), 366 / 411 (0x26db0), 555 (0x257a0 / 0x3a980), leaves 409 / 414 / 494
  (0x17070 / 0x3b310 / 0x26e340 / 0x26e770), 418 (4,726 B with 179 hash calls).
- The 0-unmodelled-helper M-tier slots (e.g. 442, 550, 450, 514, 455, 570, 460, 512, 369, 443, 420, 488, 397) were not
  analysed in this batch; their shapes (functor slot-3 sites, 0x26db0 / 0x6aa00 rows, 6-8 distinct callees) go beyond the
  bool-only and bool-plus-toggle bodies modelled here. They are NOT claimed refused for a missing callee.

## Risks

- Slot 518: "no earlier constructor writes the 8.5f cache cell 0x16048a0" is a static enumeration (its only other code
  reference is a runtime function at 0x1a39a6), not an execution. The model keeps the cache as an explicit field with the
  ELF default so a host that learns a different runtime value can supply it.
- Slot 518 applies the tail after the atexit record and before the canary, which is the body's order; a failed
  variable build suppresses the tail (`ok == false`), which the original cannot do (unconditional) - fail-visible, not a
  silent pass.
- Slot 477 release order differs from the other bool slots (see above); unobservable because both releases are inert.
- Rip-operand pin lists cover every direct call, rip operand, immediate store and branch; they were generated from the
  disassembly and validated by the mutation suite, not independently derived. The three non-pinned instruction classes
  (register moves, vector zeroing, stack-view field stores) are covered only by the whole-body hash.
- The lane-M6 driver supports `--tamper` (ELF) only; native-source mutation coverage comes from the self-test's native
  mutants (constants, strings, tokens), not a separate `--tamper native` switch (518 has both, via
  `debug_page_batch_lib`).
- Coverage pins (pending 192, atexit 431, roots 125) were measured by the tests, not derived independently.
- Gate: no boot, audio or save claim; STARTUP coverage is a static/fixture claim.

## Paste-ready text

HANDOFF: "2026-11-02 worker startup batch 3: coverage 465->471/573 (518 static-float tail; bool pages 410, 362, 477,
423, 449), cliff 336/0x7ec4c0 unchanged, 102 remain; build exit 0, ctest 551/551, run_all_checkers 759/0, publish audit
OK. 518: 0x1598a50 relocs = float bodies, tail modelled with explicit 8.5f cache cell (inference risk documented). No
escalations. Record: docs/checkpoints/2026-11-02-startup-batch-3.md. STATIC_ANALYSIS_VERIFIED/FIXTURE_VERIFIED."

TASKS: `CPU-01-AOT-CFG-SCALE-01` startup coverage 471/573; next: 0-unmodelled-helper M-tier debug-page slots (442,
550, 450, 514, 455 first), then the callee models for 0x26db0 / 0xc580 rows.

EVIDENCE:
"| E1101 | 2026-11-02 | Slot 518 (0x1a57d0): relocations `0x1598a50 + 8*i` resolve to float bodies 0xc4a0/0xc4c0/0xc4e0/0xc4f0; one bounded float var `vgAmbientSoundSpacing` plus a static-float tail (pairs {1,0.5},{0,0.866},{1,-0.5774},{0,1.1547}; spacing = cache*0.5, pitch = 2*spacing*0.866, inverse = 1/pitch; cache default 8.5f at 0x16048a0). Checker with independent float32 model and mutation/tamper probes. STATIC_ANALYSIS_VERIFIED / FIXTURE_VERIFIED. Risk: cache writer is runtime-only by static enumeration. |"
"| E1102 | 2026-11-02 | Bool debug pages 410/362/477/423/449 modelled from shared callees only (0x20af0, 0x20d60, 0xf590, atexit, 0x26ea0, 0x52a990). 449's toggles target bool vars (slot3=1, slot6=0): no {0.1,0.5} narrowing, range {1,10}. 477 releases are reordered but inert. Per-slot checkers with 82/89/96/113/87 pins and 247/270/298/332/277 mutants all rejected. STATIC_ANALYSIS_VERIFIED / FIXTURE_VERIFIED. |"
"| E1103 | 2026-11-02 | Startup batch 3 totals: coverage 471/573, cliff 336 (0x7ec4c0), 102 remain. pins: coverage test, expect[], check_ctor_slots (digest 40c76945...), check_console_var_ctors, check_fctor, entry_map (1930 consts), G1 evidence, gap survey. Build exit 0, ctest 551/551, run_all_checkers 759/0, publish audit OK. Record: docs/checkpoints/2026-11-02-startup-batch-3.md. |"
