# GPU-03 logo VS/PS Metal pipeline — task claim

## 2026-10-25 09:56 CDT — focused implementation and path-safety checkpoint

- Integrator: Codex `/root`; task remains **ACTIVE**, attempt 2. Independent source review found that
  MSL/report output could alias an ABI or shader input under `local/`. The CLI now canonicalizes all
  five paths, requires each to remain under canonical workspace `local/`, and rejects any collision
  before input reads or output writes. A CLI regression test covers output aliasing the VS input.
- Implementation: `tools/dev/gcn_to_msl.py` composes E636's actual 25-instruction vertex stage with
  E646's actual 34-instruction fragment stage, sharing `ShaderCase`/`RasterValue` and keeping the
  standard `asset_vertex`/`asset_fragment` entrypoints. Exact hashes remain
  `430f9adbb07d387c0b2b877b98300396f183faeab7f75cba0c9b0c67ada20c0c` and
  `4309caada7f114ec822f61646be536f50a8261a8c6e54f2ce709db7c70b8b39a`. The generated MSL is 4,094 B,
  SHA-256 `37e09aef0838dff77c8f151f288ed8cb2cdf7db26e202b40bb9f6f27a34dbd56`.
- Failed check and correction: first collision-regression run reported one Python assertion failure
  because its expected diagnostic ordered the two colliding labels differently; the guard did reject
  the alias before I/O. Input paths are now checked before outputs, so the diagnostic identifies the
  output collision. Preserved first-run log is
  `local/validation/gpu03-logo-vs-ps-pipeline-20261025/attempt2-python-lowering-tests-first-run.log`
  (SHA-256 `7a34cc5da7e78da5651772b2aeae94c484f14341cc229b2569f8c460448c7918`). Corrected
  `python3 -I -W error tools/dev/test_gcn_to_msl.py` passed **23/23**; log SHA-256
  `0b487dd197331c02512202319dd6e3bcc9b246f43271f82cc2097443075db78f`.
- Focused Metal test command used the three private `SOTC_LOGO_PAIR_*` paths below `local/`; direct
  CTest exited 0, **1/1**, with Metal API and GPU validation enabled. Existing assertions check the
  selected pixel against an independent nearest/clamp fp16-RTZ oracle, repeat output, texel mutation,
  and position mutation. CTest log SHA-256 `eccd4978abc7b6cbbb6220ee2742b77bd64bad0e6bc1c97cdaf968a3145bfa1f`.
  Baseline PNG SHA-256 `db954a90001319db7954ac569c3ceffed9143de27ef3eda2fda37f4a24efac02`; mutated PNG
  SHA-256 `74ee455f6c7bce82b70e2ff65e54dfe27a8c5de51ef392d4df469b43796be694`.
- Attempt 1 remains recorded separately: the first Metal oracle pass used an incorrect Y flip and
  probed the wrong quad corner; correction followed E636's `clip.y = 1 - 2*position.y` and Metal's
  y-down readback. This was an oracle/harness correction, not shader semantic evidence.
- Observation: pair compilation and focused Metal execution work with the declared synthetic ABI.
  Inference: position-correlated UV is sufficient for this fixture. Retail UV/TEX0/NRM0 binding,
  WQM/helper-lane equivalence, game-driven frame, boot/audio/save gates, and G3 remain unverified.
- Changed source files: `tools/dev/gcn_to_msl.py`, `tools/dev/test_gcn_to_msl.py`, and
  `native/tests/test_metal_texture_binding.cpp`; documentation records E667. No dependencies were
  installed. `python3 scripts/project_index.py` indexed **19,593 folders**; `trail build` rebuilt
  **8,560 nodes / 11,611 edges / 1,497 cards** across 2,002 files and `trail build --check` passed
  (deep meaning layer not built). Exact next action: run full CMake configure/build/CTest and lift
  checkers, then staged/all-path and attribution audits; publish only allowlisted files on `main`
  under the owner identity.
- Focused gate status: **PASSED**. Full repository gates: **NOT RUN**. Task status: **ACTIVE**, attempt 2.

## 2026-10-25 09:58 CDT — focused evidence checkpoint published

- Documentation checkpoint commit `c03d6a51ae7df9a732180495f8e298613eef6b2e` was pushed to GitHub
  `main`; fetch confirmed `HEAD == origin/main`. Commit author/committer are Johnny Wang; no
  Claude/Anthropic/co-author trailer. Staged/all path audits passed 6/2,795; attribution passed.
- `main` is checked out; the previous `cursor/init-hash-551140-9dfc` branch is absent locally and on
  `origin`; no open PRs remain. The source changes are still local/uncommitted pending full gates.
- Exact next action: run full repository CMake configure/build/CTest (with logo-pair opt-in paths),
  lift checkers, and final path/attribution audits. This publication is documentation only; E667 is
  the focused local implementation evidence, and G3 remains PARTIAL.

## 2026-10-25 09:40 CDT — claimed on `main`

- Integrator: Codex `/root`; task `GPU-03-LOGO-VS-PS-METAL-PIPELINE-01` is **ACTIVE**, attempt 0.
- Files changed: `HANDOFF.md`, `docs/TASKS.md`, `docs/METAL_FRAME_PATH.md`, `docs/EVIDENCE.md`,
  `docs/INDEX.md`, `docs/FOLDERS.md`, and this checkpoint. No implementation files changed.
- Bounded output: a closed-world `logo-pair` profile composing E636's 140-byte logo VS and E646's
  152-byte logo PS, then exercising them through direct Metal on a synthetic indexed quad and an
  asymmetric RGBA8 slot-0 texture. Exact shader digests are recorded in E636/E646 and E664.
- Chosen ABI: `logo_clip` and `logo_param1` both read the per-vertex position stream; `logo_param0`
  reads a unit draw-level object row. The synthetic UV therefore follows position by construction.
  This is fixture-only and does not infer retail descriptors or TEX0 semantics.
- Observation: `graft grep "AssetMeshDraw" --in native/` found its three-file API use surface;
  `trail callers AssetMeshDraw --depth all` had no indexed incoming edges. The independent source
  review confirmed the E636/E646 stage interface and that this ABI alias does not require a renderer
  API change.
- Commands/results: `git status --short --branch` was clean on `main` at
  `d85e70018737a9351176ce21e437820c6b08cd35` before the claim; the Trail query reported no call edges.
  `git diff --check` passed; project index passed at 19,592 folders; CMake configure/build passed
  (no work); full CTest passed 397/397; lift checkers passed 481/0/0; Trail wiring check passed at
  8,546 nodes. Logs are under `local/validation/gpu03-logo-vs-ps-pipeline-20261025/`. Staged/all
  publish-path audits passed 8/2,795; attribution audit passed for `origin/main..HEAD` and the
  configured owner identity. These gates establish no game/runtime behavior.
- Observation versus inference: E636/E646 inputs and their standalone fixture evidence are pinned;
  the stages structurally share `param0` and `param1`. A position-correlated UV alias is a synthetic
  route to sampled pixels, not a claim about game input streams.
- Remaining risks: the profile and Metal pipeline are not implemented; CPU interpolation/oracle,
  deterministic repeat, mutation, and fail-closed rejection remain unchecked. TEX0/NRM0, retail
  binding, WQM/helper-lane equivalence, game-driven rendering and G3 remain unverified.
- Exact next action: implement the hash/ABI-gated `logo-pair` generator and add the opt-in Metal
  quad/texture oracle test. Then run focused checks followed by every publication gate in `AGENTS.md`.
- Status: **ACTIVE**, attempt 0, zero failed hypotheses.
- Publication: commit `478163c32c2f7f4b099d46189f17fb468703d3c7` is on GitHub `main`; post-push fetch
  confirmed matching `origin/main`, clean `main`, and no open PRs. The staged/all path audits passed
  8/2,795 and owner-only attribution passed. See E666 for full gates. This publication adds no
  implementation or runtime evidence.
