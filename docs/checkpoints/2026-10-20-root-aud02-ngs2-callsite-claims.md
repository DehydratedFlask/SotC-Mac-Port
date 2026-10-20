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
- Publication: commit `d7cc85d` (`docs: record NGS2 CLI config failure`) pushed
  `d78fd8c..d7cc85d`; staged path audit passed three allowlisted paths, full audit passed 600,
  `git diff --cached --check` passed, fetch showed divergence `0 0`. Full audit log is
  `local/checkpoints/2026-10-20-root-aud02-cli-config-publish-all.log`, SHA-256
  `8f022f03c0f875d7113c9cd175501ae1b25edf7b6cb184065982160db56df4d4`.

## Rack-create root review — 2026-10-20 02:01 CDT, `/root`

- Task: `AUD-02-NGS2-RACKCREATE-01`; bounded status `STATIC_PATH_BYTE_VERIFIED`; worker recorded
  one checker correction failure and zero failed analysis hypotheses. Root independently replayed
  the dossier checksum manifest and byte verifier.
- Exact commands from `local/re/evidence/aud02-ngs2-rackcreate-01/`: `shasum -a 256 -c
  SHA256SUMS.txt` (exit 0); `python3 -I relocation_probe.py` (exit 0; JSON `status=PASS`). Root
  outputs: checksum log SHA-256
  `f69469305e050a01078d2608eec06897fa62a7e6aa3989c4c79bf85be8f1f7cd`; verifier JSON SHA-256
  `9c4dc2ecdffd331f165cd4b51cc25876531ca8a7077213c4016f3499a778ce29`; verifier stderr empty.
- Observation: bytes verify `FUN_0099f9d0` → direct call `0x99fbda` → thunk `0xf913e0` → import
  slot `0x1600fe0` → symbol index 235 / NID `cLV4aiT9JpA`. Evidence IDs are
  `ev_a9e34cdb3a950ac6383904cc81900d855e237014bcce91559c6ab3c3ed33700b` (slot xrefs) and
  `ev_2deb0a5ce856ce8e4a229183e5ab1a5819498ebe9398990d786bcb62f2f95e8b` (caller function).
- Limitations: targetless `CALL RAX` at `0x99fa67`, runtime import binding and reachability remain
  unresolved. No ABI, command meaning, NGS2 behavior, or runtime claim. No game execution.
- Files changed: HANDOFF, TASKS, EVIDENCE, this checkpoint. `INSTALL_LOG.md` unchanged. Next exact
  action: continue independent system-creation work; no more rack analysis is needed for this
  bounded packet.

## VoiceControl slot-reference checkpoint — 2026-10-20 02:01 CDT, `/root`

- Task: `AUD-02-NGS2-VOICE-CONTROL-01`; status ACTIVE; failed provider attempts 2 (MCP startup
  timeout; CLI missing `GHIDRA_INSTALL_DIR`); failed binary-analysis hypotheses 0. Third provider
  invocation succeeded and does not increase the failed-try count.
- Exact check: `env GHIDRA_INSTALL_DIR=/Users/dehydratedflask/Applications/ghidra_12.1.4_PUBLIC
  GHIDRA_HEADLESS_MAXMEM=8G REA_GHIDRA_STARTUP_TIMEOUT_MS=1200000
  /Users/dehydratedflask/.npm/_npx/a1722018f28f9df2/node_modules/.bin/rea xrefs
  "/Volumes/Extra/SoC Mac Native/local/re/eboot.elf" 0x1601020 --provider ghidra --snapshot
  "/Volumes/Extra/SoC Mac Native/local/re/evidence/aud02-ngs2-voice-control-01/rea-snapshot.json"
  --format json --full-output`, stdout `local/re/evidence/aud02-ngs2-voice-control-01/root-cli-xrefs-attempt3.json`,
  stderr `root-cli-xrefs-attempt3.stderr.log`. Exit 0 after 368,841 ms; result SHA-256
  `1e92beb96166045a5400d31b8c26bf3fe0c9023f988fb87b5c7fb27e493857a7`; stderr empty. ELF SHA-256
  `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`; REA 6.7.0/Ghidra
  12.1.4; profile digest `769518a8e9b57f953dd241adbb63672f0f486bb3d66359b9deef7c5e088ca7aa`.
- Evidence ID `ev_6a01174bbe75c6fd1ee3aab2a3c6e3f9f05ff63a84cb77ee9d9d9423635ac22c`; slot query
  `0x1601020` returned one reference address `0xf91460`, confidence `observed`. The xrefs result
  omits reference kind and does not yet identify a caller or callsite.
- Observation versus inference: one address is an observed static reference result. It does not
  show that a VoiceControl call executes or identify function arguments, ABI, runtime import
  binding, or NGS2 behavior.
- Remaining risk / next exact action: run REA CLI `function` on `0xf91460` with the same ELF,
  Ghidra settings, and private snapshot; preserve JSON, stderr, and exit. The next failed targeted
  provider check would be the third unsuccessful try and requires stopping and escalating this
  task under AGENTS.md.
- Files changed: HANDOFF, TASKS, EVIDENCE, this checkpoint. No code, binary, or install change;
  `INSTALL_LOG.md` unchanged. G1 remains NOT PASSED.

## System-create root review — 2026-10-20 02:08 CDT, `/root`

- Task: `AUD-02-NGS2-SYSCREATE-01`; bounded status `STATIC_PATH_BYTE_VERIFIED`; failed
  hypothesis-driven tries 0. Worker’s optional `inspect-binary-layout` capability check returned
  unsupported for its Linux-x64-only profile and was not used in the analysis.
- Exact root commands from `local/re/evidence/aud02-ngs2-syscreate-01/`: `shasum -a 256 -c
  SHA256SUMS` (exit 0; 26 entries); `python3 -I verify.py` (exit 0; JSON `result=PASS`). Root
  checksum output SHA-256 `16d6c9a282536ea7b28fa73056eb7ec470ef3cae22e543c8f675ec58905d1b65`; root
  verification JSON SHA-256 `135ced0d0c69bd6148b15742d98748802d803d5a9222da822cc9a6050b359807`; stderr
  empty.
- Observation: exact ELF/import manifest bytes verify slot `0x1601008` (`JMPREL`
  `R_X86_64_JUMP_SLOT`, symbol index 240 / NID `mPYgU4oYpuY`) → thunk `0xf91430` → direct call at
  `0x94932e` in `FUN_009485a0`. The checked 44-byte pre-call window `0x949307..0x949332` has
  SHA-256 `99e458a9170b402ead55f3b0444dc66758a1e425325a65ffc8ad39d0e4f94887`. REA Evidence IDs:
  `ev_4b730eb76f6268025a4a4435ddc14dc9ec2b474bda906a67ee269b4cfd06192b`,
  `ev_3dcf38cb48579627b09c576918571f0b2fef7efd747306dea1b01b2616affab0`,
  `ev_3c328e924109833a20ed5f3ccb54b5395b4aa0561ab1974c4e77ff97a457385f`. Worker report
  `REPORT.md` SHA-256 `a8021ebd058884f8de5df9e58b7fcf31a582d7513d9ea59babe9a68d53f94e4d`.
- Limits: runtime slot binding and callee unresolved; the loaded EAX value from `[RBP-0x5fc]`
  lacks a resolved reaching definition; high-p-code is truncated, with at least one omitted
  operation and 729 known omitted edges. No argument roles, ABI, NGS2 behavior, runtime use, or
  game behavior inferred.
- Files changed: HANDOFF, TASKS, EVIDENCE, this checkpoint. `INSTALL_LOG.md` unchanged. Next exact
  action: complete the in-flight VoiceControl function query at `0xf91460`; a failure would be its
  third unsuccessful targeted try and would require escalation.

## VoiceControl root review — 2026-10-20 02:13 CDT, `/root`

- Task: `AUD-02-NGS2-VOICE-CONTROL-01`; bounded status `STATIC_PATH_BYTE_VERIFIED`. Failed provider
  attempts remain 2 (MCP timeout and missing CLI `GHIDRA_INSTALL_DIR`); failed analysis hypotheses
  0. Both later CLI queries and root byte verification succeeded.
- REA slot-xref query at `0x1601020` exited 0 after 368,841 ms and returned `0xf91460`; Evidence ID
  `ev_6a01174bbe75c6fd1ee3aab2a3c6e3f9f05ff63a84cb77ee9d9d9423635ac22c`; JSON SHA-256
  `1e92beb96166045a5400d31b8c26bf3fe0c9023f988fb87b5c7fb27e493857a7`. The subsequent REA
  `function` analysis of `0xf91460` exited 0 after 364,994 ms; Evidence ID
  `ev_87eece642729f1ee3dcea0bb20a8601d59b5ae3f6c0af3be617c37e6109bb3ac`; JSON SHA-256
  `9a0f4d55f19cf441dbedeb61eff6d45bf804dbb3da6d646fa075b548709861a6`; stderr empty. The thunk
  query reports 126 incoming references over 22 caller functions. Its Ghidra entry classification
  is `thunk_FUN_00f91466`, with an adjacent computed-jump edge; a separate indirection edge targets
  slot `0x1601020`.
- Exact byte verifier `python3 -I
  local/re/evidence/aud02-ngs2-voice-control-01/verify.py` exited 0 with
  `STATIC_PATH_BYTE_VERIFIED`; output SHA-256
  `ca3a24f2795f2312062b946ca6d017d6d211dded2f65ec85ef68dd79e1db0e2e`; stderr empty. The 17-file
  dossier manifest check exited 0; root checksum log SHA-256
  `7d578b54f337ec33aedb72d91691a4e56d11966431790b7d0d97fd5e6dd0277e`. Report SHA-256
  `cfcd998e0817970168054dacf4e0a36ca3514b369c09b04d209ac15e992f2a16`; verifier SHA-256
  `8dd9d5a6308247bc04628c3f24ef1d9e4120a078b8af3c705e414d7108f2e7de`.
- Observation: manifest symbol index 243 / NID `uu94irFOGpA` maps through `R_X86_64_JUMP_SLOT`
  `0x1601020`; thunk bytes `ff25bafb6600` at `0xf91460` decode to that slot. A direct call at
  `0x9496d9` in `FUN_009485a0` has bytes `e8827d6400` and decodes to the thunk. The 49-byte
  pre-call range `[0x9496ad,0x9496de)` hashes to
  `485a817c02f7641cb0062cce59e2dbb88b3832a6243b0cce5885025e0cb225b7`. Its visible register and
  stack writes are recorded in the private report without assigning argument roles.
- Limits: runtime binding/callee and call reachability unknown; the adjacent computed-jump edge is
  inconsistent with the six raw thunk bytes and remains documented as a Ghidra boundary issue. No
  ABI, NGS2 semantics, or runtime behavior inferred. No game execution.
- Files changed: private report, recipe, verifier and result files under
  `local/re/evidence/aud02-ngs2-voice-control-01/`, plus HANDOFF, TASKS, EVIDENCE and this
  checkpoint. `INSTALL_LOG.md` unchanged. The previous system-create publication was commit
  `85fec59` (`docs: verify NGS2 system-create path`), pushed from `357ed0c`.
- After creating the private report/checker files, `python3 scripts/project_index.py` exited 0 and
  reported 4,145 folders indexed. Log SHA-256
  `4455cc1f3a80e3d44a602e9447ed0f93c9b9aeece8300fc9c4d7fe367a25bff8`; `docs/FOLDERS.md` content
  is unchanged.

## Follow-up callpath claims — 2026-10-20 02:13 CDT, `/root`

- Claimed `AUD-02-NGS2-SYSRENDER-01` for symbol index 242 / NID `i0VnXM-C9fc` / slot `0x1601018`,
  and `AUD-02-NGS2-VOICE-GETSTATE-01` for symbol index 244 / NID `-TOuuAQ-buE` / slot `0x1601028`.
- Planned owners `/root/aud02_sysrender` and `/root/aud02_voicestate`; separate private evidence
  directories. Both tasks have zero failed tries and no analysis started.
- Next exact action after this claim is published: dispatch both with `gpt-6-luna`, Extra High.
  Root retains integration and publication ownership. No source, executable, or game files changed.

## Packet boundaries

Each packet must report the exact imported symbol/NID and relocation slot, how the caller was
resolved, an instruction/data byte range and hash for the selected caller, caller-to-thunk edge,
visible register/stack writes at the call boundary, unresolved points, and a reproducible private
verification recipe. Do not infer semantic parameter names or function contracts from register
position, strings, community headers, or imported symbol names. Do not execute the game or load
Sony modules. Root independently verifies each accepted dossier before adding a public evidence row.

## Interim VoiceGetState slot-xref checkpoint — 2026-10-20 02:28 CDT, worker `/root/aud02_voicestate`

- Worker-reported provider success; root replay pending; `AUD-02-NGS2-VOICE-GETSTATE-01` remains
  ACTIVE with 0 failed targeted tries. The exact input ELF/import-manifest hashes match the claim.
- The parser command argv is in
  `local/re/evidence/aud02-ngs2-voice-getstate-01/parser-with-contracts.command.json`; exact target:
  `python3 -I scripts/ps4_dynlink.py local/re/eboot.elf --contracts
  reference/shadPS4/src/core/libraries --out
  local/re/evidence/aud02-ngs2-voice-getstate-01/imports.contracts.json`. Exit 0; captured stdout
  `parser-with-contracts.stdout.txt` SHA-256
  `4fa19b75352a27db8b01bd33377a68227496591a4ce4b8f65ac887f5ff2d663c`; stderr empty; output JSON
  SHA-256 `0066e845b6469e42344bfcb5caadeb934c7d6ba6047f433aee64ab4763842083`.
- Independent verifier argv is in `local/re/evidence/aud02-ngs2-voice-getstate-01/import-bytecheck.command.json`;
  exact target: `python3 -I local/re/evidence/aud02-ngs2-voice-getstate-01/verify_path.py --elf
  local/re/eboot.elf --manifest local/re/imports.json`. Exit 0; output SHA-256
  `13580cc84cf3bca99a27238a200d144282abb70c4765de932c375e9af6d2b555`; stderr empty. It reports
  JMPREL entry 526 at file offset `0x2de48b8`, raw 24-byte record
  `281060010000000007000000f40000000000000000000000`, targeting slot `0x1601028` with relocation
  type 7, symbol index 244, addend 0.
- From repository root, REA 6.7.0 `xrefs` queried slot `0x1601028` with Ghidra 12.1.4, the private
  snapshot, `GHIDRA_HEADLESS_MAXMEM=8G`, and
  `REA_GHIDRA_STARTUP_TIMEOUT_MS=1200000`; exit 0 after 371.425 s. Exact argv is in
  `local/re/evidence/aud02-ngs2-voice-getstate-01/rea-slot-xrefs.command.json`; output
  `rea-slot-xrefs.stdout.txt` SHA-256
  `3e96d3476b6af8d68d560802d4a567ce23ed17a7e13beb58a9d4a05bf422de15`; stderr empty; exit record
  says 0. Input snapshot SHA-256 `d80232c4fe22d7e75654eade8db44af093507a94723f80dd5ce67605dd382354`;
  updated snapshot SHA-256 `33a1cda725208f6438e113dbae67425b65fc81439ca1090f8cea0ed92d8e096f`.
- Evidence ID `ev_d97708e1afeaa1bfd6ffb28334a35ce08d4c195d9f0947fddb5769210cdac4a1` returned one
  reference at `0xf91470`. As with the other xrefs result, the response omits reference kind; no
  caller was claimed at that time.
- The follow-up REA 6.7.0 function query at `0xf91470` then exited 0 after 379.217 s. Exact argv
  is in `local/re/evidence/aud02-ngs2-voice-getstate-01/rea-thunk-function.command.json`; output
  `rea-thunk-function.stdout.txt` SHA-256
  `38566d0e03ebd63f58a93640050ad65ddd81438e7907c37a8a75d048d04f7999`; stderr empty; Evidence ID
  `ev_5217b06df0de0cec23cf13c060f5a3687aee147d75b5d928ffb3c57ed634e0e0`. Worker-reported output
  shows `JMP qword ptr [0x01601028]` and one incoming reference classified
  `UNCONDITIONAL_CALL` at `0x94a27d` in `FUN_0094a200`; post-query snapshot SHA-256
  `96a4d11b6904085fee939b07b4f50a58cd3953995c0c3facc1e1784a7e9d26ef`.
- Exact next action: query `FUN_0094a200`, preserve the instruction bytes/window and unresolved
  edges, and complete the private byte verifier and manifest for root replay. No NGS2
  API/audio/runtime semantics are inferred.

## Interim SystemRender slot/caller candidate checkpoint — 2026-10-20 02:30 CDT, worker `/root/aud02_sysrender`

- Worker-reported progress pending root replay; `AUD-02-NGS2-SYSRENDER-01` remains ACTIVE; failed
  targeted tries 0. Slot `0x1601018` xrefs exited 0 and returned `0xf91450`, Evidence ID
  `ev_90786a1998b746a670a1e01d847201edaddc9751c589406f0126cfb6edd7c421`; JSON
  `local/re/evidence/aud02-ngs2-sysrender-01/rea-xrefs-slot.stdout.json` SHA-256
  `e9883e93780635a6e221a60ee7cd77630378a6f5f1f504f1bf2881073fedc4fc`; stderr empty; exact argv,
  exit and stderr records are in `rea-xrefs-slot.command.txt`, `.exit.txt` and `.stderr.log`.
- Raw call scan command `python3 -I
  local/re/evidence/aud02-ngs2-sysrender-01/raw-call-scan.py` exited 0; script SHA-256
  `9185a47e06e601b979906f314888cd11f0de1f4326026dcd81b8337544062a77`, JSON result SHA-256
  `6c95726ca28a2ce4fe21111bdbbabf0d4e4a9c7892316b65f80f612bdb361ba2`; it proposes one direct-call
  candidate at `0x95383c`, bytes `e80fdc6300`. `objdump -D -M intel --start-address=0x953800
  --stop-address=0x953850 local/re/eboot.elf` exited 0 with output SHA-256
  `ae9f57f5b098030158f2cc75899a1e7996e1c95d62eb71e3b36a22824789765a`; a separate thunk disassembly
  from `0xf91440` through `0xf91470` exited 0 with output SHA-256
  `e02429d1b322d3b4947df5981149a240d2accd2c40da72ba4606de514b3a229f` and a section-range warning.
  Worker reports thunk bytes `ff25c2fb6600` at `0xf91450` targeting slot `0x1601018`.
- The targeted incoming-reference query at thunk `0xf91450` exited 0 after 378.001 s and returned
  `0x95383c`, Evidence ID `ev_62e509cad7286b27bc9176aeb8ad91e10f39728f6ded8009f3fdfd18947ee023`; JSON
  `local/re/evidence/aud02-ngs2-sysrender-01/rea-xrefs-thunk.stdout.json` SHA-256
  `e9c7a66d539b98d363cb97d24d43b0215cb53b219579f6d248938f5aec411c4a`. The xrefs response omits
  reference kind, so the worker retains raw bytes as the basis for identifying the direct call.
- Exact next action: run `rea function` at `0x95383c`, finish a byte verifier for the caller range,
  and return the private dossier for root replay. No runtime binding, reachability, ABI, NGS2
  contract, or audio behavior is inferred.
