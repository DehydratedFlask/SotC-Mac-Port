# Checkpoint: DATA-02-PSARC-PARTIAL-READ-01 — `Psarc.read(index, limit)` limit path pinned

- Time / timezone: 2026-10-21 00:19 CDT (America/Chicago)
- Agent and integrator: worker `data02-re02-fixtures` (`deepseek-v4.1-flash-1017`); integrator: orchestrator session
- Task / gate / status: `DATA-02-PSARC-PARTIAL-READ-01`; DATA-02 synthetic parser fixture; `FIXTURE_VERIFIED`
- Prerequisite evidence / input and source hashes: repo `HEAD` `06ce0f4`; `Psarc.read` unmodified (`scripts/psarc.py` final SHA-256 `036a2fe553139f777542521efaf42c9ee104babb8313fc64559eb1f1fb7e879c`, differing from `HEAD` only by the separate B8 fix).
- Owned / changed files: `scripts/test_psarc.py` only (added `Tests.test_read_limit_parameter`). No parser change.
- Hypothesis and expected result: `want = e['size'] if limit is None else min(limit, e['size'])` must yield: `b''` for `limit == 0`, exactly the first `limit` bytes otherwise, the full member for `limit == size`, and the full member for `limit > size` (clamped). Both a stored block (`csize == 0`) and a zlib-compressed block must behave identically.
- Already-covered note: `test_roundtrip_compressible_and_incompressible` already asserted one compressed partial read (`a.read(1, 5) == b'AAAAA'`). That case is therefore NOT counted as new coverage; the new test adds the stored-block path, `limit == 0`, the clamp, the `limit == size` equality with the full read, and a partial read that crosses a block boundary.
- Exact command, CWD and nonsecret environment: CWD `/Volumes/Extra/SoC Mac Native/scripts`, `python3 -I -m unittest discover -s . -p 'test_psarc.py' -v` (Python 3.15.0). The literal `-m unittest test_psarc` form fails to import under `-I`, see `/private/var/folders/vx/5z8skhb97w32g817rv7hbh940000gn/T/opencode/b8/m-form-error.log`.
- Exit / observed result: exit 0, `Ran 26 tests ... OK`. Fixture branch proof inside the test: `stored.blocks[stored.entries[1]['block']] == 0` (stored) and `compressed.blocks[compressed.entries[1]['block']] != 0` (zlib), so both decode paths are genuinely exercised.
- Log, capture and REA evidence paths/IDs: `test_psarc.after.log` SHA-256 `991ad594699e0e931a944c564d888e151766c827e75434dddfb19e10f098f73a`; mutation proof `mutation_check.log` SHA-256 `24942501845ade961d4f2118ed0db7610df31ff6872f22bc445d2bc24df3425b` (temp dir outside repo).
- Observation versus inference / coverage limitations: replacing the clamp with `want = e['size']` makes this test fail (`B4: CAUGHT`) and the harness restored `psarc.py` byte-identically. Synthetic only; no real archive payload, schema or runtime claim. `read()` still reads whole blocks internally, so this is not a memory-bounded streaming claim.
- Cumulative failed tries and distinct hypotheses: 0 failed tries.
- Remaining risks / blockers: none for the fixture. Note `limit < 0` is not special-cased by the parser (it behaves like `limit == 0`); this was left untested rather than pinned, since the task did not specify it.
- Next exact action: integrator records the row and refreshes `python3 scripts/project_index.py`.
- HANDOFF.md / TASKS / EVIDENCE / INSTALL_LOG / indexes updated: NOT updated by this worker; no install facts changed.
