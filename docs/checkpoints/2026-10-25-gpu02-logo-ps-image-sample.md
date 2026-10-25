# GPU-02 logo pixel IMAGE_SAMPLE lowering

**Date:** 2026-10-25 04:06 CDT

**Integrator:** Codex `/root`

**Task:** `GPU-02-GCN-LOGO-PS-IMAGE-SAMPLE-01`

**Status:** ACTIVE, attempt 1; E645 prerequisite fixture verified, pinned logo MSL lowering/execution NOT RUN.

## Scope

The active slice is a closed-world lowering of E634's private 152-byte/34-instruction logo pixel
shader. It must validate the exact pinned digest and WQM/save/restore, one base `IMAGE_SAMPLE`,
descriptor register ranges and compressed export pattern before emitting an ordinary PS-only MSL
fragment for a declared synthetic slot-0 texture/sampler/coordinate ABI. Unsupported changes must
fail visibly. This checkpoint records the preceding mip-sensitive Metal fixture only; it does not
claim that the shader has been lowered.

## E645: mip-sensitive Metal fixture

The `AssetTexture2D` harness input now accepts explicit RGBA8 mip levels after its base level;
`AssetSamplerInput` can select nearest/linear mip filtering and a broad ordered LOD clamp. Existing
three-field texture and four-field sampler aggregate initialization retains its previous behavior.
The backend validates level dimensions/byte counts, creates the requested Metal mip count, and
uploads each supplied level.

The existing `metal_texture_binding` target now uses a 64x64 synthetic texture with a caller-provided
64-to-1 mip chain. A fragment MSL fixture samples implicitly and with explicit `gradient2d` values.
Its affine UV scale selects mip 1 at scale 1 and mip 2 at scale 2. The independently computed CPU
oracle matches both sample channels at center `(32,32)` and primitive boundary `(3,32)`; replacing
the selected level with blue changes the output. The RGBA8 diagnostic gradient `0.5` reads back as
127, so the fixture oracle models the observed conversion by truncating that channel.

## Commands and results

- `cmake --build local/build/orch-r22 --target test_metal_texture_binding -j 12` — PASSED, 2/2
  build steps. Latest log: `local/validation/gpu02-logo-ps-image-sample-20261025/metal-test-build-rerun3.log`.
- `ctest -V --test-dir local/build/orch-r22 -R '^metal_texture_binding$'` — PASSED, 1/1, not
  skipped, with Metal API and GPU validation enabled and no validation fault. Latest log:
  `local/validation/gpu02-logo-ps-image-sample-20261025/mip-fixture-ctest-rerun3.log`. It also
  verifies fail-visible rejection for noncanonical mip dimensions, truncated mip bytes, and
  unordered sampler LOD clamps.
- Earlier attempts/logs are retained: `api-build.log` (partial-fixture build before wiring finished),
  `metal-test-build.log`, `mip-fixture-ctest.log`, `mip-fixture-ctest-rerun1.log`, and
  `metal-test-build-rerun1.log`. The first CTest found a stale expected error string and oracle
  quantization mismatch; the diagnostic rerun isolated the latter to byte `127` versus `128`.
- `SOTC_LOGO_PS_MSL` was unset. The pinned generated PS MSL was therefore NOT RUN in E645.
- `python3 scripts/project_index.py` — passed; 19,589 folders indexed.

## Observations and limits

- Observed: explicit mip data uploads and nearest mip filtering work on the local Metal device for
  the synthetic affine shader. Distinct levels respond to changed UV scale and texture input, and
  interior/edge pixels agree with the CPU oracle.
- Failed implementation hypotheses: 0. Failed fixture/oracle attempts: 1 (byte-rounding assumption;
  corrected after pixel diagnostics). No dependency changes.
- Not established: exact AMD derivative/WQM equivalence; descriptor values; game texture format,
  tiling or sampler state; semantic attribute names; retail texture/attribute binding; any game frame
  or G3 pass.
- Private shader bytes and generated MSL remain under `local/`; no payload was added to the repo.

## Next exact action

Integrate the dedicated closed-world logo PS lowerer, create the exact synthetic ABI manifest and
extract the pinned code span only under `local/`. Generate MSL under private validation output, then
run the same CTest with `SOTC_LOGO_PS_MSL` pointing to that file and preserve a direct PNG under
`local/validation/`. Keep G3 PARTIAL and make no AMD/retail equivalence claim.
