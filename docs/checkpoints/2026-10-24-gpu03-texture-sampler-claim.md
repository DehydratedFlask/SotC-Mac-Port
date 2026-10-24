# GPU-03 typed texture/sampler binding claim

- Local time: 2026-10-24 16:21 CDT. Integrator: Codex `/root`.
- Task: `GPU-03-RENDEROPS-TEXTURE-SAMPLER-01` — ACTIVE, attempt 0, zero failed implementation
  hypotheses.
- Starting revision: `e5981d81f7d9b6e14d93e2a31a25cdc420245c2d` on
  `cursor/init-hash-551140-9dfc`; preflight `git status --short --branch && git rev-parse HEAD` exited 0
  and showed a clean worktree.

## Bounded scope

Add typed host-handle `BindTexture` and `BindSampler` operations, plus a direct-Metal fixture that
samples one synthetic RGBA8 2D texture using an explicitly supplied sampler at fragment slot 0. Invalid
resource descriptors or a missing binding required by the explicit harness configuration must fail
visibly. This tests native host binding plumbing only; the generated MSL is synthetic for this fixture.

The user-selected default shader pair is already verified by E627; its synthetic renderer and parser
flows are E632/E633. This task does not add the logo shader to `gcn_to_msl.py`, prove equivalence between
AMD WQM and Metal helper/derivative behavior, read real game texture or mesh payloads, infer retail
descriptor values, attach a screenshot, or claim game-frame output. G3 remains PARTIAL.

## Inputs and current state

- `docs/METAL_FRAME_PATH.md` section 6.2 defines `BindTexture/Sampler` as native typed operations;
  GPU-03 task card 4 names the logo/PNG path as an optional later textured follow-up.
- `UnsupportedRenderOpKind` already has `bind_texture` and `bind_sampler`, but validation and the
  Metal executor reject those operations. `AssetMeshDraw` currently carries only position/index spans
  and the default shader harness case; no texture or sampler is bound by the executor.
- E634 provides static evidence for a single straight-line logo `image_sample` within a WQM region,
  but its texture, sampler, format, coordinates, and host binding ABI are still unknown.

## Claim checkpoint

- Files changed at claim: `docs/TASKS.md`, `docs/INDEX.md`, `docs/FOLDERS.md` (generated), `HANDOFF.md`,
  and this file. Production code/tests are not yet edited.
- Exact preflight command: `git status --short --branch && git rev-parse HEAD` — exit 0, clean active
  branch at the starting revision above. Trail lookup plus caller and exhaustive-use scans identified
  the renderer API, typed op layer, executor, and focused test files. No test/build command was run at
  claim time; no separate private log exists.
- Observation: typed texture/sampler op names exist only in the unsupported enum; current API binds
  vertex buffers and fragment constants. Inference: one explicit RGBA8 host texture and sampler can
  validate the next ordinary Metal resource-binding seam, but does not establish logo shader semantics.
- Remaining risks: actual logo texture provenance, format/tiling, filtering/wrapping/mips, vertex inputs,
  AMD/Metal derivative equivalence, and retail descriptor ABI are unresolved. The parent GPU-03 raw
  asset, NRM0 and screenshot decisions are unaffected.
- Next exact action: implement and fixture-test fragment slot-0 RGBA8 texture/sampler binding, then run
  the required verification and publication checks. If a semantic or API hypothesis fails, preserve the
  exact attempt and follow the three-try escalation rule.
