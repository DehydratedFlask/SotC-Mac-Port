# GPU-03 typed RenderOp API checkpoint

**Date/time:** 2026-10-24 15:37 CDT
**Integrator:** Codex `/root`
**Task:** `GPU-03-RENDEROPS-TYPED-API-01`, attempt 0
**Parent:** `GPU-03-RENDEROPS-ASSET-FRAME-01`, still ACTIVE.

## Scope and changed files

Added a pure C++20 typed operation list for the initial direct-Metal mesh slice. The list carries
host-assigned resource handles (never guest GPU addresses), render-pass begin/end, viewport, depth
state, pipeline, vertex/constants/index streams, and indexed draws. Named unsupported operation
kinds produce explicit diagnostics. Structural validation fails on invalid pass nesting, missing
state, invalid handles/spans/counts, or unsupported operations. This code does not execute Metal.

Changed files:

- `native/platform/render_ops.hpp`
- `native/platform/render_ops.cpp`
- `native/tests/test_render_ops.cpp`
- `native/CMakeLists.txt`
- `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/INDEX.md`, `docs/FOLDERS.md`
- this checkpoint

Parser subtask `GPU-03-CMSH-RAW-POSIDX-01` was committed and pushed as
`fa329cd3382b7aaf968ab86268366cc6033f8f4f`; local HEAD, tracking ref and origin were verified equal.
The owner-authored commit has no Claude/Anthropic co-author trailer.

## Exact commands and results

Focused RenderOp target:

```sh
cmake -S native -B local/build/gpu03-r31
cmake --build local/build/gpu03-r31 --target test_render_ops -j 12
ctest --test-dir local/build/gpu03-r31 -R '^render_ops$' --output-on-failure
```

All exited 0; focused CTest passed 1/1. Logs are under
`local/orchestrator/gpu03-r31/render-ops/`.

Full verification:

```sh
cmake -S native -B local/build/orch-r22
cmake --build local/build/orch-r22 -j 12
ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure
SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh
python3 -I scripts/audit_publish_paths.py --all
python3 -I scripts/check_commit_attribution.py
```

All exited 0: CTest 390/390; lift checkers 476 passed, 0 skipped, 0 failed. Logs are in
`local/orchestrator/gpu03-r31/verification/`.

After the checkpoint files were added, `python3 scripts/project_index.py` exited 0 and indexed
9,028 folders. `trail build --check` exited 0 with all 8,258 wiring nodes in sync; the optional
meaning tier remains unbuilt. `git diff --check` exited 0. Logs are `project-index.log`,
`trail-check.log` and `diff-check.log` in the verification directory.

Final pre-commit `git diff --cached --check` passed; the staged path audit passed all 10 staged
paths; the full path audit passed 2,742 paths; and the pre-commit attribution audit passed. Logs:
`staged-diff-check.log`, `staged-path-audit.log`, `all-path-audit-final.log` and
`attribution-final.log` in the verification directory.

## Evidence and limits

Observed: the independent structural fixture accepts a valid indexed depth pass and rejects missing
depth state, malformed viewport/index data, nested/unclosed passes, operations outside a pass, and
unsupported dispatch. Inference: this contract is sufficient to describe the first explicit
indexed-mesh backend slice. Risk: no resource table or Objective-C++ Metal executor is connected;
no GPU commands, asset pixels, screenshot seam, image hash, depth winner, mutation or validation
layer result exists yet.

No real `.cmsh` bytes were processed. E629 hash/count probe outputs remain the only real-member
evidence. The user is being asked whether that boundary may expand for card 4, and whether to use
fixture normals or decode NRM0 and how to represent the image evidence without inventing an engine
`frame_num`.

**Status:** typed API `FIXTURE_VERIFIED`; parent GPU-03 remains ACTIVE with 0 failed implementation
hypotheses. **Next exact action:** create the direct-Metal executor for this typed subset and a
private, explicitly labeled harness test; keep asset import, Metal validation, screenshot and game
claims bounded by the pending user choices and card-4 evidence rules.
