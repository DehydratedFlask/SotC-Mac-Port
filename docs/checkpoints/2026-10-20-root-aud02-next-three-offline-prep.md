# AUD-02 — three bounded NGS2 probe offline-prep checkpoint

- Time / timezone: 2026-10-20 02:59 CDT (America/Chicago).
- Agent / integrator: `/root`; root owns shared docs, provider-slot coordination, and final replay.
- Parent: `AUD-02`; G2 remains NOT RUN. All three bounded tasks remain ACTIVE; failed targeted
  analysis/provider tries are 0 for each. No caller edge is accepted by this checkpoint.
- Inputs: pinned `local/re/eboot.elf` SHA-256
  `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3` and
  `local/re/imports.json` SHA-256
  `78caaaae139b7c5d4d2618abab2e50e9f6918ea3cec73a90b81e0005c83b6744`.

## Offline preparation

- `AUD-02-NGS2-RACKCREATE-ALLOC-01`: worker `/root/aud02_rackcreate_allocator` recorded its
  offline subtask in `local/re/evidence/aud02-ngs2-rackcreate-allocator-01/README.md`. Its captured
  scanner command was `python3 -I local/re/evidence/aud02-ngs2-rackcreate-allocator-01/capture.py
  scan-import -- python3 -I
  local/re/evidence/aud02-ngs2-rackcreate-allocator-01/scan_import.py`, exit 0. The scan reports a
  candidate thunk at `0xf913f0` (`ff25f2fb6600`) → slot `0x1600fe8` and five raw `E8` patterns at
  `0x9493f8`, `0x9494a8`, `0x949581`, `0x94965f`, and `0x99b4eb`; these remain unvalidated
  candidates. Worker scan stdout SHA-256
  `ffb3ba1eb3d823f54e6d6330960a04990b47be35328f7b2201823c95141610ef`; initial 32-file
  `SHA256SUMS.txt` SHA-256
  `28642c5592171290e899b3fd7b3536717040acc354a12d5b6f1e72de60c0724d`. Root independently ran
  `shasum -a 256 -c SHA256SUMS.txt` from that dossier directory, exit 0; root log
  `local/re/evidence/aud02-ngs2-rackcreate-allocator-01-root-prep-checksums.log`, SHA-256
  `e8f7d5feaf81b5f57ffeba0d4e2a7230240e8124108117164a699d40e4335f39`.
- `AUD-02-NGS2-RACKQUERY-BUFFERSIZE-01`: worker `/root/aud02_rackquery_buffersize` recorded its
  prep at `local/re/evidence/aud02-ngs2-rackquery-buffersize-01/checkpoint-prep.md` (current SHA-256
  `82fb9e1603b6e89dd8c78db1aec2673b59c88426723b3114bb1dc6923a7154f0`). From repository-root CWD,
  `python3 -I local/re/evidence/aud02-ngs2-rackquery-buffersize-01/offline_scan.py` exited 0. It
  reports one `FF 25` candidate `0xf91420` → slot `0x1601000` and 15 raw `E8` candidates, from
  `0x94ebc2` through `0x9502ab` plus `0x99fb7b`. Worker verifier currently returns
  `PARTIAL_PRE_REA` and checks the pinned executable segments, tuple, thunk, and candidates; it
  does not accept a call path. Verifier SHA-256
  `5b0213b2e23eef075b1793fd2acbedfafee83ac5e609502a2aeeb6791cb5a19b`; private REA snapshot SHA-256
  `32f2c278247c85bdccaeb51f4e9d4bd8d7234eca00a78abc225b957f413af6ed`. Root independently
  rehashed the checkpoint, snapshot, verifier, scan JSON and stdout; root record
  `local/re/evidence/aud02-ngs2-rackquery-buffersize-01-root-prep-hashes.log`, SHA-256
  `2dbde2559f60e8b2a3631ac78aa0ca6650fea8f619c3f2ed5dbb2a2ffe940493`.
- `AUD-02-NGS2-RACK-VOICEHANDLE-01`: worker `/root/aud02_rack_voicehandle` checkpoint
  `local/re/evidence/aud02-ngs2-rack-voicehandle-01/offline-prep-checkpoint.md`, SHA-256
  `a8184ef981b33274e5d3bf012f812d0e2abdf281a0228484ae8bfac39706d899`; captured command list
  `offline-prep-commands.txt`, SHA-256
  `839e9640076fbd78863915cf279a4e93e29066998a8d50b3750401da0b999754`. The five offline commands
  exited 0; one `FF 25` candidate at `0xf91410` (`ff25e2fb6600`) maps to slot `0x1600ff8`; 104 raw
  `E8` patterns target it, all candidate-only. Worker verifier/scanner outputs SHA-256
  `13f59bd0823cf56e63f3b0aefcf6637fafeeac913925bf0a74dabf5ae07ab3e0`, enriched parser JSON
  `3c24374a548b92fcad98920274e10261e47decf790ea7049d87061a745971ced`, and call scan stdout
  `d291474d61b3e990ea1802c5adea33ea18885e6f637a317a343399a5a52c8775`. Root independently
  rehashed all five artifacts; root log
  `local/re/evidence/aud02-ngs2-rack-voicehandle-01-root-prep-hashes.log`, SHA-256
  `c638d2da2a0fa5dd6c7ef9300c59ac099e268f2b19e52922c9feacb3578af596`.

## Provider coordination and limits

- Because workers reported REA/Ghidra may share a persistent binary-analysis session, root
  serialized provider calls. `/root/aud02_rack_voicehandle` owns the first slot; `open_binary`
  succeeded, and `xrefs` at slot `0x1600ff8` is currently running. RackCreateWithAllocator and
  RackQueryBufferSize are holding while they finish offline preparation. No overlap is authorized.
- Root briefly attempted a `shasum` command with a zsh glob for a nonexistent `*e8*` filename; it
  exited 1 before hashing. The corrected explicit-path hash command exited 0. This was bookkeeping,
  not a targeted analysis/provider failure; no file was changed by the failed command.
- All scan results above are observations about offline bytes and parser output only. They do not
  establish instruction boundaries for raw patterns, caller/function edges, reachability, ABI,
  argument meaning, runtime binding, NGS2 contract, or audio behavior. No G1/G2/G5 gate changed.
- Shared status is updated in `HANDOFF.md` and `docs/TASKS.md`; no `docs/EVIDENCE.md`, source,
  install, or pinned-reference files changed. Private evidence remains under `local/re/evidence/`.
- `python3 scripts/project_index.py` exited 0 and indexed 4,151 folders; `docs/FOLDERS.md` now lists
  50 checkpoint files. Log `local/checkpoints/2026-10-20-root-aud02-next-three-offline-project-index.log`,
  SHA-256 `1ef16ff957f38b1921038ec99e42cb388b0c2167d3ba20167197e14ff23652d7`. `graft check` exited
  0 with the wiring graph current; its deep layer remains unbuilt with 1,225 nodes pending. Log
  `local/checkpoints/2026-10-20-root-aud02-next-three-offline-graft-check.log`, SHA-256
  `f6496628c6608d20c699a233b45e90bafc20f298616a0926f2bf4d2b576a8fdf`.
- Whitespace and publish-path checks, remote divergence, commit, and push for this shared checkpoint
  are recorded here. `git diff --check` and `git diff --cached --check` exited 0; their logs are
  `local/checkpoints/2026-10-20-root-aud02-next-three-offline-working-diff.log` and
  `...-cached-diff.log`, each SHA-256
  `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`. The staged publish-path
  audit exited 0 for four paths (`HANDOFF.md`, `docs/FOLDERS.md`, `docs/TASKS.md`, and this
  checkpoint); log `local/checkpoints/2026-10-20-root-aud02-next-three-offline-staged-audit.log`,
  SHA-256 `dc580a3e919b369b7c4a90b9b10f5a985ff4386b9921a2df1d04117096e069cb`. The full audit
  exited 0 for 606 allowlisted paths; log `local/checkpoints/2026-10-20-root-aud02-next-three-offline-full-audit.log`,
  SHA-256 `1b0bd14df3fff7214f55a8f17f82adf3609a428627ec54035418bb48c4c8071c`. `git fetch origin`
  exited 0 and `git rev-list --left-right --count HEAD...origin/main` reported `0 0` before
  publication. The existing `tools/graft/` cache remains unstaged.
- Exact next action: finish the VoiceHandle slot-xrefs query, capture its REA evidence, and release
  the session; then grant the same slot serially to RackCreateWithAllocator and RackQueryBufferSize.
  Root replays final dossiers before marking any path verified.
