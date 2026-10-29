# Task-register duplicate row cleanup (E976)

**Date:** 2026-10-29 09:48 CDT

**Agent:** `/root`

**Task:** `DOC-TASKS-SLOT309-DUPLICATE-ROW-01`

**Status:** `DOCS_VERIFIED`; one redundant task row removed, zero failed tries.

## Bounded change

The task register contained two rows with the same ID, `CPU-01-STARTUP-SLOT-309-CHARACTERIZATION-01`, at original lines 39 and 41. The later row includes E894's raw literal-census result and preserves E911's stop condition. I removed only the earlier row. A scripted check confirms the sole remaining row is byte-identical to the former later row; the task ID occurs exactly once after the edit.

No slot-309 analysis was performed. The row remains `ACTIVE`, keeps slot 309 unregistered, and retains the E911 rule to resume only on a concrete producer/consumer/order edge or principal reassessment. No code, binary, evidence artifact, or task status changed.

## Verification

The project status-cell parser after cleanup found 0 READY rows, 10 ACTIVE rows, 1 ACTIVE/PARTIAL row, and 15 WAITING_FOR_DEPENDENCY rows. The only change to the table's task statuses is elimination of the duplicated ACTIVE row.

## Publication verification

**2026-10-29 09:54 CDT — `/root`; 0 failed tries.** Changed `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/INDEX.md`, generated `docs/FOLDERS.md`, and this checkpoint. No dependency change. Precommit fetch verified `HEAD == origin/main == FETCH_HEAD == aaa53c737ee7cf8cafb28501cf57e89249967ea9`.

- `cmake -S native -B local/build/orch-r22` and `cmake --build local/build/orch-r22 -j 12`: both exited 0.
- `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure`: 487/487 passed.
- `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh`: 572 passed, 0 skipped, 0 failed.
- `trail build --check`: exit 0; wiring graph in sync with 10,779 deep-layer nodes pending.
- `python3 scripts/project_index.py` and `python3 scripts/project_index.py --check`: 43,124 folders indexed/current.
- `python3 -I scripts/audit_publish_paths.py`: 6/6 staged paths passed; `python3 -I scripts/audit_publish_paths.py --all`: 3,373/3,373 paths passed.
- `python3 -I scripts/check_commit_attribution.py` and `git diff --cached --check`: passed.
- The status-cell scan and exact retained-row comparison are logged at `local/validation/DOC-TASKS-SLOT309-DUPLICATE-ROW-01/result-publication/attempt-01/postcommit.log`.
- Exact command logs: `local/validation/DOC-TASKS-SLOT309-DUPLICATE-ROW-01/result-publication/attempt-01/`.

**Precommit gate result:** all publication gates passed. The doc-only result is ready for owner-authored commit/push and fetch verification; parent startup task and overall port remain incomplete.
