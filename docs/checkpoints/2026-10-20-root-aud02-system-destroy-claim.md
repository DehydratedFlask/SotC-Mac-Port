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

## Dispatch checkpoint — 2026-10-20 02:19 CDT, `/root`

- Claim publication: commit `19e2a43` (`docs: claim NGS2 system-destroy callpath probe`) pushed
  `873b62c..19e2a43` to `origin main`. Before commit, `git diff --cached --check` passed; staged
  path audit accepted the 4 allowlisted docs files; full audit accepted 601 tracked paths; fetch
  showed divergence `0 0`. The pre-existing untracked `tools/graft/` cache was not staged.
- Worker dispatched with `collaboration.spawn_agent`: name `aud02_sysdestroy`, model
  `gpt-6-luna`, reasoning effort `xhigh`, private output
  `local/re/evidence/aud02-ngs2-system-destroy-01/`. Tool returned agent `/root/aud02_sysdestroy`;
  analysis had just started and no result was available at dispatch.
- Worker may edit only its private dossier. Root retains shared-doc integration and will replay the
  checksum manifest and verifier before changing task status or public evidence. Failed tries at
  dispatch: 0.
- Post-dispatch checkpoint checks: `python3 scripts/project_index.py` exited 0 (4,147 folders; no
  `docs/FOLDERS.md` content change); `graft check` exited 0 (wiring graph in sync, deep meaning
  layer remains unbuilt with 1,225 nodes pending); `git diff --check` passed. Staged audit accepted
  3 allowlisted docs paths; full audit accepted 601 tracked paths; fetch showed divergence `0 0`.
  Exact next action: review the worker's dossier and report when ready; preserve all
  ABI/runtime/semantic limits stated in the claim.

## Interim slot-to-thunk checkpoint — 2026-10-20 02:28 CDT, worker `/root/aud02_sysdestroy`

- Worker-reported progress pending root replay; task remains ACTIVE; failed targeted tries remain
  0. The existing parser command
  `python3 -I scripts/ps4_dynlink.py local/re/eboot.elf --out
  local/re/evidence/aud02-ngs2-system-destroy-01/imports.reparsed.json` exited 0 and matched the
  unique slot `0x1601010` to symbol 241 / NID `u-WrYDaJA3k`. Parser stdout SHA-256
  `f8994eafed26db78a1289c62ceec9179c41aee83f38179e6c30aa898dd5fe03f`; stderr empty.
- Independent raw scan `python3 -I
  local/re/evidence/aud02-ngs2-system-destroy-01/scan_ff25_target.py` reported one `FF 25` match
  at `0xf91440`, bytes `ff25cafb6600`, targeting the claimed slot; stdout SHA-256
  `877edc78540d6371fffd0580a088cd61e5f2411a28a961e7decbde21e7217c09`.
  `python3 -I scripts/ps4_dynlink.py local/re/eboot.elf --out
  local/re/evidence/aud02-ngs2-system-destroy-01/imports.with-thunk.json --thunk 0xf91440` exited
  0 and independently mapped that thunk to the JUMP_SLOT, symbol 241, and matching NID; target
  JSON SHA-256 `87b93b1196f0d55067ba6b353c1d2b55539fbad2ee9ab52a33e514bd689ea8af`.
- REA 6.7.0 / Ghidra 12.1.4 slot query at `0x1601010`, using the exact pinned ELF and private
  snapshot with 8 GB heap / 1,200,000 ms deadline, exited 0 after about 6 minutes. Full JSON
  `local/re/evidence/aud02-ngs2-system-destroy-01/slot-xrefs.json` SHA-256
  `6da0467ea24486330ac90799353446bd816c6a7198aa52927e1a1098002fe49b`; stderr empty; Evidence ID
  `ev_37ba3af33cc684de727dfb407d145e99b12d646a8d3f3fe219a31b2c07fbd6d1`. It returns address
  `0xf91440`; xrefs output omits reference kind and has not identified a direct caller.
- Exact next action: finish the targeted REA function query at `0xf91440`, preserve results and
  unresolved edges, then byte-check a direct caller if one is found. No NGS2 meaning, ABI, runtime
  binding, call reachability, or audio behavior is inferred.
