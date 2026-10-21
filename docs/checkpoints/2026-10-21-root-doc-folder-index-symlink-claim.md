# Claim: DOC-FOLDER-INDEX-SYMLINK-01

- Local time: 2026-10-21 11:00 CDT (America/Chicago). Integrator: `/root`; worker after claim publication: `review_host_proto`.
- Task status / attempts: ACTIVE; zero failed implementation tries. No test has run for this task.
- Bounded scope: add one synthetic temporary-root regression in `scripts/test_project_tools.py` with a directory symlink pointing outside the indexed root; assert the generated folder index does not include the link or target child. Do not change production indexing code. Worker owns only the test file; root owns project/task records.
- Prerequisite: `DOC-FOLDER-INDEX-ADMIN-DIR-01` is FIXTURE_VERIFIED.

## Claim-time evidence

- Graft command: `trail grep "inventory" --in scripts/`. It located `inventory` at `scripts/project_index.py:27-40`, called by `generate` at `:59-77`. Live code uses `os.walk(root, followlinks=False)`. Existing folder-index tests cover stability/new folders and nested `.git`/`__pycache__`, but no directory symlink.
- Existing tests are in `scripts/test_project_tools.py`; baseline SHA-256: `e0edd276090e436377d2bf74dc6b852ca5aa742b0172906f8fa069a72058b51a`. Production `scripts/project_index.py` is unchanged; baseline SHA-256: `394ec07b93f1120e4fe2f5459cc0663672ad0e2165ab87afaa568bacdf40b9d9`.
- Checkout at claim: branch `main`, HEAD `667a88cda4c844a323b81c9adafdb9ffb5e63583`, matching `origin/main`. Only pre-existing `native/api/CMakeLists.txt` and `native/api/host/` changes are present and excluded.

## Acceptance and boundaries

- Acceptance command: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_project_tools.py`. Expected: the outside-target symlink and its child do not appear in the generated private inventory; existing fixtures continue to pass.
- Observation: `inventory` explicitly disables symlink traversal. Inference: the fixture will detect a future change that follows directory symlinks and imports outside-root directory names into generated indexes.
- Limits: synthetic temporary directories only; no live repository folder index or game data is changed by the test. No game acceptance gate advances.
- Next exact action: publish all three claims, then assign only `scripts/test_project_tools.py` to `review_host_proto`; root will inspect the diff, replay the acceptance command, and record the completion checkpoint and E136.
- Claim-index command: `python3 scripts/project_index.py` exited 0 and wrote the generated index; 14,214 folders indexed. Log: `local/checkpoints/root-round11-claims/index.log`.
- Claim-graph command: `trail build --check` exited 0; wiring graph in sync. Deep layer remains unbuilt. Log: `local/checkpoints/root-round11-claims/trail-check.log`.
- Repository-state command: `git status --short` showed only the three intended documentation/index changes plus the pre-existing `native/api/CMakeLists.txt` and untracked `native/api/host/`; log: `local/checkpoints/root-round11-claims/repo-state.log`.
- Source-hash command: `shasum -a 256 scripts/project_index.py`; result and path are in `local/checkpoints/root-round11-claims/project-index-source.sha256`.
