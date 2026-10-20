# launcher/ — native SwiftUI Mac launcher shell (APP-01, shell only)

Swift 6 / SwiftUI, macOS 15+, arm64. No Python, Electron, x86 helper or third-party package.
Ownership decision: the game core will run as a native child process (`SoCGameCore` in the app's
auxiliary executables), so the launcher stays responsive if the core fails. No such core exists yet.

| Target | Role |
|---|---|
| `LauncherCore` | Pure checks: `AssetValidator` (six archives, PSAR magic), `SystemChecks` (Metal Apple7 family, default audio output, native-core presence), `ReadinessReport` (runs off the main actor) |
| `SoCLauncher` | SwiftUI app: `LauncherModel` (@Observable lifecycle idle/validating/ready/starting/running/stopping/failed), views |

Honest states: "Native game core" reports **NOT BUILT** (and Play stays disabled) until a real
Mach-O `SoCGameCore` auxiliary executable exists. Shell scripts/stubs are rejected. A readiness
row and status line both carry the NOT BUILT / Play disabled wording — never fake success.
This shell cannot pass G4 or boot the game.

    swift build  --package-path launcher --scratch-path local/build/launcher
    swift test   --package-path launcher --scratch-path local/build/launcher

Not yet done: Finder-launch from a clean environment, .app bundling/signing, fullscreen/Retina/
display-change/controller checks, accessibility audit with VoiceOver, visual review.
