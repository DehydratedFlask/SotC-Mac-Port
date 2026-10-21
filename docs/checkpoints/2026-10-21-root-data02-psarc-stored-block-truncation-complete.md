# Completion: DATA-02-PSARC-STORED-BLOCK-TRUNCATION-01

- Local time: 2026-10-21 10:23 CDT (America/Chicago).
- Integrator: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`); test worker: `slot23_raw_xrefs`.
- Task status / attempts: FIXTURE_VERIFIED; zero failed tries. This task adds coverage only.
- Scope: `scripts/test_psarc.py` now pins a truncated full-size raw block. The fixture stores one 16-byte member (`bytes(range(16))`), removes the final archive byte, then asserts the exact existing error `block 1 decoded to 15 bytes, expected 16`.

## Verification

- Worker command: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_psarc.py`; exit 0, 30 tests passed. Private log `local/validation/data02-psarc-stored-block-truncation/attempt-1.log`, SHA256 `c700d752368947d95b31acbc174d59c9c1149ddeefb848253a4a53ab205dce05`.
- Root replay of the same command: exit 0, 30 tests passed. Private log `local/validation/data02-psarc-stored-block-truncation/root-replay.log`, SHA256 `15237638d7cbdbbc88e961e99c76e8a6a579f110ad73dacc2034edffe75cfd79`.
- `scripts/test_psarc.py` final SHA256: `fc3de21818d824af5bffa67cccdaf28714a331e8389ebc6d9cce63fb08582cfb`. `git diff --exit-code -- scripts/psarc.py` exited 0; parser SHA256 remains `39f2c748db013637aba8144205dc60b7e5b1eb49ee0f641960783a1e2c140485`.
- Observation: source already rejects a short raw-block chunk after reading it. The new test distinguishes that path from the existing compressed-block `truncated block` test.
- Remaining limits: synthetic fixture only; no user archive or payload was read. No parser, codec, audio playback, loader, or game behavior claim.
- Index/check results: `python3 scripts/project_index.py` exited 0 and reported 14,206 folders indexed. The first `trail build --check` exited 1 because the newly added test symbol made `scripts/test_psarc.py` stale in the local graph; it reported the existing `scripts/` scope was present. `trail build` then exited 0, parsed the changed test file, replayed the other 1,234 cached files, and produced 3,520 wiring nodes; the cache remains git-ignored. The subsequent `trail build --check` exited 0 with the graph in sync, the deep layer not built, and 3,520 meaning-tier nodes pending. Generated `docs/FOLDERS.md` increments the checkpoint-directory file count 233→234.
- Publication gates: `python3 -I scripts/audit_publish_paths.py` exited 0 for six staged allowlisted paths; `python3 -I scripts/audit_publish_paths.py --all` exited 0 for 1,500 allowed tracked paths; `git diff --cached --check` exited 0. Unstaged escalated `native/api/CMakeLists.txt` and `native/api/host/` are excluded.
- Next exact action: commit and push only the allowlisted test/docs files.
