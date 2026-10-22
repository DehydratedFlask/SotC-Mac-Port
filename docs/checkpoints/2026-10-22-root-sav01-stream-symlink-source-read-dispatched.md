# Dispatch checkpoint: SAV-01-NATIVE-STREAM-FILE-SYMLINK-SOURCE-READ-FIXTURE-01

- Local date/time: 2026-10-22 07:05 CDT (America/Chicago). Integrator: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`).
- Task / parent / status: `SAV-01-NATIVE-STREAM-FILE-SYMLINK-SOURCE-READ-FIXTURE-01`, child of `SAV-01-NATIVE-STREAM-01`; claim is published and implementation is dispatched, zero attempts and zero failed tries at dispatch.
- Claim publication: commit `354e450700e84557a30e109ef8f82425f5179983`; publication checkpoint commit `6581d40d82ae2d3efc52eeaf227f3db730ac592d`. E374/E375 and the preceding publication checkpoint record the baseline, bounds, audits, and remote state.
- Worker: `/root/next_fixture_scout_20261022`. Only authorized source path is `native/tests/test_save_stream_file.cpp`. Add the single claimed test that reads a valid synthetic stream through a symlink and asserts success, descriptor `0xd`, and expected payload. Worker must not edit shared docs, production code, CMake, API/Graft paths, references, or game files.
- Exact worker acceptance commands:
  1. `cmake -S native -B local/build/native-worker-sav01-stream-symlink-source-read-20261022 -G Ninja -DCMAKE_BUILD_TYPE=Debug`
  2. `cmake --build local/build/native-worker-sav01-stream-symlink-source-read-20261022 --target test_save_stream_file`
  3. `ctest --test-dir local/build/native-worker-sav01-stream-symlink-source-read-20261022 --output-on-failure -R '^save_stream_file$'`
- Commands were dispatched after claim publication; output and acceptance result have not yet been reported. Worker logs are precreated under `local/validation/sav01-reader-stream-symlink-source-read-20261022/worker-*.log`. Root owns independent replay in `local/build/native-root-sav01-stream-symlink-source-read-20261022/`, review, task/docs update, and publication.
- Parallel candidate scout: `/root/fixture_candidate_scout_c` was asked to find one disjoint, test-only candidate in `native/tests/`, use Graft, check `docs/TASKS.md`, and make no edits. No candidate has been accepted or claimed.
- Observation versus inference: no implementation result exists yet. The behavior under test is a POSIX filesystem/native reader case; even a pass would not establish original-game loader behavior, real-save compatibility, or game save/load.
- Files changed at this checkpoint: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, generated `docs/FOLDERS.md`, and this checkpoint; no code changed. Existing user-owned API/Graft changes remain untouched and unstaged.
- Exact next action: collect the worker result, inspect the one-file diff, run an independent root configure/build/focused CTest, and obtain read-only review before marking the fixture complete.
