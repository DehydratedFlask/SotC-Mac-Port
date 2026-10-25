# GPU-03 native raw mesh reader checkpoint

**Date/time:** 2026-10-24 15:30 CDT
**Integrator:** Codex `/root`
**Worker:** `/root/gpu03_cmsh_native`
**Task:** `GPU-03-CMSH-RAW-POSIDX-01`, attempt 0
**Parent task:** `GPU-03-RENDEROPS-ASSET-FRAME-01`, still ACTIVE at attempt 0.

## Scope and files

The parser accepts the bounded raw `.cmsh` subset: POS0 float3 positions and a U16 triangle-list
tail. It owns the decoded arrays and returns named errors with byte offsets. NRM0, TAN0 and TEX0
payloads/formats are only range-checked and remain opaque. The build integration is a separate
`sotc_cmsh_mesh` library plus `test_cmsh_mesh` CTest target.

Changed source files:

- `native/CMakeLists.txt` (root integration)
- `native/platform/cmsh_mesh.hpp`
- `native/platform/cmsh_mesh.cpp`
- `native/tests/test_cmsh_mesh.cpp`

Changed documentation files:

- `HANDOFF.md`
- `docs/TASKS.md`
- `docs/EVIDENCE.md` (E630)
- `docs/INDEX.md`
- `docs/FOLDERS.md` (regenerated)
- this checkpoint

## Exact commands and results

Worker fixture check:

```sh
clang++ -std=c++20 -Wall -Wextra -Wpedantic -Werror -I native/platform \
  native/platform/cmsh_mesh.cpp native/tests/test_cmsh_mesh.cpp \
  -o local/orchestrator/gpu03-r31/cmsh-native/test_cmsh_mesh
local/orchestrator/gpu03-r31/cmsh-native/test_cmsh_mesh
```

Both commands exited 0. The second printed `cmsh fixture checks: PASSED`. Logs are
`local/orchestrator/gpu03-r31/cmsh-native/compile.log` and `fixture.log`.

Root CMake integration:

```sh
cmake -S native -B local/build/gpu03-r31
cmake --build local/build/gpu03-r31 --target test_cmsh_mesh -j 12
ctest --test-dir local/build/gpu03-r31 -R '^cmsh_mesh$' --output-on-failure
```

All exited 0; focused CTest passed 1/1. The full verification commands also exited 0:

```sh
cmake -S native -B local/build/orch-r22
cmake --build local/build/orch-r22 -j 12
ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure
SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh
python3 -I scripts/audit_publish_paths.py --all
python3 -I scripts/check_commit_attribution.py
```

Full CTest: 389/389 passed. Lift checkers: 476 passed, 0 skipped, 0 failed. The working-tree path
audit covered 2,734 paths. The pre-commit attribution audit passed for
`origin/cursor/init-hash-551140-9dfc..HEAD`. Exact output is saved under
`local/orchestrator/gpu03-r31/verification/`.

After adding this checkpoint, `python3 scripts/project_index.py` exited 0 and indexed 9,020
folders. `trail build --check` exited 0; all 8,231 wiring nodes are in sync (the optional meaning
tier remains unbuilt). `git diff --check` exited 0. Logs: `project-index.log`, `trail-check.log` and
`diff-check.log` in the same verification directory.

Final pre-commit `git diff --cached --check` passed; the staged path audit passed all 10 staged
paths; the final full path audit passed 2,738 paths; and the attribution audit passed. Logs are
`staged-diff-check.log`, `staged-path-audit.log`, `all-path-audit-final.log`, and
`attribution-final.log` in the verification directory.

## Evidence limits

- E629 remains the only live-sample claim: the existing `gpu_asset_probe.py cmsh` replay outputs pin
  the two member hashes, sizes and vertex/index counts. Per the card-4 boundary, the native parser
  was tested only with synthetic bytes; no real `.cmsh` member bytes were passed to it.
- POS0 descriptor format code 2 is enforced by the parser and covered by a synthetic fixture. The
  replay summaries do not expose this field, so it is not independently verified for the two real
  members.
- A secondary private export was removed after scope review, before parser use. The original PSARC
  archives were not modified. No parser, renderer, G3, game-frame, boot, audio or save/load claim
  follows from that export.
- A worker's first standalone compile reported an unused-variable warning under `-Werror`; removing
  that variable produced a clean compile. This was a local compile correction, not a separate
  implementation hypothesis. No failed implementation hypothesis is counted.

## Observations, risks and next action

Observed: malformed synthetic inputs fail visibly; the no-argument fixture executable and CMake
test pass. Inference: the parser's raw descriptor/span model follows the live probe's bounded simple
layout. Risk: real-member format code and parser behavior remain unverified because the card limits
real checks to the existing hash/count outputs. Retail vertex fetch, normals and shader bindings
also remain unverified.

**Status:** `GPU-03-CMSH-RAW-POSIDX-01` is `FIXTURE_VERIFIED`; parent GPU-03 remains `ACTIVE`.
**Next exact action:** define the typed `RenderOp` subset and its fail-visible Metal execution path,
then render both admitted meshes using only the explicitly labeled L1 harness ABI. Record whether the
user selects fixture normals or NRM0 and which screenshot-seam frame labeling to use before wiring
those choices. Keep G3 at PARTIAL at best.
