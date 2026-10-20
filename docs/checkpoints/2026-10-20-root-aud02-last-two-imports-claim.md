# AUD-02 — final two NGS2 import probes claimed for offline preparation

- Time / timezone: 2026-10-20 03:20 CDT (America/Chicago).
- Agent / integrator: `/root`; root owns shared docs and provider serialization.
- Parent / gate: `AUD-02`; G2 remains NOT RUN. The two bounded import tasks are ACTIVE with zero
  failed targeted analysis/provider tries. Their workers will be dispatched after publication.
- Task IDs and live manifest identities:
  - `AUD-02-NGS2-RACKDESTROY-01`: `sceNgs2RackDestroy`, symbol 237, NID `lCqD7oycmIM`, slot
    `0x1600ff0`.
  - `AUD-02-NGS2-VOICE-GETSTATEFLAGS-01`: `sceNgs2VoiceGetStateFlags`, symbol 245,
    NID `rEh728kXk3w`, slot `0x1601030`.
- Prerequisites: RE-02 import manifest, AUD-01 inventory, and Stage 3 in
  `docs/REVERSE_ENGINEERING.md`. Exact ELF SHA-256
  `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`; import manifest SHA-256
  `78caaaae139b7c5d4d2618abab2e50e9f6918ea3cec73a90b81e0005c83b6744`.
- Root checked the live manifest with `python3 -I -c 'import json; ...'` from repository root;
  exit 0. Exact complete argv is retained in the local output record
  `local/checkpoints/2026-10-20-root-aud02-last-two-imports-live-records.json`, SHA-256
  `293ff633443e4d10b82644ab6e901ff038f527dda1cf0959046e6dfe4919d647`. It confirms both names,
  indices, NIDs and `first_site` slots. `first_site` is a relocation slot, not a game caller.
- Bounded outcome per task: privately verify the import/relocation tuple against the exact ELF,
  scan for the candidate slot thunk and raw direct-CALL byte patterns, and produce a reproducible
  offline checker with exact commands/output hashes. These are preparation claims only: they do not
  accept instruction boundaries, callers, function attribution, reachability, ABI, NGS2 meaning or
  runtime use. The later call-path task still requires serialized REA evidence and root byte replay.
- Private ownership: RackDestroy dossier
  `local/re/evidence/aud02-ngs2-rackdestroy-01/`; VoiceGetStateFlags dossier
  `local/re/evidence/aud02-ngs2-voice-getstateflags-01/`. No worker may edit shared docs or Git.
  No REA/Ghidra command is authorized by this claim; the serialized slot is currently assigned to
  the single RackCreate function query at `0x9493f8`. Root will release it explicitly.
- Graft source navigation found the offline parser at `scripts/ps4_dynlink.py` via
  `graft ask "Locate the offline import parser and explain how it records NGS2 import symbol indices, NIDs, first_site and JMPREL slots" --source --in scripts/`.
  Live manifest rows above were independently checked; Graft's source index is navigation only.
- Shared files changed: `HANDOFF.md`, `docs/TASKS.md`, generated `docs/FOLDERS.md`, and this
  checkpoint. `docs/EVIDENCE.md` and `INSTALL_LOG.md` are unchanged. No source, reference pin,
  game input or binary changed.
- Exact next action: publish this claim, then dispatch RackDestroy and VoiceGetStateFlags offline
  preparation in separate private folders. Wait for worker reports and root-replay their checksums
  before any follow-up provider call. Keep RackQuery and VoiceHandle serialized behind the
  RackCreate query. No NGS2 behavior, audio, runtime, or G1/G2 completion is claimed.
