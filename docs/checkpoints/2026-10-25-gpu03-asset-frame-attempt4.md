# GPU-03 L1 asset-derived frame — index-mutation attempt 4

**Date:** 2026-10-25 04:33 CDT
**Integrator/principal:** Codex `/root`
**Task:** `GPU-03-RENDEROPS-ASSET-FRAME-01`
**Status:** **FIXTURE_VERIFIED** for the asset-derived harness only; G3 remains **PARTIAL**.

## Principal reassessment

The original E640 attempt recorded three failed candidate searches. Each changed one U16 index value,
but the last candidate filter compared only CPU coverage and front-draw ownership. The pinned private
default-pair MSL exports transformed position through vertex `param0` and uses the perspective-correct
interpolated value in fragment lighting. An index change can therefore preserve both masks while
changing shader inputs and rendered color. Root recorded this fourth hypothesis and its observable in
`HANDOFF.md` and the GPU-03 escalation before implementation.

The final CPU projection stores each winning fragment's perspective-correct `param0`. The index search
now renders only a valid one-index candidate when coverage and front-draw ownership match the baseline
and at least one visible winning fragment's `param0` differs by more than `1e-6`.

## Exact verification

Build:

```sh
cmake --build local/build/orch-r22 --target test_metal_asset_frame -j 12
```

Result: exit 0. Logs: `local/validation/gpu03-r31/attempt4-build.log` and
`local/validation/gpu03-r31/attempt4-final-build.log`.

Asset-derived Metal replay:

```sh
env MTL_DEBUG_LAYER=1 \
  MTL_SHADER_VALIDATION=1 \
  MTL_SHADER_VALIDATION_DEFAULT_STATE=all \
  MTL_SHADER_VALIDATION_ENABLE_ERROR_REPORTING=1 \
  MTL_SHADER_VALIDATION_REPORT_TO_STDERR=1 \
  MTL_SHADER_VALIDATION_ABORT_ON_FAULT=1 \
  SOTC_DEFAULT_PAIR_MSL=local/validation/gcn-default-root-v1/default-pair.metal \
  local/build/orch-r22/test_metal_asset_frame --asset-derived \
  local/orchestrator/gpu03-r31/asset-derived/cube_1m.cmsh \
  local/orchestrator/gpu03-r31/asset-derived/cone.cmsh \
  local/validation/gpu03-r31/asset-derived-final.png
```

Result: exit 0; Metal API validation and GPU validation were enabled, with no fault reported. Both
private raw members matched their pinned byte sizes and SHA-256 values: cube 850 B / 24 positions /
36 indices / `44738dbf663af252898bad5582f2b48f1aadf6fbaab403c20880410b79a380b7`; cone 1,462 B /
42 positions / 114 indices / `5456afadabe96b9f7f02c503dc0ed5410652ccd32601c19e8aeeaaf36c88600c`.

Observed baseline: 256x256 RGBA, 3,147 B PNG, SHA-256
`af42ca554f5d24a9f981fc92156dea50616c172af1faba12eef1ff0d4db0f645`; 8,464 covered pixels,
6,584 projected overlap, 0 silhouette errors, and 0 depth-class errors. Repeat output was
deterministic; projected bounds, vertex mutation, and shader-constant mutation checks passed.

Observed index mutation: cone draw 1, U16 slot 33, value 14 → 23. CPU coverage and front-draw
ownership were identical to baseline; interpolated `param0` changed on 677 visible pixels. Metal PNG
SHA-256 changed to
`e3384475049c0e174895c736edb7248c5925ea82ae3924db507efb302641432c`.

The private baseline PNG and build/replay logs remain under `local/validation/gpu03-r31/`; no game
asset bytes, generated MSL, PNG, or logs are staged.

## Full-suite and pre-publication verification

- `cmake -S native -B local/build/orch-r22`: passed;
  `local/validation/gpu03-r31/full-configure.log`.
- `cmake --build local/build/orch-r22 -j 12`: passed;
  `local/validation/gpu03-r31/full-build.log`.
- Full CTest with absolute `SOTC_LOGO_PS_MSL` and `SOTC_LOGO_PS_PNG` set: **397/397 passed**;
  `local/validation/gpu03-r31/full-ctest.log`.
- `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh`:
  **481 passed, 0 skipped, 0 failed**; `local/validation/gpu03-r31/lift-checkers.log`.
- Project index: **19,589 folders**. Trail wiring graph rebuilt from 2,001 files to **8,522 nodes /
  11,526 edges**; `trail build --check` passed, and `trail grep` returned the new native interpolant
  helper. Deep/concept layer remains unbuilt and is not required for this wiring check.
- Pre-publication path audit: staged **8/8** allowlisted paths; all-path audit **2,788/2,788**.
  The first `git diff --cached --check` found trailing Markdown hardbreak spaces in this checkpoint;
  they were removed and the final cached-diff check passed. The owner identity audit passed for
  `origin/main..HEAD` before commit.

## GitHub publication

Owner commit `0f67022cea442727326af7b675942929add1f4c0` (`test(metal): validate asset index mutations
through interpolants`) was pushed to GitHub `main` per the user's instruction. After a fresh fetch,
local `main` and `origin/main` both resolve to this SHA and the worktree is clean. The commit author
and committer are Johnny Wang (`dehydratedflask@users.noreply.github.com`); it contains no
Claude/Anthropic coauthor trailer. [E651](../EVIDENCE.md) records publication.

## Limits and next state

This is an asset-derived harness using the real local cube and cone POS0/U16 bytes and the explicit
synthetic default-pair shader ABI. It does not verify retail attribute/resource bindings, WQM/helper
lane equivalence, a game-driven frame, G3 PASS, boot, audio, or save/load. GPU-03's bounded L1
acceptance checks are now fixture-verified; no fifth index-mutation attempt is planned.
