# E1004 — Current launcher bundle rebuild claim

**Date:** 2026-10-29 13:11 CDT
**Agent:** `/root`
**Task:** `APP-01-CURRENT-BUNDLE-REBUILD-01`
**Status:** `ACTIVE`

## Bounded scope

Rebuild the current ARM64 SwiftUI launcher and native core into a fresh private app bundle. Verify Mach-O architecture, strict signatures, system-only dependencies, and absence of original-game/firmware artifacts. This is packaging evidence only; no Finder launch, readiness, engine, game boot/audio/save, or G4 completion claim is in scope.

## Starting evidence

- Starting checkout was clean `main` at `0f18c6f9f91486bc13a276575b53f996d435ee52`.
- `git fetch origin main` exited 0; `HEAD`, `origin/main`, and `FETCH_HEAD` matched.
- The prior `local/build/launcher-app/SotC Launcher.app` and its launcher/core executables were timestamped 2026-10-24 00:44:09 CDT, before current source changes.
- `scripts/package_launcher.sh` accepts an output directory, verifies a supplied core is thin ARM64 `MH_EXECUTE` with system-only dependencies, rejects game/firmware artifacts, and deletes only the selected `SotC Launcher.app` within that output directory before packaging. The new output path will be `local/build/e1004-launcher`; it must be absent before running the script.
- Current `sotc_core_start()` still returns `SOTC_ERR_ENGINE_NOT_IMPLEMENTED`; a rebuilt bundle cannot pass G4.

## Claim publication

`git fetch origin main`, `git status --short --branch`, and `git rev-parse HEAD origin/main FETCH_HEAD` completed successfully. Owner identity resolved to Johnny Wang / `dehydratedflask@users.noreply.github.com`. `python3 scripts/project_index.py --check` first exited 1 because the private inventory was stale after creating new task log files; regeneration and a second check passed at 43,193/current. `trail build --check` passed the wiring graph with 10,787 meaning nodes pending, and the tracked-path audit passed 3,391/3,391. Exact logs are under `local/validation/APP-01-CURRENT-BUNDLE-REBUILD-01/claim-publication/attempt-01/`.

## Next action and limits

Confirm the fresh output does not exist, configure/build `SoCGameCore` as Release with sanitizers disabled, package the launcher and core to the fresh path, then inspect the artifact. Do not launch the app or use original game files. No product gate advances from this claim.
