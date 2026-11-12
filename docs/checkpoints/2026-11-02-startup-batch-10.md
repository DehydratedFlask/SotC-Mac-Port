# Startup batch 10 (worker CPU-01-STARTUP-BATCH-10), 2026-11-02

Labels: STATIC_ANALYSIS_VERIFIED + FIXTURE_VERIFIED. Nothing executed an original RVA; no boot, audio or save claim.
Build dir `local/build/startup-b10` (owned by this worker). Not committed (workers do not commit). HANDOFF/TASKS/EVIDENCE not edited (paste text below).
Slots 391/561/469/401/501/508/352 (the helper-callee worker's list) were not touched or wired.

## Result

Coverage 511 -> 516/573 (57 open). The batch-9 cliff **340 (0x7de7e0) is moved**. New first unrecovered slot: **352 (0x7a2390)**, which belongs to the
helper-callee worker's list and was deliberately left alone.

Static resolution of the indirect calls (observation: the ELF RELA at the loaded slot; the checker pins it):

- `callq *0x20(%rax)` / `movq 0x20(%rax),%r15 ... callq *%r15` / the `r13`, `0x38(%rsp)` register-once forms in 340, 342, 388 and 480 all load from
  `rax = 0x1594dc0` (the FConCmd registration vtable). The slot `0x1594dc0 + 0x20` is RELA-resolved to **0x20d60** (the console-command register-once). So
  these are direct calls in disguise, not data-dependent dispatch. Modelled by `init_fconcmd_object` (vptr 0x1594dd0, flags 2, delegate {self,0x1990,0,handler}, atexit 0x4b0).
  The handler bodies are identity tags (hash-pinned by FDE extent, never run).
- `callq *0x18(%rax)` in 342, 349 and 480 is the float caller-functor destroy of the 68-table family. The functor tag must be in `kBareRetDestroyFunctors`
  and the four float bodies (0xc4a0/0xc4c0/0xc4e0/0xc4f0) are pinned by each checker. New tags: 0x15ce2b8 (349), 0x15a66b8 (480).
- Slot 342's `0x23fd00` calls are the HashedName20 name-cell initialiser (new `name_cell_23fd00`, 86 B, hash-pinned; calls `0x162020` and the strncpy PLT).

| Slot | RVA / bytes | Shape | Checker (pins; ELF + native mutants) |
|---|---|---|---|
| 340 | 0x7de7e0 / 1800 | six console commands, a name cell, a menu group and a `0x26ea0` toggle over a command object | check_ctor_slot_340 (133; 345 + 59) |
| 342 | 0x7d8e10 / 1070 | bool var, two unbounded floats (tag 0x15ce3f0), two NULL `0x23fd00` cells, command `cp10_skipCurCutsceneCut` | check_ctor_slot_342 (95; 287 + 44) |
| 349 | 0x7bde50 / 11443 | 9 bool, 28 bounded float (tag 0x15ce2b8), 2 toggles, 3 sliders, 7 NameHolder30 (camera/photo-mode page, inference) | check_ctor_slot_349 (856; 1909 + 186) |
| 388 | 0x6a8c60 / 733 | bool var, `0x26ea0` toggle, command `gCP10ExtractCP3EventFrameTables` | check_ctor_slot_388 (78; 231 + 38) |
| 480 | 0x30daf0 / 1907 | 3 bounded floats (tag 0x15a66b8), 2 bool, commands `clearPersistentMessages` / `clearErrorMessages` | check_ctor_slot_480 (177; 459 + 40) |
| helper | 0x23fd00 / 86 | name cell initialiser | check_name_cell_23fd00 (8; 37 + 19) |

Each slot checker: plain 0, `--tamper elf` 1, `--tamper native` 1 (re-run at close: 340/342/349/388/480 all `0 1 1`; name cell plain 0).

## Refusals (with evidence)

- **402**: not mechanical. It needs a `0x257a0` string var, described bool vars, a global value-ref float and `0x23c90`; none is modelled, and the generator's
  row accounting (`len(rows) - cells == len(atexit)`) does not close. Not wired, nothing faked.
- **370** (52949 B): 138 slot-3 sites, 3 register-form indirect calls, unmodelled wrappers `0x720430` / `0x7207f0`, `0x257a0` and holders. Not mechanical via the
  batch generator. Refused.
- **367** (0x740980, 4701 B; 6 bool + 12 unbounded + 1 bounded float, 19 atexit records, NO menu rows): generator-clean as a row table, but the generator is
  menu-coupled (first-item index, `menu.root.count`, the `0x27280` root record, the failure test). Producing it needs a menu-less generator mode; not done in this
  batch (open candidate, not a failure: zero tries consumed).
- Other candidates assessed and rejected: 381 (3 `*%r15` commands, 6 cells; needs the command row kind in the generator), 503 (9 commands, `0xf8f630` call unknown).
- No hypothesis "tries" were consumed on any slot, so no `docs/escalations/` file was written.

## Tooling note

The generator (scratchpad, not in the repo) gained per-row holder classes: two NameHolder30 classes exist, vptr 0x15976b0/atexit 0x8ef00 and vptr 0x159e1a0/atexit
0x2fced0. Its tracker silently drops unrecognised rows and misclassifies console commands (vptr 0x1594dd0, flags 2) as `bool`, so commands are hand-written with the
340/388/480 pattern; row accounting is the safety net.

## Files

New: `native/core/ctor_slot_{340_7de7e0,342_7d8e10,349_7bde50,388_6a8c60,480_30daf0}.{hpp,cpp}`, `native/core/name_cell_23fd00.{hpp,cpp}`,
`native/tests/test_ctor_slot_{340,342,349,388,480}.cpp`, `native/tests/test_name_cell_23fd00.cpp`,
`tools/lift/check_ctor_slot_{340,342,349,388,480}.py`, `tools/lift/check_name_cell_23fd00.py`.
Edited: `ctor_shapes_343_356.{hpp,cpp}` (tags 0x15ce2b8, 0x15a66b8), `startup_ctors.{hpp,cpp}`, `native/CMakeLists.txt`; pins: `test_startup_ctor_coverage.cpp`
(516 / first 352 / 0x7a2390 / +57; pending 435, menu roots 242, atexit 818), `test_startup_ctors.cpp` `expect[]`, `check_ctor_slots.py` (DEBUG_PAGE_BATCH_10, 57 unregistered,
first 352, index digest ea25d747...), `check_console_var_ctors.py`, `check_fctor_slots_296_297_300.py` (516 rows).
Regenerated: `docs/entry_map.json` (2109 constants -> 1429 RVAs in 475 headers), `docs/G1_EVIDENCE.md`, `docs/STARTUP_GAP_SURVEY.md`, `docs/FOLDERS.md` (80554 folders).
Note: several of the above files are also modified by the concurrent helper-callee worker; the regenerated docs reflect the shared working tree.

## Commands and results (workspace root)

- `cmake -S native -B local/build/startup-b10 && cmake --build local/build/startup-b10 -j 12`: exit 0, 0 warnings.
- `ctest --test-dir local/build/startup-b10 -j 12 --output-on-failure`: 602/602 passed.
- `SOTC_NATIVE_BUILD=local/build/startup-b10 tools/lift/run_all_checkers.sh`: 811 passed, 0 skipped, 0 failed.
- `python3 tools/lift/gen_entry_map.py [--check]`, `g1_evidence_pack.py --write` + `--check`, `check_g1_evidence_pack.py`, `survey_startup_gaps.py --write-doc`
  (HEAD 460/573, working tree 516/573, 113 gaps, 57 open), `check_evidence_grades.py` (41 site-provable / 42 table-level / 83 rows): all OK.
- `python3 -I scripts/audit_publish_paths.py --all`: exit 0 (4110 paths). `python3 scripts/project_index.py`: written.

## Paste-ready text

### HANDOFF.md
```
CPU-01-STARTUP-BATCH-10 (2026-11-02, worker): coverage 511 -> 516/573. Slots 340, 342 (+ name cell 0x23fd00), 349, 388, 480 modelled, tested, ELF-pinned, registered.
Indirect calls resolved statically: *0x20(%rax)/*%r15/r13/0x38(%rsp) register-once forms load [0x1594dc0+0x20] = 0x20d60 (RELA); *0x18(%rax) is the float caller-functor destroy.
New first unrecovered slot 352 (0x7a2390, helper-callee worker's list, untouched). Refused: 402, 370 (not mechanical), 367 (needs menu-less generator mode), 381, 503.
Gates: build exit 0 / 0 warnings; ctest 602/602; run_all_checkers 811/0 failed; audit --all OK. Labels STATIC_ANALYSIS_VERIFIED + FIXTURE_VERIFIED; no boot/audio/save claim.
Next exact action: wait for the helper-callee worker's 352/391/561/469/401/501/508 wiring; then 367 (menu-less generator mode), 381/503 (command row kind). Attempts: 0 consumed.
```

### docs/TASKS.md
```
CPU-01 startup ctors: 516/573 (57 open), cliff 352 (0x7a2390). Batch 10 DONE for 340/342/349/388/480. Open: 367 (generator-clean, menu-less), 381, 503, 402 (0x257a0 string var), 370 (52949 B).
```

### docs/EVIDENCE.md
```
| E1190 | 2026-11-02 | Slot 340 (0x7de7e0, 1800 B): six console commands registered through a register-once indirect call; `r13`/`0x38(%rsp)` forms resolved to [0x1594dc0+0x20] = 0x20d60 by RELA; name cell, menu group, `0x26ea0` toggle over a command object. `check_ctor_slot_340` 133 pins, 345 ELF + 59 native mutants rejected. STATIC_ANALYSIS_VERIFIED / FIXTURE_VERIFIED. Record: `docs/checkpoints/2026-11-02-startup-batch-10.md`. |
| E1191 | 2026-11-02 | `name_cell_23fd00` (86 B, HashedName20 initialiser; calls 0x162020 + strncpy PLT) and slot 342 (0x7d8e10, 1070 B: bool, two unbounded floats tag 0x15ce3f0 with `*0x18(%rax)` caller-functor destroy, two NULL name cells, command `cp10_skipCurCutsceneCut`). 8 + 95 pins; 37+19 and 287+44 mutants rejected. STATIC_ANALYSIS_VERIFIED / FIXTURE_VERIFIED. |
| E1192 | 2026-11-02 | Slot 349 (0x7bde50, 11443 B): 9 bool, 28 bounded float (tag 0x15ce2b8 added to kBareRetDestroyFunctors), 2 toggles, 3 sliders, 7 NameHolder30 of vptr 0x159e1a0 / atexit 0x2fced0 (two holder classes exist). 856 pins; 1909 ELF + 186 native mutants rejected. Reading of the page as photo-mode camera is inference from names. |
| E1193 | 2026-11-02 | Slots 388 (0x6a8c60, 733 B: bool + `0x26ea0` toggle + command `gCP10ExtractCP3EventFrameTables`) and 480 (0x30daf0, 1907 B: 3 bounded floats tag 0x15a66b8, 2 bool, commands `clearPersistentMessages`/`clearErrorMessages`). 78 + 177 pins; 231+38 and 459+40 mutants rejected. Command handler bodies are identity tags, hash-pinned by extent, never run. |
| E1194 | 2026-11-02 | Refusals: 402 (needs 0x257a0 string var, described bools, global value-ref float, 0x23c90), 370 (52949 B, 138 slot-3 sites, 0x720430/0x7207f0 wrappers, 0x257a0), 367 (generator-clean rows but menu-less; generator is menu-coupled), 381 (3 `*%r15` commands), 503 (0xf8f630 unknown). No fake indirect call, no emulation. Coverage 516/573; ctest 602/602; run_all_checkers 811 passed. |
```
