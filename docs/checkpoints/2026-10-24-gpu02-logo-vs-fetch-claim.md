# GPU-02 pinned logo vertex fetch lowering claim

- Local time: 2026-10-24 16:40 CDT. Integrator: Codex `/root`.
- Task: `GPU-02-GCN-LOGO-VS-FETCH-01` — ACTIVE, attempt 0, zero failed semantic hypotheses.
- Starting revision: `73d485b8abd037a4bbde479c8ab73317c221d195` on
  `cursor/init-hash-551140-9dfc`.
- Input: existing private `local/fixtures/gpu-r30/logo-vs.bin`, 140 bytes,
  SHA-256 `430f9adbb07d387c0b2b877b98300396f183faeab7f75cba0c9b0c67ada20c0c`; existing pinned
  inventory reports 25 instructions, zero unknowns, exact end, and `per_lane_eligible=true`.
  MUBUF instructions at offsets `0x34` and `0x3c` are `buffer_load_format_xy` with 2 components;
  the instruction at `0x44` is `buffer_load_format_xyzw` with 4 components.
- Exact preflight replay: `python3 -I local/orchestrator/gpu02-logo-vs-fetch/claim_replay.py` — exit 0;
  log `local/orchestrator/gpu02-logo-vs-fetch/claim-replay.log`; payload-free result
  `local/orchestrator/gpu02-logo-vs-fetch/claim-replay.json`.
- Claim publication preflight: `python3 scripts/project_index.py` — 9,043 folders indexed;
  `python3 -I scripts/audit_publish_paths.py` — 6 staged paths passed;
  `python3 -I scripts/audit_publish_paths.py --all` — 2,752 paths passed;
  `python3 -I scripts/check_commit_attribution.py` — passed.
  Logs: `local/orchestrator/gpu02-logo-vs-fetch/claim-{staged-audit,all-audit,attribution}.log`.
- Claim published as `6e60c96046fbe87db8ea24c070b75c02564bf826`; the remote branch ref matched local
  HEAD, worktree was clean, and post-push attribution audit passed. Author/committer are Johnny Wang.
  Exact output: `local/orchestrator/gpu02-logo-vs-fetch/claim-push-verify.log`.
- Checkpoint-record refresh: staged path audit 2 paths, full path audit 2,752 paths, attribution audit
  passed; logs `local/orchestrator/gpu02-logo-vs-fetch/published-{staged-audit,all-audit,attribution}.log`.

## Bounded implementation

- Extend `tools/dev/gcn_to_msl.py` to accept explicit `explicit_float2`, `explicit_float3`, and
  `explicit_float4` stream ABI entries only when the MUBUF component count, descriptor identity,
  one-vertex-per-case index, and existing offset/flag contract agree. Write exactly the fetched number
  of destination VGPR cells. Preserve visible failure for missing, mismatched, or unsupported formats.
- Add public synthetic tests for XY, XYZ, XYZW and rejected width/ABI mismatches. Do not add game bytes,
  asset-specific instruction tables, descriptors, or generated MSL to `tools/`.
- Root creates a private explicit ABI using the pinned logo VS digest and synthetic stream fields,
  compares its vertex outputs with an independent formula-level CPU oracle, and compiles/executes the
  generated MSL through the existing Metal verifier. Private inputs, generated MSL, logs, and output
  stay under `local/orchestrator/gpu02-logo-vs-fetch/`.

## Excluded scope and acceptance

No logo pixel-stage conversion, WQM, MIMG, texture, real texture binding, `.cmsh`/NRM0, screenshot
seam, retail descriptor mapping, semantic attribute naming, or G3 promotion. This child can become
`FIXTURE_VERIFIED` only if the synthetic width/rejection tests pass, the exact private shader digest
lowers without unsupported instructions, generated MSL compiles and executes on Metal, and the vertex
outputs match a CPU oracle under the explicitly synthetic ABI. G3 remains PARTIAL.

## Claim checkpoint

- No source files or tests changed and no tests/builds were run at claim time.
- The first private report attempt returned exit 0 but displayed instruction code offsets as `0x0`
  because its JSON `offset` key collided with the MUBUF immediate field. The mistake was caught by
  inspection; the corrected replay now distinguishes `instruction_offset` and pins all three expected
  offsets. This was report formatting, not a semantic hypothesis; no implementation attempt is counted.
  Note: `local/orchestrator/gpu02-logo-vs-fetch/claim-replay-first-failure.txt`.
- Next exact action: implementation worker changes only `tools/dev/gcn_to_msl.py` and
  `tools/dev/test_gcn_to_msl.py`; root prepares and runs private shader replay/oracle/Metal verification,
  then independently reviews the worker diff.
