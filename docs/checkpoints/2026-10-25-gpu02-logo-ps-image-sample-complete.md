# GPU-02 logo pixel IMAGE_SAMPLE fixture completion

**Date:** 2026-10-25 04:16 CDT

**Integrator:** Codex `/root`

**Task:** `GPU-02-GCN-LOGO-PS-IMAGE-SAMPLE-01`

**Status:** FIXTURE_VERIFIED for the exact digest-pinned PS specialization under its synthetic ABI;
retail bindings, AMD WQM equivalence, and G3 remain unverified.

## E646: pinned profile and Metal replay

The private E634 shader slice is 152 bytes at `.csdr` file offset `0x4d8`; SHA-256 is
`4309caada7f114ec822f61646be536f50a8261a8c6e54f2ce709db7c70b8b39a`. The `logo-ps` profile accepts
that exact digest, validates 34 decoded instructions and the WQM save/enable/restore, single base
`IMAGE_SAMPLE`, descriptor-register, compressed-export, and end-of-program anchors, then emits
PS-only MSL for a strict synthetic slot-0 texture/sampler/coordinate ABI. This profile is a fixed
template gated by the exact shader digest, not a general row-by-row GCN translator. The shader bytes,
ABI manifest, decoded rows, generated MSL/report, PNG, and logs are private under `local/`.

The generated MSL compiled and executed through the existing direct-Metal harness. Implicit
derivatives selected red mip 1 at UV scale 1 and green mip 2 at scale 2; center and primitive-edge
pixels matched the independent oracle. Mutating the selected mip to blue changed the readback. A
second set of checks used exactly representable binary-fraction channel multipliers to exercise the
shader's fp16 round-toward-zero export; red, green, and blue selected-mip output matched the CPU byte
oracle. A 64x64 PNG was written directly under `local/validation/` (351 bytes, SHA-256
`267a344d7533777e675ba8360ca2e7cd2dcad5ff3b256d7c27f8867f4abc0fa8`).

## Commands and results

- `python3 -I -W error tools/dev/test_gcn_to_msl.py` — PASSED, 17/17.
- `python3 -I -W error tools/dev/gcn_to_msl.py --profile logo-ps --ps local/validation/gpu02-logo-ps-image-sample-20261025/logo-ps.bin --abi local/validation/gpu02-logo-ps-image-sample-20261025/logo-ps-abi.json --output local/validation/gpu02-logo-ps-image-sample-20261025/logo-ps.metal --report local/validation/gpu02-logo-ps-image-sample-20261025/logo-ps-lowering-report.json` — PASSED; exact private code digest accepted.
- Focused `ctest -V --test-dir local/build/orch-r22 -R '^metal_texture_binding$'` with absolute
  `SOTC_LOGO_PS_MSL` and `SOTC_LOGO_PS_PNG` paths — PASSED, 1/1, no device skip; Metal API/GPU
  validation enabled with no fault. Final log: `local/validation/gpu02-logo-ps-image-sample-20261025/logo-msl-metal-ctest-rerun3.log`.
- `cmake -S native -B local/build/orch-r22` — PASSED.
- `cmake --build local/build/orch-r22 -j 12` — PASSED.
- Full CTest with the same private MSL/PNG environment — PASSED, 397/397; log:
  `local/validation/gpu02-logo-ps-image-sample-20261025/full-ctest.log`.
- `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` — PASSED, 481 passed,
  0 skipped, 0 failed.
- `trail build` and `trail build --check` — PASSED; 8,519 nodes, 11,521 edges, graph in sync.
  The separate deep/concept layer remains unbuilt and is not used as the wiring gate.
- `python3 scripts/project_index.py` — PASSED; 19,589 folders indexed.
- `python3 -I scripts/audit_publish_paths.py` staged-path audit — PASSED, 14 paths;
  `python3 -I scripts/audit_publish_paths.py --all` — PASSED, 2,786 paths. The attribution audit is
  PASSED for `origin/main..HEAD` after setting the task branch's upstream to `origin/main`. The first
  no-upstream default audit scanned all history and flagged inherited base commit `f6604a86e60e`
  (`orchestrator@local`); that commit is already reachable from `origin/main` and is not part of this
  PR's unpublished range. This task's only unpublished commit is authored and committed by Johnny Wang.

## Failed attempts and reassessment

Three unsuccessful fixture/oracle hypotheses occurred: the derivative diagnostic was expected to
encode 0.5 as 128 but read 127; the first PS vertex adapter passed synthetic position W=0 and
produced no covered samples; and an added color-multiply oracle truncated exact normalized products
that Metal encoded as 128/64. The root reassessment and fourth focused run are recorded in
[`GPU-02-GCN-LOGO-PS-IMAGE-SAMPLE-01`](../escalations/GPU-02-GCN-LOGO-PS-IMAGE-SAMPLE-01.md). The
fourth check passed without weakening the shader or mip acceptance assertions.

## Limits and next exact action

Observed: exact private bytes pass the digest/structure gates, the fixed PS-only MSL compiles and
executes, tested synthetic image-sample/color-export outputs match the CPU oracle, and the full native
fixture suite passes. Not established: AMD WQM/helper-lane derivative equivalence, retail descriptor
or SPI interpretation, original texture data, an asset-derived or game-driven frame, or G3. Keep
GPU-03 `NEEDS_PRINCIPAL`, G3 `PARTIAL`, and make no boot/audio/save completion claim. PR #12 merged
the tested tree into `main` as `a07d45184c978bdd7a0a937be90c83f499fcca50`; GitHub and the local checkout
removed the task branch. See the [merge closeout](2026-10-25-gpu02-logo-ps-merge-pr12.md).
