# E868 — APP-01 initial launcher readiness accessibility observation

- Local date/time: 2026-10-27 22:29 CDT (America/Chicago)
- Agent: `/root`
- Task: `APP-01-VOICEOVER-READINESS-01`
- Status: `PARTIAL`; initial workspace-bundle accessibility tree observed, but keyboard focus traversal and spoken VoiceOver announcements are NOT VERIFIED.
- Attempt count: 0 failed hypotheses; 1 partial runtime UI observation.

## Scope and prerequisite

The bounded check covers the launcher's initial readiness screen only: accessibility labels/details, focus, and status announcements. Do not choose a game folder or activate Play. The Mac was unlocked by the user; the native app inventory then returned normally. The older waiting checkpoint is [APP-01 environment pending](2026-10-20-root-app01-voiceover-environment-pending.md).

## Commands and artifact identity

`swift run --package-path launcher --scratch-path local/build/launcher SoCLauncher` built successfully in 4.14 seconds. The executable remained in its GUI event loop until root sent SIGTERM after the observation; the wrapper command therefore exited 143. This was deliberate cleanup, not a build failure. Log: `local/validation/orch-20261027/e867/voiceover-swift-run.log` (SHA-256 `5e993d0c99194f744b879305c57b89fce2889740e71b0be4ed3c763ef0a88128`).

The SwiftPM executable was not directly selectable in CUA. Its bundle ID was ambiguous across three registered apps, and CUA rejected the bare executable path. The observed window instead came from the exact workspace bundle `/Volumes/Extra/SoC Mac Native/local/build/launcher-app/SotC Launcher.app`. Its executable is ARM64, strict codesign verification exited 0, and its SHA-256 is `2d73543374d9a3bc0405032b78b324d2ce6226bfd3c293b3774fef831b798b0a`. The SwiftPM executable is also ARM64 but has a different SHA-256 (`71b663dacd659df2bf6bc584ae0ec465d61d76c22e742b4d3b044efc8dcbb04e`). The inspected workspace bundle's executable mtime is 2026-10-24 00:44 CDT, before commit `95653223`; inspection of that commit shows its only `ContentView.swift` change is the diagnostics label/value. The initial readiness rows and status source spans were unchanged by that commit. The bundle reports 311/573 constructor slots while E865 documented 308/573; this review does not reconcile the difference.

## Runtime observations

CUA opened the initial `Shadow of the Colossus` window and returned a complete AX tree plus screenshot. The readiness rows exposed:

- `Game assets` with detail `Missing: Choose the folder with the extracted game archives`.
- `Native game core` with a `NOT BUILT` engine explanation, startup progress/failure details, and `Play disabled until the engine is implemented`.
- `Metal` and `Audio output` as `Ready`.
- Overall `Game core status`, value `Not ready`, and the same native-core explanation.
- A disabled `Play` button whose help text repeats the blocking reason.

The AX adapter represented readiness rows as role `unknown`, but included their Description and Details. Source inspection of `ReadinessRow` and the status view confirms combined accessibility elements with explicit labels and values. No live-region/update-frequency announcement modifier was present on those inspected spans. The diagnostics disclosure remained closed.

The initially focused AX element was the main window. Read-only System Settings inspection found VoiceOver off and Full Keyboard Access off; neither setting was changed. One Tab press did not move the reported focus under FKA-off settings. Control+Option+Right was not an active VoiceOver test. No spoken output was captured, and no readiness state transition was triggered. Consequently, this does not establish the spoken phrase, automatic announcement behavior, or full keyboard focus sequence.

No folder was chosen, Play was not activated, no app preference was changed, and no game input or source file was touched. The app was quit after inspection. The CUA screenshot was visible in the session but is not separately saved.

## Evidence and documentation

Private AX text plus read-only VoiceOver/FKA setting observations: `local/validation/orch-20261027/e867/voiceover-ax-state.txt`, SHA-256 `382940a407c5b0ad812c39ce1512c1f1c61b221ecaaffdf220bfd54147f504cc`. The screenshot is not a persistent evidence file. No dependency was acquired; `INSTALL_LOG.md` is unchanged. E868 updates `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/INDEX.md`, this checkpoint, and the E867 publication status in its checkpoint.

## E867 publication closeout

E867 commit `45af5f281fa5e93891795766fb0fb3646547df7f` (`docs: close E866 attribution and SAV checkpoint`) is by Johnny Wang `<dehydratedflask@users.noreply.github.com>`. `python3 -I scripts/check_commit_attribution.py` passed on `origin/main..HEAD`; `git push origin main` exited 0. A subsequent fetch and live `git ls-remote` matched local `HEAD`, `origin/main`, and `refs/heads/main` at `45af5f281fa5e93891795766fb0fb3646547df7f`. The post-push GitHub contributors API query exited 0 and returned only `DehydratedFlask`.

## Remaining gate and next exact action

G4 remains PARTIAL. VoiceOver remained off and Full Keyboard Access remained off; no user preference was changed. If actual spoken/caption behavior is required later, temporarily enable VoiceOver only with explicit authorization and restore it off afterward.

Prepublication gates run from the workspace root:

| Gate | Result |
|---|---|
| `cmake -S native -B local/build/orch-r22` | exit 0 |
| `cmake --build local/build/orch-r22 -j 12` | exit 0; Ninja no work |
| `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure` | exit 0; 431/431 |
| `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` | exit 0; 516 passed, 0 skipped, 0 failed |
| `trail build --check` | exit 0; 9,877-node wiring graph current; deep meaning layer unbuilt |
| `python3 -I scripts/audit_publish_paths.py --all` before staging | exit 0; 3,084 paths |
| `python3 -I scripts/check_commit_attribution.py` before staging | exit 0; empty `origin/main..HEAD` range |
| `python3 scripts/project_index.py --check` | first run stale after new log paths; regenerated index and final check exit 0; 32,011 folders current |

Private command logs are in `local/validation/orch-20261027/e867/` with `e868-` prefixes (project-index generation is `project-index-e868.log`). The staged path audit exited 0 for 7/7 paths, the final full audit exited 0 for 3,085/3,085 paths, cached whitespace exited 0, and precommit attribution passed on the empty `origin/main..HEAD` range. Next exact actions: confirm remote base, commit the seven allowlisted docs paths under the owner identity, run the exact-range attribution checker, push `origin main`, and fetch-verify.
