# AUD-02 — next three NGS2 import-to-caller probe claims

- Time / timezone: 2026-10-20 02:51 CDT (America/Chicago).
- Agent / integrator: `/root`; root owns shared documentation, independent replay, and publication.
- Parent task and gate: `AUD-02`, Stage 3; gate G2 remains NOT RUN. All three bounded tasks are
  `ACTIVE`; each starts with zero failed targeted analysis/provider tries.
- Tasks claimed:
  - `AUD-02-NGS2-RACKCREATE-ALLOC-01`: `sceNgs2RackCreateWithAllocator`, symbol index 236,
    NID `U546k6orxQo`, import slot `0x1600fe8`.
  - `AUD-02-NGS2-RACKQUERY-BUFFERSIZE-01`: `sceNgs2RackQueryBufferSize`, symbol index 239,
    NID `0eFLVCfWVds`, import slot `0x1601000`.
  - `AUD-02-NGS2-RACK-VOICEHANDLE-01`: `sceNgs2RackGetVoiceHandle`, symbol index 238,
    NID `MwmHz8pAdAo`, import slot `0x1600ff8`.
- Prerequisites / immutable inputs: RE-02 import manifest, AUD-01 inventory, and
  `docs/REVERSE_ENGINEERING.md` Stage 3. `local/re/eboot.elf` SHA-256
  `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`; `local/re/imports.json`
  SHA-256 `78caaaae139b7c5d4d2618abab2e50e9f6918ea3cec73a90b81e0005c83b6744`.
- Claim hypothesis / bounded acceptance: independently recover one static import-slot → thunk →
  direct-caller path for each tuple against that exact ELF; preserve exact caller bytes, function
  attribution, available argument/dataflow observations, tool/Evidence IDs, and all unresolved
  pointers and indirect edges. Each worker must provide a checksum manifest and reproducible
  verifier. Root must replay both before a task can become complete. An import name, Ghidra xref,
  static caller, or nonzero byte sequence alone does not establish reachability, ABI, argument
  meaning, Sony command behavior, middleware runtime binding, or audio output.
- Private ownership: separate worker-only dossiers under
  `local/re/evidence/aud02-ngs2-rackcreate-allocator-01/`,
  `local/re/evidence/aud02-ngs2-rackquery-buffersize-01/`, and
  `local/re/evidence/aud02-ngs2-rack-voicehandle-01/`. Workers must not edit shared docs, source,
  pins, or Git state; root integrates only after replay. Use existing parser/probe workflow rather
  than inventing a provider or changing pinned reference trees. Never copy game code/evidence into
  public paths.
- Inventory observation: the manifest has 11 NGS2 import records. These three tuples were
  present at the stated symbol indices/slots; existing task/checkpoint search returned no claim
  for these task IDs or symbol names. Two manifest imports remain unclaimed for a later batch:
  `sceNgs2RackDestroy` (237 / `lCqD7oycmIM` / `0x1600ff0`) and
  `sceNgs2VoiceGetStateFlags` (245 / `rEh728kXk3w` / `0x1601030`).
- Exact manifest query from repository root (exit 0; five expected records printed):
  ```sh
  python3 -I -c 'import json; p=json.load(open("local/re/imports.json")); items=p if isinstance(p,list) else p.get("imports",[]); wanted={"sceNgs2RackCreateWithAllocator","sceNgs2RackDestroy","sceNgs2RackGetVoiceHandle","sceNgs2RackQueryBufferSize","sceNgs2VoiceGetStateFlags"};
  for x in items:
   if x.get("name") in wanted or x.get("symbol") in wanted or x.get("symbol_name") in wanted: print(json.dumps(x,sort_keys=True))'
  ```
  Claim search command `rg -n "RACKCREATE-ALLOC|RACKQUERY-BUFFERSIZE|RACK-VOICEHANDLE|sceNgs2RackCreateWithAllocator|sceNgs2RackQueryBufferSize|sceNgs2RackGetVoiceHandle" docs/TASKS.md docs/checkpoints` exited 1 with no matches, confirming no prior claims.
- No analysis or implementation has been run for the three newly claimed tasks. Do not infer a
  failed try from setup or query issues; record each distinct analysis hypothesis/change and its
  targeted check separately. At three unsuccessful targeted tries, stop and escalate under the
  repository's three-try rule.
- Shared files changed for this claim: `HANDOFF.md`, `docs/TASKS.md`, this checkpoint, and the
  generated folder index `docs/FOLDERS.md`. `docs/EVIDENCE.md` and `INSTALL_LOG.md` are unchanged;
  no source files or public game binaries changed.
- `python3 scripts/project_index.py` from repository root exited 0 and indexed 4,148 folders;
  `docs/FOLDERS.md` now records 49 checkpoint files. Log
  `local/checkpoints/2026-10-20-root-aud02-next-three-project-index.log`, SHA-256
  `9e7e2b6742a42ea454a0d23092e76314e2cf4a229c8e92231fedd41ce1da0ce8`.
- `graft check` from repository root exited 0: wiring graph is in sync; deep meaning layer remains
  unbuilt with 1,225 nodes pending. Log
  `local/checkpoints/2026-10-20-root-aud02-next-three-graft-check.log`, SHA-256
  `f6496628c6608d20c699a233b45e90bafc20f298616a0926f2bf4d2b576a8fdf`.
- `git diff --check` and `git diff --cached --check` exited 0 with empty logs. Logs
  `local/checkpoints/2026-10-20-root-aud02-next-three-working-diff-check.log` and
  `...-cached-diff-check.log` each have SHA-256
  `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`.
- `python3 -I scripts/audit_publish_paths.py` exited 0 and accepted four staged documentation
  paths; log `local/checkpoints/2026-10-20-root-aud02-next-three-staged-audit.log`, SHA-256
  `9598f28c6ec508eece1299177d418689656b00f2dfc8ce8b40d7bfef181bf4c6`. The `--all` audit exited
  0 and accepted 605 tracked paths; log
  `local/checkpoints/2026-10-20-root-aud02-next-three-full-audit.log`, SHA-256
  `7a9ef96ac3a56fca1f92583cd859dd1bd66af5caf15480a9c015b9ed820f3026`.
- `git fetch origin` exited 0; `git rev-list --left-right --count HEAD...origin/main` returned
  `0 0`. The staged set is exactly `HANDOFF.md`, `docs/FOLDERS.md`, `docs/TASKS.md`, and this
  checkpoint; `tools/graft/` remains untracked and unstaged. Commit and push details will be added
  before worker dispatch.
- Exact next action: dispatch the three private dossiers to separate `gpt-6-luna` agents at Extra
  High reasoning; wait for each bounded report; root-replay checksums and byte verifiers before
  updating any task status.
