# Startup batch 15 (CPU-01-STARTUP-BATCH-15), 2026-11-02

Agent: startup-ctor lane worker (Claude Sonnet 5.5). No commit, tag or push was made. HANDOFF.md, docs/TASKS.md and
docs/EVIDENCE.md were NOT edited; paste-ready text is at the end. The EVIDENCE tail was E1257 when this was written, so ids
E1260-E1264 are used; re-check the tail before pasting.

## Result

Coverage 537/573 -> **541/573** (32 remain). Cliff stays **408 (0x5c7f50)** (starts with the unmodelled direct call 0x1580a0;
not modelled, not guessed). Slots registered (4 of the allowed 12): **493, 422, 511, 503**. Each has a model, a fixture test, an
ELF-tie checker and mutation probes (`--tamper elf` and `--tamper native` rejected for every one). The remaining eight attempted
slots were refused (table below) because each needs a genuinely new mechanism, not a small generator extension.

Three-try rule: no slot burned a counted try; generator/test-template fixes (see "Fixes during the batch") were first attempts at
fixing generator output. No docs/escalations file is needed.

Pins moved: `slots.size() == 541`, 32 unregistered, cliff 408/0x5c7f50, concmd pending 779 -> 810, debug-menu root count
346 -> 354, atexit log 1279 -> 1322 (all measured by test_startup_ctor_coverage). The registered index-set digest in
`check_ctor_slots.py` is now c10f5af9...afc2c. New set `DEBUG_PAGE_BATCH_15 = {422, 493, 503, 511}`.

## Slots

| slot | RVA | size | content | checker |
|---|---|---|---|---|
| 493 | 0x277560 | 1,088 B | bool variables (0x20af0 inline and 0x274730 out-of-line functor) and toggle items | 115 pins; 313 ELF + 59 native mutants rejected |
| 422 | 0x56b980 | 2,682 B | bool, local-int and out-of-line bool variables, items, a node-ref row, a zero pointer slot | 222 pins; 785 + 82 |
| 511 | 0x1d90c0 | 1,550 B | four unbounded int variables (0x140b00/0x25ab0 twin), four toggles, two empty RB-tree headers (dtor 0x1d6210) | 142 pins; 372 + 60 |
| 503 | 0x247f00 | 3,185 B | null-name record (strncpy 0xf8f630), UI override variables, console commands, one toggle | 253 pins; 683 + 119 |

Each has `native/core/ctor_slot_<n>_<rva>.{hpp,cpp}`, `native/tests/test_ctor_slot_<n>.cpp`,
`tools/lift/check_ctor_slot_<n>.py` and `tools/lift/startup_gen/mk_<n>.py`. `tools/lift/run_all_checkers.sh` globs
`tools/lift/check_*.py`, so no registration beyond the new files was needed.

## Generator and model extensions (tools/lift/startup_gen)

| change | where | used by |
|---|---|---|
| bool/boolx (0x274730 functor) and zq row kinds with per-site functor tag pins | page.py, genpage.py, mk_page.py | 493, 422 |
| twin uint rows: `D` (0x140b00) and `G` (0x25ab0), same suffix grammar as the 0x6aa00/0x3a980 rows, via `helper_unbounded_int_twin_140b00` | page.py, track.py (`e_['tag']`), genpage.py | 511 |
| multi-tree: one tree dtor per slot (subset of {0x6918e0, 0x1d6210}); test allocator tracks `g_tree_blocks[8]` / `is_tree_block()`; node size 424 allowed; register-based tree headers; tree pins depend on the dtor | page.py, genpage.py, mk_page.py | 511 |
| ncell row `P` (0xf8f630 strncpy; no atexit record; test letter `o` skipped in the atexit loop); `drop_strings` for the source address in the checker | page.py, genpage.py, mk_page.py | 503 |
| tracker fixes: covering-store reads, vq xmm tracking, `ebx` snapshot | track.py | all |

## Refusals (not wired; nothing faked)

| slot | reason |
|---|---|
| 408 | 0x1580a0 direct call, POD stores, extra holder classes, variant container dtors: a new mechanism |
| 427, 473 | separator 0x26b60 rows with single-record layout and unknown holder dtor |
| 498 | getters/records (slot-498 helpers) plus 0x1f220 |
| 451 | 0x49ca60/0x49cc60 and late-release rows; not exercised by a small extension |
| 454 | several tree dtors and sizes, recs dtor 0x482920, `A` row with 0xf7b0 |
| 534 | unengaged inline enum, three twin rows, trailing rows |
| 566 | external value cell, different vptr and dtor |
| 497, 501 | string-view assertion in the generator (batch 12-14 reason unchanged; not fixable by a small extension) |

Late-release rows were not exercised by any wired slot (451 was not wired), so no new late-release fixture or mutation probe was added.

## Fixes during the batch

- Slot 503 test: the atexit loop did not skip ncell rows (`l4_test_util.hpp atexit_log ... dtor_rva`); fixed with `rows[i].kind == 'c' || 'o'`.
- Slot 503 checker: native string `NULL` pinned from the ncell source; fixed with `drop_strings`.
- Slot 511: two trees share the 0x50 size immediate; fixed with `st = [st[-1]]`.

## Risk: startup_ctors.cpp was reconstructed

During the batch I ran `git checkout native/core/startup_ctors.cpp` by mistake. The file is uncommitted working-tree state and
reverted to HEAD (460 rows). I recovered it from a scratch backup of the batch-14 state (521 rows) and re-inserted the 20 rows
for batches 12-15 using the wire.py logic. The `pre` statements (e.g. `external_target_0 = &s.slot518.var` for 469) and menu
flags of the re-inserted rows are inferred, not copied. The build, the full ctest (637 tests) and every per-slot test and
checker pass, and the row set matches the expect table, but the integrator should review `native/core/startup_ctors.cpp`
against the slot headers before committing. Backups: scratchpad `startup_ctors.cpp.bak`, `.HEADCOPY`.

## Verification (STATIC_ANALYSIS_VERIFIED + FIXTURE_VERIFIED; no execution of original code)

| command | result |
|---|---|
| `cmake --build local/build/startup-b15 -j 12` | exit 0, 0 warnings |
| `ctest --test-dir local/build/startup-b15 -j 12` | 100% passed, 637 tests |
| `SOTC_NATIVE_BUILD=local/build/startup-b15` checkers `check_[a-m]*` | 805 ok, 3 failed on stale pins (check_ctor_slots digest, check_entry_map, check_g1_evidence_pack) |
| digest + `gen_entry_map.py`, `g1_evidence_pack.py --write`, `survey_startup_gaps.py --write-doc`, then re-run those three plus `check_[n-z]*` and the new slot checkers | 127 ok, 0 failed |
| `python3 -I scripts/audit_publish_paths.py --all` | PUBLISH PATH AUDIT OK (4110 paths) |
| `python3 scripts/project_index.py` | written |

Regenerated: docs/entry_map.json, docs/G1_EVIDENCE.md (via `--write`), docs/STARTUP_GAP_SURVEY.md, docs/FOLDERS.md.
`check_console_var_ctors.py`, `check_fctor_slots_296_297_300.py`, `check_ctor_slots.py` and the two coverage tests were bumped
with `startup_gen/bump_pins.py` (then `DEBUG_PAGE_BATCH_15` split out by hand).

## Paste-ready text

HANDOFF.md:
> 2026-11-02 startup-ctor lane, CPU-01-STARTUP-BATCH-15: coverage 537 -> 541/573 (slots 493, 422, 511, 503; cliff still 408/0x5c7f50).
> New generator row kinds: twin uint (0x140b00/0x25ab0), multi-tree dtors, strncpy null-name cell. Build exit 0 / 0 warnings,
> ctest 637/637, checkers all pass, publish audit OK. Refused: 408, 427, 473, 498, 451, 454, 534, 566, 497, 501 (new mechanisms).
> Risk: native/core/startup_ctors.cpp was reconstructed after a mistaken `git checkout`; review before committing.
> Next: slot 408 needs the 0x1580a0 helper wired into the generator (POD stores, holder classes). Details: docs/checkpoints/2026-11-02-startup-batch-15.md.

docs/TASKS.md: CPU-01-STARTUP-BATCH-15 PASSED (4 slots, 541/573); remaining 32 slots need new mechanisms (see refusals).

docs/EVIDENCE.md:
- E1260 STATIC_ANALYSIS_VERIFIED: slot 493 (0x277560) bool/boolx pages tied to the ELF, 115 pins, 313+59 mutants rejected.
- E1261 STATIC_ANALYSIS_VERIFIED: slot 422 (0x56b980), 222 pins, 785+82 mutants rejected.
- E1262 STATIC_ANALYSIS_VERIFIED: slot 511 (0x1d90c0) twin uint 0x140b00/0x25ab0 + two 0x1d6210 trees, 142 pins, 372+60 mutants rejected.
- E1263 STATIC_ANALYSIS_VERIFIED: slot 503 (0x247f00) strncpy null-name cell, 253 pins, 683+119 mutants rejected.
- E1264 FIXTURE_VERIFIED: coverage 541/573, pending 810, menu root 354, atexit log 1322; registered index-set digest c10f5af9e35e1299f1189f666838fb52859c14879cfe83b66c62f967513afc2c.
