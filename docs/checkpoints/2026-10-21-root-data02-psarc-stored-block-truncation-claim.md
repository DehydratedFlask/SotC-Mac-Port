# Claim: DATA-02-PSARC-STORED-BLOCK-TRUNCATION-01

- Local time: 2026-10-21 10:20 CDT (America/Chicago).
- Agent / integrator: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`); worker after claim publication: `slot23_raw_xrefs`.
- Task status / attempts: ACTIVE; 0 failed implementation tries.
- Bounded scope: add a synthetic regression to `scripts/test_psarc.py` for a truncated full-size raw block. Use the existing archive builder with a 16-byte incompressible member, remove its final byte, then assert `Psarc.read(1)` raises the precise decoded-length `PsarcError`. No source change or live archive read.
- Prerequisite: `DATA-02-PSARC-PARTIAL-READ-01` is FIXTURE_VERIFIED.

## Claim-time evidence

- Graft command `trail ask "How does Psarc.read handle raw stored blocks and truncated block bytes, and which synthetic tests cover that path?" --source --in scripts/` refreshed two indexed files. It located `Psarc.read` at `scripts/psarc.py:249-280`: the `csize == 0` path reads a full block, slices to the member's expected block length, then rejects `len(chunk) != full` with `PsarcError`. Existing truncation coverage at `scripts/test_psarc.py:216-218` covers a compressed block and expects `truncated block`.
- The existing builder at `scripts/test_psarc.py:20-50` uses 16-byte blocks and records a full raw block as block-table size 0. `bytes(range(16))` is the bounded fixture payload; deleting its final archive byte makes the raw read return 15 of 16 bytes. Expected message from the live source is `block 1 decoded to 15 bytes, expected 16`.
- Prerequisite task `DATA-02-PSARC-PARTIAL-READ-01` is FIXTURE_VERIFIED. This task adds missing coverage only; it does not change the parser, negative-limit semantics, or archive behavior.
- Checkout at claim: `main`, HEAD `c0c7177a5b6da761c8e12dac7aaf56c92aa3ce55`, matching `origin/main`. `git status --short` shows only pre-existing escalated `native/api/CMakeLists.txt` and untracked `native/api/host/`; these are excluded.

## Acceptance and boundaries

- Acceptance command: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_psarc.py`.
- Exact commands/results so far: no test has been run for this task; baseline is NOT RUN. No source/test code changes or log files exist yet. `python3 scripts/project_index.py` exited 0 and reported 14,205 folders indexed; `trail build --check` exited 0 with the wiring graph in sync, deep layer not built, and 3,519 meaning-tier nodes pending.
- Observation: raw-block reads already compare the returned chunk length with the expected full block length. Inference: a synthetic test can pin this rejection path without a production parser change.
- Remaining risks: fixture only. No real game archive or payload is read; no member-format or runtime claim is made. Keep private logs under `local/validation/data02-psarc-stored-block-truncation/`.
- Files changed to claim: `HANDOFF.md`, `docs/TASKS.md`, generated `docs/FOLDERS.md` (checkpoint count 232→233), the prior overlap-map completion checkpoint's push record, and this claim checkpoint. No test/source code changed.
- Publication gates: `python3 -I scripts/audit_publish_paths.py` exited 0 for five staged allowlisted paths; `python3 -I scripts/audit_publish_paths.py --all` exited 0 for 1,499 allowed tracked paths; `git diff --cached --check` exited 0. Only the claim/handoff/index paths are staged; escalated host-protocol files remain unstaged.
- Next exact action: commit and push the claim, then assign only `scripts/test_psarc.py` to `slot23_raw_xrefs`.
