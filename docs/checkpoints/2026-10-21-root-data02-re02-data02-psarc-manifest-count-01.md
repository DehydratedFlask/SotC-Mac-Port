# Checkpoint: DATA-02-PSARC-MANIFEST-COUNT-01 — manifest path-count mismatch rejected

- Time / timezone: 2026-10-21 00:19 CDT (America/Chicago)
- Agent and integrator: worker `data02-re02-fixtures` (`deepseek-v4.1-flash-1017`); integrator: orchestrator session
- Task / gate / status: `DATA-02-PSARC-MANIFEST-COUNT-01`; DATA-02 synthetic parser fixture; `FIXTURE_VERIFIED`
- Prerequisite evidence / input and source hashes: repo `HEAD` `06ce0f4`; `scripts/psarc.py` parser unchanged by this task (final SHA-256 `036a2fe553139f777542521efaf42c9ee104babb8313fc64559eb1f1fb7e879c`, which differs from `HEAD` only by the separate B8 `load_members_jsonl` fix).
- Owned / changed files: `scripts/test_psarc.py` only. `build()` gained an optional `manifest_paths=` override that changes the manifest text only, leaving the payload/block/offset layout valid; existing calls are unaffected (default `None` reproduces the old behaviour).
- Hypothesis and expected result: `Psarc.paths()` raises `PsarcError(f'manifest has {len(names)} paths for {self.count - 1} members')` when the manifest member's decoded path count differs from `count - 1`. Two fixtures: one-too-many (`['a','b']` paths for 1 member) and one-too-few (`['a']` path for 2 members). The branch was previously untested.
- Exact command, CWD and nonsecret environment: CWD `/Volumes/Extra/SoC Mac Native/scripts`, `python3 -I -m unittest discover -s . -p 'test_psarc.py' -v` (Python 3.15.0). The literal `-m unittest test_psarc` form fails to import under `-I` (CWD removed from `sys.path`), see `/private/var/folders/vx/5z8skhb97w32g817rv7hbh940000gn/T/opencode/b8/m-form-error.log`.
- Exit / observed result: exit 0, `Ran 26 tests ... OK`. Asserted exact messages: `manifest has 2 paths for 1 members` and `manifest has 1 paths for 2 members`. Both archives open successfully (structure valid) and fail only in `paths()`, which is the intended branch.
- Log, capture and REA evidence paths/IDs: `test_psarc.after.log` SHA-256 `991ad594699e0e931a944c564d888e151766c827e75434dddfb19e10f098f73a`; mutation proof `mutation_check.log` SHA-256 `24942501845ade961d4f2118ed0db7610df31ff6872f22bc445d2bc24df3425b` (temp dir outside repo).
- Observation versus inference / coverage limitations: neutralising the `len(names) != self.count - 1` guard (`if False: pass`) makes this test fail (`B2: CAUGHT`) and the harness restored `psarc.py` byte-identically. Synthetic only; no real-manifest count, schema, mount-order or runtime claim.
- Cumulative failed tries and distinct hypotheses: 0 failed tries.
- Remaining risks / blockers: the message says "members" for `count - 1`, matching existing parser text; no other manifest validation was added.
- Next exact action: integrator records the row and refreshes `python3 scripts/project_index.py`.
- HANDOFF.md / TASKS / EVIDENCE / INSTALL_LOG / indexes updated: NOT updated by this worker; no install facts changed.
