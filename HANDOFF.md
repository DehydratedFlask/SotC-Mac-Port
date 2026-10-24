# SoC Mac Native — current handoff

## 2026-10-24 16:09 CDT — GPU-03 synthetic parser-to-Metal flow verified

- Integrator: Codex `/root`; active branch `cursor/init-hash-551140-9dfc`. Task
  `GPU-03-CMSH-SYNTHETIC-METAL-FLOW-01` is `FIXTURE_VERIFIED` at attempt 1, with 0 failed
  implementation hypotheses. Parent `GPU-03-RENDEROPS-ASSET-FRAME-01` remains ACTIVE.
- Source changed: `native/tests/test_metal_asset_frame.cpp`; added an in-memory 178-byte synthetic
  POS0 float3/U16 triangle-list serializer, parser round-trip assertions, and an RGBA/PNG equality
  check between the parser-owned mesh and the equivalent hand-built draw. No production API or
  renderer behavior changed.
- Exact focused commands and results: `cmake --build local/build/orch-r22 --target
  test_metal_asset_frame -j 12` exit 0; `ctest --test-dir local/build/orch-r22 -V -R
  '^metal_asset_frame$'` exit 0, 1/1. Metal API/GPU validation both logged enabled, with no
  error/fault diagnostics. Parser-produced PNG is 5,999 B, SHA-256
  `34fc0b4366dfe97c46a61ca659a6b19bd55c54992d56765227533f25094c72e2`; parsed output equals the
  direct draw byte-for-byte.
- Full required gates all exited 0: configure; full build; CTest 391/391; lift checkers 476 passed,
  0 skipped, 0 failed; staged publish-path audit 9 paths; full publish-path audit 2,747 paths;
  commit-attribution audit; project index 9,039 folders; `git diff --check`; and `trail build --check`
  with 8,295 wiring nodes. Full logs are
  under `local/orchestrator/gpu03-r31/synthetic-parser-metal/`. A reviewer checked the source diff
  read-only and found no actionable issues.
- One log-capture wrapper exited 1 because zsh's `status` variable is read-only; direct `trail build
  --check` succeeded. This was a command-wrapper correction, not a product failure or failed
  implementation hypothesis.
- Observation versus inference: synthetic parser output feeds the current direct-Metal API and
  matches a direct draw; retail POS0 format, NRM0, resource ABI, frame convention, game rendering,
  and game acceptance remain unverified. Generated fixture bytes and its PNG hash stayed in memory;
  logs and all build/PNG artifacts are private under `local/`.
- G3 is now `PARTIAL`, following the explicit synthetic-scene rule in `docs/VALIDATION.md`; it is
  not PASSED. E629 remains the only real-mesh evidence (probe hashes/counts). Real archive bytes,
  NRM0 handling, and direct PNG versus `attach_screenshot` are awaiting the user's response.
- Updated `docs/TASKS.md`, `docs/EVIDENCE.md` (E633), `docs/VALIDATION.md`, `docs/METAL_FRAME_PATH.md`,
  `docs/INDEX.md`, `docs/FOLDERS.md`, this HANDOFF, and new checkpoint
  `docs/checkpoints/2026-10-24-gpu03-synthetic-parser-metal-flow.md`. After adding the checkpoint,
  `python3 scripts/project_index.py` exited 0 and indexed 9,039 folders.
- Publication: implementation/evidence commit `400152ad6de95f4147ee5dbf1e6d6dbb3194fd30` was pushed
  to `origin/cursor/init-hash-551140-9dfc`. `git ls-remote`, local HEAD and tracking ref all matched
  that SHA; worktree is clean. Author and committer are Johnny Wang
  `<dehydratedflask@users.noreply.github.com>`; attribution audit passed and the commit has no
  Claude/Anthropic co-author trailer.
- Next exact action: characterize the logo pixel shader's whole-quad lane behavior in the separately
  claimed task below, using only E625's pinned private shader spans and authoritative ISA evidence.
  Real raw `.cmsh` permission, NRM0 handling, and screenshot seam choices remain pending.
- Status: child task `FIXTURE_VERIFIED`; parent GPU-03 `ACTIVE`; no game-frame claim.

## 2026-10-24 16:10 CDT — GPU-02 logo WQM analysis task claimed

- Integrator: Codex `/root`; active branch `cursor/init-hash-551140-9dfc`, clean at claim time at
  `400152ad6de95f4147ee5dbf1e6d6dbb3194fd30`. Task:
  `GPU-02-GCN-LOGO-WQM-ANALYSIS-01`, attempt 0, 0 failed hypotheses.
- User order: the default material pair was selected and is now fixture-verified (E627); the next
  shader-only step characterizes the deferred logo pixel shader's whole-quad behavior before any
  logo MSL lowering.
- Bounded output: byte-bound analysis of the pinned E625 logo pixel program's execution-mask/WQM
  controls and texture-sample dependencies. Record exact input identity and instruction boundaries,
  map supported operations to authoritative AMD ISA semantics, and keep unresolved behavior named.
  This is analysis only: no new MSL, renderer integration, asset re-extraction, or game-frame claim.
- Files changed at claim: `docs/TASKS.md` (task row) and this HANDOFF checkpoint. No source or private
  shader files were edited. No asset archive was opened.
- Specification: `docs/METAL_FRAME_PATH.md` sections 4.6, 4.7 and task card 3; E625 inventories the
  logo VS/PS spans and flags WQM as deferred. The next exact action is a Trail source query for the
  decoder/emitter's current WQM and texture-sample support, then inspect only the existing E625
  private span and cross-check official ISA wording.
- Observation versus inference: default-pair completion permits advancing in the chosen shader order;
  the logo shader's earlier inventory label alone is not proof of its wave/quad semantics.
- Status: `ACTIVE`; attempt 0; no failed hypotheses. G3 remains PARTIAL at best; logo WQM/texture
  lowering and the L1 real-asset frame are not verified.

## 2026-10-24 16:03 CDT — GPU-03 synthetic parser-to-Metal task claimed

- Integrator: Codex `/root`; branch `cursor/init-hash-551140-9dfc`, clean at claim time, HEAD
  `863f8a86ebe189731e9ad9230746edfe29552e3b`. Task:
  `GPU-03-CMSH-SYNTHETIC-METAL-FLOW-01`, attempt 0, 0 failed implementation hypotheses.
- Bounded output: extend only the private Metal fixture test to create well-formed synthetic raw
  `.cmsh` bytes, parse them with `parse_cmsh_mesh`, pass the resulting live POS0/U16 vectors through
  `AssetMeshDraw`, then compare direct-Metal output with an equivalent hand-built draw using the
  hash-pinned default-pair MSL. No renderer API change is expected.
- Files changed at claim: `docs/TASKS.md` (this HANDOFF checkpoint and the task row are the claim).
  Source/tests have not been edited yet. Exact claim preflight: `git status --short --branch`,
  `git log -3 --oneline --decorate`, and `trail grep 'parse_cmsh_mesh'`; all completed with exit 0.
- Evidence prerequisites: E630 synthetic parser fixture; E632 synthetic direct-Metal frame; default
  MSL SHA-256 `5524537e36b2481798abe5bf6c311e37082aed26b136c4590f7d685e671a455e`. Reviewers verified
  that `CmshMesh` positions/indices match `AssetMeshDraw` spans, but the current Metal test does not
  call the parser.
- Ruling: keep this step synthetic while permission to parse real local asset bytes is pending. The
  test will use the existing harness-supplied `DefaultShaderCase` and private PNG output, without
  decoding NRM0 or attaching a screenshot. Cost if wrong: one extra synthetic-only checkpoint before
  the desired real-asset harness.
- Current claim check: `NOT RUN` for source implementation and tests. Next exact action: record this
  docs-only claim checkpoint under the owner identity and push the active branch, then implement the
  fixture and run its focused CTest plus publication gates. Real archive bytes, NRM0 semantics, and
  `attach_screenshot` remain unresolved; no L1 asset-derived frame or G3 claim is made.
- Observation versus inference: Trail reports 0.23.0 in use and 0.25.0 available; no upgrade or
  install occurred. The task uses the existing direct Metal path and adds no dependency.
- Task status: `ACTIVE`; one implementation hypothesis is pending, attempt count 0.

## 2026-10-24 15:58 CDT — GPU-03 direct-Metal synthetic-frame checkpoint published

- Integrator: Codex `/root`; active branch `cursor/init-hash-551140-9dfc`. Previous published
  checkpoint is `3f2f51b0b88186c4f0247f60b7f783fe24b5fee8` (typed RenderOp API). The synthetic
  Metal source/doc checkpoint is commit `c7adfdd68db845a590d963989ff35d2a627cac50`, pushed to
  `origin/cursor/init-hash-551140-9dfc`. Johnny Wang is author and committer; the commit has no
  Claude/Anthropic co-author trailer.
- `GPU-03-METAL-SYNTHETIC-FRAME-01` is `FIXTURE_VERIFIED`. Parent
  `GPU-03-RENDEROPS-ASSET-FRAME-01` remains `ACTIVE`, 0 failed implementation hypotheses. E632
  records an Objective-C++ direct-Metal executor for the supported typed-op subset and a synthetic
  quad/triangle fixture. The default-pair MSL input is pinned to SHA-256
  `5524537e36b2481798abe5bf6c311e37082aed26b136c4590f7d685e671a455e` (E627).
- Files changed: `native/CMakeLists.txt`, `native/platform/metal_capture.{hpp,mm}`,
  `native/platform/metal_asset_frame.{hpp,mm}`, `native/tests/test_metal_capture.cpp`,
  `native/tests/test_metal_asset_frame.cpp`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/INDEX.md`,
  `docs/FOLDERS.md`, `docs/checkpoints/2026-10-24-gpu03-metal-synthetic-frame.md`, `INSTALL_LOG.md`,
  and this file.
- Exact root commands: `cmake -S native -B local/build/orch-r22`; `cmake --build
  local/build/orch-r22 -j 12`; `ctest --test-dir local/build/orch-r22 -j 12
  --output-on-failure`; `ctest --test-dir local/build/orch-r22 -V -R '^metal_asset_frame$'`;
  `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh`. All exited 0. CTest
  passed 391/391; lift checkers passed 476, 0 skipped and 0 failed. Private logs are under
  `local/orchestrator/gpu03-r31/metal-frame/`.
- Metal API and GPU validation both logged enabled. The private 256x256 PNG is 6,395 bytes with
  SHA-256 `35039e0fe8a9700ccfbe8bb66181426c16ea41849677633cea81ac99c0d21a94`; CPU projection
  reports 33,856 covered pixels, 0 y-down silhouette errors, 0 y-down per-pixel draw/depth-class
  errors, and 3,424 y-up per-pixel errors. Repeat RGBA/PNG output matches; test-input vertex, index,
  and shader-constant mutations each change the PNG hash. No Metal validation error/fault diagnostic
  appears in the verbose test log.
- Observation versus inference: the fixture uses hand-built geometry and an explicit encoded +Z
  harness normal. It does not call `parse_cmsh_mesh`, read `cube_1m`/`cone` bytes, use `attach_screenshot`,
  or represent a game frame. PNG output is a private direct test artifact. NRM0 semantics, real asset
  reads, screenshot frame convention/seam, retail bindings, cold boot, game audio, real save/load and
  G3 remain unresolved or `NOT RUN`. Existing real-mesh evidence stays limited to E629 hashes/counts.
- Toolchain: Apple M4 Pro / arm64, macOS 27.0.1 (26A434), Xcode 27.1 (27A9275), Apple clang
  21.0.0, CMake 4.4.4 and Ninja 1.13.2. No tool/dependency was installed or downloaded; generated
  MSL, PNG, logs and build products remain under `local/`.
- `python3 scripts/project_index.py` indexed 9,038 folders. Trail initially reported stale edited
  test nodes; `trail build` rebuilt the source wiring graph and the following `trail build --check`
  passed with 8,292 nodes. `trail grep 'MetalAssetFrameRenderer'` found the new API in the header and
  test. The deep meaning layer remains unbuilt.
- Publication checks: staged path audit passed 14 paths before commit; full path audit passed 2,746
  paths after commit; attribution passed both before and after the commit. The default staged audit
  invoked after commit returned exit 2 because the index was empty; its required 14-path pre-commit
  audit passed. Post-push `git ls-remote`, local HEAD and tracking ref all report `c7adfdd...`,
  divergence is 0/0, and `git status` is clean. Logs are under
  `local/orchestrator/gpu03-r31/metal-frame/`.
- Next exact action: preserve the real-mesh/NRM0/screenshot choices as pending; a separate read-only
  survey of startup slot 306 is in progress while those GPU-03 inputs remain unresolved. Do not claim
  an L1 asset-derived frame or promote G3 until the real-asset and screenshot gates are met.
- Status: synthetic direct-Metal subtask `FIXTURE_VERIFIED`; parent GPU-03 `ACTIVE`; overall game
  acceptance remains incomplete.

## 2026-10-24 15:37 CDT — GPU-03 typed RenderOp API checkpoint

- Integrator: Codex `/root`; active branch `cursor/init-hash-551140-9dfc`. The prior native mesh
  parser checkpoint is published as `fa329cd3382b7aaf968ab86268366cc6033f8f4f`; local, tracking and
  origin refs matched. That owner-authored commit has no Claude/Anthropic co-author trailer.
- Task `GPU-03-RENDEROPS-TYPED-API-01` is `FIXTURE_VERIFIED`; parent
  `GPU-03-RENDEROPS-ASSET-FRAME-01` remains ACTIVE at attempt 0. Typed operations now describe
  pass, viewport, depth, pipeline, vertex/constants/index streams and indexed draws with host IDs;
  unsupported operations and invalid state/order fail visibly.
- Files: `native/platform/render_ops.{hpp,cpp}`, `native/tests/test_render_ops.cpp` and the CMake
  target. E631 and `docs/checkpoints/2026-10-24-gpu03-renderops-api.md` hold the checkpoint.
- Focused CTest passed 1/1. Full configure/build/CTest passed 390/390; lift checkers passed 476,
  with 0 skipped and 0 failed. Logs: `local/orchestrator/gpu03-r31/render-ops/` and
  `local/orchestrator/gpu03-r31/verification/`.
- `python3 scripts/project_index.py` indexed 9,028 folders; `trail build --check` found all 8,258
  wiring nodes in sync; `git diff --check` passed.
- Final staged path audit passed 10 paths; the full path audit passed 2,742 paths; pre-commit
  attribution passed. These checks are captured under `local/orchestrator/gpu03-r31/verification/`.
- The typed contract is structural only: no Metal executor, asset draw, PNG hash, CPU silhouette or
  depth comparison, mutation run, validation-layer result, or G3 claim exists. Real `.cmsh` bytes
  remain limited to the E629 probe hashes/counts while a scope question is pending; NRM0 and the
  screenshot frame label are unresolved. G3 remains PARTIAL at best.
- Next exact action: implement a direct-Metal executor for the supported typed ops and a private
  asset-derived harness test, respecting the user's evidence boundary and harness-input choices.
  Continue to exclude original binaries and generated shader/mesh data from publication.
- Status: typed API `FIXTURE_VERIFIED`; parent GPU-03 ACTIVE, 0 failed implementation hypotheses;
  cold boot, game audio and real save/load remain NOT RUN.

## 2026-10-24 15:30 CDT — GPU-03 native mesh reader integration checkpoint

- Integrator: Codex `/root`; active branch `cursor/init-hash-551140-9dfc`. Parent task
  `GPU-03-RENDEROPS-ASSET-FRAME-01` remains ACTIVE at attempt 0. The parser lane
  `GPU-03-CMSH-RAW-POSIDX-01` is complete at `FIXTURE_VERIFIED`.
- User direction: prioritize the asset-derived Metal slice and implement the default shader pair
  first. Round-30 source `9565322` already contains the offline default pair. This checkpoint adds
  only the raw POS0/index parser and its project CMake target; it does not implement Metal draws.
- Files: `native/platform/cmsh_mesh.{hpp,cpp}`, `native/tests/test_cmsh_mesh.cpp`, and the
  `sotc_cmsh_mesh` / `test_cmsh_mesh` entries in `native/CMakeLists.txt`. Evidence/checkpoint records
  are E630 and `docs/checkpoints/2026-10-24-gpu03-cmsh-reader.md`.
- Worker fixture compile and executable both exited 0. Root configure, target build, and focused
  CTest exited 0. Full native configure/build/CTest also exited 0: 389/389 tests. Lift checkers:
  476 passed, 0 skipped, 0 failed. Final staged path audit passed 10 paths; full path audit passed
  2,738 paths; pre-commit attribution audit passed. `python3 scripts/project_index.py` indexed
  9,020 folders; `trail build --check`
  found all 8,231 wiring nodes in sync, and `git diff --check` passed. Logs are in
  `local/orchestrator/gpu03-r31/verification/` and `cmsh-native/`.
- Evidence boundary: E629's existing probe outputs remain the only real-member evidence. The C++
  parser was exercised with synthetic bytes only; POS0 format code 2 is not confirmed on the actual
  sample bytes. No NRM0 decode, Metal frame, mutation result, G3 pass, or game behavior is claimed.
  A temporary private raw export was removed before parser use; the original archives were not
  modified.
- Remaining risks: native parser behavior on real members is intentionally NOT RUN; shader normals
  and the screenshot seam's harness frame label are awaiting the user's choices. Metal draw/depth,
  independent pixel checks and mutation sensitivity remain NOT RUN.
- Next exact action: implement the typed RenderOp subset and direct-Metal indexed/depth backend,
  using a per-vertex 288 B harness `ShaderCase` from the default MSL. Keep original member data and
  generated MSL private under `local/`; keep the log/artifact label `asset-derived harness` and G3
  at PARTIAL at best.
- Status: parser subtask `FIXTURE_VERIFIED`; parent GPU-03 ACTIVE, 0 failed implementation
  hypotheses. All game acceptance gates remain NOT RUN.

## 2026-10-24 15:17 CDT — GPU-03 raw-mesh and capture-seam checkpoint

- Integrator: Codex `/root`. Active branch `cursor/init-hash-551140-9dfc` contains the published
  round-30 source checkpoint `95653223f279bbaee5fb68743d415039e1a46f61`.
- **Task:** `GPU-03-RENDEROPS-ASSET-FRAME-01`, claimed by root at attempt 0. Its dependencies
  `GPU-01-CSDR-READER-01` and the user-selected default-pair slice of
  `GPU-02-GCN-SUBSET-DEFAULT-PAIR-01` are complete (E626/E627).
- **Bounded output:** an L1 **asset-derived harness** drawing the raw `cube_1m` and `cone` meshes
  with the default vertex/pixel pair through typed native render operations and the existing
  direct-Metal offscreen capture/PNG seam. Acceptance is the deterministic image, independent CPU
  projection/depth/outside-bounds assertions, Metal validation, and vertex/index/shader-constant
  mutation sensitivity in `docs/METAL_FRAME_PATH.md` card 4. No game-driven frame or G3 PASS claim.
- **Claim preflight:** `rg -n 'GPU-03-RENDEROPS-ASSET-FRAME-01|Asset-derived harness|### 6\.2 The seam|cube_1m|simple decoder|default pair draw' docs/TASKS.md docs/METAL_FRAME_PATH.md`
  exited 0. Log: `local/orchestrator/gpu03-r31/active-claim-preflight.log`. At claim time only
  docs were edited; the configured Projects wiki index has no SotC project entry.
- **Root input replay:** both exact `gpu_asset_probe.py cmsh` commands exited 0. `cube_1m.cmsh`:
  850 B, SHA-256 `44738dbf663af252898bad5582f2b48f1aadf6fbaab403c20880410b79a380b7`, 24 vertices/36
  indices. `cone.cmsh`: 1,462 B, SHA-256
  `5456afadabe96b9f7f02c503dc0ed5410652ccd32601c19e8aeeaaf36c88600c`, 42 vertices/114 indices.
  Exact commands and outputs: `local/orchestrator/gpu03-r31/root-mesh-replay/`.
- **Read-only reviews:** `/root/gpu03_mesh_triage` confirmed raw POS0 float3 and U16 index streams
  via `local/orchestrator/gpu03-r31/mesh-triage/mesh-triage-report.md`; NRM0 values are not decoded.
  `/root/gpu03_metal_seam` found the current provider is an L0 fullscreen pattern with no indexed
  draw/depth path, while `attach_screenshot` is a PNG callback/attachment seam. Details:
  `local/orchestrator/gpu03-r31/metal-review/metal-review.md`. Both reviews were read-only; no
  source or shared docs were edited.
- **Remaining risks:** the `encoded_normal` input, retail vertex fetch and shader bindings remain
  unverified; the Metal asset-derived draw path has not been implemented. Logo WQM, compressed
  assets, the retail renderer, boot, game audio and real save/load stay outside this slice or NOT RUN.
- **Next exact action:** implement a native reader for admitted raw POS0/index data under
  `GPU-03-CMSH-RAW-POSIDX-01` (worker owns only its parser/test files; root owns CMake), then add the
  typed-op Metal draw and independent projection/depth/mutation checks. The user is deciding
  whether this L1 shader harness supplies explicit fixture normals or decodes the NRM0 stream.
- **Status:** GPU-03 ACTIVE / root, 0 failed implementation tries. Raw mesh identity is
  ARTIFACT_VERIFIED; native decode and rendering remain NOT RUN. Detailed claim is in
  [GPU-03 checkpoint](docs/checkpoints/2026-10-24-gpu03-claim.md).

## 2026-10-24 15:08 CDT — round 30 published checkpoint

- Integrator: Codex `/root`. Round-30 source commit
  `95653223f279bbaee5fb68743d415039e1a46f61` is pushed to active branch
  `cursor/init-hash-551140-9dfc`. Detailed commands, evidence paths, limits and the next task queue are in
  [round-30 integration checkpoint](docs/checkpoints/2026-10-24-round30-integration.md).
- `GPU-01-CSDR-READER-01` attempt 3 is complete. The final parser validates a complete raw
  structure before classifying any compressed prefix, including every `cc xx` second byte.
  Independent review passed the six raw count collisions, all 256 short `ccXX` signatures, and
  full-member verifier ordering; focused checks were 563 native and 19 Python. Root replay of
  corpus v5 exited 0: 14,967 raw members accepted, 4 named structurally checked type-5/6 rejects,
  19,320 compressed-prefix members rejected, 21,861 accepted program rows, 34,291 member rows.
  All archives/manifests remain private; rejected compressed contents are not decompressed. The
  earlier type-2 and prefix-preemption hypotheses remain documented as failed, while attempt 3
  had no failed targeted check. LeakSanitizer remains NOT RUN (unsupported on this host).
- `GPU-02-GCN-SUBSET-DEFAULT-PAIR-01` default-pair slice is independently reviewed and root
  replayed. Exact hash-pinned input regenerated byte-for-byte identical MSL (`cmp` exit 0); the
  standalone harness compiled and ran on Apple M4 Pro with Metal API/GPU validation enabled and
  fast math disabled. All 2,141 compute cases completed; the offscreen render completed; the
  separate oracle passed 34,256 components with maximum finite absolute difference 0, and 2,131
  finite alpha outputs matched exactly. Source review found no P0-P2 issue. The shader uses the
  explicit test ABI only: real packed-mesh format, retail descriptor/SPI bindings, and retail FP
  mode remain UNVERIFIED. Logo WQM was not lowered; G3 is NOT RUN.
- Root integration gates passed: CMake configure/build exit 0; CTest **388/388**; lift checkers
  **476 passed, 0 skipped, 0 failed**; repository Python suite **228/228**; GCN inventory **23/23**;
  GCN-to-MSL/oracle tests **11/11**; Swift package **148/148**. Logs are under
  `local/orchestrator/r30/final-*` and `root-default-*`; private corpus output is
  `local/validation/csdr-reader-r30-root-v5/summary.json`.
- Project index exited 0 with **8,211 folders indexed**; `trail build --check` exited 0 with the
  wiring graph in sync (deep meaning summaries remain unbuilt). Full publication path audit exited
  0 across **2,675 paths**, attribution audit exited 0, and `git diff --check` exited 0. Exact logs
  are `final-project-index.log`, `final-trail-check.log`, `final-publish-audit-all.log`,
  `final-attribution-audit.log`, and `final-diff-check.log`.
- Root validated the exact staged set (**90 allowlisted paths**); staged path audit and
  `git diff --cached --check` both exited 0. Logs: `final-publish-audit-staged.log` and
  `final-staged-diff-check.log`. Remote fetch confirmed the active branch is **0 ahead / 0 behind**
  before this commit; effective git identity is Johnny Wang (`dehydratedflask@users.noreply.github.com`).
- Commit attribution audit passed for commit `9565322`; author/committer are Johnny Wang and the
  message has no co-author trailer. `git push origin cursor/init-hash-551140-9dfc` exited 0; fetch,
  local HEAD, tracking ref, and `git ls-remote` all verify `95653223f279bbaee5fb68743d415039e1a46f61`.
  Private confirmation: `local/orchestrator/r30/push-result.txt` and `postpush-*.log`.
- These are bounded fixture/artifact and static-analysis results, not game behavior. Real cold
  boot, audible game content, retail save/load after relaunch, Metal frame validation, and all
  required game acceptance gates remain NOT RUN. Round-30 integration is published; the project
  remains open. `GPU-03-RENDEROPS-ASSET-FRAME-01` is now active at attempt 0 with its L1 scope and
  native mesh-reader subtask recorded above.

## 2026-10-24 Codex round 30 — active orchestration claim

- Integrator: Codex `/root`; user resumed `/goal` with parallel workers. Active branch remains
  `cursor/init-hash-551140-9dfc`; baseline HEAD `9f79b65`. The prior round's no-publication
  instruction applied to that session; this run follows the current checkpoint publication policy.
- Initial checkpoint: prior modified and untracked files preserved privately in
  `local/orchestrator/r30/baseline.diff`, `baseline-untracked.tar.gz`, `baseline.json`.
- Bounded lanes claimed before edits: `CPU-01-STARTUP-302-303-305-01` (startup worker),
  `DATA-02-PSARC-MD5-RECONCILE-01` (asset evidence worker), and
  `APP-01-LAUNCHER-R29-VERIFY-01` (launcher worker). Root owns shared docs, evidence IDs,
  CMake integration if requested, full-suite verification, attribution, commit and push.
- Initial inspection: round 29 files plus an unfinished slot-303 module exist. Live files take
  precedence over the stale “Next exact work” list. Prior no-publication status is retained below
  as historical evidence. No game gate promoted. Verification pending; 0 new failed hypotheses.
- Next exact action: workers verify/complete their disjoint lanes; root reconciles documents and
  runs the required workspace-wide checks before publishing allowlisted changes.
- 2026-10-24 01:36 CDT, root targeted checkpoint: `python3 -I
  tools/lift/check_fowner_ctor_14db00.py` and `check_fowner_ctor_3c5cb0.py` both exit 0;
  respectively 256 ELF + 123 native and 151 ELF + 84 native mutations rejected. Evidence:
  `local/orchestrator/r30/owner-row3.log`, `owner-row18.log`, `root-targeted-checks.json`.
  `python3 -I tools/dev/gpu_asset_probe.py --self-test` exits 0 (fixture only).
- Attribution: effective identity is the owner. Unpublished audit exits 0. The optional `--all`
  audit exits 1 only for already-published `f6604a86e60e` (`orchestrator@local`), the known legacy
  issue; no history rewrite is authorized or performed. Active-branch Claude/Anthropic credit
  scan is clean (`local/orchestrator/r30/active-branch-attribution.json`).
- Observed launcher defect: CoreLocator accepts incompatible Mach-O images; launcher worker owns
  strict thin-arm64 `MH_EXECUTE` validation and rejection tests. Startup 302 also publishes
  identity vptr `0x15d4638` after the ten handle initializers (worker is verifying it).
  Status ACTIVE; 0 new failed product hypotheses. Publication awaits settled/reviewed lanes and
  full required verification; inherited slot303 is incomplete. Next: integrate worker reports.
- 2026-10-24, user priority update: **prioritize the Metal shader slice**. Startup worker is closing
  only its already-started 302/303/305 slice; subsequent worker capacity is assigned to
  `GPU-01-CSDR-READER-01` and `GPU-02-GCN-SUBSET-DEFAULT-PAIR-01`. These are importer/offline
  groundwork, with G3 still NOT RUN. Briefs and reports are private under `local/orchestrator/r30/`.
- Asset lane interim checkpoint, `/root/assets`, 2026-10-24: inherited MD5(path) pairing retained;
  malformed/ambiguous/non-string lookup regressions added; legacy XFF module relabelled as a
  generic prefix reader with an explicit identity manifest. `python3 -I -W error
  local/orchestrator/r30/assets-run-tests.py` exits 0: **228/228 tests in 10 script files**.
  Expected-red regression runs are retained as such, not failed product hypotheses. Live seven-
  archive/corrected-manifest replay is still pending at this interim checkpoint. Full report:
  `local/orchestrator/r30/assets-report.md`; source and historical manifests are preserved.
  The default shader probe exits 0 (1,988 B; VS 272 B, PS 240 B). No audio/Metal game gate claim.
  Next: finish live replay, apply its evidence supersessions, then verify native shader imports.
- Startup lane checkpoint, `/root/startup`, 2026-10-24: targeted CTest **5/5**; table **317/573**,
  first missing **306 / `0xa633b0`**; registered-set SHA256
  `ee1bf21bc35e9baddf2d4a54f8296379786ab103b630c92779d2f2ae53d976e0`.
  Five ELF/source checkers passed; slot303 rejects 330 ELF + 48 source mutations, slot302/305
  checker rejects 51 mutations; two native fixture mutants fail as required. REA MCP timed out
  at 330,000 ms (FAILED tool operation); persistent Ghidra read-only replay passed and is the
  accepted static evidence. No semantic retry count reset or execution claim. Exact report and
  log paths: `local/orchestrator/r30/startup-report.md`; full integration is pending.
- Launcher lane checkpoint, `/root/launcher`, 2026-10-24: locator and packager require thin ARM64
  `MH_EXECUTE`, reject non-system dependencies and inspection failure, and validate before
  replacing an existing app. Hermetic Swift: **157 passed + 1 skipped**, separate real-core
  integration **1/1 passed**; **5/5** invalid packaging fixtures rejected without replacing output.
  Release configure/build and strict signing/inspection exit 0; signed core SHA256
  `12ee91174f82ce1a351d2852afa56d8aa66745bd2dee9ef1d86e1f26ab691840`.
  Finder double-click of absent/present-core apps and AX diagnostics verified: Play disabled,
  present core engine NO, 317/573, gap306, absent fixture fails visibly at `0xe71990` after23.
  Native Quit leaves no app/child. Assets-ready GUI, VoiceOver listening and full G4 matrix NOT RUN.
  `local/orchestrator/r30/launcher-report.md` and `launcher/` hold exact commands/logs/captures.
  Status FIXTURE_VERIFIED / ARTIFACT_VERIFIED for this bounded scope; 0 failed implementation
  tries, no dependency changes or app/release publication. Next: review and full integration.
- Asset lane final checkpoint, `/root/assets`, 2026-10-24: live replay exits 0; **7/7** archive
  identities and **170,451** corrected MD5(path) pairings/row fields verified. All **40,213** shared
  base/patch cachedFiles members are byte-identical (66,897,142 decoded bytes compared).
  Corrected metadata cross-tab is **40,215 / 1 / 0 / 0**; 130,235 distinct exact paths/keys,
  no key spans distinct paths. Audio inventory: **86** rows / 2,073,162,522 B, zero XFF audio
  members; former 44 XFF samples map to non-audio extensions. Historical manifests unchanged.
  `assets-validation.json` SHA256 `2cb20dd0dfc371231004a4218731f77b0be53def80260480fa110694fa0aabc3`.
  Full report: `local/orchestrator/r30/assets-report.md`; public checkpoint
  `docs/checkpoints/2026-10-24-psarc-md5-reconciliation.md`. Status ARTIFACT_VERIFIED /
  FIXTURE_VERIFIED; 0 failed hypotheses. Next: root applies evidence/task supersessions and
  independently reviews source; Metal reader worker is active on the corrected mapping.
- Metal bounded ownership claim, 2026-10-24, root: `/root/shader_reader` owns
  `native/core/csdr_container.{hpp,cpp}`, `native/tests/test_csdr_container.cpp`,
  `tools/dev/csdr_inspect.cpp`, `tools/dev/verify_csdr_corpus.py` and its dedicated checkpoint;
  CMake additions are limited to that module/fixture/tool and require the build lock.
  `/root/shader_decode` owns `tools/dev/gcn_inventory.py`, `test_gcn_inventory.py`, private
  four-program fixtures/evidence and its dedicated checkpoint. Inventory must checkpoint
  before MSL generation. Root owns all shared evidence/task/index changes and publication.
- User decision, 2026-10-24: **implement the default shader pair first**. The logo pixel shader's
  `s_wqm_b64` is outside the simple per-lane subset; keep that path explicitly unresolved.
  Strict four-program instruction inventory must checkpoint before the default-only MSL task.
- Independent integration review found a publication blocker in inherited
  `tools/lift/check_fctor_slot_303_a82170.py`: its full contiguous original instruction-byte table
  must move to private evidence. Public checker should retain body hash and selected operand
  assertions. Review/fix pending; no completed task is published until this is resolved.
- Independent review checkpoint, `/root/integration_review`, 2026-10-24:
  `local/orchestrator/r30/integration-review-report.md` records owner/assets/launcher spec and
  quality PASSED for their bounded scope. Startup native quality PASSED; publication spec FAILED
  pending removal of the full-byte table at checker lines 56–153 (consumer 212–216): 96 entries /
  455 contiguous bytes reproduce the pinned function hash. No other complete function dumps in
  changed checkers and no other actionable P1/P2 findings. No tests were redundantly rerun.
  Next: startup implementer repairs checker, then scoped independent review and full integration.
- Continuation audit, 2026-10-24: agents hit a usage limit after writing source artifacts; current
  worktree and process inventory were rechecked. No shader/corpus job was live, so the two Metal
  workers and startup checker fix were explicitly resumed.
- GCN inventory checkpoint, `/root/shader_decode`, 2026-10-24: strict decoder covers all four
  private spans (804 B total: default VS 54 instructions/272 B, default PS 55/240 B, logo VS
  25/140 B, logo PS 34/152 B); unknown count **0**, each span ends exactly at `s_endpgm`.
  Synthetic tests 23/23; independent opcode cross-check 28/28 pairs plus EXP special case;
  private truncation and opcode/reserved mutations 804/804 and 168/168 rejected. A v1 mutation
  test changed VOP2 into a different valid encoding and was corrected to preserve VOP2. Logo PS
  uses WQM, EXEC restoration and implicit image derivatives; default pair has no WQM. User chose
  **default pair first**. Awaiting root acknowledgment of the inventory checkpoint before MSL.
- CSDR reader checkpoint, `/root/shader_reader`, 2026-10-24: C++ parser/tool/test are present and
  compiled. Root recheck: CTest `csdr_container` 1/1 and native-reader verifier self-test 10/10.
  The first private corpus pass accepted all raw pak0 (1039/1039) and pak1 (1070/1070), then found
  the four two-program exceptions have actual BinaryInfo type values **5 and 6**, not the earlier
  assumed **2**. This is failed hypothesis 1; v1 output remains in `local/validation/csdr-reader-r30-v1/`.
  V2 must not widen accepted content: structurally validate every record before returning the
  documented exception. Stage names for 5/6 remain UNVERIFIED. Full main-corpus pass pending; G3
  remains NOT RUN.
- Root continuation checkpoint, 2026-10-24 14:15 CDT:
  - Startup publication repair reviewed independently. Root ran
    `python3 -I local/orchestrator/r30/check_startup_publication_ast.py` and
    `python3 -I tools/lift/check_fctor_slot_303_a82170.py`; both exit 0. AST scan found 0 public
    instruction-table entries, selected pins cover 195/455 original body bytes, longest
    contiguous literal run 52 B, and the renamed-full-table positive control was rejected. The
    checker rejects 226 ELF + 48 native mutants. Root also ran
    `tools/dev/with_build_lock.sh ctest --test-dir local/build/orch-r22 -R '^ctor_slot_303$'
    --output-on-failure`: 1/1 passed. Logs:
    `local/orchestrator/r30/startup-publication-fix-checker.log`,
    `startup-publication-fix-ast.log`, `startup-publication-fix-ctest.log`,
    `startup-publication-fix-SHA256SUMS`. P1 publication issue resolved; no game/runtime claim.
  - GCN inventory independent replay: `python3 -I -W error tools/dev/test_gcn_inventory.py`
    exits 0, 23/23; `python3 -I -W error
    local/orchestrator/r30/verify-gcn-inventory.py` exits 0. Root replay logs:
    `local/orchestrator/r30/gcn-root-synthetic-tests.log`,
    `gcn-root-corpus-verification.log`. Static counts remain 4/4 exact spans, 168 instructions,
    804 B, zero unknowns. Decoder checkpoint is accepted for inventory only; MSL equivalence
    remains NOT RUN.
  - **Ruling:** proceed with the owner-selected default VS/PS pair MSL slice using the exact,
    hash-pinned private spans and an explicit harness-only resource ABI. This narrow work can run
    while the full CSDR corpus reader resolves its two-program exception because the selected raw
    asset and spans were independently extracted and checked. Cost if wrong: rerun the import
    against the settled reader. Stop on any unknown operand/binding; do not add runtime GCN,
    substitute zeroes, or claim live mesh/engine binding or G3.
  - Shader reader v2 self-test/build passed, but explicit `ASAN_OPTIONS=detect_leaks=1` aborts on
    this host before fixture execution (`AddressSanitizer: detect_leaks is not supported on this
    platform`; CTest exit 8 / direct exit 134). This is a failed environment/tool check, not a
    product hypothesis. LeakSanitizer is NOT RUN; worker is rerunning corpus checks under the
    supported default ASan+UBSan configuration. Full raw corpus result remains pending.
  - Next exact action: shader decoder emits default-only MSL and a separate oracle, then proves
    numerical agreement and Metal compilation; root reviews the diff. Shader reader completes its
    default-sanitizer replay and v2 corpus/fixture evidence. Full workspace verification,
    attribution/path audits, commit, and push remain root-owned.
- Root coordination checkpoint, 2026-10-24 14:44 CDT:
  - Slot303 repair lane accepted and worker closed after root replay. Files changed in this
    checkpoint: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/INDEX.md`, and new
    `docs/checkpoints/2026-10-24-slot303-publication-repair.md`.
  - Shader decode task now active on the explicitly authorized default-only emitter/oracle; logo
    WQM remains outside scope. `shader_reader` is still scanning v2 against the 12.8 GB pakN
    archive. Its latest report: 553 fixture checks, 216 structural flips and 264 truncations
    rejected, focused CTest1/1, native-reader self-test10/10; no complete pakN result yet.
  - `csdr_review` was dispatched read-only to independently audit reader bounds, exception handling,
    fixture quality and publication privacy. Exact action: `collaboration.spawn_agent` with task
    `/root/csdr_review`; exit returned the agent identity; review is ACTIVE and report pending.
  - Next exact action: integrate shader reader's final corpus evidence and decoder's default MSL
    report; then obtain independent shader review and run all required workspace gates.
- Root integration checkpoint, 2026-10-24 14:50 CDT:
  - Root CSDR replay of the v2 reader exited0 for all three archives: pak0 1,039/1,039 raw, pak1
    1,070/1,070, pakN 12,858/12,862 accepted with four named rejects; 19,320 compressed-prefix
    rejects. Exact command is the v2 `verify_csdr_corpus.py --expect-main-corpus` invocation; log
    `local/orchestrator/r30/shader-reader-corpus-root.log`, summary and manifests under
    `local/validation/csdr-reader-r30-root/`. This replay used the pre-fix binary and is retained
    as independent corpus evidence, not evidence for the current classifier.
  - Independent review found the two-byte prefix correction still preempts valid raw containers
    with counts 1,676 (`8c06`), 2,700 (`8c0a`), 1,740 (`cc06`) and 2,764 (`cc0a`). The reviewer
    verified the synthetic containers with the Python parser; native CSDR reported
    `unsupported_compression`. Actual archive counts are unaffected. The two-byte patch is not
    accepted; this is reader issue hypothesis 2. The first original type-2 stage inference remains
    failed hypothesis 1.
  - **Principal ruling / authorized final attempt:** perform full raw structural validation before
    compressed-prefix fallback, for both native reader and corpus verifier. The documented compressed
    signatures are `8c 06`, `8c 0a` and `cc xx` (any second byte). Test the four collision counts
    plus all signature classes; a valid raw parse must win, while compressed inputs still return a
    named rejection and expose no partial programs. This resolves
    the ambiguity because the raw format begins with an unrestricted little-endian count. Cost if
    wrong: the corpus verifier may read each exact-prefix candidate in full, increasing local scan
    time; correctness takes precedence and inputs stay private. If this third distinct hypothesis
    fails its targeted check, stop and escalate `GPU-01` per AGENTS.md; do not attempt a fourth fix.
  - Root's focused replays remain green for the intermediate two-byte patch: CTest1/1, synthetic
    fixture555 checks, verifier self-test12 checks. These do not resolve the outstanding collision.
    Worker `shader_reader` is applying the structural-first ruling and running a post-fix corpus
    pass; independent scoped re-review is pending.
  - Index gate: `python3 scripts/project_index.py` exited0, indexing8,198 folders;
    `trail build --check` exited0 (wiring graph in sync; deep meaning summaries remain pending).
    `mcp__trail__trail_file_api(native/core/csdr_container.cpp)` refreshed2 files and returned the
    parser API, proving the changed native source is queryable. Live graph has default root scope,
    includes native/launcher/scripts/tools, and contains no persisted `onlyDirs` filter; native
    tests are under `native/tests` and no root `tests/` directory exists. No redundant path filter
    was added. Logs/results are in the tool outputs for this root checkpoint.
  - Default-only GCN/Metal implementation interim: `Metal.framework newLibraryWithSource` compiled
    the generated MSL on M4 Pro with fast math disabled; 2,141 compute vectors and an offscreen
    pipeline ran with `MTL_DEBUG_LAYER=1` and shader validation, no reported faults. A separate
    high-level transform/interpolation/lighting oracle matched 34,256 components exactly (max
    absolute error0); 2,131 finite alpha moves/conversions exact. Input set: 2,048 random, 90
    NaN/Inf/signed-zero/subnormal/half-edge and 3 raster cases. Private logs:
    `local/orchestrator/r30/default-metal-execution.log` and
    `default-oracle-comparison.log`. `xcrun metal` is an unavailable toolchain stub; this FAILED
    environment check did not stop the existing Metal.framework route and no dependency was
    installed. MSL mutation/rejection checks and the shader worker's public checkpoint are pending.
    Harness-only bindings; packed mesh format, SPI interpolation mode, live engine binding, actual
    scene and G3 remain UNVERIFIED / NOT RUN.
  - Next exact action: finish the principal structural-first CSDR attempt and default-pair negative
    mutations, then root performs independent review/full workspace gates, path/attribution audits,
    allowlisted owner commit and active-branch push.
- Root review checkpoint, 2026-10-24 14:55 CDT:
  - Structural-first CSDR attempt3 is implemented. Independent scoped reviewer `/root/csdr_review`
    found no P0-P2 issue in the collision handling: native parser performs full structural parsing
    before prefix fallback; corpus verifier reads full members before classifying. Reviewer replayed
    561 C++ checks and18 Python verifier checks; both passed. Private full-corpus v4 run is still
    active; no post-fix corpus result is recorded yet.
  - Root's live spec review found that the documented stored-block signature is `cc xx` with any
    second byte, while the current structural-first fallback/test set only includes `cc06`/`cc0a`.
    The same authorized attempt3 now adds wildcard-CC fallback and a `cc7f` short-prefix regression,
    retaining valid raw count204 precedence. This is a spec-completeness follow-up, not a failed
    hypothesis; final scoped re-review is pending.
  - Next exact action: rerun reviewer after the CC wildcard case lands, await corpus v4, then root
    independently replays the current final reader and runs the full integration gates.

Historical status snapshot from the prior handoff: 2026-10-24 (America/Chicago). Integrator:
OpenCode orchestrator (goal resumed by explicit user command). Branch `cursor/init-hash-551140-9dfc`;
round-28 code tip `344efa8`, round-27 code/docs tips
`991b7da`/`c61d2ca`, round-25 tips `f394fc8`/`1eebca2`/`12f0ca2`/`c01f774`, round-24 `9a85650`.
Older rounds, the history-rewrite record and the full pre-tidy handoff are archived verbatim in
[docs/checkpoints/HANDOFF-archive-2026-10-23.md](docs/checkpoints/HANDOFF-archive-2026-10-23.md).
The rounds that follow the archive are in chronological order.

## Historical status snapshot — round 29 (superseded by the round-30 checkpoint above)

- Full native suite: **385/385** (round 29, uncommitted; was 382 at `9f79b65`) (`ctest --test-dir local/build/orch-r22`), ASan+UBSan, Mach-O arm64.
- ELF checker sweep: **471 passed / 0 skipped / 0 failed** with
  `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh`.
- Init prefix `0x531d60..0x532b0c` (the full body ends at `0x532c8b`): 100% of prefix bytes claimed by
  native modules (`tools/lift/report_init_coverage.py`), with the recorded gaps and limits. **Claimed
  bytes are not semantics** — most of it is call-site pinning with callee hooks.
- Publish gate: `python3 -I scripts/audit_publish_paths.py` and
  `python3 -I scripts/check_commit_attribution.py` both exit 0 at this tip.
- A native `SoCGameCore` arm64 executable now exists and reports typed readiness over protocol v1
  (314/573 constructors, first gap 302 after round 29). It has **no engine** and never reports running.
- **G1 PARTIAL. G2–G6 NOT RUN.** No boot, no game audio, no save/load of retail data, no Metal game
  frame. The Metal capture provider renders only a synthetic pattern; the save persistence proof
  uses synthetic `ProgressV1` data.

## Housekeeping 2026-10-23 (no code change)

- Removed 4 stale git worktrees. The two that held unpushed work were preserved first on
  `origin/cursor/parallel-foundations-86ac55b` (detached tip) and
  `origin/cursor/parallel-foundations-wip-20261023` (10 uncommitted files).
- Pruned 242 stale `local/build/*` trees, keeping `native` (several `tools/lift/check_*.py` pin it)
  and `orch-r22` (current suite); ~9.5 GB reclaimed. Re-verified after pruning: 339/339 and 419/0.
- Deleted 10 root `0x*.s` incbin scratch stubs, stray `.DS_Store` files, and a 0-byte
  `/Volumes/Extra/SoC`.

## 2026-10-23 Orchestrator round 21 — 0x1bdeb0 blocker resolved (E558)

- Agent: integrator (goal resumed by explicit user command).
- Task: `CPU-01-LIST-DTOR-1BDEB0-RESOLVE-01` (E558).
- Observation: `0x1bdeb0` has exactly one caller, `0x25dc76`, which passes `*(0x2ee68c8)` — the same
  singleton `0x1290` uses as the A-insert owner. `0x1bdb50` is that list's inserter and allocates
  **0x58-byte nodes**, writing a dword flag at `+0x50` and two 0x20 B callables at `+0x10`/`+0x30`.
  The container therefore holds two node types (0x58 flag nodes and 0x60 callable nodes), which is
  exactly why the `+0x50` test looked contradictory.
- Correction: E557's "count is seeded at construction" is **withdrawn** — 33 indirect
  `lock incq (%reg)` acquire sites exist (including `0x1bdb9d`/`0x1bdbce` in `0x1bdb50`); only the
  narrower "`0x14ccb0` is not the add side" claim stands.
- Next exact action: lift `0x1bdeb0` over an explicitly typed 0x58 node (prev `+0x00`, next `+0x08`,
  callables `+0x10`/`+0x30`, dword flag `+0x50`), releasing via `shared_release_f590` and freeing
  via tag 2, with mixed-flag fixtures.

## 2026-10-23 Orchestrator round 22 — 0x1bdb50 inserter lifted (E560)

- Agent: integrator (goal resumed by explicit user command).
- Task: `CPU-01-INSERT-1BDB50-NATIVE-01`.
- Observation: `0x1bdb50` is 304 B (`add72de8…`), signature sentinel/A/B/flag. It acquires each live
  control block, allocates 0x58 through `0x17090`, stores the callables at `+0x10`/`+0x30` and the
  flag at `+0x50`, links, `incq [sentinel+0x10]`, then **zeroes and releases both temporaries**.
- Key detail: those releases are no-ops by construction (zeroed holder + null-control early return),
  so exactly one reference survives per callable — the acquire is transferred to the node.
- Second detail: the original increments the count 64-bit and the destructor decrements 32-bit; equal
  below 2^32, now documented instead of glossed.
- Verification: 339/339 ctest, checker OK, sweep 419 passed / 0 failed.
- Next exact action: find the caller of `0x1bdb50` (same `E8` scan shape as E558) and determine
  whether it sits inside `0x2569d0`, the owner installer that still gates real singletons.

## 2026-10-23 Orchestrator round 23 — the 0x58 list is an application info registry (E561)

- Agent: integrator.
- Task: `CPU-01-REGISTRY-SEMANTICS-01`.
- Observation: `0x1bdb50` has five call sites (`0xb72`, `0x25a538`, `0x25a604`, `0x25a68a`,
  `0x25a88f`), all passing string views and `ecx = 1`. The strings name the registry:
  `"Game"` → `"Shadow of the Colossus"`, `"Build Version"`, `"Platform Type"` →
  `"ps4 - NEO"`/`"ps4 - STA"`, `"Is Debug Memory Available"` → `"True"`/`"False"`.
- Container: `*(0x2ee68c8)`; its installer `0x25a4d0` has no direct caller and is installed through
  the function-pointer slot `0x15ff080`, the fifth entry of an eight-slot table.
- Negative: `0xa54920` never references `0x2ee68c8`, so this registry is **not** the ConVar path.
- Flag: all observed inserts pass 1; no writer of 0 found, so the destructor's `flag == 0` purge
  targets a state not yet located. Headers updated to say that instead of "unknown".
- Next exact action: identify the owner of the `0x15ff060..0x15ff0a0` table, which is what constructs
  `0x2ee68c8`.
## 2026-10-23 Orchestrator round 24 — app-info registry path (E562–E567)

- Agent: integrator plus six workers (three static-RE, one integration, one f590 census, one f590
  wiring).
- **E562** — the `0x15ff060..0x15ff0a0` window is eight `R_X86_64_RELATIVE` slots in `.data.rel.ro`,
  not a vtable: the base is never taken, nothing writes it, slots 6/7 are data. `0x2569d0` is a
  consumer; it constructs the registry and registers slot 4 (`0x25a4d0`) as a list-3 delegate.
- **E563** — `0x25a4d0` is 1014 B (`12a2d0d7…`): four product entries (Build Version, Platform Type,
  Is Debug Memory Available, Camera Location), all flag 1. `"Game"` is a separate 139 B writer
  `0xb10`. E561's `"ps4 - STA"` and functor claims are corrected; platform is `ps4 - STANDARD`, the
  debug predicate is always false. Byte check also corrects list-3 dispatch to **FIFO**.
- **E564** — native registry path landed: `app_info_registry`, `app_info_install`, `registry_owner`
  (FIFO list-3 dispatch; identity-only `fn!=0` nodes are skipped, never executed) and
  `container_dtor_1bede0` (208 B `632451ff…`, list-C vtable slot 3 = `0x19f0` bare ret). Four new
  fixtures plus `check_fapp_info_install.py` and `check_fcontainer_dtor_1bede0.py`.
- **E565** — 10,168 direct `0xf590` sites classified by holder; the 17 Init hooks are all zeroed
  32 B temporaries, so the real release is an exact no-op there. All 17 modules now call
  `shared_release_f590` with a holder and assert `had_control == false`; mutation removing one lift
  fails the slice test.
- **E566** — preserved parallel-foundations commit `a49fd2a` integrated: exclusive save temp,
  `shared_str_release_f590`, rackquery CFG tool + 20 tests, `check_ff590.py`.
- **E567** — `0x2569d0` mapped: 12,621 B (`4ce6b5d8…`), verified `sub rsp,0x1118`, 87 reachable
  blocks, 369 calls. Its registry block `0x256f98..0x257021` is modelled by `registry_slot`
  (construct replacement → tear down previous → replace); the four callback installs and ordering
  are recorded.
- Verification: **345/345 ctest**, sweep **442/0/0**, publish and attribution audits exit 0.

## 2026-10-23 Orchestrator round 25 — list A reader/drain, list B, owner rows/properties (E569–E572)

- Agent: integrator plus five workers (list reader, camera chain, owner rows, Init f590 remainder,
  0x533100 f590 sites).
- **E569** — the list-A reader is the second half of `0x1bdf30` (`0x1be14d..0x1be222`); it serializes
  every entry with no lookup, then drains `flag == 1` nodes (`0x1be3f4..0x1be456`). The gate is the
  `snapshotBugData` command, so dispatch is not per-frame and re-entry cannot duplicate. `0x1bdeb0`
  handles `flag == 0` (none produced). Native `app_info_collect` models the serialization/drain;
  checker `check_fapp_info_collect.py` pins the bytes.
- **E570** — `0x2569d0` non-registry rows: 20 `0x17070` sites = 18 slot stores + 2 self-registered +
  1 factory; three destroy idioms; gate = **RetailBuild**; mode provably **1** (mode-2 block
  unreachable); 259 constant property rows (map A `{i,f=1,tag}`, map B `{pair,v,tag}`), 139
  reachable. Native `owner_properties_2569d0` + 260-row generated table + `classify_owner_mode` and
  `apply_owner_properties`; checker pins the gate/mode/record bytes and table hash.
- **E571** — list-B inserter `0x1bdc80` lifted (417 B, `0x50` nodes, container+0x20 sentinel, count
  +0x30, four `0xf590` hand-offs); `container_dtor_1bede0` now uses the 0x50 `Node1bdc80` for list B.
- **E572** — round verification: **349/349 ctest**, sweep **446 passed / 0 skipped / 0 failed**,
  publish/attribution audits exit 0.
- **E573** — camera chain characterized; pure `0x50ba0` Euler math and the six-double `setCameraPos`
  formatter are native (`camera_location_25a4d0`). The live pose remains blocked on engine state.
- **E574** — the four provably-zero `0xf590` sites in app vslot `0x533100` now run the lifted
  `shared_release_f590` with real holders (`"r_fps"` ×2, `"Position"` ×2). The nine rank-2
  key-holder sites remain real refcount releases and are untouched; `0x5333e3` stays a per-element
  hook. Mutation checks fail on a non-null control and on a skipped release.
- **E575** — list B is the screenshot-attachment channel and is **serialized, not counted**: the
  reader (`0x1be2f7`, 239 B) walks FIFO and emits one `"Attachment"` node plus a kind-4 value child
  per entry, copying value and name. The clear (`0x1be458`, 69 B) releases value then name, frees
  with tag 2, and zeroes both links and the 8-byte count. Native `report_tree` + `appinfo_listb`;
  list-B fields renamed to the recovered next/prev order and the node allocation corrected to the
  `0x17090` flags-2 wrapper. Checkers 447/0/0, ctest 350/350.
- **E576** — `0xac5260`/`0xac5320` are `insert_equal` consumers, not tables: 236 + 23 unrolled
  call sites, no loop, duplicate keys allowed (8 keys appear twice). Map A is
  `{i32 i, f32 f=1.0, u32 tag}`; map B is `{f32 lo, f32 hi, f32 v, u32 tag}` — the old "u64 pair"
  reading was a float range. The sink/table now carry `lo/hi`; the fixture pins 122 A + 12 B
  inserts, 114 distinct A keys and two `0x7b` rows. The bag and insert bodies remain contracts.
- **E577** — the bag is now a native model: `OwnerPropBag` (0x178 B factory/base ctor, four
  sentinels, `*(0x3305568)` republish) uses ordered multimaps so `insert_equal` duplicates survive,
  and `OwnerPropBagSink` lets `apply_owner_properties` populate it. Gate 1 yields 122 A + 12 B
  (114 distinct A keys, two `0x7b` rows); gate 0 yields 125 + 12. The exact RB nodes/fixups and
  trees 0/1 remain contracts.
- **E578** — the full `0x1bdf30` report path is composed natively in the recovered order:
  list-A `"FieldEntry"` serialization, list-B `"Attachment"` serialization, list-A `flag==1`
  drain, list-B release/free/reset. `drain_app_info` is public; `app_info_snapshot` owns the
  composition. The fixture covers order, counts, the flag-0 survivor, deep copy and both failure
  paths. The `snapshotBugData` gate and root adoption remain contracts.
- **E579** — the 21 `0x2569d0` install rows are now a native driver: 19 slot stores + 2
  self-registered, the three destroy idioms (virtual dtor, dtor+free, row 7 no replace), row 3/17/18
  arg dependencies, and visible stops for a missing ctor/allocator/dtor. All 18 non-trivial ctor
  bodies remain hooks.
- **E580** — the three trivial `0x2569d0` ctors are native: `0xa1cc0` (no-op), `0xfafb0`
  (0x82028-byte layout) and `0x26f820` (base/vptr/+0x18/sub-object). The guest vptrs are host
  values, never executed.
- **E581** — the 0x20 B `SharedStr` ctor `0xf1a0` is native (align4 capacity, data tag 0, control
  tag 1, `*control=1`), and the key blocker is resolved: `[[0x2ee2cf8]]+0x18` is
  `ResourcePool::GetResource`, not a destructor, so the nine `0x533100` sites need a real release
  with `previous ∈ {1,2}`. The existing `shared_release_f590` is unchanged; the nine sites are the
  next wiring step.
- **E582** — `0x14c2f0` is native: it builds the `SharedStr`, rewrites the `"****"` placeholder with
  the runtime platform tag (`movbe` fourcc + `0x147e20` hook), then hashes the patched text with
  FNV-1a-32. A placeholder without a tag hook fails before allocating. The nine per-site literals
  are ready for the fragment wiring.
- **E584** — the measured `0x1bdf30` path is composed natively: list-C dispatch then the report
  snapshot (list-A FieldEntry, list-B Attachment, drain, clear). The fixture installs five entries
  through the real product/Game delegates and serializes/drains all five; a dispatch failure stops
  before serialization. The gate, screenshot query and root adoption remain contracts.
- **E585** — the `snapshotBugData` block is native: `0x25db56` is a block inside `0x25d8b0`, not a
  function. It clears the debug gate before building, calls `0x1bdf30`, emits the synthetic
  `"Command"`/`"screenshot"`/`"bug_screenshot"` entry when the screenshot flag is 0, inserts list-B
  `("Screenshot","bug_screenshot.png")` on a successful readback, and adopts the root through the
  Frame holder. The GPU readback, file sink and `frameNum` remain contracts.
- **E586** — the nine rank-2 `0x533100` key sites now build the real `HashedStr` key
  (`resource_id_ctor_14c2f0`, length-pinned literals) and release it with `shared_release_f590`;
  the 8-byte stand-in is gone. Production requires a key allocator and platform-tag hook and fails
  visibly without them. `resource_pool_miss` models GetResource's +1.
- **E587** — `0xf3c0` resize/COW and `0xf6b0` terminate are native. Four paths (control null,
  shared control, capacity sufficient, capacity short); **only the capacity-short path returns
  true**, correcting the audit table. The fixture covers all paths, the shared-control refcount,
  the three size classes and failure preservation.
- **E588** — the bug-report Frame root is native: `0xa70c30` lazily creates `"Frame"` with the
  integer `frameNum`, and `0xa70d10` adopts the report root through `0xab0d40`. `ReportItem` now
  carries integer properties. The live frame counter and the holder's `ctrl` remain contracts.
- **E589** — the ResourceSystem platform-tag lookup is native: `0x147e20` is a 25-row
  `fourcc -> tag` table, `crmp`/`calt` map to `_cmn`, only `cfon`/`cptx`/`cctx` map to `_ps4`, the
  `"****"` wildcard is `_cmn`, and an unpopulated registry falls back to `_ps4`. The nine attach
  fragments default to the real table, and the integration fixture asserts the nine derived FNV
  key hashes.
- **E590** — the bag derived ctor is characterized and **E576 is corrected**: the four 0xc88
  objects are per-user input devices, not language tables, and they do not affect
  `insert_a`/`insert_b`. The property-map path is complete; the input-device lifetime, mutex, IME
  block and vectors remain contracts.
- **E591** — the bag's 1..4 input-device selection is native: initial device + login ids, negative
  skips, initial-only dedupe, and the `r13 <= 2` cap. The report's exact case
  `initial=5, login={0,1,2,3}` yields `{5,0,1,2}`. The 0xc88 device bodies remain contracts.
- **E592** — `owner_bag_construct` models the observable derived-ctor sequence with pad/initial/
  gesture/login/publish hooks, the 1..4 selection, and `+0x10 = 1` last. The four input devices are
  opaque/null-safe and are not allocated by the native model.
- **E593** — ResourcePool is characterized: the 0x100 B object is `ResourceWrapper` (no vtable),
  `+0x30` external refs, `+0x40` deps; the pool map is a non-owning `std::map<ResourceId,
  ResourceWrapper*, hash-then-strcmp>`; `GetResource` returns `owned=false`, and the nine attach
  sites only use pointer identity and the two counters; sites 1-8 take the priority-13 early
  return, site 9 (priority 9) writes state 9 and notifies the engine. Slots start
  `{null_wrapper,false}`.
- **E595** — the nine `0x533100` attach fragments are wired to the real `ResourcePool`: key ->
  `GetResource` -> `AddReference` -> persistent-slot `assign` -> two `RemoveReference` -> `0xf590`
  key release -> `Lock(13)` (site 9 `Lock(9)` at the `0x53403c` tail). The earlier
  `loaded`/`success_flag`/`notify_95580` reading is withdrawn: `[out]`/`[out+8]` are the
  `GetResource` `{ptr,owned}` result and `owned` is always 0, so those branches are dead. Steady
  state `refs=1`, `deps=1`; a second attach is a hit with an idempotent lock. The appmain slice
  owns one native pool plus nine slots and frees all nine wrappers at teardown.
- **E596** — the owner's `+0x48`/`+0x58` vtable slots are native, and the premise is corrected: they
  are virtual methods, not constructor rows. `+0x48` ensures/activates the 0x210 capture singleton
  (`0x32f79a8`); `+0x58` is the gated dev property insert (key 4) into the bag tree 0.
- **E597** — the screenshot path is characterized: `0x961a0` is a PNG encoder, the report sink is a
  socket `"SendFile"` channel (not a file), and `frameNum` reads `engine+0x70` whose writer is not
  found. The capture/sink contract is proposed but not implemented.
- **E598** — the SaveData contract: a flat 15,576-byte progress blob behind a `{1,hash,size,4}`
  header with a 1-based result table. No real save exists; the transaction is designed, not built.
- **E599** — the camera `+0x2e8` census is negative and changes the reading: no writer exists, and in
  `_Root` the field behaves as an intrusive-list node, not a bound PMF. `0x216480` is never
  address-taken; the pose path is debug/snapshot-only or `D` is a different class. Do not model a
  native `BoundMemberFn` until `0xa68030` resolves `D`'s class.
- **E600** — the screenshot seam is native: `attach_screenshot` implements the E597 decision table
  (no renderer/capture failure -> synthetic `Command`; ok -> best-effort `SendFile` + list-B
  `("Screenshot","bug_screenshot.png")`, synthetic suppressed; sink result discarded). The snapshot
  block accepts the seam. The GPU encoder, surface and socket remain hooks.
- **E601** — the SaveData transaction slice is native and fixture-verified: `ProgressV1`, the
  `20005+4N` body, `{1, FNV(body), size, 4}` container, load/store/backup/enumerate over a
  `SaveIoOps` seam, atomic rename and backup-before-commit. The N=2 golden is 20,013 B /
  `0x5ce4db99`, independently reproduced. No real save exists; G2/G5 NOT RUN.
- **E602** — `D` is a Camera-typed registry object, **not** `_Root`; the 24-byte block is at
  `[D+0x18]+0x2e8` and the pose path is the debug-only `SyncCameraAcrossTools` tool command. The
  `element+0x18` field does not exist (it is the next slot's value). No native BoundMemberFn.
- **E603** — the owner row-9 ctor `0xa5c850` is native: 721 B, the 0xa530 object's partial
  initialization (head scalars, six inline vectors, 256×0x40 records, the lane group, 32×1.0f,
  `0xf180`, `0xb09650`, two event flags) and the 24 B global copy. `0x3b3d0` is
  `sceKernelCreateEventFlag`, not a mutex, and `0x32f8ff0` is the pointer slot. The dtor
  `0xa5cb30` remains a contract.
- **E604** — the SaveData metadata/icon/delete contract is characterized: `0x530` block
  (`memset` + three `strcpy` + `+0x500`), `SetParam` result discarded, only `+0x500` read back, the
  icon is a shared resource copy, delete has three sites and no BUSY retry, and dialog status 2/3
  is corrected. `save_store` does **not** call `save_check_backup`. Implementation is the next save
  slice (`SAV-03-METADATA-01`).
- **E606** — the metadata/icon/delete slice is native: `save_meta` (length-checked `0x530` block,
  `USR-META` container, shared icon cap), `save_store(SaveAttachments)` with advisory failures
  keeping `data_committed`, `save_delete` (satellites then data, no BUSY retry), enumerate metadata
  fields and the result/dialog tables. 102-check fixture.
- **E607** — the owner row-9 dtor `0xa5cb30` is native: 594 B, 22 calls in the recovered order,
  event flags `+0xa510`/`+0xa500`, the vector/tree/shared releases, the drain, and the head
  releases; 8-section fixture including ctor prefixes and double destroy.
- **E605** — `_Root::vtable[+0x10]` is a 5-argument pass-through (`0x1d64e0` -> `0xa6c7d0`) invoked
  once in the world base ctor; it does **not** attach a Camera. The camera pose consumer is the
  debug-only `SyncCameraAcrossTools` command and is not reachable from gameplay, so a native
  `BoundMemberFn` is not a gameplay requirement.
- **E594** — the native ResourceWrapper/ResourcePool/ResourceRef model is implemented: exact
  0x100 B wrapper, hash-then-strcmp comparator, non-owning map, hit/miss references, erase at zero,
  dependency recursion, the priority state table, and lock/unlock refs. Hooks cover allocation,
  payloads, manager deletion and engine notification; 317 checks. It is not wired into the launcher.
- **E583** — the camera delegate has **no static writer**: `D` is the `_Root` node (vtable
  `0x15992b8`, ctor `0x1d6230`, 0x3c8 B), and the ctor zeroes `+0x2e8..+0x307`; an opcode-aware
  census found zero stores. The bound PMF target returns `const CameraTransform*`, but it is
  unbound in this build, so the live pose remains blocked. `bpe::Camera`/`DebugCamera` were also
  recovered, and the program-header map was corrected (phdr 1 carries `.data.rel.ro`).

## 2026-10-24 Orchestrator round 28 — SoCGameCore, Metal capture, owner rows, ctor coverage (E608–E617)

- Agent: integrator plus eight workers (ctor coverage, SoCGameCore executable, Metal capture,
  owner rows 14/21, launcher process client, startup-cliff analysis, startup slots 296/297/300).
- **E608** — constructor coverage: `recovered_constructors()` now registers **308/573** entries
  (311 after E618).
  152 already-lifted `vec_*`/`leaf_*` modules are on the production path and index 95 (`0xd71f60`,
  a 17-byte `vmovups` leaf) is lifted. `ctor_original_table` carries the 573 rows (backwards table)
  and reports the first unrecovered index **296 / `0xb2b6b0`**; 265 `other`-class slots remain.
- **E609** — native `SoCGameCore` arm64 executable with protocol v1 (`status`, `coverage`,
  `validate`, `start`, `serve`). Status reports 308/573 at this slice (311 after E618) and
  `run_ok=true`; `--start` exits 3 with
  `SOTC_ERR_ENGINE_NOT_IMPLEMENTED`; a missing fixture is fail-visible at `0xe71990`; the serve
  state after a refused start is `failed`, never `running`. API coverage now says startup/save
  PARTIAL, engine/Metal/audio NOT_IMPLEMENTED, and the save row is no longer falsely “no path”.
- **E610** — direct-Metal offscreen capture provider (`native/platform/metal_capture.{hpp,mm}`):
  640x360 BGRA8 render, tight RGBA8 readback, ImageIO PNG (5,258 B, sha256 `e6a64e93…`), wired into
  `attach_screenshot`. Synthetic MSL pattern only; **G3 NOT RUN**.
- **E611/E612** — owner rows 14 (`0xa597d0`, 0x202 B, deque + 64-trip clear loop + 7-arg thread
  frame) and 21 (`0x4a690`, 0x194 B, drains an intrusive queue into an ordered tree via
  `insert_unique`). Both correct E570's summaries and carry 8-section fixtures + checkers.
- **E613/E614** — launcher child-process client (`CoreLocator`, `ProcessCoreClient`,
  `CoreClientFactory`) with 147 Swift tests and an opt-in real-core test; `package_launcher.sh` now
  bundles a thin-arm64 `SoCGameCore` into a signed `.app` (artifact verified, Finder launch NOT RUN).
- **E615** — cross-process save persistence: a slot written in one process is loaded and enumerated
  in two fresh processes (`ProgressV1` + `USR-META`). Synthetic only; G2/G5 NOT RUN.
- **E616** — SAV-04 acceptance harness (`tools/dev/save_inspect.py`, `docs/SAVE_ACCEPTANCE.md`).
  **NOT RUN**: no user-owned PS4 save exists in the workspace.
- **E617** — startup cliff 296 is a 12-byte thunk into a 67 B TSC-timer constructor
  (`sceKernelGetTscFrequency`), tooling/debug and inert in this image; 297 is a 400 x 0x118 record
  initializer, 300 an inert `__cxa_atexit`, 301 the `rumbleEnabled` ConCmd. Proposed slice
  296+297+300.
- **E618** — startup slots 296/297/300 are native: 296 is a 12-byte thunk into the already-native
  TSC store `0x323d0` (object `0x334ac18`, partial frequency reported honestly), 297 is the 400 x
  0x118 record initializer (acquire/retain/clear order, exact prefixes, dtor pinned not executed),
  and 300 is an inert `__cxa_atexit` thunk. The production table is **311/573**, first gap
  **298 / `0xb0e250`**, 262 remain.
- Verification at this tip: ctest **382/382**, checker sweep **471/0/0**, publish and attribution
  audits exit 0; Swift **147** tests.

## 2026-10-24 Orchestrator round 29 — concmd slots, owner rows 3/18, launcher GUI, Metal path (UNCOMMITTED)

- Agent: integrator plus six workers. **Owner instruction: nothing from this session is committed or
  pushed.** All round-29 work below is in the working tree on top of `9f79b65`.
- **E619** (`CPU-01-CONCMD-298-299-301-01`, FIXTURE_VERIFIED) — startup slots 298/299/301 are native
  (`concmd_base`, `concmd_vars`, `debug_menu_item`, `ctor_slot_298/299/301_*`). The `0x15d5960`
  functor vtable is resolved (slot 3 = bare `ret`). 299's range max is 10.0, not 20.0 (E617
  corrected). Manager cell `[0x2dfcbc8]` and debug-menu parent `[0x2dfd4b8]` are null at startup by
  xref census (not an execution trace). Table **314/573**, first gap **302 / `0xaaeee0`**, 259 remain;
  registered-set digest `e612c3dd…d0f2`. Report: `docs/checkpoints/2026-10-24-concmd-slots-298-299-301.md`.
- **E620** — owner rows 18 (`0x3c5cb0`, 317 B) and 3 (`0x14db00`, ResourcePool ctor, 387 B) are native
  with fixtures, checkers and mutation self-tests. **E570's ctor sizes are wrong** (it fell through the
  noreturn `__stack_chk_fail`): row 8 `0x95a90` = 988 B, row 1 `0x144ca0` = 1276, row 15 `0xa42c80` =
  1374, row 20 `0xa3da30` = 1543, row 13 `0xa52530` = 1595, row 17 `0x30bef0` = 1633, row 9 `0x112db0`
  = 2141, row 4 `0x8f6f0` = 2350, row 12 `0x25fbd0` = 3701, row 5 `0x13a9a0` = 4246. `resource_dump`
  passes ConCmd flags **2** (`concmd_base.hpp:83` comment still says "every site passes 1"; fix when idle).
  `0xa5c850` is row 10 (1-based), not "row 9". 10 ctor bodies remain hooks.
- **Row 8 (`0x95a90`) is not cheap**: display/render-target config owner, ~12 hooks (Sony-import PLT
  group `0xf90060..0xf90030` unnamed), self-registers via cell `0x2ed3670`; the `&0x2eec4e0` argument is
  unused. Name the PLT stubs via `local/re/imports.json` before writing hooks.
- **Launcher (ARTIFACT_VERIFIED, UI only)** — Swift suite **158/158** (was 147). Fixed: wrong copy with
  a core present, "NOT LINKED" diagnostics with a live core, absent core indistinguishable from present
  (`CoreTransport`), VoiceOver could not read diagnostics. **Round 28's bundled core was the ASan build**
  (Xcode-rpath dependent); `scripts/package_launcher.sh` now rejects cores linking outside `/usr/lib`
  or `/System/Library`. Package the release core:
  `cmake -S native -B local/build/r29b-release -G Ninja -DSOTC_SANITIZE=OFF -DCMAKE_BUILD_TYPE=Release`
  then `scripts/package_launcher.sh local/build/launcher-app local/build/r29b-release/SoCGameCore`.
  NOT RUN: assets-ready GUI case, Finder double-click, VoiceOver listening.
- **Metal path (design, G3 NOT RUN)** — `docs/METAL_FRAME_PATH.md` + `tools/dev/gpu_asset_probe.py`:
  Gnm/Gnmx band `0x85c040..0x885f90`, PS4 backend `0x917c0..0x99720` (`PS4CRenderContext`), 132
  dispatch vs ~15 draw sites, renderer unreachable from main by direct edges (install rows 4/5/8/9 are
  the only links). 5 proposed cards (not yet in `docs/TASKS.md`); recommended first slice
  `GPU-02-GCN-SUBSET-DEFAULT-PAIR-01`, honestly labelled "asset-derived harness, not game-driven".
- **PSARC pairing defect — independently VERIFIED and FIXED in scripts (uncommitted).** TOC entries
  are sorted by MD5(path); the manifest is alphabetical; positional pairing matched 0-1 members per
  archive, MD5 pairing matches all (pak2 1,313; cachedFiles 40,213; patch 40,216; pak0 6,918).
  `scripts/psarc.py` gained `path_md5()`, `Psarc.members()`, `entry_index()`; `classify_audio_headers.py`
  and `audio_bank_header.py` use it; every `scripts/test_*.py` passes (`-I -W error`).
  Regenerated manifests sit in `local/manifests/assets-md5pairing-r30/` (NOT swapped in: swapping breaks
  the hash-bound inputs of E77/E123/E126/E239; swap is an owner decision). Corrected facts: base vs
  patch share 40,213 paths with identical TOC value, size and decoded bytes (patch adds 3 `.ctxt`
  members); the old "35,734 digest differ" figures were artifacts; `.sts_cp3` base/patch members are
  identical (120 B); TOC "value" is MD5 of the path, not a content digest; the `xff` magic belongs to
  `.anb/.nmo/.nto/.clb/.skb/.cls/.nso`, not audio banks, so **AUD-01-XFF-01 and `audio_bank_header.py`'s
  premise are void**. Still stale (not edited): EVIDENCE.md E55/E75/E77/E123/E126/E239, ASSETS.md lines
  5, 12, 49-53, 80-122, GPU01_PREP private JSONL, hash-bound `local/validation/` outputs.
- Verification (run by a worker after the slot/owner work, before the launcher/doc changes settled):
  ctest **385/385**, sweep **474/0/0**, publish audit exit 0 (2,675 paths). Swift 158/158.
  The integrator has not re-run the full suite after final settling; attribution audit run by worker only.
- Not lifted, with next actions: slot 303 (`0xa82170`, `ProfileCPU` bool + debug toggle; needs a
  `default/value` parameter on `init_bool_var_static`), 302 (ten null handles + atexit `0xaaf050`),
  305 (`0xa77b60`, guarded statics) — all analysed read-only, nothing written.

## Settled decisions (do not re-ask)

- `StartupState` RAII ownership for the slot-10 mutex — implemented.
- Testable `LauncherCore` lifecycle helper and a system alert for folder-import errors — keep.
- Offline analysis of the local `eboot.bin` is allowed; no original/Sony executable may ship or run.
- The shared descriptor helper models the 34-slot family; slots 24–47 remain in a test-only ahead
  table outside `recovered_constructors()` until production reaches and resolves slot 23.
- Native app-info entries are **borrowed string views** (`control == nullptr`) by default; callers
  keep the text alive for the registry's lifetime. This is a stated native contract, not a claim
  that the original never owns formatted values.
- The native list-3 dispatcher never calls an original RVA: native delegates are tagged `fn == 0`;
  nodes with non-zero `fn` (the identity-only `0x1290` handlers) are counted and skipped.
- The owner allows downloading/using pre-existing tools for concrete tasks. Follow the
  `AGENTS.md` tool-acquisition policy: official source, checksum, signature/architecture smoke test,
  and an `INSTALL_LOG.md` record. Never install guest runtimes, Sony SDKs, firmware or game files.

## Next exact work in the historical round-29 snapshot (superseded by the round-30 checkpoint above)

1. **Startup continuation.** Slots 296/297/300 are native (E618). Continue at index 298
   (`0xb0e250`, `shaderTypeToLoad` ConCmd; its `0x15d5960+0x18` indirect call is unresolved), then
   299 (debug-text ConCommands) and 301 (`0xac9550`, `rumbleEnabled` ConCmd). The 262 remaining
   slots are still `other`-class bodies; do not claim startup completeness.
2. **SAV-04 real-save acceptance.** The inspector is ready
   (`python3 -I tools/dev/save_inspect.py <slot-dir>`), but no user-owned PS4 save exists in the
   workspace. Run it privately on such a pair before promoting any save/load claim. G2/G5 remain
   NOT RUN.
3. **Launcher GUI integration.** The signed bundle now carries `SoCGameCore`; run a Finder launch
   and inspect the readiness/diagnostics UI with the core present, then with it absent. This is
   UI evidence only; Play must stay disabled while `engine_implemented=false`.
4. **Metal next step.** The capture provider proves the offscreen Metal/readback/PNG path with a
   synthetic pattern. The next slice is a real game frame source (renderer/scene), not another
   synthetic pattern; G3 stays NOT RUN until then.
5. **Owner rows.** Rows 14 and 21 are native. The remaining 18 ctor bodies stay hooks until lifted;
   the drain/element loops and refcount bodies remain contracts.
6. Round end: fresh build, full `ctest`, `SOTC_NATIVE_BUILD=local/build/orch-r22
   tools/lift/run_all_checkers.sh`, `python3 -I scripts/audit_publish_paths.py --all`,
   `python3 -I scripts/check_commit_attribution.py`, `python3 scripts/project_index.py`.

## Parallel-agent state

- No round-28 workers are running. All lanes (ctor coverage, SoCGameCore, Metal, owner rows 14/21,
  launcher client, startup-cliff analysis, startup slots 296/297/300) have reported; the code is
  committed and the startup slice is being integrated. Workers do not edit `HANDOFF.md` or commit.
- Builds are serialized with `tools/dev/with_build_lock.sh` because several workers share
  `local/build/orch-r22` and `native/CMakeLists.txt`.
- The preserved `origin/cursor/parallel-foundations-wip-20261023` content
  (shared_str_release_f590, save_stream exclusive temp, aud02 rackquery scripts, check_ff590) is now
  **integrated** (E566); `origin/cursor/parallel-foundations-86ac55b` was its base.
- Round 24 used six workers on disjoint files: table owner, installer semantics, 0x2569d0 mapping,
  f590 census, f590 wiring, and the wip integration. No worker touched `HANDOFF.md` or committed.
- `local/build/native` is pinned by several checkers and is stale (313 tests); keep it.
  `local/build/orch-r22` is the current full-suite tree.
- The persistent Ghidra project `local/ghidra/persistent-20261020` must never be re-imported or
  overwritten while in use; read-only `-noanalysis` reopening is the supported path.

## Private evidence pointers

- Family manifest: `local/re/work/ctor-classes.json` (per-slot span, digest, calls, class).
- Consumer packet: `local/re/evidence/cpu01-ctor10-descriptor-consumer-01/`
  (`report.md`, `verify.py`, `verification.json` 45/45, `ghidra-*.txt`, `SHA256SUMS`).
- Round-24 evidence: `local/re/evidence/appinfo-table-owner-15ff060/`,
  `local/re/evidence/appinfo-installer-25a4d0/`, `local/re/evidence/owner-installer-2569d0/`,
  `local/re/evidence/f590-callsite-holders/`, `local/re/evidence/f590-wiring/`,
  `local/re/evidence/parallel-foundations-integration/`.
- Reopen log: `local/checkpoints/root-20261020-reopen-console.log`.

