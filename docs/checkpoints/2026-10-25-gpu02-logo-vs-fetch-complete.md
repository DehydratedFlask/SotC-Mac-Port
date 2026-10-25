# GPU-02 logo vertex fetch lowering completion

**Date:** 2026-10-25 02:03 CDT
**Integrator:** Codex `/root`
**Task:** `GPU-02-GCN-LOGO-VS-FETCH-01`
**Status:** `FIXTURE_VERIFIED`; attempt 1; 0 failed shader-semantic hypotheses

## Scope and changes

The pinned 140-byte logo vertex shader contains two `buffer_load_format_xy` fetches and one
`buffer_load_format_xyzw` fetch. `tools/dev/gcn_to_msl.py` now lowers explicit
`explicit_float2`, `explicit_float3`, and `explicit_float4` synthetic stream views and writes exactly
the fetched number of destination VGPR components. Existing descriptor, one-vertex-per-case index,
offset, and MUBUF flag restrictions remain in place. Unsupported or mismatched stream ABIs fail
conversion. `tools/dev/test_gcn_to_msl.py` covers width behavior, untouched trailing destination
registers, and invalid format, binding, index, and fetch contracts.

The private input is `local/fixtures/gpu-r30/logo-vs.bin`, 140 bytes, SHA-256
`430f9adbb07d387c0b2b877b98300396f183faeab7f75cba0c9b0c67ada20c0c`. Its inventory contains 25
instructions, 0 unknown instructions, and an exact end. The private ABI maps descriptor dword offsets
0, 4, and 12 to synthetic float streams; no retail descriptor values or semantic names are inferred.
The already-verified default pixel shader served only as an execution sink; it is not the logo PS.

## Verification

| Command | Result | Evidence |
|---|---|---|
| `python3 -I -W error tools/dev/test_gcn_to_msl.py` | **PASSED**, 14/14 | `local/orchestrator/gpu02-logo-vs-fetch/final-lowerer.log` |
| `python3 -I -W error tools/dev/test_gcn_inventory.py` | **PASSED**, 23/23 | `local/orchestrator/gpu02-logo-vs-fetch/final-inventory.log` |
| `python3 -I local/orchestrator/gpu02-logo-vs-fetch/run_private_metal.py` | **ARTIFACT_VERIFIED**; Apple M4 Pro; 1,024 cases; compiled MSL; compute and offscreen execution completed; Metal API/GPU validation enabled; 12,288 exact components, 0 mismatches against the separate CPU oracle; `fastMath=NO` | `local/orchestrator/gpu02-logo-vs-fetch/metal-oracle-final.log`, report and generated MSL in the same private directory |
| `cmake -S native -B local/build/orch-r22` | **PASSED**, exit 0 | `local/orchestrator/gpu02-logo-vs-fetch/configure-final.log` |
| `cmake --build local/build/orch-r22 -j 12` | **PASSED**, exit 0 (`ninja: no work to do`) | `local/orchestrator/gpu02-logo-vs-fetch/build-final.log` |
| `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure` | **PASSED**, 392/392 | `local/orchestrator/gpu02-logo-vs-fetch/ctest-final.log` |
| `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` | **PASSED**, 476 passed / 0 skipped / 0 failed | `local/orchestrator/gpu02-logo-vs-fetch/lift-checkers-final.log` |

Generated MSL SHA-256: `316b5505512c6f377415f486d7e5c4abd983a9aff56061efb107c14ab800226d`.

## Correction and interpretation

The first synthetic width-1 fixture used an opcode rejected by the inventory decoder, so conversion
did not reach the lowerer. The test was corrected to use a valid width-2 opcode with an
`explicit_float1` ABI mismatch and now verifies the fail-visible conversion path. This was fixture
construction correction, not a failed shader-semantic hypothesis.

This proves only the pinned shader bytes under a deliberately synthetic float stream ABI. It does not
prove retail descriptor values, attribute semantics, logo pixel shader WQM/MIMG behavior, actual asset
bindings, an asset-derived frame, game boot/audio/saving, or G3. G3 remains `PARTIAL`.

## Next action

Complete the owner-requested GitHub PR review/repair and merge sequence. PR #5 is held on the confirmed
BucketAllocator/HeapAllocator alignment defect. Then start GPU-03 L1 using the owner-approved raw local
`.cmsh` inputs, harness normal `[0.5, 0.5, 1.0]`, and direct private PNG output; do not supply an engine
frame number or call `attach_screenshot`.
