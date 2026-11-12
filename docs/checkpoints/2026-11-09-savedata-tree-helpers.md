# Checkpoint — savedata tree helpers `0xa57d40` / `0xa58490`

- **Date/time:** 2026-11-09 (local)
- **Agent:** Buffy (Freebuff)
- **Task ID:** CPU-01-SAVEDATA-TREE-HELPERS-A57D40-01
- **Status:** PASSED / STATIC_ANALYSIS_VERIFIED + FIXTURE_VERIFIED
- **Code change:** real erase/release models, corrected RB layout, fixture and oracle extension; no constructor row changed

## Why

E1420 left `0xa57c10` releasing its subtree through a host callback and documented the two helpers
it calls as unlifted. This pass bounds both helpers and replaces the callback with the real release
model, so the savedata list lifecycle is native end to end.

## Exact bodies

| Body | Range | Size | SHA-256 | Result |
|---|---|---|---|---|
| `0xa55050` | `0xa55050..0xa551ab` | 347 B | `062b16fd7c32ca8f25c760e91c7f90e1855cd0ada958128706fee4e140a7456e` | Full lift (unchanged) |
| `0xa57c10` | `0xa57c10..0xa57d3f` | 303 B | `6c03fcb0da5ce003b10b190f91c46e6c1e49e899c38d156920c3f64f9dfd029c` | Reachable path; release now real; traversal dead |
| `0xa57d40` | `0xa57d40..0xa581f1` | 1,201 B | `4f54c1a69b06da222423c7f9ce724823be6698d3fa8acbb699e2ab611f1b3f2e` | **Full lift by delegation to `0x482a90`** |
| `0xa58490` | `0xa58490..0xa584dc` | 76 B | `7003816aa67ab301f6d65354bbc84182d442f44743ab3316123002631943c24b` | **Full recursive lift** |
| `0xa586a0` | `0xa586a0..0xa5880f` | 367 B | `723d9b307cdcc377080ddb78944172777def53b65e3df917ceacbc7a015b791f` | Documented fail-closed hook (unchanged) |

Counts: `0xa57d40` 340 instructions, 2 direct calls (`0x170b0`, `0xf8fc20`), 0 indirect, 77 branches
(29 `je`, 25 `jne`, 23 `jmp`), 86 stores, one RIP operand (`0x12c4741`). `0xa58490` 26 instructions,
2 direct calls (recursive `0xa58490`, `0x170b0`), 0 indirect, 2 branches (`je`, `jne`), 0 stores.

## Decisions

1. **`0xa57d40` is the same routine as `0x482a90`.** Normalizing every immediate yields 0
   differences over 340 instructions; both call targets match; the assert string at `0x12c4741` is
   byte-identical to `0xffb153` (`invalid map/set<T> iterator`). The model delegates rather than
   duplicating the rebalance.
2. **RB node layout corrected in place.** The admitted model had mirrored `+0x00/+0x10`; bytes and
   the `0xa57c10` successor walk prove `+0x00 left / +0x08 parent / +0x10 right / +0x18 colour /
   +0x19 nil`, with the header root at `header+0x08`. Root rotations and header references were
   corrected; no parallel model was added.
3. **`0xa57c10` traversal stays dead.** The first compare at `a57c37..a57c3f` compares `[r13]` with
   itself and always branches to `a57ceb`. A full disassembly scan found no call or branch into
   `a57c45..a57cff`, so the traversal is dead code, not a hook and not modelled as if it ran.
4. **Sentinel reset asymmetry is modelled exactly.** `a57cf7..a57d11` sets `header+0x8 = header`,
   `header+0x0 = header`, `header+0x10 = header`, then zeroes `this+0x10` (the same cell as
   `header+0x8`). The fixture checks `left == header`, `right == header`, `parent == nullptr`.

## Changes

| File | Change |
|---|---|
| `native/core/savedata_family.hpp` | Added exact `0xa57d40`/`0xa58490` pins and duplicate pin; replaced `SavedataTreeClearHooks` with `AllocDispatchContract`; documented the equivalence, release walk and dead traversal. |
| `native/core/savedata_family.cpp` | Added `savedata_tree_erase_a57d40` (delegates to `rb_erase_482a90`) and `savedata_tree_release_a58490` (right recurse / `free_170b0` / left iterate); `0xa57c10` now calls it and resets through a byte view. |
| `native/core/rb_tree_482a90.hpp/.cpp` | Corrected node field order to left/parent/right and header root/rightmost references. |
| `native/tests/test_rb_tree_482a90.cpp` | Fixed rotation root updates and removed the temporary debug dumper. |
| `native/tests/test_savedata_family.cpp` | Real three-node tree; release-order 3,2,1; partial-failure and refusal cases; one-node `0xa57d40` erase and sentinel rejection. |
| `tools/lift/trace_savedata_family.py` | Registered the two new bodies. |
| `tools/lift/check_savedata_family.py` | Pins five bodies, adds the `0xa57d40 == 0x482a90` equivalence check, rejects stale release seams, and tamper-tests five bodies and five header RVAs. |
| `docs/STARTUP_GAP_SURVEY.md` | Cleared the four pre-existing trailing-whitespace lines. |
| `docs/entry_map.json`, `docs/ENTRY_MAP.md` | Regenerated. |

## Commands and results

```sh
cmake --build local/build/orch-r22 --target test_savedata_family test_rb_tree_482a90 \
  test_ctor_slot_454 test_savedata_stats_a54920 -j 12
# BUILD_EXIT=0

ctest --test-dir local/build/orch-r22 -R '^(savedata_family|savedata_stats_a54920|rb_tree_482a90|ctor_slot_454)$' --output-on-failure
# 4/4 passed; CTEST_EXIT=0; logs/savedata_tree_helpers_chain.log

python3 -I tools/lift/check_savedata_family.py
# OK ... 5 bodies pinned (347/303/1201/76/367 B; 98/81/340/26/108 insn;
# 0/3/2/2/4 direct calls; 8/12/77/2/7 branches; 1/9/86/0/14 stores);
# 0xa57d40 == 0x482a90 over 340 normalized instructions; ORACLE_EXIT=0

python3 -I tools/lift/check_savedata_family.py --self-test
# OK tamper modes: all five body bytes and all five header RVAs rejected; SELFTEST_EXIT=0

python3 -I tools/lift/report_init_coverage.py --start 0xa57d40 --end 0xa581f1
# before: 0 B / 0.0%; after: 1,201 B modelled, 100.0% (1 span, 0 gaps)
python3 -I tools/lift/report_init_coverage.py --start 0xa58490 --end 0xa584dc
# before: 0 B / 0.0%; after: 76 B modelled, 100.0% (1 span, 0 gaps)
python3 -I tools/lift/report_init_coverage.py --start 0xa55050 --end 0xa551ab   # 347 B, 100.0%
python3 -I tools/lift/report_init_coverage.py --start 0xa57c10 --end 0xa57d3f   # 303 B, 100.0%
python3 -I tools/lift/report_init_coverage.py --start 0xa586a0 --end 0xa5880f   # 367 B, 100.0%
python3 -I tools/lift/report_init_coverage.py                                   # Init 3,500/3,500 B, 100.0%

python3 -I tools/lift/gen_entry_map.py
# wrote docs/entry_map.json: 2907 constants -> 1848 distinct guest RVAs in 574 headers (440 duplicates)
python3 -I tools/lift/check_entry_map.py      # OK; ENTRYMAP_EXIT=0
python3 -I tools/lift/check_g1_evidence_pack.py  # EVIDENCE_EXIT=0
python3 -I scripts/audit_publish_paths.py --all  # PUBLISH PATH AUDIT OK (4110 path(s)); 0
python3 scripts/project_index.py                 # INDEX_EXIT=0
git diff --check                                 # DIFFCHECK_EXIT=0
git diff --cached --name-only | wc -l            # 0
```

Final rerun after the sentinel-path fidelity fix is in `logs/savedata_tree_helpers_final.log`:
build 0, focused CTest 4/4, oracle 0, tamper self-test 0, and every checker above still 0.

## Limits and next

- `0xa586a0` remains a precise fail-closed hook; its container layout and four helpers are not
  lifted, so the host still must supply the grow contract. This was not weakened.
- `0xa57c10`'s traversal is dead code, not exercised; the model documents it rather than pretending
  it runs.
- G1 remains **PARTIAL**; no boot, audio, save/load or runtime gate is claimed.
- Next exact action: recover `0xa586a0`'s container layout and `0x17090`/`0x170c0`/`0xf8f470`/
  `0xf8f480` to replace the grow contract, or take the next ROADMAP runtime slice.
- Tree left with nothing staged, committed, tagged or pushed.
