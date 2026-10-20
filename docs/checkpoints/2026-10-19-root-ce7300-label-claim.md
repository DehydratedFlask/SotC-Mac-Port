# Claim checkpoint: CPU-01d `0xce7300` provenance and index labels

- Time / timezone: 2026-10-19 22:39 CDT (America/Chicago)
- Agent: `/root`; sole owner for this bounded follow-up.
- Task / gate / status: CPU-01d fixture metadata correction; ACTIVE; failed tries 0.
- Owned files: `native/core/vec_ce7300.hpp`, `native/tests/test_vec_ce7300.cpp`, and `tools/lift/check_fce7300.py` only. Function behavior and startup registration are not part of this claim.
- Input identity: private ELF SHA256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`.

## Read-only evidence and hypothesis

- `graft grep "init_vec_ce7300"` shows the implementation/test references and registration in `native/core/startup_ctors.cpp` at survey position 149.
- A read-only Python query of `local/re/work/ctor-survey.json` and `local/re/ctor-vec-copies.json` found `0xce7300` at survey-list position 149 and vector-copy-manifest position 91. The vector manifest row records source `0x12f8bb0`, destination `0x3351cf0`, and lanes `{0xffffffff, 0xffffffff, 0xffffffff, 0}`.
- The carried header diff attributes the lift to `bc-02397584`, but HANDOFF's completed claim and original commit identify the lift worker as `bc-effcf5dc`. The checker says “ctor-survey index 91,” conflating two different list positions.
- Hypothesis: correcting the header attribution and exposing/verifying both list positions will remove the provenance ambiguity without changing the function or its initialization behavior.

## Exact commands and current result

Working directory: `/Volumes/Extra/SoC Mac Native`.

```sh
graft grep "init_vec_ce7300"
```

Exit 0; Graft found five hits in four symbols. The constructor registry entry is at `native/core/startup_ctors.cpp:129` and uses survey position 149.

```sh
python3 - <<'PY'
import json
from pathlib import Path
for name in ('local/re/work/ctor-survey.json', 'local/re/ctor-vec-copies.json'):
    value = json.loads(Path(name).read_text())
    rows = value.get('entries', []) if isinstance(value, dict) else value
    print(name, len(rows))
    for index in (91, 149):
        if index < len(rows):
            print(index, rows[index])
PY
```

Exit 0. Survey position 149 is `0xce7300`; vector-copy manifest position 91 is `0xce7300`. Survey position 91 is instead `0xd7c380`; vector-copy position 149 is `0xc42b00`.

Initial working tree had exactly these three pre-existing unstaged `0xce7300` files in addition to the already published fixture batch. No edits or tests have been made for this follow-up yet.

## Remaining risk and next exact action

Preserve the function body and test behavior. Fix attribution, report `survey_index: 149` and `copy_manifest_index: 91`, and make the checker validate both sources and the decoded source/destination/data. Then run the focused CTest and ELF checker, update the completion checkpoint, run the publish-path audits, and push the allowlisted changes. This small metadata/checker correction does not establish new runtime or G1 behavior.
