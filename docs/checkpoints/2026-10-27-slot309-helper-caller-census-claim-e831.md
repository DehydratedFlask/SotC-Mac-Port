# E831 — Claim slot-309 helper caller census

- **Date / local time:** 2026-10-27 15:59 CDT (America/Chicago).
- **Integrator:** `/root`.
- **Task:** `CPU-01-STARTUP-SLOT-309-HELPER-CALLER-CENSUS-01`, a bounded continuation of the active slot-309 characterization.
- **Starting tree:** E830 commit `3a0d4e52588dc2ca59227fe83a554f381c0a0f8b` is pushed/fetched; `HEAD == origin/main`.
- **Claimed scope:** use the existing saved Ghidra ReferenceManager in `-noanalysis -readOnly` mode to enumerate incoming call references to `0xa50280` and `0xa50440`. For every reported caller, inspect only enough exact pinned ELF bytes to identify argument setup for the source object's `+0x10` qword and determine whether a nonzero seed is statically proven. If a concrete nonzero seed is found, trace only its corresponding copy/use/cleanup path.
- **Acceptance:** record the full saved-reference caller sets, exact caller address/range and ELF hash, byte-level argument mapping, saved-project command/exit, reproducible private output hashes, and explicit unknowns. Independently replay the query and request a separate review. If argument values are dynamic or evidence is incomplete, keep those values UNKNOWN and report the coverage boundary.
- **Limits:** distinct from E829's query of vtable targets. No new binary import, persistent-project mutation, game execution, asset extraction, source modification, slot registration, or general ownership claim from unresolved dataflow. Keep slot 309 unregistered and G1 **PARTIAL**.
- **Attempts:** 0 failed hypotheses; 0 tries for this bounded caller census.

## Publication baseline

E830 owner commit `3a0d4e52588dc2ca59227fe83a554f381c0a0f8b` is pushed and fetched at `origin/main`.
The post-E830 public-ref scan found 9 branches, 12 PR heads, 0 tags, 15 unique tips, and **2,443
reachable commits**, with zero Claude-family author/committer identity or credit hits. Private stash
`92ceba574bd1e8445b9aa4f728f878fbfd4f57f8` remains unchanged per owner instruction. E830 left the
slot-309 parent task ACTIVE, with the copied `+0x10` field's ownership UNKNOWN.

## Prepublication repository verification

Commands run from the workspace root:

```sh
cmake -S native -B local/build/orch-r22
cmake --build local/build/orch-r22 -j 12
ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure
SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh
trail build --check
python3 scripts/project_index.py
python3 scripts/project_index.py --check
```

Configure/build exited 0 (`ninja: no work to do`); CTest passed **431/431**; lift checkers passed
**516 / 0 skipped / 0 failed**; Trail graph check passed at **9,877 nodes / 13,470 edges** (deep
layer not built); project index regenerated and checked current at **31,945 folders**. Logs are
private under `local/validation/orch-20261027/e831/`. These checks do not advance a game gate.
