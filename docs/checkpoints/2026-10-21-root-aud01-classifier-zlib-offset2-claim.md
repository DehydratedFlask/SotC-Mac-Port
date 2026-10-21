# Checkpoint: AUD-01-CLASSIFIER-ZLIB-OFFSET-02-FIXTURE-01 — claim

- Local date/time: 2026-10-21 11:24 CDT.
- Agent ID: `/root` (integrator); `review_host_proto` is assigned after claim publication.
- Task ID: `AUD-01-CLASSIFIER-ZLIB-OFFSET-02-FIXTURE-01`.
- Files changed: `docs/TASKS.md`, `HANDOFF.md`, this checkpoint, and generated `docs/FOLDERS.md`. No production source or test file has changed.
- Status: `ACTIVE`; failed tries: 0. The test has not run.
- Graft command: `trail grep "zlib_offset2" --in scripts/`. It located `decompress_probe()` and the two-byte zlib probe in `scripts/classify_audio_headers.py:81-127`. The scout and root review found the current real-zlib test only exercises offset 0; this fixture is absent.
- Scope: add a synthetic two-byte-prefix plus zlib-compressed payload case to `scripts/test_audio_headers.py`, assert the `zlib_offset2` result and decoded length, and leave production code unchanged.
- Acceptance command: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_audio_headers.py`.
- Repository baseline command: `git status --short && git rev-parse HEAD && git rev-parse origin/main` exited 0; HEAD and `origin/main` both resolved to `59dad26d20bc89fd64e5f74e9a94deba9d93147b`.
- Observations: classifier source tries zlib at offsets 0 and 2; existing test coverage only asserts offset 0. Inference: a synthetic prefix fixture pins the offset-2 branch. It cannot establish whether any game asset uses this framing or whether audio can play.
- Log/evidence paths: query result and live review are summarized in `HANDOFF.md`; test logs will be kept under `local/validation/aud01-classifier-zlib-offset2/`; claim publication logs under `local/checkpoints/root-round14-aud01-claim/`.
- Remaining risks: no real archive, codec, NGS2, runtime, or playback claim is made. Existing escalated `native/api/CMakeLists.txt` and untracked `native/api/host/` remain outside this task.
- Next exact action: regenerate and check the folder index, run `trail build --check` and `git diff --check`, stage only allowlisted claim documentation, pass both publication audits, commit and push, then assign the test-only file to `review_host_proto`.
