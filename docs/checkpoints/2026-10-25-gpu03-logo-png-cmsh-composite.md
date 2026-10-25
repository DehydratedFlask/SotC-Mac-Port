# GPU-03 logo PNG and raw mesh composite fixture

Date: 2026-10-25 05:37 CDT
Integrator: Codex `/root`
Task: `GPU-03-LOGO-PNG-CMSH-ASSET-FRAME-01`
Status: **FIXTURE_VERIFIED**; publication gates in progress

## Scope and implementation

The fixture combines the existing digest-pinned logo PS MSL and PNG path with the exact local raw
`cube_1m.cmsh` and `cone.cmsh` byte streams. `parse_cmsh_mesh` returns POS0 positions and U16 indices;
the test validates each byte count, SHA-256, parsed count, finite positions, and index bounds before
rendering. Test-side transforms place the two meshes in separate clip-space regions. The fixture
uses fixed harness normals, nearest slot-0 sampling, and the MSL adapter's synthetic
`position.xy * material.xy + 0.5` UV mapping.

A CPU barycentric search selects samples well inside indexed triangles and away from nearest-sampler
texel boundaries. The existing independent sampler/fp16-RTZ oracle checks the two covered readback
pixels. Repeated RGBA/PNG output must match exactly; changing the selected source texel must change
the expected oracle value, readback, and PNG. Baseline and mutation PNGs are written directly under
ignored `local/` evidence paths.

## Review and fail-closed path fix

The first focused target compile found an unused bookkeeping variable promoted to an error by
`-Werror`; removing it was a compile-only correction. A fresh review then found that a standalone
texture PNG output variable alone could be silently ignored. Any texture input, baseline output, or
mutation output now requests the replay. The complete MSL/input/baseline/mutation path set is
required, checked for canonical local containment and aliases, and partial configuration fails
before optional reads or writes. A second read-only review found no remaining issue.

Focused build and log:

```sh
tools/dev/with_build_lock.sh cmake --build local/build/orch-r22 --target test_metal_texture_binding -j 12
```

Exit 0; log: `local/validation/gpu03-logo-png-texture-20261025/composite-build.log`.

The output-only negative configuration was exercised with the complete set of unrelated optional
inputs unset. The test's expected nonzero result proves the partial request is rejected, and the
named output file remained absent:

```sh
env -u SOTC_LOGO_PS_MSL -u SOTC_LOGO_PS_PNG -u SOTC_LOGO_PS_TEXTURE_PNG -u SOTC_LOGO_PS_TEXTURE_MUTATION_PNG -u SOTC_LOGO_PS_CUBE_CMSH -u SOTC_LOGO_PS_CONE_CMSH -u SOTC_LOGO_PS_MESH_OUTPUT_PNG -u SOTC_LOGO_PS_MESH_MUTATION_PNG SOTC_LOGO_PS_TEXTURE_OUTPUT_PNG='/Volumes/Extra/SoC Mac Native/local/validation/gpu03-logo-png-texture-20261025/partial-output-only.png' ctest --test-dir local/build/orch-r22 -R '^metal_texture_binding$' --output-on-failure -V
```

The command exited 8 because the test correctly reported the incomplete replay configuration. The
log is `local/validation/gpu03-logo-png-texture-20261025/partial-config-negative.log`; the optional
PNG did not exist afterward.

## Asset-derived fixture result

The exact configured fixture command passed CTest test 378 (`metal_texture_binding`) 1/1 with Metal
API validation and GPU shader validation enabled:

```sh
env SOTC_LOGO_PS_MSL='/Volumes/Extra/SoC Mac Native/local/validation/gpu02-logo-ps-image-sample-20261025/logo-ps.metal' SOTC_LOGO_PS_PNG='/Volumes/Extra/SoC Mac Native/local/validation/gpu03-logo-png-texture-20261025/logo-ps-synthetic.png' SOTC_LOGO_PS_TEXTURE_PNG='/Volumes/Extra/SoC Mac Native/local/validation/gpu03-logo-png-texture-20261025/saveshadow_scee.png' SOTC_LOGO_PS_TEXTURE_OUTPUT_PNG='/Volumes/Extra/SoC Mac Native/local/validation/gpu03-logo-png-texture-20261025/asset-derived.png' SOTC_LOGO_PS_TEXTURE_MUTATION_PNG='/Volumes/Extra/SoC Mac Native/local/validation/gpu03-logo-png-texture-20261025/asset-derived-mutated.png' SOTC_LOGO_PS_CUBE_CMSH='/Volumes/Extra/SoC Mac Native/local/orchestrator/gpu03-r31/asset-derived/cube_1m.cmsh' SOTC_LOGO_PS_CONE_CMSH='/Volumes/Extra/SoC Mac Native/local/orchestrator/gpu03-r31/asset-derived/cone.cmsh' SOTC_LOGO_PS_MESH_OUTPUT_PNG='/Volumes/Extra/SoC Mac Native/local/validation/gpu03-logo-png-texture-20261025/mesh-asset-derived.png' SOTC_LOGO_PS_MESH_MUTATION_PNG='/Volumes/Extra/SoC Mac Native/local/validation/gpu03-logo-png-texture-20261025/mesh-asset-derived-mutated.png' ctest --test-dir local/build/orch-r22 -R '^metal_texture_binding$' --output-on-failure -V
```

The CTest log is `local/validation/gpu03-logo-png-texture-20261025/composite-metal-ctest.log`.
Parsed inputs:

| Member | Bytes | SHA-256 | POS0 positions | U16 indices |
|---|---:|---|---:|---:|
| `cube_1m.cmsh` | 850 | `44738dbf663af252898bad5582f2b48f1aadf6fbaab403c20880410b79a380b7` | 24 | 36 |
| `cone.cmsh` | 1,462 | `5456afadabe96b9f7f02c503dc0ed5410652ccd32601c19e8aeeaaf36c88600c` | 42 | 114 |

| Mesh | Output pixel | Source texture texel | Minimum triangle barycentric |
|---|---:|---:|---:|
| Cube | `(20,30)` | `(93,65)` | `0.272` |
| Cone | `(43,27)` | `(134,68)` | `0.297` |

The CPU oracle checks, deterministic repeat, and selected-texel mutation assertions passed. Private
64x64 RGBA PNG SHA-256 values:

- Baseline: `5aa812cfd4db7056c7cb7c9193a7edc445e5e1ede23fa110b83580f667959154`
- Mutated: `a6e72b7044326b63619e7e64d4ef9ebc70e3421d61a14643e1d6c313bbb7c57c`

Both images and all logs remain under ignored
`local/validation/gpu03-logo-png-texture-20261025/`.

## Repository gates completed so far

| Command | Result | Log |
|---|---|---|
| `tools/dev/with_build_lock.sh cmake -S native -B local/build/orch-r22` | PASSED | `local/validation/gpu03-logo-png-texture-20261025/full-configure-composite.log` |
| `tools/dev/with_build_lock.sh cmake --build local/build/orch-r22 -j 12` | PASSED, no work to do | `local/validation/gpu03-logo-png-texture-20261025/full-build-composite.log` |
| `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure` | PASSED, 397/397 | `local/validation/gpu03-logo-png-texture-20261025/full-ctest-composite.log` |
| `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` | PASSED, 481/0/0 | `local/validation/gpu03-logo-png-texture-20261025/lift-checkers-composite.log` |
| `trail build` | PASSED, 8,546 nodes / 11,572 edges / 1,497 cards across 2,002 files | `local/validation/gpu03-logo-png-texture-20261025/trail-build-composite.log` |
| `graft grep 'find_mesh_logo_probe'` | PASSED; helper and both call sites are queryable | command output in task session |
| `trail build --check` | PASSED; wiring graph is in sync; optional deep meaning layer not built | `local/validation/gpu03-logo-png-texture-20261025/trail-check-composite.log` |
| `python3 scripts/project_index.py` | PASSED, 19,590 folders | `local/validation/gpu03-logo-png-texture-20261025/project-index-composite.log` |
| `python3 -I scripts/audit_publish_paths.py --all` (pre-staging) | PASSED, 2,791 tracked paths; rerun staged/all after staging the new checkpoint | `local/validation/gpu03-logo-png-texture-20261025/all-path-audit-composite.log` |

## Limits and next action

This verifies the combined pinned inputs through this direct-Metal harness only. TEX0/NRM0
interpretation, retail UV or vertex bindings, retail orientation, AMD WQM/helper-lane equivalence,
game-driven rendering, and G3 remain unverified. The evidence does not establish boot, game audio,
or save/reload.

Next exact action: rebuild/check the Trail graph and query the new fixture helpers; run the project
index, required publish-path and attribution audits, then stage only allowlisted source/docs and
publish to `main` as Johnny Wang with no Claude/Anthropic contributor trailer. Capture final gate
counts and commit/push SHAs in this checkpoint and `HANDOFF.md`.
