# Audit of the reconstructed `native/core/startup_ctors.cpp` rows (batches 12-15), 2026-11-02

Agent: audit worker (Claude Sonnet 5.5), read-only audit. **No source file was changed**: no discrepancy was found, so no fix was made.
No commit, tag or push. HANDOFF.md, docs/TASKS.md and docs/EVIDENCE.md were NOT edited; paste-ready text is at the end
(EVIDENCE ids E1270+; re-check the tail before pasting, E1264 was the last one seen).

Subject: the "Risk: startup_ctors.cpp was reconstructed" section of `docs/checkpoints/2026-11-02-startup-batch-15.md`.

## Result

**All 20 re-inserted rows are OK.** The reconstructed file differs from the untouched batch-14-state backup
(`startup_ctors.cpp.bak`, 521 rows) by exactly 20 added lines and nothing else. The full startup sequence completes 541/541 rows with
`init_failed == false`, and every stored `slotN_result.ok` (151 results) is true. One low-severity observation (identity cell for
0x2dfcbd0, below) is recorded as "suspicious (informational)"; it does not affect correctness of any gate.

## Method

1. **Row set.** `diff startup_ctors.cpp.bak native/core/startup_ctors.cpp` shows exactly 20 `+` lines, for slots
   358, 370, 381, 390, 391, 399, 402, 418, 422, 453, 464, 469, 493, 503, 508, 511, 513, 561, 571, 572 (batches 12: 358/391/381/561/399/418;
   13: 370/469/513/508; 14: 390/402/571/572/453/464; 15: 493/422/511/503). This equals the set in the task and in the batch 12-15 checkpoints.
2. **Host-input inventory (a).** For each of the 20 slots, extracted every non-`rNN` member of `SlotNObjects` and every `objects.<...>`
   host access in `core/ctor_slot_<n>_*.cpp`. Exactly nine slots carry host inputs: `tree_alloc` (390, 511), `record_registry` + `record_alloc`
   (453, 464, 513), `external_target_0` (469, 571, 572), `source_bits` (508). The other eleven slots read no host input (only padding `gapNN` fields).
3. **Generator intent (b).** `tools/lift/startup_gen/wire.py` emits `pre=''` by default; no `mk_<n>.py` carries a `pre` (the host-input `pre`
   statements were always hand-added). Cross-checked against the written batch records: batch 13 (513: `record_registry` + `default_slot513_record_alloc()`;
   469: `external_target_0 = &s.slot518.var`), batch 14 (571/572: `external_target_0 = &s.global_2dfcbd0_standin`; 453/464 use `s.record_registry` +
   `default_slot<n>_record_alloc()`; 390 tree header with host allocator).
4. **Neighbouring rows (c).** Compared against rows from the backup that use host hooks (306, 309, 420) and plain rows (337, 339, 340, 349 ...): same row shape
   `{rva, idx, [](StartupState& s) noexcept { <pre> s.slotN_result = init_slotN_<rva>(s.slotN, s.concmd[, s.debug_menu]); if (!s.slotN_result.ok) s.init_failed = true; }}`.
5. **Structural script.** For each of the 20 rows: row RVA == header file RVA == `init_slotN_<rva>` name; every `s.slotNNN` / `init_slotNNN_` /
   `default_slotNNN_` token uses the row's own N; menu flag (`s.debug_menu` present) == init signature takes `DebugMenuRegistry&`; `SlotNObjects`/`SlotNResult`
   members present in `startup_ctors.hpp`; all 541 indices strictly ascending and unique. All true. `tools/lift/check_ctor_slots.py` re-verified
   all 541 RVA/index pairs against the pinned ELF constructor table (digest `c10f5af9...afc2c`).
6. **Test-level check** (scratch program, not in the repo because adding a test needs a CMake edit; see below): runs the full
   `recovered_constructors()` sequence, asserts `r.ok()`, `!callback_failed`, `completed == 541`, `!init_failed`, `slotN_result.ok` for all 151 stored results;
   asserts the host-input state of each re-inserted row; then **replays each of the 20 slots standalone** the way its `test_ctor_slot_<n>.cpp` `section_alone`
   sets it up (fresh `ConCmdRegistry`/`DebugMenuRegistry`, same host inputs) and requires equal `ok`, equal `rows_done`, and an equal count of atexit
   records whose object lies inside the slot's own Objects.
7. Build and tests in the audit build directory `local/build/audit-b15` (fresh configure, ASan+UBSan on, `-Werror`).

## Per-row verdicts

Legend: host input = what the row's `pre` must supply; verdict OK means row, pre, menu flag, init signature and startup-vs-standalone state all match.

| slot | rva | idx | menu flag | host input / `pre` in row | matches standalone test setup | startup rows_done == standalone | verdict |
|---|---|---|---|---|---|---|---|
| 358 | 0x786f50 | 358 | yes (menu rows) | none | n/a | 9 == 9, atexit_own 9 == 9 | OK |
| 391 | 0x68ebc0 | 391 | yes | none | n/a | 4 == 4, 4 == 4 | OK |
| 381 | 0x6cd5b0 | 381 | no (header: "No menu rows") | none | n/a | 17 == 17, 11 == 11 | OK |
| 561 | 0x3ac00 | 561 | yes | none | n/a | 9 == 9, 9 == 9 | OK |
| 399 | 0x632ca0 | 399 | yes | none | n/a | 31 == 31, 22 == 22 | OK |
| 418 | 0x572a10 | 418 | no (cells only) | none | n/a (cells-only page, zero atexit records, same in both) | 179 == 179, 0 == 0 | OK |
| 370 | 0x720b50 | 370 | yes | none | n/a | 232 == 232, 222 == 222 | OK |
| 469 | 0x394a20 | 469 | yes | `external_target_0 = &s.slot518.var` | standalone test uses a non-null identity (`g_external_0`, "stands for the object at 0x2ee65e8"); `Slot518Objects::var` is the 0xd8 B object at exactly 0x2ee65e8 (`kSlot518VarRva`); null is refused in the standalone test | 19 == 19, 19 == 19 | OK |
| 513 | 0x1d0770 | 513 | yes | `record_registry = &s.record_registry`, `record_alloc = default_slot513_record_alloc()` | standalone: fresh cell + `default_slot513_record_alloc()` | 15 == 15, 15 == 15 | OK |
| 508 | 0x2241e0 | 508 | no | none (`source_bits` left at the ELF-image default) | standalone overrides with marker inputs only to prove the input path; the default equals `kSlot508SourceImageBits` (asserted in the test, and unchanged after startup); batch-13 record documents this as the intended host-input default | 12 == 12, 12 == 12 | OK |
| 390 | 0x691c50 | 390 | no | `tree_alloc = default_empty_tree_alloc()` | standalone: `allocate_17070 = test_tree_alloc` (non-null); empty allocator refused in standalone test | 7 == 7, 7 == 7 | OK |
| 402 | 0x610430 | 402 | yes | none | n/a | 14 == 14, 14 == 14 | OK |
| 571 | 0xc780 | 571 | yes | `external_target_0 = &s.global_2dfcbd0_standin` | standalone: non-null identity (`g_external_0`); null fails closed | 44 == 44, 44 == 44 | OK (see observation 1) |
| 572 | 0x1a20 | 572 | yes | `external_target_0 = &s.global_2dfcbd0_standin` | standalone: `o.external_target_0 = &g_external_0`; null refused | 8 == 8, 8 == 8 | OK (see observation 1) |
| 453 | 0x48ac10 | 453 | no | `record_registry = &s.record_registry`, `record_alloc = default_slot453_record_alloc()` | standalone: fresh cell + default alloc; null registry / empty alloc refused | 1 == 1, 1 == 1 | OK |
| 464 | 0x3db070 | 464 | no | `record_registry = &s.record_registry`, `record_alloc = default_slot464_record_alloc()` | same | 2 == 2, 2 == 2 | OK |
| 493 | 0x277560 | 493 | yes | none | n/a | 6 == 6, 6 == 6 | OK |
| 422 | 0x56b980 | 422 | no | none | n/a | 13 == 13, 13 == 13 | OK |
| 511 | 0x1d90c0 | 511 | yes | `tree_alloc = default_empty_tree_alloc()` | standalone: `test_tree_alloc`; both trees non-null and distinct after startup, `size == 0` | 10 == 10, 10 == 10 | OK |
| 503 | 0x247f00 | 503 | yes | none | n/a | 15 == 15, 14 == 14 | OK |

Whole-sequence results (scratch test, ASan+UBSan): `rows=541 completed=541 blocked=0 init_failed=0`, `slotN_result.ok true for 151 stored results`,
`audit: 0 failure(s)`. Registry sharing: exactly one `0xa81b80` first-use atexit record across 453/464/513 (the shared process registry cell is initialised
once, `guard != 0`), as the original's function-local static requires; standalone tests each use a fresh cell, so each records it once. This is the only
intended difference between standalone and startup state, and it is the correct one.

## Observations (not discrepancies)

1. **Identity cell for 0x2dfcbd0 is not shared with slot 306 (suspicious, informational).** Slots 571/572 point their 0x26f90 items at
   `s.global_2dfcbd0_standin` (a `StartupState` member declared in the untouched `startup_ctors.hpp`, documented in batch-14), whereas slot 306's item #45
   targets its own `slot306.command_target_cell` (`ctor_slot_306_a633b0.cpp:177-179`). The three represent the same opaque original global, but are three distinct
   native addresses. Nothing reads these targets (stored by pointer only), so no gate is affected. If a later consumer dereferences 0x26f90 targets, route all
   three to one cell (needs a change to slot 306's model, outside `startup_ctors.cpp`). Left unchanged.
2. **Omission of a `pre` is detectable by `init_failed`, a wrong `pre` is not.** For the refusing inputs (`tree_alloc`, `record_registry`/`record_alloc`,
   `external_target_0`), a missing `pre` makes the slot return `ok == false`, which the row converts to `init_failed`; the existing coverage test would then fail. A *wrong*
   but non-null value (e.g. a different identity for 469's target) passes every existing test. The scratch test closes that gap with pointer-identity asserts
   (469 -> `&slot518.var`, 571/572 -> `&global_2dfcbd0_standin`, 453/464/513 -> `&record_registry`). Suggest adding these asserts to `test_startup_ctors.cpp`/
   `test_startup_ctor_coverage.cpp` (integrator decision; not done here because only `startup_ctors.cpp` was in scope).
3. 508's `source_bits` default is the ELF initial image for the four `.data` globals (0x1604918..0x1604924) and zero for the `.bss` global 0x2ee8820; no registered
   constructor writes any of these addresses (grep over `native/` and `tools/lift/`), so the default equals the value in effect when the original runs at this point of the
   sequence. This is an inference from the absence of writers, not an execution result.
4. The existing `test_startup_ctors.cpp` re-runs every row on the already-initialised state (the "leaf-fixture coverage" loop) and still passes; the shared registry cell
   and duplicate-variable handling therefore tolerate a second pass. Not a regression.

## Verification (STATIC_ANALYSIS_VERIFIED + FIXTURE_VERIFIED; nothing original was executed)

| command | result |
|---|---|
| `diff <scratch>/startup_ctors.cpp.bak native/core/startup_ctors.cpp` | exactly 20 added rows, 0 other changes |
| `cmake -S native -B local/build/audit-b15 && cmake --build local/build/audit-b15 -j 12` | exit 0 |
| `ctest --test-dir local/build/audit-b15 -j 12 --output-on-failure` | 100% passed, 637/637 |
| `python3 -I tools/lift/check_ctor_slots.py` | OK, 541 RVA/index pairs verified against the ELF table, digest c10f5af9...afc2c |
| `SOTC_NATIVE_BUILD=local/build/audit-b15 python3 -I tools/lift/check_ctor_slot_<n>.py` for the 20 slots | 20/20 exit 0 |
| scratch audit program (`audit_startup_rows.cpp`, linked to `local/build/audit-b15/libsotc_core.a`, same flags as the repo tests, `SOTC_SLOT23_NAMES_FIXTURE` set) | 0 failures; 20/20 standalone-parity blocks match |

Scratch program (kept outside the repo): `/private/tmp/claude-501/-Volumes-Extra-SoC-Mac-Native/8f859ae1-38f7-4a5f-86f0-4b01f8bd446d/scratchpad/audit/audit_startup_rows.cpp`.
Build command: `c++ -std=gnu++20 -arch arm64 -Wall -Wextra -ffp-contract=off -fsanitize=address,undefined -fno-omit-frame-pointer -I native audit_startup_rows.cpp local/build/audit-b15/libsotc_core.a`.
If the integrator wants it as a permanent test, add `add_executable(test_startup_ctor_rows tests/test_startup_ctor_rows.cpp)` linking `sotc_core`, with the slot-23 fixture
`ENVIRONMENT` as the other startup tests have; the parity pattern per slot is:

```cpp
auto o = std::make_unique<SlotNObjects>(); ConCmdRegistry creg; DebugMenuRegistry menu; RecordRegistryCell cell{};
/* same host inputs as test_ctor_slot_N.cpp section_alone */
const auto res = init_slotN_<rva>(*o, creg[, menu]);
CHECK(res.ok && st.slotN_result.ok && res.rows_done == st.slotN_result.rows_done);
CHECK(atexit_in(creg, *o) == atexit_in(st, st.slotN));   // atexit records whose object lies inside the slot's Objects
```

## Paste-ready text

HANDOFF.md:
> 2026-11-02 startup-ctor audit (read-only): the 20 re-inserted rows of native/core/startup_ctors.cpp (358, 370, 381, 390, 391, 399, 402, 418, 422, 453, 464, 469,
> 493, 503, 508, 511, 513, 561, 571, 572) all verified OK against the slot headers/cpps, generator intent and the batch 12-15 records; the file differs from the batch-14 backup by exactly those
> 20 rows. Whole startup: 541/541 rows, init_failed false, 151/151 stored slotN_result.ok; every re-inserted slot matches its standalone replay (ok, rows_done, own atexit count).
> Build exit 0, ctest 637/637, check_ctor_slots OK, 20 per-slot checkers exit 0. No source change. The batch-15 "reconstructed" risk is retired.
> Open (informational): 571/572 target `global_2dfcbd0_standin` while slot 306 targets its own `command_target_cell`; same original global, three native addresses, nothing dereferences them.
> Suggested: add pointer-identity asserts (469 -> &slot518.var, 571/572 -> &global_2dfcbd0_standin, 453/464/513 -> &record_registry) to test_startup_ctors.cpp.

docs/TASKS.md: CPU-01-STARTUP-CTORS-AUDIT PASSED (read-only audit of the reconstructed startup_ctors.cpp rows; no discrepancies).

docs/EVIDENCE.md:
- E1270 STATIC_ANALYSIS_VERIFIED: startup_ctors.cpp equals the batch-14-state backup plus exactly 20 rows (batches 12-15); row RVA, index, init function, menu flag and host-input `pre` of each cross-checked against the slot header/cpp and the generator; `check_ctor_slots.py` verifies 541 RVA/index pairs against the pinned ELF table (digest c10f5af9e35e1299f1189f666838fb52859c14879cfe83b66c62f967513afc2c).
- E1271 FIXTURE_VERIFIED: full startup sequence 541/541, `init_failed == false`, `slotN_result.ok` for 151 stored results (ASan+UBSan scratch program linked to libsotc_core.a, slot-23 fixture set).
- E1272 FIXTURE_VERIFIED: standalone-parity replay of the 20 re-inserted slots (ok, rows_done, own-object atexit count equal to the startup run); host-input identities asserted (469 -> &slot518.var, 571/572 -> &global_2dfcbd0_standin, 453/464/513 -> &record_registry with the default record allocators, 390/511 tree allocators non-null, 508 source_bits at the ELF-image default).
- E1273 FIXTURE_VERIFIED: audit build `local/build/audit-b15` exit 0, ctest 637/637, 20 per-slot checkers exit 0. Record: `docs/checkpoints/2026-11-02-startup-ctors-audit.md`.
- E1274 STATIC_ANALYSIS_VERIFIED (informational): 0x2dfcbd0 has three native stand-ins (`global_2dfcbd0_standin` for 571/572, `slot306.command_target_cell` for 306); no consumer dereferences them.
