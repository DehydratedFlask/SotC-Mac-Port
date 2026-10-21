# Claim: DATA-02-OVERLAP-MAP-UNSHARED-SIZE-VALIDATION-01

- Local time: 2026-10-21 10:14 CDT (America/Chicago).
- Agent / integrator: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`); worker after claim publication: `launcher_review`.
- Task status / attempts: ACTIVE; 0 failed implementation tries.
- Bounded scope: make `compare_member_maps` enforce its existing nonnegative integer size contract across every supplied map entry, including paths not shared by another archive. Add synthetic tests for a negative integer and a boolean size on an unshared path. Keep edits to `scripts/audit_archive_overlap_map.py` and `scripts/test_archive_overlap_map.py`. No live manifest/archive data or runtime claims.
- Prerequisite: `DATA-02-ARCHIVE-OVERLAP-MAP-01` is ARTIFACT_VERIFIED.

## Claim-time evidence

- Graft command `trail ask "How does compare_member_maps validate member sizes across archives, especially unshared paths?" --source --in scripts/` refreshed two indexed files. It located `compare_member_maps` at `scripts/audit_archive_overlap_map.py:96-138`. The function validates `type(size) is int` and `size >= 0` only while iterating a path shared between a particular archive pair; unshared members bypass that check.
- Graft command `trail callers compare_member_maps --depth all --in scripts/` found `build_report`, four synthetic tests, and the module `main` call chain; there are no other indexed source callers.
- Existing synthetic tests cover disjoint valid maps, equal/different shared sizes, and stable pair order at `scripts/test_archive_overlap_map.py:125-163`. They do not cover invalid sizes in unshared entries.
- Checkout at claim: `main`, HEAD `35370fcb394ade4087aa079fe3530a46f72bce7f`, matching `origin/main`. `git status --short` shows only the pre-existing escalated `native/api/CMakeLists.txt` and untracked `native/api/host/`; both are excluded from this task and commits.

## Acceptance and boundaries

- Test-first evidence: add synthetic maps with an unshared path whose size is `-1` and `True`; run the focused test to demonstrate the assertions fail on current code, then validate all map entries before comparing pairs.
- Acceptance command: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_archive_overlap_map.py`.
- Observation: the helper documents nonnegative integer member sizes, and the existing shared-path check explicitly uses `type(size) is int` to exclude booleans. Inference: the same contract should apply to every map entry regardless of overlap.
- Exact commands/results so far: no acceptance test has been run for this task; baseline is NOT RUN. No logs or code changes exist yet. `python3 scripts/project_index.py` exited 0 and reported 14,204 folders indexed; `trail build --check` exited 0 with the wiring graph in sync, deep layer not built, and 3,518 meaning-tier nodes pending.
- Remaining risks: synthetic helper-input validation only; it does not change manifest parsing (which already rejects invalid sizes), archive contents, overlap semantics, or game behavior. Keep private logs under `local/validation/data02-overlap-map-unshared-size/`.
- Files changed to claim: `HANDOFF.md`, `docs/TASKS.md`, generated `docs/FOLDERS.md` (checkpoint file count 230→231), the completed ELF checkpoint's push record, and this claim checkpoint. No source/test code changed.
- Publication gates: `python3 -I scripts/audit_publish_paths.py` exited 0 for five staged allowlisted paths; `python3 -I scripts/audit_publish_paths.py --all` exited 0 for 1,497 allowed tracked paths; `git diff --cached --check` exited 0. Only these claim/handoff/index paths are staged; escalated host-protocol files remain unstaged.
- Next exact action: commit and push this claim, then assign the two owned files to `launcher_review`.
