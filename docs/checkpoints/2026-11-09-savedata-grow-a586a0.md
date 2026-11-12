# Checkpoint — savedata grow `0xa586a0` full lift

- **Date/time:** 2026-11-09 05:51 UTC (2026-11-08 23:51 CST)
- **Agent:** Buffy (Freebuff)
- **Task ID:** CPU-01-SAVEDATA-GROW-A586A0-01
- **Status:** PASSED / STATIC_ANALYSIS_VERIFIED + FIXTURE_VERIFIED
- **Code change:** real block-map grow model, reusable callee proofs, extended oracle/fixture; no constructor row changed

## Why

E1421 left `0xa586a0` as the last fail-closed seam in the savedata family: it called a host grow
hook because its 0x50 B container layout and four callees were undocumented. This pass replaces
that contract with the actual 108-instruction body and proves every callee from the pinned ELF.

## Exact body

| Item | Value |
|---|---|
| Range | `0xa586a0..0xa5880f` |
| Size | 367 B |
| SHA-256 | `723d9b307cdcc377080ddb78944172777def53b65e3df917ceacbc7a015b791f` |
| Instructions | 108 |
| Direct calls | 4 (`0x17090`, `0x170c0`, `0xf8f470`, `0xf8f480`) |
| Branches | 7 (`jne` 2, `jb` 2, `jmp` 2, `je` 1) |
| Stores | 14 |
| RIP operands | 0 |

## Admitted layout

The map is 0x50 B and its fields are pinned by both the traversal and the stores:

| Offset | Field | Evidence |
|---|---|---|
| `+0x00` | block-array base | load `a586c2`, store `a587c4` |
| `+0x08` | capacity in slots | load `a586c5`, store `a587c7` |
| `+0x18` | first used block data | store `a587df` |
| `+0x20` | first block end (`+0x100`) | store `a587e5` |
| `+0x28` | current slot | load `a586ba`, store `a587cf` |
| `+0x38` | last used block data | store `a587f2` |
| `+0x40` | last block end (`+0x100`) | store `a587fc` |
| `+0x48` | last used slot | load `a586be`, store `a587e9` |

`front = (current - begin) / 8`, `used = (map_end - current) / 8 + 1`. Blocks are 0x100 B
(`addq $0x100` at `a587df`/`a587f6`). Mode 1 with `front >= requested` shifts used slots left by
`max(requested, front/2)`; mode 0 with `capacity - front - used >= requested` shifts right by
`max(requested, free/2)`. Otherwise the body allocates
`(capacity + max(capacity, requested) + 2) * 8`, uses a front gap of `requested` only for mode 0,
copies the used slots, frees the old base, and commits base/capacity. An inconsistent map is
refused before any read; allocation or host-free failure leaves the map unchanged and is reported.

## Callees: reused vs newly modelled

| Callee | Size | Insn | Evidence | Decision |
|---|---|---|---|---|
| `0x17090` | 20 B | 4 | sha `f26d9b99…7ad11`; `jmp 0x12820` | **Reused** existing `alloc_17090`; same RVA, no duplicate body |
| `0x170c0` | 10 B | 2 | sha `39cd562d…ace372`; `jmp 0x12af0` | **Reused** existing `free_170c0`; same RVA, no duplicate body |
| `0xf8f470` | 6 B | 1 | sha `bd719a31…adf75`; bytes `ff25b20b6700`; GOT `0x1600028`; NID `Q3VBxCXhUHs` | **Newly modelled** as ISO C `memcpy`; PLT import, not game code |
| `0xf8f480` | 6 B | 1 | sha `02101fcd…7cc51`; bytes `ff25aa0b6700`; GOT `0x1600030`; NID `+P6FRGH4LfA` | **Newly modelled** as ISO C `memmove`; PLT import, not game code |

The two wrappers are the same RVAs already pinned by `native/core/alloc_wrappers_17070.*`; no
parallel allocator model was added. The two import thunks have no game body to normalize: their
entire semantics are the GOT jump, proven by the NID check in
`tools/lift/check_savedata_family.py`.

## Changes

| File | Change |
|---|---|
| `native/core/savedata_family.hpp` | Added the four callee pins, 0x50 B `SavedataBlockMap`, offset/block-size constants, and the real `SavedataGrowResult`/`savedata_grow_a586a0` contract; removed `SavedataGrowHooks`; rewrote the body comment as a full lift. |
| `native/core/savedata_family.cpp` | Replaced the hook adapter with the real front/back shift and allocate/copy/free paths, layout validation and fail-closed cleanup. |
| `native/tests/test_savedata_family.cpp` | Replaced the hook test with nine grow cases: front shift, back shift, both allocation modes, allocation failure, failed old-base free, missing contract, null map, invalid layout. |
| `tools/lift/check_savedata_family.py` | Added the four callee windows, exact thunk bytes/GOT/NID checks, wrapper jump-target checks, map/header tokens, and nine body/nine header tamper modes. |
| `native/core/savedata_stats_a54920.hpp` | Corrected the stale comment: the family grow model is now full, not fail-closed. |
| `ROADMAP.md`, `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md` | Recorded E1422 and the next integration slice. |
| `docs/entry_map.json`, `docs/ENTRY_MAP.md` | Regenerated. |

## Commands and results

```sh
cmake --build local/build/orch-r22 -j 12
# BUILD_STATUS=0

ctest --test-dir local/build/orch-r22 -R '^(savedata_family|savedata_stats_a54920|rb_tree_482a90|ctor_slot_454|alloc_wrappers_17070)$' --output-on-failure
# 5/5 passed; CTEST_CHAIN_STATUS=0
ctest --test-dir local/build/orch-r22 -R '^(registry_init_531d60|init_531d60_entry_gaps)$' --output-on-failure
# 2/2 passed; INIT_CTEST_STATUS=0; focused chain total 7/7

python3 -I tools/lift/check_falloc_wrappers.py
# OK: 0x17070->0x12820 tag1/align8/flags1; 0x170b0/0x170c0->0x12af0; ALLOC_ORACLE_STATUS=0

python3 -I tools/lift/check_savedata_family.py
# OK ... 9 windows (5 bodies + 4 callees; 347/303/1201/76/367 B;
# 98/81/340/26/108 insn; 0xa57d40 == 0x482a90 over 340 normalized instructions;
# 0x17090->0x12820, 0x170c0->0x12af0, 0xf8f470=memcpy, 0xf8f480=memmove)
# ORACLE_STATUS=0

python3 -I tools/lift/check_savedata_family.py --self-test
# OK tamper modes: all 9 body bytes and all 9 header RVAs rejected; SELFTEST_STATUS=0

python3 -I tools/lift/report_init_coverage.py --start 0xa586a0 --end 0xa5880f
# 367 B modelled, 100.0% (before: 367 B, 100.0% via the removed hook range)
python3 -I tools/lift/report_init_coverage.py --start 0x17090 --end 0x170a4   # 20 B, 100.0%
python3 -I tools/lift/report_init_coverage.py --start 0x170c0 --end 0x170ca   # 10 B, 100.0%
python3 -I tools/lift/report_init_coverage.py --start 0xf8f470 --end 0xf8f476 # 6 B, 100.0% (new pin; 0 B before)
python3 -I tools/lift/report_init_coverage.py --start 0xf8f480 --end 0xf8f486 # 6 B, 100.0% (new pin; 0 B before)
python3 -I tools/lift/report_init_coverage.py --start 0xa55050 --end 0xa551ab  # 347 B, 100.0%
python3 -I tools/lift/report_init_coverage.py --start 0xa57c10 --end 0xa57d3f  # 303 B, 100.0%
python3 -I tools/lift/report_init_coverage.py --start 0xa57d40 --end 0xa581f1  # 1,201 B, 100.0%
python3 -I tools/lift/report_init_coverage.py --start 0xa58490 --end 0xa584dc  # 76 B, 100.0%
python3 -I tools/lift/report_init_coverage.py                                  # 3,500/3,500 B, 100.0%

python3 -I tools/lift/gen_entry_map.py
# wrote docs/entry_map.json: 2913 constants -> 1852 distinct guest RVAs in 574 headers (440 duplicates)
python3 -I tools/lift/check_entry_map.py       # OK; CHECK_STATUS=0
python3 -I scripts/audit_publish_paths.py --all # PUBLISH PATH AUDIT OK (4110 path(s)); AUDIT_STATUS=0
python3 scripts/project_index.py                # 114634 folders indexed; INDEX_STATUS=0
git diff --check                                # DIFFCHECK_STATUS=0
git status --porcelain=v1                       # STAGED=0
```

## Limits and next

- The failed self-test on the first attempt was the harness, not the model: `tamper_header` replaced
  the first textual `0x…`, which was in the header comment, so the constant stayed valid. It now
  targets `= 0x…;` and the 9×9 rejection passes. This did not weaken any assertion.
- `0xa586a0` coverage was already 367/367 B because the old contract pinned the range; the gain is
  that the behavior is now real, with no host grow callback. The two import pins add 6 B each.
- `0xa54920` still uses caller-supplied `sibling_ctor`/`list_node_dtor`/`vector_grow` seams; the
  underlying bodies are now real models in `savedata_family`. Wiring them is the next slice.
- G1 remains **PARTIAL**; no boot, audio, save/load or Metal gate is claimed. No guest byte was
  executed.
- Tree left with nothing staged, committed, tagged or pushed. No subagent was used.
