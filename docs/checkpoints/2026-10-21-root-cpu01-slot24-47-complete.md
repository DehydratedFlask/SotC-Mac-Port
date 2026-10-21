# Checkpoint: CPU-01-NATIVE-SLOT24-47-01 — fixture verified

- Time / timezone: 2026-10-21 08:58 CDT (America/Chicago).
- Agent and integrator: `/root` (Codex task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`).
- Task / gate / status: `CPU-01-NATIVE-SLOT24-47-01`, FIXTURE_VERIFIED, one stale-build-directory
  miss retained as failed try 1. G1 NOT RUN.
- Files changed for this task: `native/CMakeLists.txt`, `native/core/registration_thunks.cpp`,
  `native/core/registration_thunks.hpp`, `native/core/startup_ctors.cpp`,
  `native/core/startup_ctors.hpp`, `native/tests/test_registration_thunks_ahead.cpp`, and
  `tools/lift/check_ctor_slots.py`. Changes in `native/api/host/` and
  `native/api/CMakeLists.txt` belong to the separately escalated APP-01 host task.
- Exact build configuration: `cmake -S native -B local/build/native -DSOTC_SANITIZE=ON
  -DCMAKE_OSX_ARCHITECTURES=arm64` exited 0; log
  `local/checkpoints/root-cpu01-slot24-47-cmake-reconfigure.log`.
- Exact focused build: `cmake --build local/build/native -j 8 --target
  test_registration_thunks_ahead` exited 0; log
  `local/checkpoints/root-cpu01-slot24-47-build.log`.
- Exact focused test: `ctest --test-dir local/build/native --output-on-failure
  -R '^registration_thunks_ahead$'` exited 0; **1/1 passed**. Log:
  `local/checkpoints/root-cpu01-slot24-47-ctest.log`.
- Full native build: `cmake --build local/build/native -j 8` exited 0; log
  `local/checkpoints/root-cpu01-slot24-47-full-build.log`.
- Broad native regression suite: `ctest --test-dir local/build/native --output-on-failure
  -E '^sotc_host_protocol$'` exited 0; **295/295 passed**. The one excluded host test is
  `NEEDS_PRINCIPAL` in `docs/escalations/APP-01-CORE-HOST-01.md`; this command does not establish
  host behavior. Log: `local/checkpoints/root-cpu01-slot24-47-ctest-excluding-host.log`.
- Offline source checks passed (all exit 0):
  - `python3 -I tools/lift/check_ctor_slots.py`: 118 production RVA/index rows; slot 23 remains
    `0xe71990`; log `local/checkpoints/root-cpu01-slot24-47-slot-table-and-native-mapping-checker.log`.
  - `python3 -I tools/lift/verify_ctor_family.py`: all 34 family rows; log
    `local/checkpoints/root-cpu01-slot24-47-34-slot-family-verifier.log`.
  - `python3 -I tools/lift/check_ctor_thunks.py`: detailed slots 12–19 and negative controls; log
    `local/checkpoints/root-cpu01-slot24-47-detailed-slots-12-19-thunk-checker.log`.
- What the fixture proves: rows 24–47 have callbacks, ordered original RVAs, descriptor publication,
  recursive mutex behavior, idempotence and visible failure behavior in the native test model. The
  production table remains independent: the runner completes 23 prior rows, blocks at slot 23 / RVA
  `0xe71990`, and publishes none of slots 24–47. The checker validates the two tables separately
  against the pinned ELF row identities.
- Observation versus inference / acceptance limits: this is offline fixture behavior only. It does
  not lift the original slot-23 string table, register the ahead entries during product startup,
  demonstrate original ordering, boot a game, or advance G1. Do not claim gameplay or runtime
  compatibility from these tests.
- Remaining risk / next exact action: slot 23 remains the production blocker and its original-name
  data question is still for the principal. Keep the ahead table test-only until that blocker is
  resolved. The remaining task stream is listed in `docs/TASKS.md`; host task is independently
  stopped at `NEEDS_PRINCIPAL`.
- Graph / project index: `trail build --check` exited 0 after rebuilding with preserved
  `scripts`, `tools`, `reference` plus `native`, `launcher`, `tests`; the ahead test is queryable by
  `graft skeleton native/tests/test_registration_thunks_ahead.cpp`. The graph cache remains private.
- Task status / attempt count: FIXTURE_VERIFIED; one failed build-target discovery attempt due to
  stale CMake generation, then reconfiguration and focused fixture passed. No dependency changed.
