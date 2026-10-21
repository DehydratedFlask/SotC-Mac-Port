# Checkpoint: DATA-02-PSARC-BE-BOUNDS-01 — `psarc.be()` bound semantics pinned

- Time / timezone: 2026-10-21 00:19 CDT (America/Chicago)
- Agent and integrator: worker `data02-re02-fixtures` (`deepseek-v4.1-flash-1017`); integrator: orchestrator session
- Task / gate / status: `DATA-02-PSARC-BE-BOUNDS-01`; DATA-02 synthetic unit fixture; `FIXTURE_VERIFIED`
- Prerequisite evidence / input and source hashes: repo `HEAD` `06ce0f4`; `psarc.be` unmodified (`scripts/psarc.py` final SHA-256 `036a2fe553139f777542521efaf42c9ee104babb8313fc64559eb1f1fb7e879c`, differing from `HEAD` only by the separate B8 fix).
- Owned / changed files: `scripts/test_psarc.py` only (added `Tests.test_be_bounds`). No parser change.
- Hypothesis and expected result: `be` is exact-bound, i.e. it succeeds when `pos + n == len(data)` and when `n == 0` at `pos == len(data)`, and raises `PsarcError(f'out of bounds: {pos:#x}+{n}')` for `pos + n > len(data)` and for negative `pos`. `be` had no direct unit test before this batch.
- Exact command, CWD and nonsecret environment: CWD `/Volumes/Extra/SoC Mac Native/scripts`, `python3 -I -m unittest discover -s . -p 'test_psarc.py' -v` (Python 3.15.0). The literal `-m unittest test_psarc` form fails to import under `-I`, see `/private/var/folders/vx/5z8skhb97w32g817rv7hbh940000gn/T/opencode/b8/m-form-error.log`.
- Exit / observed result: exit 0, `Ran 26 tests ... OK`. Pinned exact messages: `out of bounds: 0x1+2`, `out of bounds: -0x1+1` (negative position renders as `-0x1`), `out of bounds: 0x3+0` (zero length past the end is still rejected). Allowed cases asserted equal to `0x0102`, `0x02`, `0` (`n == 0` at `len`), and `0` for empty input at `0`.
- Log, capture and REA evidence paths/IDs: `test_psarc.after.log` SHA-256 `991ad594699e0e931a944c564d888e151766c827e75434dddfb19e10f098f73a`; mutation proof `mutation_check.log` SHA-256 `24942501845ade961d4f2118ed0db7610df31ff6872f22bc445d2bc24df3425b` (temp dir outside repo).
- Observation versus inference / coverage limitations: neutralising the bound check makes this test fail (`B3: CAUGHT`) and the harness restored `psarc.py` byte-identically. The negative-position rendering (`-0x1`) is now a pinned string; if a future change normalises it, this test will flag it. Synthetic only; no archive or runtime claim.
- Cumulative failed tries and distinct hypotheses: 0 failed tries.
- Remaining risks / blockers: none; note the parser relies on this helper for every TOC field, so the exact message text is load-bearing for other tests.
- Next exact action: integrator records the row and refreshes `python3 scripts/project_index.py`.
- HANDOFF.md / TASKS / EVIDENCE / INSTALL_LOG / indexes updated: NOT updated by this worker; no install facts changed.
