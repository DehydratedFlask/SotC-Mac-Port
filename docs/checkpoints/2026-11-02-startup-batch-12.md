# Startup batch 12 (CPU-01-STARTUP-BATCH-12), 2026-11-02

Agent: startup-ctor lane worker (Claude Sonnet 5.5). No commit, tag or push was made. HANDOFF.md, docs/TASKS.md and
docs/EVIDENCE.md were NOT edited; the paste-ready text is at the end (ids E1210-E1219; E1220-E1226 already belong to the
helper-models worker, re-check the EVIDENCE tail before pasting).

## Result

Coverage 521/573 -> **527/573** (46 remain). Cliff 358 (0x786f50) -> **370 (0x720b50)**.
Slots registered: 358, 391, 381, 561, 399, 418 (chain 367 -> 358 -> 391 -> 381 -> 561 -> 399 -> 418 in startup_ctors.cpp).
Three-try rule: no try was burned on any slot (all six passed their targeted gate on the first generation), so no escalation file.

Pins moved: coverage test `slots.size()==527`, cliff 370/0x720b50, 46 remaining; pending 481 -> 515 (+34), debug-menu roots 257 -> 278 (+21),
atexit 879 -> 934 (+55). The +34/+21/+55 deltas were cross-checked independently from the tracker rows
(358: 4/5/9, 391: 3/1/4, 381: 11/0/11, 561: 4/5/9, 399: 12/10/22, 418: 0/0/0).

## Generator extensions (tools/lift/startup_gen)

All gains are ELF-tie accounted: `page.account()` requires the call/atexit token stream of the whole body to equal the concatenation of the
per-row grammars, otherwise it is a hard AssertionError (an unknown row can no longer be silently dropped).

| row kind | ELF shape | grammar | unlocks |
|---|---|---|---|
| `slider50` (0x27050 explicit-text slider) | ctor `0x27050` (kind=2, tail jmp `0x268b0`), min/max floats from .rodata, pinned byte for byte | `SffA` | 399, 469 (469 still needs the sentinel row) |
| `cmd` (console command) | `0xf2f0` name view, `0x20af0` flags 2, register-once through `[0x1594dc0+0x20]`, handler lea tagged and hash-pinned by FDE extent, never run | `NBiffA` | 381, 391 |
| `uint` (0x3a980 unbounded int, early release) | family-admitted functor tag; late-release is a hard refusal | `W fiRfA` | 358, 399 |
| cells-only page (no variable, no menu) | 179 name-hash cells | `H` x N | 418 |

Also: extra `cmd_target_gate` (command class vptr 0x1594dd0 slot 3 = 0x22ae0 `xor eax,eax; ret`, so a toggle over a command keeps the {1,10} range;
only float/ufloat targets narrow to {0.1,0.5}) in `bool_page_lib.py`; the `0xf2f0` and handler hashes in `mk_page.py`;
cells-only guards in `genpage.py` (no `concmd` parameter name when there is no variable, no first-variable-failure block, pending check `vars == 0`);
`bump_pins.py` now maintains `DEBUG_PAGE_BATCH_12`.

Shared-file change to review: `native/core/ctor_shapes_343_356.cpp::is_known_float_arg_functor` now also admits `is_float_functor_tag()`
(the 68-tag ELF-proven float family from helper_functor_tags). Slot 381 uses tag 0x15cc838 which is in that family but not in the old private list.
The existing refusal tests still pass; all checkers that pin that file pass.

## Slots

| slot | RVA | size | content | status |
|---|---|---|---|---|
| 358 | 0x786f50 | | uint/float/bool vars, items | model + test + checker, tamper elf/native rejected |
| 391 | 0x68ebc0 | | console command rows + toggles | same |
| 381 | 0x6cd5b0 | | 11 variables incl. commands | same |
| 561 | 0x3ac00 | | 4 variables, 5 items | same |
| 399 | 0x632ca0 | 4642 | player/controls: bool, float, local-int wrappers, 4 x 0x27050 sliders, 0x26ea0 toggles, 9 cells | same |
| 418 | 0x572a10 | 4726 | 179 name-hash cells (stat counters), no menu, no variable | same, mutation self-test 1510 ELF + 569 native mutants rejected |

Files per slot: `native/core/ctor_slot_<n>_<rva>.{hpp,cpp}`, `native/tests/test_ctor_slot_<n>.cpp`, `tools/lift/check_ctor_slot_<n>.py`,
`tools/lift/startup_gen/mk_<n>.py`; wired in `native/core/startup_ctors.{hpp,cpp}` and `native/CMakeLists.txt`.
Labels: STATIC_ANALYSIS_VERIFIED (ELF byte oracle, `--tamper elf|native` exit 1) and FIXTURE_VERIFIED (native fixture test). What the variables
do at run time is NOT established. No boot, audio or save claim.

## Refusals (documented, not faked)

- **370** (0x720b50, 52949 B): 138 slot-3 sites through wrappers 0x720430/0x7207f0 and 0x257a0. Needs a genuinely new mechanism (large
  generated wrapper family) and the 0x257a0 callee, which belongs to the other worker. Left unregistered; it is now the cliff.
- **494**: allocating builder 0x26e340/0x26e770 (heap objects); needs an allocator contract. Refused.
- **508**: five unbounded floats whose initial value is loaded at run time from the mutable `.data` global 0x160491c; the generator hard-refuses
  (initial value is not a constant of the body).
- **469** (0x394a20 callee chain) and **424**: accounting fails on a trailing sentinel-container row (stores [0]=[8]=obj+0x28, [0x10]=obj+0xa8,
  [0x20]=obj+0x28 at 0x2ef6290, atexit 0x3941b0). A new row kind is required before these can be wired; not faked.
- Late-release unbounded int (451), non-empty description shapes (422, 446, 513), tracker gaps (462, 468, 524, 568, 530), register-once command
  variant (510), name-view bool (570): accounting refuses; each listed for a next batch.
- Unknown-callee slots left to the helper worker: 390, 402, 408, 427, 428, 429, 445, 452, 453, 454, 464, 473, 493, 495, 497, 498, 501, 503, 505,
  511, 532, 534, 537, 552, 554, 556, 566, 567, 571, 572. The helper_* files existing in the tree (helper_3a980, helper_a81700, helper_56b4b0,
  helper_8d650, ...) were not touched, and I did not wire slots depending on them because their checkers/models were not complete when I stopped
  (re-check the tree).

## Verification (working tree, build dir local/build/startup-b12)

| gate | command | result |
|---|---|---|
| build | `cmake -S native -B local/build/startup-b12 && cmake --build local/build/startup-b12 -j 12` | exit 0, no warnings/errors printed |
| ctest | `ctest --test-dir local/build/startup-b12 -j 12` | 100% passed, 617/617 |
| new checkers | `python3 -I tools/lift/check_ctor_slot_{358,391,381,561,399,418}.py` (+ `--tamper elf`, `--tamper native`) | exit 0 plain; both tampers exit 1 |
| pin checkers | check_ctor_slots (527 registered, digest re-derived 87981116...f0317062), check_console_var_ctors, check_fctor_slots_296_297_300 (527 rows), check_evidence_grades | all OK |
| all checkers | `SOTC_NATIVE_BUILD=local/build/startup-b12`, chunks `check_[a-m]*` and `check_[n-z]*` | 788 ok + 37 ok, 0 failed (environmental "not built in" skips not counted) |
| docs | gen_entry_map.py (+ --check OK: 2145 constants, 1465 RVAs, 490 headers), survey_startup_gaps.py --write-doc (HEAD 460/573, tree 527/573, 46 open), g1_evidence_pack.py --write/--check, check_g1_evidence_pack.py | OK |
| publish | `python3 -I scripts/audit_publish_paths.py --all` | OK (4110 paths) |
| index | `python3 scripts/project_index.py` | 84543 folders indexed; docs/FOLDERS.md regenerated |

## Risks / observations vs inference

- Observation: ELF bytes, call grammar, operand pins. Inference: page names/themes (from the strings), "player statistics" for 418.
- The shared whitelist change in ctor_shapes_343_356.cpp widens an accepted-tag set to an ELF-proven family; reviewer should confirm.
- Cells-only guard in genpage: slot 418 registers no variable, so its test cannot exercise the registration-failure path; there is none to fail.
- The working tree also contains the other worker's uncommitted helper_* files; counts above include them only through the shared build.

## Next exact action

Add a sentinel-container row kind (469, 424), then re-triage 451/422/446/513; wire unknown-callee slots once the helper worker's checkers
exist; attack 370 only with an approved wrapper-family design. Next batch starts at `python3 -I tools/lift/startup_gen/page.py <slot>`.

## Paste-ready HANDOFF / TASKS / EVIDENCE text

HANDOFF: 2026-11-02 startup-ctor lane worker, task CPU-01-STARTUP-BATCH-12: coverage 521 -> 527/573, cliff 358 -> 370 (0x720b50).
Slots 358, 391, 381, 561, 399, 418 modelled, ELF-pinned, registered. Generator gained cmd, uint, slider50 and cells-only support with hard-error
row accounting. Build exit 0, ctest 617/617, run_all_checkers 825 OK / 0 failed (chunked), publish audit OK. Status DONE for the six slots; attempts 1 each.
Refused: 370, 494, 508, 469/424 (sentinel row), unknown-callee slots. Next: sentinel row kind.

TASKS: CPU-01-STARTUP-BATCH-12 DONE (6 slots); follow-up CPU-01-STARTUP-SENTINEL-ROW (469, 424) OPEN; 370, 494, 508 BLOCKED (documented).

EVIDENCE:
| E1210-E1215 | 2026-11-02 | Startup batch 12: slots 358, 391, 381, 561, 399, 418 modelled (STATIC_ANALYSIS_VERIFIED ELF-tie checkers, exit 0 plain, 1 for `--tamper elf|native`; FIXTURE_VERIFIED tests), registered. |
| E1216 | 2026-11-02 | Generator row kinds cmd (`NBiffA`), uint (`W fiRfA`), slider50 (`SffA`), cells-only page; `page.account()` hard-errors on any unaccounted call token. |
| E1217 | 2026-11-02 | Refusals: 370 (138 wrapper sites), 494 (allocating builder), 508 (.data-loaded initial values), 469/424 (sentinel container row). |
| E1218 | 2026-11-02 | Batch 12 totals: coverage 527/573, 46 remain, cliff 358 -> 370 (0x720b50); pending 515, menu roots 278, atexit 934; ctest 617/617; checkers 788+37 OK, 0 failed. |
