# AUD-01 sound metadata sidecar byte identities

- Time: 2026-10-20 04:15 CDT (America/Chicago).
- Agent / integrator: `/root`; task `AUD-01-METADATA-SIDECAR-IDENTITY-01`.
- Status: `ARTIFACT_VERIFIED`; one checker correction, zero failed analysis hypotheses.
- Bounded result: exact archive and decoded-member byte identities for nine member instances across six logical sound metadata path families. No member payloads were written or exported.

## Commands and result

- Corrected replay 1: `PYTHONDONTWRITEBYTECODE=1 python3 -I local/validation/aud01-sound-metadata-identity-01.py > local/validation/aud01-sound-metadata-identity-01.run2.json 2> logs/aud01-sound-metadata-identity-01.run2.stderr.log` — exit 0.
- Corrected replay 2: `PYTHONDONTWRITEBYTECODE=1 python3 -I local/validation/aud01-sound-metadata-identity-01.py > local/validation/aud01-sound-metadata-identity-01.run3.json 2> logs/aud01-sound-metadata-identity-01.run3.stderr.log` — exit 0.
- Repeatability check: `cmp local/validation/aud01-sound-metadata-identity-01.run2.json local/validation/aud01-sound-metadata-identity-01.run3.json` — exit 0, no differences. Both JSON reports are 5,682 bytes and SHA-256 `2ce90160cdc38a883d459e66d96be2673a66b264da468ba47b1d69e750ea33c0`. Both stderr logs are empty.
- Report status: `PASS_BYTE_IDENTITY_ONLY`; 3 archives, 9 member instances. The first checker attempt failed before member decoding because it assumed `Psarc.paths()` aligned with TOC entry zero; the corrected mapping uses `paths[member_id - 1]` and TOC entry `member_id`. This single checker correction is recorded separately in [the correction checkpoint](2026-10-20-root-aud01-identity-checker-correction.md).

## Observations

- Base cachedFiles archive: 37,159,164 bytes; SHA-256 `373828b9a6e5c689a66b05a01aa14b295026976438aa7796e9baa789ae03dda0`.
- Patch cachedfiles archive: 37,274,125 bytes; SHA-256 `656d6870a159013921d036d0b7cf18c679bfc62fe5862b31332c459ff7da05d0`.
- pakN archive: 12,838,532,222 bytes; SHA-256 `559f7a3fa66bc652d36c5f1a885b351e4aeb17dadbba06a0006520bfd325be73`.
- Verified paths: base and patch `sound/shadow/_cmn/shadowbanks.crmp`, `shadowbossmusic.crmp`, and `sound_data.calt`; pakN `shadow_sounds.cxml`, `shadowrandomevents.crmp`, and `shadowstreams.crmp`.
- For each selected row, the checker matched manifest ID/path/MD5/offset/block/size to the parsed PSARC TOC, checked the one-based manifest path-table mapping, decoded with `Psarc.read()`, and computed payload length and SHA-256.
- Each of the three base/patch path pairs has differing decoded size and SHA-256. This is only an observation about the bytes in the named archive layers.

## Limits and next action

- No format/schema, codec, patch precedence, mount behavior, runtime binding, game use or audio playback is inferred. No original PKG was read and no REA/Ghidra query was used for this task.
- `python3 scripts/project_index.py` exited 0; 4,156 folders indexed and `docs/checkpoints` reports 63 files; log `logs/root-aud01-complete-voiceflags-correction-project-index.log`.
- `graft check` exited 0 with the wiring graph in sync; deep meaning layer remains unbuilt with 1,225 nodes pending; log `logs/root-aud01-complete-voiceflags-correction-graft-check.log`.
- Staged/full path audits, diff checks, remote divergence, commit and push are pending. `tools/graft/` remains unstaged.
- Next: index and publish the completion checkpoint, then continue with independent tasks. G1 remains NOT PASSED and G2 NOT RUN.
