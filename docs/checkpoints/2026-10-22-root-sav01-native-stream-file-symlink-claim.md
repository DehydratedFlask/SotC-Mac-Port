# Checkpoint: SAV-01-NATIVE-STREAM-FILE-SYMLINK-DESTINATION-FIXTURE-01 — claimed

- Time / timezone: 2026-10-22 01:36 CDT (America/Chicago)
- Agent and integrator: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`)
- Task / gate / status: `SAV-01-NATIVE-STREAM-FILE-SYMLINK-DESTINATION-FIXTURE-01`, child of
  `SAV-01-NATIVE-STREAM-01`; `ACTIVE`, implementation not started, 0 failed tries.
- Prerequisite evidence / input and source hashes: parent checkpoint
  `docs/checkpoints/2026-10-21-agentD-sav01-native-stream.md`; G2 validation B06 says use
  same-volume replacement and B07 warns atomic writing alone does not prove power-loss durability.
  Baseline test file `native/tests/test_save_stream_file.cpp` SHA256
  `ffc9a985c134383ae1f3c8af8f6a448b6ab7a8e00113b7c9fb306c03f4504617`; unchanged implementation
  `native/core/save_stream.cpp` SHA256
  `7bc9763539dfa6fdd40f26d3cd9c1dd4c0d06e50bbd3af87d0c7b1f2cb1c0c7e`.
- Owned / changed files: one synthetic test method and its registration in
  `native/tests/test_save_stream_file.cpp`. No production-code edit. Claim records are this file,
  `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, and generated folder indexes.
- Hypothesis and expected result: when `write_file_atomic` replaces a destination symlink using
  same-directory temporary-file rename, the destination directory entry becomes a regular file
  containing the new stream; the prior symlink referent retains its original bytes and the `.tmp`
  file is removed. The test must use `lstat` to distinguish replacing the link from following it.
- Exact baseline commands, CWD `/Volumes/Extra/SoC Mac Native`:
  `cmake --build local/build/native --target test_save_stream_file` -> exit 0;
  `ctest --test-dir local/build/native --output-on-failure -R '^save_stream_file$'` -> exit 0, 1/1.
- Exit / observed result: both baseline commands passed before any source change. Logs are
  `local/validation/20261022-root-fixture-baselines/sav01-native-stream-file-symlink-baseline-build.log`
  (SHA256 `e498f88e3ddd30adf6b222cc60decde2b2ecab59b135e58bc2e629ee7f923598`) and
  `local/validation/20261022-root-fixture-baselines/sav01-native-stream-file-symlink-baseline-ctest.log`
  (SHA256 `a995ef51f87ef41b99020281aa60971b70be936cf09c8a78a71347e332e6fdd8`).
- Log, capture and REA evidence paths/IDs: baseline logs above; no REA or game input is involved.
- Observation versus inference / coverage limitations: the live helper uses same-volume rename of
  a sibling temp file. POSIX rename is expected to replace the symlink entry; this fixture will pin
  the observed helper behavior only. It does not test power-loss durability, concurrent writers,
  save-format compatibility, or game progress.
- Cumulative failed tries and distinct hypotheses: 0 implementation tries / 0 failed tries.
- Remaining risks / blockers: synthetic filesystem path only; parent stream payload semantics remain
  unknown; no real save compatibility claim. No blocker to the bounded test.
- Claim commit `457601bde1c1f5931590a0c8fb1deb52d55e3e73` was pushed. Pre-push staged/full audits
  accepted 5/1,709 paths; fresh fetch/divergence was 0/0. Post-push explicit/full audits accepted
  5/1,709 paths. The staged-mode audit after commit returned exit 2 because the index had no staged
  paths; the exact-path rerun passed. Publication log:
  `local/validation/20261022-root-fixture-baselines/sav01-native-stream-file-symlink-publication.log`,
  SHA256 `9872813b280b0a12ffe792b32f88add53c214ebba5cc4ae11c06e9255d480105`.
- Next exact action: have `/root/next_fixture_scout_2` finish only
  `native/tests/test_save_stream_file.cpp`, run the target build and focused CTest, and report its
  diff and log hashes. Root reviews and independently replays the commands before task completion.
- HANDOFF.md / TASKS / EVIDENCE / INSTALL_LOG / indexes updated: HANDOFF, TASKS and EVIDENCE are
  updated; INSTALL_LOG has no fact change. `python3 scripts/project_index.py` and its `--check`
  both exited 0 at 24,080 folders. `trail build --check` exited 0 at 3,616 nodes / 1,240 files;
  the deep meaning layer remains unbuilt. Combined output is
  `local/validation/20261022-root-fixture-baselines/sav01-native-stream-file-symlink-work.log`,
  SHA256 `56c3cfce7a7d5cb0626b95c0fa4e2cc256a8b56df55392536d9c1906e597457c`. `git diff --check`
  passed. Existing user-owned `native/api/CMakeLists.txt` and `native/api/host/` remain untouched.
