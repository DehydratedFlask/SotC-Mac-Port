# Checkpoint: GPU-03-CMSH-SYNTHETIC-METAL-FLOW-01 — fixture verified

- Time / timezone: 2026-10-24 16:07 CDT.
- Integrator: Codex `/root`; task `GPU-03-CMSH-SYNTHETIC-METAL-FLOW-01` completed at attempt 1,
  with 0 failed implementation hypotheses. Parent `GPU-03-RENDEROPS-ASSET-FRAME-01` remains ACTIVE.
- Gate status: this subtask is `FIXTURE_VERIFIED`. G3 is `PARTIAL` under the synthetic-scene rule,
  not PASSED. The real asset-derived frame, retail bindings, game frame, boot, audio, and save/load
  remain unverified or NOT RUN.
- Prerequisites: E630 bounds-checked POS0/U16 parser, E632 direct-Metal synthetic indexed/depth
  renderer, and E627 default-pair MSL SHA-256
  `5524537e36b2481798abe5bf6c311e37082aed26b136c4590f7d685e671a455e`.
- Changed source: `native/tests/test_metal_asset_frame.cpp`. The test serializes a 178-byte synthetic
  raw POS0 float3/U16 triangle-list member in memory, parses it, checks positions/indices against
  their inputs, and passes the live `CmshMesh` vectors as `AssetMeshDraw` spans. It renders that draw
  and an equivalent hand-built draw with the pinned default-pair MSL, then requires exact RGBA and
  PNG equality. The existing harness `DefaultShaderCase` supplies the explicit synthetic normal;
  NRM0 is not decoded.
- Exact focused commands, working directory `/Volumes/Extra/SoC Mac Native`:
  - `cmake --build local/build/orch-r22 --target test_metal_asset_frame -j 12` — exit 0.
  - `ctest --test-dir local/build/orch-r22 -V -R '^metal_asset_frame$'` — exit 0, 1/1.
- Focused Metal result: API and GPU validation both logged enabled. No error/fault diagnostic appears.
  The parser-produced draw emitted a 5,999-byte PNG with SHA-256
  `34fc0b4366dfe97c46a61ca659a6b19bd55c54992d56765227533f25094c72e2`; output matched the direct
  draw byte-for-byte. The pre-existing two-draw fixture still emitted its 6,395-byte deterministic
  PNG with SHA-256 `35039e0fe8a9700ccfbe8bb66181426c16ea41849677633cea81ac99c0d21a94`, CPU coverage
  33,856, zero y-down silhouette and per-pixel draw/depth errors, and all three input mutations
  changed the output hash.
- Exact full verification commands, all exit 0:
  - `cmake -S native -B local/build/orch-r22`
  - `cmake --build local/build/orch-r22 -j 12`
  - `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure` — 391/391.
  - `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` — 476 passed, 0 skipped,
    0 failed.
  - `python3 -I scripts/audit_publish_paths.py --all` — 2,746 paths passed.
  - `python3 -I scripts/check_commit_attribution.py` — passed.
  - `python3 scripts/project_index.py` — 9,039 folders indexed.
  - `trail build --check` — wiring graph in sync, 8,295 nodes. The optional deep meaning layer is
    unbuilt. Trail 0.23.0 was reused; 0.25.0 availability notice was not acted on.
  - `git diff --check` — passed.
- Private logs: `local/orchestrator/gpu03-r31/synthetic-parser-metal/`; existing PNG artifact:
  `local/validation/gpu03-r31/synthetic-asset-frame.png`. No real asset bytes were opened or passed
  to the parser. The generated 178-byte fixture and parsed-result PNG hash were held in memory; no
  game data or generated fixture output is committed.
- Review: read-only independent review of the source diff found no actionable findings. The reviewer
  did not read game assets or run tests.
- Observation versus inference: the parser and direct-Metal renderer interoperate for the synthetic
  bounded subset. This does not establish that retail `.cmsh` data uses the tested POS0 format or that
  the chosen normal, interpolation, vertex fetch, resource bindings, or screenshot seam matches the
  game.
- Owner choices still pending at this checkpoint: whether real local raw `.cmsh` bytes may be passed
  through the parser/renderer; NRM0 versus harness normal handling for an asset-derived frame; direct
  PNG versus `attach_screenshot` frame sequence 0. E629 remains the only real-mesh evidence: hashes
  and counts from the read-only probe.
- Ruling: keep this integration fixture synthetic until the owner answers the real-byte question.
  Cost if wrong: another fixture-only step before the desired asset-derived frame.
- Failed tool wrapper: one `trail build --check` capture command used zsh's read-only `status`
  variable and exited 1 before running Trail; the direct command was rerun and passed. No product
  change or implementation hypothesis was involved.
- Publication: the docs-only task claim was pushed in owner-authored commit
  `3b65899826c4126cc85fbb3b9ae8bd58b3c56e8d`. The completion implementation/documentation commit
  SHA and remote verification are recorded in HANDOFF after publication. No Claude/Anthropic
  contributor trailer is used.
- Next exact action: continue with the user-approved GPU-03 decisions when they arrive. Until then,
  keep real mesh parse/render, NRM0 decoding, and screenshot attachment unclaimed.
