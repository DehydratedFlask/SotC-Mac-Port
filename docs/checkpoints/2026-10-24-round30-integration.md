# Round 30 integration checkpoint — 2026-10-24

**Time:** 15:02 CDT

**Integrator:** Codex `/root`

**Task:** `INT-R29-R30-VERIFY-01`, integrating `GPU-01-CSDR-READER-01` and the user-selected
default-pair slice of `GPU-02-GCN-SUBSET-DEFAULT-PAIR-01`.
**Branch/base:** `cursor/init-hash-551140-9dfc`, source commit
`95653223f279bbaee5fb68743d415039e1a46f61` on top of `9f79b650844765378c54a696b1a4abc4315194e4`.

## Changes and evidence

- The CSDR reader attempt 3 is complete. Its parser validates the complete raw container before
  compressed-prefix fallback, including the documented `cc xx` wildcard. The independent reviewer
  found no P0-P2 issue and checked all 256 short `ccXX` prefixes plus valid raw count collisions.
  Root's complete v5 archive replay exited 0 with **14,967 accepted raw members**, **4 named,
  structurally checked type-5/6 exceptions**, **19,320 compressed-prefix rejections**, **21,861
  accepted program rows**, and **34,291 member rows**. The four exception paths are recorded in
  the private summary, not repeated here. The summary has zero native errors and preserves archive,
  source, and manifest hashes.
- The default shader pair has an offline GFX7 decoder and bounded SSA-to-MSL emitter, a standalone
  Metal harness, and a separate equation-based CPU oracle. The harness ABI is explicit and applies
  only to the supplied synthetic inputs. The logo pixel shader's whole-quad path remains unresolved.
  Independent review found no P0-P2 issue. Root regenerated the MSL and matched the worker artifact
  byte-for-byte, compiled the harness with Apple clang, and reran Metal with API/GPU validation and
  fast math disabled on Apple M4 Pro. The **2,141-case** compute and offscreen executions completed;
  **34,256 components** matched the independent oracle with maximum finite absolute difference 0;
  all **2,131 finite alpha** values matched exactly. Separate CPU projection checks report 4,608
  covered pixels and no color mismatch; this is synthetic input, not a game frame.
- MAD/MAC lowering was checked against the AMD Sea Islands ISA 1.3 and LLVM AMDGPU backend. The
  V_MAD entry explicitly specifies ADD after MUL_IEEE; V_MAC is the accumulator form and the cited
  LLVM handling corroborates separate-operation arithmetic. The harness profile selects nearest-even
  binary32 with FTZ and binary16 round-toward-zero; the retail MODE state is unverified. The official
  AMD source is [Sea Islands Series ISA, revision 1.3](https://www.amd.com/content/dam/amd/en/documents/radeon-tech-docs/instruction-set-architectures/sea-islands-instruction-set-architecture_0.pdf).
- Asset reconciliation, startup, and launcher worker results are recorded separately in E621-E624
  and their linked checkpoints. They remain bounded evidence only; they do not pass boot, audio,
  saving, or launcher game-readiness gates.

## Root commands and results

All commands ran from the workspace root. Commands that build or run CTest used the shared build
lock. Exact output is retained under `local/orchestrator/r30/`.

| Command | Result | Log/evidence |
|---|---|---|
| `tools/dev/with_build_lock.sh cmake -S native -B local/build/orch-r22` | exit 0 | `final-cmake-configure.log` |
| `tools/dev/with_build_lock.sh cmake --build local/build/orch-r22 -j 12` | exit 0 | `final-cmake-build.log` |
| `tools/dev/with_build_lock.sh ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure` | exit 0, **388/388** | `final-ctest.log` |
| `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` | exit 0, **476 passed / 0 skipped / 0 failed** | `final-checkers.log` |
| `python3 -I -W error local/orchestrator/r30/assets-run-tests.py` | exit 0, **228/228** in 10 files | `final-asset-python-tests.log` |
| `python3 -I -W error tools/dev/test_gcn_inventory.py` | exit 0, **23/23** | `final-gcn-inventory-tests.log` |
| `python3 -I -W error tools/dev/test_gcn_to_msl.py` | exit 0, **11/11** | `final-gcn-msl-tests.log` |
| `swift test --package-path launcher --scratch-path local/build/launcher` | exit 0, **148/148** | `final-launcher-swift-tests.log` |
| `python3 -I tools/dev/verify_csdr_corpus.py --native local/build/orch-r22/csdr_inspect --output local/validation/csdr-reader-r30-root-v5 --expect-main-corpus local/extracted/CUSA08809/pak0.psarc local/extracted/CUSA08809/pak1.psarc local/extracted/CUSA08809/pakN.psarc` | exit 0, **ARTIFACT_VERIFIED** | `csdr-root-corpus-v5.log`, `local/validation/csdr-reader-r30-root-v5/summary.json` |
| `python3 -I tools/dev/gcn_to_msl.py --vs local/fixtures/gpu-r30/default-vs.bin --ps local/fixtures/gpu-r30/default-ps.bin --abi local/fixtures/gpu-r30/default-abi.json --output local/validation/gcn-default-root-v1/default-pair.metal --report local/validation/gcn-default-root-v1/default-lowering.json` | exit 0; `cmp` with worker MSL exit 0 | `root-default-lowering.log` |
| `xcrun clang++ -std=c++20 -fobjc-arc -framework Foundation -framework Metal tools/dev/metal_shader_verify.mm -o local/validation/gcn-default-root-v1/metal_shader_verify` | exit 0 | `root-metal-verify-build.log` |
| `python3 -I -W error tools/dev/default_shader_oracle.py fixture local/validation/gcn-default-root-v1` | exit 0, **2,141 cases** | `root-default-oracle-fixture.log` |
| `MTL_DEBUG_LAYER=1 MTL_SHADER_VALIDATION=1 local/validation/gcn-default-root-v1/metal_shader_verify local/validation/gcn-default-root-v1/default-pair.metal local/validation/gcn-default-root-v1/harness-inputs.bin local/validation/gcn-default-root-v1/metal-output.bin local/validation/gcn-default-root-v1/offscreen.rgba32f` | exit 0, compute and offscreen complete on Apple M4 Pro | `root-default-metal-execution.log` |
| `python3 -I -W error tools/dev/default_shader_oracle.py compare local/validation/gcn-default-root-v1` | exit 0, **34,256 components**, max finite absolute error **0**, **2,131** finite alpha exact | `root-default-oracle-comparison.log` |
| `python3 scripts/project_index.py` | exit 0, **8,211 folders indexed** | `final-project-index.log` |
| `trail build --check` | exit 0, wiring graph in sync; deep meaning summaries remain unbuilt | `final-trail-check.log` |
| `python3 -I scripts/audit_publish_paths.py --all` | exit 0, **2,675 paths** | `final-publish-audit-all.log` |
| `python3 -I scripts/audit_publish_paths.py` | exit 0, **90 staged paths** | `final-publish-audit-staged.log` |
| `python3 -I scripts/check_commit_attribution.py` | exit 0 for `origin/cursor/init-hash-551140-9dfc..HEAD` (pre-commit state) | `final-attribution-audit.log` |
| `git diff --check` / `git diff --cached --check` | both exit 0 | `final-diff-check.log` / `final-staged-diff-check.log` |

## Attempts, limitations, and next action

- CSDR history retains two failed hypotheses: type-2 stage inference and prefix fallback that
  preempted valid raw count collisions. Structural-first attempt 3 passed all targeted tests and the
  full corpus; it had no failed targeted check. There is no fourth attempt.
- The decoder inventory retains one failed mutation-harness assumption that changed an instruction
  family; it was corrected without changing decoder acceptance. The default-only lowering work
  retains two unsuccessful fixture/diagnostic checks, both corrected. No shader semantic hypothesis
  failed. `xcrun metal` is unavailable because the optional Metal Toolchain component is absent; the
  existing Metal.framework compiler and runtime succeeded, and no dependency was installed.
- LeakSanitizer is **NOT RUN**: explicit leak detection aborts on this host before fixture execution.
  The supported AddressSanitizer+UndefinedBehaviorSanitizer run had zero diagnostics.
- Actual retail vertex format, descriptors, interpolation mode, and FP MODE remain UNVERIFIED.
  There is no game-driven scene or G3 result. Native cold boot, audible game content, real save/load
  after relaunch, Metal validation on game frames, and G4 are **NOT RUN**.
- **Status:** CSDR importer `ARTIFACT_VERIFIED / FIXTURE_VERIFIED`; selected default-pair harness
  `ARTIFACT_VERIFIED / FIXTURE_VERIFIED`; combined renderer/G3 work remains partial. Integration
  task was completed and published in source commit `95653223f279bbaee5fb68743d415039e1a46f61`.
- **Publication:** effective author and committer are Johnny Wang
  (`dehydratedflask@users.noreply.github.com`). Commit message contains no co-author trailer;
  `python3 -I scripts/check_commit_attribution.py` exited 0 after the commit. The command
  `git push origin cursor/init-hash-551140-9dfc` exited 0 and advanced the branch from `9f79b65`
  to `9565322`. A fresh fetch, local HEAD, tracking ref and `git ls-remote` agree on the full
  revision. Private confirmation is in `local/orchestrator/r30/push-result.txt` and
  `postpush-{status,revisions,remote-ref}.log`.
- **Follow-on task:** `GPU-03-RENDEROPS-ASSET-FRAME-01` is now claimed by root at attempt 0 in
  [the active GPU-03 checkpoint](2026-10-24-gpu03-claim.md). It targets the L1 asset-derived
  `cube_1m`/`cone` draw only; the public round-30 source commit does not claim G3 or any real game
  gate.
