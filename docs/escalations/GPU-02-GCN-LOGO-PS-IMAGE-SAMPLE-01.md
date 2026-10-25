# NEEDS_PRINCIPAL: GPU-02-GCN-LOGO-PS-IMAGE-SAMPLE-01 — resolved after principal reassessment

- Owner / principal / date: Codex integrator `/root`; root acted as principal for the bounded
  reassessment; 2026-10-25.
- Requirement and failing gate: Lower the E634-pinned logo pixel program to a PS-only MSL
  specialization for the explicitly synthetic slot-0 ABI, then match its implicit-sample and
  color-export behavior against an independent CPU oracle on Metal. Do not claim AMD WQM or retail
  binding equivalence.
- Input SHA256, source pins, original RVA / file offset / image base: private shader span is 152 B at
  `.csdr` file offset `0x4d8` (1240), SHA-256
  `4309caada7f114ec822f61646be536f50a8261a8c6e54f2ce709db7c70b8b39a` (E634). No runtime RVA is
  used; the original shader bytes, generated MSL, manifest, PNG and logs remain under ignored `local/`.
- Reproducer / expected result: run the private pinned profile with the local ABI manifest, then run
  `metal_texture_binding` with absolute `SOTC_LOGO_PS_MSL` and `SOTC_LOGO_PS_PNG` paths. Expected:
  exact digest/structure acceptance, MSL pipeline creation, mip 1/mip 2 selection, texture mutation,
  color multiplication/half-RTZ export and CPU-oracle agreement at tested pixels. Final focused and
  full CTest runs passed.
- Current tree changes and recovery location: local branch
  `codex/gpu02-logo-ps-image-sample-20261025`, based on `main` at
  `376c080fa2664497f0626871cacb8536a154a6f7`; source/docs changes are uncommitted at this checkpoint.
  Private replay and logs: `local/validation/gpu02-logo-ps-image-sample-20261025/`.

| Failed try | Distinct hypothesis/change | Exact command / environment | Actual result | Log/evidence |
|---|---|---|---|---|
| 1 | The mip fixture's exact `0.5` gradient channel should round to byte 128. | `ctest -V --test-dir local/build/orch-r22 -R '^metal_texture_binding$'` | Sampled mip colors matched; the computed diagnostic readback was 127. Replaced the oracle with the observed conversion for this derivative diagnostic. | `local/validation/gpu02-logo-ps-image-sample-20261025/mip-fixture-ctest-rerun1.log`; earlier E645 checkpoint. |
| 2 | The PS-only vertex adapter can pass the mesh position's W component through as clip W. | `SOTC_LOGO_PS_MSL=<absolute logo-ps.metal> SOTC_LOGO_PS_PNG=<absolute logo-ps-baseline.png> ctest -V --test-dir local/build/orch-r22 -R '^metal_texture_binding$'` | MSL compiled but the synthetic renderer supplies position W as 0, producing clear-black pixels and five oracle failures. Adapter now explicitly constructs harness clip W=1. | `local/validation/gpu02-logo-ps-image-sample-20261025/logo-msl-metal-ctest-absolute.log`; corrected replay `logo-msl-metal-ctest-rerun1.log`. |
| 3 | Exact binary products such as 0.5 and 0.25 should be truncated during RGBA8 output conversion. | Same absolute-path replay with per-channel multipliers; see `logo-msl-metal-ctest-rerun2.log`. | Shader values matched exact fp16-RTZ inputs, while output bytes were 128 and 64; the new oracle expected 127 and 63. Root reassessment found an oracle-only nearest-byte conversion mismatch. | `local/validation/gpu02-logo-ps-image-sample-20261025/logo-msl-metal-ctest-rerun2.log`; successful reassessed replay `logo-msl-metal-ctest-rerun3.log`. |

- Smallest failing fixture and how to run it: private shader slice `local/validation/gpu02-logo-ps-image-sample-20261025/logo-ps.bin`, closed ABI manifest `logo-ps-abi.json`, and the existing synthetic quad/mip chain in `native/tests/test_metal_texture_binding.cpp`. The final replay command is recorded in the completion checkpoint.
- Verified facts versus guesses: verified exact input length/digest, 34 decoded rows, MSL pipeline creation, mip and channel readback values, CPU oracle, direct PNG, Metal API/GPU validation, and passing full CTest/lift checks. The root cause of each failed check is directly visible in the corresponding output. No inference about AMD helper-lane behavior or retail attribute/descriptor meaning follows.
- Why the previous approaches failed / missing information: try 1 and try 3 encoded different output-byte quantization assumptions for different floating-point paths; try 2 violated the renderer's synthetic clip-position contract. None showed an image-sample or channel-dataflow mismatch.
- Independent work that can continue: publish the allowlisted source/docs through a reviewed PR and return the local checkout to `main`. GPU-03 remains `NEEDS_PRINCIPAL`; this task does not change it.
- Task register and HANDOFF checkpoint links: [TASKS](../TASKS.md), [HANDOFF](../../HANDOFF.md), [E646 completion](../checkpoints/2026-10-25-gpu02-logo-ps-image-sample-complete.md).
- Principal reassessment / authorized next approach: after try 3, root reviewed the actual `128`/`64` readback against the exact binary-fraction shader products. Root classified the failure as the RGBA8 CPU-oracle conversion only, retained the mip/color and fp16-RTZ checks unchanged, and authorized one focused rerun using nearest-byte conversion. That fourth focused replay passed 1/1; the full suite also passed 397/397 with the private MSL enabled. The task is no longer blocked and is recorded as **FIXTURE_VERIFIED** in E646.
