# AUD-02 — next three NGS2 probes: provider timeout and root offline replay

- Local time: 2026-10-20 03:09 CDT (America/Chicago); integrator `/root`.
- Task IDs: `AUD-02-NGS2-RACK-VOICEHANDLE-01`, `AUD-02-NGS2-RACKCREATE-ALLOC-01`,
  `AUD-02-NGS2-RACKQUERY-BUFFERSIZE-01`.
- Status: all remain ACTIVE/PARTIAL; no new call path accepted. VoiceHandle has one failed
  provider attempt and zero failed analysis hypotheses; RackCreate has one Ghidra xrefs query in
  progress and zero failed targeted tries; RackQuery has zero failed targeted tries and awaits the
  serial REA slot. G1 remains NOT PASSED; G2 remains NOT RUN.
- Shared files changed in this checkpoint: `HANDOFF.md`, `docs/TASKS.md`, generated
  `docs/FOLDERS.md`, and this checkpoint. Private root replay outputs remain under each task's
  ignored `local/re/evidence/` dossier. No source, reference clone, installation, or game input
  changed.

## VoiceHandle — provider timeout, independently replayed

- The worker's exact request was `mcp__rea__xrefs({address:"0x1600ff8"})` against the pinned ELF
  (`d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`) with Ghidra 12.1.4.
  It returned `provider_timeout` after `330000 ms` during Ghidra import/startup; no Evidence ID or
  xref result was produced. The provider reported truncated/incomplete output. The open and xrefs
  raw responses were not saved; in-memory serialization hashes are preserved in the private
  summary but are not raw evidence. `close_binary` succeeded and released the provider slot.
- Private worker summary SHA-256
  `ce02e0419f4b9da3b884b80a9ee2d49939006245bd122b040b40aa8d5002b2ea`; checkpoint SHA-256
  `2ad4cca2e23ae15f29045255495a33569d33474891a0180b07a5a0c24eac54e`. Root parsed the summary
  independently with the exact command recorded in
  `local/re/evidence/aud02-ngs2-rack-voicehandle-01/root-timeout-replay.md`; exit 0, JSON parse
  passed, and the two hashes matched. Root replay note SHA-256
  `2efc94175d02e4ab409afe02e85c0c053252b5b596c48c4ae26a06160d6a7c46`.
- Root checked `test -e /var/folders/vx/5z8skhb97w32g817rv7hbh940000gn/T/rea-ghidra-cOj0ZF/ghidra.log`
  (exit 1) and `ps -p 84741 -o pid=,stat=,comm=` (no process row). The transient provider logs
  and failed process were gone. A separate read-only triage found the cause beyond the reported
  startup deadline remains unknown; current host memory/disk observations cannot reconstruct the
  failed run conditions. The save wrapper also had a `TextEncoder` error before xrefs and later
  file-write errors; this is recorded as persistence failure, not an analysis hypothesis failure.
- Observation: the offline scan still finds thunk candidate `0xf91410` for slot `0x1600ff8` and 104
  raw `E8` patterns. They do not establish caller/function attribution or reachability. No Ghidra
  edge is accepted, and the task remains ACTIVE/PARTIAL with one provider timeout.

## RackQueryBufferSize — selected offline call slice root-replayed

- Root ran `objdump -D -M intel --start-address=0x94fc1b --stop-address=0x94fc32 local/re/eboot.elf`
  from the repository root, redirecting stdout/stderr to
  `local/re/evidence/aud02-ngs2-rackquery-buffersize-01/root-objdump-selected.{stdout,stderr}.txt`;
  exit 0. Root stdout is byte-identical to the worker's saved output, both SHA-256
  `5752b8ebc18ae52e20ae35fd72fb1aaf5ecd0fc8bbdea9f658b3fbacdbf28577`. Apple objdump stderr SHA
  `f4b108ab5a619cc939e78fe3a4be0b6fd344bd2c4d40f67fa3c5966f513e8aaa` contains a warning that no
  section overlaps the requested address range. Its PT_LOAD#0 listing decodes
  `0x94fc2d: e8 ee 17 64 00` as `call 0xf91420`.
- Root ran `python3 -I local/re/evidence/aud02-ngs2-rackquery-buffersize-01/verify_path.py`, saving
  stdout/stderr to the root replay paths; exit 0, output SHA-256
  `4da98060c3e2bd2741615958296e2b5f254b9fb2596d7e0b23df8f4b6432e340d`, byte-identical by hash to
  the worker's pre-REA report. It remains `PARTIAL_PRE_REA`, with no selected callsite and 15 raw
  candidates. Live verifier SHA-256 `c0a817276b4ec3880afdcad76dacc6e2663b49133a1492c68751925e70955820`;
  updated worker prep checkpoint SHA-256
  `8bdc575ff7ddee19e6957ff9c26266a449caf3545677c0477b1775a4a3f32630`. Root replay note SHA-256
  `b43c88aaf1a6f4371160b3ec804a7cbd3a51a445e48f5f82cdb9d6c546ab20e1`.
- Root mistakenly attempted `cd local/re/evidence/aud02-ngs2-rackquery-buffersize-01 && shasum -a
  256 -c SHA256SUMS.txt`; exit 2 because this dossier has no such manifest. Error output is
  `local/re/evidence/aud02-ngs2-rackquery-buffersize-01/root-manifest-verify.log`, SHA-256
  `3891b7216e80e9df4ec942e1e781c743fe7f5f612cff1160dc2007cac816ccd9`. This bookkeeping mistake
  did not inspect or modify target bytes and is not a failed analysis hypothesis.
- Observation vs inference: the selected address's offline bytes and objdump listing support a
  direct CALL candidate to `0xf91420`. The tool's section-range warning, absence of REA evidence,
  and lack of function boundaries mean no instruction-boundary-from-entry, reference-manager edge,
  caller identity, control-flow/reachability, ABI, runtime binding, NGS2 meaning, or audio use is
  inferred. Task remains ACTIVE/PARTIAL.

## RackCreateWithAllocator — current serialized query

- Worker report SHA-256 `898dd1ebd19aaf1cf1c3af116a004ae21a6fdd05aac48075f4d7363b81e16468`; private
  manifest SHA-256 `681255138d8ec7c21aaa5becadfbec3c269190a582933b5cd608765b44db073d`. Root independently
  ran `shasum -a 256 -c SHA256SUMS.txt` from that dossier directory; exit 0. Root verification log
  SHA-256 `7de6d7009070159e15c156281fb2fe414bb17ba745d122c8d35127069d6b67bc` covers 61 listed
  artifacts.
- A root attempt to run `python3 -I local/re/evidence/aud02-ngs2-rackcreate-allocator-01/verify_path.py`
  exited 1 because the final checker requires `rea-slot-xrefs.stdout.json`, which does not exist
  before the provider operation. Root captured the traceback in
  `local/re/evidence/aud02-ngs2-rackcreate-allocator-01/root-verify-pre-rea.stderr.log`, SHA-256
  `48b94275c76c8eedc998a39e029b3096b79be16f66d889452ca660e82ad8545b`. This was a premature
  incomplete-input check, not a byte/path hypothesis failure; rerun only after REA output is saved.
- Following the VoiceHandle timeout, root authorized one changed provider plan for RackCreate:
  the agent reports the Ghidra xrefs CLI is running with `GHIDRA_HEADLESS_MAXMEM=8G` and
  `REA_GHIDRA_STARTUP_TIMEOUT_MS=1200000`, and is preserving command, provider logs, exit/status,
  duration, observed peak RSS, and any snapshot before cleanup. The exact xrefs request is slot
  `0x1600fe8`; operation result and saved artifact hashes are pending. No second provider operation
  may start until this one returns and root authorizes it.
- Offline bytes still indicate one candidate thunk `0xf913f0` and five raw CALL candidates. They
  remain unvalidated; no static path or runtime/semantic claim is made. Task remains ACTIVE with
  zero failed targeted tries while the provider request is in progress.

## Gates and exact next action

- The three Graft/static probes establish only bounded source-navigation and byte observations.
  No new E-ID was created and `docs/EVIDENCE.md` was not changed. `AUD-02` remains partial; G1 is
  NOT PASSED and G2 is NOT RUN. No executable/core, user game input, reference pin, or runtime was
  changed or launched.
- Exact next action: wait for the single RackCreate xrefs request to return; root replay its saved
  output, then decide whether the changed 8G/1200-second provider plan is suitable for one RackQuery
  request. Keep VoiceHandle paused until root can reuse a documented, live Ghidra analysis or
  authorize a further changed provider plan. Do not overlap REA/Ghidra calls.
- The immediately preceding offline-prep checkpoint was committed as `e3042f9` with
  `git commit -m "docs: checkpoint NGS2 probe offline prep"` (exit 0), then pushed by
  `git push origin main` (exit 0; remote advanced `89233ef..e3042f9`). Before this checkpoint's
  edits, root observed only the pre-existing untracked `tools/graft/`; `HEAD` and `origin/main`
  matched at `e3042f9`.
- `python3 scripts/project_index.py` exited 0 (`4151 folders indexed; written`); log
  `local/checkpoints/2026-10-20-root-aud02-next-three-replay-project-index.log`, SHA-256
  `1ef16ff957f38b1921038ec99e42cb388b0c2167d3ba20167197e14ff23652d7`. `graft check` exited 0;
  the wiring graph is in sync and the optional deep meaning layer remains unbuilt (1,225 nodes
  pending). Log `local/checkpoints/2026-10-20-root-aud02-next-three-replay-graft-check.log`,
  SHA-256 `f6496628c6608d20c699a233b45e90bafc20f298616a0926f2bf4d2b576a8fdf`.
- `python3 scripts/project_index.py` ran again after adding this checkpoint; exit 0 with 4,151
  folders indexed. `graft check` exit 0 found the wiring graph in sync; the deep meaning layer
  remains unbuilt (1,225 nodes pending). Exact log paths and hashes are recorded above.
- Pre-publication `git diff --check` and `git diff --cached --check` exit 0. The staged path audit
  `python3 -I scripts/audit_publish_paths.py` exit 0 accepts 4 staged documentation paths; the
  full audit `python3 -I scripts/audit_publish_paths.py --all` exit 0 accepts 607 paths. Logs are
  `local/checkpoints/2026-10-20-root-aud02-next-three-replay-{working-diff,cached-diff,staged-audit,full-audit}.log`.
  `git fetch origin` exit 0 and `git rev-list --left-right --count HEAD...origin/main` reports
  `0 0`; logs are `...-fetch.log` and `...-divergence.log`. The pre-existing untracked
  `tools/graft/` remains unstaged. Commit and push result will be recorded by the next checkpoint.
