# Completion: DOC-FOLDER-INDEX-SYMLINK-01

- Local time: 2026-10-21 11:07 CDT (America/Chicago). Integrator: `/root`; worker: `review_host_proto`.
- Task status / attempts: FIXTURE_VERIFIED; zero failed implementation tries.
- Files changed for this task: `scripts/test_project_tools.py`, this completion checkpoint, and the task/evidence/handoff records. Production indexer unchanged.
- Prerequisite: `DOC-FOLDER-INDEX-ADMIN-DIR-01` is FIXTURE_VERIFIED.

## Verification

- Worker command: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_project_tools.py`; exit 0, 7 tests. Log: `local/validation/doc-folder-index-symlink/attempt-1.log`; SHA-256 `bd8b6e98c3fde35b63ab6d268fdd3a11412f774f5efed5e707ad2bef28c91f16`.
- Root replay: same command; exit 0, 7 tests. Log: `local/validation/doc-folder-index-symlink/root-replay.log`; SHA-256 `55c443882838af1a647fbd8c514e974fa1fc55b0f5546ea5d7e5f0d11fdd0998`.
- Final test SHA-256: `b25538e8d67119cac0b4a5834c27a7e9b757c706a936081380b96f822ca3cb2b`. Production `scripts/project_index.py` remains at SHA-256 `394ec07b93f1120e4fe2f5459cc0663672ad0e2165ab87afaa568bacdf40b9d9`.
- The fixture creates an outside target directory and a symlink under a temporary indexed root; neither the link path nor its child appears in generated `folders.json`.
- `git diff --check` exited 0. Source graph is rebuilt and `trail build --check` passes; private logs: `local/checkpoints/root-round11-claims/trail-build-post-fixtures.log` and `trail-check-post-fixtures.log`.
- Folder-index checkpoint: `python3 scripts/project_index.py` and `python3 scripts/project_index.py --check` both exited 0 with 14,217 folders; generated `docs/FOLDERS.md` now includes this completion checkpoint. Logs: `local/checkpoints/root-round11-claims/index-final.log` and `index-final-check.log`.

## Limits and next action

- Observation: `inventory` calls `os.walk(root, followlinks=False)`. Inference: the fixture protects generated indexes from importing outside-root directory names through directory symlinks.
- Temporary directories only; `docs/FOLDERS.md` and the private recursive index are current. No game data or behavior claim.
- Next exact action: after publishing E136, continue with the next bounded task from the read-only scout.
