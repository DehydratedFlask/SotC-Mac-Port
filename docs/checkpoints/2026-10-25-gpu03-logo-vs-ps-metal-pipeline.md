# GPU-03 logo VS/PS Metal pipeline — task claim

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
