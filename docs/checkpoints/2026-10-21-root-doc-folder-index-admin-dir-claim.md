# Claim: DOC-FOLDER-INDEX-ADMIN-DIR-01

- Local time: 2026-10-21 10:28 CDT (America/Chicago).
- Agent / integrator: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`); worker after claim publication: `slot23_raw_xrefs`.
- Task status / attempts: ACTIVE; zero failed tries.
- Bounded scope: add one test to `scripts/test_project_tools.py` using `generate()` on a temporary root. Add nested `.git` and `__pycache__` directories with marker files, then assert `generate(root, check=True)` remains current. No source change, live index regeneration, or project behavior claim.
- Prerequisite: none.

## Claim-time evidence

- Graft command `trail ask "How does project_index inventory and check mode handle .git and __pycache__, and which tests cover ignored directories?" --source --in scripts/` located `inventory` at `scripts/project_index.py:27-40`, which prunes `.git` and `__pycache__` from directory traversal, and `generate(..., check=True)` at `:59-77`, which reports stale indexes. The existing `test_index_is_stable_and_detects_a_new_folder` at `scripts/test_project_tools.py:66-78` covers `.DS_Store` and newly added folders but not administrative directories.
- The synthetic fixture can verify both ignored directory names without touching the live folder tree. No production behavior change is requested.
- Checkout at claim: `main`, HEAD `fb284810b612cc97ed3397f02eec143e61876f3b`, equal to `origin/main`. `git status --short` shows only pre-existing escalated changes under `native/api/CMakeLists.txt` and `native/api/host/`; all are excluded from this task.

## Acceptance and boundaries

- Acceptance command: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_project_tools.py`.
- Exact task-test command/result so far: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_project_tools.py` has NOT RUN; no source or test code has changed for this task.
- Observation: the live inventory routine already excludes `.git` and `__pycache__`. Inference: a temp-root check can prove these subtrees are omitted from stale-index comparison.
- Remaining limits: this verifies synthetic folder-index behavior only; it does not verify the live project inventory or Graft/source graph. Keep focused logs under `local/validation/doc-folder-index-admin-dir/`.
- Files changed to claim: `HANDOFF.md`, `docs/TASKS.md`, generated `docs/FOLDERS.md`, the PSARC completion-checkpoint publication follow-up, and this claim checkpoint. No source/test code changes.
- Project-index command `python3 scripts/project_index.py` exited 0 and reported 14,207 folders indexed; it generated `docs/FOLDERS.md`, updating `docs/checkpoints` from 234 to 235 files. Log: `local/checkpoints/root-doc-folder-index-admin-dir-claim/project-index.log`.
- Source graph command `trail build --check` exited 0 and reported the wiring graph in sync. The deep layer is not built and 3,520 meaning-tier nodes remain pending; this task changes no source symbols. Log: `local/checkpoints/root-doc-folder-index-admin-dir-claim/trail-check.log`.
- Claim commit gates: `python3 -I scripts/audit_publish_paths.py` exited 0 for 5 staged allowlisted paths; `python3 -I scripts/audit_publish_paths.py --all` exited 0 for 1,501 allowed tracked paths; `git diff --cached --check` exited 0. Logs are `local/checkpoints/root-doc-folder-index-admin-dir-claim/{audit-staged.log,audit-all.log,cached-check.log}`. Staged paths are limited to the claim, task/handoff/index documentation, and the PSARC publication follow-up. Escalated host files remain unstaged.
- Next exact action after claim publication: assign only `scripts/test_project_tools.py` to the worker; review its diff and rerun the acceptance command independently, then record the result and refresh project/source indexes.
