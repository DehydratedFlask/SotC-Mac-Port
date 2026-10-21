# Checkpoint: APP-01-CORE-HOST-01 — closed-output fixture failure

- Time / timezone: 2026-10-21 08:51 CDT (America/Chicago).
- Agent and integrator: `/root` (Codex task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`).
- Task / gate / status: `APP-01-CORE-HOST-01`, ACTIVE, failed try 2. G4 NOT RUN.
- Change since try 1: `native/api/host/tests/test_host_protocol.cpp` now stores
  `reap(unused)` in a local `int` before calling `WIFEXITED`. This resolved the Apple SDK compile
  error. `cmake --build local/build/native -j 8 --target test_sotc_host_protocol` exited 0;
  output: `local/checkpoints/root-app01-core-host-build-attempt2.log`.
- Exact targeted check: `ctest --test-dir local/build/native --output-on-failure
  -R '^sotc_host_protocol$'` exited 8; test `sotc_host_protocol` failed at
  `native/api/host/tests/test_host_protocol.cpp:333` because
  `run_session(in, sink) == SessionEnd::OutputClosed` was false. Log:
  `local/checkpoints/root-app01-core-host-ctest-attempt2.log`.
- Index / graph checks: `python3 scripts/project_index.py` exited 0 and refreshed
  `docs/FOLDERS.md`; `trail build --check` exited 0 with the wiring graph in sync. Log:
  `local/checkpoints/root-app01-core-host-output-failure-trail-check.log`.
- Observation: the session code checks stream state after writing a response, newline, and flush.
  The fixture overrides `stringbuf::overflow` only for newline, yet the run returns a different
  `SessionEnd`. Inference: the newline may be accepted through the `stringbuf` put area without
  calling `overflow`, so this fixture does not reliably fail the output stream. No process-level
  behavior conclusion follows from this fixture failure.
- Cumulative failed tries: 2. Try 1 was the test compile error recorded in
  `docs/checkpoints/2026-10-21-root-app01-core-host-build-failure.md`; this second try compiled but
  exposed a non-failing test sink.
- Remaining risks: the rest of the test executable may reveal additional defects after this first
  assertion is repaired; the real child-process closed-stdout and bad-stdin tests have not yet run.
  `sotc_game_core` still reports `ENGINE_NOT_IMPLEMENTED`; G4 remains NOT RUN.
- Next exact action: replace the fixture's newline-only `overflow` rule with a deterministic failing
  `sync()` on flush, rebuild `test_sotc_host_protocol`, and run the same focused CTest as try 3.
  Stop and escalate if that targeted check fails again.
- Evidence updates: no real-game evidence or dependency/install facts changed. Update `HANDOFF.md`,
  `docs/TASKS.md`, and `docs/FOLDERS.md` in the integrator checkpoint commit.
