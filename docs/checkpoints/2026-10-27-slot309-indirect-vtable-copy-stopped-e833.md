# E833 — Stop proposed slot-309 indirect-copy lead

- **Date / local time:** 2026-10-27 16:20 CDT (America/Chicago).
- **Integrator:** `/root`.
- **Task:** `CPU-01-STARTUP-SLOT-309-INDIRECT-VTABLE-COPY-01`, screened as a possible one-callsite follow-up to the active slot-309 characterization.
- **Starting tree:** `main == origin/main == bbf731830fb4ca59b1fa905cd754b07e67d8ffb3` after E832. The unpublished E833 claim draft was converted to this disposition before any byte or Ghidra analysis.

## Gate disposition

E832 permits another slot-309 pass only when existing evidence supports a distinct concrete nonzero producer or semantic consumer. The proposed call at `0xa5054d` in `0xa50440` did not meet that bar. Existing E830 material identifies vtable slot 0 as helper `0xa50220`, whose observed operation is a copy; the proposal's source field `[rbx+0xb0]` is already zeroed before the call. Independent reviewer `/root/slot309_helper_caller_review` reached the same conclusion from the published E830/E832 findings and the proposed scope.

**Status: NOT RUN / STOPPED before analysis.** No fresh ELF verifier, Ghidra query, private analysis report, or source change was made for this candidate. This is a scope-gate disposition, not a failed semantic hypothesis.

## Repository verification

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

Configure/build exited 0; CTest passed **431/431**; lift checkers passed **516 / 0 skipped / 0 failed**; Trail wiring graph check passed at **9,877 nodes / 13,470 edges** (deep layer not built); project index regenerated and checked current at **31,948 folders**. The first index check exited 1 because the private E833 log folder was created after index generation. A second check detected two newly created index-output log files; its exact stale-index output is retained at `project-index-check-second-stale.log`. After precreating all remaining private log paths, project-index generation exited 0 and the final check passed. Exact logs are under `local/validation/orch-20261027/e833/`: first stale output `project-index-check.log`, second stale output `project-index-check-second-stale.log`, final generation `project-index-generate.log`, final check `project-index-check-final.log`, and CMake/CTest/lift/Trail logs.

## Remaining boundary and next action

Keep slot 309 unregistered, `+0x10` ownership and native-lift boundary UNKNOWN, and G1 PARTIAL. No game acceptance gate advances. Reassess this parent only when existing evidence supports a distinct concrete nonzero producer or semantic consumer; re-triage other lanes after an external dependency or input changes.

Attempts: **0 failed hypotheses; 0 analysis attempts**. No source, game binary, game input, persistent Ghidra project, or private stash was changed.
