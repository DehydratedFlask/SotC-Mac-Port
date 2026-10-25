# Checkpoint — launcher bundle with the native core

- Local: 2026-10-24 (America/Chicago)
- Agent: orchestrator
- Task: `APP-01-LAUNCHER-BUNDLE-CORE-01`
- Gate: **ARTIFACT_VERIFIED** for bundling/signing; **NOT RUN** for a GUI launch

## What changed

`scripts/package_launcher.sh <out-dir> [core-executable]` now accepts an optional core. When given:

- the core must be an executable **thin arm64 Mach-O** (`lipo -archs` exactly `arm64`; magic
  `cffaedfe`/`feedfacf`), otherwise packaging exits 1;
- it is copied to `Contents/MacOS/SoCGameCore`, ad-hoc code-signed and strict-verified;
- the app is then ad-hoc signed and strict-verified as before.

Without a core argument the bundle remains a launcher-only shell and reports NOT BUILT.

## Measured artifact

`scripts/package_launcher.sh local/build/launcher-app local/build/orch-r22/SoCGameCore`

- `local/build/launcher-app/SotC Launcher.app/Contents/MacOS/SoCLauncher` (744,848 B) and
  `SoCGameCore` (Mach-O 64-bit executable arm64).
- `codesign --verify --strict` passes for the app and for the bundled core.
- The forbidden-artifact scan (names and ELF/SELF/PSAR/PKG magic) passed; the private name fixture
  is **not** bundled.

The bundled core answers `--status` with `slots_registered=308`, `run_ok=true` when run with
`SOTC_SLOT23_FIXTURE` pointing at the private fixture, and `engine_implemented=false`. Without the
fixture it reports `run_ok=false` / `blocked_on_rva=0xe71990` (honest).

## What this does and does not prove

It proves a signed `.app` can carry the native core where `CoreLocator` expects it, and that the
bundled executable speaks the same protocol. It does **not** prove a Finder launch, any window or
readiness UI, game boot, Metal output, audio or save. Play remains disabled because the core has no
engine. The fixture is user-derived private data and is deliberately not distributed.
