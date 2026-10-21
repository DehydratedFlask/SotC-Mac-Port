# Checkpoint: AUD-PUBLISH-GIT-NUL-FILENAME-FIXTURE-01 — claim

- Local date/time: 2026-10-21 11:59 CDT.
- Agent ID: `/root` (integrator).
- Task ID: `AUD-PUBLISH-GIT-NUL-FILENAME-FIXTURE-01`.
- Status: `ACTIVE`; failed tries: 0.
- Files changed at claim time: shared task/handoff documentation only; no implementation edit yet.
- Graft review: `trail ask "NUL-delimited git_names filename decoding with embedded newline coverage in audit_publish_paths tests" --source --in scripts/` located the related audit/test files; `trail grep "git_names"` identified `scripts/audit_publish_paths.py:L75-L83`; `trail skeleton scripts/test_audit_publish_paths.py` listed the full test API. Live source confirms `git_names("staged")` invokes `git diff --cached --name-only -z`, decodes each NUL-delimited byte path, and uses `cwd=ROOT`. The existing test module covers `violations()` only and has no `git_names()` fixture.
- Scope: add one test method in `scripts/test_audit_publish_paths.py` that mocks `audit.subprocess.check_output` to return `b"scripts/name\nwith-newline.py\0"`; assert `audit.git_names("staged") == ["scripts/name\nwith-newline.py"]` and assert the exact staged command and `cwd=audit.ROOT`. Do not change `scripts/audit_publish_paths.py` or run the Git audit against game files.
- Target command: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_audit_publish_paths.py`.
- Prerequisites: `AUD-PUBLISH-EDGE-01`, `AUD-PUBLISH-NESTED-01`, and `AUD-PUBLISH-SECRET-NAME-FIXTURE-01` are `FIXTURE_VERIFIED`; no matching NUL-filename test or task is registered.
- Acceptance limits: mocked Git-name parsing only; no real staged path or repository publication behavior is established.
- Remaining risks: test has not been implemented or run yet.
- Next exact action: after this claim is published, assign only `scripts/test_audit_publish_paths.py` to `/root/review_host_proto`; worker saves its log under `local/validation/aud-publish-git-nul-filename/` and makes no shared-document edits.
