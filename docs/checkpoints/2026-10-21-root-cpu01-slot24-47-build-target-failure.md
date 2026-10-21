# Checkpoint: CPU-01-NATIVE-SLOT24-47-01 — focused target missing

- Time / timezone: 2026-10-21 08:47 CDT (America/Chicago).
- Agent and integrator: `/root` (Codex task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`).
- Task / gate / status: `CPU-01-NATIVE-SLOT24-47-01`, ACTIVE, failed try 1. G1 NOT RUN.
- Owned / changed files: implementation edits are under `native/core/`, native CMake files,
  `native/tests/test_registration_thunks_ahead.cpp`, and `tools/lift/check_ctor_slots.py`.
  This checkpoint records a build-system miss; it does not add a code change. The worker's
  independent host edits are under `native/api/host/` and tracked under `APP-01-CORE-HOST-01`.
- Exact focused build command, CWD and result: from `/Volumes/Extra/SoC Mac Native`,
  `cmake --build local/build/native -j 8 --target test_registration_thunks_ahead` exited 2
  before compilation: `make: *** No rule to make target 'test_registration_thunks_ahead'. Stop.`
  Captured output: `local/checkpoints/root-cpu01-slot24-47-native-build.log`.
- Build configuration observed: `CMAKE_HOME_DIRECTORY=/Volumes/Extra/SoC Mac Native/native`,
  `CMAKE_GENERATOR=Unix Makefiles`, `CMAKE_OSX_ARCHITECTURES=arm64`, and `SOTC_SANITIZE=ON` in
  `local/build/native/CMakeCache.txt`. The cache points to this checkout but predates the new
  focused target registration, so reconfigure the same build rather than modifying the test.
- Independent offline checks passed before this build attempt:
  - `python3 -I tools/lift/check_ctor_slots.py` exited 0; 118 production RVA/index pairs and the
    slot-23 `0xe71990` boundary passed. Log:
    `local/checkpoints/root-cpu01-slot24-47-slot-table-and-native-mapping-checker.log`.
  - `python3 -I tools/lift/verify_ctor_family.py` exited 0; 34 family rows passed. Log:
    `local/checkpoints/root-cpu01-slot24-47-34-slot-family-verifier.log`.
  - `python3 -I tools/lift/check_ctor_thunks.py` exited 0; the detailed slot 12–19 effect checks
    and three negative controls passed. Log:
    `local/checkpoints/root-cpu01-slot24-47-detailed-slots-12-19-thunk-checker.log`.
- Graph recovery: first `trail build --check` exited 1, STALE, identifying the new host-protocol
  test, `spawn_host`, and `test_process_bad_stdin_exits_nonzero` in
  `native/api/host/tests/test_host_protocol.cpp`. Rebuilt with
  `trail build --follow-nested-repos --only-dir scripts --only-dir tools --only-dir reference
  --only-dir native --only-dir launcher --only-dir tests`, exit 0; log:
  `local/checkpoints/root-20261021-source-graph-refresh.log`. The follow-up `trail build --check`
  exited 0 (wiring graph in sync); log: `local/checkpoints/root-20261021-source-graph-check.log`.
  `graft skeleton native/tests/test_registration_thunks_ahead.cpp` exited 0 and returned the test
  functions, proving the new test is indexed. The graph retains all prior scopes and adds the three
  new scopes. `graft/` remains a private ignored cache.
- Observation versus inference: the focused native target did not compile or run; this is a stale
  CMake generation, not a source/test failure. The unchanged production runner still stops at
  unresolved slot 23 by source contract, but runtime proof awaits the focused test.
- Remaining risks: native ahead-table behavior and host protocol changes are both unverified by
  build/CTest. Slot 23 remains unresolved; no G1 evidence exists.
- Next exact actions: reconfigure with `cmake -S native -B local/build/native -DSOTC_SANITIZE=ON
  -DCMAKE_OSX_ARCHITECTURES=arm64`; then build `test_registration_thunks_ahead` and
  `test_sotc_host_protocol`, run their focused CTest cases, and record actual outputs.
- Files / logs: source changes remain uncommitted at this checkpoint; no dependency changed.
  Update `HANDOFF.md`, `docs/TASKS.md`, and `docs/FOLDERS.md` in the integrator checkpoint commit.
