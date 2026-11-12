# Checkpoint — savedata family neighbours `0xa55050` / `0xa57c10` / `0xa586a0`

- **Date/time:** 2026-11-09 (local)
- **Agent:** Buffy (Freebuff)
- **Task ID:** CPU-01-SAVEDATA-FAMILY-A55050-01
- **Status:** PASSED / STATIC_ANALYSIS_VERIFIED + FIXTURE_VERIFIED
- **Code change:** new native family model, fixture, trace, oracle and CMake wiring; no constructor row changed

## Why

`0xa54920` landed with its three nearest neighbours still represented by default hooks: the
`0xa55050` slot allocator, the `0xa57c10` sentinel/list cleanup and the `0xa586a0` deque-map grow.
This pass bounded all three by FDE and either lifted them or replaced the placeholder with a
fail-closed contract that names the exact instruction range and the reason it remains a hook.

## Exact bodies

| Body | Range | Size | SHA-256 | Result |
|---|---|---|---|---|
| `0xa55050` | `0xa55050..0xa551ab` | 347 B | `062b16fd7c32ca8f25c760e91c7f90e1855cd0ada958128706fee4e140a7456e` | **Full native lift** |
| `0xa57c10` | `0xa57c10..0xa57d3f` | 303 B | `6c03fcb0da5ce003b10b190f91c46e6c1e49e899c38d156920c3f64f9dfd029c` | **Native reachable path**; dead traversal documented |
| `0xa586a0` | `0xa586a0..0xa5880f` | 367 B | `723d9b307cdcc377080ddb78944172777def53b65e3df917ceacbc7a015b791f` | **Precise fail-closed contract** |

## Changes

| File | Change |
|---|---|
| `native/core/savedata_family.hpp` | Family constants, slot allocator result, tree-clear hooks/result and grow contract, with exact RVAs/sizes/offsets and the dead-branch explanation. |
| `native/core/savedata_family.cpp` | Full `0xa55050` slot selection; reachable `0xa57c10` sentinel cleanup; fail-closed `0xa586a0` hook adapter. |
| `native/tests/test_savedata_family.cpp` | Slot search/wrap/full/null cases; sentinel reset and hook cases; grow success/refusal cases. |
| `tools/lift/trace_savedata_family.py` | Re-derives SHA, instruction/call/branch/store/RIP counts from the pinned ELF for all three bodies. |
| `tools/lift/check_savedata_family.py` | Oracle pins all three bodies and model/header tokens; body-byte and header-RVA tamper self-tests. |
| `native/CMakeLists.txt` | `core/savedata_family.cpp` source, `test_savedata_family` target and CTest registration. |
| `native/core/savedata_stats_a54920.hpp` | Corrected the stale “`0xa55050` body is NOT lifted” comment; the remaining `sibling_ctor` is now explicitly an integration seam, and `vector_grow` points at the family contract. |
| `docs/entry_map.json`, `docs/ENTRY_MAP.md` | Regenerated for the three new RVA declarations. |
| `docs/TASKS.md`, `docs/EVIDENCE.md`, `HANDOFF.md`, `ROADMAP.md` | New task row, E1420, handoff and roadmap note. |

## Commands and results

```sh
cmake --build local/build/orch-r22 --target test_savedata_family test_savedata_stats_a54920 -j 12
# BUILD_STATUS=0; all targets built

ctest --test-dir local/build/orch-r22 -R '^(savedata_family|savedata_stats_a54920)$' --output-on-failure
# 2/2 passed (savedata_family, savedata_stats_a54920), CTEST_STATUS=0
# logs/savedata_family_chain.log

python3 -I tools/lift/check_savedata_family.py
# OK savedata family: 3 bodies pinned (347/303/367 B; 98/81/108 insn;
# 0/3/4 direct calls; 8/12/7 branches; 1/9/14 stores), ORACLE_STATUS=0

python3 -I tools/lift/check_savedata_family.py --self-test
# expected tamper FAIL diagnostics; OK tamper modes: all three body bytes and
# all three header RVAs rejected, TAMPER_STATUS=0

python3 -I tools/lift/report_init_coverage.py --start 0xa55050 --end 0xa551ab
# after: 347 B modelled, 100.0% (1 span, 0 gaps); before: 0 B / 0.0% (header renamed)
python3 -I tools/lift/report_init_coverage.py --start 0xa57c10 --end 0xa57d3f
# after: 303 B modelled, 100.0%; before: 0 B / 0.0%
python3 -I tools/lift/report_init_coverage.py --start 0xa586a0 --end 0xa5880f
# after: 367 B modelled, 100.0%; before: 0 B / 0.0%
python3 -I tools/lift/report_init_coverage.py
# Init coverage 0x531d60..0x532b0c (3500 B): 3500 B modelled, 100.0%; no regression

python3 -I tools/lift/gen_entry_map.py
# wrote docs/entry_map.json: 2902 constants -> 1844 distinct guest RVAs in 574 headers
# (439 duplicates)
python3 -I tools/lift/check_entry_map.py
# OK check_entry_map: 2902 constants -> 1844 distinct guest RVAs in 574 headers (439 duplicates)

python3 -I tools/lift/check_g1_evidence_pack.py
# OK; 573/573 startup rows, 83 frontier rows, 41 site-provable

python3 -I scripts/audit_publish_paths.py --all
# PUBLISH PATH AUDIT OK (4110 path(s))

python3 scripts/project_index.py
# 114634 folders indexed; written
```

## Observations vs inference

- **Observed:** `0xa55050` is 98 instructions, no calls, eight `je` branches, one store and two
  RIP operands to the cursor cell `0x32f88a0`. The model scans the eight `0xb8` slots at
  `parent+0x28`, tests the dword at `+0x14`, writes `idx+1` on a hit, and returns `parent+0x28`
  unchanged when all are full. Fixtures cover cursor 5 → slot 6, wrap 7 → slot 1, and the full
  fallback.
- **Observed:** `0xa57c10` is 81 instructions, three direct calls (`0xa57d40`, `0xa58490`,
  `0xf8f3a0`), 12 branches and nine stores. At `a57c37` it loads `rbx = [r13]`; at `a57c3b` it
  compares `rbx` with `[r13]`, so the first `je` is always taken. The reachable cleanup is
  `a57ceb..a57d11` (release hook, sentinel reset, `[this+0x10]=0`), then the shared canary/return
  `a57d1a..a57d38`. The second compare and traversal `a57c45..a57cea` are dead from this entry.
  The full body is still pinned by the oracle, so the dead block cannot silently change.
- **Observed:** `0xa586a0` is 108 instructions, four direct calls (`0x17090`, `0x170c0`,
  `0xf8f470`, `0xf8f480`), seven branches and 14 stores, with no RIP operands. Its container
  layout and those four helpers are not lifted, so the model refuses unless the host supplies the
  grow hook; it does not invent capacity arithmetic.
- **Fixture correction:** the first test draft wrote `0xDEADBEEF` into `object+0x10`, which is both
  the sentinel free-slot word read for the hook and the final `[this+0x10]=0` cell. The fixture now
  asserts the body’s real final layout: `header[0] == header`, `header+0x10 == header`,
  `header+8 == nullptr`; the missing-hook case restores a non-null subtree first.
- **Inference:** the `0xa57c10` non-empty traversal is unreachable from this entry, not proven
  unreachable from every possible entry; it remains unmodelled. `0xa58490` recursive release and
  `0xa586a0` container fields are the next real dependencies.
- **No gate advance:** static/fixture evidence only, no guest byte executed. G1 remains
  **PARTIAL** (boot/audio/save/Metal ungated).

## Risks / limits

- `0xa57c10` models only the reachable empty/sentinel path; the traversal body is dead here but
  not lifted.
- `0xa586a0` is a documented fail-closed contract, not a capacity/layout implementation.
- `0xa54920` still exposes `sibling_ctor` and `vector_grow` as caller integration seams; the
  family models are separate and do not claim a full runtime wiring.
- The tree is dirty with pre-existing work plus this unit: **812 changed/untracked paths, 0 staged**;
  nothing committed, tagged or pushed.

## Next exact action

Lift `0xa57d40` (the per-node erase worker) and then `0xa58490` (recursive node release) so the
`0xa57c10` traversal can be modelled honestly; that is the shortest path to a complete list
lifecycle and is higher leverage than starting a new subsystem. The alternative is to recover the
`0xa586a0` container layout and its four helpers before turning the grow contract into a model.
