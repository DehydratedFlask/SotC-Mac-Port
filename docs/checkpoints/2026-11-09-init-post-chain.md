# 2026-11-09 — Init post-base chain executes native bodies

**Task:** `CPU-01-INIT-POST-CHAIN-01`
**Agent:** Buffy
**Status:** PASSED / ARTIFACT_VERIFIED
**Gate:** G1 **PARTIAL** (unchanged status); G0/G3/G4 PARTIAL; G2/G5/G6 NOT RUN

## Files changed

- `native/core/init_post_bodies.{hpp,cpp}` — native `0x56dcb0`, `0x5c64c0`, `0x7f7220`
  bodies with explicit hooks and named blocked stages.
- `native/core/init_window_runtime.{hpp,cpp}` — host cells/hooks, real `0x1290`
  containers, per-site results and chain status.
- `native/app/sotc_init_window_main.cpp` — schema `sotc.init_window.v4` and per-site JSON.
- `native/tests/test_init_post_bodies.cpp`, `native/CMakeLists.txt` — focused fixture and
  `init_post_bodies` CTest.
- `native/core/init_531d60_post_base_{56dcb0,5c64c0,1290,7f7220,56db70,5b3d00,5b3cf0,56fad0,f90310}.hpp`,
  `init_1290.hpp`, `init_531d60_post_base_f90310.cpp`,
  `appmain_init_ctor_98a90_slice.hpp` — comment/state corrections.
- `docs/evidence/2026-11-09-init-post-chain.md` — evidence artifact.
- `docs/EVIDENCE.md`, `docs/TASKS.md`, `ROADMAP.md`, `HANDOFF.md`,
  `docs/VALIDATION.md`, `docs/G1_EVIDENCE.md` — records updated.

## Exact commands

```sh
cmake --build local/build/orch-r22 -j 12 --target sotc_init_window test_init_post_bodies
SOTC_INPUTTABLE_NAMES_FIXTURE=local/fixtures/inputtable_551140_names.bin \
  ./local/build/orch-r22/sotc_init_window
ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure
git diff --check
python3 scripts/project_index.py
python3 -I scripts/audit_publish_paths.py --all
```

## Result

- Build exit **0**; runtime exit **0**; stderr 0 B.
- `post_chain_complete=true`, `post_chain_blocked=""`; 275 registry entries, status 0,
  boss flag `0xA7`, `first_failed_stage=""`.
- `0x1290`: A/B containers inserted 1/1, vcall 1.
- `0x56dcb0`: 5 calls; descriptor `0x101e7ac`/`0x030a`.
- `0x5c64c0`: 9 calls/tail; BSS hashes `0x19edac14`/`0x75a104c5`/`0x8428e931`;
  host cell sentinel `0x5c64c0`.
- `0x7f7220`: branch false; `(0,0x128b8ac)` and `(1,0x128b892)`.
- `0x56dca0`/`0x56db70`/`0x5b3d00`/`0x5b3cf0`/`0x56fad0`: one native empty-ret call each
  (`0x56dca0` runs in entry-gap span C).
- `0xf90310`: one splash-hide host-service call.
- Full CTest **702/702**, exit **0** (final log `...-ctest-final.log`).
- Final build exit **0**; `git diff --check` exit **0**; project index **114,651 folders**
  exit **0**; publish-path audit **OK (4,110 paths)** exit **0**; **0 staged**.

## Observations vs inference

Observed: the runtime JSON fields above came from the real `0x531d60` entry. Inferred/contract:
the helper bodies, host source/BSS cells, singleton owners and Sony splash-hide service are
host-owned; the native bodies model the measured instruction sequences, not guest execution.
The one-byte `ret` targets have no state to observe; their native empty bodies are exact for
their observable behavior. `0xf90310` is explicitly not a lifted game body.

## Remaining risks

- `0x56dcb0`/`0x5c64c0`/`0x7f7220` helper/callee bodies remain host seams.
- `0xf90310` remains a platform service seam, not a recovered Sony body.
- `0x1290` singleton owners remain host-owned.
- `0x499f80` replacement branch and the engine/capture/savedata contracts remain as in E1424/E1425.
- No cold boot, audio, save/reload or Metal validation claim; G1 stays PARTIAL.

## Next exact action

Take the next runtime gate (cold boot path) or lift one of the named host seams with real RE;
do not start another constructor batch and do not relabel a host seam as a native body.

**Attempts:** 0.
