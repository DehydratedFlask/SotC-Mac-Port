# Startup batch 7 (worker CPU-01-STARTUP-BATCH-7), 2026-11-02

Labels: STATIC_ANALYSIS_VERIFIED + FIXTURE_VERIFIED. Nothing executed an original RVA; no boot, audio or save claim.
Build dir `local/build/startup-b7` (owned by this worker). Not committed (workers do not commit).

## Result

Coverage 492 -> 497/573 (76 open). Cliff unchanged: first unrecovered slot 337 (0x7e81d0). Six slots in this task
(336 first, then 506, 392, 396, 409, 414); 336 and 506 are in the 492 baseline of the previous report.

| Slot | RVA / bytes | Shape | New helper (own ELF-tie checker) |
|---|---|---|---|
| 336 | 0x7ec4c0 | attract-mode page (enum var, bool, ranged int, rand cell, group + 3 toggles, holder, unbounded float, cell) | 0x7ec160 (`check_helper_7ec160.py`) |
| 506 | Touch Manager page | 2 vars, group, Draw toggle | 0x233ec0 (`check_helper_233ec0.py`) |
| 392 | 0x681690 / 1612 | 4 bounded float vars (tag 0x15cc618) + 4 `0x27120` sliders, caption `Shadow AI` | 0x27120 (`check_helper_27120.py`, 210 B) |
| 396 | 0x650f20 / 2845 | 6 bool vars (tag 0x15cc548) with `0x26db0` toggles (one `0x26ea0`: `Enable blend root fitting`), 2 bounded float vars (tag 0x15cc568) + `0x27120` sliders, parent `CP10-Anim` | none new |
| 409 | 0x5b50f0 / 89 | sentinel-node cell (alloc 0x40, host malloc stand-in) + atexit 0x5b3d10 | already tied by `check_ctor_tiny_misc_slots.py`; only wired |
| 414 | 0x57b9b0 / 72 | native CritSec handle (`core/critsec_3b310`) + atexit 0x3b380 (recorded, never run) | already tied by `check_ctor_tiny_misc_slots.py`; only wired |

New files: `native/core/ctor_slot_{336,506,392,396}_*.{hpp,cpp}`, helper modules, `native/tests/test_ctor_slot_{336,506,392,396}.cpp`,
`tools/lift/check_ctor_slot_{336,506,392,396}.py`, `check_helper_{7ec160,233ec0,27120}.py`. Edited: `debug_menu_item.*`
(`init_debug_menu_item_27120`), `ctor_shapes_343_356.*` (float functor tags 0x15cc618 / 0x15cc568 admitted), `startup_ctors.*`
(members, rows), `native/CMakeLists.txt`, pins (coverage test, `expect[]` in test_startup_ctors, check_ctor_slots incl. index digest,
check_console_var_ctors, check_fctor_slots_296_297_300). Regenerated: docs/STARTUP_GAP_SURVEY.md, docs/entry_map.json (2048 constants,
1372 RVAs, 455 headers), docs/G1_EVIDENCE.md, docs/FOLDERS.md.

Measured fixture counts after the batch: pending 264, menu roots 169, atexit 559 (the 0x27280 root record already exists in the full run).

## Mutation self-tests (ELF + native mutants, all rejected; `--tamper`, `--tamper elf`, `--tamper native` each exit 1)

336: 542+80; 506: 253+33; 392: 317+38; 396: 579+58 (236 operand pins); h7ec160: 178+35; h233ec0: 141+30; h27120: 32+9 (7 operand pins).

## Refused / not taken (need genuinely new mechanisms)

- 494 (0x26f7c0): needs `0x26e340` / `0x26e770` (allocating vector builders over `0x26e5f0` / `0x26ea10`, canary-guarded) and the vector destructors
  `0x269590` / `0x269610`; four new bodies plus allocator contracts. Exists only as hooks in `ctor_tiny_misc_slots`; left unwired.
- 464 / 454 (unmodelled local helper plus bodies of that size), 342 (indirect call), 351 (needs 0x257a0-class helper plus 7 slot-3 sites),
  380 / 402 (blocked by 0x257a0), 566 (0x25ab0 twin), 390 / 495 / 398 / 493 (unanalysed helpers). Not attempted; no failed tries on any task (0 of 3).

## Commands and results

| Command | Result |
|---|---|
| `cmake -S native -B local/build/startup-b7`; `cmake --build local/build/startup-b7 -j 12` | exit 0, 0 warnings |
| `ctest --test-dir local/build/startup-b7 -j 12 --output-on-failure` | 581/581 passed |
| new checkers (all above) | exit 0 each |
| `check_ctor_slots`, `check_console_var_ctors`, `check_fctor_slots_296_297_300`, `check_evidence_grades`, `check_ctor_tiny_misc_slots`, `check_critsec_3b310_3b380`, `gen_entry_map.py --check`, `g1_evidence_pack.py --check` | OK |
| `SOTC_NATIVE_BUILD=local/build/startup-b7 tools/lift/run_all_checkers.sh` | 790 passed, 0 skipped, 0 failed (exit 0) |
| `python3 -I scripts/audit_publish_paths.py --all` | OK (4110 paths) |
| `python3 scripts/project_index.py` | written (75376 folders) |

## Risks

- Stack-guard compares are hooks, as in every earlier slot. All `0xf590` releases are exact only because views are borrowed.
- 409's sentinel node is a host malloc stand-in and is never freed by the startup state; 414's CritSec sidecar lives in a process-wide registry
  until the opt-in teardown runs (not run in the startup path).
- Name readings (shadow hack scale, root fitting, Touch Manager, attract mode) are inference from strings; no variable consumer is lifted.
- Bool class answers slot3=1 / slot6=0, so every 0x26ea0/0x26db0 range stays {1.0, 10.0}.
- Other workers add concurrent slots (504/512/514/518/550/555 appeared untracked mid-session); counts were re-measured, not assumed.

## Paste-ready text

HANDOFF: "2026-11-02 worker startup batch 7: coverage 492->497/573 (slots 336, 506, 392, 396, 409, 414 wired; new native helpers
0x7ec160, 0x233ec0, 0x27120, each with an ELF-tie checker and --tamper probes; 409/414 were already modelled in
ctor_tiny_misc_slots and are now wired with host stand-ins). Pending 264, menu roots 169, atexit 559. Cliff unchanged at 337
(0x7e81d0), 76 remain. 581/581 ctest, count pins and docs regenerated. STATIC_ANALYSIS_VERIFIED + FIXTURE_VERIFIED; no
boot/audio/save claim. Not committed. Refused: 494 (four new vector-builder bodies). Next: re-run survey_startup_gaps.py."

TASKS: "CPU-01-AOT-CFG-SCALE-01: startup coverage 497/573 (batch 7 done; 76 open, first 337). Status in progress, attempts 0 failed."

EVIDENCE (re-check the tail first; last at write time was E1144; use E1150+):
E1150 slot 336 + helper 0x7ec160; E1151 slot 506 + helper 0x233ec0; E1152 helper 0x27120 (slider from variable name view);
E1153 slot 392; E1154 slot 396; E1155 slots 409 and 414 wired (host malloc / CritSec stand-ins); E1156 batch 7 totals
(coverage 497/573, pending 264, roots 169, atexit 559, 581/581 ctest). Each: `STATIC_ANALYSIS_VERIFIED + FIXTURE_VERIFIED`,
`tools/lift/check_ctor_slot_N.py` / `check_helper_*.py` (mutants as in the table above).
