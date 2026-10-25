# GPU-03-LOGO-VS-PS-RAW-CMSH-FRAME-01 — position-mutation probe history

Status: the three failed probes were escalated and the principal-authorized fourth attempt passed.
This task is now **FIXTURE_VERIFIED** for the bounded synthetic asset-derived fixture only; no fifth
attempt is authorized or required. See E677–E679 and the current checkpoint for the accepted result.

- Owner / principal / date: implementation `/root/review_logo_pair_metal`; principal Codex `/root`; 2026-10-25 CDT.
- Requirement and failing gate: execute the exact E669 composed vertex/pixel MSL on the exact-hash raw cube/cone POS0/U16 streams plus the E656 logo PNG, and prove a mesh-position mutation changes the selected readback pixel and private PNG. Exact hashes/counts, parsing, pipeline compilation, baseline render, interior CPU oracle samples for both meshes, deterministic repeat, source-texel mutation, and Metal API/GPU validation passed on tries 2 and 3. The position-mutation probe did not find a qualifying pixel. Do not weaken or omit this gate. This is not evidence of a shader-semantic failure; the task is not fixture-verified.
- Input SHA256, source pins, original RVA / file offset / image base: E669 composed MSL, 4,094 B, SHA-256 `37e09aef0838dff77c8f151f288ed8cb2cdf7db26e202b40bb9f6f27a34dbd56`; cube, 850 B, SHA-256 `44738dbf663af252898bad5582f2b48f1aadf6fbaab403c20880410b79a380b7`, 24 positions / 36 U16 indices; cone, 1,462 B, SHA-256 `5456afadabe96b9f7f02c503dc0ed5410652ccd32601c19e8aeeaaf36c88600c`, 42 positions / 114 U16 indices; source PNG, 24,636 B, SHA-256 `b4b135232304ab696785353b6ba96f874fc4bcfd8805e56b931119e415a300f5`, 228x128 RGB. No executable RVA, game frame, or runtime claim is used.
- Reproducer / expected result: build `test_metal_texture_binding`, then run the focused CTest with the seven absolute private input/output paths and Metal validation variables set as below. Expected: exit 0, a deterministically repeated composed-MSL frame, both per-mesh CPU oracle samples, changed texel output, and a position-mutation pixel/PNG difference. Actual by try 3: exit 1 only because the CPU position-mutation candidate search returned false.

```sh
cmake --build local/build/orch-r22 --target test_metal_texture_binding -j 12
env MTL_DEBUG_LAYER=1 MTL_SHADER_VALIDATION=1 \
  MTL_SHADER_VALIDATION_DEFAULT_STATE=all \
  MTL_SHADER_VALIDATION_ENABLE_ERROR_REPORTING=1 \
  MTL_SHADER_VALIDATION_REPORT_TO_STDERR=1 \
  MTL_SHADER_VALIDATION_ABORT_ON_FAULT=1 \
  SOTC_LOGO_PAIR_CMSH_MSL="$PWD/local/validation/gpu03-logo-vs-ps-pipeline-20261025/logo-pair.metal" \
  SOTC_LOGO_PAIR_CMSH_CUBE="$PWD/local/orchestrator/gpu03-r31/asset-derived/cube_1m.cmsh" \
  SOTC_LOGO_PAIR_CMSH_CONE="$PWD/local/orchestrator/gpu03-r31/asset-derived/cone.cmsh" \
  SOTC_LOGO_PAIR_CMSH_PNG="$PWD/local/validation/gpu03-logo-png-texture-20261025/saveshadow_scee.png" \
  SOTC_LOGO_PAIR_CMSH_BASELINE_PNG="$PWD/local/validation/gpu03-logo-vs-ps-raw-cmsh-20261025/baseline.png" \
  SOTC_LOGO_PAIR_CMSH_TEXTURE_MUTATED_PNG="$PWD/local/validation/gpu03-logo-vs-ps-raw-cmsh-20261025/texture-mutated.png" \
  SOTC_LOGO_PAIR_CMSH_POSITION_MUTATED_PNG="$PWD/local/validation/gpu03-logo-vs-ps-raw-cmsh-20261025/position-mutated.png" \
  ctest --test-dir local/build/orch-r22 -R '^metal_texture_binding$' --output-on-failure --verbose
```

The command above is a corrected reproduction recipe: the attempt-4 log records the focused pass,
Metal validation and aggregate hashes/counts but not the literal seven environment values. The MSL
path above is the extant E669 artifact and its hash was rechecked against the pinned digest. No replay
was performed after the authorized fourth attempt.

- Tree at the three-try stop and recovery location: checkout was `main` at source base `2055cb20` before the escalation checkpoint. The implementation diff then was `native/tests/test_metal_texture_binding.cpp` (+553 lines; source SHA-256 `6592730c2008540a67f9a70fe33383e03c55be1027834a31fafb1ccbda8f16ad`). Private inputs, logs, and output PNGs remain under ignored `local/`. Attempts 1–3 logs are preserved in `local/validation/gpu03-logo-vs-ps-raw-cmsh-20261025/`; baseline PNG is 938 B / SHA-256 `ab7826d09a9ff8dfaa31fa2e338e73fb83a8c73d4cae7890a5cd5b6c6dc8a811`; texture-mutated PNG is 947 B / SHA-256 `e3f59b486ee631141074f9d73c843427ee4c9b9e8453c0a133d2d126b40da218`; no position-mutated PNG had yet been produced.

| Failed try | Distinct hypothesis/change | Exact command / environment | Actual result | Log/evidence |
|---|---|---|---|---|
| 1 | With unit camera scale, a safe CPU sample would be found away from both indexed-triangle edges and texture texel boundaries. | Reproducer command above; CTest logged `MTL_DEBUG_LAYER=1`, `MTL_SHADER_VALIDATION=1`, default-state-all, error reporting, and abort-on-fault. | Exit 1 before Metal pipeline/render: cube and cone CPU interior probes both failed. Mesh hash/count checks passed. | `local/validation/gpu03-logo-vs-ps-raw-cmsh-20261025/metal-raw-pair-ctest-first.log`; existing camera/texel geometry analysis in the task checkpoint. |
| 2 | Set synthetic camera XY scale to 0.8 to avoid the exact texture-V texel-boundary pattern and find safe samples; then the +0.06 X mutation would expose a newly covered, non-clear interior pixel. | Same build and reproducer command above; same Metal validation environment. | Exit 1 at the position probe. Pipeline and baseline frame, both mesh CPU oracle samples, repeat determinism, source-texel mutation, PNG writes, and Metal API/GPU validation passed. Candidate search returned false. | `local/validation/gpu03-logo-vs-ps-raw-cmsh-20261025/metal-raw-pair-ctest-attempt2.log`; baseline and texture-mutated PNGs. |
| 3 | Increase the test-only cube X translation to +0.12 so a newly covered interior pixel is large enough to pass the margin and differ from clear. | Same build and reproducer command above; same Metal validation environment. | Exit 1 at the same position-probe assertion. All passing checks from try 2 remained green; candidate search still returned false. | `local/validation/gpu03-logo-vs-ps-raw-cmsh-20261025/metal-raw-pair-ctest-attempt3.log`; source diff hash above. |

- Smallest failing fixture and how to run it: the pinned cube/cone raw files and PNG above with the exact E669 MSL. Focused test is `metal_texture_binding`. The current helper at `native/tests/test_metal_texture_binding.cpp:1016-1055` accepts only pixels outside original coverage and well inside mutated coverage, then rejects a candidate when the sampled output equals the baseline pixel. Tries 2 and 3 emitted safe mesh probes at cube `(14,29)` / texel `(64,73)` and cone `(45,22)` / texel `(202,56)`, with minimum barycentric margins `0.310` and `0.326`.
- Verified facts versus guesses: attempts 2 and 3 compiled and rendered the exact composed pair with Metal API/GPU validation enabled and no reported fault; both raw inputs passed the hash/count parser; both selected mesh pixels matched the CPU nearest/clamp fp16-RTZ oracle; repeat output matched byte-for-byte; the chosen source texel mutation changed the selected cube output and PNG. Only candidate selection for position sensitivity failed. It is a hypothesis, not yet verified, that moving a pixel out of coverage will provide a more robust signal than sampling newly covered texture space.
- Why the previous approaches failed / missing information: the detector is one-sided. It searches newly covered pixels and further requires that the E669 correlated UV sample differ from the clear baseline. A position shift can instead provide a clearer signal at a baseline-covered pixel that becomes uncovered, especially when newly exposed coordinates sample the PNG's clear-colored area. The test has not yet searched that direction.
- Independent work that can continue: complete and publish the already gathered read-only slot-306 survey; no Ghidra rerun is needed. Preserve its unresolved indirect-call and side-effect limitations.
- Task register and HANDOFF checkpoint links: [TASKS](../TASKS.md), [HANDOFF](../../HANDOFF.md), and [raw CMSH + logo-pair checkpoint](../checkpoints/2026-10-25-gpu03-logo-vs-ps-raw-cmsh-frame.md).
- Independent read-only review — 2026-10-25 10:31 CDT: confirmed the finder accepts only outside→inside pixels and noted that the retained logs do not show whether geometry margins or the color filter reject candidates. Review also found a definite CPU oracle mismatch in the mutation-render assertion: the finder scales output-center UV by `kLogoPairAssetCameraX/Y`, but its caller omitted those factors. The review additionally requested complete mutated-frame shape validation before range indexing. No code was changed and no test was run by the reviewer. See the root checkpoint for the ruling.
- Principal reassessment / authorized fourth approach — 2026-10-25 10:32 CDT, Codex `/root`: authorize exactly one try 4 to instrument the newly-covered search with geometric-candidate and post-color-filter counts; search the opposite coverage transition using the same safe margins (`original_margin >= 0.18`, `mutated_margin < -0.08`); correct the CPU position-mutation sample to use UV `(x+0.5)/(cameraX*width)`, `(y+0.5)/(cameraY*height)`; validate the mutated RGBA dimensions/stride before indexing; and require a robust baseline-covered pixel to match the independent texture oracle and differ from the renderer's documented clear RGBA8 value `{0,0,0,255}`. Verify the clear sample is uncovered by both CPU mesh coverages. After the GPU mutation, require that same pixel to equal the clear value and the PNG bytes/hash to differ. The direct-Metal pass clear is defined as `{0,0,0,1}` by `native/platform/render_ops.hpp` `BeginPass`; the fixture's RGBA8 readback is checked explicitly. Preserve all existing checks. This resolves a definite oracle defect and probes the opposite coverage side without weakening the mesh-position sensitivity gate; the synthetic position/UV alias remains explicit. If the candidate or GPU result fails, stop and reassess; no fifth attempt is authorized.

## Attempt 4 result — focused gate passed; independent review pending

- Date: 2026-10-25 10:37 CDT. Implementer: `/root/review_logo_pair_metal`. This was the single authorized fourth attempt; no fifth attempt is authorized or needed after this pass.
- Source: only `native/tests/test_metal_texture_binding.cpp` changed; 626 lines added relative to published base; SHA-256 `c048c6d3912da2cb80a731bc05635c18be641d9bdc535ef8e30729baa213fcaf`. `git diff --check` passed.
- Build: `tools/dev/with_build_lock.sh cmake --build local/build/orch-r22 --target test_metal_texture_binding -j 12` exited 0; log is `local/validation/gpu03-logo-vs-ps-raw-cmsh-20261025/attempt4-build.log`.
- Exactly one focused CTest ran with the seven pinned private paths, Metal API validation, and Metal GPU validation; `metal_texture_binding` passed 1/1. Full output: `local/validation/gpu03-logo-vs-ps-raw-cmsh-20261025/attempt4.log`, SHA-256 `9f8585bfde6fa11b53708e2f3a20dda6123187d417d4d528e45f4c31712ab510`.
- Candidate counts: newly-covered geometry/baseline-color qualified `0/0`; newly-uncovered geometry/baseline-oracle qualified `13/2`. Selected pixel `(5,19)` has original/mutated margins `+0.194/-0.106`; the corrected camera-scaled CPU oracle passed, mutated readback equaled RGBA8 clear `{0,0,0,255}`, and the position-mutated PNG differed.
- Baseline PNG: 938 B, SHA-256 `ab7826d09a9ff8dfaa31fa2e338e73fb83a8c73d4cae7890a5cd5b6c6dc8a811`. Texture-mutated PNG: 947 B, SHA-256 `e3f59b486ee631141074f9d73c843427ee4c9b9e8453c0a133d2d126b40da218`. Position-mutated PNG: 969 B, SHA-256 `d683ea8d0aa9f54aedffb14175fa931d9340fb215e1a5106aee33aa06de45aad`.
- Status remains ACTIVE until independent spec/code review and repository-wide gates complete. This is bounded synthetic asset-derived evidence only; retail bindings, TEX0/NRM0, AMD WQM equivalence, game frame, and G3 remain unverified/PARTIAL.

## Independent review ruling — 2026-10-25 10:39 CDT

- Reviewer `/root/review_gpu03_attempt4` reports no Critical or Important findings and confirms the
  attempt-4 result supports **FIXTURE_VERIFIED** for this bounded synthetic fixture. Reviewer made no
  edits and ran no tests.
- Deferred minor: without all seven private opt-in paths, ordinary CTest prints `NOT RUN`. Root
  ruling: retain the opt-in behavior because this fixture requires owner-local assets and the
  acceptance run explicitly supplied the inputs. Cost if wrong: a default CTest invocation can skip
  this asset fixture, so the task's verified status must cite its explicit run.
- Repository-wide gates are still required before publishing the test source. This evidence does not
  establish G3, retail bindings/streams, TEX0/NRM0, WQM equivalence, a game-driven frame, boot, audio,
  or save/reload.

## 2026-10-25 10:53 CDT — final attempt and repository gates reconciled

- The principal-authorized fourth attempt passed the focused fixture and independent review found no
  Critical or Important issues. The `NEEDS_PRINCIPAL` escalation is closed for attempt management;
  task status is **FIXTURE_VERIFIED** for the synthetic asset-derived fixture only. No fifth attempt
  was made or authorized.
- Required repository configure/build passed; CTest passed 397/397 (8.58 s); lift checkers passed
  481/0/0; project index reported 20,420 folders; Trail wiring check reports 8,571 nodes in sync.
  Exact log paths and hashes are in E679 and the current checkpoint. The full CTest left the opt-in
  asset variables unset; E677 remains the fixture execution record.
- The reproduction recipe at the start of this file now points to the existing E669 MSL artifact at
  `local/validation/gpu03-logo-vs-ps-pipeline-20261025/logo-pair.metal`, whose SHA-256 matches the
  pinned 4,094-byte MSL. The attempt-4 output does not capture its seven literal path values; this
  limitation is recorded, and the test was not rerun after its authorized pass.
- Precommit publication audits passed: staged path audit 12, all-path audit 2,799, and commit
  attribution audit. E681 records commit `ebae3a83` pushed to `main` under Johnny Wang and verified
  by fetch; no Claude/Anthropic or assistant co-author trailer was added. G3 remains **PARTIAL**;
  retail resource/stream/texture/normal bindings, AMD WQM equivalence, game-driven rendering, boot,
  audio and save/reload remain unverified.
