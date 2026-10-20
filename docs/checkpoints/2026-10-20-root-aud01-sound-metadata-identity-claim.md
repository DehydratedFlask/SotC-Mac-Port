# Claim — AUD-01 sound metadata sidecar identities

- Time: 2026-10-20 04:05 CDT (America/Chicago).
- Agent / integrator: `/root`; task `AUD-01-METADATA-SIDECAR-IDENTITY-01`.
- Status: `CLAIMED`; attempt count 0.
- Bounded outcome: independently validate exact PSARC locator, decoded member size, and SHA-256 for nine member instances covering six logical sound metadata path families. For the three `cachedFiles` path families, compare the base and patch instances. Do not infer format, schema, codec, patch precedence, NGS2 behavior, or runtime use.
- Inputs: `local/manifests/assets/CUSA08809-cachedFiles.members.jsonl`, `CUSA08809-patch-cachedfiles.members.jsonl`, and `CUSA08809-pakN.members.jsonl`; corresponding summary JSON files; extracted archives `local/extracted/CUSA08809/cachedFiles.psarc`, `local/extracted/CUSA08809-pakN/pakN.psarc`, and `local/extracted/CUSA08809-patch/cachedfiles.psarc`.
- Target families: `sound/shadow/_cmn/shadowbanks.crmp`, `shadowbossmusic.crmp`, and `sound_data.calt` in both cachedFiles layers; `shadow_sounds.cxml`, `shadowrandomevents.crmp`, and `shadowstreams.crmp` in pakN. These are six logical names and nine archive-member records.
- Planned owned files: private `local/validation/aud01-sound-metadata-identity-01.py`, JSON result, and repeated-run logs; root owns `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/FOLDERS.md`, and claim/completion checkpoints.
- Planned acceptance command: run the identity script twice with `PYTHONDONTWRITEBYTECODE=1 python3 -I local/validation/aud01-sound-metadata-identity-01.py`, compare the JSON outputs with `cmp`, and verify the report against the three manifest rows/summary identities and `Psarc.read()` results. Exact final invocation/log paths will be recorded at completion.
- Safety / scope: read extracted PSARC files through the existing parser; do not read original PKGs, write extracted member payloads, edit parser/source, invoke REA/Ghidra, or make runtime/format claims. This task is independent of the in-flight serialized VoiceGetStateFlags provider query.
- Project-index check: `python3 scripts/project_index.py` exited 0, indexed 4,155 folders; captured at `logs/root-aud01-identity-claim-project-index.log`.
- Graft check: `graft check` exited 0, wiring graph synced; meaning layer remains unbuilt with 1,225 pending nodes; captured at `logs/root-aud01-identity-claim-graft-check.log`.
- Publication: pending staged/full path audits, diff checks, remote-divergence check, commit and push before creating/running the private checker.
- Next exact action: publish this claim checkpoint, then build and replay the private identity checker twice.
