# AUD-02-NGS2-SYSDESTROY-01 — bounded import callpath claim

- Time / timezone: 2026-10-20 02:19 CDT (America/Chicago).
- Agent / integrator: `/root`; sole shared-document integrator.
- Task: `AUD-02-NGS2-SYSDESTROY-01`; parent `AUD-02`; status ACTIVE; failed tries 0.
- Prerequisites: RE-02 import manifest and AUD-01 local inventory; `docs/REVERSE_ENGINEERING.md`
  Stage 3 and `docs/VALIDATION.md`. The source ELF SHA-256 is
  `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`; `local/re/imports.json`
  SHA-256 is `78caaaae139b7c5d4d2618abab2e50e9f6918ea3cec73a90b81e0005c83b6744`.
- Claim: resolve one static slot → thunk → direct caller path for imported symbol
  `sceNgs2SystemDestroy`, symbol index 241, NID `u-WrYDaJA3k`, relocation slot `0x1601010`. The
  manifest has one site. Its `first_site` names the relocation slot, not a game callsite.
- Exact prerequisite check from repository root: `python3 -I - <<'PY'` loading
  `local/re/imports.json`, selecting the exact `name`, and printing `name`, `symbol_index`, `nid`,
  `first_site`, `sites`, `library`, and `module`; exit 0. It returned the claimed tuple above.
  `sha256sum local/re/eboot.elf local/re/imports.json` returned the hashes above.
- Claim-preparation checks from repository root: `python3 scripts/project_index.py` exited 0 and
  indexed 4,147 folders, updating `docs/FOLDERS.md` from 44 to 45 checkpoint files;
  `graft check` exited 0 with the wiring graph in sync (deep meaning layer remains unbuilt, 1,225
  nodes pending); `git diff --check` exited 0. No code or private artifacts changed.
- Planned private output: `local/re/evidence/aud02-ngs2-system-destroy-01/`; no game bytes,
  executables, references, or private evidence are to be published. Worker must use the existing
  ELF parser and REA CLI, independently byte-check any selected path, and preserve unresolved
  targets and edges. No ABI, NGS2 contract, semantic arguments, reachability, runtime binding,
  or audio behavior may be inferred.
- Acceptance: root replays the worker's checksum manifest and verifier, which must bind the import
  slot, thunk bytes, direct call instruction, pre-call bytes, and exact ELF digest. A static path
  verifies only those bytes; it does not pass G2 or establish game audio.
- At claim creation no worker was assigned and no analysis had started. Exact next action after
  publication: dispatch one independent `gpt-6-luna`, Extra High worker to the private dossier.
  Root owns review, shared documentation, and publication. The two earlier probes for
  `sceNgs2SystemRender` and `sceNgs2VoiceGetState` are already running in separate private
  directories.
- Files changed: `HANDOFF.md`, `docs/TASKS.md`, this checkpoint. `docs/EVIDENCE.md` and
  `INSTALL_LOG.md` are unchanged pending verified output; no source files changed. G1 remains NOT
  PASSED.
