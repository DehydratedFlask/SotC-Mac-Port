# Claim: DOC-FOLDER-INDEX-CHECK-READONLY-01

- Local time: 2026-10-21 11:16 CDT (America/Chicago).
- Agent / integrator: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`); worker after claim publication: `slot23_raw_xrefs`.
- Task status / attempts: ACTIVE; 0 failed implementation tries.
- Bounded scope: add a test in `scripts/test_project_tools.py` using an empty temporary root. Call `generate(root, check=True)`, assert the exact stale-index error names both outputs, and assert neither `docs/` nor `local/` was created. Edit only the test file. No indexer change or live project inventory claim.
- Prerequisite: none.

## Claim-time evidence

- Graft command `trail ask "What does project_index.generate(root, check=True) do when generated index outputs are missing, and do tests verify it avoids writes?" --source --in scripts/` returns `scripts/project_index.py:59-77`: check mode skips parent creation, tests both output paths for existence/content, and raises `ValueError("Stale indexes: ...")` when stale.
- Live test inspection (`sed -n '1,150p' scripts/test_project_tools.py`) shows `test_index_is_stable_and_detects_a_new_folder` first calls `generate(root)`, creating `docs/` and `local/indexes/`; no test covers an empty root in check mode.
- Observation: check mode appears read-only in this path. The planned temporary-root test will demonstrate that it reports missing outputs and leaves the output parent directories absent. No production fix is in scope.
- Registry search found no existing task with this ID or scope. Checkout at claim: branch `main`, HEAD `7028abeb774724cd4ab5a3ebabc9068a762f644b`, matching `origin/main`. Only pre-existing escalated `native/api/CMakeLists.txt` and untracked `native/api/host/` are dirty; exclude them.

## Acceptance and boundaries

- Acceptance command: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_project_tools.py`.
- Baseline status: NOT RUN for this fixture. Worker may edit only `scripts/test_project_tools.py`.
- Private evidence path: `local/validation/doc-folder-index-check-readonly/`.
- Files changed to claim: `HANDOFF.md`, `docs/TASKS.md`, generated `docs/FOLDERS.md`, and this checkpoint. No test or indexer source changed at claim time.
- Publication gates and claim commit are pending. Do not stage or change user inputs, local indexes, or the host escalation paths.

## Claim publication and assignment checkpoint

- Local date/time: 2026-10-21 11:16 CDT (America/Chicago). Integrator: `/root`; worker after publication: `slot23_raw_xrefs`.
- Exact publication command/result: `git push origin main` exited 0 and advanced
  `7028abe..f427b2b`. Commit `f427b2bb412379a54feb7e5f3af27420420a31da` is on `origin/main`; root
  verified HEAD equals `origin/main`.
- Publication checks: staged audit accepted five allowlisted files; full tracked-path audit accepted
  1,516 files; `git diff --cached --check` exited 0. `python3 scripts/project_index.py` indexed
  14,222 folders and `--check` reports current. `trail build --check` reports the graph in sync at
  3,528 nodes (deep meaning layer remains unbuilt). Logs are in `local/checkpoints/root-round13-claims/`.
- Publication-record follow-up: `git push origin main` exited 0 and advanced `f427b2b..95642f0`.
  Commit `95642f017d58c2fccaf1cd2398a32077230a5d8d` records both claim-publication results; HEAD
  equals `origin/main`.
- Observation: worker scope is only `scripts/test_project_tools.py`; no test has run for this task.
- Remaining risks: synthetic empty-root check only; no live index regeneration or repository inventory claim.
- Next exact action: assign the test file, then review the worker's diff and log and independently replay the acceptance command.
