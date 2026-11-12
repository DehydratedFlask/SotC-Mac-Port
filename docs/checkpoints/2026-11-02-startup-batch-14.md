# Startup batch 14 (CPU-01-STARTUP-BATCH-14), 2026-11-02

Agent: startup-ctor lane worker (Claude Sonnet 5.5). No commit, tag or push was made. HANDOFF.md, docs/TASKS.md and
docs/EVIDENCE.md were NOT edited; the paste-ready text is at the end. The EVIDENCE tail was E1235 (batch 13) when this was
written, so ids E1240-E1245 are used; re-check the tail before pasting.

## Result

Coverage 531/573 -> **537/573** (36 remain). Cliff 390 (0x691c50) -> **408 (0x5c7f50)**; slot 408 starts with an unknown
direct call 0x1580a0 (not modelled, not guessed).
Slots registered (6 of the allowed 10): 390, 402, 571, 572, 453, 464. All with model, fixture test, ELF-tie checker and
mutation probes (`--tamper elf` and `--tamper native` rejected for every one).
Three-try rule: no registered slot burned a try that counts (the generator/test-template fixes for 453 and 464 were the
first attempts at fixing generator output, not failed hypotheses on a slot). No slot reached three failed hypotheses, so no
docs/escalations file is needed.

Pins moved: `slots.size() == 537`, 36 unregistered, cliff 408/0x5c7f50, pending 720 -> 779, debug-menu roots 346,
atexit log 1279. The registered index-set digest in `check_ctor_slots.py` was re-derived with `startup_gen/fixdigest.py`
(sha256 4dde86a8...49a08). No `DEBUG_PAGE_BATCH_14` set was added there; the batch-13 set is unchanged.

## Slots

| slot | RVA | size | content | checker |
|---|---|---|---|---|
| 390 | 0x691c50 | 1,035 B | empty tree header (host allocator input), node reference record, three name holders, two showLegacyParticle bools | 119 pins; 336 ELF + 63 native mutants rejected |
| 402 | 0x610430 | 2,663 B | CP10 animation debug variables incl. the inline-built CP10_AnimMode ranged enum, string var, menu rows | 262 pins; 708 + 102 |
| 571 | 0xc780 | 10,644 B | 26 bools, 13 bounded floats, 4 commands, one 0x26f90 command item | 859 pins; 1920 + 189 |
| 572 | 0x1a20 | 1,668 B | 5 commands, 2 `0x26ea0` toggles, one 0x26f90 command item | 127 pins; 337 + 73 |
| 453 | 0x48ac10 | 856 B | 11 named records (0xa81700) and the group destructor 0x48a250 (identity tag) | 71 pins; 198 + 53 |
| 464 | 0x3db070 | 697 B | one bool, 5 named records, group destructor 0x3d9b40 (identity tag) | 69 pins; 222 + 55 |

Each has `native/core/ctor_slot_<n>_<rva>.{hpp,cpp}`, `native/tests/test_ctor_slot_<n>.cpp`, `tools/lift/check_ctor_slot_<n>.py`
and `tools/lift/startup_gen/mk_<n>.py`. 571 and 572 take `external_target_0 = &s.global_2dfcbd0_standin` (the opaque global
0x2dfcbd0 is the target of every 0x26f90 item; the model fails closed with `ok == false` when the host supplies null). 453 and
464 use the shared process registry cell `s.record_registry` and the host malloc stand-in `default_slot<n>_record_alloc()` (as 513).

## Generator and model extensions (tools/lift/startup_gen, native/core)

| change | where | used by |
|---|---|---|
| `cmdi` row: 0x26f90 command item (`xfffA`), `family_command_item_26f90` | page.py, genpage.py, mk_page.py, ctor_slot_family_items.{hpp,cpp} | 571, 572 |
| tree node size generalised (0x30/0x38/0x48/0x50) | page.py, genpage.py | 390 |
| records-group destructor generalised: any of 0x1c8990, 0x48a250, 0x3d9b40; page.py re-verifies the unregister-last-first shape of the actual dtor body, genpage emits `RecsDtorRva` from the row, mk_page.py hashes that dtor | page.py, genpage.py, mk_page.py | 453, 464 |
| test template fix: menu-less pages with records no longer leak the registry set-up into the non-menu path (`@@RECSETM@@`), and the cells-only failure block now sets the registry first | genpage.py | 453, 464 |
| `wire.py` handles "no later index" (appends after the highest lower row) | wire.py | 572 |
| late-release bool (`BRffA`), late-release unbounded uint/int (`iRffA`), `early_description_release` flag in `init_bool_var_static`, `init_bool_var_literal`, `HelperIntSiteSpec` | page.py, genpage.py, concmd_vars, lane_m6_shapes, helper_int_var_sites | **no registered slot yet** (see risks) |
| `check_fctor_slot_303_a82170.py` token for the `init_bool_var_static` declaration updated for the new trailing parameter | tools/lift | fixes a regression the flag introduced |

`page.account()` still hard-asserts that the call/atexit token stream equals the concatenation of the row grammars.

## Refusals (documented, not stubbed)

The answers to the ordered task list. All static triage from the register tracker plus ELF disassembly; no native code was
written for a refused slot.

| slot | verdict | evidence |
|---|---|---|
| 446 | refused | late bool is now implemented, but the page also needs `NN<3be2d0>ffA` command rows (token 43): an unknown callee 0x3be2d0 between the `N` and `ff`. Not modelled. |
| 451 | refused | `rows_of` fails at token 52: `UiRffAWiRffA<49ca60>iRffA...`. The late uint/bool grammar now parses, but a further unknown call 0x49ca60 sits between rows. |
| 468, 524, 568, 510 | refused | inline-functor commands: `0x20af0` followed by an inline vptr store, a ymm functor copy into obj+0x60, `0x20540` and a local callee. The functor body is not pinned to a single hashable callee that can be modelled without executing it; unchanged from batch 13. |
| 422, 427, 473, 493 | refused | unknown direct call 0x274730 (661-B bool-var constructor with functor-tag handlers) between rows. Own helper + checker not written in this batch. |
| 462 | refused | `toge` text view still unresolved (`TypeError`, view is None). |
| 530 | refused | `KeyError 'f_r8'`: float argument register unresolved. |
| 570 | refused | mixed new kinds (leading `AAA` atexits, `N` rows). |
| 497, 501 | refused | unmodelled helpers beyond helper_a81700/8d650. |
| 424 | refused | ~26 KB inline static-table init, atexit 0x54dbb0 object not resolved; not exactly representable. |
| 454 | refused | tree node allocation size 424 (0x1a8) not in the supported set. |
| 408 (cliff) | refused | leading unknown call 0x1580a0. |
| 429, 452 | refused | zero-cell / zero-container generalisation needed (429: `gffAgffAeffAAAAA...` vs rows missing five atexits; 452 starts with `MAA`, a per-row dtor and tree dtor 0x497610). |
| 428, 445, 494, 505, 532, 537, 552, 556, 567 | refused | row accounting fails at token 0 or early on a new shape (`<a2ddc0>`, `<26e340>`, `<162970>`, `<157e90>`, `<475c0>`, `<20d80>`, `<4b560>` unknown callees, or an `NA` run). |
| 495, 498, 503, 511, 534, 554, 566 | refused | unknown trailing callee (`<a38960>`, `<25ab0>`, `<f8f630>` guard), two trees, or inline enum variants. |

## Verification (build dir local/build/startup-b14)

| command | result |
|---|---|
| `cmake --build local/build/startup-b14 -j 12` | exit 0, 0 warnings/errors |
| `ctest --test-dir local/build/startup-b14 -j 12 --output-on-failure` | 627/627 passed |
| `check_ctor_slot_{390,402,571,572,453,464}.py` (+ `--tamper elf|native`) | all OK, tamper probes rejected |
| `SOTC_NATIVE_BUILD=local/build/startup-b14 tools/lift/run_all_checkers.sh` | 836 passed, 0 skipped, 0 failed (single run; chunking not needed) |
| `python3 -I tools/lift/check_ctor_slots.py` | OK, 537 registered RVA/index pairs |
| `gen_entry_map.py` | 2176 constants -> 1493 distinct guest RVAs in 500 headers (364 duplicates) |
| `g1_evidence_pack.py --write`, `check_g1_evidence_pack.py` | OK |
| `survey_startup_gaps.py --write-doc` | working tree 537/573, 36 open |
| `python3 -I scripts/audit_publish_paths.py --all` | PUBLISH PATH AUDIT OK (4110 paths) |
| `python3 scripts/project_index.py` | 87253 folders indexed |

Docs regenerated: docs/entry_map.json, docs/G1_EVIDENCE.md, docs/STARTUP_GAP_SURVEY.md, docs/FOLDERS.md.
All results are STATIC_ANALYSIS_VERIFIED / FIXTURE_VERIFIED. No guest code was executed.

## Risks and caveats

- Late-release bool / uint code paths (`early_description_release = false`) are compiled and parse at the `rows_of` level, but no registered slot exercises them; they have no native fixture and no mutation probe yet. They must get a slot (451/446 once 0x49ca60 / 0x3be2d0 are resolved) before being relied on.
- The registry cell (453, 464, 513) and the 0x26f90 target (571, 572) are host inputs/stand-ins, not models of the guest allocator or of the opaque global 0x2dfcbd0; fixtures leak intentionally. The consumers of the console variables and the command items are NOT established (readings are inference from the strings).
- The atexit-log pin moved by +3 for 453/464 only; the registry's first-use record 0xa81b80 is recorded by whichever records slot runs first, so the total is order-independent but the per-slot attribution is not.
- `HEAD 460/573` in the survey is the committed state; the 77 wired-uncommitted slots belong to the integrator.
- No boot, audio or save claim is made.

## Next exact action

Cliff slot 408: model or identify the unknown call 0x1580a0. Next most valuable: 0x274730 (unlocks 422/427/473/493, four slots), then 0x3be2d0 and 0x49ca60 to give the late bool/uint code a real slot (446, 451).

## Paste-ready text

HANDOFF checkpoint: 2026-11-02, startup-ctor lane (Claude Sonnet 5.5), task CPU-01-STARTUP-BATCH-14. Files: native/core/ctor_slot_{390,402,571,572,453,464}_*,
native/tests/test_ctor_slot_*.cpp, tools/lift/check_ctor_slot_*.py, tools/lift/startup_gen/*, startup_ctors.{hpp,cpp}, concmd_vars, helper_int_var_sites, lane_m6_shapes,
count pins. Commands as in the Verification table. Coverage 537/573 (STATIC_ANALYSIS_VERIFIED + FIXTURE_VERIFIED), cliff 408 (0x5c7f50, unknown call 0x1580a0); pins pending 779,
menu roots 346, atexit 1279. Refused with documentation: 446, 451, 468, 524, 568, 510, 422, 427, 473, 493, 462, 530, 570, 497, 501, 424, 454, and the rest in the table.
Attempts: 1 per registered slot, no escalation. Next: 0x274730, then 0x3be2d0 / 0x49ca60.

TASKS: CPU-01-STARTUP-BATCH-14 done (537/573); remaining 36, next cliff slot 408.

EVIDENCE rows:
| E1240 | 2026-11-02 | Startup batch 14: slot 390 (tree header, node ref, name holders, two bools) wired, 119 pins, 336 ELF + 63 native mutants rejected (STATIC_ANALYSIS_VERIFIED + FIXTURE_VERIFIED) |
| E1241 | 2026-11-02 | Slot 402 (CP10 animation variables, inline enum, string var, menu rows) wired, 262 pins, 708 + 102 mutants rejected |
| E1242 | 2026-11-02 | Slots 571 and 572: new `cmdi` row (0x26f90 command item, opaque target 0x2dfcbd0 supplied by the host, fails closed on null); 859 and 127 pins |
| E1243 | 2026-11-02 | Slots 453 and 464: records-group destructor generalised (0x48a250, 0x3d9b40, shape re-verified from the ELF body); 71 and 69 pins |
| E1244 | 2026-11-02 | Batch 14 refusals: 446/451 (unknown 0x3be2d0 / 0x49ca60), 468/524/568/510 (inline functors), 422/427/473/493 (0x274730), 462, 530, 570, 497/501, 424, 454, 408 (0x1580a0), others per checkpoint; static triage, no native code |
| E1245 | 2026-11-02 | Batch 14 totals: coverage 537/573, 36 remain, cliff 390 -> 408 (0x5c7f50); pending 779, menu roots 346, atexit 1279; ctest 627/627, run_all_checkers 836 passed, publish audit OK |
