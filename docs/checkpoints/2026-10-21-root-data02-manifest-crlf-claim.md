# Checkpoint: DATA-02-ARCHIVE-MANIFEST-CRLF-01 — claim

- Local date/time: 2026-10-21 11:34 CDT.
- Agent ID: `/root` (integrator); `review_host_proto` is assigned after publication.
- Task ID: `DATA-02-ARCHIVE-MANIFEST-CRLF-01`.
- Status: `ACTIVE`; failed tries: 0. No test has run for this task.
- Files changed by this claim: `HANDOFF.md`, `docs/TASKS.md`, and this checkpoint. Implementation scope is only `scripts/test_archive_overlap_map.py`.
- Graft command: `trail grep "line.endswith" --in scripts/`. It found the `line.endswith("\r")` CRLF stripping branch in `parse_manifest_bytes()` at `scripts/audit_archive_overlap_map.py:66`.
- Live verification: the parser splits JSONL on `\n`, strips one terminal `\r` from each record, and then parses it. Existing tests exercise LF-delimited records and have no CRLF case.
- Dependency: `DATA-02-ARCHIVE-OVERLAP-MAP-01` is `ARTIFACT_VERIFIED`; no duplicate task ID was present before this claim.
- Scope: add a synthetic two-record CRLF input and assert exact path/size output. Do not change production parser code; no real archive or manifest claim.
- Acceptance command: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_archive_overlap_map.py`.
- Observations versus inference: this fixture will pin the explicit CRLF acceptance branch. It cannot establish how live manifests are encoded or interpreted.
- Log/evidence paths: `local/validation/data02-manifest-crlf/{attempt-1.log,root-replay.log}`.
- Remaining risks: no archive, asset, schema, or runtime behavior claim.
- Next exact action: after publication, assign only `scripts/test_archive_overlap_map.py` to `review_host_proto`; root will replay and publish the result.
