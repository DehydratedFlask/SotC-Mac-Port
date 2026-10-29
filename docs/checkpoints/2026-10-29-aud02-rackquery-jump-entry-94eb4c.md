# AUD-02 RackQuery dispatch-entry review at `0x94eb4c` — claim (E968)

**Date:** 2026-10-29 08:38 CDT
**Agent:** `/root`
**Task:** `AUD-02-NGS2-RACKQUERY-JUMP-ENTRY-94EB4C-01`
**Status:** `ACTIVE`; claim only, 0 completed semantic hypotheses, 0 failed semantic hypotheses.

## Bounded output

Verify the Ghidra-reported candidate computed-jump entry `0x94eb4c` from dispatch at `0x94f1ac`, trace only that selected route to RackQuery call `0x94ebc2`, and determine RBX's reaching definition at `0x94eb78`. Independently compare relevant instruction bytes and direct flow targets against the pinned ELF. Keep the result UNKNOWN if the entry, route, or reaching definition cannot be established.

Do not enumerate sibling indirect destinations, trace the negative branch from `0x94f18a` to `0x94f370`, infer an NGS2 ABI/semantic name, or claim codec, playback, runtime, or product behavior.

## Claim and preconditions

- Base is clean `main` at `409ed112cf1d285dd4973756c7370a915d367dba`; fetch verified `HEAD == origin/main == FETCH_HEAD`.
- Pinned plaintext ELF: `local/re/eboot.elf`, SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`; Ghidra saved project `local/ghidra/persistent-20261020`, version 12.1.4, image base 0.
- E967's independently verified observations are limited to callee `0x954d70` returning zero-extended RDX in `0..14`, a nonnegative caller dispatch that sets RDX to a jump-table base at `0x94f1a2`, Ghidra's report of `0x94eb4c` as one possible dispatch entry, and `MOV RDX,RBX` at `0x94eb78` before call `0x94ebc2`. E967 did not establish this entry's actual route or RBX's value.
- Claim changes only the allowlisted docs listed in E968. No source edits, local game execution, or game/firmware acquisition.

## Observation, uncertainty, and next action

**Observation:** the current saved-analysis report names `0x94eb4c` as a possible target of the computed jump at `0x94f1ac`; the selected RackQuery path consumes RBX through `MOV RDX,RBX` at `0x94eb78`.

**Unknown:** whether the pinned instruction bytes and control flow confirm this entry, whether the selected route reaches `0x94ebc2`, and which value reaches RBX at the move.

**Next exact action:** use the existing Ghidra project read-only to capture the selected block path and reaching-definition chain for RBX, then independently verify those bytes and edges against the pinned ELF. Do not inspect other jump-table entries.

## Claim publication gates

- `cmake -S native -B local/build/orch-r22` and `cmake --build local/build/orch-r22 -j 12`: both exited 0.
- `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure`: 487/487 passed.
- `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh`: 572 passed, 0 skipped, 0 failed.
- `trail build --check`: exited 0; wiring graph in sync, 10,779 deep-layer nodes pending.
- `python3 scripts/project_index.py` and `python3 scripts/project_index.py --check`: 43,102 folders indexed; current.
- `python3 -I scripts/audit_publish_paths.py`: passed 6/6 staged paths; `python3 -I scripts/audit_publish_paths.py --all`: passed 3,370/3,370 paths.
- `python3 -I scripts/check_commit_attribution.py` passed (`origin/main..HEAD`); `git diff --cached --check` passed; `python3 scripts/project_index.py --check` confirmed 43,102 folders current.
- Exact logs/status files are under `local/validation/AUD-02-NGS2-RACKQUERY-JUMP-ENTRY-94EB4C-01/claim-publication/attempt-01/`.

## Claim publication closeout (E969)

- Owner-authored commit `12ad9e2ad6fe6f969bdea753c5c5a1df4a659621` (`docs: claim RackQuery dispatch entry review`) changed six allowlisted docs paths and was pushed to `origin main`; fetch verified `HEAD == origin/main == FETCH_HEAD`, with a clean worktree.
- Postcommit `python3 -I scripts/audit_publish_paths.py --all` passed 3,370/3,370; attribution, `git show --format= --check HEAD`, and `python3 scripts/project_index.py --check` (43,102 folders current) passed.
- Logs are under `local/validation/AUD-02-NGS2-RACKQUERY-JUMP-ENTRY-94EB4C-01/claim-publication/attempt-02/`. This closes publication only; the static task remains ACTIVE with 0 completed and 0 failed semantic hypotheses.
