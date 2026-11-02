# G1 PARTIAL evidence pack refresh (E1071) — 2026-11-01

- Task: `CPU-01-G1-PARTIAL-EVIDENCE-01` (rerun; the 2026-10-22 pack was stale).
- Gate: **G1**; status **PARTIAL — not PASSED**. G0 PARTIAL, G2 NOT RUN, G3 PARTIAL, G4 PARTIAL,
  G5–G6 NOT RUN.
- Tip at capture: `b5798ead` on `cursor/init-hash-551140-6aa93e8`, dirty tree.
- Machine: Apple M4 Pro, macOS 27.0.1, Xcode 27.1, Python 3.15.
- Failed tries: 0. One transient tree inconsistency was observed and is recorded below.

## What changed

1. `docs/G1_EVIDENCE.md` — new single pack (supersedes
   `docs/checkpoints/2026-10-22-g1-partial-evidence-pack.md`). It separates what the ARM64 harness
   proves from what it does not, states the evidence-grade split, and carries a machine-checked block.
2. `tools/lift/g1_evidence_pack.py` — measures the block from `native/` and, when present, the pinned
   ELF and the strict tie artifact. `--check` compares, `--write` regenerates, `--json` dumps.
3. `tools/lift/check_g1_evidence_pack.py` — sweep wrapper; fails when the block drifts.
4. `local/validation/g1-partial-20261101/{capture.py,manifest.json,SUMMARY.json}` — private capture,
   `manifest.json` sha256 `12433a36d0687a960bae22ea2ee0a72973542b7683a3607b4472c6d4ab55e2bc`.
5. `docs/INDIRECT_FRONTIER.md` — classification table corrected to the live tool's counts, with a note
   that the older row did not reproduce.
6. `docs/TASKS.md` row and `docs/EVIDENCE.md` E1071.

## Commands and results (all run in this session)

| Command | Result |
|---|---|
| `python3 tools/lift/g1_evidence_pack.py` | 451/573 rows, first gap 314/`0xa41810`, 122 remaining, 137 above the gap; frontier 91/37/21 split 41/50; 26/36 graded headers |
| `python3 -I tools/lift/check_g1_evidence_pack.py` | exit 0 (and exit 1 after a deliberate doc mutation) |
| `python3 -I tools/lift/check_ctor_slots.py` | exit 0 — 573-qword table sha256 `5917cb85…ab28`, 451 verified pairs |
| `python3 -I tools/lift/check_startup_slot309_table.py` | exit 0 — data-only row, no near caller |
| `python3 -I tools/lift/check_ctor_slot_312.py` | exit 0 — 1,250 B FDE, 394 ELF + 145 native mutants rejected |
| `python3 -I tools/lift/check_vtable_resolved_entries.py` | exit 0 — 91 sites / 37 targets |
| `python3 -I tools/lift/check_indirect_frontier.py` | exit 0 — 608 / 105 / 499 / 5 |
| `python3 -I tools/lift/check_evidence_grades.py` | exit 0 — 41 site-provable / 50 table-level / 91 rows, doc matches |
| `python3 -I tools/lift/store_to_load_forwarding.py` | 91 rows, 50 analysed, **0 promoted** |
| `python3 -I tools/lift/indirect_frontier.py --seed 0x531d60` (and `0x1bdf30`, union) | 608/105/499/5, 416/59/122/3, 817/136/562/6 into a temp path |
| `python3 -I tools/lift/cfg_census.py --seed …` | 55,790 FDE / 15,839,273 B; 227,401 call sites (47,481 indirect) |
| `python3 -I local/validation/g1-partial-20261101/capture.py` | exit 0 — ctest 533/533, G1 subset 24/24, 5 checkers exit 0, 4 binaries arm64 and newer than `libsotc_core.a` |
| `./local/build/orch-r22/test_startup_ctor_coverage` | exit 0 — asserts 451 / 314 / `0xa41810` |
| `python3 -I tools/lift/gen_entry_map.py --check` | exit 0 after the integrator's 21:05 regeneration (1,638 constants → 1,099 RVAs / 406 headers) |
| inline byte scan for `e8`/`e9` → `0xa505e0` / `0xa46130` | 0 sites each (superset scan over every executable segment) |

Not run here by instruction: `tools/lift/run_all_checkers.sh`, `scripts/project_index.py`,
`gen_entry_map.py` (write mode), any `git commit/tag/push`.

## Observations versus inference

- **Observation:** every number above was measured in this session and is reproducible from the
  commands listed. **Inference:** none of it establishes retail behaviour; the pack says so.
- **Observation:** `ResolvedVtableCallSite` has no grade field, so the 41/50 split is not expressible
  in the type. **Inference:** the "table-level must not drive dispatch" rule is documentation only
  until the struct changes — this is a finding, not a fix.
- **Observation:** 10 of 36 vtable model headers carry no grade, six of them site-provable targets.
- **Observation:** the bounded store-to-load forwarding pass over the 91 rows promotes **0 of the
  50** (40 no store in the FDE, 8 call between, 1 control flow, 1 indexed); `check_evidence_grades.py`
  now re-derives 41/50/91 and fails when the frontier card drifts.
- **Observation:** MSL `lowered: 0` over 34,291 members in the private coverage artifact.
- **Transient:** at 21:07 the tree briefly contained a 452-row `kSlots` with an un-updated test and
  checker (`check_ctor_slots.py`: `extra=[314]`); by 21:08 the integrator had parked slot 314 and the
  tree was consistent again. The capture was re-run after that (manifest `12433a36…`).

## Remaining risks

- The pack is bound to 451 rows: registering slot 314 or adding a frontier row makes
  `check_g1_evidence_pack.py` fail until `--write` is run. That is deliberate; the failure message
  names the command.
- `docs/ENTRY_MAP.md` prose (1,549 / 1,052 / 399) lags its own generated JSON (1,638 / 1,099 / 406).
- `ROADMAP.md`'s operating-plan paragraph still says "449/573 with first gap 309"; the measured
  numbers are 451/573 with first gap 314.
- The store-to-load pass was run only over the 91 frontier rows; the image-wide class is unmeasured.
- The 50 table-level rows and the ungraded headers are documentation debt that a future dispatcher
  could trip over.

## Next exact action

Regenerate the folder index and entry-map prose in one integrator pass, then either finish slot 314
from `local/wip/slot314/` (fix the page-layout `static_assert`, write the fixture) or record a hook
contract that reports truthfully — and re-run `python3 tools/lift/g1_evidence_pack.py --write` the
moment coverage moves.
