# Checkpoint: DATA-02-PSARC-BLOCK-SIZE-RANGE-01 — complete

- Local date/time: 2026-10-21 11:56 CDT.
- Agent ID: `/root` (integrator); implementation by `/root/slot23_raw_xrefs`.
- Task ID: `DATA-02-PSARC-BLOCK-SIZE-RANGE-01`.
- Status: `FIXTURE_VERIFIED`; failed tries: 0.
- Changed file: `scripts/test_psarc.py` only. The fixture starts with a valid `build([('a', b'1')])` archive, then writes block-size values `0` and `(1 << 24) + 1` at header offset 24 using big-endian encoding. It asserts each case raises exactly `PsarcError('unsupported entry/block size')`.
- Worker command: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_psarc.py`; exit 0, 32/32. Log `local/validation/data02-psarc-block-size-range/attempt-1.log`, SHA256 `8c087540c3e5ac7d58c2cafc88fed6bfcd9054f7901bf4b2439219876039ecc4`.
- Root replay command: `set -o pipefail; PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_psarc.py 2>&1 | tee local/validation/data02-psarc-block-size-range/root-replay.log`; exit 0, 32/32. Log SHA256 `77a5d3aa4c29e0cb892f45bf67a3d832edc5530a788069566674f05a56f9b83c`.
- Test SHA256: `0a922203a17cfe7454cbf5f1f8e5787413511320d6bd4a54415c205360263dfc`. Production parser `scripts/psarc.py` is unchanged at SHA256 `39f2c748db013637aba8144205dc60b7e5b1eb49ee0f641960783a1e2c140485`. `git diff --check` passed; E147 records both runs and hashes.
- Observation: both out-of-range values trigger the parser's current exact guard error before block-table parsing. Inference limit: the test uses only a synthetic archive and does not validate any user archive.
- Remaining risks: no live archive content, extraction, or game behavior is established; actual game acceptance gates remain open.
- Graph evidence: `trail build` exited 0 and reported 3,536 nodes / 4,756 edges; `trail build --check` exited 0 after the rebuild. The project-index check later reported stale because the root replay log had just been created.
- Next exact action: run `python3 scripts/project_index.py` and `python3 scripts/project_index.py --check`, then stage only this test and its task/evidence/checkpoint records, run both publication audits, commit, and push.
