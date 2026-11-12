# Checkpoint — G1/startup-gap record sync after batch 13

- **Date/time:** 2026-11-08 (local)
- **Agent:** Buffy (Freebuff)
- **Task ID:** CPU-01-STARTUP-RECORD-SYNC-01
- **Status:** DONE_DOC / STATIC_ANALYSIS_VERIFIED
- **Code change:** none (documentation and generator records only)

## Why

BATCH49 closed slot 554 (E1417) and made the working tree **573/573 registered rows**, but three
records still described slot 554 as **PARTIAL**:

- `tools/lift/g1_evidence_pack.py` still had `PARTIAL_SLOTS = {554}` and three hardcoded prose
  branches that named 554 as PARTIAL.
- `docs/G1_EVIDENCE.md` (generated) reported `"partial_rows": [554]`, `"fully_lifted_rows": 572`,
  and a manual paragraph claiming the "current cliff" was slot 445.
- `docs/STARTUP_GAP_SURVEY.md` (generated) still ranked 554 as the one open gap.

## Changes

| File | Change |
|---|---|
| `tools/lift/g1_evidence_pack.py` | `PARTIAL_SLOTS` is an empty set; `render_prose`, `render_ctor_check` and `render_coverage` now state that every registered row is fully lifted and cite batch 13 as the close-out. |
| `docs/G1_EVIDENCE.md` | Regenerated. §4 JSON `registered_rows 573`, `fully_lifted_rows 573`, `partial_rows []`, `remaining_rows 0`, `first_unrecovered_index -1`. Manual slot-445 cliff paragraph replaced with the no-cliff state; changelog row updated to 573/573. |
| `tools/lift/survey_startup_gaps.py` | `--write-doc` emits an explicit "no open constructor gap remains" paragraph when `open_count == 0`, so the empty ranking is explained rather than looking like a failed run. |
| `docs/STARTUP_GAP_SURVEY.md` | Regenerated: HEAD 460/573, working tree 573/573, 113 gaps, **0 open**, empty ranked table and batch plan. |
| `ROADMAP.md` | Constructor paragraph now says 573/573 registered and fully lifted; points the next work at the Init chain and runtime gates. |
| `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md` | New top checkpoint, `CPU-01-STARTUP-RECORD-SYNC-01` row and E1418. |

## Commands and results

```sh
python3 tools/lift/g1_evidence_pack.py --write
# wrote docs/G1_EVIDENCE.md
# startup: 573/573 rows, first gap -1/0x0, 0 remaining
# frontier: 83 rows / 32 targets / 20 table bases — 41 site-provable, 42 table-level
# exit 0

python3 -I tools/lift/check_g1_evidence_pack.py
# OK tools/lift/g1_evidence_pack.py: docs/G1_EVIDENCE.md matches the tree
# (573/573 startup, 83 frontier rows, 41 site-provable)
# exit 0

python3 tools/lift/survey_startup_gaps.py --write-doc > logs/survey_regen.log 2>&1
# OK survey_startup_gaps: HEAD 460/573, working tree 573/573,
# 113 gaps (0 open, in-flight [], wired-uncommitted 113, modelled-unwired [])
# exit 0 (logs/survey_regen.log)
```

## Observations vs inference

- **Observed:** the generator parses 573 registered rows from `native/core/startup_ctors.cpp` and
  finds `remaining_rows == 0`; the C++ test pins `slots.size() == 573` and
  `first_unrecovered_constructor_index() == -1`; the survey run exits 0 with `open_count == 0`.
- **Observed:** `check_g1_evidence_pack.py` passes against the regenerated pack.
- **Inference:** none of this advances a runtime gate. G1 remains **PARTIAL** because boot, audio,
  real save/load and Metal validation are still not demonstrated.

## Risks / limits

- This sync does not commit. The working tree still contains the pre-existing 797 changed/untracked
  paths, including all batch 1–13 work.
- `docs/STARTUP_GAP_SURVEY.md` still lists the 113 wired-but-uncommitted rows; that is an accurate
  HEAD-vs-worktree distinction, not an open-gap claim.

## Next exact action

With no constructor row left, inventory the Init `0x531d60` closure remainder (`0x499f80` and the
`appmain_init_ctor_98a90` slice are already modelled) and register one bounded slice, or take
`APP-01-NOTARIZED-DMG-01`, which needs signing/packaging.
