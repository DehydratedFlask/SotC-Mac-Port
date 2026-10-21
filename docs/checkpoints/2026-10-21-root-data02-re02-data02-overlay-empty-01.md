# Checkpoint: DATA-02-OVERLAY-EMPTY-01 — empty overlay, duplicate JSONL path, single-layer stats

- Time / timezone: 2026-10-21 00:19 CDT (America/Chicago)
- Agent and integrator: worker `data02-re02-fixtures` (`deepseek-v4.1-flash-1017`); integrator: orchestrator session
- Task / gate / status: `DATA-02-OVERLAY-EMPTY-01` (also covers the duplicate-manifest-record case); DATA-02 synthetic overlay fixtures; `FIXTURE_VERIFIED`
- Prerequisite evidence / input and source hashes: repo `HEAD` `06ce0f4`; `build_overlay`/`load_members_jsonl`/`resolve_path` unmodified by this task (`scripts/psarc.py` final SHA-256 `036a2fe553139f777542521efaf42c9ee104babb8313fc64559eb1f1fb7e879c`, differing from `HEAD` only by the separate B8 record-shape fix).
- Owned / changed files: `scripts/test_psarc.py` only (added `test_overlay_requires_a_layer`, `test_load_members_jsonl_rejects_duplicate_exact_path`, `test_overlay_single_layer_stats_and_resolve`, plus the `write_jsonl` helper). No parser change.
- Hypothesis and expected result: (a) `build_overlay([])` raises `PsarcError('overlay requires at least one layer')`; (b) a JSONL file with the same exact path on two lines raises `PsarcError("duplicate exact path in <path>: 'same'")`; (c) a valid one-layer overlay reports `members == 1`, `added == 1`, `overrides == 0`, `sources == {'base': 1}`, returns the record with `source_layer='base'` for the exact path, and `None` for `'ONLY/one'` and `'only/on'`.
- Already-covered note: `test_overlay_patch_wins_exact_path` already covers a two-layer override and an exact-only `resolve_path` miss; the single-layer `added`/`overrides` accounting and the empty-layer rejection were not covered. `test_overlay_from_member_jsonls_roundtrip` never exercises a duplicate path.
- Exact command, CWD and nonsecret environment: CWD `/Volumes/Extra/SoC Mac Native/scripts`, `python3 -I -m unittest discover -s . -p 'test_psarc.py' -v` (Python 3.15.0). The literal `-m unittest test_psarc` form fails to import under `-I`, see `/private/var/folders/vx/5z8skhb97w32g817rv7hbh940000gn/T/opencode/b8/m-form-error.log`.
- Exit / observed result: exit 0, `Ran 26 tests ... OK`.
- Log, capture and REA evidence paths/IDs: `test_psarc.after.log` SHA-256 `991ad594699e0e931a944c564d888e151766c827e75434dddfb19e10f098f73a`; mutation proof `mutation_check.log` SHA-256 `24942501845ade961d4f2118ed0db7610df31ff6872f22bc445d2bc24df3425b` (temp dir outside repo).
- Observation versus inference / coverage limitations: neutralising the `if not layers` guard makes (a) fail (`B5a: CAUGHT`); neutralising the duplicate check makes (b) fail (`B5b: CAUGHT`); the harness restored `psarc.py` byte-identically. Synthetic only; the overlay/mount order remains `PROVISIONAL_OBS`, and no loader precedence, schema or runtime claim is made.
- Cumulative failed tries and distinct hypotheses: 0 failed tries.
- Remaining risks / blockers: the duplicate-path message interpolates the file path, so the assertion is tied to the temp path — intentional, since the message must name the file.
- Next exact action: integrator records the row and refreshes `python3 scripts/project_index.py`.
- HANDOFF.md / TASKS / EVIDENCE / INSTALL_LOG / indexes updated: NOT updated by this worker; no install facts changed.
