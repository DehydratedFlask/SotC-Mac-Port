# E877 — APP-01 VoiceOver third observation and escalation

- Time / timezone: 2026-10-27 23:44 CDT (America/Chicago)
- Agent and integrator: `/root`
- Task / gate / status: `APP-01-VOICEOVER-READINESS-02`, `NEEDS_PRINCIPAL` after its third unsuccessful focus/caption observation; G4 remains `PARTIAL`.
- Prerequisite evidence / input and source hashes: E869 and E873 are the two prior observations. No game binary, asset, source binary, or RVA was read. The running app's CUA bundle ID is `io.github.dehydratedflask.sotc-mac-port.launcher`; no binary hash was captured.
- Owned / changed files: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/INDEX.md`, generated `docs/FOLDERS.md`, this checkpoint, and `docs/escalations/APP-01-VOICEOVER-READINESS-02.md`. Private observation: `local/validation/orch-20261027/e876/voiceover/observation.txt`, SHA-256 `f2edfd23bd1ca6a302e4bc4dd85d913d918f3a0fcb9dddcc630b49b517ad044c`.
- Hypothesis and expected result: With VoiceOver explicitly on in System Settings, its cursor should advance from the current readiness control and speech/captions should be observable; restore VoiceOver off afterward.
- Exact command, CWD and nonsecret environment: CUA on macOS at the workspace launcher. `System Settings > Accessibility > VoiceOver` showed off, then on, then off. VoiceOver Utility > Visuals > Panels and Menus showed “Show caption panel” Value 1. Launcher `pressKey("ctrl+alt+Right")` followed by `getAXState()` and `getScreenshot()`.
- Exit / observed result: VoiceOver was verified on during the test and restored off after. The launcher AX tree continued to report focus at `Choose Folder…`. The selected-app screenshot showed no caption text; no speech audio was captured. This does not establish that system-wide captions or speech were absent. No folder was selected and Play remained disabled and untouched.
- Log, capture and REA evidence paths/IDs: Private note above. CUA screenshots appeared inline but were not persisted. No REA/Ghidra evidence applies.
- Observation versus inference / coverage limitations: The VoiceOver toggle and caption preference are direct observations. CUA's focus report may not be the VoiceOver cursor; the screenshot is app-window-only; no audio capture exists. Actual cursor traversal, spoken content, and caption output are `NOT VERIFIED`.
- Cumulative failed tries and distinct hypotheses: Three unsuccessful observations: E869 expected VO cursor and caption display after enabling; E873 tested Tab focus/announcement; E876 used the explicit Settings switch after the shortcut route was inconclusive. Counts do not reset across sessions.
- Remaining risks / blockers: No evidence demonstrates the accessibility path works or fails. G4 remains `PARTIAL`; no real-game or launcher acceptance claim follows from this observation.
- Next exact action: Principal reassessment of a desktop-wide capture or permitted audio-capture route. If none exists, request a direct owner observation; do not make a fourth attempt using the same evidence surface.
- HANDOFF.md / TASKS / EVIDENCE / INSTALL_LOG / indexes updated: HANDOFF, TASKS, EVIDENCE, INDEX updated. `docs/FOLDERS.md` was regenerated and `project_index.py --check` passed at 32,022 entries. `INSTALL_LOG.md` unchanged. Staged/full path audits, attribution, and publication are pending.

## Publication validation (E877 checkpoint)

| Gate | Result |
|---|---|
| `cmake -S native -B local/build/orch-r22` | PASSED, exit 0 |
| `cmake --build local/build/orch-r22 -j 12` | PASSED, exit 0; Ninja no work |
| `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure` | PASSED, 431/431 |
| `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` | PASSED, 516 passed / 0 skipped / 0 failed |
| `trail build --check` | PASSED, exit 0; wiring graph 9,877/current; deep meaning layer not built |
| `python3 -I scripts/audit_publish_paths.py` (staged) / `--all` | PASSED, 7/7 staged and 3,094/3,094 tracked paths |
| `python3 -I scripts/check_commit_attribution.py` | PASSED before commit for the empty unpublished range; exact E877 commit range is recorded in the publication closeout |
| `python3 scripts/project_index.py` and `--check` | PASSED, 32,022 indexed/current. One initial check found the private folder index stale because its redirected check log was created after generation; regeneration and direct check both passed. |
| Commit / push / fetch verification | Pending |
