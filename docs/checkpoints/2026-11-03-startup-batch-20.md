# Checkpoint: CPU-01-AOT-CFG-SCALE-01-BATCH20 (startup slots 445, 501, 494 + hash-store / allocator-pool models)

Agent: batch-20 worker. Local date 2026-11-03. Workers did not run git commit/tag/push/checkout/restore/reset/stash/clean, did not run `git add`,
and did not edit HANDOFF.md, docs/TASKS.md or docs/EVIDENCE.md: the integrator pastes sections 5 and 6. Build dir: `local/build/startup-b20`.

## 1. Result

| slot | rva | size | what | status |
|---|---|---|---|---|
| 445 | 0x4cfe80 | 2016 B | four name holders, `uiForceAllowExtendedVideoModes` bool, 79 hash-to-dword static stores (rodata UI name hashed by the path-folding FNV 0xa2ddc0, stored as a dword) | WIRED, PASSED (`check_ctor_slot_445.py`, tamper elf/native exit 1) |
| 501 | 0x24db80 | 3244 B | animation debug page: 7 variables (one string variable with an EMPTY initial view from 0xf730), a group, a label item (0x26800), 8 toggles | WIRED, PASSED (`check_ctor_slot_501.py`, tamper elf/native exit 1) |
| 494 | 0x26f7c0 | 84 B | two pool registries built by 0x26e340 / 0x26e770 (4096 nodes of 0x18 / 0x20 B each, free-list chain), `atexit(0x269590 / 0x269610)`; builders, reserve and push-grow helpers LIFTED natively | WIRED, PASSED (`check_ctor_slot_494.py`, tamper elf/native exit 1) |
| 570 | 0x16d80 | 740 B | three bespoke atexit-registered static tables (dword flags, zero cells, `xchgq`, self-referential triple) + bool variable + 0x26ea0 toggle | NOT WIRED (see section 6; no try spent) |
| 497 | 0x262a80 | 9164 B | 87 releases, 54 atexit records, 7 functor fix-ups, 0x8d650 prelude, 0x20 B config record with name view | NOT WIRED (see section 6; no try spent) |
| 454 | 0x483e40 | 1431 B | RB erase 0x482a90 not modelled | stays BLOCKED (not faked) |

Coverage: registered 551 -> 554 of 573 in the working tree (HEAD is 460; batches 12-20 are uncommitted); 19 unregistered, first **446 / 0x4c92a0**
(slot 445 closed the old cliff). Registered-index-set sha256 7dd480b66acc35dabc2b9aaf3021640e1c9ca8b22128726865d96fa83b70cb0b. Failed tries: 0 for every slot
(570 and 497 were triaged, not attempted: no try spent).

## 2. Generator and library changes

* `tools/lift/hash_store_bytes.py` (new): closed byte decoder for the "hash rodata string, store the dword" run. Allowed bytes only: `lea r64,[rip+d]`, `mov rdi,r14|r15`,
  `call 0xa2ddc0`, `mov [rbx+disp],eax`, `mov rcx,[rip+stack-guard]`. Anything else raises `HashStoreRefused`. Overlay aware (a perturbed ELF perturbs the decode).
* `tools/lift/store_bytes_replay.py` and `tools/lift/startup_gen/replay.py`: hash-store row kind (hstab) in the generator pipeline (page.py, genpage.py, mk_page.py).
  Row accounting is unchanged and still a hard error. No `xchg` form was added to either decoder (570 not attempted), so the two decoders still agree.
* `tools/lift/bool_page_lib.py`: hash-store checks and the empty-view string variable check.
* `native/core/hash_store_table.{hpp,cpp}` (new): the 79 stores as a native table of (hash-of-name, destination) pairs; hash is the native FNV (no original call).
* `native/core/ctor_slot_family_items.{hpp,cpp}`: `family_label_item_26800` (the 0x26800 label item used by 501).
* `native/core/ctor_slot_494_26f7c0.{hpp,cpp}` (hand-written with `l4_slot_lib`): `slot494_reserve_more` (0x26e5f0 / 0x26ea10 / 0x26e4d0 / 0x26e8f0), `slot494_build_registry`
  (0x26e340 / 0x26e770), `slot494_destroy_registry` (0x269590 / 0x269610 modelled and tested; the startup runner only RECORDS them). The allocator thunks 0x17070 / 0x170b0 are HOOKS
  (TLS allocator dispatch, not lifted). The length_error / bad_alloc paths are modelled as `false`. `ctor_tiny_misc_slots.hpp` keeps its older hook model of 494 (names carry the
  `Pools` tag to coexist); the startup run uses the lifted model.
* Wiring: `native/core/startup_ctors.{hpp,cpp}` (rows 445, 501, 494; the 501 row sets `separator_alloc = default_family_separator_alloc()`; the 494 row uses `slot494_default_pool_hooks()`),
  `native/tests/test_startup_ctor_coverage.cpp` (554 / cliff 446 / 0x4c92a0 / +19; pending 927, menu root 404, atexit 1563 - batch 19: 918 / 394 / 1538), `test_startup_ctors.cpp`,
  `tools/lift/check_console_var_ctors.py`, `check_fctor_slots_296_297_300.py`, `check_ctor_slots.py` (digest via fixdigest), `docs/entry_map.json`, `docs/STARTUP_GAP_SURVEY.md`,
  `docs/G1_EVIDENCE.md` (generated regions plus the changelog line), `native/CMakeLists.txt` (appended with `>>`).

## 3. Slot 445 and the hash policy

What the body does (observed): four name holders (borrowed 0xf2f0 views released by 0xf590, atexit tags), the bool variable `uiForceAllowExtendedVideoModes`, and 79 times
`rdi = &rodata_string; call 0xa2ddc0; mov [rbx+disp], eax` into one static object. The strings are plain UI option names (benign, not credential-like); the table lives in
`hash_store_table.cpp`. The checker ties the 79 (string, destination) pairs against the ELF, the body SHA `4cc79931...` and FDE extent, 232 operand pins, and the 0xa2ddc0 /
0x162020 callee bodies by hash. Mutation self-test: 724 ELF + 67 native mutants, all rejected. Meaning ("UI option page") is INFERENCE from the strings.

Policy note carried over from batch 19: no credential-like byte string from the ELF was copied into the repository this batch. All strings in 445 / 501 are ordinary UI names.

## 4. Slots 501 and 494

**501** (observed): 7 variables (5 bool at functor tag 0x159bee0, the rest per the shape row), one string variable whose initial value is the shared EMPTY static SharedStr view
returned by 0xf730 (so the variable holds a borrowed view, `control == nullptr`, per the native convention), a group, a 0x26800 label item and 8 toggles; 18 objects from 0x2eeaed8.
Checker: 324 operand pins, body SHA `a960bf23...`, mutation 857 ELF + 103 native, all rejected. Inference: "skeleton / bounds / joint-name display" from the strings.

**494** (observed, ELF-tied): the body is only 84 B (2 builder calls, 2 `__cxa_atexit`); everything interesting is in the 14 hash-pinned helper bodies. Pool block 0x28 B
(+0 never written, node vector at +8/+0x10/+0x18, free-list head at +0x20), 4096 nodes {0, 0xffffffff@8, 0@0x10 [, 0xffffffff@0x18]}, dword at +0xc never written, free list built from the last
node down, registry vector<Pool*> at +8 with push_back growth max(size+1, cap+cap/2). The aliasing branches in the push compare a stack slot and are unreachable for a heap vector (STATIC reading,
not tested against the guest). The startup run allocates two pools and never frees them: this matches process-lifetime statics, the dtor model is exercised by `test_ctor_slot_494` only
(allocation failure at each of 6 allocations, destroy shapes, reserve growth).

## 5. Evidence and probes (all run in this tree)

* `cmake -S native -B local/build/startup-b20` + `cmake --build ... -j 12`: clean (exit 0).
* `ctest --test-dir local/build/startup-b20 -j 12 --output-on-failure`: 660/660 passed (batch 19: 657; +3: ctor_slot_445, ctor_slot_501, ctor_slot_494).
* `check_ctor_slot_445.py`, `check_ctor_slot_501.py`, `check_ctor_slot_494.py`: OK; `--tamper elf` and `--tamper native` exit 1 for each (run explicitly).
* `check_ctor_slots.py` OK (554 registered pairs, 19 unregistered, first 446 / 0x4c92a0); `check_entry_map.py` OK (2298 constants -> 1585 distinct RVAs in 532 headers);
  `g1_evidence_pack.py --write` then `check_g1_evidence_pack.py` OK (554/573 startup); `python3 -I scripts/audit_publish_paths.py --all` OK (4110 paths); `python3 scripts/project_index.py` OK (99800 folders).
* `survey_startup_gaps.py --write-doc`: HEAD 460/573, working tree 554/573, 19 open gaps (14 debug-page template, 5 unique large body).
* `SOTC_NATIVE_BUILD=local/build/startup-b20 tools/lift/run_all_checkers.sh`: 868 passed, 0 skipped, 0 failed (exit 0).

NOT RUN / not claimed: no boot, no audio, no save/load; the guest bytes were never executed; table meanings (UI option page, animation debug page, pool registries) are INFERENCE;
Metal/GPU and launcher gates untouched; the 494 aliasing-branch and throw-path reading is static only.

## 6. Open slots: next-blocker analysis (survey: 19 open)

| slot | blocker (observed) | next exact action |
|---|---|---|
| 570 (0x16d80, 740 B, effort M) | `genpins.gen(570)` gives 82 pins, sha `506dcf2a...c18255`; row accounting fails on the 3 bespoke preamble tables: dword flags, zero cells, `xchgq %rax, 0x2dd9ff0`, a self-referential pointer triple, plus 3 identity dtor atexit records (0x11070, 0x11080, 0x11090; dtor 0x13790 body must be hash-pinned) | hand-written `l4_slot_lib` model (like 494) using the 82 pins, `lane_m6::init_bool_var_literal` and `family_toggle_item_26ea0`; OR add an `xchg` row to BOTH decoders (`store_bytes_replay.py`, `startup_gen/replay.py`) |
| 497 (0x262a80, 9164 B, XL) | 0x8d650 prelude; 0x20 B config record with name view and `atexit(0x253430)`; shapes `E<261b50>ifA`, `E<261f10>ifA`, `<323d0>QfA`, `N<4b560>fA`; 87 releases / 54 atexit records / 7 functor fix-ups | new row kinds in the generator for those four shapes + the 0x8d650 prelude; after 501 the empty-view row kind exists |
| 454 (0x483e40, M) | RB erase `0x482a90` not modelled | model the erase as a native RB-tree contract; do not fake |
| 495 (0x268ee0, L) | one unmodelled helper `0xa38960` | model 0xa38960 first |
| 537 (0x1324c0, L) | 40 calls to unmodelled `0x157e90` (203 B) + 40 rip stores | model 0x157e90 (looks like a store/register helper) |
| 532 (0x143f50, L) | 20 atexit tail records after 21 name views; no ret (tail jump to `__cxa_atexit`) | tail-jump row kind |
| 452, 524, 568, 446, 556, 530, 505, 510, 462, 468, 552, 567, 554 | see `docs/STARTUP_GAP_SURVEY.md`: indirect-call "other" sites (452 four register calls), unmodelled helpers (0x20540, 0x20640, 0x210e00, 0x16a380, 0x6af60, 0x3c2a00), 505 two un-lifted callees (0x1621b0, 0x162970), 552 177 rip stores, 554 is 112 KB (XL, 22 unmodelled helpers) | per-slot, not attempted |

## 7. Paste-ready HANDOFF / TASKS text

HANDOFF: batch 20 (CPU-01-AOT-CFG-SCALE-01-BATCH20), 2026-11-03. Slots 445 (79 FNV hash-to-dword stores), 501 (string variable with empty 0xf730 view) and 494 (allocator pools,
builders lifted) wired. Startup coverage 551 -> 554 of 573 (working tree; HEAD 460), 19 unregistered, first 446 / 0x4c92a0. ctest 660/660, run_all_checkers 868/0/0, audit OK. 570 and 497 triaged
(not attempted, 0 tries); 454 stays blocked on RB erase 0x482a90. Next exact action: 570 hand-written model (see section 6), then 495 (0xa38960) and 537 (0x157e90).

TASKS: CPU-01-AOT-CFG-SCALE-01: 554/573; attempts 0 failed on every open slot; open: 570, 497, 454 (BLOCKED), 495, 537, 532, 452, 524, 568, 446, 556, 530, 505, 510, 462, 468, 552, 567, 554.

## 8. Paste-ready evidence rows (E1380+)

| E1380 | 2026-11-03 | hash_store_bytes.py: closed byte decoder for hash-rodata-string-then-store-dword runs (lea/mov/call 0xa2ddc0/store/guard load only; anything else refused); generator hstab row kind added, row accounting still a hard error | docs/checkpoints/2026-11-03-startup-batch-20.md | STATIC_ANALYSIS_VERIFIED |
| E1381 | 2026-11-03 | slot 445: 0x4cfe80 wired (ctor_slot_445_4cfe80, hash_store_table), check_ctor_slot_445 OK (79 hash stores, 232 pins, mutation 724 ELF + 67 native rejected; tamper elf/native exit 1), ctor_slot_445 test passed | docs/checkpoints/2026-11-03-startup-batch-20.md | STATIC_ANALYSIS_VERIFIED + FIXTURE_VERIFIED |
| E1382 | 2026-11-03 | slot 501: 0x24db80 wired (ctor_slot_501_24db80, family_label_item_26800), string variable with empty 0xf730 view, check_ctor_slot_501 OK (324 pins, mutation 857 ELF + 103 native rejected; tamper elf/native exit 1), ctor_slot_501 test passed | docs/checkpoints/2026-11-03-startup-batch-20.md | STATIC_ANALYSIS_VERIFIED + FIXTURE_VERIFIED |
| E1383 | 2026-11-03 | slot 494: 0x26f7c0 wired with builders 0x26e340 / 0x26e770, reserve 0x26e5f0 / 0x26ea10 and push-grow 0x26e4d0 / 0x26e8f0 lifted (ctor_slot_494_26f7c0); 14 helper bodies hash-pinned; dtors 0x269590 / 0x269610 modelled and tested, identity tags in the run; allocator thunks 0x17070 / 0x170b0 are hooks; check_ctor_slot_494 OK (tamper elf/native exit 1), ctor_slot_494 test passed | docs/checkpoints/2026-11-03-startup-batch-20.md | STATIC_ANALYSIS_VERIFIED + FIXTURE_VERIFIED |
| E1384 | 2026-11-03 | startup gap survey after batch 20: working tree 554/573, 19 open, cliff 446 / 0x4c92a0; 570 and 497 triaged (0 tries); 454 stays blocked on RB erase 0x482a90; next blockers 495 (0xa38960), 537 (0x157e90) | docs/checkpoints/2026-11-03-startup-batch-20.md | STATIC_ANALYSIS_VERIFIED |
| E1385 | 2026-11-03 | batch20-verification: ctest 660/660 in local/build/startup-b20; check_entry_map OK 2298 constants; check_ctor_slots OK 554 pairs; audit_publish_paths --all OK 4110 paths; run_all_checkers 868 passed / 0 skipped / 0 failed | docs/checkpoints/2026-11-03-startup-batch-20.md | ARTIFACT_VERIFIED (tooling) |
