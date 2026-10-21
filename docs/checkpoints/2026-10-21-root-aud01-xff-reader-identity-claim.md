# Claim: AUD-01-XFF-READER-IDENTITY-FIXTURE-01

- Local time: 2026-10-21 10:00 CDT (America/Chicago).
- Agent / integrator: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`).
- Task status / attempts: ACTIVE; 0 failed tries.
- Bounded scope: add synthetic tests for `audio_bank_header.inspect_members` to
  `scripts/test_audio_bank_header.py` only. Cover a valid full identity and a manifest identity
  mismatch; verify that the mocked reader closes on success and on the post-open error path.
  Do not modify parser implementation, read live archives, or make codec/playback claims.
- Prerequisite: `AUD-01-XFF-01` is FIXTURE_VERIFIED.

## Claim-time evidence

- Graft `trail_find_code` located `inspect_members` at
  `scripts/audio_bank_header.py:101-198`. It hashes the temporary archive path, checks the
  supplied summary byte count and SHA256, matches one exact member path, reads the entry, verifies
  decoded size, and compares manifest locator fields and member SHA256 before parsing XFF bytes.
  The reader is closed in a `finally` block.
- Graft `trail_find_all` found two references in the indexed source scope: the function definition
  and the CLI call at `scripts/audio_bank_header.py:212`. No test reference is currently indexed.
- Checkout: `main`, HEAD and `origin/main` both `1abb65d`. `git status --short` shows only the
  pre-existing modified `native/api/CMakeLists.txt` and untracked `native/api/host/`, both
  excluded as escalated `APP-01-CORE-HOST-01` work.

## Acceptance and boundaries

- Mock `Psarc` and use temporary archive, summary, and manifest files. Avoid invoking the real
  PSARC decoder or reading user game inputs.
- Accepted command: `python3 -I scripts/test_audio_bank_header.py`.
- The successful case must assert the full identity report fields. The error case must assert a
  manifest mismatch and confirm the reader was closed. If testing summary mismatch, assert the
  reader is never opened because validation precedes construction.
- Observation: current code has a `finally: archive.close()` around the member read/identity loop.
  Inference: synthetic identity fixtures can pin this contract without archive data.
- Remaining risks: exact synthetic XFF bytes and helper setup need to match the live parser; this
  task does not establish real archive identity, codec correctness, or playback.
- Files changed at claim: `HANDOFF.md`, `docs/TASKS.md`, and this checkpoint. No implementation or
  test file has been edited yet.
- Next exact action: assign the single test file to `review_host_proto`; root will review the
  changes, replay the acceptance command, update the evidence/indexes, then commit and push.
