# Checkpoint: AUD-01-XFF-PREFIX-OFFSET-ONLY-FIXTURE-01 — verified

- Time / timezone: 2026-10-22 00:52 CDT (America/Chicago).
- Agent / integrator: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`);
  implementation worker `/root/next_bounded_scout`.
- Task / parent / status: `AUD-01-XFF-PREFIX-OFFSET-ONLY-FIXTURE-01`, child of `AUD-01-XFF-01`;
  `FIXTURE_VERIFIED`; 1 successful implementation attempt, 0 failed tries.
- Claim record: [claim checkpoint](2026-10-22-root-aud01-xff-prefix-offset-only-claim.md).
- Baseline and final source hashes: baseline `scripts/test_audio_bank_header.py` SHA-256
  `4162d392a35d6154a02de207348483bfa233f5339645785b5f323b569c4ae7f5`; final test SHA-256
  `54e3bce09addda52330107dde497988a48da6661c644f7f61058d49ddbae615d`. Production
  `scripts/audio_bank_header.py` remained unchanged at SHA-256
  `bdad7d192b0ecdb929daf5147a139e7545fd5c546f01ed4ffb4005f2cea9f9c0`; applicable
  `docs/ASSETS.md` baseline specification SHA-256 is recorded in E260.
- Files changed for this task: one test method in `scripts/test_audio_bank_header.py`; the worker
  owned no other file. Integrated task records are `HANDOFF.md`, `docs/TASKS.md`,
  `docs/EVIDENCE.md`, generated `docs/FOLDERS.md`, and this checkpoint. Existing user-owned
  `native/api/CMakeLists.txt` and `native/api/host/` were not part of the task and remain unstaged.
- Hypothesis / expected result: an XFF magic byte sequence inside the opaque span (bytes 4..7)
  cannot replace the required magic at byte zero. A synthetic member with `qath` at byte zero and
  `XFF_MAGIC` only at bytes 4..7 should raise `AudioBankFormatError` with the exact unsupported
  byte-zero prefix message.
- Exact command, CWD and environment: from `/Volumes/Extra/SoC Mac Native`,
  `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_audio_bank_header.py`.
- Exit / observed result: baseline passed 21/21. Worker and root final replays each passed 22/22.
  Baseline log `local/validation/20261022-root-fixture-baselines/aud01-xff-prefix-offset-baseline.log`
  SHA-256 `1d8170c6d126f7d82088a647b4863a8c5c09c279d6d63f82f2fda0ad3a747c0d`; worker log
  `.../aud01-xff-prefix-offset-only-worker.log` SHA-256
  `24a63c6503c5e070adbb89751be5e57ceab526e2a28cf85012eb48835c26bab5`; root log
  `.../aud01-xff-prefix-offset-only-root-final.log` SHA-256
  `86eaa534cabf5cc8150a0ba91a7d2161ec2a13b9e72dc739d6d21705209efd4b`.
- Trail evidence: `trail build --check` passed at 3,612 nodes / 1,240 files; deep meaning layer is
  not built. `trail grep "test_rejects_xff_magic_only_in_opaque_span"` returned the method in
  `scripts/test_audio_bank_header.py:L67-L79`. Check and query logs are
  `local/validation/20261022-root-fixture-baselines/aud-fixtures-trail-check.log` SHA-256
  `53451a65e275859a881899073bf9409ef57118bd99c37fa7eaf91d7246b7d19b` and
  `.../aud01-xff-prefix-offset-only-trail-query.log` SHA-256
  `9c63bf327cb707a5c04a9ef28714379becb8cb9fc5456177fbced34dae80189b`.
- Observation / inference and limits: the synthetic parser call raises the expected error for this
  crafted member. It does not establish any actual asset decoding, codec interpretation, audio
  playback, or game behavior. Production parser logic is unchanged.
- Failed tries / risks: 0 failed tries. The format bytes 4..19 remain opaque per `docs/ASSETS.md`;
  this test establishes only that a decoy magic at bytes 4..7 is not treated as the byte-zero
  prefix.
- Project index: `python3 scripts/project_index.py` wrote 24,078 folders and
  `python3 scripts/project_index.py --check` passed. Logs are
  `local/validation/20261022-root-fixture-baselines/aud-fixtures-project-index-generation.log`
  SHA-256 `a79b0d25fced0d9c18dda3386413644761318dd915da578a4bbe36f239469b0c` and
  `.../aud-fixtures-project-index-check.log` SHA-256
  `2e24f86275b84b7efc28ec198e984a6698bc3584a72c960f1b280bdaa45c1b67`. An initial check failed
  because its newly created log changed the indexed `local/validation` file count after generation;
  the retained first-check log is SHA-256
  `31dad00ffd5208458ee89a329fea70b0fa13494e525ddd8a7e837c7614a2266b`. After precreating all
  expected audit logs and regenerating, the check passed.
- Next exact action: stage only allowlisted task files and docs, run both publish-path audit
  commands required by `AGENTS.md`, and push `origin main`.
- `HANDOFF.md`, `docs/TASKS.md`, and `docs/EVIDENCE.md` have the completion status recorded;
  `INSTALL_LOG.md` is unchanged. Publication details are to be recorded in the following docs-sync
  checkpoint after the completion commit is pushed.
