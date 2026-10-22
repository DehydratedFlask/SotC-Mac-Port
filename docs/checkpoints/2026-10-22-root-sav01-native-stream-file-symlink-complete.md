# Checkpoint: SAV-01-NATIVE-STREAM-FILE-SYMLINK-DESTINATION-FIXTURE-01 — verified

- Time / timezone: 2026-10-22 01:43 CDT (America/Chicago)
- Agent and integrator: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`); implementation by `/root/next_fixture_scout_2`.
- Task / gate / status: `SAV-01-NATIVE-STREAM-FILE-SYMLINK-DESTINATION-FIXTURE-01`, child of
  `SAV-01-NATIVE-STREAM-01`; `FIXTURE_VERIFIED`, one successful implementation attempt, 0 failed
  tries. The G2 real-save gate remains unproven.
- Prerequisite evidence / input and source hashes: parent checkpoint
  `docs/checkpoints/2026-10-21-agentD-sav01-native-stream.md`; G2 validation B06 requires same-volume
  replacement and B07 says atomic writing alone does not prove power-loss durability. Test file
  baseline SHA256 `ffc9a985c134383ae1f3c8af8f6a448b6ab7a8e00113b7c9fb306c03f4504617`, final SHA256
  `2fc07ba535ad267bcd40a74231b22062953f4e714bf10f5110c827450b44aaac`; unchanged implementation
  `native/core/save_stream.cpp` SHA256
  `7bc9763539dfa6fdd40f26d3cd9c1dd4c0d06e50bbd3af87d0c7b1f2cb1c0c7e`.
- Owned / changed files: one test method and its call in `main()` in
  `native/tests/test_save_stream_file.cpp`. No production code changed.
- Hypothesis and expected result: when `write_file_atomic` atomically replaces a destination
  symlink through a same-directory temp-file rename, the destination entry becomes a regular file
  containing new stream bytes, the symlink referent remains byte-identical, and no `.tmp` entry
  remains. The test uses `lstat` so it verifies the directory entry itself.
- Exact command, CWD `/Volumes/Extra/SoC Mac Native`: baseline, worker, and root each ran
  `cmake --build local/build/native --target test_save_stream_file` and
  `ctest --test-dir local/build/native --output-on-failure -R '^save_stream_file$'`.
- Exit / observed result: all three target builds exit 0. Baseline, worker, and root focused CTest
  runs each exit 0 and pass 1/1. Root's build was incremental; worker's build compiled and linked
  the changed test source. `git diff --check` passes.
- Log, capture and REA evidence paths/IDs: build log SHA256 baseline/root
  `e498f88e3ddd30adf6b222cc60decde2b2ecab59b135e58bc2e629ee7f923598`, worker
  `23b802e59005a3cb8e43be3619a0ead486d249860d2c858aa477800b8d2e50b6`; CTest logs SHA256 baseline
  `a995ef51f87ef41b99020281aa60971b70be936cf09c8a78a71347e332e6fdd8`, worker
  `b2c646782762a540fd133e08344471de26303dc33cdf55e33b34b2595faff73e`, root
  `b7f494b756da79bcf0d8880fa67920944736a53e2547a31d84ac1b141566f3f4`. All logs are under
  `local/validation/20261022-root-fixture-baselines/`. Trail query resolves the new method at
  `native/tests/test_save_stream_file.cpp:L104-L119` and its `main()` call at L238. No REA/game
  input is involved.
- Observation versus inference / coverage limitations: current POSIX helper behavior is pinned
  against a synthetic temporary directory. No real game save, payload compatibility, concurrent
  writer behavior, power-loss durability, game progress, or save/load after relaunch is shown.
- Remaining risks / blockers: parent stream payload semantics are still unknown. No blocker for
  this test-only fixture.
- Index and Trail checks: `python3 scripts/project_index.py` and its `--check` exit 0 at 24,080
  folders. `trail build --check` exits 0 at 3,617 nodes / 1,240 files (deep meaning layer unbuilt).
  The Trail query resolves the new method at `native/tests/test_save_stream_file.cpp:L104-L119`
  and its `main()` call at L238. Combined output is
  `local/validation/20261022-root-fixture-baselines/sav01-native-stream-file-symlink-complete-work.log`,
  SHA256 `b1d83d4c38401086e7ead9c309dcdb6748e2aa6df045053e5a395e528f1ae6f7`.
- Next exact action: run staged/full path audits, commit the test and completion records, push, then
  verify fresh fetch/divergence and post-push path audits.
- HANDOFF.md / TASKS / EVIDENCE / INSTALL_LOG / indexes updated: HANDOFF and TASKS mark the task
  verified; E278 records test evidence. `INSTALL_LOG.md` has no change. Regenerate `docs/FOLDERS.md`
  after this checkpoint and before commit; it is regenerated and current at 24,080 folders.
