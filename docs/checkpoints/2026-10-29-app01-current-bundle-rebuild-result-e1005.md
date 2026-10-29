# E1005 — Current launcher bundle artifact result

**Date:** 2026-10-29 13:19 CDT
**Agent:** `/root`
**Task:** `APP-01-CURRENT-BUNDLE-REBUILD-01`
**Status:** `ARTIFACT_VERIFIED`

## Result

Built a current-source release bundle at `local/build/e1004-launcher/SotC Launcher.app` with a standalone Release core at `local/build/e1004-native/SoCGameCore`.

- CMake configure: `cmake -S native -B local/build/e1004-native -DCMAKE_BUILD_TYPE=Release -DSOTC_SANITIZE=OFF`, exit 0.
- CMake build: the initial case-sensitive target `SoCGameCore` did not exist and exited 2 without compiling. `cmake --build local/build/e1004-native --target help` showed `soc_game_core`; `cmake --build local/build/e1004-native --target soc_game_core -j 12` exited 0. Result is a thin ARM64 `MH_EXECUTE` Mach-O.
- Packaging: `scripts/package_launcher.sh local/build/e1004-launcher local/build/e1004-native/SoCGameCore`, exit 0. Swift release build completed, the core and app strict code signatures verified, and the bundle contains five files.
- Root verification and independent `/root/bundle_artifact_review` agree: both executables are thin ARM64 `MH_EXECUTE`; all dynamic dependencies are under `/usr/lib` or `/System/Library`; recursive filename and magic scan found no forbidden game/firmware artifacts. The bundle core is byte-identical to the standalone core after ad-hoc signatures are removed from temporary copies.
- SHA-256: launcher `a118c3cdcfbbf8f4a1334c486eee16d83129e81feb71a797337a29de157d56a5`; bundled core `cca7378d668287a8149e2eb7a3f9f0994abde519d229c4de77db32d8df2ce00a`; standalone core `6f3c6d8e26118764455e476658b28f9a957f16cb365b585a213a17eaff6bdc70`.
- Repository gates: CMake configure/build exited 0; CTest 487/487; lift checkers 572 passed, 0 skipped, 0 failed. Logs: `local/validation/APP-01-CURRENT-BUNDLE-REBUILD-01/implementation/attempt-01/` and `local/validation/APP-01-CURRENT-BUNDLE-REBUILD-01/result-publication/attempt-01/`.

## Limits and gate status

No original game files were read. Neither app nor core was launched. The current core still returns `SOTC_ERR_ENGINE_NOT_IMPLEMENTED`; artifact verification does not demonstrate Finder launch, meaningful subsystem lifecycle, game readiness, cold boot, gameplay audio, real save/reload, or a game-driven Metal frame. G4 is PARTIAL and not PASSED; G5/G6 remain NOT RUN.

`docs/VALIDATION.md` was reconciled with the current G4 evidence: the current thin-ARM64 signed bundle exists, but meaningful native start/stop and Finder launch are still unverified.

## Attempts and next action

Zero failed implementation hypotheses. One target-name setup correction and one verifier assumption correction were recorded: the first build target was case-mismatched; the first signed-file byte comparison differed only because the bundle core was re-signed, and the unsigned Mach-O copies matched exactly. Next, continue an independent native implementation path; do not use this packaging result as game readiness evidence.
