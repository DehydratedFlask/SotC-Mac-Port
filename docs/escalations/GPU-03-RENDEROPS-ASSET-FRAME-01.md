# NEEDS_PRINCIPAL: GPU-03-RENDEROPS-ASSET-FRAME-01 — raw mesh index mutation is not visible in the frame

- Owner / principal / date: Codex integrator `/root`; principal reassessment required; 2026-10-25.
- Requirement and failing gate: after importing and parsing the pinned `cube_1m.cmsh` and `cone.cmsh`,
  render both actual mesh position/index streams with the default vertex/pixel MSL pair and explicit
  harness normal `[0.5, 0.5, 1.0]`, save a private PNG, and prove that a vertex, an index, and a shader
  constant mutation each change its hash. The parser, baseline render, silhouette and depth checks pass;
  the index mutation gate remains failed. Do not weaken or omit it. No game-driven frame or G3 PASS is
  claimed.
- Input SHA256, source pins, original RVA / file offset / image base: `cube_1m.cmsh` SHA-256
  `44738dbf663af252898bad5582f2b48f1aadf6fbaab403c20880410b79a380b7`, exact 850 B; `cone.cmsh` SHA-256
  `5456afadabe96b9f7f02c503dc0ed5410652ccd32601c19e8aeeaaf36c88600c`, exact 1,462 B (E629/E639).
  Default-pair MSL SHA-256 `5524537e36b2481798abe5bf6c311e37082aed26b136c4590f7d685e671a455e`. The native
  parser/test harness is source code; this probe does not execute the original ELF or use an RVA.
- Reproducer / expected result: build `test_metal_asset_frame`, then run the command below. Expected:
  exit 0, a deterministic asset-derived PNG, correct CPU-projected silhouette/depth, and different PNG
  hashes for vertex/index/constant mutations. Actual on try 3: exit 1; vertex and shader-constant
  mutations pass, index mutation fails because the bounded CPU-visible search found zero candidates.

```sh
env MTL_DEBUG_LAYER=1 MTL_SHADER_VALIDATION=1 MTL_SHADER_VALIDATION_DEFAULT_STATE=all \
  MTL_SHADER_VALIDATION_ENABLE_ERROR_REPORTING=1 MTL_SHADER_VALIDATION_REPORT_TO_STDERR=1 \
  MTL_SHADER_VALIDATION_ABORT_ON_FAULT=1 \
  SOTC_DEFAULT_PAIR_MSL=local/validation/gcn-default-root-v1/default-pair.metal \
  local/build/orch-r22/test_metal_asset_frame --asset-derived \
  local/orchestrator/gpu03-r31/asset-derived/cube_1m.cmsh \
  local/orchestrator/gpu03-r31/asset-derived/cone.cmsh \
  local/validation/gpu03-r31/asset-derived-cube-cone.png
```

- Current tree changes and recovery location: root branch `codex/merge-all-open-changes`, root test
  and escalation checkpoint commit `01e6019ed8fa24acb6c8fdebf0ffe30585eaec43`; latest `origin/main`
  is `a1f21e62fe25294ac70d6753be7ab18322d7a169`, and its merge into the integration branch is in
  progress. The test source is committed in `native/tests/test_metal_asset_frame.cpp`; task records
  are in `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, and this escalation. Raw inputs,
  executable, MSL, PNG and logs stay under ignored `local/`. Modified test source SHA-256 is
  `c7d760ac64311d05f811f01b172a2126fb57a7b9d35fba3a7675ebc247ed3f36`.

| Failed try | Distinct hypothesis/change | Exact command / environment | Actual result | Log/evidence |
|---|---|---|---|---|
| 1 | Replacing one of the first 12 cone index slots with each of the next four in-range vertex indices would alter rasterized pixels. | Reproducer command above; build: `cmake --build local/build/orch-r22 --target test_metal_asset_frame -j 12`. | Exit 1; only `asset index mutation changes PNG hash` failed. Baseline PNG hash `af42ca554f5d24a9f981fc92156dea50616c172af1faba12eef1ff0d4db0f645`; silhouette/depth checks passed. | Handoff 2026-10-25 02:31. The shared `asset-derived-frame.log` was overwritten by later attempts; the original command result is preserved in the handoff text, not as a retained log. |
| 2 | Target one of the eight cone triangles with the most CPU-visible pixels and replace each candidate corner with one of four farthest projected vertices. | Same build and reproducer command above, with `MTL_DEBUG_LAYER=1` and shader validation enabled. | Exit 1; index PNG hash remained unchanged; baseline silhouette, depth and overlap checks passed. Cause unknown. | Handoff 2026-10-25 02:33. The shared log was subsequently overwritten by try 3; no individual log was retained. |
| 3 | Extend the search to both draws; rank triangles by visible pixels, choose distant in-range vertices, and render only candidates that change the CPU front-draw mask. | Same build and reproducer command above, with `MTL_DEBUG_LAYER=1` and shader validation enabled. | Exit 1; `asset index mutation search: CPU-visible candidates=0`; other asset-frame checks passed. | `local/validation/gpu03-r31/asset-derived-frame.log`; `local/validation/gpu03-r31/asset-derived-cube-cone.png`; Handoff 2026-10-25 02:34. |

- Smallest failing fixture and how to run it: the two pinned `.cmsh` inputs above plus the private
  default-pair MSL. The bounded case is the actual cone U16 index array (42 parsed positions, 114
  indices) overlaid with the actual cube mesh (24 positions, 36 indices). The command above is the
  complete reproducer; no synthetic fallback is used in `--asset-derived` mode.
- Verified facts versus guesses: verified, both asset byte hashes/sizes; parser accepts each POS0/U16
  stream and reports the listed counts; baseline PNG is 256x256 RGBA and 3,147 PNG bytes; hash is
  `af42ca554f5d24a9f981fc92156dea50616c172af1faba12eef1ff0d4db0f645`; CPU silhouette errors 0,
  depth-class errors 0, projected overlap 6,584 pixels; Metal API/GPU validation enabled with no fault
  reported; vertex and shader-constant mutation assertions pass; index assertion fails. It is only a
  guess that ties, occlusion, projection quantization, or a wrong but self-consistent parser/index
  interpretation explains the missing visible mutation.
- Why the previous approaches failed / missing information: no current observation distinguishes
  raster invariance from a mismatch between the POS0/U16 fixture interpretation and actual draw
  semantics. The current independent CPU projection agrees with the baseline frame but has not yet
  produced an index mutation candidate that changes its front-draw mask. Do not infer that all
  indices are semantically valid from baseline agreement alone.
- Independent work that can continue: finish reconciling the integration branch, run its publication
  gates, publish and merge its PR, then return the workspace to `main`. The already verified private
  parser and baseline frame may be used as evidence; the GPU-03 task itself must stop here.
- Task register and HANDOFF checkpoint links: [TASKS.md](../TASKS.md), [HANDOFF.md](../../HANDOFF.md),
  and [raw member import checkpoint](../checkpoints/2026-10-25-gpu03-raw-cmsh-import.md).
- Principal reassessment / authorized next approach: no fourth try is authorized. The principal must
  record a new hypothesis, targeted observable, and recovery plan here before resuming this task.

Stop after try 3. Do not reset counts in another agent/session, retry by rewording, substitute
emulation/silence or fabricate success. The principal must record a new assessment before try 4.
