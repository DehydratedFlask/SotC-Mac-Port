# GPU-03 synthetic fragment texture/sampler binding fixture

- Local time: 2026-10-24 16:37 CDT. Integrator: Codex `/root`.
- Task: `GPU-03-RENDEROPS-TEXTURE-SAMPLER-01` — **FIXTURE_VERIFIED**. One successful implementation;
  two compile-only harness corrections; zero failed texture/resource semantic hypotheses. Parent
  `GPU-03-RENDEROPS-ASSET-FRAME-01` remains ACTIVE. G3 remains PARTIAL.
- Starting revision: `f025917` (`docs(gpu): claim texture binding fixture`) on
  `cursor/init-hash-551140-9dfc`.

## Implementation

- `native/platform/render_ops.{hpp,cpp}` now has typed `BindTexture` and `BindSampler` operations with
  render-stage and host-handle validation. Their former values in the unsupported-op enum were removed.
- `native/platform/metal_asset_frame.hpp` adds borrowed RGBA8Unorm 2D input, explicit min/mag filter and
  U/V address modes, plus a configuration flag declaring the fragment texture/sampler slot-0 MSL ABI.
  The bytes are read synchronously during `render`; the caller keeps them alive until return.
- `native/platform/metal_asset_frame.mm` validates exact dimensions/byte count and enum values, creates
  one-mip Metal textures and samplers, emits host-handle binding ops, and calls
  `setFragmentTexture`/`setFragmentSamplerState` for fragment slot 0. Texture descriptor creation is
  inside an autorelease pool so CLI fixtures do not accumulate autoreleased descriptors. Other stages
  or slots fail visibly.
- New `native/tests/test_metal_texture_binding.cpp` compiles an ordinary synthetic MSL fragment sample.
  Its 2x1 RGBA8 texture and linear/clamp sampler yield an exact center value `(50,70,90,255)` at UV
  `(0.5,0.5)`. Repeated output bytes match; changing a source texel changes the sample and PNG. Missing
  resources, truncated pixel bytes, and unknown sampler filters reject visibly. MTL API/GPU validation
  were enabled and produced no diagnostics.
- Compatibility: the pre-existing default-pair synthetic `metal_asset_frame` test still passes and
  produces the E632/E633 PNG hashes. No original game asset, `.cmsh`, NRM0, or screenshot seam was used.

## Checks and recorded corrections

- Initial `cmake --build local/build/orch-r22 --target test_render_ops test_metal_texture_binding -j 12`
  exited 1 because the first C++ test draft imported `<Metal/Metal.h>`; Objective-C declarations are
  not valid in a `.cpp` translation unit. It was corrected to use the existing no-device error contract.
- The next target build exited 1 on `-Wmissing-field-initializers` for the new optional draw fields. The
  API now gives both fields explicit `std::nullopt` defaults. Both compile-only corrections are recorded
  in `local/orchestrator/gpu03-texture-sampler/first-build-failure.txt`; no shader/resource behavior
  hypothesis failed, and no third unsuccessful try occurred.
- `cmake -S native -B local/build/orch-r22` — post-review exit 0; log
  `local/orchestrator/gpu03-texture-sampler/post-review-configure.log`.
- `cmake --build local/build/orch-r22 --target test_render_ops test_metal_texture_binding -j 12` —
  post-review exit 0; log
  `local/orchestrator/gpu03-texture-sampler/post-review-target-build.log`.
- `ctest --test-dir local/build/orch-r22 -R 'render_ops|metal_texture_binding' --output-on-failure` —
  2/2 passed after the pool fix. Log
  `local/orchestrator/gpu03-texture-sampler/post-review-target-ctest.log`.
- `cmake --build local/build/orch-r22 -j 12` — post-review exit 0. Log
  `local/orchestrator/gpu03-texture-sampler/post-review-full-build.log`.
- `ctest --test-dir local/build/orch-r22 -V -R '^metal_asset_frame$'` — 1/1 passed, preserving the
  existing synthetic default-pair renderer/parser path. Log
  `local/orchestrator/gpu03-texture-sampler/metal-asset-frame-ctest.log`.
- `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure` — **392/392 passed**. Log
  `local/orchestrator/gpu03-texture-sampler/full-ctest.log`.
- `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` — post-review **476 passed,
  0 skipped, 0 failed**. Log
  `local/orchestrator/gpu03-texture-sampler/post-review-lift-checkers.log`.
- Independent review found the descriptor lifetime issue fixed by the autorelease pool and reported no
  further correctness, lifetime, or coverage issues. The reviewer did not run tests; the post-review
  checks above were run by the integrator.
- Trail was rebuilt with all existing scopes (`scripts`, `tools`, `reference`) plus `native`, `launcher`,
  and `tests`. `trail build --check` passed with 8,306 wiring nodes; the Trail file API returned this
  test's `main`, proving the new C++ test is queryable. Deep meaning summaries remain unbuilt.
- `git diff --check` passed. `python3 scripts/project_index.py` indexed 9,043 folders.
  `python3 -I scripts/audit_publish_paths.py` passed with 14 staged paths;
  `python3 -I scripts/audit_publish_paths.py --all` passed with 2,751 paths; commit-attribution audit
  passed for `origin/cursor/init-hash-551140-9dfc..HEAD`.

## Evidence boundary and next action

This verifies the host texture/sampler seam on synthetic RGBA8 input only. The default pair is already
verified in E627; logo `image_sample` remains blocked on WQM/Metal semantics and actual resource details.
Retail texture formats/tiling, the `logo.cmat` texture relationship, live descriptors, filter/address/mip
state, texture coordinate/attribute mapping, and exact AMD-to-Metal derivative equivalence remain UNK.
No real archive bytes or game screen were processed; no boot/audio/save or G3 PASS claim is made.

Next exact action: inspect the final staged diff, commit as Johnny Wang without Claude/Anthropic
coauthors, push the active branch, and verify remote head and clean worktree.
