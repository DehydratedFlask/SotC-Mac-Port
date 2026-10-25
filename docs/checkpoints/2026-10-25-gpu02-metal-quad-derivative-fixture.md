# GPU-02 Metal fragment quad-derivative fixture

**Date:** 2026-10-25 03:48 CDT

**Integrator:** Codex `/root`

**Task:** `GPU-02-METAL-FRAGMENT-QUAD-DERIVATIVE-01`

**Status:** `FIXTURE_VERIFIED`

## Scope

Added a synthetic fragment fixture to the existing `metal_texture_binding` target. The vertex stage
maps a synthetic affine quad to UV coordinates using a harness scale. The fragment stage writes
`dfdx`/`dfdy`, samples the same one-mip constant RGBA8 texture once implicitly and once with the
explicit `gradient2d` overload. A separate CPU calculation predicts the pixel values. The test
checks an interior pixel and a pixel at the primitive boundary, then changes the UV scale and texel
to prove both inputs affect readback.

This is Metal-side fixture evidence only. It does not compile the pinned logo shader, implement AMD
WQM, or prove derivative-driven LOD selection. Retail texture/descriptor/coordinate semantics stay
unknown. No game assets were read and no G3 or gameplay claim follows.

## Commands and results

- `python3 -I -W error tools/dev/test_gcn_to_msl.py` — exit 0, 14/14.
- `cmake --build local/build/orch-r22 --target test_metal_texture_binding -j 12` — exit 0, 2/2.
- `ctest -V --test-dir local/build/orch-r22 -R '^metal_texture_binding$'` — exit 0, 1/1; not
  skipped. The target enabled `MTL_DEBUG_LAYER=1`, `MTL_SHADER_VALIDATION=1`,
  `MTL_SHADER_VALIDATION_DEFAULT_STATE=all`, error reporting to stderr, and abort-on-fault.
- `git diff --check` — exit 0.
- Captured logs are under `local/validation/gpu02-fragment-derivative-20261025/`.

## Full project checks

- `cmake -S native -B local/build/orch-r22` — PASSED.
- `cmake --build local/build/orch-r22 -j 12` — PASSED; no work remained after the changed test
  target had compiled 2/2.
- `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure` — PASSED, 397/397.
- `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` — PASSED, 481 passed,
  0 skipped, 0 failed.
- The initial `trail build --check` found the graph stale after the test edit. `trail build` reparsed
  one file and produced 8,491 nodes / 11,466 edges; the subsequent check passed. The generated graph
  remains a local git-ignored cache. `python3 scripts/project_index.py` passed at 19,588 folders.
- Staged publication audit passed 7 paths; `python3 -I scripts/audit_publish_paths.py --all` passed
  2,783 paths; staged and committed diff checks passed. Commit attribution passed for
  `origin/main..HEAD`; the commit author and committer are Johnny Wang
  (`dehydratedflask@users.noreply.github.com`), with no Claude, Anthropic, or `Co-Authored-By`
  trailer.

## Observations and limits

- Observed: Metal output matched the independent CPU values at pixel `(32,32)` and boundary pixel
  `(3,32)`. A UV scale change from `0.5` to `0.375` changed the gradient channels as predicted;
  changing the single texel's red channel from `64` to `65` changed both sample channels as predicted.
- Observed: the CTest output contained no Metal API or shader-validation fault, and the test returned
  `PASSED` rather than the no-device skip code.
- Inference not claimed: equality of these one-mip implicit/explicit samples does not prove mip
  selection, whole-quad expansion equivalence with AMD EXEC, or retail sampler behavior.
- Failed semantic hypotheses: 0. No dependency changes.

## Next exact action

Publish the owner-authored commit through an exact-head PR, merge it, and return the checkout to
`main`. Then use this fixture as one Metal-side input when
planning a separate pinned logo `IMAGE_SAMPLE`/WQM lowering task; that task still needs an observable
mip/LOD-sensitive test before claiming derivative equivalence.
