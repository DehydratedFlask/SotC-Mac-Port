# launcher/ — native SwiftUI Mac launcher shell (APP-01, shell only)

Swift 6 / SwiftUI, macOS 15+, arm64. No Python, Electron, x86 helper or third-party package.
Ownership decision: the game core will run as a native child process (`SoCGameCore` in the app's
auxiliary executables), so the launcher stays responsive if the core fails. No such core exists yet.

| Target | Role |
|---|---|
| `LauncherCore` | Pure checks: `AssetValidator` (six archives, PSAR magic, ordered `AssetValidationReport`), `SystemChecks` (Metal Apple7 family, default audio output, native-core presence), `ReadinessReport` (runs off the main actor), `ReadinessAccessibility`, `DiagnosticsSummary`, `LauncherStatusCopy` |
| `SoCLauncher` | SwiftUI app: `LauncherModel` (@Observable lifecycle idle/validating/ready/starting/running/stopping/failed), views |

Honest states: "Native game core" reports **NOT BUILT** (and Play stays disabled) until a real
Mach-O `SoCGameCore` auxiliary executable exists. Shell scripts/stubs are rejected. A readiness
row and status line both carry the NOT BUILT / Play disabled wording — never fake success.
This shell cannot pass G4 or boot the game.

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
