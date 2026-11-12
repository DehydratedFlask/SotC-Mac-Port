# Checkpoint: CPU-01-AOT-CFG-SCALE-01-BATCH17 (startup slots 451, 427, 534)

Agent: batch-17 worker. Local date 2026-11-03. Workers did not run git commit/tag/push/checkout/restore/reset/stash/clean, did not run `git add`,
and did not edit HANDOFF.md, docs/TASKS.md or docs/EVIDENCE.md: the integrator pastes sections 4 and 5. Build dir: `local/build/startup-b17`.

## 1. Result

| slot | rva | size | rows | status |
|---|---|---|---|---|
| 451 | 0x49d230 | 5126 B | variables, two dword-ranged variables (0x49ca60), two critical sections (0x3b310), bare name holder (0x499e90), zq row, command, menu items | WIRED, PASSED (FIXTURE_VERIFIED + STATIC_ANALYSIS_VERIFIED) |
| 427 | 0x5388f0 | | variables, items, `rec1` single named record (0xa81950), 3-word zero cell with dtor 0x534580, first-use registry record | WIRED, PASSED |
| 534 | 0x1418d0 | 6207 B | texture-streaming variables, twin-A uint rows, one UNBOUNDED inline enum, 1-word zero cell (dtor 0x13a0d0), two commands | WIRED, PASSED |
| 566 | 0x25ab0 twin | | uint row with external value cell 0x2dfd298 = obj-8 | NOT DONE (analysed, deferred; see 3) |
| 424 | 0x551140 | 28370 B | | NOT STARTED, feasibility report in 3 |
| 454 | | | RB-tree erase 0x482a90 not modelled | stays BLOCKED (not faked) |

Coverage: registered 544 -> 547 of 573 in the working tree (HEAD is 460; batches 12-17 are uncommitted); 26 unregistered, first 424 / 0x551140.
Registered-index-set sha256 6762c1f20c1ef561e82236e81133ddfe37d69221228cfdc6a55878515861bf65.

## 2. Generator extension (tools/lift/startup_gen)

New or extended row kinds (page.py analysis, genpage.py emission, mk_page.py pins). Row accounting remains a hard error (`account()`).
* `dwr` (0x49ca60 / 0x49cc60 dword-ranged variable), `crit` (0x3b310 critical section + atexit), bare `hold` (atexit with no call), `zq` with dtor 0x499e90 (slot 451).
* `rec1`: a single named record (pattern `QfA`, dtor 0xa81950, registrar `register_named_record_a81700`); the first use records the registry atexit 0xa81b80 before its own (slot 427).
* `zcell`: `words=3` (dtor 0x534580: frees the pointer, keeps the cell; slot 427) and `words=1` (dtor 0x13a0d0: deletes through the pointer and clears the cell; slot 534).
* `ienum`: carries `dtor` and `bounded`. The unbounded form stores one zero byte at +0x6c and +0x74, clears both engaged bytes, never writes the other bound bytes
  (`InlineEnumSpec::bounded`, default true, in native/core/startup_row_kinds.{hpp,cpp}).
* twin-A `uint` rows (0x140b00): seed class 0x15983c8, final vptr 0x1598360, dtor 0x13a090, per-row twin table, test letter `N`
  (native/core/helper_140b00_twin_site.{hpp,cpp}: `helper_init_unbounded_int_twin_var_site`).
* New native helper native/core/helper_49ca60_dword_var_site.{hpp,cpp}.
* New: `mk_451.py`, `mk_427.py`, `mk_534.py`. `wire.py` appends CMake lines with `>>`. The slot 427 startup row was hand-edited to set `record_registry` / `record_alloc`
  (the shared `StartupState.record_registry`, also used by 453, 464, 513).

## 3. Refusals, analysis, feasibility

* 566 (0x25ab0 twin-B): rows_of passes with 3 rows. Twin-B differs from twin-A: no pre-store of the vptr and no value write (default is `*cell`); the value cell is at obj-8 (0x2dfd298);
  final vptr 0x1595098, dtor 0x23d20, seed class 0x1595100. The object is only 0xd0 B (obj+0xd0 is the next object, the toge at 0x2dfd370) while the model `ConCmdRangedVar` is 0xd8 B,
  so it needs a head-only 0xd0 B type or a scratch copy plus external-cell semantics. The generator currently asserts `twin == 'a'`. Not attempted beyond analysis: 0 failed tries.
  Earlier analysis: docs/checkpoints/2026-11-02-slot408-prereqs.md.
* 424 (0x551140, 28370 B, 3739 instructions) feasibility, report only, nothing built: the tracker runs on it, but `account()` refuses at token 18 (events `NBiffANBiffANBiffAA` vs rows
  `NBiffANBiffA`: the final atexit pair is not a recognised grammar). Only 19 call events exist: three `0xf2f0` + `0x20af0` (concmd_base_ctor) variable constructions with `0xf590` releases and
  atexit records, then one huge straight-line block (4 jumps in total; 2088 stores through %rax) that initialises ONE ~26 KB object (store offsets 16..26384, 2087 distinct offsets;
  movl x1014, movq x683, vmovq x498, vmovdqu x293, vpunpcklqdq x241, vmovups x161, vpshufd x16) with a final atexit (0xf8f390) at 0x557fe4 and a `0xf8f3a0` stack-check at 0x55800c.
  A table-driven model IS feasible: a symbolic store replayer over one base register, extended with the vector forms (vpunpcklqdq / vpshufd / vmovups), yields an offset -> value table
  (immediates and rip-relative pointers) that is emitted as a constant table and re-derived by the checker. Needed first: (a) a store-replay tracker (not in track.py today),
  (b) the three variable rows for the head, (c) recognise the tail atexit pair, (d) the meaning of the ~26 KB object is unknown (inference only). Estimated: its own task. Not started.
* 454: the RB-tree erase chain (0x482a90, 1201 B; 0x482920, 0x47e0f0, 0x482f50) is not modelled and is not stubbed. Stays BLOCKED.
* NEEDS_PRINCIPAL: none. Failed tries: 0 per slot (generator bugs found and fixed inside the same attempt, before any gate: UnboundLocalError r1idx, an unbounded-enum accounting miss,
  a two-tag uint assertion, narrowing of 4294967295, a missing bool constant in the checker, missing `record_registry` / `record_alloc` in the slot 427 startup row).

## 4. Paste-ready evidence rows (E1320+)

```
E1320 | STATIC_ANALYSIS_VERIFIED | startup_gen-batch17 | row kinds dwr / crit / hold / zq-0x499e90 / rec1 / zcell(words 1 and 3) / unbounded ienum / twin-A uint added; row accounting remains a hard error; wire.py CMake edits append-only | tools/lift/startup_gen/{page,genpage,mk_page}.py, native/core/helper_{49ca60_dword_var_site,140b00_twin_site}.*
E1321 | STATIC_ANALYSIS_VERIFIED+FIXTURE_VERIFIED | slot 451 | 0x49d230 5126 B: variables, 2 dword-ranged vars, 2 critical sections, bare holder, zq, command, items; 409 operand pins; 1069 ELF + 177 native mutants rejected; tamper elf/native exit 1; plain stores outside rows pinned only by body hash, NOT modelled | check_ctor_slot_451.py, test_ctor_slot_451
E1322 | STATIC_ANALYSIS_VERIFIED+FIXTURE_VERIFIED | slot 427 | 0x5388f0: variables, items, rec1 record (0xa81950), 3-word zero cell (dtor 0x534580), registry first-use record 0xa81b80; 391 operand pins; 1046 ELF + 145 native mutants rejected; tamper exit 1 | check_ctor_slot_427.py, test_ctor_slot_427
E1323 | STATIC_ANALYSIS_VERIFIED+FIXTURE_VERIFIED | slot 534 | 0x1418d0 6207 B: texture streaming variables, twin-A uint rows, unbounded inline enum (one zero byte at +0x6c/+0x74, engaged bytes cleared), 1-word zero cell (dtor 0x13a0d0), 2 commands; 596 operand pins; 1409 ELF + 154 native mutants rejected; tamper exit 1 | check_ctor_slot_534.py, test_ctor_slot_534
E1324 | FIXTURE_VERIFIED | startup-count-pins | coverage 547/573 (460 at HEAD); concmd pending 901 (+55), debug menu root 388 (+7), atexit log 1467 (+70); unregistered 26, first 424 / 0x551140; each slot test derives its own counts from the ELF rows | test_startup_ctor_coverage, test_startup_ctors, check_ctor_slots.py
E1325 | STATIC_ANALYSIS_VERIFIED | slot424-feasibility | 28370 B, 3739 insns, 19 call events (3 concmd_base_ctor variables, tail atexit pair), 2088 straight-line stores into one ~26 KB object (offsets 16..26384); account() refuses at token 18; table-driven store-replay model feasible, not started | docs/checkpoints/2026-11-03-startup-batch-17.md
E1326 | STATIC_ANALYSIS_VERIFIED | slot566-454-status | 566 twin-B needs a 0xd0 B head-only variable type and external value cell (obj-8 = 0x2dfd298), not generated; 454 BLOCKED on RB erase 0x482a90 | docs/checkpoints/2026-11-03-startup-batch-17.md
E1327 | ARTIFACT_VERIFIED (build/test/checker only) | batch17-verification | cmake build clean; ctest 650/650; run_all_checkers 859 passed 0 failed; audit_publish_paths --all OK (4110 paths); check_entry_map OK (2258 constants -> 1556 RVAs in 523 headers); check_g1_evidence_pack OK | local/build/startup-b17
```

## 5. Paste-ready HANDOFF / TASKS text

HANDOFF: CPU-01-AOT-CFG-SCALE-01-BATCH17 (2026-11-03): slots 451, 427 and 534 wired (registered 547/573, 26 open, first 424 / 0x551140).
Verification: ctest 650/650, run_all_checkers 859/0, audit --all OK, entry_map regenerated and OK, G1_EVIDENCE.md / STARTUP_GAP_SURVEY.md / FOLDERS.md regenerated.
Next exact action: slot 566 (head-only 0xd0 B variable + external value cell; spec in section 3), then slot 424 as its own task (store-replay tracker + constant table; feasibility in section 3);
454 stays blocked on RB erase 0x482a90. NOT RUN: boot, audio, save/reload, Metal validation; this batch advances no runtime gate.

TASKS: `CPU-01-AOT-CFG-SCALE-01-BATCH17` PARTIAL: 451, 427, 534 PASSED; 566 open (analysed, not generated); 424 feasible, not started; 454 BLOCKED. Attempt count 1, failed tries 0.

## 6. Risks and disclosures

* Plain stores in a slot body that belong to no constructor row are pinned only by the body sha256 and the per-instruction pins; they are NOT modelled (451, 427, 534).
* The old generated files of slots 408, 473, 498 and 453 were regenerated by the extended generator during this batch (the earlier copies were untracked, hence not recoverable from git).
  Their tests and checkers pass in the full run (859/0), but anyone diffing against the batch-16 text will see generator-driven differences.
* The twin-site helper `helper_140b00_twin_site` models the twin-A uint rows only (seed 0x15983c8, final vptr 0x1598360, dtor 0x13a090); twin-B (566) is deliberately not covered.
* Destructor RVAs, vtables and functor tags are identity tags; nothing executes an original RVA.
* Slot 427 uses the shared `StartupState.record_registry` (also 453, 464, 513); the startup row sets `record_registry` and `record_alloc` (default_slot427_record_alloc). A host that omits them gets `ok == false`.
* `wire.py` edits startup_ctors.hpp/.cpp in place: concurrent workers on those files can collide. native/CMakeLists.txt was only appended to (`>>`).
* The stale prose in docs/G1_EVIDENCE.md (e.g. "451 of 573 rows") predates the regenerated numeric block; the checker passes but the prose needs a human pass.
* The working tree holds uncommitted batches 12-17; HEAD coverage is 460/573. The integrator should commit the allowlisted paths and re-run the audit.

## 7. Commands run (all from the workspace root)

```
cmake --build local/build/startup-b17 -j 12                                                      # incremental, clean
ctest --test-dir local/build/startup-b17 -j 12 --output-on-failure                               # 650/650 passed
SOTC_NATIVE_BUILD=local/build/startup-b17 tools/lift/run_all_checkers.sh                         # 859 passed, 0 failed
python3 -I scripts/audit_publish_paths.py --all                                                  # OK (4110 paths)
python3 tools/lift/gen_entry_map.py; python3 tools/lift/check_entry_map.py                       # OK
python3 tools/lift/g1_evidence_pack.py --write; python3 tools/lift/check_g1_evidence_pack.py     # OK
python3 tools/lift/survey_startup_gaps.py --write-doc; python3 scripts/project_index.py           # OK
python3 -I tools/lift/check_ctor_slot_{451,427,534}.py [--tamper elf|native]                     # OK; both tamper modes exit 1
```
NOT RUN: `python3 -I scripts/check_commit_attribution.py` (no commits made by this worker), the full AGENTS.md `orch-r22` build dir (the batch used startup-b17), a from-scratch clean configure of startup-b17.
