# APP-01 initial readiness accessibility — hint and placeholder fixes

- Local date/time: 2026-11-01 20:35 CST (America/Chicago)
- Agent: `app01_voiceover_worker` (subagent; integrator owns `HANDOFF.md`)
- Task: `APP-01-VOICEOVER-READINESS-01` (`docs/TASKS.md`)
- Status: `PARTIAL` — source inspection `STATIC_ANALYSIS_VERIFIED`, fixes built and unit-tested, spoken output and focus traversal `NOT VERIFIED`.
- Attempt count: 0 failed hypotheses; this pass adds 1 static inspection and 3 defects fixed.

## Scope

Bounded to the launcher's initial readiness screen: what its accessibility tree exposes before
anything is loaded, and whether that tree tells a screen-reader user (a) that the game engine is
not implemented and (b) what they can do about it. No game folder was chosen, Play was not pressed,
no app was launched, and no runtime/game claim is made. E868 remains the only runtime AX observation
and it used an older bundle; this pass did not rebuild or re-observe that bundle.

## What the initial tree exposes (source inspection)

With the documented bundle built **without** a core (`scripts/package_launcher.sh <out>`):

| Element | Label | Value |
|---|---|---|
| Readiness row | `Game assets` | `Missing: Choose the folder with the extracted game archives` (`CoreReadiness.swift:15`, `ReadinessAccessibility.swift:47-48`) |
| Readiness row | `Native game core` | `NOT BUILT: no SoCGameCore path (auxiliary executable absent). Play disabled until a native SoCGameCore exists` (`CoreLocator.swift:85,90` via `CoreTransport.swift:15-18`, applied at `LauncherModel.swift:133`) |
| Readiness row | `Metal` / `Audio output` | `Ready` — host probes only (`LauncherModel.swift:14`), independent of the engine |
| Status | `Game core status` | `Not ready. <same native-core row text>` (`LauncherPresentation.swift:45-52`) |
| Play button | `Play` (disabled) | hint `Unavailable. <same text>` (`ContentView.swift:62-67`) |

With a core present but no engine, the row/status text is
`NOT BUILT: native game engine not implemented (core ABI <v>); startup constructors … . Play disabled
until the engine is implemented` (`CoreClientMapping` in `CoreClient.swift:42`). Both wordings say the
engine gap honestly; the second names it explicitly. Rows are one combined element each
(`ReadinessRow.swift:16-18`), so the spoken value is the row's own status, never a summary that could
promote a partially ready screen to "ready".

## Defects found

1. **Every visible recovery hint was invisible to assistive technology.** `ContentView.swift:42-51`
   makes the status a `.accessibilityElement(children: .combine)` element and then overrides its
   label *and* value. The override replaces the combined children's text, so the hint rendered at
   `ContentView.swift:45-47` — the only "what you can do" line for a refused start
   (`LauncherPresentation.swift:62`, `LauncherStatusCopy.swift:39,62`) — never reached VoiceOver.
   Today that state is reachable only with a client that can accept a start (`README.md:94-95`);
   the defect becomes live the moment a start can be attempted.
2. **The initial blocked state had no guidance at all.** `LauncherPresentation.make` passed
   `hint: nil` for every `.idle` presentation, including the one the app actually starts in. A
   screen-reader user heard the engine was not implemented and nothing about next steps; the only
   action-shaped text in the tree was the disabled Play button's hint, which repeats the reason.
3. **The Readiness placeholder contradicted the Status line while the first check ran.**
   `ContentView.swift:29-37` showed `Choose your game folder to check readiness.` whenever
   `report == nil`, which includes the whole `.validating` window (`LauncherModel.swift:120` sets
   `.validating`, `:140` assigns `report`). The check itself runs with no folder
   (`LauncherModel.swift:106-116`), while Status simultaneously said `Checking files and hardware…`.

Reported but not changed: when no child core is found, `CoreTransport.nativeCoreRowOverride`
replaces the engine fact with the locator's missing-executable sentence, so the row (and its spoken
value) implies a `SoCGameCore` is what unblocks Play even though `native/app/` exists and still has
no engine. That is a transport-honesty contract pinned by `Round29CoreTransportTests`; changing it
is a separate task.

## Changes

- `launcher/Sources/LauncherCore/LauncherPresentation.swift`: new `accessibilityHint` (exactly the
  visible hint, empty when there is none) and `blockedActionHint`; `.idle` now carries that hint only
  when a blocked reason exists, so nothing is announced before the first report.
- `launcher/Sources/SoCLauncher/ContentView.swift`: apply `.accessibilityHint` to the status
  element; state-aware readiness placeholder (`Checking readiness…` while `.validating`).
- Tests: new `launcher/Tests/LauncherCoreTests/InitialReadinessAccessibilityTests.swift` (6 tests);
  two previously pinned `hint == nil` assertions updated in `LauncherModelTests.swift` and
  `PlayBlockedSummaryTests.swift` (their honesty assertions are unchanged).

No honesty path was weakened: status text and spoken values are unchanged, `NOT BUILT` stays
verbatim, and the new hint states that Play stays disabled.

## Verification

| Command | Result |
|---|---|
| `swift build --package-path launcher --scratch-path local/build/launcher` | exit 0 |
| `swift test --package-path launcher --scratch-path local/build/launcher` | exit 0; `LauncherCoreTests` 166 tests / 22 suites (baseline before changes: 160 / 21) + `LauncherNativeCoreTests` 10 / 1 — 176 tests / 23 suites across both products. The opt-in `Real SoCGameCore protocol` suite runs without `SOTC_CORE_PATH`, so it proves nothing about a real core. |
| `swift test … --filter InitialReadinessAccessibilityTests` | exit 0; 6/6 |
| mutation: `accessibilityHint` → `""` | 4 assertions across 3 tests failed as required; restored |
| mutation: idle hint → `nil` | 3 tests failed across 3 suites as required; restored (sha256 re-checked) |
| `scripts/package_launcher.sh local/build/launcher-app-app01-voiceover` | exit 0; `SotC Launcher.app` arm64, `codesign --verify --strict` exit 0, no core bundled; executable SHA-256 `904a694f892c3a37c71c644d69a7a3a2ea26c84c30899c80ed8b1148085bf8da` (identical before and after the mutation/restore cycle) |
| `python3 -I scripts/audit_publish_paths.py --all` | exit 0; `PUBLISH PATH AUDIT OK (3996 path(s))` |
| `python3 scripts/project_index.py` | exit 0; 50,371 folders written. `--check` afterwards still reports the *private* `local/indexes/folders.json` stale: it records immediate file counts for every folder under `local/`, which concurrent workers keep changing; the published `docs/FOLDERS.md` is current. |

## Limits

- No app launch, no CUA/AX dump, no VoiceOver session, no spoken output, no focus traversal, no
  folder selection, no Play press. The built bundle was not run.
- SwiftUI view changes (`.accessibilityHint` placement, placeholder branch) are compile-verified
  only: the SwiftPM test targets are `LauncherCore` and `LauncherNativeCore`; there is no view test
  target, and no parallel harness was added.
- `Text("")` as an empty hint is not spoken by VoiceOver in the documented behaviour, but that is
  not verified here.
- No announcement/notification code exists in the launcher; none was added, because a launch-time
  announcement would fire before readiness data exists (the anti-pattern this task warns about) and
  cannot be verified without VoiceOver enabled.

## Next exact action

With explicit authorization to enable VoiceOver, launch this bundle, and hear the initial screen:
confirm the status element reads `Game core status`, the verbatim `NOT BUILT … Play disabled` value,
and the new hint; confirm the placeholder reads `Checking readiness…` during the first check; then
restore VoiceOver off. G4 remains `PARTIAL`.
