# Logo VS/PS pair on raw CMSH meshes

## 2026-10-25 10:17 CDT — task claim

- Integrator: Codex `/root`; branch `main`, base `0e93d14b`.
- Task: `GPU-03-LOGO-VS-PS-RAW-CMSH-FRAME-01`, **ACTIVE**, attempt 0.
- Scope: combine the E669 closed synthetic `logo-pair` MSL with the previously parsed raw
  `cube_1m.cmsh` and `cone.cmsh` POS0/U16 geometry and the E656 pinned `saveshadow_scee.png` test
  texture. Keep all game bytes and generated outputs under ignored `local/`. Apply a test-only
  position transform into disjoint visible `[0,1]` XY regions; in E669, the transformed position
  supplies both clip XY and correlated interpolated UV. Use fixed harness normals and synthetic
  shader constants.
- Hash/count gates: composed MSL 4,094 B, SHA-256
  `37e09aef0838dff77c8f151f288ed8cb2cdf7db26e202b40bb9f6f27a34dbd56`; cube 850 B, SHA-256
  `44738dbf663af252898bad5582f2b48f1aadf6fbaab403c20880410b79a380b7`, 24 positions/36 indices;
  cone 1,462 B, SHA-256
  `5456afadabe96b9f7f02c503dc0ed5410652ccd32601c19e8aeeaaf36c88600c`, 42 positions/114 indices;
  PNG 24,636 B, SHA-256
  `b4b135232304ab696785353b6ba96f874fc4bcfd8805e56b931119e415a300f5` (228x128 RGB before the
  documented test-only RGBA8 conversion).
- Acceptance: fail closed on partial paths, path escapes, aliases, wrong bytes, parse/count/index
  errors; compare selected interior covered pixels on both meshes against an independent CPU oracle
  for the E636 clip transform and E646 nearest/clamp sample + fp16-RTZ output; require repeat RGBA/PNG
  determinism; mutate a sampled source texel and a mesh position and require their selected output
  pixels/PNGs to change; write private baseline and mutation PNGs directly; focused Metal CTest must
  run without device skip under Metal API/GPU validation. Then run repository gates from `AGENTS.md`.
- Owner decisions already in force: use the synthetic fixture first, fixed harness normals, direct
  private PNG output, and the documented raw-asset import; defer the `0x8C` decoder. Do not infer
  TEX0 or NRM0 semantics and do not use `attach_screenshot` or invent an engine frame number.
- Implementation lane: `/root/review_logo_pair_metal` owns only
  `native/tests/test_metal_texture_binding.cpp`; root owns docs, independent verification, and Git.
- No code, parser, renderer API, dependency, or test change has been made for this card yet. Test
  status: **NOT RUN**. G3 remains **PARTIAL**; retail resource/stream ABI, AMD WQM/helper-lane
  equivalence, game-driven rendering, boot, audio, and save/reload are not established.
- Exact next action: implement the opt-in raw-mesh/pinned-PNG composed-pair case with canonical
  distinct path checks, run the focused `metal_texture_binding` CTest with its exact local inputs,
  and report outputs/hashes under `local/validation/gpu03-logo-vs-ps-raw-cmsh-20261025/`.

## 2026-10-25 10:19 CDT — claim published

- Commit `e2faefcd64de171d2890a1da2dd319afde9bd2c5` pushed directly to GitHub `main`; post-push
  fetch verified `HEAD == origin/main`, clean `main`, and no open PRs.
- Staged/all-path audits passed 7/2,797; attribution passed. Commit author and committer are Johnny
  Wang, with no Claude/Anthropic/co-author trailer.
- No implementation or test was run in this publication step. Task remains **ACTIVE**, attempt 0.

## 2026-10-25 10:32 CDT — attempts 1–3 stopped; reassessment recorded

- Integrator/principal: Codex `/root`; branch `main`, source base `2055cb20` before this checkpoint.
- Implementation lane `/root/review_logo_pair_metal` changed only `native/tests/test_metal_texture_binding.cpp` (+553 lines; SHA-256 `6592730c2008540a67f9a70fe33383e03c55be1027834a31fafb1ccbda8f16ad`). Focused target build and `git diff --check` passed, as reported by the worker.
- Exact focused command/environment and per-attempt outcomes are recorded in `docs/escalations/GPU-03-LOGO-VS-PS-RAW-CMSH-FRAME-01.md`; all three CTest logs are preserved under `local/validation/gpu03-logo-vs-ps-raw-cmsh-20261025/`. Attempt 1 failed before pipeline execution because CPU interior probes were not found. Attempts 2/3 passed pinned raw input checks, E669 pipeline creation, both mesh CPU-oracle probes, baseline rendering, byte-identical repeat, texture mutation, PNG writes, and Metal API/GPU validation, failing only the position-mutation candidate search. Baseline PNG is 938 B / `ab7826d09a9ff8dfaa31fa2e338e73fb83a8c73d4cae7890a5cd5b6c6dc8a811`; texture-mutated PNG is 947 B / `e3f59b486ee631141074f9d73c843427ee4c9b9e8453c0a133d2d126b40da218`; no position-mutated PNG exists.
- Independent read-only review confirmed the search only tests outside→inside coverage and found the position CPU oracle omits camera scale 0.8; it also noted the lack of mutated-frame length/stride checks and that the logs do not isolate candidate-filter counts. No code was changed and no test run by the reviewer.
- Before any fourth test, root recorded a principal reassessment authorizing exactly one try 4: record geometric/color-qualified entrant counts; search robust inside→outside coverage; correct UV to `(x+0.5)/(cameraX*width)` and `(y+0.5)/(cameraY*height)`; validate mutated RGBA shape; compare baseline-covered oracle color against a verified pixel uncovered by both meshes; require the mutated Metal pixel to equal clear and the PNG to differ. Preserve all existing checks. No fifth attempt is authorized.
- Files changed for the checkpoint: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/INDEX.md`, `docs/METAL_FRAME_PATH.md`, this checkpoint, and the new escalation file. Task is ACTIVE after reassessment, 3 prior failed tries and 1 authorized attempt; G3 remains **PARTIAL**.
- Exact next action: publish this recorded escalation/reassessment on `main`, then resume the owned test file for try 4; request scoped independent review before any completion claim.

## 2026-10-25 10:37 CDT — principal-authorized attempt 4 passed; review pending

- Implementation: `/root/review_logo_pair_metal`; source changed only `native/tests/test_metal_texture_binding.cpp`, 626 lines added relative to published base; SHA-256 `c048c6d3912da2cb80a731bc05635c18be641d9bdc535ef8e30729baa213fcaf`. `git diff --check` passed.
- Build: `tools/dev/with_build_lock.sh cmake --build local/build/orch-r22 --target test_metal_texture_binding -j 12`; exit 0, log `local/validation/gpu03-logo-vs-ps-raw-cmsh-20261025/attempt4-build.log`.
- Exactly one focused CTest ran, `metal_texture_binding`, with Metal API and GPU validation enabled. Result **PASSED 1/1**. Log `local/validation/gpu03-logo-vs-ps-raw-cmsh-20261025/attempt4.log`, SHA-256 `9f8585bfde6fa11b53708e2f3a20dda6123187d417d4d528e45f4c31712ab510`.
- Probe counts were newly-covered geometry/color-qualified `0/0` and newly-uncovered geometry/oracle-qualified `13/2`. Selected pixel `(5,19)` changed CPU coverage margin from `+0.194` to `-0.106`; the corrected camera-scaled oracle passed; the position-mutated GPU pixel matched verified clear RGBA8 `{0,0,0,255}`. Baseline 938 B SHA-256 `ab7826d09a9ff8dfaa31fa2e338e73fb83a8c73d4cae7890a5cd5b6c6dc8a811`; texture mutation 947 B SHA-256 `e3f59b486ee631141074f9d73c843427ee4c9b9e8453c0a133d2d126b40da218`; position mutation 969 B SHA-256 `d683ea8d0aa9f54aedffb14175fa931d9340fb215e1a5106aee33aa06de45aad`.
- Attempt 4 result: focused gate **PASSED**; task remains **ACTIVE** pending independent code/spec review and required full repo gates. No fifth attempt is authorized. G3 remains PARTIAL; retail bindings/WQM/game frame/boot/audio/save are not established.
- Exact next action: complete the dispatched read-only review, then run the full verification set in `AGENTS.md`; publish only after review/gates and attribution/path audits.

## 2026-10-25 10:39 CDT — independent review approved the bounded fixture

- Reviewer `/root/review_gpu03_attempt4` inspected the implementation diff, accepted task scope,
  escalation and attempt-4 log without editing or running tests. Verdict: supports
  **FIXTURE_VERIFIED** for this synthetic asset-derived fixture; no Critical or Important findings.
- Review confirmed the seven-input path/hash/count gates, both per-mesh oracle samples, deterministic
  repeat, camera-scaled mutation UV, transition counts, clear-color check, mutated-frame shape
  guards, position/texture mutations and no game-frame/retail ABI claim. Attempt-4 evidence and PNG
  hashes are recorded in E677 and `docs/escalations/GPU-03-LOGO-VS-PS-RAW-CMSH-FRAME-01.md`.
- Minor finding: ordinary CTest leaves the private raw-asset case `NOT RUN` when none of its seven
  opt-in paths are supplied. Root ruling: defer as spec-compatible because game assets remain owner-
  local and the accepted focused run supplies the complete paths. Cost if wrong: a default test run
  could omit this fixture, so its status must be tied to the explicit opt-in invocation.
- Task status: **FIXTURE_VERIFIED** for this bounded test only. Retail TEX0/NRM0, stream/resource
  semantics, WQM equivalence, game frame and G3 remain unverified/PARTIAL. Full repository gates are
  pending and required before publication.
- Exact next action: run all `AGENTS.md` verification commands with the asset opt-in variables unset
  for the full general suite, preserving the attempt-4 log as the only raw-pair gate run; then audit,
  commit and push the allowlisted source/docs.

## 2026-10-25 10:53 CDT — repository gates passed; publication pending

- Integrator: Codex `/root`; branch `main`, pre-publication HEAD and `origin/main` both
  `ea5b561b19c77001981a7c27e7bfdcd0720c8b03`. Open PR query returned `[]`; the previously active
  `cursor/init-hash-551140-9dfc` ref is absent remotely. The owner explicitly requested resuming on
  `main`; repo branch guidance is being synchronized to that instruction.
- Current source remains `native/tests/test_metal_texture_binding.cpp`, SHA-256
  `c048c6d3912da2cb80a731bc05635c18be641d9bdc535ef8e30729baa213fcaf`. No source change occurred
  after the reviewed focused attempt. Exact output logs for configure, build, full CTest, lift
  checkers, project index and Trail check are in
  `local/validation/gpu03-raw-pair-final-20261025/`; hashes are enumerated in E679.
- Exact repository commands and results: configure exit 0; build exit 0 (`ninja: no work to do`);
  full CTest exit 0, **397/397**, 8.58 s; lift checkers exit 0, **481 passed / 0 skipped / 0
  failed**; project index exit 0, **20,420** folders; Trail wiring check exit 0, **8,571** nodes
  synchronized, deep concept layer not built. The full CTest run left the seven private asset paths
  unset; the focused E677 run remains the only accepted attempt-4 asset-fixture gate.
- The escalation recipe's MSL path was corrected to the extant E669 artifact. Its SHA-256 re-matches
  the pin. The historical focused log does not record literal path environment values; this remains
  an auditability limitation, not a reason to rerun the now-passing fourth attempt.
- Files changed for this checkpoint: existing allowlisted source/docs plus
  `docs/checkpoints/2026-10-25-f590-release-review-recovery.md`; no dependency or install changes.
  Observation: the recorded commands and test counts passed. Inference boundary: no retail texture or
  normal mapping, WQM equivalence, game-driven frame, G3, boot, audio or save/load proof follows.
- Precommit publication checks passed: staged audit **12** paths; all-path audit **2,799** paths;
  attribution audit reports an empty unpublished range `origin/main..HEAD`; staged whitespace and
  project-index checks passed. The exact commands are recorded in the current `HANDOFF.md` entry.
- Status: **FIXTURE_VERIFIED** bounded test; full repository gates and precommit audits passed;
  direct-main publication remains pending. G3 **PARTIAL**. Exact next action: commit and push the
  allowlisted source/docs to `origin main` under Johnny Wang, then fetch and verify local HEAD equals
  `origin/main`.

## 2026-10-25 10:58 CDT — source and evidence published to main

- Commit `ebae3a83e602385d2732a68e4d99d8064ec5f167` published the reviewed source and checkpoint
  updates. Exact push command `git push origin main` exited 0; after `git fetch origin main`, both
  `HEAD` and `origin/main` are `ebae3a83e602385d2732a68e4d99d8064ec5f167`.
- `git status --short --branch` reports a clean `main`; the old task branch is absent from
  `git ls-remote`; the open-PR query returned `[]`. Commit author/committer are Johnny Wang and no
  Claude/Anthropic/assistant co-author trailer appears.
- Task status: **PUBLISHED / FIXTURE_VERIFIED** for the bounded synthetic asset-derived test only.
  G3 stays **PARTIAL**; no retail binding, WQM/game-frame, boot, audio or save/load claim follows.
- Exact next action: commit/push the post-publication handoff record, then claim the raw-material
  selector task from the pinned default material/shader assets before editing source.
