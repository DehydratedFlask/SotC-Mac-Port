# Checkpoint: AUD-02 NGS2 callsite packets — three bounded claims

- Time / timezone: 2026-10-20 01:38 CDT (America/Chicago).
- Agent and integrator: `/root`; sole shared-document integrator.
- Task / gate / status: `AUD-02-NGS2-SYSCREATE-01`, `AUD-02-NGS2-RACKCREATE-01`, and
  `AUD-02-NGS2-VOICE-CONTROL-01`; G2 remains NOT RUN; all three claims ACTIVE.
- Prerequisite evidence / input and source hashes: `local/re/eboot.elf` SHA-256
  `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`; imported-symbol
  manifest `local/re/imports.json` SHA-256
  `78caaaae139b7c5d4d2618abab2e50e9f6918ea3cec73a90b81e0005c83b6744`; existing RE-02 and
  AUD-01 evidence; `docs/REVERSE_ENGINEERING.md` Stage 3 scope.
- Owned / changed files: claim only; `HANDOFF.md`, `docs/TASKS.md`, this checkpoint, generated
  `docs/FOLDERS.md`. No private evidence or source code changed yet.
- Hypothesis and expected result: resolve one statically identifiable caller/dataflow path for
  each import `sceNgs2SystemCreateWithAllocator`, `sceNgs2RackCreate`, and
  `sceNgs2VoiceControl`; anchor all claimed instruction/data bytes to the exact ELF hash and
  retain unresolved indirect references. The manifest's `first_site` values are import slots,
  not caller addresses.
- Exact command, CWD and nonsecret environment: from repository root,
  `git fetch origin main`; `git rev-list --left-right --count HEAD...origin/main` returned `0 0`;
  `sha256sum local/re/eboot.elf local/re/imports.json` returned the hashes above. `date
  '+%Y-%m-%d %H:%M:%S %Z'` returned `2026-10-20 01:38:22 CDT`.
- Exit / observed result: claim established; no implementation or analysis command run yet.
  The imported-symbol manifest contains 11 NGS2 entries. This is an inventory observation only.
- Log, capture and REA evidence paths/IDs: none yet; workers must keep private dossiers under
  separate `local/re/evidence/aud02-ngs2-{syscreate,rackcreate,voice-control}-01/` directories.
- Observation versus inference / coverage limitations: imported names are labels in a local
  manifest; they do not establish callsite reachability, PS4 parameter ABI, command meaning,
  rack/voice lifecycle, audio playback, or runtime semantics. A bounded static callsite probe is
  useful to AUD-02 but cannot pass G2 or G5.
- Cumulative failed tries and distinct hypotheses: 0 for each task; claim only.
- Remaining risks / blockers: the import's caller and pointer provenance may remain unresolved;
  no first-party Sony contract is assumed. All binary evidence remains private.
- Publication checkpoint — 2026-10-20 01:39 CDT, `/root`: commit `e71c55a`
  (`docs: claim NGS2 callsite probes`) pushed
  `45223f6..e71c55a`. Exact checks: `python3 scripts/project_index.py` exited 0 (4,141 folders);
  `graft check` passed wiring freshness with 1,225 meaning nodes pending; `git diff --check` and
  `git diff --cached --check` passed; `python3 -I scripts/audit_publish_paths.py` accepted four
  staged paths; `python3 -I scripts/audit_publish_paths.py --all` accepted 600 tracked paths;
  `git fetch origin main` succeeded with divergence `0 0`. The only untracked path shown was
  `tools/graft/.cache/telemetry-repo-id.json`; it was not staged.
- Next exact action: dispatch `/root/aud02_syscreate` and `/root/aud02_rackcreate` on the first two
  tasks in separate private evidence directories; root probes VoiceControl and verifies all reports.
- HANDOFF.md / TASKS / EVIDENCE / INSTALL_LOG / indexes updated: HANDOFF, TASKS and generated
  `docs/FOLDERS.md` updated; EVIDENCE and INSTALL_LOG unchanged.

## Dispatch checkpoint — 2026-10-20 01:40 CDT, `/root`

`/root/aud02_syscreate` (gpt-6-luna, Extra High) owns system-creation probe;
`/root/aud02_rackcreate` (gpt-6-luna, Extra High) owns rack-creation probe. Each uses its own
private evidence directory and process-local REA CLI; neither may change shared documentation or
publish. Root remains the integrator and owns the VoiceControl probe. No binary analysis had begun
at dispatch; task failed-try counters remain zero. `HANDOFF.md` and `docs/TASKS.md` record this
assignment. Exact next action: publish this dispatch checkpoint, then root begins the bounded
VoiceControl analysis and verifies worker evidence as it arrives.

## Packet boundaries

Each packet must report the exact imported symbol/NID and relocation slot, how the caller was
resolved, an instruction/data byte range and hash for the selected caller, caller-to-thunk edge,
visible register/stack writes at the call boundary, unresolved points, and a reproducible private
verification recipe. Do not infer semantic parameter names or function contracts from register
position, strings, community headers, or imported symbol names. Do not execute the game or load
Sony modules. Root independently verifies each accepted dossier before adding a public evidence row.
