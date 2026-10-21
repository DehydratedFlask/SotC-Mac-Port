# Checkpoint: AUD-PUBLISH-GIT-NUL-FILENAME-FIXTURE-01 — complete

- Local date/time: 2026-10-21 12:02 CDT.
- Agent ID: `/root` (integrator); implementation by `/root/review_host_proto`.
- Task ID: `AUD-PUBLISH-GIT-NUL-FILENAME-FIXTURE-01`.
- Status: `FIXTURE_VERIFIED`; failed tries: 0.
- Changed file: `scripts/test_audit_publish_paths.py` only. A mocked `subprocess.check_output` returns one NUL-terminated path containing an embedded newline. The test asserts that `git_names("staged")` returns one path with the newline intact and that the exact command is `git diff --cached --name-only -z` with `cwd=audit.ROOT`.
- Worker command: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_audit_publish_paths.py`; exit 0, 8/8. Log `local/validation/aud-publish-git-nul-filename/attempt-1.log`, SHA256 `2311584f5362a1c70d8f10891323af40f8647e0f19fa7d0d006c67482d2f4d38`.
- Root replay command: `set -o pipefail; PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_audit_publish_paths.py 2>&1 | tee local/validation/aud-publish-git-nul-filename/root-replay.log`; exit 0, 8/8. Log SHA256 `f8f7b0ac7f2b53ac4e96000f166430c488f2c37e059ad5502b09d60f29d363b8`.
- Test SHA256: `2759f5b9f554388ca7fd95ab2ee6457d16af995d3f0e86bbe85458422e65d2ac`. Production audit `scripts/audit_publish_paths.py` is unchanged at SHA256 `2ea984cfb38329f4a9ec0b46300b0513998a1a642d224572906cee531653bba3`. `git diff --check` passed; E148 records both runs and hashes.
- Observation: the helper's NUL split preserves a newline inside one returned path. Inference limit: the test mocks Git and does not exercise a real index or publication audit.
- Remaining risks: no actual staged pathname was audited, and no game/runtime acceptance claim follows.
- Graph evidence: `trail build` exited 0 and reported 3,537 nodes / 4,758 edges; `trail build --check` exited 0 after the rebuild.
- Next exact action: run `python3 scripts/project_index.py` and `python3 scripts/project_index.py --check`, then stage only this test and its task/evidence/checkpoint records, run both publication audits, commit, and push.
