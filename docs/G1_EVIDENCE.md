# G1 PARTIAL evidence pack — what the ARM64 harness proves, and what it does not

**Gate status: G1 PARTIAL — not PASSED.** Measured 2026-11-01 21:11 CDT (capture `f3d42240…`, see §7) on
`cursor/init-hash-551140-6aa93e8` + uncommitted work, Apple M4 Pro, macOS 27.0.1, Xcode 27.1.
Every claim below is labelled `STATIC_ANALYSIS_VERIFIED`, `FIXTURE_VERIFIED`, `PARTIAL` or
`NOT RUN`. Nothing in this pack was produced by executing the game, its `eboot.bin`, or any guest
instruction.

This supersedes the 2026-10-22 pack (`docs/checkpoints/2026-10-22-g1-partial-evidence-pack.md`,
private manifest `local/validation/g1-partial-20261022/`). The changelog at the end says what moved.

**E1431 update (2026-11-10):** the 803 B `0x560b60` collision/descriptor body (sha
`9f67f04f…`) and the 41 B `0x96f20` wrapper (sha `384d9d1f…`) now execute in the real
`0x531d60` run. `0x560b60` reports 13 calls, 4 host allocations, a non-null 0x136f0 object
and three buffers, `global28=0`, and the expected callbacks; its cleanup and `0x95580`
branches are fixture-covered. `0x96f20` forwards the exact ten arguments to `0x97a50`;
the 2,835 B callee is **not** lifted, and the host contract allocates the observed
0x1c0-byte object at `0x97af7` (448 B). `post_calls_executed` rises **12 → 14** and
`post_calls_blocked` falls **5 → 3**; the remaining blocked records are `0xa5cd90`,
`0xa5d990` and `0x5b9f90`. `post_5c64c0.cell` is non-null, but its BSS hashes are unchanged
and `0x97a50` plus the thirteen `0x560b60` helpers remain host seams. 275 registry
entries/status 0; full CTest **702/702**; checker sweep **911/0/0**; entry map **2,983 →
1,910 RVAs / 581 headers**. This does not pass G1: callee closure, whole-program
CFG/entry-map closure and a real game boot remain open.

**E1430 update (2026-11-10):** the 656 B `0x7f6f90` staffroll loader (sha
`6b68dce8…`) is native. Its 19 direct call sites, 37-token table, path/mode/fallback
formats and three basenames are pinned by `check_7f6f90_body.py`; stdio is libc and the
four `0x56cdb0`/`0x56ceb0`/`0x56cec0`/`0x56ce60` calls are an explicit host contract.
Both real `0x7f7220` invocations execute: mode 0 records `CENTER_FULL_SCREEN` at index 19
(19 B), mode 1 records `TARGET` at index 36 (7 B), both with `fopen=false`.
`post_calls_executed` rises **10 → 12** and `post_calls_blocked` falls **7 → 5**;
`post_calls_complete=false` remains because `0xa5cd90`, `0xa5d990`, `0x560b60`,
`0x5b9f90` and `0x96f20`/`0x97a50` do not execute. This supersedes the `0x7f6f90`×2 and
10/7 lines in E1429. 275 registry entries/status 0; full CTest **702/702**; checker sweep
**909/0/0**. This does not pass G1: callee closure, whole-program CFG/entry-map closure
and a real game boot remain open.

**E1429 update (2026-11-09):** the `0x5d5410` post-base group (`0x5d5410` 420 B,
`0x8be60` 547 B, `0x8f520` 269 B, `0x8f1d0` 171 B) is native and executes in the real
`0x531d60` run. The measured path configures both 0x30 B objects, writes the nine-qword state
block, and reports 2/3/3 nested calls with zero blocked. `post_calls_executed` rises **9 → 10**
and `post_calls_blocked` falls **8 → 7**; `post_calls_complete=false` remains because
`0xa5cd90`, `0xa5d990`, `0x560b60`, `0x5b9f90`, `0x96f20`/`0x97a50` and `0x7f6f90`×2 do not
execute. `0x12820` (bounded host arena) and `0x87f3e0 -> 0xf906d0` (refusing platform tail)
are explicit contracts and are unreached on the measured `a4` bit-1 path. 275 registry
entries/status 0; full CTest **702/702**; checker sweep **908/0/0**. This does not pass G1:
callee closure, whole-program CFG/entry-map closure and a real game boot remain open.

**E1428 update (2026-11-09):** `0x946880` (353 B) and `0x9469f0` (360 B) are now native
with explicit call contracts. Both observed post-base values (1 and 0) take the
invalid-handle branch, return status 0, and report 8 inner calls with lock balance 0;
`post_calls_executed` rises **7 → 9** and `post_calls_blocked` falls **10 → 8**. The
valid-handle `0x944de0` (399 B) branch remains a named contract. `post_calls_complete=false`,
275 registry entries/status 0; full CTest **702/702**. This still does not pass G1:
`0x944de0`, the remaining eight invocations, whole-program CFG/entry-map closure and a
real game boot remain open.

**E1427 update (2026-11-09):** every outgoing call of the three lifted post-base bodies is now a
recorded invocation. `sotc.init_window.v5` reports 17 records: 7 executed native leaves (five
one-byte `ret` targets, the 174 B `0x5c91e0` copy, the 14 B `0x5d06f0` query) and 10 named
blockers (`0xa5cd90`, `0x946880`×2, `0xa5d990`, `0x560b60`, `0x5b9f90`, `0x96f20`, `0x5d5410`,
`0x7f6f90`×2) with observed args and diagnostics. `post_chain_complete=true`,
`post_calls_complete=false`, 275 registry entries/status 0; full CTest **702/702**. This does not
pass G1: callee closure, whole-program CFG/entry-map closure and a real game boot remain open.

**E1426 update (2026-11-09):** the Init `0x531d60` post-base chain now executes native bodies at
`0x499f80`, `0x56dcb0`, `0x5c64c0`, `0x1290` and `0x7f7220`, supplies native empty bodies for
the five one-byte-`ret` sites (including `0x56dca0`), and observes `0xf90310` as a named splash-hide host service.
`post_chain_complete=true`; 275 registry entries/status 0; full CTest **702/702**. The helper
dispatches, host cells, `0x1290` singleton owners and Sony PLT remain explicit seams. This does
not pass G1: whole-program CFG/entry-map closure and a real game boot are still open.

## Machine-checked block

Regenerated by `python3 tools/lift/g1_evidence_pack.py --write`; verified by
`python3 tools/lift/check_g1_evidence_pack.py` (part of the `tools/lift/check_*.py` sweep). If a
constructor is registered or a frontier row is added, the check fails until the block is
regenerated — that is the point.

<!-- g1-evidence-pack:begin -->

```json
{
  "frontier": {
    "bases_table_level_only": 14,
    "bases_with_both_grades": 2,
    "graded_model_headers": 26,
    "model_headers": 36,
    "rows": 83,
    "site_provable_rows": 41,
    "site_provable_targets": 6,
    "table_bases": 20,
    "table_level_rows": 42,
    "table_level_targets": 27,
    "targets": 32
  },
  "private": {
    "elf_ctor_table": "verified: 573 qwords at 0x2dd70e0, sha256 5917cb85..., every row matches",
    "image_wide_indirect_calls": 46830,
    "image_wide_slot_calls": 28925,
    "strict_tied_sites": {
      "site_provable_intersection": 41,
      "sites": 41,
      "table_level_intersection": 0,
      "targets": 6
    }
  },
  "schema": "sotc-g1-evidence-pack/1",
  "startup": {
    "first_unrecovered_index": -1,
    "first_unrecovered_rva": "0x0",
    "fully_lifted_rows": 573,
    "next_unrecovered": [],
    "partial_rows": [],
    "registered_rows": 573,
    "remaining_rows": 0,
    "rows_registered_above_first_gap": 0,
    "table_base": "0x2dd70e0",
    "table_indexing": "backwards: entry i is the qword at 0x2dd70e0 - 8*i",
    "table_qwords": 573,
    "test_agrees": true,
    "vec_leaf_modules": 250
  }
}
```

<!-- g1-evidence-pack:end -->

## 1. What the ARM64 harness proves

### 1.1 It is an ordinary ARM64 Mach-O build with no guest image in it — `FIXTURE_VERIFIED`

```
$ file local/build/orch-r22/test_startup_ctor_coverage
local/build/orch-r22/test_startup_ctor_coverage: Mach-O 64-bit executable arm64
$ otool -L local/build/orch-r22/test_startup_ctor_coverage
	/usr/lib/libc++.1.dylib
	@rpath/libclang_rt.asan_osx_dynamic.dylib   # this build is ASan-instrumented
	/usr/lib/libSystem.B.dylib
```

The harness is C++ compiled by clang for arm64. It loads no x86-64 image, no `libSce*` module and no
firmware. The original executable is read **as bytes** by Python checkers (`local/re/eboot.elf`); it
is never mapped or run. `native/api/sotc_core.cpp:380` always returns
`SOTC_ERR_ENGINE_NOT_IMPLEMENTED`, so even the shipped core library refuses to claim a running
engine.

**What that proves:** the build is native, and the fixture binaries are not a guest-CPU runtime in
disguise.
**What that does not prove:** anything about behaviour of the original functions, or that a full
binary audit for forbidden runtimes was performed (it was not; this is a per-binary check).

### 1.2 The startup constructor table is recovered and byte-paired — `STATIC_ANALYSIS_VERIFIED`

- The original table is **573 qwords at `0x2dd70e0`, indexed backwards**: entry `i` is the qword at
  `0x2dd70e0 - 8*i`, terminated by the `-1` sentinel below entry 572. Its index-order byte string is
  pinned by sha256 `5917cb85…ab28`.
<!-- g1-evidence-pack:startup-prose:begin -->
- **573 of 573 rows are registered**, every `(RVA, index)` pair matches the pinned table;
  **no row remains unregistered and no row is PARTIAL** — all **573 rows are fully lifted**
  (the last open row, slot **554**, closed with batch 13; see `docs/STARTUP_SLOT_554_PLAN.md`).
- The C++ test `native/tests/test_startup_ctor_coverage.cpp` asserts those exact numbers
  (`slots.size() == 573`, `first_unrecovered_constructor_index() == -1`), so the source
  cannot drift from the pack silently in one direction.
  (These bullets are generated by `tools/lift/g1_evidence_pack.py --write` between the `startup-prose` markers; do not edit them by hand.)
<!-- g1-evidence-pack:startup-prose:end -->

<!-- g1-evidence-pack:ctor-check:begin -->
```
$ python3 -I tools/lift/check_ctor_slots.py
OK: … constructor table start=0x2dd70e0 count=573 sha256=5917cb85…ab28;
verified 573 registered RVA/index pairs (250 vec_*/leaf_* modules);
all 573 rows registered and fully lifted
```
<!-- g1-evidence-pack:ctor-check:end -->

Slots 309 (`0xa505e0`) and 312 (`0xa46130`) were registered this cycle. Both are **data-only
table rows**: a byte scan over every executable segment finds **0** `e8`/`e9` near call or jump sites
targeting either address (a superset scan, so a zero is not a disassembly artifact), and each has
exactly one relocation — the type-8 row at its table entry. Registration therefore says the native
row runs in the harness, not that a game path calls it.

### 1.3 Every lifted unit is a *model* or a *hook*, and its header says which — `FIXTURE_VERIFIED`

A **model** reproduces the observable net state of the original path and reports what it refuses to
perform. The example that defines the style is `core/ranged_bound_var_a4f4d0` (slot 309): it reports
`functor_slot_calls == 11` for the eleven indirect functor dispatches instead of pretending to
dispatch them, and writes nothing when the class is not pinned.

A **hook** is a named boundary with a contract, not a stub: a caller that cannot satisfy the
contract fails visibly instead of returning success. `0x2569d0` is the largest documented case —
only its registry block, its `RetailBuild`/mode classification and its constant property table are
native; the remaining bodies are hooks with recorded contracts.

Concrete boundaries measured this cycle:

| Unit | Modelled | Hooked / not modelled |
|---|---|---|
| slot 312 `0xa46130` | 30 direct calls, 49 rip operands, both int-variable classes, 8 atexit rows | five destructors, four callback tags carried as data |
| slot 309 `0xa505e0` | 12 objects, 12 atexit records, net state of 11 functor calls | three console-variable constructor interiors |
| `0x499f80` | all 8 calls, 9 stores, 1 read, 4 branch outcomes | the two CritSec callees stay hook-bounded host models |
| `0x2569d0` | registry block, build classification, constant property table | the large bodies behind 87 blocks |

### 1.4 Bodies are byte-checked against the pinned image, not remembered — `STATIC_ANALYSIS_VERIFIED`

Checkers re-derive bytes, FDE extents, digests, relocations and call targets from the pinned ELF and
are mutation-tested:

- `tools/lift/check_ctor_slot_312.py` — 1,250 B FDE, digest, all 30 callees, 49 rip operands, the
  single branch, the 8 atexit rows; **394 ELF + 145 native mutants rejected**, `--tamper` exits 1.
- `tools/lift/check_ctor_slot_309.py` — 334 ELF + 167 native mutants rejected.
- `tools/lift/check_startup_slot309_table.py` — table row, RELA, and an exhaustive `e8`/`e9`
  near-call scan over the file-backed image.
- `tools/lift/check_vtable_resolved_entries.py` — for every frontier row: slot qword, type-8 RELA
  addend, FDE start, entry-map record (`OK: 83 sites, 32 distinct targets`).

The fixtures are host C++ that exercise the models (`ctest` **533/533** on `local/build/orch-r22` at
capture time — the total moves as lanes land — and **24/24** for the 24-test G1 harness subset:
startup constructors, registry Init-hash chain, allocator front/page_*). A passing fixture proves
the model is self-consistent and fails on the mutations it was written for. It does **not** prove
the model equals the retail function.

## 2. What the harness does NOT prove

Stated plainly, because these are the claims most likely to be over-read:

1. **No guest execution.** Every number here is read from the image (`STATIC_ANALYSIS_VERIFIED`) or
   produced by host C++ (`FIXTURE_VERIFIED`). No x86-64 instruction was executed, and none is
   translated at run time.
2. **No runtime.** `sotc_core_start()` always returns `SOTC_ERR_ENGINE_NOT_IMPLEMENTED`
   (`native/api/sotc_core.cpp:380`). There is no engine loop, no job/fibre scheduler, no live
   service layer.
3. **No boot.** No cold boot of the game has occurred in any recorded run of this project
   ([ROADMAP](../ROADMAP.md): "The game has not yet been booted on this Mac").
4. **No Metal frame.** The PM4 decoder and render-op fixtures are unit-level; no frame has been
   submitted to a device (`G3 PARTIAL`).
5. **No audio.** Format work (SPU ADPCM banks, the `.sxd2` = ATRAC9 selector) establishes
   containers and a selector; no sample has been decoded into a playable stream and no audio device
   has been opened (`G2 NOT RUN`).
6. **No save/load.** No transactional save has been written or reloaded (`G2 NOT RUN`).
7. **No equivalence.** A model reproducing stores, calls and branch outcomes on the observed path is
   not proof that the retail function behaves identically outside that path.
8. **Registration ≠ semantic coverage.** See §3.

## 3. The evidence-grade split — the most important honesty point

`native/core/vtable_resolved_entries.hpp` holds **83 rows** that reach **32 targets** through **20
table bases** (91 / 37 / 21 before E1095 withdrew eight rows whose call sites read a different slot, see
item 6 of §5). They are not equally proven, and since E1066 every row carries its grade:

| Grade | Rows | Targets | What the row proves |
|---|---:|---:|---|
| `site-provable` | **41** | 6 | the call site alone forces the target |
| `table-level` | **42** | 27 | the slot ties the target; the call is **not** proven |

- **Site-provable** means the call is `call *slot(%reg)` and the register's *nearest* preceding
  definition is the RIP-relative load of that table. The pinned image then forces the target for
  that call.
- **Table-level** means the qword at `vtable + slot` is the target, one type-8 RELA addend at that
  address matches, and the target is an FDE start — but the call register's nearest definition is a
  member load with no static source, so the site may dispatch through a different table at run time.

Measured, not estimated: the 41 site-provable rows are **exactly** the 41 sites of
`local/validation/indirect-frontier/tied-sites-image-wide-strict.json` under nearest-definition-only
semantics, 0 table-level rows intersect it, and 0 of the 41 disagree with their header row. The tool's
own `rip_global_for` fallback scans *past* the nearest definition, which is how the legacy table-level rows
inherited a tie that does not hold for the call (and, for nine of them, the wrong slot: E1095).

**The rule that follows: only a site-provable row may drive native dispatch for its call; a
table-level row must not.** A table-level row remains a sound *target* record — the body model, the
ABI row and the fixture describe the function, not the call.

**The split is checked; its use is not.** `tools/lift/check_evidence_grades.py` re-derives
41 / 42 / 83 from the pinned ELF and the header and fails when
[INDIRECT_FRONTIER](INDIRECT_FRONTIER.md) drifts (`OK check_evidence_grades: 41 site-provable
(6 targets, 6 table bases) / 42 table-level (27 targets, 16 of 20 table bases) / 83 rows`). What is
**not** enforced is the consumer side: `ResolvedVtableCallSite` carries
`{site, vtable, slot, target}` and no grade field, so `find_resolved_vtable_call_site()` cannot
distinguish the 41 from the 42. Until the type carries the grade, "table-level rows must not drive
native dispatch" is a documentation rule at the point of use, not an invariant a dispatcher can be
made to respect.

The 26 per-target model headers relabelled in E1066 now state their grade; **10 of the 36** vtable
model headers still do not (`vtable_1594ae8_slot28_13d20.hpp`,
`vtable_1594dc0_slot10_4b0.hpp`, `vtable_1594dc0_slot20_20d60.hpp`,
`vtable_15953f8_slot_a0_a8_fios_reads.hpp`, `vtable_15953f8_slot18_31310.hpp`,
`vtable_15cf248_slot68_9b7b80.hpp`, `vtable_15de228_slot20_d7c4b0.hpp`,
`vtable_15e2028_slot2a0_e21420_tail.hpp`, `vtable_2dcb170_slot08_6e92c0.hpp`,
`vtable_2dd09b0_slot08_9804c0.hpp`). Six of those ten are site-provable targets, so a reader of the
header alone can understate them — never overstate, but still wrong.

## 4. Startup coverage: what the registered-row count means

<!-- g1-evidence-pack:startup-coverage:begin -->
| Metric | Value | Source |
|---|---:|---|
| Rows in the original table | 573 | `native/core/ctor_original_table.cpp` |
| Registered rows in `recovered_constructors()` | **573** | `native/core/startup_ctors.cpp` |
| Unregistered rows | **0** | idem |
| First unrecovered index / RVA ("the cliff") | none (all rows registered) | idem + pinned table |
| Next unrecovered indices | none | idem |

All 573 registered rows pair with the pinned table and every registered body is lifted
(573/573), the last open row (**554**) having closed with batch 13 (E1417).
Registration is still not semantic coverage of the whole startup chain.

**573/573 is *registration* coverage, not semantic coverage.** A registered row means the native row
executes in `run_constructors()` and its `(RVA, index)` pair is byte-paired with the original table.
Several registered rows call hooks inside their bodies (see §1.3), and a registered row whose
original is never called by the game is still registered. Do not read 100% as "100 percent of startup
is native".
<!-- g1-evidence-pack:startup-coverage:end -->

There is **no constructor cliff left**: no row is unregistered and none is PARTIAL. The final open row, slot **554**
(`0x6efe0`), closed on 2026-11-08 with batch 13 (`docs/STARTUP_SLOT_554_PLAN.md`, E1417). Earlier cliffs (445, 446, 428,
429, 314, 424 — including the 2,088-store table initialiser handled by `tools/lift/store_bytes_replay.py` and
`native/core/store_table.*`) were registered in earlier rounds; their history is in `docs/TASKS.md` and `docs/EVIDENCE.md`.
This closes **registration/lifting** coverage only: G1 stays **PARTIAL** because the runtime, boot, audio and save gates are
still open (§5 and `docs/VALIDATION.md`).

## 5. Why PARTIAL is the honest label: the programme's scale

Measured against the whole image, the recovered surface is small:

| Metric | Value | Command |
|---|---:|---|
| Unwind (FDE) functions | 55,790 over 15.84 MB | `tools/lift/cfg_census.py` |
| Direct/indirect call sites | 227,401 (179,920 / 47,481) | idem |
| Direct/indirect jumps | 99,147 (83,638 / 15,509) | idem |
| Init closure `0x531d60` | 608 functions, 105 frontier functions, 499 indirect sites | `tools/lift/indirect_frontier.py` |
| Dispatch closure `0x1bdf30` | 416 functions, 59 frontier functions, 122 indirect sites | idem |
| Union | 817 functions, 136 frontier functions, 562 indirect sites | idem |
| Image-wide `call *` sites | 46,830 (28,925 with a slot offset) | strict tie artifact |
| Proven by the call register alone | **41** (0.14 % of slot calls) | idem |
| Slot reads with an object base that has no static source | 27,408 | idem |

`docs/entry_map.json` covers on the order of **1,100 distinct guest RVAs — roughly 2 %** of those
55,790 functions — and it records *names*, not callable ABI: a presence in the map means a header
mentions the address, nothing more. The totals move with every lane, so quote the generated
`docs/entry_map.json` for them; this paragraph deliberately does not pin a number that changes hourly.

## 6. Unresolved edges (explicit)

1. **Slot 445 / `0x4cfe80`** — the current cliff, characterized (§4), NOT modelled or registered.
2. **The remaining unregistered rows** (the count and the rows registered above the cliff are in §1.2 / §4, generated from the tree),
   listed with their shapes in `docs/STARTUP_GAP_SURVEY.md`; slot 454 stays blocked on the red-black erase `0x482a90`.
3. **The 42 table-level frontier rows** — target records, not dispatch proofs, and not representable
   in the C++ type today (§3).
4. **10 of 36 vtable model headers** carry no grade (§3).
5. **Image-wide dispatch**: 46,830 indirect calls, only 41 proven. 27,408 sites dispatch through an
   object base with no static source.
6. **Store-to-load forwarding was attempted on the 91 rows that existed then and promoted none; it
   found nine wrong rows, which E1095 fixed.** The bounded pass
   (`tools/lift/store_to_load_forwarding.py`, one hop, same FDE, single reaching definition, static
   stored value) proved **0 of the 50** table-level rows: 40 have no store to the load location
   anywhere in their own function, 8 have a call between the store and the load, 1 has control flow
   between them, 1 loads through an indexed address. Worse for the table than "unproven": the nine
   rows that do have a store all read `TABLE + 0x10 + slot`, so the recorded target sat at the
   *wrong slot* for the site. E1095 re-derived all nine from ELF bytes
   (`tools/lift/check_frontier_vptr_correction.py`, 92 mutation probes): one row was corrected
   (`0x113c5`: `0x1595b60 + 0x28 -> 0x13d20`, still table-level) and eight were withdrawn because
   their real targets (`0x47860`, `0x31900`, `0x318f0`, `0x31520`) have no model. No row was
   promoted. The same pass over the 46,830 image-wide sites has not been run.
7. **MSL: 0 real shaders lowered.** `lowered: 0` over a 34,291-member `.csdr` population (64-member
   deterministic sample; 4 decoded, 27 decode-error, 41 compressed-prefix). The blocker is a
   per-shader ABI manifest, not decoding. This is a **G1/G3 boundary fact, not a G1 pass** — the
   decoder never became a Metal pipeline (`docs/MSL_OFFLINE.md`).
8. **Audio**: the `.sxd2` selector is resolved (ATRAC9, format byte `0x42`), but the ATRAC9
   configuration word and frame geometry are unknown, so **no sample is decoded**. G2 `NOT RUN`.
9. **Save/load**: no transaction, no relaunch proof. G2 `NOT RUN`.
10. **No runtime**: `sotc_core_start()` cannot leave the failed state (§2).
11. **The entry map is a name map, not an ABI.** `gen_entry_map.py --check` passes at capture time,
    but `docs/ENTRY_MAP.md`'s prose totals lag its own generated JSON. Neither number is a claim that
    an address is callable.

## 7. Reproduce

```sh
# numbers in the machine-checked block (public sources; private inputs optional)
python3 tools/lift/g1_evidence_pack.py --check
python3 -I tools/lift/check_g1_evidence_pack.py

# the recovered constructor table against the pinned image (private ELF)
python3 -I tools/lift/check_ctor_slots.py            # pair count and first gap: see the generated §1.2 / §4
python3 -I tools/lift/check_startup_slot309_table.py # data-only row, no caller
python3 -I tools/lift/check_ctor_slot_312.py         # body + class + atexit rows

# the frontier's table-level facts and the grade split (private ELF)
python3 -I tools/lift/check_vtable_resolved_entries.py   # OK 83 sites / 32 targets
python3 -I tools/lift/check_evidence_grades.py           # OK 41 / 42 / 83, doc matches
python3 -I tools/lift/check_frontier_vptr_correction.py  # OK 9 audit rows, 92 probes rejected
python3 -I tools/lift/store_to_load_forwarding.py        # forwarding diagnostics (0 promoted)

# closures and the indirect frontier (private disassembly)
python3 -I tools/lift/cfg_census.py --seed 0x531d60
python3 -I tools/lift/indirect_frontier.py --seed 0x531d60
python3 -I tools/lift/indirect_frontier.py --seed 0x1bdf30
python3 -I tools/lift/indirect_frontier.py --seed 0x531d60 --seed 0x1bdf30

# the harness itself
cmake -S native -B local/build/orch-r22 && cmake --build local/build/orch-r22 -j 12
ctest --test-dir local/build/orch-r22 -j 12                 # 533/533 at capture time
ctest --test-dir local/build/orch-r22 -R '^(startup_ctors|startup_ctor_coverage|…)$'  # 24/24
./local/build/orch-r22/test_startup_ctor_coverage           # asserts the generated §1.2 numbers
```

Private capture for this revision: `local/validation/g1-partial-20261101/` — `manifest.json`
sha256 `f3d4224017ac0717f436de57bfcda1807ac1ca6b5dedfb17a072b18d38cf5b2a`, tip `d9ff4d2f` (dirty
tree), ctest 533/533, G1 subset 24/24, all six G1 checkers exit 0, every cited binary newer than the
library it links. Gitignored; the 2026-10-22 directory is retained as the historical artifact.

## 8. Gate status

| Gate | Status | Note |
|---|---|---|
| G0 | PARTIAL | inventory/identity baseline, unchanged |
| **G1** | **PARTIAL** | this pack — harness works, startup chain incomplete |
| G2 | NOT RUN | no service/audio/save fixture connected to game execution |
| G3 | PARTIAL | PM4 fixtures only; **0** real shaders lowered |
| G4 | PARTIAL | launcher exists, no engine lifecycle |
| G5–G6 | NOT RUN | no boot, audio, save or performance evidence |

## Changelog versus the 2026-10-22 pack

| Item | 2026-10-22 pack | This pack |
|---|---|---|
| Startup coverage | 155 verified pairs, cliff 152+ / exec 424 | the generated §4 numbers (**573/573 registered; 573/573 fully lifted**) and the historical batch-20 capture (cliff **446 / `0x4c92a0`**, 19 remaining at that time) |
| Dormant index 95 | listed as an unresolved `NEEDS_PRINCIPAL` gap | **registered** (slot 95 is covered) |
| Tests | ctest 13/13 (a filtered subset) | **ctest 533/533** full suite, **24/24** G1 subset |
| Frontier | 6 resolved rows, no grades | **83 rows / 32 targets**, split **41 site-provable / 42 table-level** (91 / 37 / 50 until E1095 withdrew eight wrong-slot rows) |
| Model headers | ungraded | 26 of 36 carry a grade; 10 still do not |
| MSL | not mentioned | stated as a G1/G3 boundary: **0 lowered** |
| Store-to-load forwarding | not mentioned | measured on 91 rows: **0 promoted** (40 no store, 8 call, 1 control flow, 1 indexed); the 9 with a store read `TABLE+0x10+slot`, so E1095 corrected 1 row and withdrew 8 |
| Entry map | not mentioned | ~1,100 distinct RVAs (≈2 % of functions); a name map, not an ABI |
| Claim | G1 PARTIAL | unchanged — **G1 PARTIAL, not PASSED** |

Corrections of prior text:

- The old pack's "155 pairs" was a count of checker-verified `(RVA, index)` pairs, not of registered
  rows; it is not comparable to the registered-row count and should not be quoted alongside it.
- The old unresolved-edge list's "cliff 152+ / exec 424" is obsolete; the current cliff is in §4 and index 95
  is registered.
- `docs/INDIRECT_FRONTIER.md`'s six-row classification table (283/96/45/47/15/2) does not reproduce
  with the current tool, which reports 319 vtable-slot / 96 first-slot / 20 register-offset /
  47 indirect-jump / 11 register / 6 member-pointer for the same `0x531d60` seed. The "Numbers"
  table in that document does reproduce (608/105/499/5). The classification table has been
  corrected to the live run.

Nothing in this pack claims a boot, a Metal frame, audio, or save/load, and none of those has been
demonstrated.
