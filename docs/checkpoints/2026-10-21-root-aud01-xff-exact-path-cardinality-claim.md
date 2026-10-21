# Claim: AUD-01-XFF-EXACT-PATH-CARDINALITY-01

- Local time: 2026-10-21 11:00 CDT (America/Chicago). Integrator: `/root`; worker after claim publication: `slot23_raw_xrefs`.
- Task status / attempts: ACTIVE; zero failed implementation tries. No test has run for this task.
- Bounded scope: extend `scripts/test_audio_bank_header.py` with synthetic zero-match and duplicate-exact-path cases for `audio_bank_header.inspect_members`. Assert the full expected-path error, zero reader calls, and reader closure for both. Do not change production code. Worker owns only the test file; root owns project/task records.
- Prerequisites: `AUD-01-XFF-01` and `AUD-01-XFF-READER-IDENTITY-FIXTURE-01` are FIXTURE_VERIFIED.

## Claim-time evidence

- Graft command: `trail grep "inspect_members" --in scripts/`. It located `inspect_members` at `scripts/audio_bank_header.py:101-198` and the current synthetic success/identity-mismatch tests at `scripts/test_audio_bank_header.py:240-288`. The live check at `audio_bank_header.py:126-132` rejects every match count other than one before reading a member, inside a `try/finally` that closes the reader.
- Existing fixture reader and helper are in `scripts/test_audio_bank_header.py:171-238`. Baseline SHA-256: `c5a09bccf054d0cd186144a22b602cb2777993857cd87d5ad3846cc04e230912`. Production source baseline SHA-256: `bdad7d192b0ecdb929daf5147a139e7545fd5c546f01ed4ffb4005f2cea9f9c0`.
- Checkout at claim: branch `main`, HEAD `667a88cda4c844a323b81c9adafdb9ffb5e63583`, matching `origin/main`. Only pre-existing `native/api/CMakeLists.txt` and `native/api/host/` changes are present and excluded.

## Acceptance and boundaries

- Acceptance command: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_audio_bank_header.py`. Each synthetic case must assert the exact `expected one exact PSARC path for 'sound/fixture.hd', found N` message, `read_calls == []`, and `closed is True`.
- Observation: the matching-count guard precedes entry lookup/member reads, and cleanup is in `finally`. Inference: both no-match and duplicate-match cases should exercise the same pre-read rejection path.
- Limits: synthetic reader only; no real archive, payload, codec, playback, or gameplay claim. No game acceptance gate advances.
- Next exact action: after claim publication, assign only `scripts/test_audio_bank_header.py` to `slot23_raw_xrefs`; root will inspect the diff, replay the acceptance command, and record the completion checkpoint and E135.
- Claim-index command: `python3 scripts/project_index.py` exited 0 and wrote the generated index; 14,214 folders indexed. Log: `local/checkpoints/root-round11-claims/index.log`.
- Claim-graph command: `trail build --check` exited 0; wiring graph in sync. Deep layer remains unbuilt. Log: `local/checkpoints/root-round11-claims/trail-check.log`.
- Repository-state command: `git status --short` showed only the three intended documentation/index changes plus the pre-existing `native/api/CMakeLists.txt` and untracked `native/api/host/`; log: `local/checkpoints/root-round11-claims/repo-state.log`.
- Publication checkpoint (2026-10-21 11:02 CDT): commit `8723be0eba56ba3ed675dc73b7379edc6f2391eb` pushed with `git push origin main`, exit 0 (`667a88c..8723be0`); log: `local/checkpoints/root-round11-claims/push.log`. `git rev-parse HEAD` and `git rev-parse origin/main` both returned `8723be0eba56ba3ed675dc73b7379edc6f2391eb`.
- Publication gates: `git diff --cached --check` exit 0; `python3 -I scripts/audit_publish_paths.py` exit 0 for 6 staged paths; `python3 -I scripts/audit_publish_paths.py --all` exit 0 for 1,509 paths. Logs: `local/checkpoints/root-round11-claims/{cached-check.log,audit-staged.log,audit-all.log}`. Only the excluded host escalation paths remain dirty.
- Private index recheck: two `python3 scripts/project_index.py --check` runs found stale `local/indexes/folders.json` while new private log files were still being added (outputs in `local/checkpoints/root-round11-claims/index-check.log` and `index-check-after-log-addition.log`). After all log paths existed, `python3 scripts/project_index.py` and `python3 scripts/project_index.py --check` both exited 0 at 14,214 folders; logs: `index-final.log` and `index-final-check.log`. Public `docs/FOLDERS.md` remains current.
- Next exact action: assign this task's single test file to `slot23_raw_xrefs`; root owns documentation, replay, and publication.
