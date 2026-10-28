# E869 — APP-01 VoiceOver caption/focus follow-up

- Local date/time: 2026-10-27 22:43 CDT (America/Chicago)
- Agent: `/root`
- Task: `APP-01-VOICEOVER-READINESS-02`
- Status: `PARTIAL`; VoiceOver was enabled and restored, but CUA did not expose caption text or focus movement.
- Attempt count: 1 unsuccessful focus/caption observation; 0 code hypotheses.

## Scope and authorization

The bounded check covered the launcher's initial readiness screen only. The user authorized temporarily enabling VoiceOver and restoring it off. No game folder was chosen and Play was not activated. The launcher bundle is the workspace bundle already identified in E868; source changes and G4 claims were outside scope.

## CUA actions and observations

1. System Settings > Accessibility > VoiceOver initially showed `off`.
2. Temporarily enabled VoiceOver; the switch showed `on`.
3. VoiceOver Utility > Visuals > Panels and Menus showed “Show caption panel” Value `1`. This preference was already enabled and was not changed.
4. Opened `/Volumes/Extra/SoC Mac Native/local/build/launcher-app/SotC Launcher.app`. The accessibility tree exposed Game assets (missing folder), Native game core (NOT BUILT and Play disabled), Metal Ready, Audio output Ready, overall Game core status Not ready, disabled Play with the blocking reason in help, and a closed Diagnostics disclosure. The screenshot showed the initial UI but no VoiceOver caption text.
5. Sent CUA `pressKey("ctrl+alt+Right")` (Control+Option+Right), then Tab. The AX tree continued to report the main window as focused; no focus traversal was observable. The CUA result did not include captured speech audio. Exact spoken phrases, whether the shortcut reached VoiceOver, announcement timing, and automatic state announcements remain NOT VERIFIED.
6. Restored VoiceOver to `off` in System Settings. Full Keyboard Access remained `off` in System Settings > Keyboard. The VoiceOver Utility process was no longer running in the final CUA inventory; its caption preference remained unchanged.

No folder picker, Play action, state transition, source edit, game input, or user preference change occurred. The launcher remains at its initial screen. Screenshots were visible in the CUA session but not persisted as files.

## Private evidence

CUA observation record: `local/validation/orch-20261027/e869/voiceover-ax-state.txt`, SHA-256 `236bf81abe7b14c6fd0655bcc1a587d9006d71e3b4bd51b408c2b6c5f0012b5a`.

## Publication gates

Results below are filled after the required workspace-root checks. The repository's verification-before-publication sequence is required for this docs checkpoint. G4 remains `PARTIAL`; this check is not a launcher or game acceptance result.

| Gate | Result |
|---|---|
| `cmake -S native -B local/build/orch-r22` | PASSED, exit 0 |
| `cmake --build local/build/orch-r22 -j 12` | PASSED, exit 0; Ninja no work |
| `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure` | PASSED, 431/431 |
| `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` | PASSED, 516 passed / 0 skipped / 0 failed |
| `trail build --check` | PASSED, exit 0; wiring graph 9,877/current; deep layer unbuilt |
| `python3 scripts/project_index.py` and `python3 scripts/project_index.py --check` | PASSED, 32,012/current. Two earlier checks found the private folder index stale because additional validation log files were created after generation; precreated the remaining log paths, regenerated, and rechecked successfully. |
| `python3 -I scripts/audit_publish_paths.py` (staged) and `python3 -I scripts/audit_publish_paths.py --all` | PASSED, 6 staged paths / 3,086 full paths |
| `git diff --cached --check` | PASSED, exit 0 |
| `python3 -I scripts/check_commit_attribution.py` before commit | PASSED on the empty precommit range; E869 commit exact-range result is recorded in the publication row below |
| Commit/push/fetch verification | PASSED: E869 commit `3ed9e49e42e3c971151eb2f51d6a41a3c4f336bf`, author/committer Johnny Wang; exact-range attribution check passed; `git push origin main` passed; `git fetch origin` and `git ls-remote --refs origin` matched `HEAD`, `origin/main`, `FETCH_HEAD`, and live `refs/heads/main`; GitHub contributors endpoint listed only `DehydratedFlask`. See E870 for post-E869 attribution census. |

Gate logs are under `local/validation/orch-20261027/e869/`. Full audit reported 3,085 paths before the new checkpoint was staged; the post-stage full audit is 3,086.

## Files and next action

Allowlisted changes: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/INDEX.md`, this checkpoint, and a generated documentation index if required. Private evidence stays under ignored `local/`. No dependency or source changes.

E869 publication is complete: commit `3ed9e49e42e3c971151eb2f51d6a41a3c4f336bf` was exact-range checked as Johnny Wang, pushed to `origin main`, and fetch/live-ref verified. The post-E869 advertised-ref census and its documentation closeout are recorded in [E870](2026-10-27-post-e869-public-attribution-census-e870.md). Keep VoiceOver readiness and G4 `PARTIAL`.
