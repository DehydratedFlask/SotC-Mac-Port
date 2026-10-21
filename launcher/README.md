# launcher/ — native SwiftUI Mac launcher shell (APP-01, shell only)

Swift 6 / SwiftUI, macOS 15+, arm64. No Python, Electron, x86 helper or third-party package.
Ownership decision: the game core will run as a native child process (`SoCGameCore` in the app's
auxiliary executables), so the launcher stays responsive if the core fails. No such core exists yet.

| Target | Role |
|---|---|
| `LauncherCore` | Pure checks: `AssetValidator` (six archives, PSAR magic, ordered `AssetValidationReport`), `SystemChecks` (Metal Apple7 family, default audio output, native-core presence), `ReadinessReport` (runs off the main actor), `ReadinessAccessibility`, `DiagnosticsSummary`, `LauncherStatusCopy` |
| `CoreClient` (in `LauncherCore`) | `CoreClient` protocol (`validate/start/stop/status/coverage`, async, throws `CoreFailure`), value models mirroring the native C ABI (`CoreModels.swift`) and `CoreClientMapping` (core facts -> `ReadinessItem.Status` / `LauncherState`). No native code. |
| `CSotcCore` | C++ SwiftPM target whose `sotc_core.cpp` and `include/sotc_core.h` are **symlinks into `native/api/`** (single source), built with `SOTC_API_WITH_CORE_LIB=0` |
| `LauncherNativeCore` | `NativeCoreClient`: `CoreClient` over the in-process C ABI. Wired into the app by `SoCLauncherApp` |
| `SoCLauncher` | SwiftUI app: views + wiring only. `LauncherModel` (now in `LauncherCore`, `@MainActor @Observable`) routes validate/start/stop/status/coverage through an injected `CoreClient` |

Honest states: "Native game core" reports **NOT BUILT** (and Play stays disabled) until a real
Mach-O `SoCGameCore` auxiliary executable exists. Shell scripts/stubs are rejected. A readiness
row and status line both carry the NOT BUILT / Play disabled wording — never fake success.
This shell cannot pass G4 or boot the game.

## Core ABI client (APP-01-CORE-ABI-01)

ABI: [native/api/README.md](../native/api/README.md). `NativeCoreClient().start()` throws
`CoreFailure(.engineNotImplemented)` and `status().state` becomes `.failed`, never `.running`;
`CoreClientMapping.launcherState` additionally turns `running` without `engineImplemented` into `.failed`.
Tests: `LauncherCoreTests/CoreClientTests` (scripted test double `FakeCoreClient`, deliberately test-only)
and `LauncherNativeCoreTests` (real C ABI, including parity with `AssetValidator`).

## LauncherModel over CoreClient (APP-01-LAUNCHER-CORECLIENT-01)

`LauncherModel(client:hostChecks:pollInterval:)` lives in `LauncherCore` so tests inject fakes; the app injects
`NativeCoreClient` (or `UnavailableCoreClient` carrying the handshake failure; it can never validate or run).
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

Not yet done: Finder-launch from a clean environment, .app bundling/signing, fullscreen/Retina/
display-change/controller checks, accessibility audit with VoiceOver, visual review.
