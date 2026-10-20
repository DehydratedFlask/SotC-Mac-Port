# Checkpoint: APP-01 child-process termination classifier claim

- Time / timezone: 2026-10-20 05:02 CDT (America/Chicago).
- Agent / integrator: `/root` owns shared documentation and Git; `/root/app01_termination` may edit only the three owned launcher code/test files.
- Task / gate / status: `APP-01-TERMINATION-CLASSIFIER-01` = `ACTIVE`; this slice does not advance a native-game gate.
- Prerequisite / decision: user selected a testable `LauncherCore` helper for the clean-exit/requested-Stop readiness behavior. Existing `LauncherProcessOutcome`, `LauncherLifecycle.state`, and `LauncherModel.processEnded` provide the code seam. `graft callers processEnded --depth all` found `LauncherModel.start` as the sole source caller.
- Pre-change SHA-256: `LauncherModel.swift` `d452846b54e6cc28e1726f6db1e017b1290a36f3fe554db948221cc0763fc848`; `LauncherLifecycle.swift` `6b0007a38fecac6645911b2e848328b3821c97eb426caa61dbbbaca9e23f31fb`; `LauncherLifecycleTests.swift` `666f35a315fc81394435428d80207c6e8c633824099895e861a73aa1844293f3`.
- Owned / changed files: worker owns `launcher/Sources/LauncherCore/LauncherLifecycle.swift`, `launcher/Sources/SoCLauncher/LauncherModel.swift`, and `launcher/Tests/LauncherCoreTests/LauncherLifecycleTests.swift`. No code changes are made at claim time.
- Hypothesis: a pure classifier taking whether Stop was requested, `Process.TerminationReason`, and termination status can preserve current behavior while independently covering Stop precedence, clean exit, nonzero exit, and signal termination. Requested Stop must win over exit reason/status; only a normal exit with status zero is clean.
- Exact baseline command and CWD: `swift test --package-path launcher --scratch-path local/build/launcher --filter LauncherLifecycleTests`; CWD `/Volumes/Extra/SoC Mac Native`.
- Baseline result: exit 0; 7 tests in 2 suites passed on `arm64e-apple-macos14.0`. Log: `local/checkpoints/root-2026-10-20-app01-termination-baseline.log`.
- Acceptance command: `swift test --package-path launcher --scratch-path local/build/launcher --filter LauncherLifecycleTests`.
- Observation versus inference / coverage limits: tests cover pure lifecycle/classifier behavior only. No native app interaction, child-process launch/termination, Finder distribution, sandbox behavior, or process lifetime on launcher quit is exercised.
- Cumulative failed tries: 0 implementation tries; baseline passed.
- Remaining risks / blockers: native alert/UI interaction and VoiceOver remain unrun; sandbox distribution and child-process-on-quit policy remain open. G1 is NOT PASSED; G2 is NOT RUN.
- Next exact action: dispatch `/root/app01_termination` after this claim is published; root reviews the source/test diff, reruns the acceptance command, then updates status and checkpoint.
- Index / Graft / diff: `python3 scripts/project_index.py` exited 0 with 4,166 folders indexed (log `local/checkpoints/root-2026-10-20-app01-termination-claim-project-index.log`); `graft check` and `git diff --check` exited 0; wiring graph has 1,250 nodes / 530 source files and the optional deep layer is unbuilt.
- HANDOFF / TASKS / EVIDENCE / INSTALL_LOG: `HANDOFF.md` and `docs/TASKS.md` updated; no new evidence record or install.
