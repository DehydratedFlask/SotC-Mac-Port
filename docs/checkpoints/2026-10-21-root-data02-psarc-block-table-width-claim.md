# Claim: DATA-02-PSARC-BLOCK-TABLE-WIDTH-01

- Local time: 2026-10-21 11:16 CDT (America/Chicago).
- Agent / integrator: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`); worker after claim publication: `launcher_review`.
- Task status / attempts: ACTIVE; 0 failed implementation tries.
- Bounded scope: make the synthetic PSARC builder in `scripts/test_psarc.py` accept a block size and encode block-table values at the corresponding width. Add one block-size-257 fixture with a 256-byte raw member, asserting width two, encoded value 256, and successful member read. Edit only `scripts/test_psarc.py`. No parser change, real archive, game payload, or runtime claim.
- Prerequisite: `DATA-02-PSARC-PARTIAL-READ-01` is FIXTURE_VERIFIED.

## Claim-time evidence

- Graft command `trail grep "block_size" --in scripts/` locates `Psarc._load()` at `scripts/psarc.py:211-247`: the parser derives width from `block_size` and converts fixed-width entries from big-endian bytes. `Psarc.read()` consumes those values at `scripts/psarc.py:249-280`.
- Live fixture inspection (`sed -n '1,150p' scripts/test_psarc.py`) shows `BS = 16`, appends each stored length to an integer list, serializes `bytes(blocks)`, and writes `BS` into the synthetic header. Existing fixture therefore exercises width one only.
- A 256-byte incompressible payload with block size 257 remains a raw partial block with table value 256 (`0x0100`), providing a nonzero high byte for a two-byte entry. The test will validate the fixture's claim locally by asserting parser width and decoded payload.
- Registry search found no existing task with this ID or block-table-width scope. Checkout at claim: branch `main`, HEAD `7028abeb774724cd4ab5a3ebabc9068a762f644b`, matching `origin/main`. Only pre-existing escalated `native/api/CMakeLists.txt` and untracked `native/api/host/` are dirty; exclude them.

## Acceptance and boundaries

- Acceptance command: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_psarc.py`.
- Baseline status: NOT RUN for this fixture. Worker may add only test-builder/test assertions. Record any expected fixture failures accurately; no parser change is authorized by this claim.
- Private evidence path: `local/validation/data02-psarc-block-table-width/`.
- Files changed to claim: `HANDOFF.md`, `docs/TASKS.md`, generated `docs/FOLDERS.md`, and this checkpoint. No test or parser source changed at claim time.
- Publication gates and claim commit are pending. Keep user game files and host escalation paths untouched.

## Claim publication and assignment checkpoint

- Local date/time: 2026-10-21 11:16 CDT (America/Chicago). Integrator: `/root`; worker after publication: `launcher_review`.
- Exact publication command/result: `git push origin main` exited 0 and advanced
  `7028abe..f427b2b`. Commit `f427b2bb412379a54feb7e5f3af27420420a31da` is on `origin/main`; root
  verified HEAD equals `origin/main`.
- Publication checks: staged audit accepted five allowlisted files; full tracked-path audit accepted
  1,516 files; `git diff --cached --check` exited 0. `python3 scripts/project_index.py` indexed
  14,222 folders and `--check` reports current. `trail build --check` reports the graph in sync at
  3,528 nodes (deep meaning layer remains unbuilt). Logs are in `local/checkpoints/root-round13-claims/`.
- Publication-record follow-up: `git push origin main` exited 0 and advanced `f427b2b..95642f0`.
  Commit `95642f017d58c2fccaf1cd2398a32077230a5d8d` records both claim-publication results; HEAD
  equals `origin/main`.
- Observation: worker scope is only `scripts/test_psarc.py`; no test has run for this task.
- Remaining risks: test fixture only; no real PSARC or game asset validation.
- Next exact action: assign the test file, then review the worker's diff and log and independently replay the acceptance command.
