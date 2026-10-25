# Checkpoint — Init `0x531d60` continuation batch A (22 units, 95 B)

- Local: 2026-10-22 ~23:45 CDT (America/Chicago)
- Agent: orchestrator round-2 worker subagent (non-Claude model) + integrator replay
- Task: `CPU-01-INIT-531D60-CONTINUE-BATCH-01` — attempt 1, success (0 failed hypotheses)
- Gate: **G1 PARTIAL — control-flow pin only, not PASSED**
- Evidence: E519

## What was lifted

One grouped module continues the Init walk from the E508 stop (`0x532720`) through
`0x532720..0x53277f` (95 B, **22 units**), replacing the former one-5-byte-file-per-unit
increment. Units are listed in `kInit531d60PostBaseContinueAUnits` with per-unit RVA/size/hash;
the module exposes one hook per observed call and one pin per observed address, and reports a
per-unit counter.

Files: `native/core/init_531d60_post_base_continue_a.{hpp,cpp}`,
`tools/lift/check_f531d60_post_base_continue_a.py`, wiring in
`native/core/appmain_init_ctor_98a90_slice.{hpp,cpp}`, assertions and telemetry in
`native/tests/test_appmain_init_ctor_98a90_slice.cpp`, one line in `native/CMakeLists.txt`.

## OBS (byte-verified from `local/re/eboot.elf`, sha256 `d0723afb…861d3`)

Region sha256 `9f4c676b6b368c43629b591db887ef2654bfade085b69fb91e493cb66ebcee40`; units are
contiguous (`0x532720 + Σsize = 0x53277f`, re-checked by the integrator, no gaps/overlaps).

| Call sites | Target |
| --- | --- |
| `0x532723` | `0xf590` |
| `0x532731` | `0xa2ea00` |
| `0x53273b` | `0xa2e5d0` |
| `0x532760` | `0xa543b0` |
| `0x532775` | `0x1fc80` |

| RIP-relative lea | Target |
| --- | --- |
| `0x532740` | `0x32f79a8` |
| `0x532747` | `0x32f84f8` |
| `0x53276a` | `0x1014d69` |

Branch: `0x53277a..0x53277d test rax,rax` + `0x53277d..0x53277f je 0x5327bd` (rel8 target pinned). The
flag computation and the branch **outcome are NOT modelled** (`branch_outcome_modelled = false`).

## NOT modelled

Bodies of `0xf590`, `0xa2ea00`, `0xa2e5d0`, `0xa543b0`, `0x1fc80`; `test` flags and the `je`
outcome; both successors (`0x53277f` fall-through and `0x5327bd`); provenance of incoming
`r13`/`r15`/`r12`/`r14`; object layouts at `0x32f79a8`/`0x32f84f8` beyond `+0xf8`; data at
`0x1014d69` (RVA pin only); the real stack frame beyond `0x20(%rsp)`; any effect on the Init
result, rendering, audio or save data.

## Verification (integrator replay, not worker output)

```
cmake -S native -B local/build/orch-final -G Ninja      -> exit 0
cmake --build local/build/orch-final                    -> exit 0 (1080 targets)
ctest --test-dir local/build/orch-final                 -> 100% tests passed out of 326
SOTC_NATIVE_BUILD=local/build/orch-final python3 -I tools/lift/check_f531d60_post_base_continue_a.py -> OK
```

Independent byte replay: region sha matches; all 22 units contiguous to `0x53277f`; all five
rel32 targets, all three lea targets and the `je` target match; sampled unit shas match.

Telemetry: `pbca_units=22`, registry `entries=275` (unchanged), `alloc_ops=13`, all prior
per-unit counters still 1; `tools/lift/check_ctor_slots.py` still **155** pairs.

## Next exact action

Continue at `0x53277f` (fall-through of the pinned `je`), still without modelling the branch
outcome; the alternate block `0x5327bd` (`lea rax,[0x2dfce70]` then indirect calls) is a second
frontier. Prefer branch-body work over the five callee bodies until `0x1290`'s insert path
(`0x1bde30` → `0x1beeb0`) settles the descriptor width question.
