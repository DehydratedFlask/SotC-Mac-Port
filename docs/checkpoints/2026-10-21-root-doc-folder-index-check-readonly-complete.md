# Completion: DOC-FOLDER-INDEX-CHECK-READONLY-01

- Local time: 2026-10-21 11:20 CDT (America/Chicago).
- Agent / integrator: worker `slot23_raw_xrefs`; integrator `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`).
- Task status / attempt count: FIXTURE_VERIFIED; 0 failed implementation tries.
- Files changed: worker changed only `scripts/test_project_tools.py`; root updated `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, generated `docs/FOLDERS.md`, and this checkpoint. Production `scripts/project_index.py` is unchanged.

## Result

- An empty `TemporaryDirectory` is passed to `generate(root, check=True)`. The fixture asserts the exact `ValueError` message `Stale indexes: docs/FOLDERS.md, local/indexes/folders.json`, then checks both `docs/` and `local/` remain absent.
- Observation: check mode reports the two missing outputs without creating their parent directories. This is limited to the synthetic empty-root case.
- Acceptance command: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_project_tools.py`.
- Worker result: exit 0, 8 tests passed. Log `local/validation/doc-folder-index-check-readonly/attempt-1.log`, SHA256 `3d05864adaa7a067c6dd8f488b76741afad7cdf90e6cdbe6566339424de57b0f`.
- Root replay: exit 0, 8 tests passed. Log `local/validation/doc-folder-index-check-readonly/root-replay.log`, SHA256 `316558a93ba028238bcd514bbec8a07c6b5122f2fa8e4ce8499b5d85bc6111f8`.
- Test SHA256 `ea02cf8cc4c1f603f8ea5b6b513d03f0f449ac6848f333444562603fb4a30fc4`; indexer SHA256 `394ec07b93f1120e4fe2f5459cc0663672ad0e2165ab87afaa568bacdf40b9d9` (unchanged). `git diff --check -- scripts/test_project_tools.py` passed.
- Graph refresh was shared with the PSARC fixture: initial check stale, `trail build` exited 0 at 3,530 nodes / 4,746 edges, final `trail build --check` exited 0. Deep meaning layer remains unbuilt.

## Limits and checkpoint

- This is a synthetic empty-root check-mode assertion only. No live project index or repository inventory was changed or validated.
- No failed implementation tries; no production indexer change.
- Remaining risk: check-mode behavior for other stale/malformed output contents is outside this task.
- Publication command/result: `git push origin main` exited 0 and advanced `c0a2df6..42af4f2`;
  commit `42af4f281e2d310024bd2ce1f99a3784bae2371e` is now on `origin/main`, and root verified HEAD
  equals `origin/main`.
- Publication checks: staged audit accepted eight allowlisted paths; full tracked-path audit accepted
  1,518 paths; `git diff --cached --check` exited 0. `python3 scripts/project_index.py` and
  `--check` report 14,222 folders current. The final `trail build --check` passes at 3,530 nodes /
  4,746 edges; the deep meaning layer remains unbuilt. The stale graph check and rebuild are recorded
  in the sibling PSARC checkpoint. Command logs are in `local/checkpoints/root-round13-claims/`.
- Next exact action: continue with the next independent unclaimed task.
