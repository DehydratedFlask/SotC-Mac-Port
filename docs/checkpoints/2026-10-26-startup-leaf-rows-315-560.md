# Startup leaf rows 315-560

Local date/time: 2026-10-26 23:26 CDT (America/Chicago). Agent: `/root` (integrator replay); implementation worker `/root/leaf_rows_review`.

Task: `CPU-01-STARTUP-LEAF-ROWS-315-560-01`.

## Implementation

Registered the 11 already-lifted rows in `native/core/startup_ctors.cpp` using their pinned RVAs:
315 `0xa3ade0`, 323 `0x842450`, 341 `0x7dace0`, 357 `0x78c760`, 378 `0x6dde20`, 394 `0x669450`,
412 `0x58bd10`, 416 `0x5777a0`, 496 `0x268ce0`, 500 `0x24ebf0`, and 560 `0x3bd70`.

State members model the exact 32-bit/8-bit/12-byte/16-byte/128-bit writes, the 64-bit atomic clear,
and the borrowed hashed-name cells. Name arrays preserve the pinned order:

- 323: `BowShotsHit`, `BowShotAccuracy`, `FurthestArrowDistance`
- 341: `InEndCutscene`, `TimeResistingTheLight`
- 378: `InSaveVolume`, `LastInSaveVolume`, `FacingSaveVolume`
- 394: `IsCutsceneRunning`

Rows remain in original ascending index order. Coverage pins and tests now expect **397** registered
rows and **176** unregistered rows; the first gap remains **309 / `0xa505e0`**. The coverage test
poisons destination cells before dispatch and checks each destination, exact string/hash pairs, and
sentinel fields not written by the helpers. `native/CMakeLists.txt` adds the existing
`zero_struct12.cpp` implementation to `sotc_core`, because the first focused link showed the
startup executables could not resolve the slot-500 helper until the core target owned that object.

## Root replay and checks

| Command | Result | Evidence |
|---|---|---|
| `trail build --only-dir scripts --only-dir tools --only-dir reference --only-dir native --only-dir launcher --only-dir native/tests --only-dir tests --follow-nested-repos` | PASSED, 2,143 files; 9,872 nodes / 13,447 edges | local Trail wiring cache |
| `trail build --check` | PASSED, graph in sync | source graph check |
| `cmake -S native -B local/build/orch-r22` | PASSED, exit 0 | `local/validation/orch-20261026/leaf-rows-gates/configure.log` |
| `cmake --build local/build/orch-r22 -j 12` | PASSED, 433/433 targets | `local/validation/orch-20261026/leaf-rows-gates/build.log` |
| `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure` | PASSED, **431/431** | `local/validation/orch-20261026/leaf-rows-gates/ctest.log` |
| `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` | PASSED, **516 / 0 / 0** | `local/validation/orch-20261026/leaf-rows-gates/lift-checkers.log` |
| `git diff --check` | PASSED | no whitespace errors |

The targeted 4-test run and the 11 per-slot checker results were also reported by the implementation
worker; the integrator then reran the complete suite and ELF checker sweep above. The original
focused link failure was a missing CMake source membership, not a constructor contract mismatch;
adding that helper source resolved it.

## Status and limits

The task is **FIXTURE_VERIFIED + STATIC_ANALYSIS_VERIFIED**. The startup registry now covers
397/573 constructor slots. The rows do not close the first gap or advance it beyond 309. G1 remains
**PARTIAL**; this work makes no cold-boot, game-audio, real-save/reload, or Metal-rendering claim.
Next exact action: run publication audits, commit the allowlisted implementation/docs as the owner,
push `origin main`, then choose the next bounded startup task.
