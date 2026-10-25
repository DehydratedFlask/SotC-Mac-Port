# launcher/ — native SwiftUI Mac launcher shell (APP-01, shell only)

Swift 6 / SwiftUI, macOS 15+, arm64. No Python, Electron, x86 helper or third-party package.
Ownership decision: the game core runs as a native child process (`SoCGameCore` in
`Contents/MacOS`), so the launcher stays responsive if the core fails. The executable now exists
(`native/app/`), but it has **no engine**: it reports `engine_implemented=false` and refuses start.

| Target | Role |
|---|---|
| `LauncherCore` | Pure checks: `AssetValidator` (six archives, PSAR magic, ordered `AssetValidationReport`), `SystemChecks` (Metal Apple7 family, default audio output, native-core presence), `ReadinessReport` (runs off the main actor), `ReadinessAccessibility`, `DiagnosticsSummary`, `LauncherStatusCopy` |
| `CoreClient` (in `LauncherCore`) | `CoreClient` protocol (`validate/start/stop/status/coverage`, async, throws `CoreFailure`), value models mirroring the native C ABI (`CoreModels.swift`) and `CoreClientMapping` (core facts -> `ReadinessItem.Status` / `LauncherState`). No native code. |
| `CSotcCore` | C++ SwiftPM target whose `sotc_core.cpp` and `include/sotc_core.h` are **symlinks into `native/api/`** (single source), built with `SOTC_API_WITH_CORE_LIB=0` |
| `LauncherNativeCore` | `NativeCoreClient`: `CoreClient` over the in-process C ABI (fallback when no child core is found) |
| `LauncherCore` (process) | `CoreLocator` + `ProcessCoreClient` + `CoreClientFactory`: locate a Mach-O `SoCGameCore`, speak protocol v1 over `--serve`, map typed facts; never invents `.running` |
| `SoCLauncher` | SwiftUI app: views + wiring only. `LauncherModel` (now in `LauncherCore`, `@MainActor @Observable`) routes validate/start/stop/status/coverage through an injected `CoreClient` |

Honest states: "Native game core" reports **NOT BUILT** (and Play stays disabled) when the child
executable is absent or the engine is unimplemented. Only thin ARM64 `MH_EXECUTE` files reach the
child-process transport; scripts, x86, universal binaries, libraries and truncated headers are rejected.
The readiness row and status line describe the missing executable or engine explicitly.
This shell cannot pass G4 or boot the game.

## Child-process core (APP-01-LAUNCHER-PROCESS-CLIENT-01)

`CoreLocator` searches `SOTC_CORE_PATH`, `<bundle>/Contents/MacOS/SoCGameCore`,
`<bundle>/Contents/Resources/SoCGameCore`, then the running executable's directory; only a regular
executable with a thin ARM64 `MH_EXECUTE` header is accepted (the same check as `SystemChecks`). `ProcessCoreClient` launches it with `--serve`, sends
`<id> <command> [argument]` lines and reads one JSON response per request with a 5 s timeout;
`CoreClientFactory` prefers it and falls back to `NativeCoreClient`.

Build and bundle the core with:

    cmake -S native -B local/build/launcher-release -DSOTC_SANITIZE=OFF -DCMAKE_BUILD_TYPE=Release
    cmake --build local/build/launcher-release --target soc_game_core -j 12
    scripts/package_launcher.sh local/build/launcher-app local/build/launcher-release/SoCGameCore

The real-core integration test is opt-in:

    SOTC_CORE_PATH="$PWD/local/build/orch-r22/SoCGameCore" \
      swift test --package-path launcher --scratch-path local/build/launcher --filter RealCoreIntegrationTests

The private slot-23 name fixture is not bundled; without `SOTC_SLOT23_FIXTURE` the core honestly
reports `run_ok=false`. Protocol agreement is not game boot: Play stays disabled.

## Packaged-app GUI findings (round 29 lane B)

- The "Native game core" row says NOT BUILT as before, but when a core answered it now says the *engine* is
  missing (`Play disabled until the engine is implemented`) and carries the core's own `startup_constructors`
  claim verbatim; the old "until a native SoCGameCore exists" wording remains only when no core was found.
- `CoreClientFactory.select` returns the chosen `CoreTransport`; an absent or unstartable `SoCGameCore` replaces the row
  with the locator's NOT BUILT reason and diagnostics gain a `Core transport:` line (child process path, or the
  in-process ABI fallback and why). The in-process fallback has no engine and never enables Play.
- Diagnostics "Constructor prefix" uses the status `ctor` object when the coverage answer carries none, including
  `N of M original slots recovered, first unrecovered index K`.
- The diagnostics text now exposes its content to assistive technology (label names the block, value is the report).
- `scripts/package_launcher.sh` requires a thin ARM64 `MH_EXECUTE` core and refuses non-system libraries (an ASan/UBSan build resolves
  `libclang_rt.asan` through an absolute Xcode rpath and only starts on the build machine). Package a release core:
  `cmake -S native -B local/build/r29b-release -G Ninja -DSOTC_SANITIZE=OFF -DCMAKE_BUILD_TYPE=Release`.
  Invalid core inputs are checked before building or replacing an existing app bundle.

Round 30 verification exercised Finder double-click launch with the core absent and present,
accessible diagnostics, a cancelled folder selection, and native Quit. Both apps kept Play disabled.
The packaged core reported 317/573 slots recovered (first unrecovered 306); Finder exposed the
unbundled private-fixture callback failure rather than hiding it. This is limited launcher evidence;
assets-ready GUI and actual game start remain unverified.

## Core ABI client (APP-01-CORE-ABI-01)

ABI: [native/api/README.md](../native/api/README.md). `NativeCoreClient().start()` throws
`CoreFailure(.engineNotImplemented)` and `status().state` becomes `.failed`, never `.running`;
`CoreClientMapping.launcherState` additionally turns `running` without `engineImplemented` into `.failed`.
Tests: `LauncherCoreTests/CoreClientTests` (scripted test double `FakeCoreClient`, deliberately test-only)
and `LauncherNativeCoreTests` (real C ABI, including parity with `AssetValidator`).

## LauncherModel over CoreClient (APP-01-LAUNCHER-CORECLIENT-01)

`LauncherModel(client:hostChecks:pollInterval:)` lives in `LauncherCore` so tests inject fakes; the app injects
the selected child-process client, falling back to `NativeCoreClient` when no usable child is found
(or `UnavailableCoreClient` carrying an in-process ABI failure; it can never validate or run).
`SystemChecks.nativeCore`/`Process` are no longer used by the model. Metal/audio stay host probes (`HostChecks`).

- validate: assets row = `CoreClient.validate` verdict (no folder -> no core call); native-core row =
  `CoreClientMapping.nativeCoreStatus` (a failed query is NOT BUILT); all core calls run in `Task.detached`.
- start: gated by `canStart`. Success is never trusted: the model asks `status()` and only
  `state == .running && engineImplemented` becomes `.running`; success followed by any other status is `.failed`.
  A refusal is `.failed`; `ENGINE_NOT_IMPLEMENTED` renders `LauncherPresentation` tone `.unavailable`
  ("Unavailable: the native game engine is not implemented yet, so the game cannot start."), spoken value starts
  `Unavailable. Not running.`, and is never followed by polling or running.
- status polling (`starting`/`running`/`stopping`) is a detached task; late answers cannot move a settled state;
  a core that stops/fails/vanishes on its own becomes `.failed`.
- diagnostics text = `DiagnosticsSummary` + `CoreDiagnostics` core section (ABI, engine implemented, ctor prefix,
  each subsystem status, core state/last error, last refused request), shown in a copyable Diagnostics section.

Real core today: Play is always disabled (engine row NOT BUILT), so the unavailable-start state is exercised by tests
with a scripted client that lets `validate` claim readiness; the UI never reaches it with `NativeCoreClient`.

## Stable copy (LauncherCore, unit-tested strings)

`ReadinessAccessibility` — spoken label/value per readiness row (`ReadinessRow` supplies both):

| Kind | Label | Value |
|---|---|---|
| `.ready` (any kind) | kind label | `Ready` |
| `.missing(detail)` (any kind) | kind label | `Missing: <detail>` |
| `.notBuilt(detail)` (any kind) | kind label | `<detail>` verbatim (already starts `NOT BUILT:` and ends with the Play-disabled sentence) |

Kind labels are exactly the visible row labels: `Game assets`, `Native game core`, `Metal`,
`Audio output`. Switches are exhaustive; a new kind or status fails to compile until it decides
its own wording.

`DiagnosticsSummary.text(for:)` — fixed order `assets, nativeCore, metal, audio`, then one overall
line, newline-separated, no trailing newline:

    Game assets: READY
    Native game core: NOT BUILT — NOT BUILT: <detail>
    Metal: MISSING — <detail>
    Audio output: READY
    Play is disabled: <report.playBlockedSummary>

A subsystem absent from the report renders `NOT REPORTED — no result for this subsystem`, and such
a report is never called ready (an empty `ReadinessReport` is vacuously `canStart`; the summary is
deliberately stricter). The overall line is `Play is ready: every subsystem reported READY.` only
when all four subsystems are present and ready.

`LauncherStatusCopy.for(_ outcome:)` / `.for(_ state:)`:

- requested stop → `Stopped at your request.`, no hint (never a crash/failure).
- clean exit → `Game core exited normally (status 0).`, no hint.
- unexpected exit → `Game core ended unexpectedly (status N).` + retry/diagnostics hint. The
  classifier cannot distinguish a signal from a nonzero exit, so this copy names neither cause
  and never calls it success.
- states: `Not ready to play.` / `Checking files and hardware…` / `Ready to play.` /
  `Starting the game core…` / `Game core running.` / `Stopping the game core…` /
  `Failed: <message>` (only `.failed` carries a recovery hint).

`AssetValidator.report(directory:)` returns `AssetValidationReport(problems:missingArchives:)` in
`requiredArchives` order; `validate(directory:)` is `report(...).status`, unchanged. A directory
wearing an archive name counts as missing, never as an archive.

    swift build  --package-path launcher --scratch-path local/build/launcher
    swift test   --package-path launcher --scratch-path local/build/launcher

The bundle is ad-hoc signed for local verification. It is not notarized or a release artifact.
Remaining G4 checks include meaningful game-core start/stop, the full window/display/controller matrix,
and a VoiceOver listening audit. Unit tests and launcher UI checks do not establish game boot.
