# Checkpoint: CPU-01-AOT-CFG-SCALE-01-BATCH18 (startup slots 566, 424)

Agent: batch-18 worker. Local date 2026-11-03. Workers did not run git commit/tag/push/checkout/restore/reset/stash/clean, did not run `git add`,
and did not edit HANDOFF.md, docs/TASKS.md or docs/EVIDENCE.md: the integrator pastes sections 5 and 6. Build dir: `local/build/startup-b18`.

## 1. Result

| slot | rva | size | rows | status |
|---|---|---|---|---|
| 566 | 0x25de0 | 1194 B | build-info variables (RetailBuild, BuildVersion, ToolServerIP, MirrorWorld), one twin-B uint with an external value cell (obj-8 = 0x2dfd298) and a menu row | WIRED, PASSED (FIXTURE_VERIFIED + STATIC_ANALYSIS_VERIFIED) |
| 424 | 0x551140 | 28370 B | three console commands (unlockGalleryItem, unlockGallerySoundtrackItem, unlockColossusViewerItem) and one 0x6720 B object written by 2088 straight-line stores | WIRED, PASSED (FIXTURE_VERIFIED + STATIC_ANALYSIS_VERIFIED); table semantics are INFERENCE |
| 454 | 0x483e40 | | RB-tree erase 0x482a90 not modelled | stays BLOCKED (not faked) |

Coverage: registered 547 -> 549 of 573 in the working tree (HEAD is 460; batches 12-18 are uncommitted); 24 unregistered, first **428 / 0x52ec10**.
Registered-index-set sha256 9c4e927ca9217a3d00d4f0be4727b8edacd0d93b70e55c4747f58ad63e8e33ed. Failed tries: 0 for both slots.

## 2. Slot 424: the store-replay tracker and the table model

What the original does (observed): three `concmd_base_ctor` command rows (0xa0 B objects at 0x2eff700 / 7a0 / 840), then 3577 straight-line instructions
(0x55141d..0x557fe4, no call, no branch) that write 2088 stores into ONE static object at 0x2eff8e0 (size 0x6720 = 274 x 0x60), then
`__cxa_atexit(0x54dbb0, 0, &__dso_handle)`. The destructor 0x54dbb0 walks the object last-to-first in 0x60 B records and releases a pointer at +0x40 of each record
(INFERENCE: an array of records with a small-buffer callable; what the records mean is NOT established).

Instruction classes in the range (all pinned, nothing skipped): `movl $imm32, mem` 1011, `movq reg, mem` 406, `movq $imm32, mem` 232, `vmovdqu xmm, mem` 292, `vmovups xmm, mem` 147, plus
`lea`, `vmovq`, `vpunpcklqdq`, `vpshufd`, `vpxor`/`vxorps`, `addq`, `xorl`, `movq %r14,%rdx`.

Tracker (new): `tools/lift/store_bytes_replay.py` is a closed byte decoder (REX.W lea/mov/add, c7 /0, 89, 83 /0, 31, VEX2 7f/11/70/6c/ef/57, VEX3 vmovq) over `Elf.raw` bytes;
`tools/lift/startup_gen/replay.py` is the objdump-text twin. `startup_gen/page.py::_store_table_row` runs BOTH, requires identical stores, final registers and instruction counts, and
raises `ReplayRefused` for any other instruction class, a SIB byte, a rip-relative memory operand, a load, a branch, a store through a non-address register, or a shuffle that splits a 64-bit lane.
Every lane is classified: constant (0), self pointer into the object (1), `.rodata` printable NUL-terminated C string (2; 531 distinct), or identity tag (3; 44: seven text functions
0x54bcf0 0x54bd50 0x54bd60 0x54c1e0 0x54d080 0x54d8a0 0x54d8b0 verified as FDE starts, and 37 relro vtable-like addresses). Anything else is refused.

Native: `native/core/store_table.{hpp,cpp}` (`replay_store_table`) applies a table of 32 B records (instruction rva, offset, width, two lane kinds, two values) to an image. It validates EVERY
record before writing the first byte (image untouched on refusal) and then applies them in order; bytes no record covers are never written. String lanes store pointers to NATIVE string-pool copies (the original
`.rodata` address is kept in `kSlot424StoreStringRva` for the ELF tie), self lanes store `image + offset`, tag lanes are stored as integers and never followed. `native/tests/test_store_table.cpp` tests lanes,
ordering, untouched bytes and fail-closed behaviour. The generated `native/core/ctor_slot_424_551140.{hpp,cpp}` carry the 2088-record table, the string pool and the row functions; the atexit record for 0x54dbb0 is logged
with a null object, like the original.

Checker `tools/lift/check_ctor_slot_424.py` (EXTRA `store_table` in `tools/lift/bool_page_lib.py`): re-decodes the 3577 instructions from ELF bytes (overlay aware), requires them to equal the native header table record
by record (instruction rva, offset, width, lane kinds and values), re-reads each of the 531 pooled strings at its `.rodata` address, checks the identity-tag set, the atexit argument registers (dtor, 0, &__dso_handle) and the PLT
call, and hash-pins the destructor and the seven tag functions; the whole range plus the seed `lea` and the call are also pinned byte for byte.

## 3. Generator and doc changes

* `startup_gen`: new row kind `stab` (page.py, genpage.py, mk_page.py, `mk_424.py`); `wire.py` unchanged (its CMake lines were already added by `>>`, the duplicate it appended was removed).
* `tools/lift/g1_evidence_pack.py`: now also owns three generated regions of docs/G1_EVIDENCE.md (`startup-prose`, `ctor-check`, `startup-coverage`, section 1.2 bullets, the checker transcript and the section 4 table/prose),
  so the stale "451 of 573" text cannot recur; `--check` fails when any region drifts. Section 4's cliff narrative and section 6 item 1 were rewritten for slot 428.
* `native/tests/test_startup_ctor_coverage.cpp`: 549 / cliff 428 / 0x52ec10 / +24; concmd pending 909, menu root 389, atexit 1477 (the batch-17 values 901 / 388 / 1467 plus slot 566 (+5 variables, +1 menu row, +6 atexit)
  and slot 424 (+3 commands, +4 atexit: three commands and the 0x54dbb0 record)).

## 4. Evidence and probes (all run in this tree)

* `cmake -S native -B local/build/startup-b18` + `cmake --build ... -j 12`: clean (0 errors, 0 warnings in the grep filter).
* `ctest --test-dir local/build/startup-b18 -j 12`: 653/653 passed (includes `store_table`, `ctor_slot_424`, `ctor_slot_566`, `startup_ctor_coverage`).
* `python3 -I tools/lift/check_ctor_slot_424.py`: OK, body a77e438645f9... 28370 B + 1895 operand pins; mutation self-test 4918 ELF + 62 native mutants all rejected (44 s).
* `--tamper elf` exit 1, `--tamper native` exit 1. Extra native-drift probes (header edited in place, restored): a record value, a record instruction rva, a pooled string, and a dropped record each exit 1 with a specific message.
* `python3 -I tools/lift/check_ctor_slots.py` OK (549 registered pairs, 24 unregistered, first 428 / 0x52ec10); `python3 tools/lift/check_entry_map.py` OK (2267 constants -> 1564 distinct RVAs in 525 headers);
  `python3 tools/lift/g1_evidence_pack.py --check` and `check_g1_evidence_pack.py` OK; `python3 -I scripts/audit_publish_paths.py --all` OK (4110 paths).
* `run_all_checkers.sh` (build dir startup-b18): 861 passed, 0 skipped, 0 failed (8 min).
* `python3 tools/lift/survey_startup_gaps.py --write-doc`: HEAD 460/573, working tree 549/573, 24 open gaps.

NOT RUN / not claimed: no boot, no audio, no save/load; the table's meaning, the order in which the original reads the 274 records and any consumer of the object are NOT established. The strings are
observed .rodata text, not a verified meaning. Metal/GPU and launcher gates are untouched.

## 5. Paste-ready evidence rows (E1340+)

| E1340 | 2026-11-03 | slot566: 0x25de0 wired (build-info variables + twin-B uint with external cell + menu row), check_ctor_slot_566 OK, ctor_slot_566 test passed | docs/checkpoints/2026-11-03-startup-batch-18.md | STATIC_ANALYSIS_VERIFIED + FIXTURE_VERIFIED |
| E1341 | 2026-11-03 | store-replay tracker: tools/lift/store_bytes_replay.py (closed byte decoder) and startup_gen/replay.py (objdump twin) agree on slot 424's 3577 instructions / 2088 stores; 531 rodata strings, 44 identity tags, 135 self lanes classified; any other form refused | docs/checkpoints/2026-11-03-startup-batch-18.md | STATIC_ANALYSIS_VERIFIED |
| E1342 | 2026-11-03 | slot424: 0x551140 wired as three commands + ELF-tied 0x6720 B store table (native/core/store_table, ctor_slot_424_551140), check_ctor_slot_424 OK (4918 ELF + 62 native mutants rejected; tamper elf/native exit 1; four native-drift probes rejected), ctor_slot_424 + store_table tests passed | docs/checkpoints/2026-11-03-startup-batch-18.md | STATIC_ANALYSIS_VERIFIED + FIXTURE_VERIFIED (table meaning INFERENCE) |
| E1343 | 2026-11-03 | g1_evidence_pack.py generates the section 1.2 / ctor-check / section 4 prose; stale "451 of 573" removed; --check covers all regions | docs/G1_EVIDENCE.md | ARTIFACT_VERIFIED |
| E1344 | 2026-11-03 | startup gap survey after batch 18: working tree 549/573, 24 open, cliff 428 / 0x52ec10 (5375 B, 1071 insn, 49 atexit, helpers 0xa59620 x37 and 0xa42960 x6 unmodelled) | docs/STARTUP_GAP_SURVEY.md | STATIC_ANALYSIS_VERIFIED |
| E1345 | 2026-11-03 | batch18-verification: see section 4 counts (ctest 653/653; `SOTC_NATIVE_BUILD=local/build/startup-b18 tools/lift/run_all_checkers.sh`: 861 passed, 0 skipped, 0 failed; audit_publish_paths --all OK 4110 paths; check_entry_map OK 2267 constants) | docs/checkpoints/2026-11-03-startup-batch-18.md | PASSED for the listed commands |

## 6. Paste-ready HANDOFF / TASKS text

HANDOFF: CPU-01-AOT-CFG-SCALE-01-BATCH18 complete for slots 566 and 424 (attempts: 0 failed). Coverage 549/573, cliff 428 / 0x52ec10 (5375 B unique large body; needs helpers 0xa59620 and 0xa42960).
Next exact action: slot 428 (model the two helpers; 49 repeated atexit+release blocks suggest a table-driven shape like 424) or the pure debug-page slots 445, 501, 570 and the simple leaf 494; 454 stays blocked on the RB erase 0x482a90.
Risks: store-table object semantics are inference; `ctest` 653/653 and the checkers are the only gates here; nothing was committed by the worker.
TASKS: CPU-01-AOT-CFG-SCALE-01-BATCH18 -> done (566, 424); new open work: slot 428, then 445 / 501 / 570 / 494; 454 BLOCKED.

## 7. What the next open slots need (from the refreshed survey)

* 428 (cliff, 0x52ec10, 5375 B, 1071 insn): one body, 49 `0xa59620`(249 B, x37) / `0xa2ddc0`(10 B, x20) / `0xa42960`(590 B, x6) call groups each followed by `0xf590` and `__cxa_atexit`; 0 indirect calls, one canary branch,
  224 rip targets. Needs the two unmodelled helpers (0xa59620, 0xa42960) as native functions first; then it is a repeated-block table (not a store run), so a block-structure checker + table-driven model is the sensible shape.
* 570 (0x16d80), 445 (0x4cfe80), 501 (0x24db80) are the "pure" debug-page shapes (only canary branch, only functor slot-3 calls, helpers already modelled): templates 316 / 309 / 326. 494 (0x26f7c0) is a 22-instruction leaf with two unmodelled helpers.
* 454 (0x483e40, 272 insn): blocked on the red-black erase 0x482a90 (not modelled, not faked).
* 554 (0x6efe0, 112 KB, 19550 insn, 436 atexit, 22 unmodelled helpers incl. 17 near-identical 397 B bodies) is an order of magnitude larger and needs its own decomposition.
