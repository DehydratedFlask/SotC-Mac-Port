# Checkpoint 2026-11-02: helper callee models, part 3 (CPU-01-HELPER-CALLEE-MODELS-03, worker helper-d)

Agent: helper-d worker (Claude Sonnet 5.5). Workers do not edit HANDOFF.md / docs/TASKS.md / docs/EVIDENCE.md; the integrator pastes the sections below.
Nothing was committed. Build dir `local/build/helper-d`. Attempt count: 1, no failed tries (no escalation). Labels: STATIC_ANALYSIS_VERIFIED +
FIXTURE_VERIFIED. No original RVA was executed; no boot, audio or save claim. `native/core/startup_ctors.*`, `ctor_slot_*`, count pins, survey / G1 /
entry_map / FOLDERS and `tools/lift/startup_gen/` were not touched; no slot was registered.

## Premise check (batch 11 "unknown callee" list, 17 addresses)

The survey names callees by address; several of them are native already, one is a libc import, and some are the slots themselves. Re-measured from
live files (`grep` of `native/core`, the FDE table, whole-image direct-call scan of `[0, 0xf90000)`):

| address | what it is | status |
|---|---|---|
| 0xa81700 | 190 B named-record registrar (+ 0xa81840 push 265 B, 0xa82050 reserve 245 B, 0xa81b50 ctor 34 B); 48 direct call sites in the image, 20 in slots 497 / 513 | **newly modelled** |
| 0x8d650 | 7 B leaf `mov dword [rdi], 1 ; ret`; one direct call site in the whole image (slot 497, 0x262ab1) | **newly modelled** |
| 0xf8f630 | PLT thunk of the libc `strncpy` import (GOT 0x1600108, NID `6sJWiWSRuqk` recomputed); not game / Sony code; 51 call sites in the image | **contract documented + ISO C model + slot 503's cell block** |
| 0x56b4b0 | 138 B node-reference builder; direct callers 0x56c39d (slot 422) and 0x691cc9 (not modelled here) | **newly modelled** (all callees already native) |
| 0x4b560 | 233 B entry register-once + queue | already native (`lane_m5_app_entry`), **null-singleton path only**: the singleton cell 0x2ea8978 is stored only by the owner constructor 0x4a690 which drains this queue later, so slot 497's three calls (0x264c32 / 0x264d18 / 0x264dfe) take the modelled path (INFERENCE, inherited from slots 492 / 543) |
| 0x26800 | label item ctor | already native (`init_531d60_post_base_*`, `init_slot326_label_item_26800`) |
| 0x26b60 | 17 call-site family | already native (`mid_255340`, `ctor_slot_306/326/329`) |
| 0x257a0 | string variable ctor | already native (`string_var_257a0`) |
| 0x3a980 | unbounded-int ctor wrapper | already native (`helper_3a980`, helper-b) |
| 0x4c8f40 | 436 B local ranged-int wrapper | already native (row of `helper_620e10_local_int_var`, helper-c) |
| 0x56b980 | **startup slot 422 itself** (2682 B), listed as a callee | not a helper |
| 0x4c92a0 | **startup slot 446 itself** (2334 B), listed as a callee | not a helper |
| 0x3ac00 | **startup slot 561 itself** | not a helper |
| 0x24db80, 0x394a20 | **slots 501 / 469 themselves** (survey callee lists name the slot being analysed) | not helpers |
| 0x274730 | 661 B bool-variable constructor with `call [tag+...]` functor handlers over the class pair `0x1594208` / `0x159ca60` (slot 422, call at 0x56c248) | **deferred** (see below) |
| 0x3be2d0 | 192 B tree lookup (`0xf8f500` compare, node `+0x40` test) that on a miss calls the allocating insert `0x3c0840` and stores the key | **refused** (see below) |

So only four of the 17 addresses were genuinely missing bodies (0xa81700, 0x8d650, 0xf8f630 contract, 0x56b4b0); 0x274730 and 0x3be2d0 are real but not
done. The slot-level blockers of 561 / 508 / 501 / 469 (and the helper side of 446 apart from 0x3be2d0 / 0x52a990) are not helpers and cannot be unblocked by new
helper models.

## What was added (new files only, plus one appended CMake stanza)

| model | files | what it covers |
|---|---|---|
| 0x8d650 | `native/core/helper_8d650_store_one.{hpp,cpp}`, `native/tests/test_helper_8d650.cpp`, `tools/lift/check_helper_8d650.py` | `store_one_8d650(uint32_t& cell)`: `*cell = 1`, exactly four bytes written |
| 0xf8f630 | `native/core/helper_f8f630_strncpy.{hpp,cpp}`, `native/tests/test_helper_f8f630.cpp`, `tools/lift/check_helper_f8f630.py` | `strncpy_f8f630` (ISO C: copy to NUL, pad with NULs to exactly n, no terminator when the source fills n); `NullNameCell247f47{u32 id; char name[16]}` and `store_null_name_cell_247f47` (id = 0, `strncpy(name, "NULL", 15)`, byte 0x13 = 0) |
| 0x56b4b0 | `native/core/helper_56b4b0_node_ref.{hpp,cpp}`, `native/tests/test_helper_56b4b0.cpp`, `tools/lift/check_helper_56b4b0.py` | `NodeRef56b4b0 {HashedStr id; Object14c9a0* node; u8 flag}` built from the already-native pieces: `static_empty_hashed_2ed3640`, `copy_hashed_14c3a0`, `getter_14d880`, `add_ref_14ccb0`. Reference counts: first build adds 2 (getter first use + tail), later builds add 1 |
| 0xa81700 | `native/core/helper_a81700_named_record.{hpp,cpp}`, `native/tests/test_helper_a81700.cpp`, `tools/lift/check_helper_a81700.py` | `register_named_record_a81700(cell, record, name, kind, flag, alloc)`; `construct_record_registry_a81b50`; `vec_reserve_a82050` (capacities from empty 1, 2, 3, 4, 6, 9, 13, 19, 28, 42 via the libc++-style 1.5x rule); process cell accessor `record_registry_cell_32fd398()` |
| shared | `tools/lift/gen_helper_pins_d.py` (generator, offline `objdump -d`), `tools/lift/helper_callee_pins_d.py` (266 lines of per-instruction byte pins; checkers merge it with `lib.pins.PINS.update(pins_d.PINS)`), `native/CMakeLists.txt` (stanza appended at the very end with `>>`: `target_sources(sotc_core PRIVATE ...)`, four `test_helper_*` executables, four ctests `helper_8d650`, `helper_f8f630`, `helper_56b4b0`, `helper_a81700`) | |

### 0xa81700 contract (what the native model decides and what it records)

`0xa81700(rdi = record, rsi = const SharedStr* name, edx = kind, cl = flag)`:

- record `+0x00..0x1f` = copy of `*name` (16 B view, aux, ctl) with `lock inc qword [ctl]` when ctl is non-null; `+0x20` dword = kind; `+0x28` qword = 0; `+0x30` byte = 0;
  `+0x31` byte = flag. Bytes `+0x24` and `+0x32..0x37` are never written (the fixture poison proves the model leaves them).
- Process registry (function-local static at 0x32fd398, guard byte 0x33053f0): first use zeroes `+0x08` (16 B), `+0x18`, `+0x28` (32 B), `+0x50` (dword) -- the same stores as the
  stand-alone 0xa81b50 -- then `__cxa_atexit(0xa81b80, registry, DSO 0x1604218)`. The model reports this as `registry_initialised_now` (atexit pending); the destructor is identity only.
- Tail `jmp 0xa81840`: push the record into vector A (`+0x08/+0x10/+0x18`) and vector B (`+0x28/+0x30/+0x38`); each is `if (end == cap) reserve(1); *end++ = record`. 0xa81840 also
  has a branch for an element aliasing the vector's own storage; the pushed value is a stack copy of the record pointer, so that branch is unreachable here and is not modelled.
- `0xa82050` reserve: growth `new = cap/2 + cap` (0 when `max - cap/2 < cap`), `max(new, size + n)`, allocation through `alloc_17070(new * 8)` (tag 1, align 8, flags 1),
  `memmove`, old block through `free_170b0` (tag 1). The two exits `0xa82150` (length error) and `0xf8f5f0` (import, NID `eT2UsmTewbU`, name not recovered) are called and followed by `ud2`; that they throw
  `std::length_error` / `std::bad_alloc` is INFERENCE. The model returns `length_error` / `bad_alloc` refusals instead of continuing.
- Allocator policy (differs from the original on purpose): the original has no check at all (a null allocation would fault). Here a missing allocate/deallocate hook, a null result, or an
  oversize request returns `ok == false` with a refusal and leaves the failing vector untouched; the record itself and the registry initialisation are always done first, and a vector A
  push that already happened is not rolled back (the original has no rollback either). A failing deallocate hook reports `free_failed` after the state update (block leaked).
- A host must supply `AllocDispatchContract` hooks for every reachable call. Without them the first registration reports `no_allocator`.

### Derived call-site tables (re-derived from bytes by the checker, not asserted)

| slot | calls | records (rip lea) | kind (edx) | flag (cl) |
|---|---|---|---|---|
| 497 (0x262a80) | 0x264938, 0x264995, 0x2649eb, 0x264a41, 0x264a97, 0x264aed, 0x264b43 | 0x2eee620, 0x2eee658, ... stride 0x38 ..., 0x2eee770 | 0, 1, 1, 1, 1, 1, 1 | 1 x7 |
| 513 (0x1d0770) | 0x1d07db ... 0x1d0b4b (13) | 0x2ee6d00 ... stride 0x38 ... 0x2ee6fa0 | 0, 0, 0, then 1 x10 | 0 x13 |

Every site passes a stack `SharedStr` with `ctl == nullptr` (the 16-byte vmovups of zero covers aux and ctl) and releases it with 0xf590 afterwards, so the ctl increment never fires at these sites.
Slot 497 also stores 0x780 / 0x438 / 0 into the three dwords after the 0x8d650 cell 0x2eec4d8 (pinned).

## Commands and results

```sh
cmake -S native -B local/build/helper-d
cmake --build local/build/helper-d -j 12 --target test_helper_8d650 test_helper_f8f630 test_helper_56b4b0 test_helper_a81700   # exit 0, -Wall -Wextra -Werror
ctest --test-dir local/build/helper-d -R '^helper_' --output-on-failure        # 11/11 passed (the four new + seven earlier helper tests)
python3 -I tools/lift/check_helper_{8d650,f8f630,56b4b0,a81700}.py             # exit 0 each
python3 -I tools/lift/check_helper_<rva>.py --tamper elf                       # exit 1 each ("sha256" mismatch)
python3 -I tools/lift/check_helper_<rva>.py --tamper native                    # exit 1 each ("missing native constant")
python3 -I scripts/audit_publish_paths.py --all                                # PUBLISH PATH AUDIT OK (4110 path(s))
```

Checker mutation self-tests (all mutants rejected, 0 survivors):

| checker | ELF byte / relocation mutants | spec-field mutants | native mutants |
|---|---|---|---|
| check_helper_8d650 | 31 | 11 | 9 |
| check_helper_f8f630 | 37 | 6 | 16 |
| check_helper_56b4b0 | 113 | 22 | 20 |
| check_helper_a81700 | 674 | 46 | 45 |

A full `cmake --build local/build/helper-d` (all targets) stops at `native/tests/test_ctor_slot_418.cpp:481` (a syntax error in another worker's in-progress file, not in any file of this task);
the four new targets and every `helper_` test build and pass independently. The full-suite counts therefore are NOT RUN here.

## Which slots this unblocks (callee side only; none is registered)

- **497**: unknown callees 0xa81700 and 0x8d650 are now modelled; 0x4b560 / 0x26800 / 0x26b60 / 0x257a0 / 0x3a980 were already native. Remaining risk: the slot is 9164 B and this task did not
  re-measure its other shape gaps; wiring still needs a slot owner that supplies `AllocDispatchContract` and the 7 records.
- **513**: unknown callee 0xa81700 (13 sites) is now modelled (0x3a980 was already native). The generator assertion about the description argument noted in batch 11 is slot-side and unchanged.
- **503**: the `0xf8f630` blocker is a libc import with ISO C semantics, and its first block (`store_null_name_cell_247f47`) is native. 503 is also refused for its command rows (381 / 391 / 503 need
  a command-row generator kind); that part is unchanged.
- **422**: 0x56b4b0 is modelled; 0x274730 is still open (and the description-assertion failure).
- **446**: no new unblock (0x3be2d0 refused, 0x52a990 not touched; 0x4c8f40 already native).
- **561 / 508 / 501 / 469**: not helper-blocked (see the premise check); no change.

## Refusals and deferrals (with evidence; none is a failed try)

- **0x3be2d0** (192 B, slot 446, two calls): a red-black-tree map lookup (compare via the import 0xf8f500, nil byte at node `+0x19`), then on a miss the allocating insert `0x3c0840` (not analysed,
  contains the allocation + rebalance) and `*result = key`. Modelling it needs a node-allocation contract plus the RB insert/fixup of 0x3c0840: a new mechanism, refused rather than faked.
- **0x274730** (661 B, slot 422): bool-variable constructor with class pair `0x1594208` / `0x159ca60` and indirect `call [tag+...]` functor handlers. Feasible with the existing functor-tag
  family proof pattern (helper_620e10 / 7ec160) but heavy; not started because it blocks a single slot that also has a slot-side assertion failure.
- **0x52a990** (10 B, `mov esi, 0x811c9dc5 ; jmp 0x161fb0`, called once by slot 446): FNV-1a seeded tail; not modelled here (needs 0x161fb0 confirmed native first).
- **Not modelled inside the new bodies**: the exit-time record unregistration `0xa81950` / `0xa819f0` and the registry destructor `0xa81b80` (identity tags); the aliasing-element branch of 0xa81840.

## Paste-ready text

### HANDOFF.md (checkpoint)

2026-11-02 helper-d worker, task CPU-01-HELPER-CALLEE-MODELS-03, attempt 1 (no failed tries). Premise check: of the 17 batch-11 "unknown callees", 4 were genuinely missing and are now modelled
(0xa81700 registrar + 0xa81840 / 0xa82050 / 0xa81b50, 0x8d650, 0x56b4b0, and the libc `strncpy` import contract 0xf8f630); 0x4b560 / 0x26800 / 0x26b60 / 0x257a0 / 0x3a980 / 0x4c8f40 were already
native; 0x56b980 / 0x4c92a0 / 0x3ac00 / 0x24db80 / 0x394a20 are slots, not callees. Files: `native/core/helper_{8d650_store_one,f8f630_strncpy,56b4b0_node_ref,a81700_named_record}.{hpp,cpp}`,
`native/tests/test_helper_{8d650,f8f630,56b4b0,a81700}.cpp`, `tools/lift/check_helper_{8d650,f8f630,56b4b0,a81700}.py`, `tools/lift/gen_helper_pins_d.py`, `tools/lift/helper_callee_pins_d.py`,
`native/CMakeLists.txt` (appended stanza), `docs/checkpoints/2026-11-02-helper-callee-models-3.md`. Commands: see the checkpoint. Results: 4 new targets build under -Werror, `ctest -R ^helper_` 11/11,
4 checkers exit 0 (mutants rejected 31+11+9, 37+6+16, 113+22+20, 674+46+45), `--tamper elf|native` exit 1, path audit OK (4110 paths). Observations vs inference: byte-pinned bodies, NID of the strncpy
import, derived (record, kind, flag) tables for 20 call sites are observations; the meaning of the records, the throw semantics of the two noreturn exits, and the register assignments between lea and call
(straight-line reading) are inference. Not done: 0x274730 (deferred), 0x3be2d0 (refused: allocating RB insert 0x3c0840), 0x52a990. Risks: a full build currently fails in another worker's
`test_ctor_slot_418.cpp`; wiring 497 / 513 needs an `AllocDispatchContract` host hook. Labels: STATIC_ANALYSIS_VERIFIED, FIXTURE_VERIFIED. Next exact action: wire slot 497 / 513 owners to
`register_named_record_a81700` (or refuse on missing allocator), then 0x274730 for slot 422.

### docs/TASKS.md

`CPU-01-HELPER-CALLEE-MODELS-03` | helper-d | DONE for 0xa81700 / 0x8d650 / 0x56b4b0 / 0xf8f630 contract | open: 0x274730 (deferred), 0x3be2d0 (refused, allocating RB insert), 0x52a990 | attempts 1 | evidence E1220..E1226.

### docs/EVIDENCE.md

| E1220 | 2026-11-02 | 0x8d650 (7 B, sha bf063ac8...a8fc): `mov dword [rdi],1 ; ret`; sole direct call site in the image is slot 497 (0x262ab1, rdi = 0x2eec4d8). `store_one_8d650`; 31 + 11 + 9 mutants rejected. STATIC_ANALYSIS_VERIFIED + FIXTURE_VERIFIED. |
| E1221 | 2026-11-02 | 0xf8f630 is the PLT thunk of libc `strncpy` (GOT 0x1600108, JMPREL JUMP_SLOT, NID 6sJWiWSRuqk recomputed with the same function that proves __cxa_atexit / memmove). Slot 503 call 0x247f47: cell 0x2eea480, `strncpy(+4, "NULL"@0xfc15f3, 15)`, byte 0x13 = 0. ISO C model `strncpy_f8f630`; 37 + 6 + 16 mutants rejected. |
| E1222 | 2026-11-02 | 0x56b4b0 (138 B, sha 725d88e5...30dcbd): guarded static HashedStr 0x2ed3640 (guard 0x2ed3668, atexit 0x95610, shared with 0x14c9a0), copy_hashed, getter 0x14d880, `[+0x28] = node`, `[+0x30] = 0`, tail add_ref 0x14ccb0. Slot 422 call 0x56c39d with rbx = 0x2f08928. Other direct caller 0x691cc9 (not modelled). 113 + 22 + 20 mutants rejected. |
| E1223 | 2026-11-02 | 0xa81700 (190 B) + 0xa81840 (265 B) + 0xa82050 (245 B) + 0xa81b50 (34 B) pinned per instruction; registry 0x32fd398 / guard 0x33053f0 / atexit(0xa81b80); reserve growth 1.5x, capacities 1, 2, 3, 4, 6, 9, 13, 19; allocator via 0x17070 / 0x170b0 hook contract; failures refuse, never fake. 674 + 46 + 45 mutants rejected. |
| E1224 | 2026-11-02 | 20 call sites re-derived from bytes: slot 497 (7 records stride 0x38 from 0x2eee620, kinds 0,1x6, flag 1) and slot 513 (13 records stride 0x38 from 0x2ee6d00, kinds 0,0,0,1x10, flag 0); all names are stack SharedStr with null ctl. Whole-image scan: 48 direct call sites of 0xa81700. |
| E1225 | 2026-11-02 | Survey premise correction: of the 17 batch-11 unknown callees, 6 are already native (0x4b560 null-singleton path, 0x26800, 0x26b60, 0x257a0, 0x3a980, 0x4c8f40), 5 are the slots themselves (0x56b980, 0x4c92a0, 0x3ac00, 0x24db80, 0x394a20), 4 were new bodies, 0x274730 deferred, 0x3be2d0 refused. |
| E1226 | 2026-11-02 | Refusals: 0x3be2d0 (RB-tree lookup + allocating insert 0x3c0840: new mechanism), 0x274730 (661 B functor-tag bool constructor, deferred), 0x52a990 (FNV seed tail, needs 0x161fb0). No fake call, no emulation. |

### INSTALL_LOG.md

No install, download or dependency change.

## Risks

- The allocation-failure behaviour intentionally differs from the original (which would fault); a host that ignores `ok == false` would see a record whose registry push did not happen.
- `std::atomic_ref` on the `SharedStr` control word assumes 8-byte alignment (the project's existing convention for `ctl`).
- The registry cell, its guard and the atexit registration are modelled as state plus a pending flag; no exit-time destructor exists natively, so records are never unregistered.
- The record meaning (kind / flag) is not recovered; only the byte layout and the call-site tables are proven.
- A full-tree build is currently red because of `native/tests/test_ctor_slot_418.cpp` (another worker); the helper targets were built and tested in isolation.
