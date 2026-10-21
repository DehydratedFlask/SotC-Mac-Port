# Checkpoint: DATA-02-MANIFEST-JSON-SYNTAX-01 — malformed JSONL diagnostics

- Time / timezone: 2026-10-21 09:48 CDT (America/Chicago).
- Agent and integrator: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`); implementation by `/root/slot23_raw_xrefs`.
- Task / gate / status / attempts: `DATA-02-MANIFEST-JSON-SYNTAX-01` / synthetic parser fixture / FIXTURE_VERIFIED / 0 failed implementation tries.
- Prerequisites: DATA-02 and `DATA-02-MANIFEST-RECORD-SHAPE-01`; JSON syntax errors had explicitly been left unchanged by that prior task.
- Files changed: `scripts/psarc.py` (only `load_members_jsonl`), `scripts/test_psarc.py`, `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, generated `docs/FOLDERS.md`, and this checkpoint. Private logs/checkpoint are under `local/validation/data02-manifest-json-syntax/`. `INSTALL_LOG.md` unchanged.
- Hypothesis / expected result: malformed nonblank JSONL records raise `PsarcError` naming the manifest path and 1-based line; blank lines continue raising the original `JSONDecodeError`.
- Failing-before command and result: from `/Volumes/Extra/SoC Mac Native`, the implementer ran `python3 -I scripts/test_psarc.py` with two new malformed-line cases and the old parser. Exit 1; 28 tests, two expected `JSONDecodeError` errors. Private log SHA256 `88acd0d202a4f01cb063a4aeeafea42bd5bd1ad5dc54ca3dbd1b1d5cb0be1dfc`. The old parser source was the committed file SHA256 `036a2fe553139f777542521efaf42c9ee104babb8313fc64559eb1f1fb7e879c`.
- Fix and exact command: `load_members_jsonl` now catches `json.JSONDecodeError` only for nonblank lines, raises `PsarcError(f'{path}:{lineno}: malformed JSONL record')` from the decoder error, and re-raises the original error for blank lines. From the same CWD, the implementer ran `python3 -I scripts/test_psarc.py`; exit 0, 29 tests passed. Root independently ran the same command; exit 0, 29 tests passed.
- Logs and source hashes: worker after-fix log SHA256 `1d67f3b3dd32cb18fa9af36ab5dfc28b05e89a5390c469bf8d0d9c929f28b7ef`; root replay log `local/validation/data02-manifest-json-syntax/root-replay.log`, SHA256 `a845184b1ee38769e3a3ce6ba63a1195adcce071389ab343352c078bca01ed21`. Final `scripts/psarc.py` SHA256 `39f2c748db013637aba8144205dc60b7e5b1eb49ee0f641960783a1e2c140485`; `scripts/test_psarc.py` SHA256 `9736d1dfe13bada55a57b73b4c6682c3b79f0b2743d4661a1af146595d5dec98`.
- Observation versus inference / limits: the tests cover malformed JSON on line 1 and line 2 after a valid record, plus a blank-line regression. This is a synthetic parser diagnostic only; no live manifest, archive, payload, overlay or runtime was used.
- Remaining risks: broader loader/runtime behavior remains untested; DATA-02 schema and selected-asset resolution remain open.
- Next exact action: root independently replay `DATA-02-TOC-VALUE-DISTINCT-PATH-REUSE-01`; keep aggregate reports free of paths and TOC values.
- Index / graph: `python3 scripts/project_index.py` exited 0 and indexed 14,202 folders. Graft
  refreshed the graph after the two source files changed; `trail build --check` exited 0 with 3,508
  nodes synchronized. The meaning layer remains unbuilt (0% complete); `graft/` is ignored locally.
- Publication: only `scripts/psarc.py`, `scripts/test_psarc.py`, HANDOFF, task/evidence/folder
  indexes, and this checkpoint are allowlisted for this task. Native host work remains unstaged.
