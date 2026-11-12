# Startup batch 11 (worker CPU-01-STARTUP-BATCH-11), 2026-11-02

Labels: STATIC_ANALYSIS_VERIFIED + FIXTURE_VERIFIED. Nothing executed an original RVA; no boot, audio or save claim.
Build dir `local/build/startup-b11` (owned by this worker). Not committed (workers do not commit). HANDOFF/TASKS/EVIDENCE not edited (paste text below; EVIDENCE ids E1200+, the last id in use was E1194).

## Result

Coverage 516 -> 521/573 (52 open). The batch-10 cliff **352 (0x7a2390) is moved**. New first unrecovered slot: **358 (0x786f50)**.
Five slots modelled, tested, ELF-tied with tamper probes and registered (of a maximum of 10). Zero escalation tries consumed; no `docs/escalations/` file.

| Slot | RVA / bytes | Shape | Checker (pins; ELF + native mutants) |
|---|---|---|---|
| 352 | 0x7a2390 / 2290 | menu-less: 6 bounded floats (tag 0x15ce1e0), a ranged int (0xc580), an unbounded float (0x6aa00) | check_ctor_slot_352 (170; 440 + 64) |
| 401 | 0x621530 / 3799 | 10 bool vars, 2 local-int wrapper sites (0x620e10 family), 11 `0x26db0` toggles | check_ctor_slot_401 (315; 892 + 83) |
| 398 | 0x63cf60 / 1408 | 2 bool vars, 2 local-int wrapper sites (0x529160 family, class-alias tags), 2 toggles + 2 `0x26ea0` toggles | check_ctor_slot_398 (118; 423 + 56) |
| 363 | 0x751c70 / 701 | menu-less: 3 local-int wrapper sites (0x751190 / 0x751550 / 0x751910) | check_ctor_slot_363 (52; 379 + 37) |
| 367 | 0x740980 / 4701 | menu-less: 6 bool, 12 unbounded floats, 1 bounded float (tag 0x15cd1e0) | check_ctor_slot_367 (369; 851 + 97) |
| helper | 0x620e10 family | caller-twin admission added to the existing helper checker | check_helper_620e10 (18; 2333 + 70) |

Each slot checker: plain 0, `--tamper elf` 1, `--tamper native` 1 (re-run at close for 352/401/398/363/367: all `0 1 1`).

## Per-site tag proof for the local-int wrappers (the 401 / 398 / 363 gate)

The wrapper (shape of 0x7ec160) builds a local functor of its OWN class (lea at +0x5a) and calls the caller's functor class through slot 1 (clone), the copy
slots and `call [tag+0x18]` (destroy). The final object's `fn_vptr` is always the wrapper's own class. A caller tag is therefore admissible only if it is
(a) the wrapper's own class, or (b) a **class-table alias**: a different table whose four slot targets equal the aliased wrapper class's.

- Slots 398 passes `0x15cc508` and `0x15cc4a8` to wrapper 0x529160 (own class `0x15c8268`, slots `0x529100/120/140/150`). Both alias that class
  (all four slot targets equal), so they are recorded as `(tag, aliased_class)` pairs in `kLocalIntCallerTwins` (`helper_620e10_local_int_var.hpp`).
- `is_local_int_caller_twin_tag(tag, own_class)` admits only the exact pair; an alias of another wrapper's class is refused (`unadmitted_arg_functor`,
  covered by a native negative test). The earlier idea "twin of the int-body storing its own class" was rejected: those bodies sit at 0x529100.. and store
  a different class.
- `tools/lift/local_int_site_lib.py` (the `lint_sites` extra) re-proves every site from the ELF: caller calls the wrapper, the wrapper is a `same_outside`
  twin of 0x7ec160 with an inner twin of 0xc580, own-class lea equals the expected class (or aliases all four slots), final-class slot 0 equals the atexit
  dtor, object/tag/dtor leas match, the f730 call, the `ff5018` destroy followed by the f590 release, the `__cxa_atexit` call, slot3/slot6 are
  `mov eax,1; ret`, functor bodies are int-body twins.
- Slot 401: the admission result (own class) is checked per site, not assumed.

## Refusals and deferrals (with evidence; none of these is a failed "try")

- **399** (0x632ca0): generated, then unwired. Needs a `0x27050` slider row kind that the generator lacks; the native model exists but the generator work is
  not done. Open candidate. (A first wiring used a guessed RVA 0x633000; fixed by deriving the RVA from the ELF in `run_slot.sh`, then removed entirely.)
- **497, 513**: unknown callees (0xa81700, 0x4b560, 0x26800, 0x26b60, 0x257a0, 0x8d650, 0x3a980). 513 (and 422) also fail the generator assertion that the
  description argument is not the empty static string.
- **446**: zero rows recognised; unknown calls 0x3be2d0, 0x4c92a0, 0x4c8f40.
- **422**: unknown 0x56b980, 0x56b4b0, 0x274730, plus the description-assertion failure.
- **381**: command rows (`*%r15`) and 0x52a990 hash cells. **391**: command rows and 0x68ebc0.
- **561**: 0x3ac00 + 0x3a980. **508**: 0x3a980 and untracked unbounded-float values. **501**: 0x3a980, 0x24db80, 0x26800, 0x257a0.
  **469**: four 0x27050 sliders + 0x394a20.
- **503**: `0xf8f630` is an unknown call. **370**: 52949 B, not mechanical. **494**: allocating builder.
- No indirect call was faked, no stub, no emulation.

## Tooling note

The slot generator is now repo-resident in `tools/lift/startup_gen/` (static register tracker over `objdump`, no execution): `rows.py, track.py, page.py,
genpage.py, genpins.py, mkchecker.py, mk_page.py, wire.py, bump_pins.py, fixdigest.py, newmk.py, run_slot.sh, mk_<slot>.py` (352/401/398/363/367).
New this batch: menu-less mode (352, 363, 367), the `lint` row kind with per-site twin tags, empty-`pad` boundary pins (last body byte + next 8 bytes, which
otherwise let boundary mutations survive). Open generator gaps: `0x27050` slider row kind, command rows (`*%r15`), string var `0x257a0`.
`check_ctor_slots.py` digest refresh: `SOTC_NATIVE_BUILD=local/build/startup-b11 python3 tools/lift/startup_gen/fixdigest.py`.
`helper_620e10_local_int_var.{hpp,cpp}`, `test_helper_620e10.cpp` and `check_helper_620e10.py` are untracked files created by the helper-callee worker; this
batch edited them (caller-twin admission), so the integrator must stage them together.

## Files

New: `native/core/ctor_slot_{352_7a2390,401_621530,398_63cf60,363_751c70,367_740980}.{hpp,cpp}`, `native/tests/test_ctor_slot_{352,401,398,363,367}.cpp`,
`tools/lift/check_ctor_slot_{352,401,398,363,367}.py`, `tools/lift/local_int_site_lib.py`, `tools/lift/startup_gen/`.
Edited: `helper_620e10_local_int_var.{hpp,cpp}`, `test_helper_620e10.cpp`, `check_helper_620e10.py`, `ctor_shapes_343_356.{hpp,cpp}` (tags 0x15ce1e0 and
0x15cd1e0 added to `kBareRetDestroyFunctors`), `startup_ctors.{hpp,cpp}`, `native/CMakeLists.txt`; pins: `test_startup_ctor_coverage.cpp` (521 / first 358 /
0x786f50 / +52; pending 481, menu roots 257, atexit 879), `test_startup_ctors.cpp` `expect[]`, `check_ctor_slots.py` (52 unregistered, first 358, refreshed
digest), `check_console_var_ctors.py`, `check_fctor_slots_296_297_300.py` (521 rows).
Regenerated: `docs/entry_map.json` (2119 constants -> 1439 RVAs in 480 headers), `docs/G1_EVIDENCE.md`, `docs/STARTUP_GAP_SURVEY.md` (HEAD 460/573, working tree
521/573, 52 open), `docs/FOLDERS.md` (81878 folders). Many of these files are also modified by concurrent workers; regenerated docs reflect the shared tree.

## Commands and results (workspace root)

- `cmake -S native -B local/build/startup-b11 && cmake --build local/build/startup-b11 -j 12` (log `local/build/b11final.log`): exit 0, 0 warnings.
- `ctest --test-dir local/build/startup-b11 -j 12 --output-on-failure`: 607/607 passed.
- `SOTC_NATIVE_BUILD=local/build/startup-b11 tools/lift/run_all_checkers.sh` (log `local/build/b11checkers.log`): 816 passed, 0 skipped, 0 failed.
- Per-slot checkers 352/401/398/363/367 and `check_helper_620e10.py`: OK, plain 0 / `--tamper elf` 1 / `--tamper native` 1.
- `gen_entry_map.py`, `g1_evidence_pack.py --write`, `check_g1_evidence_pack.py`, `survey_startup_gaps.py --write-doc`, `check_evidence_grades.py`: OK.
- `python3 -I scripts/audit_publish_paths.py --all`: exit 0 (4110 paths). `python3 scripts/project_index.py`: written.

## Remaining risks

- Caller-twin admission relies on class-table slot equality of exactly two tags; any further wrapper caller needs its own per-site proof (the checker enforces it).
- Menu-less pins (`menu.root.count`) assume 352/363/367 add no menu root; the pending/menu/atexit totals are pinned (481 / 257 / 879) and would fail on drift.
- Atexit destructors are recorded, never run; command-handler/functor bodies are identity tags (hash-pinned), never executed.
- The generator is a heuristic row tracker: it silently drops unknown rows, so the row-accounting assertion and the per-slot checkers are the safety net.

## Paste-ready text

### HANDOFF.md
```
CPU-01-STARTUP-BATCH-11 (2026-11-02, worker): coverage 516 -> 521/573. Slots 352, 401, 398, 363, 367 modelled, tested, ELF-pinned (tamper elf/native probes), registered.
Wrapper callers 401/398/363 carry a per-site tag proof: caller tag = wrapper's own class or a class-table alias (all four slot targets equal); kLocalIntCallerTwins holds
{0x15cc4a8,0x15c8268},{0x15cc508,0x15c8268}. Menu-less generator mode (352/363/367) landed; generator now repo-resident in tools/lift/startup_gen/.
New first unrecovered slot 358 (0x786f50). Refused/deferred: 399 (needs 0x27050 slider row kind), 497/513/446/422/381/391/561/508/501/469/503 (unknown callees or command rows), 370, 494.
Gates: build exit 0 / 0 warnings; ctest 607/607; run_all_checkers 816 passed / 0 failed; audit --all OK (4110 paths). STATIC_ANALYSIS_VERIFIED + FIXTURE_VERIFIED; no boot/audio/save claim.
Pins: pending 481, debug_menu.root.count 257, atexit_log 879. Next exact action: slot 358 (0x786f50) survey; add 0x27050 slider row kind (399, 469) and command row kind (381, 391, 503).
Attempts: 0 consumed.
```

### docs/TASKS.md
```
CPU-01 startup ctors: 521/573 (52 open), cliff 358 (0x786f50). Batch 11 DONE for 352/401/398/363/367. Open: 399 (0x27050 slider kind), 381/391/503 (command rows), 497/513/446/422/561/508/501/469 (unknown callees), 370 (52949 B), 494 (allocating builder).
```

### docs/EVIDENCE.md
```
| E1200 | 2026-11-02 | Slot 352 (0x7a2390, 2290 B, menu-less): 6 bounded floats (tag 0x15ce1e0 added to kBareRetDestroyFunctors), a ranged int (0xc580), an unbounded float (0x6aa00). check_ctor_slot_352 170 pins, 440 ELF + 64 native mutants rejected. STATIC_ANALYSIS_VERIFIED / FIXTURE_VERIFIED. Record: docs/checkpoints/2026-11-02-startup-batch-11.md. |
| E1201 | 2026-11-02 | Slot 401 (0x621530, 3799 B): 10 bool vars, 2 local-int wrapper sites (0x620e10 family, per-site tag = own class), 11 `0x26db0` toggles. 315 pins; 892 + 83 mutants rejected. helper_620e10 checker 18 pins, 2333 + 70 mutants. |
| E1202 | 2026-11-02 | Slot 398 (0x63cf60, 1408 B): 2 bool vars, 2 wrapper sites into 0x529160 (class 0x15c8268) passing tags 0x15cc508 / 0x15cc4a8, proven class-table aliases (all four slot targets 0x529100/120/140/150 equal). `kLocalIntCallerTwins` admits exactly those pairs; other aliases refused (`unadmitted_arg_functor`). 118 pins; 423 + 56 mutants. |
| E1203 | 2026-11-02 | Slots 363 (0x751c70, 701 B: 3 wrapper sites 0x751190/0x751550/0x751910, menu-less; 52 pins, 379 + 37 mutants) and 367 (0x740980, 4701 B: 6 bool, 12 unbounded + 1 bounded float tag 0x15cd1e0, menu-less; 369 pins, 851 + 97 mutants). |
| E1204 | 2026-11-02 | Refusals: 399 (needs 0x27050 slider kind), 497/513 (0xa81700, 0x4b560, 0x26800, 0x26b60, 0x257a0, 0x8d650, 0x3a980), 446 (0x3be2d0, 0x4c92a0, 0x4c8f40), 422 (0x56b980, 0x56b4b0, 0x274730), 381/391 (command rows; 0x52a990, 0x68ebc0), 561/508/501/469, 503 (0xf8f630), 370 (52949 B), 494 (allocating builder). No fake call, no emulation. Coverage 521/573; ctest 607/607; run_all_checkers 816 passed / 0 failed. |
```
