# Claim: DATA-02-OVERLAP-MAP-JSON-CONSTANTS-01

- Local time: 2026-10-21 11:00 CDT (America/Chicago). Integrator: `/root`; worker after claim publication: `launcher_review`.
- Task status / attempts: ACTIVE; zero failed implementation tries. No test has run for this task.
- Bounded scope: add synthetic coverage in `scripts/test_archive_overlap_map.py` for JSONL values `NaN`, `Infinity`, and `-Infinity`, asserting the exact `ManifestError` message `synthetic: malformed JSONL record at line 1`. Do not change parser behavior. Worker owns only the test file; root owns project/task records.
- Prerequisite: `DATA-02-ARCHIVE-OVERLAP-MAP-01` is ARTIFACT_VERIFIED. Existing `DATA-02` synthetic fixtures are complete.

## Claim-time evidence

- Graft command: `trail grep "_reject_constant" --in scripts/`. It located `_reject_constant` at `scripts/audit_archive_overlap_map.py:50-51` and `parse_manifest_bytes` at `:54-93`; the parser supplies `_reject_constant` as `json.loads(parse_constant=...)`. The live source at these lines raises `ValueError` for non-standard constants and wraps it as the exact malformed-record `ManifestError` above.
- Existing parser fixtures are in `scripts/test_archive_overlap_map.py`; baseline SHA-256: `44750175cef31076a8c068496479e06f22c1d3f4fd6876ab7a5dc9462b81ec8b`. Production parser baseline SHA-256: `25889f7ff85db9154ba209bed4913b1bb9e1a627e8a984ef7c11580b24e982d1`.
- Checkout at claim: branch `main`, HEAD `667a88cda4c844a323b81c9adafdb9ffb5e63583`, matching `origin/main`. Only pre-existing `native/api/CMakeLists.txt` and `native/api/host/` changes are present and excluded.

## Acceptance and boundaries

- Acceptance command: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_archive_overlap_map.py`. Expected: three synthetic constants each raise `ManifestError` with the exact line-1 message; parser unchanged.
- Observation: Python's JSON parser invokes the configured `parse_constant` hook for these unquoted constants, and the live wrapper catches its `ValueError`. Inference: the fixture pins this existing strict-JSON behavior against regression.
- Limits: no private manifest, archive payload, digest interpretation, game behavior, or production parser change. No game acceptance gate advances.
- Next exact action: after claim publication, assign only `scripts/test_archive_overlap_map.py` to `launcher_review`; root will inspect the diff, replay the acceptance command, and record the completion checkpoint and E134.
- Claim-index command: `python3 scripts/project_index.py` exited 0 and wrote the generated index; 14,214 folders indexed. Log: `local/checkpoints/root-round11-claims/index.log`.
- Claim-graph command: `trail build --check` exited 0; wiring graph in sync. Deep layer remains unbuilt. Log: `local/checkpoints/root-round11-claims/trail-check.log`.
- Repository-state command: `git status --short` showed only the three intended documentation/index changes plus the pre-existing `native/api/CMakeLists.txt` and untracked `native/api/host/`; log: `local/checkpoints/root-round11-claims/repo-state.log`.
