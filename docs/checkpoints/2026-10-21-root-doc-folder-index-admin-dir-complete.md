# Completion: DOC-FOLDER-INDEX-ADMIN-DIR-01

- Local time: 2026-10-21 10:32 CDT (America/Chicago).
- Integrator: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`); test worker: `slot23_raw_xrefs`.
- Task status / attempts: FIXTURE_VERIFIED; zero failed tries. This is test-only coverage.
- Scope: `scripts/test_project_tools.py` now includes `test_index_ignores_nested_admin_directories`. In a temporary root, it generates an index, adds nested `.git` and `__pycache__` directories containing marker files, then confirms `generate(root, check=True)` remains current.

## Verification

- Worker command: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_project_tools.py`; exit 0, 5 tests passed. Log: `local/validation/doc-folder-index-admin-dir/attempt-1.log`, SHA256 `6299c61ecefc2c6e262ba2dccd92b35e4ade4e41b723fe2eac602dbd7b324b98`.
- Root replay of the same command: exit 0, 5 tests passed. Log: `local/validation/doc-folder-index-admin-dir/root-replay.log`, SHA256 `d1aaab86fa02d87997171aeb3220b40f5e28046ec4d1674848f9ec5f589f297c`.
- Test file SHA256: `9b385a4578100833be17a70060839aec84487028a35dc04fe0e62f46dc6126e6`. No production source changed.
- Observation: the fixture passes after adding administrative marker directories under a nested project folder. Inference: check mode excludes these nested directories as intended by the existing inventory pruning.
- Limits: temporary synthetic root only; no claim about the live project folder inventory, build products, Graft graph behavior outside its recorded check, or any game runtime gate.

## Index and graph checkpoint

- First `trail build --check` during the worker edit exited 1 with `STALE`: `scripts/test_project_tools.py` and its `ToolChecks` node had changed, and the new test symbol was absent. This was a graph freshness result, not a failed test or implementation hypothesis. Captured in `local/checkpoints/root-doc-folder-index-admin-dir-claim/trail-check-final.log`.
- `python3 scripts/project_index.py` exited 0 and reported 14,208 folders indexed. Log: `local/checkpoints/root-doc-folder-index-admin-dir-claim/project-index-final.log`.
- `trail build` exited 0 after parsing 1,235 source files; log: `local/checkpoints/root-doc-folder-index-admin-dir-claim/trail-build.log`.
- Subsequent `trail build --check` exited 0 with the wiring graph in sync; 3,521 meaning-tier nodes remain pending because the deep layer was not built. Log: `local/checkpoints/root-doc-folder-index-admin-dir-claim/trail-check-final.log`.
- Final project-index command `python3 scripts/project_index.py` exited 0 and reported 14,208 folders indexed; `docs/FOLDERS.md` now records 236 files under `docs/checkpoints`. Log: `local/checkpoints/root-doc-folder-index-admin-dir-claim/project-index-complete.log`.
- Final `trail build --check` exited 0 with the wiring graph in sync; the deep layer is not built and 3,521 meaning-tier nodes remain pending. Log: `local/checkpoints/root-doc-folder-index-admin-dir-claim/trail-check-complete.log`.
- Publication gates: `python3 -I scripts/audit_publish_paths.py` exited 0 for 7 staged allowlisted paths; `python3 -I scripts/audit_publish_paths.py --all` exited 0 for 1,502 allowed tracked paths; `git diff --cached --check` exited 0. Logs: `local/checkpoints/root-doc-folder-index-admin-dir-claim/{completion-audit-staged.log,completion-audit-all.log,completion-cached-check.log}`. The staged set contains only `HANDOFF.md`, `docs/EVIDENCE.md`, `docs/FOLDERS.md`, `docs/TASKS.md`, the claim and completion checkpoints, and `scripts/test_project_tools.py`. Pre-existing escalated `native/api/CMakeLists.txt` and `native/api/host/` remain unstaged.
- Publication follow-up checkpoint:
  - Local time: 2026-10-21 10:33 CDT (America/Chicago). Integrator: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`). Task `DOC-FOLDER-INDEX-ADMIN-DIR-01`; status FIXTURE_VERIFIED, zero failed tries.
  - Exact command: `git push origin main`; exit 0, advanced `51f6549..94a8cbc`. `git rev-parse HEAD` and `git rev-parse origin/main` both returned `94a8cbc922a32ab37d8215cbac16d98c6f56077f`. Push output was not persisted to a separate log.
  - Observation: test and evidence changes are published. Remaining limits are the synthetic-only scope recorded above; no runtime/game gate advanced.
  - Next exact action: claim another bounded non-escalated task and keep the host and slot-23 blockers unchanged.
