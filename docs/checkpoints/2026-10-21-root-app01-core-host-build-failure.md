# Checkpoint: APP-01-CORE-HOST-01 — test compile failure

- Time / timezone: 2026-10-21 08:49 CDT (America/Chicago).
- Agent and integrator: `/root` (Codex task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`). Host patch by
  `/root/review_host_proto`; `/root` integrates and verifies.
- Task / gate / status: `APP-01-CORE-HOST-01`, ACTIVE, failed try 1. G4 NOT RUN.
- Owned / changed files: worker patch in `native/api/host/protocol.cpp`, `protocol.hpp`, `main.cpp`,
  and `tests/test_host_protocol.cpp`; no code fix has yet been made for this failure. This
  checkpoint changes docs only.
- Build setup: from `/Volumes/Extra/SoC Mac Native`, root reconfigured using
  `cmake -S native -B local/build/native -DSOTC_SANITIZE=ON -DCMAKE_OSX_ARCHITECTURES=arm64`
  (exit 0; log `local/checkpoints/root-cpu01-slot24-47-cmake-reconfigure.log`).
- Exact targeted check: `cmake --build local/build/native -j 8 --target test_sotc_host_protocol`
  exited 2 while compiling `native/api/host/tests/test_host_protocol.cpp:508`. Apple SDK's
  `WIFEXITED` macro takes the address of its argument; the test passes temporary expression
  `reap(unused)`, producing “cannot take the address of an rvalue of type 'int'”. Log:
  `local/checkpoints/root-app01-core-host-build.log`. No host CTest ran.
- Observation versus inference: the host library and `sotc_game_core` executable built successfully;
  only the test target failed to compile. The runtime protocol changes remain unverified. This is a
  compile error in test code, not evidence of a host protocol behavior failure.
- Cumulative failed tries: 1 distinct build attempt; no prior host test attempt. The constructor
  task's unrelated stale-target failure is not counted here.
- Remaining risks: the `bad stdin` child-process test still needs live confirmation after compilation;
  `start()` continues to return `ENGINE_NOT_IMPLEMENTED`, so G4 remains NOT RUN.
- Next exact action: assign `reap(unused)` to a local `int` and pass that lvalue to `WIFEXITED`,
  rebuild `test_sotc_host_protocol`, then run `ctest --test-dir local/build/native
  --output-on-failure -R '^sotc_host_protocol$'`.
- Evidence updates: no real-game evidence or installation/dependency facts changed. Update
  `HANDOFF.md`, `docs/TASKS.md`, and `docs/FOLDERS.md` in the integrator checkpoint commit.
