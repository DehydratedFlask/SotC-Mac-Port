# SoC Mac Native — current handoff

## 2026-10-25 04:55 CDT — task claim published to GitHub main

- Integrator: Codex `/root`; commit `bc12aed0dad90ef9e0034e70a9872fe545613128` published to
  `main`. `git fetch origin main` and `git rev-parse HEAD origin/main` confirmed equal SHAs;
  `git status --short --branch` is clean on `main`.
- The commit records the active `GPU-03-LOGO-PNG-TEXTURE-SAMPLE-01` claim, verified PR #1–#12
  merge state and safe PR #4 branch deletion, and the documentation-only gates. Attribution audit
  passed for `origin/main..HEAD` before push; author and committer are Johnny Wang
  (`dehydratedflask@users.noreply.github.com`), with no Claude/Anthropic coauthor trailer. All-path
  audit passed at 2,788 paths. No source changes or private data were committed.
- The removed branch `cursor/init-hash-551140-9dfc` is absent locally and from `git ls-remote`.
  Checkout remains on `main`; no open PRs were observed at the claim checkpoint.
- Task status remains ACTIVE, attempt 0. Exact next action: import the pinned PNG to ignored
  `local/`, integrate the test-only ImageIO helper and pinned logo shader replay, then run the focused
  Metal fixture with API/GPU validation before broad repository gates.

## 2026-10-25 04:53 CDT — task-claim publication gates pass

- Integrator: Codex `/root`; `GPU-03-LOGO-PNG-TEXTURE-SAMPLE-01` remains **ACTIVE**, attempt 0,
  with documentation only changed. No new task implementation hypothesis has been tried.
- Exact successful gates: `cmake -S native -B local/build/orch-r22` and
  `cmake --build local/build/orch-r22 -j 12` exited 0; full CTest exited 0, 397/397 passed, with
  the private logo MSL replay enabled using absolute `SOTC_LOGO_PS_MSL` and `SOTC_LOGO_PS_PNG`
  paths; `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` passed 481, skipped
  0, failed 0; `python3 scripts/project_index.py` indexed 19,590 folders; `trail build --check`
  passed with the 8,522-node graph in sync; staged/all path audits passed 3/2,788 paths; and
  `git diff --cached --check` passed. Private logs are under
  `local/validation/gpu03-logo-png-texture-20261025/docs-{configure,build,full-ctest,lift-checkers,project-index,trail-check,path-audit-staged,path-audit-all}.log`.
- One initial full CTest invocation exited 8 with 396/397 passing because the optional shader MSL
  path was relative to the repo rather than the CTest working directory. This was recorded in the
  preceding task-claim entry; rerunning with absolute local paths passed all 397 tests. No source
  change was made to address it.
- Next exact action: commit the three allowlisted docs files on `main`, run the unpublished-range
  attribution audit, then push under Johnny Wang's identity and verify `main == origin/main`. Keep
  the local asset importer, generated images, and logs ignored.

## 2026-10-25 04:52 CDT — GPU-03 logo PNG texture fixture claimed on main

- Integrator: Codex `/root`; checkout stays on `main`. Starting code head is
  `565e57efba2602dc65279ac16d5f91839f175356`, equal to `origin/main` before these documentation
  edits. PRs #1–#12 are merged and `gh pr list --state open` returned `[]`.
- The user-requested PR #4 branch cleanup is complete: GitHub reports PR #4 `MERGED` into `main` at
  `29b1305b4fd0d8fd82fc54c1a5b3cf35e0cab745`; `cursor/init-hash-551140-9dfc` is an ancestor of
  `main` (`git rev-list --left-right --count` = `0 45`), and both local and remote branch refs have
  been deleted. No worktree was attached to that branch.
- Claimed bounded task `GPU-03-LOGO-PNG-TEXTURE-SAMPLE-01`, status **ACTIVE**, attempt 0. Input is
  the already hash-pinned plain PNG member `pakN misc/boot/_ps4/saveshadow_scee.ctxr` (24,636 B,
  228x128, RGB, SHA-256
  `b4b135232304ab696785353b6ba96f874fc4bcfd8805e56b931119e415a300f5`). Import only by the
  documented exact PSARC `MD5(path)` lookup into ignored `local/`; decode to test-only RGBA8 via
  ImageIO, then sample it with the digest-gated logo PS MSL and the existing slot-0 Metal harness
  using synthetic geometry, coordinates and sampler state. Compare selected texel-center readback
  against an independent CPU oracle, require deterministic repeat and a visible copied-texel
  mutation, enable Metal API/GPU validation, and write PNG/log evidence directly under `local/`.
- Scope excludes retail texture/descriptor/UV/orientation semantics, AMD WQM/Metal helper-lane
  equivalence, `attach_screenshot`/engine frame numbers, a game-driven frame, and G3/boot/audio/save
  claims. The real asset bytes and all generated outputs stay private. The user selected direct PNG
  output and previously approved use of documented local asset imports.
- Documentation files changed for this claim: `HANDOFF.md`, `docs/TASKS.md`, and `docs/EVIDENCE.md`;
  no source files or private inputs have been changed or staged. Reconciliation updates E652 and adds
  E653 for the verified merged-PR and branch-cleanup state.
- Exact pre-edit checks: `git fetch origin` exited 0; `git status --short --branch` showed only the
  three pending documentation edits; `gh pr list --state open --json number,state,headRefName
  --limit 50` returned `[]`; `gh pr view 4 --json state,mergedAt,headRefName,baseRefName,mergeCommit`
  returned `MERGED`, base `main`, merge time `2026-10-25T07:04:42Z`; the branch-ancestry check
  returned `0 45`. `git push origin --delete cursor/init-hash-551140-9dfc` and
  `git branch -d cursor/init-hash-551140-9dfc` both succeeded. Evidence is the live GitHub response
  and git output in this session; validation results are recorded in the following checkpoint.
- First publication-gate run: `SOTC_LOGO_PS_MSL=local/validation/gpu02-logo-ps-image-sample-20261025/logo-ps.metal SOTC_LOGO_PS_PNG=local/validation/gpu02-logo-ps-image-sample-20261025/logo-ps-final.png ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure` exited 8 (396/397 passed); `metal_texture_binding` could not read the MSL because CTest resolves this environment path from its build working directory. No source changed and no shader-semantic hypothesis failed. Rerun the same suite with absolute private-input paths.
- Graft `ask` located the existing slot-0 texture renderer and private logo MSL replay; it reported
  approximately 36,527 saved tokens. Exact next action: finish the documentation claim commit on
  `main`, then implement the private PNG import/decode fixture and extend
  `native/tests/test_metal_texture_binding.cpp`; run focused Metal validation followed by the
  repository verification gates. Commit/push only allowlisted files as Johnny Wang, with no Claude
  or Anthropic coauthor.

## 2026-10-25 04:39 CDT — GPU-03 L1 fixture published to GitHub main

- Integrator: Codex `/root`; task `GPU-03-RENDEROPS-ASSET-FRAME-01` is **FIXTURE_VERIFIED** for
  the L1 asset-derived harness. Owner commit `0f67022cea442727326af7b675942929add1f4c0`
  (`test(metal): validate asset index mutations through interpolants`) is on GitHub `main`.
- Publication checks: staged-path audit 8/8, all-path audit 2,788/2,788, final
  `git diff --cached --check` passed, and `python3 -I scripts/check_commit_attribution.py` passed for
  `origin/main..HEAD` before push. The pushed commit's author and committer are Johnny Wang
  (`dehydratedflask@users.noreply.github.com`); no Claude/Anthropic coauthor trailer exists.
- Documentation closeout `565e57efba2602dc65279ac16d5f91839f175356` was then pushed to `main`;
  the latest `git fetch origin main` confirmed `HEAD == origin/main ==
  565e57efba2602dc65279ac16d5f91839f175356`, and `git status --short --branch` is clean on `main`.
- Full validation remains as recorded below and in E649/E650: CTest 397/397, lift checkers 481/0/0,
  Trail 8,522 nodes / 11,526 edges and in sync, project index 19,589 folders. No private assets,
  generated MSL, PNG, or logs were committed. G3 remains PARTIAL; no game-driven frame, boot, audio,
  or real save/load claim follows.
- Next exact action: inspect `docs/TASKS.md` for the next independent, unblocked task consistent with
  the user's broader request to complete as many tasks as possible, then claim a bounded task before
  editing shared files. Keep working on local `main` as the user requested.

## 2026-10-25 04:33 CDT — GPU-03 L1 asset-derived frame fixture passes

- Integrator: Codex `/root`; user-directed work remains on local `main`. Task
  `GPU-03-RENDEROPS-ASSET-FRAME-01` is **FIXTURE_VERIFIED** after the recorded root principal
  reassessment and fourth index-mutation hypothesis. The fourth attempt was one index change only;
  the PNG-hash diagnostic was added and the same focused attempt replayed on the final source.
- Exact commands/results: `cmake --build local/build/orch-r22 --target test_metal_asset_frame -j 12`
  passed. The asset-derived executable ran with `MTL_DEBUG_LAYER=1`, full Metal shader validation,
  and the pinned default-pair MSL; exact final output is captured in
  `local/validation/gpu03-r31/attempt4-final-asset-derived.log`. The 256x256 baseline PNG is 3,147 B,
  SHA-256 `af42ca554f5d24a9f981fc92156dea50616c172af1faba12eef1ff0d4db0f645`; covered=8,464,
  overlap=6,584, silhouette errors=0, depth errors=0. Direct output is private at
  `local/validation/gpu03-r31/asset-derived-final.png`.
- Observed: one cone index mutation (draw 1, slot 33, value 14 → 23) preserves CPU coverage and
  front-draw ownership while changing perspective-correct `param0` on 677 visible pixels. Metal
  produced a different PNG SHA-256 `e3384475049c0e174895c736edb7248c5925ea82ae3924db507efb302641432c`.
  Baseline determinism, CPU silhouette/depth/bounds checks, and vertex/index/constant mutation checks
  all passed; Metal API/GPU validation reported enabled and no faults. Build logs are
  `attempt4-build.log` and `attempt4-final-build.log` in that private validation folder.
- Files changed: `native/tests/test_metal_asset_frame.cpp`, this handoff, `docs/TASKS.md`,
  `docs/EVIDENCE.md`, `docs/INDEX.md`, `docs/escalations/GPU-03-RENDEROPS-ASSET-FRAME-01.md`, and
  new checkpoint `docs/checkpoints/2026-10-25-gpu03-asset-frame-attempt4.md`. No private inputs,
  MSL, PNG, or logs are included in the changes.
- Limits: this verifies one L1 asset-derived harness fixture from local cube/cone mesh bytes and the
  harness ABI. Retail mesh interpretation/attribute binding, WQM equivalence, a game-driven frame,
  G3, boot, audio, and save/load remain unverified; G3 stays PARTIAL.
- Full verification passed: configure and full native build; CTest 397/397 with private logo MSL;
  lift checkers 481 passed / 0 skipped / 0 failed. `trail build` processed 2,001 source files into
  8,522 nodes / 11,526 edges; `trail build --check` passed and `trail grep` found the new interpolant
  helper. Project index: 19,589 folders. Staged-path audit passed 8/8 and all-path audit 2,788/2,788.
  The first cached-diff check caught trailing Markdown hardbreak whitespace in the new checkpoint;
  those spaces were removed and a final cached-diff check is pending. Attribution currently passes
  `origin/main..HEAD` (empty unpublished range); rerun after commit. Full logs are under
  `local/validation/gpu03-r31/`.
- Next exact action: finish the cached-diff and staged audit recheck, record these full gates in
  E650, then make an owner-authored commit and push to `main` under the user's explicit publication
  instruction. Attempt count is 4; no further GPU-03 retry is planned.

## 2026-10-25 04:31 CDT — GPU-03 principal reassessment; attempt 4 claimed

- Integrator/principal: Codex `/root`; user-directed continuation on `main` after the GPU-02 Metal
  shader slice. Starting point is clean `main` at `b2f6fefe29e11234c9c8c0c53bf60bf65bfc5637`, equal
  to `origin/main`.
- Bounded task: `GPU-03-RENDEROPS-ASSET-FRAME-01`, root reassessed the three-attempt escalation and
  authorized exactly one focused fourth hypothesis. The baseline still parses the pinned local cube
  and cone and passes asset-derived CPU silhouette/depth checks; only index-mutation sensitivity is
  incomplete. G3 remains PARTIAL and no game-frame/boot/audio/save claim follows.
- New hypothesis: an individual index substitution may preserve covered pixels and front-draw
  ownership yet change the interpolated `param0` supplied to the default-pair fragment shader.
  Live private MSL inspection shows the vertex stage exports transformed position in `param0`, and
  the fragment stage consumes interpolated `param0` in its lighting calculation. The prior try-3
  candidate filter compared only coverage and front-draw masks, so it could exclude this case.
- Targeted observable: extend the independent CPU projection to retain the winning fragment's
  perspective-correct `param0`; search the existing valid one-index substitutions for one that keeps
  coverage/front-draw ownership unchanged while changing `param0` on at least one visible pixel.
  Render only such candidates and require a changed PNG SHA-256, preserving all baseline checks.
- Files claimed before editing: `native/tests/test_metal_asset_frame.cpp`,
  `docs/escalations/GPU-03-RENDEROPS-ASSET-FRAME-01.md`, `docs/TASKS.md`, and `HANDOFF.md`.
  No private raw member, MSL, PNG, or log will be staged.
- Exact next action: implement the CPU interpolant observability and new candidate filter, build
  `test_metal_asset_frame`, then run the recorded asset-derived command with Metal API/GPU
  validation. Attempt count is 4 total; no fifth attempt will run without another reassessment.

## 2026-10-25 04:26 CDT — main publication and branch cleanup verified

- Integrator: Codex `/root`; GPU-02 logo PS fixture is **FIXTURE_VERIFIED** under its exact private
  shader digest and synthetic ABI. GPU-03 is still `NEEDS_PRINCIPAL`, G3 remains `PARTIAL`.
- PR #12 is `MERGED` at `a07d45184c978bdd7a0a937be90c83f499fcca50`; its tested tree is exactly the
  second-parent tree. The owner-authored documentation closeout commit
  `9225de3e3ba719ba1e9c4135a369ee42adc9720c` was pushed to GitHub `main`.
- Immediately before this final handoff-only update, `git fetch origin main` confirmed local `HEAD`
  and `origin/main` both at `9225de3e3ba719ba1e9c4135a369ee42adc9720c`; `git status --short --branch`
  was clean; the local and remote refs for `codex/gpu02-logo-ps-image-sample-20261025` were absent.
  PR #12 remote branch removal is confirmed by GitHub.
- Owner-only attribution audit passed for the unpublished commit before push; author and committer
  are Johnny Wang. The all-path audit passed at 2,787 paths. No Claude or Anthropic contributor
  trailer was added. The source tree was unchanged after the E646 full 397-test and 481-checker pass.
- This final handoff/evidence update is documentation-only and will be published on `main`. After it
  is pushed, leave the checkout clean on `main`; no additional work in this Metal slice is pending.
  Select a different task from `docs/TASKS.md` only when directed. Do not resume GPU-03 without
  principal reassessment.

## 2026-10-25 04:22 CDT — PR #12 merged; checkout restored to main

- Integrator: Codex `/root`; `GPU-02-GCN-LOGO-PS-IMAGE-SAMPLE-01` remains
  **FIXTURE_VERIFIED** for its exact digest-pinned shader specialization and synthetic ABI. GPU-03
  remains `NEEDS_PRINCIPAL`; G3 remains `PARTIAL`.
- GitHub PR #12, `feat(metal): validate pinned logo image sampling`, merged into `main` from head
  `ed08e2b83329738113d5611b34fca30d4177ab42` as merge commit
  `a07d45184c978bdd7a0a937be90c83f499fcca50` at `2026-10-25T09:22:29Z`. GitHub removed the remote
  task branch. PR: https://github.com/DehydratedFlask/SotC-Mac-Port/pull/12.
- Local cleanup passed: `git switch main`, `git pull --ff-only origin main`,
  `git branch -d codex/gpu02-logo-ps-image-sample-20261025`, and `git remote prune origin`. The local
  task branch and `origin/codex/gpu02-logo-ps-image-sample-20261025` are absent; the source checkout
  was clean `main` at the same SHA as `origin/main` before this documentation closeout.
- The CTest/build/checker results in E646 were run on head `ed08e2b...`; the merge commit preserves
  that exact source tree. No tests or code changes were made during branch cleanup.
- This merge closeout is a documentation-only update on `main`; the implementation arrived through
  PR #12. The owner's explicit instruction to merge all changes to GitHub `main` covers publishing
  this factual closeout. Validation for the staged docs passed: project index 19,589 folders, Trail
  8,519 nodes / 11,521 edges in sync, staged/all path audits 6/2,787, `git diff --cached --check`,
  and owner identity audit. Owner-authored closeout commit is now on local `main`; the full
  unpublished-range attribution audit passes, author/committer are Johnny Wang, no coauthor trailer
  is present, and the all-path audit passes for 2,787 repository paths. Next exact action: push this
  commit to `origin/main`, then verify clean `main` and exact remote SHA.

## 2026-10-25 04:16 CDT — E646 pinned logo PS Metal replay passes

- Integrator: Codex `/root`; `GPU-02-GCN-LOGO-PS-IMAGE-SAMPLE-01` is **FIXTURE_VERIFIED** for the
  exact private shader digest and synthetic ABI. Three distinct test/oracle hypotheses failed during
  this work. Root reassessment permitted one focused fourth check; it passed. The detailed try table
  and reassessment are in `docs/escalations/GPU-02-GCN-LOGO-PS-IMAGE-SAMPLE-01.md`.
- Exact private slice: 152 B at `.csdr` offset `0x4d8`, SHA-256
  `4309caada7f114ec822f61646be536f50a8261a8c6e54f2ce709db7c70b8b39a`. The profile uses a fixed,
  digest-gated PS-only MSL template for the closed synthetic slot-0 ABI; it is not a generic
  row-by-row GCN translator. Retail descriptors/interpolation and AMD WQM/helper-lane equivalence
  remain unverified.
- Exact commands/results: `python3 -I -W error tools/dev/test_gcn_to_msl.py` passed 17/17;
  `cmake -S native -B local/build/orch-r22` and `cmake --build local/build/orch-r22 -j 12` exited 0;
  full CTest with absolute `SOTC_LOGO_PS_MSL`/`SOTC_LOGO_PS_PNG` paths passed 397/397;
  `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` passed 481/0/0;
  `trail build --check` passed at 8,519 nodes / 11,521 edges. Logs are in
  `local/validation/gpu02-logo-ps-image-sample-20261025/`.
- Observed: implicit sampling selects the expected red mip 1/green mip 2, center and primitive-edge
  pixels match the CPU oracle, blue mip mutation is visible, and tested channel multipliers plus
  fp16 RTZ outputs match. Metal API/GPU validation enabled with no fault. A direct private PNG is
  64x64, 351 B, SHA-256
  `267a344d7533777e675ba8360ca2e7cd2dcad5ff3b256d7c27f8867f4abc0fa8`.
- Files changed in this task: `native/platform/metal_asset_frame.hpp`,
  `native/platform/metal_asset_frame.mm`, `native/tests/test_metal_texture_binding.cpp`,
  `tools/dev/gcn_to_msl.py`, `tools/dev/test_gcn_to_msl.py`, `HANDOFF.md`,
  `docs/{TASKS,EVIDENCE,FOLDERS,INDEX,METAL_FRAME_PATH}.md`, this checkpoint, the E645 prerequisite
  checkpoint, and the escalation record. Private data remains ignored under `local/`.
- Principal reassessment followed the third oracle/harness failure; the fourth focused Metal replay
  passed. This task has no remaining shader fixture gate. GPU-03 remains `NEEDS_PRINCIPAL`; G3 remains
  `PARTIAL`; no game-frame, boot, audio or save claim follows.
- Final publication preflight currently passes: staged-path audit 14/14, all-path audit 2,786/2,786,
  and `git diff --cached --check`; project index is 19,589 folders and Trail wiring check is in sync.
  The first attribution invocation had no upstream configured, so it audited all history and flagged
  inherited base commit `f6604a86e60e` (`orchestrator@local`). After setting this task branch's
  upstream to `origin/main`, `python3 -I scripts/check_commit_attribution.py` passed for
  `origin/main..HEAD`; the only unpublished commit is by Johnny Wang as author and committer, with no
  coauthor trailer. After push, rerun attribution with the explicit `origin/main..HEAD` range.
  Next exact action: push/create a PR, merge the reviewed exact head to GitHub `main`, then checkout
  clean `main` and delete this local task branch. No direct push to `main`.

## 2026-10-25 04:07 CDT — mip input rejection cases pass

- Integrator: Codex `/root`; task `GPU-02-GCN-LOGO-PS-IMAGE-SAMPLE-01` remains ACTIVE, attempt 1.
  Added fail-visible C++ checks for noncanonical mip dimensions, truncated mip bytes, and unordered
  LOD clamps. `python3 scripts/project_index.py` exited 0 and indexed 19,589 folders.
- Exact commands/results: `cmake --build local/build/orch-r22 --target test_metal_texture_binding -j 12`
  passed 2/2 (`local/validation/gpu02-logo-ps-image-sample-20261025/metal-test-build-rerun3.log`);
  `ctest -V --test-dir local/build/orch-r22 -R '^metal_texture_binding$'` passed 1/1 without a
  no-device skip (`local/validation/gpu02-logo-ps-image-sample-20261025/mip-fixture-ctest-rerun3.log`).
  API/GPU validation was enabled with no Metal faults. The mip selection, center/edge oracle,
  texture mutation, and malformed-input checks pass.
- Observation: the texture path rejects bad mip dimensions/byte counts and invalid LOD ordering
  before issuing Metal draws. The pinned generated logo MSL path is still NOT RUN because the
  lowerer implementation is in progress.
- Files changed since the previous checkpoint: `native/tests/test_metal_texture_binding.cpp`,
  `HANDOFF.md`, `docs/EVIDENCE.md`, and this checkpoint. No private shader bytes were staged.
- Next exact action: review the lowerer worker's code and tests, produce private MSL from the pinned
  code, and run CTest with `SOTC_LOGO_PS_MSL` plus direct PNG output configured. No shader-semantic
  hypothesis failed; one byte-rounding oracle attempt was corrected. G3 remains PARTIAL.

## 2026-10-25 04:06 CDT — synthetic mip LOD fixture passes

- Integrator: Codex `/root`; task `GPU-02-GCN-LOGO-PS-IMAGE-SAMPLE-01` is ACTIVE on
  `codex/gpu02-logo-ps-image-sample-20261025`, attempt 1. The mip API worker's changes preserve
  legacy aggregate initialization and compile; root's test-file changes add a seven-level synthetic
  RGBA8 chain, implicit-vs-explicit fragment sample, and optional private generated-logo MSL replay.
- Exact commands/results: focused target build
  `cmake --build local/build/orch-r22 --target test_metal_texture_binding -j 12` passed 2/2 (log
  `local/validation/gpu02-logo-ps-image-sample-20261025/metal-test-build-rerun2.log`); focused
  `ctest -V --test-dir local/build/orch-r22 -R '^metal_texture_binding$'` passed 1/1, not skipped
  (log `local/validation/gpu02-logo-ps-image-sample-20261025/mip-fixture-ctest-rerun2.log`). Metal
  API and GPU validation were enabled, with no validation fault. `SOTC_LOGO_PS_MSL` was unset, so
  generated pinned-logo MSL execution is explicitly NOT RUN in this pass.
- Observation: the mip-distinct synthetic MSL returns the expected red mip at UV scale 1 and green
  mip at scale 2, at both center and primitive boundary. Mutating the selected level to blue changes
  readback as the CPU oracle predicts. The diagnostic half-gradient RGBA8 channel reads 127 at 0.5;
  the CPU oracle now uses observed truncation and passes.
- Correction/resolution: the 04:04/04:05 failure was byte quantization and stale error wording, not a
  mip-selection mismatch. Failed shader-semantic hypotheses: 0. Total unsuccessful fixture/oracle
  attempts: 1. E645 records this prerequisite fixture; no logo MSL conversion or retail ABI fact is
  inferred. G3 remains PARTIAL.
- Files changed so far: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/METAL_FRAME_PATH.md`,
  `docs/INDEX.md`, this checkpoint, `native/platform/metal_asset_frame.hpp`,
  `native/platform/metal_asset_frame.mm`, and `native/tests/test_metal_texture_binding.cpp`.
- Next exact action: integrate the dedicated closed-world logo PS lowerer from `/root/logo_ps_lowerer`,
  generate source from the private pinned code under `local/`, and run this same CTest with
  `SOTC_LOGO_PS_MSL` set. Preserve direct PNG output under local evidence if configured. No game frame
  or retail-sampling claim.

## 2026-10-25 04:05 CDT — readback narrows the mip-oracle mismatch to byte quantization

- Integrator: Codex `/root`; parent task `GPU-02-GCN-LOGO-PS-IMAGE-SAMPLE-01` remains ACTIVE on
  `codex/gpu02-logo-ps-image-sample-20261025`, attempt 1. Root build
  `cmake --build local/build/orch-r22 --target test_metal_texture_binding -j 12` passed; log:
  `local/validation/gpu02-logo-ps-image-sample-20261025/metal-test-build-rerun1.log`.
- Focused CTest was rerun as
  `ctest -V --test-dir local/build/orch-r22 -R '^metal_texture_binding$'`; it exited 8 and is
  captured at `local/validation/gpu02-logo-ps-image-sample-20261025/mip-fixture-ctest-rerun1.log`.
  Metal API/GPU validation were enabled and reported no Metal faults. Pixel diagnostics show the
  implicit and explicit samples matched expected mip colors at the interior and primitive boundary;
  all three remaining failures are only blue diagnostic channel `127` versus oracle `128` at
  gradient `0.5`. The old truncated-input error-string assertion now passes.
- Observation: the tested input gradients selected the expected discrete mip level, including the
  selected-level texture mutation. The current CPU oracle rounds `0.5*255` to 128; this Metal
  RGBA8 render target produced 127. Inference: the test oracle should model the observed 8-bit
  conversion as truncation for this output path; no AMD WQM equivalence is implied.
- Correction to the 04:04 checkpoint: the initial log did not include channel values and tentatively
  called the mismatch a shader-semantic hypothesis. The diagnostic rerun found no mip selection
  disagreement; reclassify the one failed attempt as a byte-quantization oracle assumption. Failed
  shader-semantic hypotheses remain 0; total unsuccessful fixture attempts: 1.
- Files changed remain `HANDOFF.md`, `docs/TASKS.md`, the mip-aware platform API/Metal backend, and
  `native/tests/test_metal_texture_binding.cpp`. No private shader bytes or generated MSL are in the
  repository. No dependency changes.
- Next exact action: update only the CPU diagnostic-byte conversion to match observed RGBA8
  quantization, rerun the focused CTest, then add the private generated-logo MSL replay once the
  lowerer worker returns. G3 remains PARTIAL.

## 2026-10-25 04:04 CDT — mip-sensitive fixture first run needs oracle reconciliation

- Integrator: Codex `/root`; worker subtask `/root/asset_texture_mips` changed only
  `native/platform/metal_asset_frame.hpp` and `.mm`. It adds optional explicit RGBA8 mip levels and
  mip sampler controls while preserving the old aggregate defaults. Worker command
  `cmake --build local/build/orch-r22 --target test_metal_texture_binding -j 12` passed 4/4 compile/
  link steps; its output was in the worker tool transcript. Root replay of the same build after
  beginning the test-file edit failed because the new fixture symbols were not yet wired; the log is
  `local/validation/gpu02-logo-ps-image-sample-20261025/api-build.log`. After wiring the fixture,
  the target rebuilt successfully; exact log:
  `local/validation/gpu02-logo-ps-image-sample-20261025/metal-test-build.log`.
- Focused `ctest -V --test-dir local/build/orch-r22 -R '^metal_texture_binding$'` exited 8; log:
  `local/validation/gpu02-logo-ps-image-sample-20261025/mip-fixture-ctest.log`. Metal API and GPU
  validation were enabled with no reported Metal fault. Four assertions failed: an old truncated-
  input error-string expectation and the new mip oracle at the center/boundary plus selected-mip
  texture mutation. The underlying output pixel values were not yet printed, so the cause remains
  unknown and is not being declared an implementation defect or a pass. The stale error expectation
  has since been updated; `is_color` now prints actual/expected channels for the next run.
- Initial accounting: 1 attempted fixture hypothesis, later reclassified by the 04:05 diagnostic
  checkpoint as a byte-quantization oracle assumption. It is not a failed shader-semantic
  hypothesis. The Metal mip API was compile-verified; the output-level result is summarized in the
  newer checkpoint above.
- Observation: the one-mip derivative checks ran, but the combined CTest still failed. Inference:
  either the independent footprint oracle or the Metal mip setup/filter contract needs correction;
  inspect logged channel values before choosing.
- Files changed across this checkpoint: `HANDOFF.md`, `docs/TASKS.md`,
  `native/platform/metal_asset_frame.hpp`, `native/platform/metal_asset_frame.mm`, and
  `native/tests/test_metal_texture_binding.cpp`. No private shader bytes or generated MSL have been
  added to the repository. No dependency changes.
- Next exact action at the time: rebuild and rerun focused CTest with the improved pixel diagnostic.
  The newer 04:05 checkpoint records that resolution. Parent task remains ACTIVE; G3 remains PARTIAL.

## 2026-10-25 03:56 CDT — logo pixel IMAGE_SAMPLE lowering claimed

- Integrator: Codex `/root`. Starting point was clean `main` and `origin/main` at
  `376c080fa2664497f0626871cacb8536a154a6f7` (PR #11 merge). GitHub has no open PRs. The prior
  derivative-fixture remote ref is already deleted; only a stale local tracking ref was present.
- Bounded task: `GPU-02-GCN-LOGO-PS-IMAGE-SAMPLE-01`, ACTIVE, attempt 0. Work is on
  `codex/gpu02-logo-ps-image-sample-20261025`, created from that `main` commit.
- Scope: extend the offline lowerer for the E634-pinned 152-byte/34-instruction logo pixel shader's
  WQM/save/restore and one base `IMAGE_SAMPLE`, emitting an ordinary PS-only MSL fragment against
  an explicitly synthetic coordinate/texture/sampler ABI. Extend the existing test-only Metal
  texture path to mip levels and verify implicit-sample LOD with a mip-distinct synthetic texture,
  partial quad coverage, and an independent CPU oracle. Require exact input digest/instruction
  pattern and fail visibly on unsupported changes. No retail binding or AMD/Metal equivalence claim.
- Ruling: extend the existing test-only texture/sampler API instead of adding a parallel Metal test
  target. This reuses the direct-Metal fixture and API/shader validation setup; the cost if wrong is
  coupling two synthetic fixtures that may later need separate lifecycles.
- User-selected constraints carried forward: default shader pair first, harness normals for fixture
  geometry, and direct private PNG output where image evidence is needed. No engine frame number is
  introduced. G3 remains PARTIAL; no gameplay, asset-binding, or game-frame claim.
- Next exact action: inspect the live lowerer/test code spans, then implement the pinned PS-only
  conversion and mip-sensitive fixture. Before stopping or changing task state, record exact test
  commands/results, logs, limitations, and files in a checkpoint/EVIDENCE/TASKS update.
- Initial claim changed `HANDOFF.md` only; the mip API subtask and Metal fixture changes are recorded
  by the later checkpoint above. No dependency change.
- Observation: `main` is clean at the recorded hash and PR #11 is merged. Inference: the existing
  derivative fixture is a useful Metal validation host for this next isolated shader slice.
- Risks: the private logo code binds an unverified synthetic ABI; Metal implicit LOD is not proof of
  AMD WQM equivalence. Failed shader-semantic hypotheses: 0.
- Status: ACTIVE; attempt 0. G3 remains PARTIAL.

## 2026-10-25 03:52 CDT — Metal derivative fixture publication gates pass

- Integrator: Codex `/root`; `GPU-02-METAL-FRAGMENT-QUAD-DERIVATIVE-01` is
  `FIXTURE_VERIFIED`, 0 failed hypotheses. The seven staged allowlisted files pass the staged path
  audit; the all-path audit passes 2,782 paths; staged and committed diff checks pass.
- Commit attribution audit `python3 -I scripts/check_commit_attribution.py --range origin/main..HEAD`
  passes. The commit author and committer are Johnny Wang
  (`dehydratedflask@users.noreply.github.com`); no Claude, Anthropic, or `Co-Authored-By` trailer
  is present. Full logs remain under `local/validation/gpu02-fragment-derivative-20261025/`.
- Next exact action: push `codex/gpu02-fragment-derivative-fixture-20261025`, open a PR against
  `main`, merge only the exact verified head, fetch/switch to `main`, delete the local task branch,
  and verify clean `main` equals `origin/main`. The one-mip derivative fixture remains limited as
  described in E644; no G3 or gameplay claim.

## 2026-10-25 03:51 CDT — Metal derivative fixture full verification complete

- Integrator: Codex `/root`; `GPU-02-METAL-FRAGMENT-QUAD-DERIVATIVE-01` remains
  `FIXTURE_VERIFIED`, 0 failed hypotheses. The source test and E644 docs are recorded in the section
  below.
- Full configure `cmake -S native -B local/build/orch-r22` passed. Full build
  `cmake --build local/build/orch-r22 -j 12` exited 0 (`ninja: no work to do`); the changed target
  had already compiled 2/2. Full CTest passed 397/397. Lift checkers passed 481/0/0.
- The first `trail build --check` exited 1 because the graph was stale for the changed test file
  (two changed spans, one added `gradient_oracle` symbol). `trail build` exited 0 after parsing that
  one file and produced 8,491 nodes / 11,466 edges; the repeated `trail build --check` exited 0.
  The graph is a git-ignored local cache and is not staged. `python3 scripts/project_index.py`
  passed, indexing 19,588 folders. Logs are under
  `local/validation/gpu02-fragment-derivative-20261025/`.
- Staged publication audit passed 7 paths; all-path audit passed 2,783 paths; staged and committed
  diff checks and commit attribution passed. The owner authored and committed the change; no Claude
  attribution was introduced.
- Next exact action: publish the branch through an exact-head PR, merge, and return to a clean
  `main`. No dependency changes; no game claim.

## 2026-10-25 03:48 CDT — Metal derivative fixture passes focused gate

- Integrator: Codex `/root`; task `GPU-02-METAL-FRAGMENT-QUAD-DERIVATIVE-01` is
  `FIXTURE_VERIFIED`, 0 failed hypotheses. Changed `native/tests/test_metal_texture_binding.cpp`,
  `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/FOLDERS.md`, `docs/INDEX.md`, and added
  `docs/checkpoints/2026-10-25-gpu02-metal-quad-derivative-fixture.md`; private logs are in
  `local/validation/gpu02-fragment-derivative-20261025/`.
- Baseline `python3 -I -W error tools/dev/test_gcn_to_msl.py` exited 0 (14/14). Focused build
  `cmake --build local/build/orch-r22 --target test_metal_texture_binding -j 12` exited 0 (2/2);
  `ctest -V --test-dir local/build/orch-r22 -R '^metal_texture_binding$'` exited 0 (1/1), with
  Metal API and shader validation enabled and no validation fault. `git diff --check` passed.
- Observation: the synthetic affine quad's CPU oracle matched Metal `dfdx`/`dfdy` values and the
  implicit/explicit texture samples at an interior pixel and a primitive boundary. Changing the
  input UV scale from 0.5 to 0.375 and changing the texel red value from 64 to 65 both changed the
  output as expected. The exact test MSL and output are in the source and private logs above.
- Limit: the fixture uses a one-mip constant synthetic texture; it cannot expose derivative-driven
  LOD selection. It does not lower the pinned logo shader or establish AMD WQM equivalence, retail
  binding semantics, an asset-derived frame, or G3 PASS. No game assets were read.
- No game-frame claim; shader-semantic failed tries: 0; no dependency changes.

## 2026-10-25 03:44 CDT — Metal fragment derivative fixture claimed

- Integrator: Codex `/root`; bounded task `GPU-02-METAL-FRAGMENT-QUAD-DERIVATIVE-01` is ACTIVE,
  attempt 0. This follows the user-selected default shader pair (E627) and the static logo WQM
  characterization (E634); it does not reopen escalated GPU-03.
- The clean starting point was `main` / `origin/main` at
  `8229b5405b9441ff79c0d55cf3d178e4e84014bb`. The task branch
  `codex/gpu02-fragment-derivative-fixture-20261025` was created with `git switch -c ...` (exit 0).
- Bounded output: extend the existing direct-Metal texture-binding fixture with synthetic varying
  fragment coordinates, explicit `dfdx`/`dfdy` sampling gradients, and a comparison between implicit
  and explicit-gradient sampling against a separate CPU expectation at an interior pixel and a
  primitive boundary. Keep the synthetic texture/sampler ABI explicit; this one-mip fixture proves
  neither derivative-driven LOD selection, AMD WQM equivalence, nor game bindings.
- Acceptance: Metal shader compilation and API/GPU validation succeed; derivative/sample pixels
  match independent expected values, including a helper-lane edge case; mutations to the input
  gradient or texel change the expected readback; focused CTest passes without a no-device skip.
  No game-frame or G3 PASS claim.
- Baseline command `python3 -I -W error tools/dev/test_gcn_to_msl.py` passed 14/14; output is in
  `local/validation/gpu02-fragment-derivative-20261025/lowerer-baseline.log`. No source change or
  failed hypothesis has occurred yet.
- Next exact action: add the bounded Metal fixture and its focused assertions in
  `native/tests/test_metal_texture_binding.cpp`, then run the focused target under Metal validation.
  Record the exact outputs before deciding whether to attempt pinned logo `image_sample` lowering.
- No dependency changes. Remaining risks: hardware helper-lane behavior and conversion from AMD's
  whole-quad sequence to MSL derivatives remain unverified. Failed tries: 0.

## 2026-10-25 03:34 CDT — PR #9 closeout reconciled; final record on main

- Integrator: Codex `/root`; task `INT-OPEN-PRS-1-2-3-5-01` is `FIXTURE_VERIFIED`. PR #8's
  integration merge is recorded at `61c96f6`; follow-up PR #9
  (`docs: close out integration PR8 and branch cleanup`) merged at `2026-10-25T08:31:56Z` with
  merge commit `a8bd805c48867b378a22efa6ed15e41c29583dbe`. PR #9's source branch was
  `codex/integration-closeout-20261025` and GitHub removed it.
- Immediately before this documentation closeout, `main` and `origin/main` both resolved to
  `a8bd805c48867b378a22efa6ed15e41c29583dbe`; the checkout was clean on `main` and GitHub reported
  no open PRs. The captured live queries and refs are in
  `local/validation/root-pr9-final-closeout-20261025/`.
- PR #9 changed documentation only and did not alter code or validation results. The exact PR #8
  tree remains verified by CTest 397/397 and lift checkers 481/0/0, with publication audits and
  attribution checks recorded in E642. GPU-03 remains `NEEDS_PRINCIPAL` after three failed
  index-mutation hypotheses; G3 remains PARTIAL. No cold boot, real game audio, or save/reload is
  claimed.
- The PR #10 documentation-only candidate passes `cmake -S native -B local/build/orch-r22`, a full
  build (700/700), CTest (397/397), and lift checkers (481/0/0). `trail build --check` passes at
  8,490 nodes / 11,465 edges; `python3 scripts/project_index.py` indexes 19,587 folders; the
  staged-path audit passes 6 paths and the all-path audit passes 2,782 paths. `git diff --cached
  --check` and `python3 -I scripts/check_commit_attribution.py --range origin/main..HEAD` pass;
  the commit author/committer is Johnny Wang and no Claude/Anthropic coauthor trailer is present.
  Logs are under `local/validation/root-pr9-final-closeout-20261025/`.
- This closeout checkpoint reconciles the PR #9 merge into the handoff/task/evidence indexes. The
  publication branch is `codex/integration-final-closeout-20261025`; after its exact-head PR merges,
  fetch `main`, switch/pull it, verify a clean checkout, and delete only that local branch. Then
  continue the next unblocked task from `main`; do not retry GPU-03 without principal reassessment.
- Failed tries: 0. No dependency changes. Checkpoint: [E643](docs/checkpoints/2026-10-25-integration-pr9-final-closeout.md).

## 2026-10-25 03:30 CDT — PR #8 merged; integration branch removed

- Integrator: Codex `/root`; task `INT-OPEN-PRS-1-2-3-5-01` is `FIXTURE_VERIFIED`. PR #8
  (`integration: reconcile reviewed work and publish current checkpoints`) merged at
  `2026-10-25T08:29:00Z`; reviewed head `8f57b64e58622ba181e797701ed407cd38e64cef`, merge commit
  `61c96f6de128b53a5970a33ed504346e47ce42f2`. GitHub deleted remote branch
  `codex/merge-all-open-changes` through `--delete-branch`.
- `git fetch origin main`, `git switch main`, `git pull --ff-only origin main`, and
  `git branch -d codex/merge-all-open-changes` all exited 0. `HEAD` and `origin/main` both resolve to
  `61c96f6de128b53a5970a33ed504346e47ce42f2`; `git status --short --branch` is clean on `main`.
  `git ls-remote` returned no `codex/merge-all-open-changes` ref.
- On the merged PR head, CMake configure/build passed, CTest passed 397/397, and lift checkers passed
  481/0/0. `trail build --check` passed at 8,490 nodes / 11,465 edges; project index: 19,586
  folders. Staged audit passed 41 paths before merge, all-path audit 2,780 paths, PR-diff audit 11
  paths, and attribution/diff checks passed. Logs are under
  `local/validation/root-merge-integration-20261025/`.
- PR #8 also includes the E641 checker build-directory fix. GPU-03 asset-frame remains
  `NEEDS_PRINCIPAL` after three failed index-mutation hypotheses; G3 remains PARTIAL. No cold boot,
  real game audio, or save/reload is claimed.
- Exact next action: continue on `main` with the next unblocked task. Do not resume GPU-03 without a
  principal reassessment recorded in its escalation. No dependency changed; integration task attempt
  count: 0.

## 2026-10-25 03:27 CDT — main merge committed; publication audits pass

- Integrator: Codex `/root`; `INT-OPEN-PRS-1-2-3-5-01` remains ACTIVE until the integration PR is
  merged and the workspace returns to `main`. GitHub has PRs #1–#7 MERGED and no open PRs. Current
  main is `a1f21e62fe25294ac70d6753be7ab18322d7a169`; the integration merge keeps its E637 App-01 and
  E638 allocator evidence, with root asset import/frame records at E639/E640.
- Native configure/build passed; CTest passed 397/397. The first full lift-checker run failed one
  checker because it ignored `SOTC_NATIVE_BUILD` and read a stale five-case binary. After making the
  default fixture path honor that variable, the targeted checker and full lift checkers passed
  (481 passed / 0 skipped / 0 failed). The fix is recorded as E641 in
  `tools/lift/check_f19e00_accounting.py` and
  [its checkpoint](docs/checkpoints/2026-10-25-srv01-f19e00-build-dir-env.md). The old-binary failure is retained at
  `local/validation/root-merge-integration-20261025/stale-fixture-path-repro.log`; passing logs are
  in the same directory. No original ELF code was executed.
- Merge commit `1a8319a7667eff5a1e322f8232ce9f4e7e647af8` has parents root checkpoint
  `01e6019ed8fa24acb6c8fdebf0ffe30585eaec43` and latest main
  `a1f21e62fe25294ac70d6753be7ab18322d7a169`; it is authored and committed by Johnny Wang. The
  integration branch is clean locally and 29 commits ahead of its previous remote tip.
- After the final checker-source edit, `trail build` indexed 2,001 files into 8,490 nodes / 11,465
  edges and 1,496 cards; `trail build --check` passed. `trail grep 'bootstrap_allocator_hierarchy'`
  found 14 hits in 10 symbols across 5 files. `python3 scripts/project_index.py` indexed 19,586
  folders after the new checkpoint was added. Staged path audit passed 41 paths; full path audit
  passed 2,780 paths; the committed PR diff path audit passed 11 paths. The post-commit staged-only
  audit found nothing staged, so it returned its documented empty-index error; the earlier 41-path
  staged audit and 11-path committed-diff audit passed. Both diff checks pass after removing trailing
  spaces caught in the first check. `check_commit_attribution.py --range origin/main..HEAD` passes
  for the merged head.
- Root GPU-03 asset frame remains `NEEDS_PRINCIPAL`: parser and baseline frame observations are
  recorded, but index-mutation sensitivity failed three hypotheses. G3 remains PARTIAL; no boot,
  real game audio, save/reload, or game-driven frame claim.
- Exact next action: push `codex/merge-all-open-changes`, create and review its integration PR against
  `main`, merge the exact verified head, then switch to `main` and delete the local and remote
  integration branch. Do not resume GPU-03 without principal reassessment. INT task remains ACTIVE,
  attempt count 0.

## 2026-10-25 03:18 CDT — latest main reconciliation in progress

- Integrator: Codex `/root`; task `INT-OPEN-PRS-1-2-3-5-01` remains ACTIVE until its changes are
  published and merged. GitHub reports PRs #1–#7 MERGED and no open PRs. PR #5 merged normally at
  `2026-10-25T08:14:28Z` from exact head `6b10496e4a0ff36f3b80b78e7c44cd43391d77c8`; merge commit
  `a1f21e62fe25294ac70d6753be7ab18322d7a169`.
- Root checkpoint commit `01e6019ed8fa24acb6c8fdebf0ffe30585eaec43` records the GPU-03 harness and
  three failed index-mutation hypotheses. `git merge --no-edit origin/main` fetched latest main at
  `a1f21e62fe25294ac70d6753be7ab18322d7a169` and stopped with conflicts in `HANDOFF.md`,
  `docs/EVIDENCE.md`, and `docs/INDEX.md`. Resolution preserves main's E637 App-01 and E638 allocator
  evidence, and assigns the root GPU records E639 (raw private member import) and E640 (asset-frame
  attempt). The separate PR #5 implementation and validation details remain in
  [its checkpoint](docs/checkpoints/2026-10-25-pr5-allocator-rollback-fixed-quantum.md).
- `git status` shows this merge is still in progress. Files changed by the root checkpoint:
  `HANDOFF.md`, `docs/EVIDENCE.md`, `docs/FOLDERS.md`, `docs/INDEX.md`, `docs/TASKS.md`, the GPU-03
  import checkpoint and escalation, and `native/tests/test_metal_asset_frame.cpp`; current main also
  brings its already-reviewed PR changes. No dependency changed.
- Observation versus inference: GitHub PR state and the `origin/main` SHA are live checks. GPU-03's
  parser/baseline Metal observations are recorded, but index-mutation acceptance failed on try 3;
  task remains `NEEDS_PRINCIPAL`. No cold boot, real game audio, or save/reload claim follows.
- Next exact action: finish the documentation resolution, run the full publication gates on the
  merged tree, update the index/checkpoint with exact results, commit the merge under Johnny Wang,
  push this integration branch, publish and merge its PR, then switch this workspace to `main` and
  delete local/remote `codex/merge-all-open-changes`. GPU-03 itself must not resume without principal
  reassessment. Attempt count for the integration task: 0.

## 2026-10-25 02:56 CDT — PR #2 merged to current main

- Integrator: Codex `/root`; task `INT-OPEN-PRS-1-2-3-5-01` remains ACTIVE. PR #2 was marked ready
  and merged through GitHub with exact head `f19251df002a94195e23c2f1755441e6772d91cc` using
  `gh pr merge 2 --match-head-commit f19251df002a94195e23c2f1755441e6772d91cc --merge`; exit 0.
  GitHub reports MERGED at `2026-10-25T07:56:01Z`, merge commit
  `92f2d5c7660daf8581997370a9290c5ec34acd0e`. `git fetch origin main` and `git ls-remote origin
  refs/heads/main` both confirm current main at this SHA.
- The PR branch was reconciled through merge commits `d560bd1` (main `8bd3d10`) and `f19251d`
  (main `271e656`). Current diff was six intended paths (158 insertions / 25 deletions); tests on the
  latest-main candidate: architecture 7/7, CoreLocator 14/14, AssetValidator 11/11, full launcher
  153 tests / 20 suites with one opt-in protocol test skipped. Publish audits accepted 8 staged and
  2,756 total paths. Attribution had no Claude/Anthropic trailer. No GitHub checks/review decision
  were listed.
- Main now advanced from `271e656` to `92f2d5c`. PR #5 head `25bfcfe` and the independent E28 checker
  candidate based at `271e656` need refresh against `92f2d5c`. PR #5's two reviewed fixes are present
  only in `/tmp/sotc-pr5-fix`; focused allocator tests pass 2/2, including exact two-free-slot and
  preoccupied-table retry cases. A fresh full CTest/checker run is still required after those final
  test additions. E28 checker parity is split into its own small PR candidate.
- Files changed in this checkpoint: `HANDOFF.md`, `docs/TASKS.md` locally; main contains PR #2. No
  dependency change. Next exact action: update the E28 checker branch and PR #5 branch with latest
  main, run final gates, refresh PR descriptions, merge through PRs, then integrate the root branch.
  PR #2 complete; overall integration remains ACTIVE, attempt count 0.

## 2026-10-25 02:50 CDT — PR #1 merged to current main

- Integrator: Codex `/root`; task `INT-OPEN-PRS-1-2-3-5-01` remains ACTIVE. PR #1 was refreshed on
  latest main, marked ready, and merged with exact expected head
  `815c6e3d9b08c0145ed64cc96e63fcf3df8527d8` using
  `gh pr merge 1 --match-head-commit 815c6e3d9b08c0145ed64cc96e63fcf3df8527d8 --merge`; exit 0.
  GitHub reports MERGED at `2026-10-25T07:50:51Z`, merge commit
  `271e656474cf54a84d7d68d0474ba861d1706168`. `git fetch origin main` and `git ls-remote origin
  refs/heads/main` both confirm that exact current main SHA.
- PR #1 diff was four intended files (test, TASKS, EVIDENCE, checkpoint), 118 insertions. Focused
  Swift tests 4/4; full launcher 152 tests / 20 suites with the opt-in core-protocol test skipped;
  diff and path audits passed. `python3 -I scripts/check_commit_attribution.py --range
  origin/main..HEAD` passed on the candidate; commits were authored under Johnny Wang without
  Claude/Anthropic attribution. GitHub had no attached checks or review decision.
- Current main advanced from `8bd3d10` to `271e656`; PR #2 head `d560bd1` and PR #5 head `25bfcfe`
  now need another merge of latest main before either can be merged. The PR #5 P2 fixes are in its
  isolated candidate only and pass focused tests; full CTest is 396 passed / 1 opt-in asset test
  skipped, and lift checkers are 481/0/0 with the separately scoped E28 case temporarily present.
- Files changed in this checkpoint: `HANDOFF.md`, `docs/TASKS.md` locally; GitHub main contains PR
  #1. No dependency change. Next exact action: merge current main into PR #2 and PR #5 candidates,
  preserve their branch ancestry, re-run focused gates and refresh PR summaries; decide the separate
  E28 checker fixture before final PR #5 gates. PR #1 complete; overall integration task ACTIVE,
  attempt count 0.

## 2026-10-25 02:45 CDT — PR #2 conflict resolved against current main

- Integrator: Codex `/root`; task `INT-OPEN-PRS-1-2-3-5-01` remains ACTIVE. `origin/main` is still
  `8bd3d101a497db42f032829067b995657f046011`.
- Isolated candidate `/tmp/sotc-pr2-integration` had PR branch
  `a1791381bf20cc53af3bbf7dbaf6aea34599d642` merged with latest main, no unresolved paths. Created
  merge-only commit `d560bd182efbc3b2de515cf721165de9661833fe` as Johnny Wang
  (`dehydratedflask@users.noreply.github.com`) using
  `git -c user.name='Johnny Wang' -c user.email='dehydratedflask@users.noreply.github.com' commit -m 'merge: reconcile launcher PR with current main'`; exit 0. Pushed with
  `git push origin HEAD:refs/heads/cursor/app-honesty-next-5d26`; exit 0. No Claude/Anthropic
  contributor trailer was added.
- Live PR #2 is OPEN, base `8bd3d10`, head `d560bd1`, `MERGEABLE` / `CLEAN`, no review decision or
  GitHub checks listed. The candidate retains exactly six intended paths (152 insertions, 25
  deletions): evidence/task/checkpoint plus `CoreLocator.swift`, `SystemChecks.swift`, and the
  architecture tests. Architecture tests 7/7, CoreLocator 14/14, AssetValidator 11/11, full launcher
  suite 149 tests / 19 suites. Staged/full publish audits accepted 925/2,754 paths; attribution passed.
  Logs: `/private/tmp/sotc-pr2-integration/local/orchestrator/pr2-main-integration/`.
- `trail build --check` reports `NO GRAPH` in the isolated candidate; source graph availability is
  the only recorded candidate limitation. No dependency changed. Because PR #1 also updates shared
  docs, merge ordering is deferred until its refreshed candidate is ready, then recheck PR #2's
  base/head and diff before merge.
- Files changed in this checkpoint: `HANDOFF.md`, `docs/TASKS.md`; remote PR branch changed by the
  merge-only commit. Next exact action: review refreshed PR #1 candidate, merge it if clean, then
  merge PR #2 after a current-head check. PR #2 conflict gate is resolved; task remains ACTIVE,
  attempt count 0.

## 2026-10-25 02:41 CDT — PR #5 conflict resolved against current main

- Integrator: Codex `/root`; task `INT-OPEN-PRS-1-2-3-5-01` remains ACTIVE. Latest verified
  `origin/main` is `8bd3d101a497db42f032829067b995657f046011`.
- In isolated candidate `/tmp/sotc-pr5-fix`, created merge-only commit
  `25bfcfe68b091de9c6b83b4bf3e025fd56a00d4e` under Johnny Wang
  (`dehydratedflask@users.noreply.github.com`) with exact command
  `git -c user.name='Johnny Wang' -c user.email='dehydratedflask@users.noreply.github.com' commit -m 'merge: reconcile allocator PR with current main'`; exit 0.
  Pushed with `git push origin HEAD:refs/heads/cursor/heap-depth-5d26`; exit 0. No Claude or
  Anthropic trailer was added. The commit has parents PR tip `545003f8f7089bab478e8e6aa7064a86527e67cd`
  and current `main` `8bd3d101a497db42f032829067b995657f046011`.
- Live `gh pr view 5 --json state,headRefOid,baseRefOid,mergeable,mergeStateStatus,...` reports
  OPEN, head `25bfcfe`, base `8bd3d10`, `MERGEABLE` / `CLEAN`, no review decision and no checks
  listed. Remote refs confirmed by `git ls-remote origin`; conflict gate is resolved. PR body still
  has stale base/tip text and needs refresh after the reviewed code fixes and final gate results.
- Only the merge tree was committed; isolated candidate source followups remain unstaged. Review
  found two P2 correctness issues in allocator bootstrap retry handling and configurable heap
  quantum enforcement, plus a one-line E28 checker case outside PR #5 scope. No fix was applied yet;
  a bundled approach question is pending with the owner. Earlier candidate verification remains
  limited to its recorded results and is not a verification of this merge commit.
- Files changed in this checkpoint: `HANDOFF.md`, `docs/TASKS.md`. No dependency change. Evidence:
  live PR #5 URL `https://github.com/DehydratedFlask/SotC-Mac-Port/pull/5`; merge commit above.
- Next exact action: apply the owner-selected fixes to the PR #5 candidate, separate the unrelated
  E28 checker case if still out of scope, rerun the full publication gates, refresh the PR summary,
  then merge only after reviewing the exact current head. Status: conflict resolved; PR #5 remains
  open pending code fixes/review and final verification; attempt count for those fixes is 0.

## 2026-10-25 02:34 CDT — GPU-03 index-mutation gate escalated after try 3

- Integrator: Codex `/root`; task `GPU-03-RENDEROPS-ASSET-FRAME-01` is now `NEEDS_PRINCIPAL` after
  three distinct failed index-mutation selection hypotheses. Per `AGENTS.md`, no fourth try is
  authorized automatically; the full test gate remains failed and task work stops here.
- Exact latest command: the `MTL_DEBUG_LAYER=1`, `MTL_SHADER_VALIDATION=1` reproducer in
  `docs/escalations/GPU-03-RENDEROPS-ASSET-FRAME-01.md` — exit 1; the index check failed with
  `asset index mutation search: CPU-visible candidates=0`. Log
  `local/validation/gpu03-r31/asset-derived-frame.log` records try 3. Output PNG remains private at
  `local/validation/gpu03-r31/asset-derived-cube-cone.png`.
- Verified positive results: raw member sizes/hashes match E639; `parse_cmsh_mesh` returns cube 24/36
  and cone 42/114 position/index counts; baseline Metal validation enabled with no fault reported;
  PNG SHA-256 `af42ca554f5d24a9f981fc92156dea50616c172af1faba12eef1ff0d4db0f645`; CPU silhouette and
  depth-class errors 0; 6,584 overlapping pixels; vertex and material-constant mutations change the
  image. The required index mutation did not pass. These observations are recorded as E640 only for
  parser/baseline-artifact facts, not full task completion.
- Failed attempts and precise repro details are in
  `docs/escalations/GPU-03-RENDEROPS-ASSET-FRAME-01.md`; `docs/TASKS.md` is marked `NEEDS_PRINCIPAL`.
  Files changed: `native/tests/test_metal_asset_frame.cpp`, `HANDOFF.md`, `docs/TASKS.md`,
  `docs/EVIDENCE.md`, `docs/INDEX.md`, and the escalation file. No dependency or public game asset
  changed. Raw inputs, build, PNG and validation log remain under ignored `local/`.
- Independent work that continues: resolve the current GitHub PR conflict and reconcile PRs #1, #2,
  and #5 with the current main, then publish the integration ledger via PR. Do not resume GPU-03 without
  a principal reassessment recorded in its escalation.
- Status: GPU-03 L1 incomplete / `NEEDS_PRINCIPAL`; baseline frame partial evidence recorded; overall G3
  remains PARTIAL, not PASSED.

## 2026-10-25 02:33 CDT — second GPU-03 index-mutation hypothesis failed

- Integrator: Codex `/root`; `GPU-03-RENDEROPS-ASSET-FRAME-01` remains ACTIVE at attempt 2. The
  existing raw parse, Metal frame, silhouette, overlap and depth evidence still passes.
- Exact command: the `MTL_DEBUG_LAYER=1` / `MTL_SHADER_VALIDATION=1` asset-derived invocation recorded
  at 02:31, with the rebuilt test binary; exit 1, only `asset index mutation changes PNG hash` failed.
  Current log: `local/validation/gpu03-r31/asset-derived-frame.log`.
- Failed hypothesis: choosing one of the eight cone triangles with the most CPU-visible pixels and
  replacing a corner with one of four farthest projected vertices would change the private PNG. The
  baseline remains SHA-256 `af42ca554f5d24a9f981fc92156dea50616c172af1faba12eef1ff0d4db0f645`, with CPU
  silhouette errors 0, depth errors 0, and 6,584 overlap pixels. No Metal validation fault was
  reported. The cause of the unchanged image remains unknown.
- Files changed since prior checkpoint: `native/tests/test_metal_asset_frame.cpp`, `HANDOFF.md`,
  `docs/TASKS.md`; no dependency or public asset change.
- Remaining risk: index-mutation hash sensitivity has failed two distinct selection hypotheses. One
  further task-level try remains before the escalation rule.
- Next exact action: search the CPU-visible triangles of the exposed red cube draw, choose a corner
  whose replacement with a distant valid cone/cube vertex changes the predicted front-draw mask, then
  run one corresponding Metal draw and require the PNG hash to change. If no supported index mutation
  passes after that try, stop GPU-03 index-mutation work and escalate per `AGENTS.md`.
- Status: parser and baseline asset-derived Metal frame verified; index-mutation sub-gate FAILED twice;
  task ACTIVE, 2 failed hypotheses.

## 2026-10-25 02:31 CDT — GPU-03 raw mesh Metal probe exposed a mutation-selection miss

- Integrator: Codex `/root`; task `GPU-03-RENDEROPS-ASSET-FRAME-01` remains ACTIVE at attempt 1
  (one failed test-selection hypothesis). The test target rebuilt successfully after adding a private
  `--asset-derived` mode to `native/tests/test_metal_asset_frame.cpp`.
- Exact targeted command: `env MTL_DEBUG_LAYER=1 MTL_SHADER_VALIDATION=1
  MTL_SHADER_VALIDATION_DEFAULT_STATE=all MTL_SHADER_VALIDATION_ENABLE_ERROR_REPORTING=1
  MTL_SHADER_VALIDATION_REPORT_TO_STDERR=1 MTL_SHADER_VALIDATION_ABORT_ON_FAULT=1
  SOTC_DEFAULT_PAIR_MSL=local/validation/gcn-default-root-v1/default-pair.metal
  local/build/orch-r22/test_metal_asset_frame --asset-derived
  local/orchestrator/gpu03-r31/asset-derived/cube_1m.cmsh
  local/orchestrator/gpu03-r31/asset-derived/cone.cmsh
  local/validation/gpu03-r31/asset-derived-cube-cone.png` — exit 1 because the first index-mutation
  search (slots 0–11, replacement deltas 1–4) did not change the PNG hash. Log:
  `local/validation/gpu03-r31/asset-derived-frame.log`.
- Observations: both raw inputs passed pinned size/SHA checks and `parse_cmsh_mesh` (cube 24 positions /
  36 indices; cone 42 / 114). Metal API and GPU validation enabled; no validation fault reported.
  Baseline PNG SHA-256 `af42ca554f5d24a9f981fc92156dea50616c172af1faba12eef1ff0d4db0f645`; CPU silhouette
  errors 0, depth-class errors 0, and projected overlap 6,584 pixels. The test failed only the
  index-mutation check.
- Failed hypothesis: changing one of the first 12 cone index slots to one of the next four vertex
  indices would affect a visible pixel. It did not. Do not count the parser, frame, or Metal path as
  failed; those checks passed. No evidence yet identifies whether those slots were hidden, equivalent,
  or otherwise raster-invariant.
- Files changed: `native/tests/test_metal_asset_frame.cpp`, `HANDOFF.md`, `docs/TASKS.md`. No dependency
  or tracked asset changed; raw meshes, PNG, and log remain under ignored `local/`.
- Next exact action: choose an index from a triangle with large CPU-projected area and replace one corner
  with a distant in-range vertex; rerun the same Metal validation command and require a changed PNG
  hash. After the three mutations pass, record E640 and update the GPU-03 task status without claiming
  a game-driven frame or G3 PASS.
- Status: raw member parse and baseline L1 frame are partially verified; mutation acceptance FAILED on
  the first selection hypothesis; 1 failed try, no NEEDS_PRINCIPAL threshold yet.

## 2026-10-25 02:25 CDT — PR #3 post-merge fixture correction merged

- Integrator: Codex `/root`; task `INT-PR3-PUBLISH-PATH-REGRESSION-01` is complete after the owner-
  authored follow-up PR #6 merged. Main is now `8bd3d101a497db42f032829067b995657f046011` (GitHub
  merge commit for `codex/pr3-postmerge-fix`, head `94ed6ed1cdfbc6e27f4cd2cd5400ccc00c904ab8`).
- The fix changes only three positive test paths in `scripts/test_audit_publish_paths.py` from exact
  forbidden `reference`/`logs` components to lookalike names. It preserves the path-policy
  implementation and allows the intended nested lookalikes.
- Exact checks on branch `codex/pr3-postmerge-fix` at base `aee9b0e`: `python3 -I
  scripts/test_audit_publish_paths.py` 12/12; `python3 -I scripts/test_psarc.py` 53/53;
  `python3 -I scripts/audit_publish_paths.py --all` 2,753 paths; staged publish-path audit 1 path;
  `git diff --check` passed; commit-range attribution passed. PR #6 had no GitHub checks attached and
  was merged normally. Commit `94ed6ed` is Johnny Wang, with no Claude/Anthropic trailer.
- Observation versus inference: the old PR3 positive fixtures failed against the existing policy;
  after renaming only fixture path components, both Python suites pass. No production code changed.
- Files changed for the follow-up: `scripts/test_audit_publish_paths.py` on PR #6; this root docs
  branch now records the merge in `HANDOFF.md`, `docs/TASKS.md`, and a checkpoint file. No dependency
  or game asset change.
- Next exact action: merge latest `origin/main` (`8bd3d10`) into private PR #1 candidate `28df7ed`,
  rerun all launcher and Python suites, refresh the PR body, mark ready, then merge PR #1. PR #2 and #5
  remain open; PR #5 has two checker blockers. Afterwards reconcile this docs branch and return the
  checkout to `main`, deleting `codex/merge-all-open-changes` only after its changes are merged.
- Status: PR #6 MERGED; PR #3 regression fixed; PR #1 current-main refresh pending; overall integration
  ACTIVE.

## 2026-10-25 02:23 CDT — PR #3 post-merge test regression found

- Integrator: Codex `/root`; task `INT-PR3-PUBLISH-PATH-REGRESSION-01` is ACTIVE at attempt 0. This
  is a current-main regression found while refreshing PR #1; do not merge #1 until the merged-main
  Python suite is green.
- Exact current-main command: `python3 -I scripts/test_audit_publish_paths.py` on
  `origin/main` `aee9b0e1e92a477389555a560d256a0dc62afaee` exited 1 (11/12 passed). The sole failure is
  `test_nested_lookalike_names_still_allowed`, whose fixtures use `docs/reference/notes.md`,
  `tools/logs/build-notes.txt`, and `scripts/reference/helpers.py`. The existing policy rejects exact
  `reference` and `logs` path components at any depth, so these positive fixtures contradict the
  current audit rule. `python3 -I scripts/test_psarc.py` passed 53/53 on the same main tip.
- The proposed correction is test-only: rename those fixture components to `reference-notes`,
  `logs-build`, and `references`, preserving both the forbidden-component policy and the intent to
  allow lookalike names under allowlisted roots. The separate PR3 candidate has a tested local diff;
  `scripts/test_audit_publish_paths.py` passed 12/12 there, but that diff is not on main.
- Observation versus inference: the main-tip test failure and error messages are observed; the fixture
  names are the mismatch. No production audit code change is indicated by the current evidence.
- Files changed for this claim: `HANDOFF.md`, `docs/TASKS.md`; the hotfix branch has not yet been
  created. No dependency change or source asset access.
- Risk: amending the already-merged PR3 through an unrelated PR #1 branch would mix review scopes and
  mask this regression; keep the one-file correction isolated in its own follow-up PR.
- Next exact action: branch from live `origin/main`, apply only the three fixture-name corrections,
  run PSARC and publish-path tests plus path/attribution audits, then push/merge the fix as Johnny Wang.
  After that, update and test PR #1 against the new main tip, merge it, and later return this checkout
  to `main` and delete the merged `codex/merge-all-open-changes` branch.
- Status: `FAILED` current-main merged test (1/12); fix not yet applied; 0 failed implementation
  hypotheses.

## 2026-10-25 02:20 CDT — live main refreshed; PR #1 conflict candidate update pending

- Integrator: Codex `/root`; the user directed us to resolve the GitHub PR conflict first, preserve
  current evidence, then return the checkout to `main` and delete the merged working branch. Task
  `INT-OPEN-PRS-1-2-3-5-01` remains ACTIVE at attempt 0.
- Live GitHub state changed during this work: `git fetch origin` advanced `origin/main` from
  `29b1305b4fd0d8fd82fc54c1a5b3cf35e0cab745` to `aee9b0e1e92a477389555a560d256a0dc62afaee`, a
  GitHub merge commit for PR #3. PR #3 is now MERGED; it was merged externally during this turn. PR #1
  was updated by pushing `26f7f99b67722af0c79d6c7e1f2f4a682996958e`, but this candidate was based on
  the old main `29b1305`; do not mark #1 ready until current `origin/main` is merged into it and tested.
- PR #1 candidate evidence on the old base: E393 was inserted after E392 without dropping E394–E636;
  current diff was four intended files / 106 insertions. Full launcher suite: 152 tests in 20 suites;
  focused `PlayBlockedSummaryTests` 4/4; publish-path audits 932 staged / 2,755 full. Native
  configure/build succeeded, CTest 392/392, lift checkers 476 passed / 0 skipped / 0 failed, shader
  lowerer 14/14 and inventory 23/23. These native checks ran before PR #3 advanced main; its diff only
  changed Python tests, which are being rerun against the live tip.
- Attribution: the whole-history audit first flagged inherited commit `f6604a86e60e`
  (`orchestrator <orchestrator@local>`). The unpublished `origin/main..HEAD` range passed after the
  PR #1 merge commit: both commits are by Johnny Wang and contain no Claude/Anthropic trailer. Do not
  rewrite inherited main history.
- A candidate-local `python3 scripts/project_index.py` unexpectedly reported 3,629 folders and changed
  the candidate's generated `docs/FOLDERS.md`; I restored that worktree copy, leaving its staged
  merged-main file intact. Running the required command from this repository root reported 13,937
  folders and made no tracked `docs/FOLDERS.md` change.
- GPU-03 private asset import also completed: `python3 -I
  local/orchestrator/gpu03-r31/asset-derived/import_raw_cmsh.py` used `Psarc.entry_index` and
  `Psarc.read` for exact MD5(path)-mapped members. `cube_1m` entry 31789 is 850 B, SHA-256
  `44738dbf663af252898bad5582f2b48f1aadf6fbaab403c20880410b79a380b7`; `cone` entry 5574 is 1,462 B,
  SHA-256 `5456afadabe96b9f7f02c503dc0ed5410652ccd32601c19e8aeeaaf36c88600c`. Outputs remain under
  `local/orchestrator/gpu03-r31/asset-derived/`; they have not yet been parsed or rendered.
- Files changed on this docs branch: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/INDEX.md`,
  and `docs/checkpoints/2026-10-25-gpu03-raw-cmsh-import.md`; private importer and raw mesh files are
  ignored under `local/`. No source asset was changed and no dependency was installed.
- Remaining risks: PR #1 must be retested on current main; PR #2 still has CoreLocator rejection-copy
  failures; PR #5 has unresolved checker failures; PR #3's merged tests must be run at current main.
  Asset parsing and Metal rendering remain NOT RUN; L1 does not imply a game frame or G3 PASS.
- Next exact action: merge new `origin/main` into the private PR #1 candidate, rerun focused/full
  launcher and path checks, refresh the PR description, then merge #1 through GitHub. Reconcile the
  remaining PRs and this docs branch. Switch the checkout to `main` and delete
  `codex/merge-all-open-changes` only after its changes are merged.
- Status: PR #3 MERGED; PR #1 update pushed but stale-base refresh pending; GPU-03 import
  `ARTIFACT_VERIFIED`; integration ACTIVE.

## 2026-10-25 02:20 CDT — GPU-03 raw asset inputs located

- Integrator: Codex `/root`; task `GPU-03-RENDEROPS-ASSET-FRAME-01` remains ACTIVE at attempt 0.
  Root continues its already-claimed L1 Metal work while isolated agents reconcile PRs #1, #3, and #5.
- Exact commands: `rg --files --hidden --no-ignore local/extracted | rg 'pak0|cachedFiles|\\.psarc$|\\.cmsh$'`
  (exit 0; private `pak0.psarc` and `cachedFiles.psarc` are present);
  `graft ask "Metal asset frame test seam: construct AssetMeshDraw, invoke direct Metal, and persist PNG privately" --source --in native/`
  (exit 0; found the existing `MetalAssetFrameRenderer`/PNG seam); `git status --short --branch`
  (only the integration claim docs are modified).
- Source evidence: `docs/METAL_FRAME_PATH.md` section 4.6 pins cachedFiles
  `coredata/enginesupport/models/_cmn/cube_1m.cmsh` (850 B,
  `44738dbf663af252898bad5582f2b48f1aadf6fbaab403c20880410b79a380b7`) and pak0
  `coredata/enginesupport/models/_cmn/cone.cmsh` (1,462 B,
  `5456afadabe96b9f7f02c503dc0ed5410652ccd32601c19e8aeeaaf36c88600c`). The documented `Psarc`
  API exposes exact-path `entry_index`, which uses the corrected MD5(path) pairing; the native parser
  and existing direct-Metal renderer seam are already in the active source tree.
- Owner directions applied: the identified raw `.cmsh` members may be read into the private parser /
  Metal harness; use the explicit harness normal `[0.5, 0.5, 1.0]`; write the resulting PNG directly
  under `local/`; do not pass it through `attach_screenshot` or assign an engine frame number.
- Observation versus inference: both archives are present under `local/extracted/CUSA08809/`; the
  expected file sizes/digests/counts are documented probes, not yet rechecked against newly imported
  bytes. No raw member has yet been written or parsed, and no asset-derived render has run.
- Files changed at this checkpoint: `HANDOFF.md` only; no code, source assets, dependencies, or
  generated outputs changed.
- Remaining risks: the parser's real-member format/marker assumptions must match both raw members;
  CPU pixel assertions and Metal validation may reveal render/coordinate issues. This work can only
  establish L1 asset-derived harness evidence; G3 stays PARTIAL.
- Next exact action: use `Psarc.entry_index` + `Psarc.read` to import the two named raw members into
  `local/orchestrator/gpu03-r31/asset-derived/`, verify size and SHA-256, then feed those bytes to
  `parse_cmsh_mesh` and the test renderer with visible failure on any import/parse mismatch.
- Status: input discovery `ARTIFACT_VERIFIED`; GPU-03 L1 still ACTIVE; 0 failed hypotheses.

## 2026-10-25 02:13 CDT — open PR integration claimed

- Integrator: Codex `/root`; task `INT-OPEN-PRS-1-2-3-5-01` is ACTIVE at attempt 0. The owner
  explicitly authorized merging all reviewable changes to GitHub `main`; publication will use PRs,
  with no direct push to `main`.
- Exact preflight commands: `git fetch origin` (exit 0); `gh pr list --state open --json
  number,title,headRefName,headRefOid,baseRefName,isDraft,mergeStateStatus,url` (exit 0; open #1, #2,
  #3, #5); `git status --short --branch && git log -1 --oneline && git remote -v` (clean on
  `codex/merge-all-open-changes` at `cbb7a54`, origin points to the authorized GitHub repo).
- Evidence/logs: PR #1 preflight Swift tests passed 4/4 at
  `local/orchestrator/github-merge-all/pr1-current-branch.log`; full PR/worktree review is pending.
  PR #4 is already merged at main `29b1305`. Orphan history audit found both parallel-foundations
  refs patch-equivalent to PR4/main except for one WIP bookkeeping/comment-only commit; their raw
  histories are not merge candidates.
- Observation versus inference: the open PR list and fetched refs are live state. PR1's changed test
  and evidence are the only candidate changes reported so far; PR2 has a compatibility/test issue on
  merged main; PR3 has fixtures that conflict with current path policy; PR5 requires attempt-2
  allocator fixes and full revalidation. No additional PR is ready to merge based on these interim
  observations.
- Files changed for this claim: `HANDOFF.md`, `docs/TASKS.md`; no source code or dependencies.
- Risks: candidate tests may expose additional main-branch regressions; each PR must retain current
  evidence numbering, pass applicable tests and publication/attribution audits, and contain no
  Claude/Anthropic coauthor trailer. Asset/game data must remain private.
- Next exact action: reconcile #1, #2, #3, and #5 in isolated worktrees against current `origin/main`,
  run their covering and required full checks, then commit/push only allowlisted branch updates as
  Johnny Wang and merge each reviewed PR. Publish this checkpoint branch through a PR after the
  integration results are recorded.
- Status: ACTIVE / root; 0 failed implementation hypotheses.

## 2026-10-25 02:05 CDT — PR #4 merged to main

- Integrator: Codex `/root`; GitHub repository `DehydratedFlask/SotC-Mac-Port`. PR #4 was refreshed
  with current E636 status, marked ready, and merged after the owner explicitly requested merging all
  changes to `main`. Head `8a93c0319a6eeb59a06f691ded7e18f818e4960c` is now on `main` through merge commit
  `29b1305b4fd0d8fd82fc54c1a5b3cf35e0cab745`; GitHub reports state `MERGED`, and `git ls-remote`
  confirms the same SHA at `refs/heads/main`. PR #4 now covers 428 commits and 925 changed files.
- The PR body/title were updated before merge to identify E636, the current local checks, and the
  explicit G1/G3 `PARTIAL` boundary. No GitHub check runs were attached; local verification on the
  exact head passed: CTest 392/392; lift checkers 476 passed / 0 skipped / 0 failed; shader lowerer
  14/14 and inventory 23/23. PR #4 source/history attribution audit found no Claude/Anthropic
  co-author trailers. The merge commit is authored by the repository owner and contains no such
  trailer.
- `python3 scripts/project_index.py` indexed 9,045 folders; `trail build --check` passed with 8,311
  wiring nodes in sync (deep meaning tier remains unbuilt).
- PR #4's body preserves the older E490 summary as historical context. No cold boot, real game audio,
  save/reload, or complete asset-derived game frame was claimed; G1 and G3 remain PARTIAL.
- Other open PR state at this checkpoint: #1 is conflicting in `docs/EVIDENCE.md`; #2 and #3 are
  independent, reviewable changes with their focused tests recorded but no GitHub checks; #5 is held
  because review found alignment-contract and CompositeAllocator bookkeeping defects. Separate remote
  branches `cursor/parallel-foundations-86ac55b` and `cursor/parallel-foundations-wip-20261023` also
  diverge from `main` and have no open PR; do not fold them in without reviewing their unique changes.
- Files changed for this checkpoint: `HANDOFF.md`, `docs/INDEX.md`, generated `docs/FOLDERS.md`, and
  `docs/checkpoints/2026-10-25-pr4-merged.md`. No source or dependency change.
- Exact publication observations are logged under `local/orchestrator/github-merge-all/`. `main` was
  fetched locally after the merge and matched GitHub's reported merge SHA.
- Next exact action: finish the PR #5 alignment/bookkeeping fix in its isolated worktree; then update
  and test PR heads against current `main`, merge #1–#3 and #5, and audit the unassociated foundation
  branches before deciding whether they contain additional requested changes.
- Status: overall merge-all request ACTIVE; PR #4 complete; remaining PR/branch audit incomplete.

## 2026-10-25 02:03 CDT — GPU-02 logo vertex fetch lowering verified

- Integrator: Codex `/root`; active branch `cursor/init-hash-551140-9dfc`. Task
  `GPU-02-GCN-LOGO-VS-FETCH-01` is `FIXTURE_VERIFIED`, attempt 1, with 0 failed shader-semantic
  hypotheses. `/root/gpu_next_lane` implemented the lowerer/tests; root independently ran the private
  ABI/oracle/Metal replay.
- Source change: `tools/dev/gcn_to_msl.py` accepts explicit float2/float3/float4 formatted MUBUF
  fetches, preserves descriptor/index/offset/flags checks, and writes exactly the fetched component
  count. `tools/dev/test_gcn_to_msl.py` tests widths, retained trailing VGPRs, and fail-visible ABI
  mismatches. No generated MSL or game bytes were added to the public tree.
- Pinned input: `local/fixtures/gpu-r30/logo-vs.bin`, 140 B, SHA-256
  `430f9adbb07d387c0b2b877b98300396f183faeab7f75cba0c9b0c67ada20c0c`; 25 instructions, no unknown
  instructions, exact end. The two XY fetches and one XYZW fetch use only a private synthetic float
  ABI. The default pixel shader is a synthetic execution sink, not the logo PS.
- Exact commands/results: `python3 -I -W error tools/dev/test_gcn_to_msl.py` (14/14),
  `python3 -I -W error tools/dev/test_gcn_inventory.py` (23/23),
  `python3 -I local/orchestrator/gpu02-logo-vs-fetch/run_private_metal.py` (exit 0; Apple M4 Pro;
  Metal API/GPU validation enabled; 1,024 cases; generated MSL compiled; compute and offscreen
  execution complete; `fastMath=NO`; 12,288 components exact against the separate CPU oracle, 0
  mismatches). Generated MSL SHA-256 `316b5505512c6f377415f486d7e5c4abd983a9aff56061efb107c14ab800226d`.
  Full verification: CMake configure/build exit 0; CTest 392/392; lift checkers 476 passed / 0 skipped /
  0 failed. Logs are under `local/orchestrator/gpu02-logo-vs-fetch/`.
- A first synthetic width-1 fixture used an unsupported opcode and was rejected by decode before
  reaching the lowerer; the test was corrected to use a valid width-2 opcode with an
  `explicit_float1` ABI mismatch. This was fixture construction correction, not a shader-semantic
  failure. No semantic hypothesis failed.
- Observation versus inference: MSL for the pinned shader bytes matches the independent oracle under
  this declared synthetic stream ABI; this does not establish retail descriptor values, semantic
  attribute names, interpolation state, actual mesh bindings, logo pixel shader WQM/MIMG semantics,
  game rendering, boot, audio, or saving. G3 remains PARTIAL.
- Files changed: `tools/dev/gcn_to_msl.py`, `tools/dev/test_gcn_to_msl.py`, `docs/TASKS.md`,
  `docs/EVIDENCE.md`, `docs/METAL_FRAME_PATH.md`, `docs/INDEX.md`, generated `docs/FOLDERS.md`, this
  HANDOFF, and `docs/checkpoints/2026-10-25-gpu02-logo-vs-fetch-complete.md`. No dependency change;
  `INSTALL_LOG.md` unchanged.
- User resolved the next GPU-03 L1 boundaries: raw identified local `.cmsh` bytes may be passed to the
  parser/private Metal harness; use explicit harness normals `[0.5, 0.5, 1.0]`; write a direct private
  PNG without `attach_screenshot` or an invented engine frame number. Those asset checks have not run.
- Next exact action: finish the owner-requested GitHub PR audit/repairs, run publication audits, and
  merge the reviewable PRs into `main`. PR #5 currently has a confirmed alignment-contract defect and
  remains held for correction. After that publication step, start the authorized GPU-03 raw-mesh L1
  frame task.
- Publication of this checkpoint is pending pre-commit audits.

## 2026-10-24 16:38 CDT — E635 publication verified

- The synthetic texture/sampler fixture is published on `cursor/init-hash-551140-9dfc` as
  `17844d8fad183554f75214dc18d391fdb95eacf3`. `git ls-remote` matches local HEAD; worktree is clean.
- Commit author and committer are both Johnny Wang. The commit message contains no Claude, Anthropic,
  or co-author trailer; post-push `check_commit_attribution.py` passed. Exact output is in
  `local/orchestrator/gpu03-texture-sampler/post-push-verify.log`.
- The task remains `FIXTURE_VERIFIED`, not asset-derived or G3-passed. G3 remains PARTIAL.
- Next exact action: read the current GPU shader handoff, index, roadmap, task registry and lowerer
  specification, then claim a bounded offline logo vertex-shader fetch-format lowering task. Keep
  pixel-shader WQM/MIMG, real texture bindings, and the unresolved `.cmsh`/NRM0/screenshot choices out
  of that task.

## 2026-10-24 16:40 CDT — GPU-02 logo vertex fetch lowering claimed

- Integrator: Codex `/root`; active branch `cursor/init-hash-551140-9dfc`, starting revision
  `73d485b8abd037a4bbde479c8ab73317c221d195`. Task
  `GPU-02-GCN-LOGO-VS-FETCH-01` is ACTIVE, attempt 0, zero failed semantic hypotheses.
- Scope: extend the offline MUBUF lowerer for explicit float2/float3/float4 formatted vertex streams;
  add synthetic component-count/binding tests; lower and Metal-execute the pinned 140-byte logo vertex
  program using a private, explicitly synthetic input ABI and compare vertex outputs to a separate
  CPU oracle. Exact program digest `430f9adbb07d387c0b2b877b98300396f183faeab7f75cba0c9b0c67ada20c0c`.
- The pinned logo VS has two `buffer_load_format_xy` and one `buffer_load_format_xyzw`; the current
  lowerer accepts only `explicit_float3`. This is a one-vertex-per-case, straight-line offline lowering
  task. The private fixture is `local/fixtures/gpu-r30/logo-vs.bin`; no game bytes, per-asset
  instruction table, or generated MSL may enter the public tree.
- Preflight replay: `python3 -I local/orchestrator/gpu02-logo-vs-fetch/claim_replay.py` now exits 0,
  confirms the exact 140 B program hash/25 instructions/zero unknowns/exact end, and reports MUBUF
  code offsets `0x34` XY, `0x3c` XY, `0x44` XYZW. Its log and payload-free report are under
  `local/orchestrator/gpu02-logo-vs-fetch/`. The first replay printed all code offsets as `0x0` because
  a report key named `offset` overwrote the instruction offset with the immediate value; exit was 0,
  so the mismatch was caught by inspection, not an assertion. The private failure note records this;
  the corrected report uses `instruction_offset` and asserts the expected offsets. No shader-semantic
  hypothesis failed and task attempt count remains 0.
- Claim-doc checks: `python3 scripts/project_index.py` indexed 9,043 folders; staged publish-path
  audit passed 6 paths; full publish-path audit passed 2,752 paths; attribution audit passed.
  Exact audit logs are in `local/orchestrator/gpu02-logo-vs-fetch/`.
- Task claim is published as `6e60c96046fbe87db8ea24c070b75c02564bf826`; remote SHA matches local HEAD,
  worktree is clean, author/committer are Johnny Wang, and post-push attribution audit passed. Log:
  `local/orchestrator/gpu02-logo-vs-fetch/claim-push-verify.log`.
- Publication-record refresh preflight: staged path audit passed 2 paths, full path audit passed
  2,752 paths, attribution audit passed; logs use the `published-` prefix in the same private folder.
- Explicit boundary: the fixture ABI supplies float stream fields only; it does not recover retail
  descriptors, semantic attribute names, or PS4 interpolation/SPI state. No logo pixel shader, WQM,
  MIMG, texture, original mesh, `.cmsh`/NRM0, or screenshot seam is part of this task. G3 remains PARTIAL.
- Next exact action: implement and test only the lowerer/test-file scope after this claim is published;
  root independently prepares the private logo-VS manifest, CPU oracle, Metal replay, and review. Workers
  do not commit or push.

## 2026-10-24 16:37 CDT — GPU-03 texture/sampler fixture completed and rechecked

- Integrator: Codex `/root`; active branch `cursor/init-hash-551140-9dfc`. Task
  `GPU-03-RENDEROPS-TEXTURE-SAMPLER-01` is `FIXTURE_VERIFIED`: one successful implementation, two
  corrected compile-only harness checks, zero failed shader/resource semantic hypotheses. Parent
  `GPU-03-RENDEROPS-ASSET-FRAME-01` remains ACTIVE; G3 remains PARTIAL.
- The user-selected default material shader pair was already implemented and independently verified
  in E627 before this task. E632/E633 cover its synthetic direct-Metal geometry paths. This task adds
  typed `BindTexture`/`BindSampler` host-handle operations and a harness input for a single RGBA8Unorm
  2D texture plus explicit min/mag filter and U/V address mode, bound to fragment slot 0. The renderer
  rejects mismatched/missing resources, bad dimensions/byte counts, and unknown sampler modes.
- Synthetic test MSL sampled a 2x1 RGBA8 texture with linear filtering and clamp-to-edge at the known
  midpoint; the center readback was exactly `(50,70,90,255)`. Repeat RGBA/PNG output matched byte for
  byte, changing one texture texel changed the sampled color and PNG, and missing/truncated/unknown
  sampler inputs failed visibly. Metal API Validation and GPU Validation both logged enabled; no
  validation diagnostic appeared. This is host binding evidence only: `gcn_to_msl.py` still rejects
  WQM/MIMG, and no `image_sample` lowering or AMD/Metal derivative equivalence is claimed.
- Independent review found texture descriptors could be autoreleased outside a pool in the CLI harness.
  `make_texture_objects()` now creates them inside `@autoreleasepool`; focused build/CTest and the full
  suite were rerun afterward. The reviewer found no further actionable issue.
- Two early build checks failed at the harness boundary and were corrected: the first test revision
  included an Objective-C Metal header from a C++ translation unit; the second exposed
  `-Wmissing-field-initializers` for new optional draw inputs. The exact command and diagnoses are in
  `local/orchestrator/gpu03-texture-sampler/first-build-failure.txt`. The corrected target build then
  passed; there was no third failed attempt or shader/resource semantic hypothesis failure.
- Post-review verification: configure exit 0; focused target build exit 0; `render_ops` +
  `metal_texture_binding` CTest **2/2**; full build exit 0; full CTest **392/392**;
  `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` **476 passed / 0 skipped /
  0 failed**. Logs are under `local/orchestrator/gpu03-texture-sampler/post-review-*`.
- Trail was rebuilt with the existing `scripts`, `tools`, and `reference` scopes plus `native`,
  `launcher`, and `tests`. `trail build --check` passed with **8,306** wiring nodes, and its file API
  returned the fixture's `main`. The deep meaning tier remains unbuilt. `python3 scripts/project_index.py`
  indexed **9,043** folders. Staged publication path audit passed with **14 paths**; full path audit
  passed with **2,751 paths**; commit-attribution audit passed for the current
  `origin/cursor/init-hash-551140-9dfc..HEAD` range. These are the final pre-commit audit results.
- Files changed: `native/CMakeLists.txt`, `native/platform/{render_ops.hpp,render_ops.cpp,
  metal_asset_frame.hpp,metal_asset_frame.mm}`, `native/tests/test_render_ops.cpp`, new
  `native/tests/test_metal_texture_binding.cpp`, `docs/TASKS.md`, `docs/EVIDENCE.md`,
  `docs/METAL_FRAME_PATH.md`, `docs/INDEX.md`, generated `docs/FOLDERS.md`, this HANDOFF, and
  `docs/checkpoints/2026-10-24-gpu03-texture-sampler-complete.md`. No package/dependency was installed.
- Remaining risks and boundaries: texture resource data is explicit one-mip RGBA8Unorm harness input;
  retail texture format/tiling, logo material texture relationship, descriptor values, sampler state,
  coordinates/attribute ABI, and AMD WQM to Metal derivative equivalence remain unknown. No original
  asset bytes were read; real `.cmsh`, NRM0 and screenshot-seam choices remain open. No game frame,
  boot, audio or saving claim is made.
- Publication verification is recorded in the 16:38 CDT checkpoint above.
- Status: texture/sampler binding child task complete as `FIXTURE_VERIFIED`; parent GPU-03 ACTIVE;
  G3 PARTIAL.

## 2026-10-24 16:21 CDT — GPU-03 texture/sampler binding fixture claimed

- Integrator: Codex `/root`; active branch `cursor/init-hash-551140-9dfc`, clean at claim time at
  `e5981d81f7d9b6e14d93e2a31a25cdc420245c2d`. Task
  `GPU-03-RENDEROPS-TEXTURE-SAMPLER-01` is ACTIVE, attempt 0, zero failed implementation hypotheses.
- The user-selected default material pair is already `FIXTURE_VERIFIED / ARTIFACT_VERIFIED` in E627;
  E632/E633 exercise it through synthetic direct-Metal geometry and parser flow. This bounded follow-up
  adds the next missing renderer seam: typed fragment `BindTexture`/`BindSampler` operations and a
  synthetic RGBA8 2D texture + explicit sampler binding at slot 0. It does not lower the logo's
  `image_sample`, resolve AMD WQM/Metal derivative equivalence, read real texture/mesh bytes, or claim
  an asset-derived/game frame.
- Specification: `docs/METAL_FRAME_PATH.md` section 6.2 names `BindTexture/Sampler`; GPU-03 task card 4
  lists the optional textured shader as a later follow-up. The renderer currently reports these ops as
  unsupported and has no fragment texture binding path. The synthetic fixture will use known RGBA8
  pixels and declared filtering/addressing values, with invalid or missing required bindings failing
  visibly.
- Files changed at claim: `docs/TASKS.md`, `docs/INDEX.md`, `docs/FOLDERS.md` (generated), this HANDOFF,
  and `docs/checkpoints/2026-10-24-gpu03-texture-sampler-claim.md`. No production source or test file has
  been edited yet. No dependency/tool was installed.
- Preflight command `git status --short --branch && git rev-parse HEAD` exited 0; result was a clean
  active branch at `e5981d81f7d9b6e14d93e2a31a25cdc420245c2d`. Trail lookup and caller/use scans identified
  `native/platform/render_ops.{hpp,cpp}`, `native/platform/metal_asset_frame.{hpp,mm}` and their focused
  tests. No tests/builds were run at claim time; no separate log artifact exists.
- Observation versus inference: `UnsupportedRenderOpKind` includes `bind_texture` and `bind_sampler`,
  but the validator/executor do not implement either. E634 identifies a single straight-line WQM-wrapped
  logo sample; a synthetic Metal texture binding would test host plumbing only, not game texture ABI or
  shader semantic equivalence.
- Remaining risks: the logo material's actual texture relationship, texture encoding/format/tiling,
  filter/address/mip state, vertex attribute contract, and native derivative equivalence remain unknown.
  The outstanding real `.cmsh`, NRM0, and screenshot-seam choices still constrain the parent asset-frame
  task; this fixture does not cross those boundaries. G3 remains PARTIAL.
- Next exact action: add host-handle texture/sampler RenderOps, explicit RGBA8 texture and sampler input
  descriptors for the bounded harness, and a focused synthetic offscreen sample fixture; then run the
  focused CTests and required pre-publication gates, record an evidence row and completion checkpoint,
  and commit/push allowlisted files under the owner identity with no Claude/Anthropic coauthor trailer.
- Status: ACTIVE, attempt 0; no implementation hypothesis has yet been tried.

## 2026-10-24 16:15 CDT — GPU-02 logo WQM analysis completed

- Integrator: Codex `/root`; active branch `cursor/init-hash-551140-9dfc`. Task
  `GPU-02-GCN-LOGO-WQM-ANALYSIS-01` is `STATIC_ANALYSIS_VERIFIED`; one successful static replay,
  0 failed semantic hypotheses. The user-selected default pair remains the implemented shader target;
  this checkpoint statically characterizes the deferred logo PS only.
- Exact inputs: private E625 `local/fixtures/gpu-r30/logo.csdr`, archive SHA-256
  `21cde037542277bbac24fdb6d1e211f40dbb2187534ce9c31c7c59fad6b68d64`; PS code span offset
  `0x4d8`, 152 B, SHA-256
  `4309caada7f114ec822f61646be536f50a8261a8c6e54f2ce709db7c70b8b39a`. Existing inventory hash
  `99bec189146f19d0248fb85759c252847411ea6adb06704b11aad4cfb194ab68`.
- Exact command: `python3 -I local/orchestrator/gpu02-logo-wqm-analysis/replay.py` exited 0;
  regenerated decoder inventory matches the recorded JSON; 34 instructions, 152 bytes consumed,
  0 unknowns, 0 branches, exact final `s_endpgm`. Payload-free private report:
  `local/orchestrator/gpu02-logo-wqm-analysis/report.json`, SHA-256
  `91c52e19625afbe4bbf9b7918ca5a1c64ad72104671d3ae09d84000d70e1558a`.
- A first replay attempt exited 1 at `assert fresh == stored` because Python tuples in decoder
  output serialize as JSON arrays. The exact diagnosis/output is retained at
  `local/orchestrator/gpu02-logo-wqm-analysis/replay-first-compare-failure.txt`; after normalizing
  to JSON form, the input/inventory equality check and remaining assertions passed. This was a
  serialization-shape correction, not a failed shader-semantic hypothesis.
- Observations: offset `0x28` saves EXEC into VCC; `0x2c` applies `s_wqm_b64` to EXEC; interpolation,
  MIMG sample, color multiply and packing follow without branches or another EXEC write; `0x88`
  restores the saved mask before compressed MRT0 color export at `0x8c`. Resource descriptor loads
  land in `s0:s7`, sampler loads in `s8:s11`; sample uses `dmask=0xf`, `vaddr=v2`, and those bases.
  AMD Sea Islands ISA rev. 1.3 confirms the four-bit WQM expansion, architectural register aliases,
  MIMG resource fields and final pixel export mask requirement.
- Inference/unknowns: attr1's first two interpolated channels supply the sample's first two address
  VGPRs, but resource dimension/coordinate role is not proven. Live descriptor contents, texture
  dimension/format/tiling, filter/wrap/mip state, actual texture binding and Metal derivative
  equivalence remain unknown. No MSL, asset import, Metal render, or game claim was made. G3 stays
  `PARTIAL`, not PASSED.
- Updated files: `docs/TASKS.md` (task status), `docs/EVIDENCE.md` (E634),
  `docs/METAL_FRAME_PATH.md`, `docs/INDEX.md`, `docs/FOLDERS.md` (generated), this HANDOFF, and
  `docs/checkpoints/2026-10-24-gpu02-logo-wqm-analysis.md`. Private verifier, report, and failure note
  remain under `local/orchestrator/gpu02-logo-wqm-analysis/`. No tests/builds were run.
- Next exact action: reconcile the logo material/PNG and descriptor/texture format inputs, then design
  the native Metal fragment-stage sample using Metal's fragment derivative behavior with fail-visible
  bindings. No code lowering until those data inputs are tied to evidence. Pending GPU-03 decisions
  remain real `.cmsh` parser scope, NRM0 handling, and screenshot attachment seam.
- Status: WQM analysis task complete; logo conversion and parent GPU-03 remain active; G3 is PARTIAL.

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

