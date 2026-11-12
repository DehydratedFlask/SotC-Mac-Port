# Checkpoint: CPU-01-AOT-CFG-SCALE-01-BATCH16 (startup slots 408, 473, 498)

Agent: batch-16 worker. Local date 2026-11-02 .. 2026-11-03. Workers did not run git commit/tag/push/checkout/restore/reset/stash/clean, did not run `git add`,
and did not edit HANDOFF.md, docs/TASKS.md or docs/EVIDENCE.md: the integrator pastes sections 4 and 5. Build dir: `local/build/startup-b16`.

## 1. Result

| slot | rva | size | rows | status |
|---|---|---|---|---|
| 408 | 0x5c7f50 | 4750 B | perspective prelude (0x1580a0), seven name holders, zero-vector headers, fourteen console commands | WIRED, PASSED (FIXTURE_VERIFIED + STATIC_ANALYSIS_VERIFIED) |
| 473 | 0x355a00 | 6487 B | name holder, variables, 0x26ea0 toggles, 0x26c10 group, five 0x26b60 separators, one command, 0x1f220 ring header | WIRED, PASSED |
| 498 | 0x252ae0 | 2379 B | two bools, int, uint, four 0x26db0 toggles, one 0x26b60 separator, 24-record member-hash table (48 six-byte getters) | WIRED, PASSED |
| 427 | | | | REFUSED this batch (see 3) |
| 451 | 0x49d230 | 5126 B | | NOT WIRED (analysed only; see 3) |
| 454 | | | RB-tree erase 0x482a90 not modelled | stays BLOCKED (not faked) |
| 534, 566 | | | | NOT ATTEMPTED (534 tracker fails on an inline enum with an unresolved store; 566 `rows_of` passes with 3 rows but no model was generated) |

Coverage: registered 460 -> 544 of 573 in the working tree (batches 12-16 are uncommitted; HEAD is 460); 29 unregistered, first 424 / 0x551140.
Batch 16 itself adds 3 (408, 473, 498).

## 2. Generator extension (tools/lift/startup_gen)

New row kinds (page.py analysis, genpage.py emission, mk_page.py pins): `pre` (0x1580a0 perspective prelude), `zvec` (zero-header vector with seven dtors),
`sep` (0x26b60 menu separator), `ringhdr` (0x1f220 + atexit 0x354470), `gettable` (run of `mov eax, imm32 ; ret` six-byte getters feeding a 24 x {lo, hi, size} dword table).
Row accounting is still a hard error (`account()`): the event token stream must equal the concatenated row grammars.
`wire.py` now appends the CMake lines with `open(mp,'a')` (append-only). It still edits startup_ctors.hpp/.cpp in place (inherited risk, see 6).
New: `mk_408.py`, `mk_473.py`, `mk_498.py`. `track.py`: the 0x1580a0 call event captures `fov` and `xmm2`.

## 3. Refusals and analysis

* 451 (5126 B): mismatch at token 52. Needs (a) a `dwr` row kind for the dword-ranged constructors 0x49ca60 / 0x49cc60 (same token grammar as an unbounded
  uint row: `<49ca60>` then `iRffA` or `fiRfA`; min/max in `f_rcx`/`f_r8`, functor class from `lint_tag` = 0x15bfce8 / 0x15bfd80; model
  `helper_ranged_dword_var_49ca60` with a `RangedVarArgs`; atexit dtor 0x498b20 / 0x498b60 = `helper_498b20_ranged_var_dtors`), (b) a `crit` row kind
  (`<3b310>(obj)` + atexit dtor 0x499e50 on 0x2ef9ec0 and 0x499e60 on 0x2ef9f98; model `critsec_init_at` + identity tag) and (c) a bare `hold` row (atexit 0x499e90 on 0x2ef9fc8 with
  no call). The dtor models exist (`helper_499e90_slot451_dtors`, `helper_498b20_ranged_var_dtors`, `critsec_3b310`). Remaining work is generator plumbing (SZ/TY tables,
  call emission, test letters, dtor expression of the atexit-order check, mk_page pins). No attempt was made, so no try is counted for 451.
* 427: `rec1` and the zero cell need an invasive rework of the `recs` kind (`nrecs <= 1` design). Deferred, no try counted.
* 454: the RB-tree erase chain (0x482a90, 1201 B; 0x482920, 0x47e0f0, 0x482f50) is not modelled. It must not be stubbed. Stays BLOCKED.
* NEEDS_PRINCIPAL: none. Failed tries: 0 per slot (several template and fixture bugs were fixed within the same attempt before any gate: ASan crash from a poisoned
  `separator_alloc` moved into the per-call record set; ring and gettable rows excluded from the menu item counter; `NullNameCell` include).

## 4. Paste-ready evidence rows (E1300+)

```
E1300 | STATIC_ANALYSIS_VERIFIED | startup_gen-batch16 | generator row kinds pre / zvec / sep / ringhdr / gettable added; row accounting remains a hard error; wire.py CMake edits append-only | tools/lift/startup_gen/{page,track,genpage,mk_page,wire}.py
E1301 | STATIC_ANALYSIS_VERIFIED+FIXTURE_VERIFIED | slot 408 | 0x5c7f50 4750 B: perspective prelude, 7 name holders, zero-vector headers, 14 commands; 412 operand pins; 1025 ELF + 145 native mutants rejected; tamper elf/native exit 1; host input perspective_tanf; plain stores outside rows pinned only by body hash, NOT modelled | check_ctor_slot_408.py, test_ctor_slot_408
E1302 | STATIC_ANALYSIS_VERIFIED+FIXTURE_VERIFIED | slot 473 | 0x355a00 6487 B: holder, variables, toggles, group, 5 separators, command, ring header; 519 operand pins; 1286 ELF + 181 native mutants rejected; tamper exit 1; host input separator_alloc; plain stores NOT modelled | check_ctor_slot_473.py, test_ctor_slot_473
E1303 | STATIC_ANALYSIS_VERIFIED+FIXTURE_VERIFIED | slot 498 | 0x252ae0 2379 B: 2 bools, int, uint, 4 toggles, separator, 24-record member-hash table (48 getters pinned body by body, call/store run pinned byte for byte); 300 operand pins; 815 ELF + 74 native mutants rejected; tamper exit 1; table meaning (type id / member hash) is INFERENCE | check_ctor_slot_498.py, test_ctor_slot_498
E1304 | FIXTURE_VERIFIED | startup-count-pins | coverage 544/573 (460 at HEAD); concmd pending 846 (+36), debug menu root 381 (+27), atexit log 1397 (+75); unregistered 29, first 424 / 0x551140; each slot test derives its own counts from the ELF rows | test_startup_ctor_coverage, test_startup_ctors, check_ctor_slots.py
E1305 | FIXTURE_VERIFIED | startup-pointer-identity | slot469.external_target_0 == &slot518.var; slot571/572.external_target_0 == &global_2dfcbd0_standin; slot453/464/513.record_registry == &record_registry | test_startup_ctor_coverage.cpp, test_startup_ctors.cpp
E1306 | STATIC_ANALYSIS_VERIFIED | slot451-427-454-refusals | 451 needs dwr / crit / hold row kinds (not built); 427 needs recs rework; 454 RB erase unmodelled; 534 tracker stops on an unresolved inline-enum store; 566 not generated | docs/checkpoints/2026-11-02-startup-batch-16.md
E1307 | ARTIFACT_VERIFIED (build/test/checker only) | batch16-verification | cmake build clean; ctest 647/647; run_all_checkers 856 passed 0 failed; audit_publish_paths --all OK (4110 paths); check_entry_map OK (2245 constants -> 1547 RVAs in 520 headers) | local/build/startup-b16, /tmp/b16_checkers.log
```

## 5. Paste-ready HANDOFF / TASKS text

HANDOFF: CPU-01-AOT-CFG-SCALE-01-BATCH16 (2026-11-03): slots 408, 473 and 498 wired (registered 544/573, 29 open, first 424 / 0x551140).
Verification: ctest 647/647, run_all_checkers 856/0, audit --all OK, entry_map regenerated (stale after the new headers, now OK). Next exact action: add the `dwr`, `crit` and `hold`
row kinds to tools/lift/startup_gen and wire 451 (spec in section 3); then rework `recs` for 427; 454 stays blocked on the RB erase 0x482a90. 534 and 566 not attempted.
NOT RUN: boot, audio, save/reload, Metal validation; this batch advances no runtime gate.

TASKS: `CPU-01-AOT-CFG-SCALE-01-BATCH16` PARTIAL: 408, 473, 498 PASSED; 451 / 427 / 534 / 566 open (not attempted beyond analysis); 454 BLOCKED. Attempt count 1, failed tries 0.

## 6. Risks and disclosures

* Plain stores in a slot body that belong to no constructor row (408, 473, 498) are pinned only by the body sha256 and the per-instruction pins; they are NOT modelled.
* Host-input hooks: `perspective_tanf` (408), `separator_alloc` (473, 498). A host that does not supply them gets `ok == false`, not a fake success.
* The 498 member-hash table is copied from ELF-derived immediates; its meaning is inference only.
* The destructor RVAs, vtables and functor tags are identity tags; nothing executes an original RVA.
* `wire.py` edits startup_ctors.hpp/.cpp in place: concurrent workers on those files can collide (inherited from batch 15).
* The working tree holds uncommitted batches 12-16; HEAD coverage is 460/573. The integrator should commit the allowlisted paths and re-run the audit.
* Pointer-identity asserts were added to both test_startup_ctors.cpp and test_startup_ctor_coverage.cpp; the field names were verified in the headers first.

## 7. Commands run (all from the workspace root)

```
cmake -S native -B local/build/startup-b16 && cmake --build local/build/startup-b16 -j 8        # clean
ctest --test-dir local/build/startup-b16 -j 8                                                    # 647/647 passed
SOTC_NATIVE_BUILD=local/build/startup-b16 tools/lift/run_all_checkers.sh                         # 856 passed, 0 failed
python3 -I scripts/audit_publish_paths.py --all                                                  # OK (4110 paths)
python3 -I tools/lift/{check_ctor_slots,check_console_var_ctors,check_fctor_slots_296_297_300,check_evidence_grades,check_entry_map}.py   # all OK after gen_entry_map.py
python3 -I tools/lift/gen_entry_map.py; g1_evidence_pack.py --write; survey_startup_gaps.py --write-doc; python3 scripts/project_index.py
python3 -I tools/lift/check_ctor_slot_{408,473,498}.py [--tamper elf|native]                      # OK; both tamper modes exit 1
```
NOT RUN: `python3 -I scripts/check_commit_attribution.py` (no commits made by this worker), the full AGENTS.md `orch-r22` build dir (the batch used startup-b16).
