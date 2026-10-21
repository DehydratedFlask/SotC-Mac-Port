# Completion: DATA-02-PSARC-BLOCK-TABLE-WIDTH-01

- Local time: 2026-10-21 11:20 CDT (America/Chicago).
- Agent / integrator: worker `launcher_review`; integrator `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`).
- Task status / attempt count: FIXTURE_VERIFIED; 0 failed implementation tries.
- Files changed: worker changed only `scripts/test_psarc.py`; root updated `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, generated `docs/FOLDERS.md`, and this checkpoint. Production `scripts/psarc.py` is unchanged.

## Result

- The synthetic builder now accepts a configurable block size, encodes each block-table entry in the width implied by that size, and writes the selected size into the header. The added test uses block size 257 and `bytes(range(256))`, forcing a raw partial block with stored size 256 (`0x0100`).
- Observation: the parser reports width 2, reads the table entry as 256, and returns the exact 256-byte member. This exercises a nonzero high byte in the synthetic block table.
- Acceptance command: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_psarc.py`.
- Worker result: exit 0, 31 tests passed. Log `local/validation/data02-psarc-block-table-width/attempt-1.log`, SHA256 `cb10a234dc7e993a37461ff7b5d08f54b12fd3ba783fb5941fa5f9ae19388218`.
- Root replay: exit 0, 31 tests passed. Log `local/validation/data02-psarc-block-table-width/root-replay.log`, SHA256 `ce81609136aa3ac83eadf23ae86fc294c66f6946b7e919e4cbe8df0a4706e7cb`.
- Test SHA256 `9238282d901148df891ee0756ee48ced145c6b4e3b310f04b271b66db412cf46`; parser SHA256 `39f2c748db013637aba8144205dc60b7e5b1eb49ee0f641960783a1e2c140485` (unchanged). `git diff --check -- scripts/test_psarc.py` passed.
- Source graph after the two concurrent fixture edits: initial `trail build --check` exited 1 and named both changed test files plus the two new test symbols. `trail build` exited 0 with 3,530 nodes / 4,746 edges; subsequent `trail build --check` exited 0. Deep meaning layer remains unbuilt. Logs: `local/checkpoints/root-round13-claims/trail-check-stale.log`, `trail-build.log`, and `trail-check.log`.

## Limits and checkpoint

- This fixture covers synthetic block-table serialization and parser roundtrip only. No real PSARC, game asset, codec behavior beyond the synthetic raw member, or gameplay is validated.
- No implementation try failed; no production parser changed.
- Remaining risk: other widths and values may have distinct issues; the claim covers one two-byte value only.
- Project-index result, staged/full path audit, commit, and push will be recorded after this checkpoint and evidence are staged.
- Next exact action: complete publication gates and publish the completion record.
