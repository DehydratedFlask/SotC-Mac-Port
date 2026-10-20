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
- Owned / changed files at claim creation: `HANDOFF.md`, `docs/TASKS.md`, this checkpoint, and
  generated `docs/FOLDERS.md`; no private evidence or source code changed at that time.
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
- Cumulative failed tries at claim creation: 0 for each task. The root VoiceControl task later
  had one MCP startup timeout; see the dedicated failure checkpoint below.
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
- Next exact action at claim creation: dispatch separate system/rack workers after publishing the
  claim, while root probes VoiceControl. Current assignments and failure/recovery state are in the
  dispatch and VoiceControl checkpoints below.
- HANDOFF.md / TASKS / EVIDENCE / INSTALL_LOG / indexes updated: HANDOFF, TASKS and generated
  `docs/FOLDERS.md` updated; EVIDENCE and INSTALL_LOG unchanged.

## Dispatch checkpoint — 2026-10-20 01:40 CDT, `/root`

`/root/aud02_syscreate` (gpt-6-luna, Extra High) owns system-creation probe;
`/root/aud02_rackcreate` (gpt-6-luna, Extra High) owns rack-creation probe. Each uses its own
private evidence directory and process-local REA CLI; neither may change shared documentation or
publish. Root remains the integrator and owns the VoiceControl probe. No binary analysis had begun
at dispatch; task failed-try counters remain zero. `HANDOFF.md` and `docs/TASKS.md` record this
assignment. Publication: commit `3e5ecea` (`docs: record NGS2 probe assignments`) pushed
`d243989..3e5ecea`; staged path audit accepted three allowlisted paths, full audit accepted 600,
`git diff --cached --check` passed, fetch showed divergence `0 0`.

## VoiceControl probe checkpoint — 2026-10-20 01:47 CDT, `/root`

- Task: `AUD-02-NGS2-VOICE-CONTROL-01`; status ACTIVE; failed provider attempts: 1; failed
  analysis hypotheses: 0.
- Exact input: `local/re/eboot.elf`, SHA-256
  `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`; Ghidra 12.1.4 on
  ARM64. REA `open_binary` returned the same hash and x86_64 format.
- Exact targeted check: `mcp__rea__xrefs({address:"0x1601020"})` after opening that ELF through
  REA; it ended with `provider_timeout`, Ghidra startup deadline 330,000 ms, 2 GB configured heap,
  no caller result and no Evidence ID. Run ID `ae5b3b78-9f1a-4d73-aea9-33c5719e55c4`, PID 56090.
- The returned partial diagnostic showed whole-image import and non-target-specific overlap/p-code
  warnings. Temporary Ghidra logs under `/var/folders/.../rea-ghidra-CkuzlE/` were no longer
  present when checked. A private normalized record is
  `local/re/evidence/aud02-ngs2-voice-control-01/root-mcp-xrefs-timeout.json`.
- Observation versus inference: startup exceeded the MCP tool's limit; this is not evidence that
  no caller exists. No analysis hypothesis has been disproved, and no NGS2 contract is inferred.
- Next exact action: retry using the existing REA CLI and the documented 8 GB/1,200,000 ms
  environment; save a fresh private log/snapshot and resolve at most one caller path. A larger
  timeout or agent/model change does not reset the failed-provider counter.
- Shared docs changed: HANDOFF and TASKS; EVIDENCE/INSTALL_LOG unchanged. Publication of this
  checkpoint precedes the CLI recovery attempt.

## CLI recovery preparation checkpoint — 2026-10-20 01:50 CDT, `/root`

- `mcp__rea__close_binary({})` completed with `result: null`; no successful query Evidence existed
  to export. The provider timeout did not establish caller presence/absence.
- `rea` is not on PATH (`rea --help` exited 127). The existing absolute CLI path
  `/Users/dehydratedflask/.npm/_npx/a1722018f28f9df2/node_modules/.bin/rea` reports version 6.7.0;
  its `--help` and `xrefs --help` commands exited 0.
- Existing `local/re/rea-snapshot.json` targets the same ELF SHA-256 and Ghidra 12.1.4 profile
  digest `769518a8e9b57f953dd241adbb63672f0f486bb3d66359b9deef7c5e088ca7aa`; file SHA-256
  `d80232c4fe22d7e75654eade8db44af093507a94723f80dd5ce67605dd382354`, one existing cached entry.
- Next exact action: copy this snapshot into this task's private evidence directory; run the absolute
  CLI `xrefs` command with `GHIDRA_HEADLESS_MAXMEM=8G` and
  `REA_GHIDRA_STARTUP_TIMEOUT_MS=1200000`; preserve complete JSON, stderr and exit code. The first
  provider attempt remains one timeout; do not count the help/close operations as new attempts.

## CLI provider-configuration failure — 2026-10-20 01:51 CDT, `/root`

- Task: `AUD-02-NGS2-VOICE-CONTROL-01`; failed provider invocations: 2; failed analysis hypotheses:
  0. This configuration error is not a negative xref result.
- Root copied the exact matching `local/re/rea-snapshot.json` to
  `local/re/evidence/aud02-ngs2-voice-control-01/rea-snapshot.json`; both SHA-256 values remain
  `d80232c4fe22d7e75654eade8db44af093507a94723f80dd5ce67605dd382354`.
- Exact CLI command: `env GHIDRA_HEADLESS_MAXMEM=8G REA_GHIDRA_STARTUP_TIMEOUT_MS=1200000
  /Users/dehydratedflask/.npm/_npx/a1722018f28f9df2/node_modules/.bin/rea xrefs
  "/Volumes/Extra/SoC Mac Native/local/re/eboot.elf" 0x1601020 --provider ghidra --snapshot
  "/Volumes/Extra/SoC Mac Native/local/re/evidence/aud02-ngs2-voice-control-01/rea-snapshot.json"
  --format json --full-output`, from repo root. Exit 1 in 1.48 seconds, stdout file SHA-256
  `d5767fb41ab4ad16791bc31155b1b50b73d0fa604da59f260478e35d6442966f`, stderr empty (SHA-256
  `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`). REA returned
  `provider_unavailable/not_configured` because CLI environment lacked `GHIDRA_INSTALL_DIR`; no
  Ghidra process or xref Evidence was created.
- The CLI path does not change the task input or analysis hypothesis. Existing Ghidra path from
  REA's live session inventory is `/Users/dehydratedflask/Applications/ghidra_12.1.4_PUBLIC`.
- Next exact action: retry once with that absolute path in `GHIDRA_INSTALL_DIR` plus the existing
  heap/deadline values. This is the third provider attempt; if it fails, stop and escalate under
  AGENTS.md. Do not reset the failed attempt count.

## Packet boundaries

Each packet must report the exact imported symbol/NID and relocation slot, how the caller was
resolved, an instruction/data byte range and hash for the selected caller, caller-to-thunk edge,
visible register/stack writes at the call boundary, unresolved points, and a reproducible private
verification recipe. Do not infer semantic parameter names or function contracts from register
position, strings, community headers, or imported symbol names. Do not execute the game or load
Sony modules. Root independently verifies each accepted dossier before adding a public evidence row.
