# Checkpoint: CPU-01-NAMEHASH-INLINE-EMBEDDED-NUL-FIXTURE-01 — fixture verified

- Time / timezone: 2026-10-22 04:07 CDT (America/Chicago)
- Agent / integrator: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`); worker
  `/root/next_fixture_scout_2` owned the single test-file edit.
- Task / gate / status: `CPU-01-NAMEHASH-INLINE-EMBEDDED-NUL-FIXTURE-01`, child of
  `CPU-01-NAMEHASH-E71990-01`; `FIXTURE_VERIFIED`; one successful implementation attempt, zero
  failed tries.
- Files changed by implementation: only `native/tests/test_name_hash.cpp`. The added assertion
  checks `name_hash_inline("abc\0def") == 0x0b873285u`, pinning the inline C-string helper's stop at
  the first NUL. The implementation, header, CMake files, and original game-name data are unchanged.
- Final source hashes: `test_name_hash.cpp`
  `c03310dceae0d3250932c020a4db3cd6e6eb99485643675c6eab403fcd439d94`; unchanged `name_hash.cpp`,
  `name_hash.hpp`, and `native/CMakeLists.txt` hashes
  `3d00d9dd2821801481b9a12e6c57fe4e7401e27fbe02af69ec2683d85cabe4a7`,
  `ca3bee89f348b67d128f9c25c8fba6793080b9bd1aeb64a8e1da190c0dd17b8e`, and
  `cd46d94590f58515152155658f609cd11ef58aeb9a45272f744402edcdeeb01f`.
- Worker commands (CWD `/Volumes/Extra/SoC Mac Native`; fresh Debug/Ninja directory
  `local/build/native-worker-cpu01-namehash-inline-embedded-nul-20261022`): configure, build
  `test_name_hash`, and focused `name_hash` CTest all exit 0; CTest passes 1/1. Configure/build/CTest/
  diff-check log SHA256 values are
  `7043082df5af531cba211b98ab4cbbe0655828f1d567e3d81f39116c621aca75`,
  `3249d74eddb93166b976d8e9d9658067049a21ddab090b866667352e713d13f5`,
  `97d41ee0102b75a33880e93183e14e2bee0d0e6797d6d5eccd4a1a50f5a5dc45`, and
  `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`.
- Independent root commands (CWD `/Volumes/Extra/SoC Mac Native`; fresh Debug/Ninja directory
  `local/build/native-root-cpu01-namehash-inline-embedded-nul-root-replay-20261022`):
  `cmake -S native -B local/build/native-root-cpu01-namehash-inline-embedded-nul-root-replay-20261022 -G Ninja -DCMAKE_BUILD_TYPE=Debug`
  — exit 0, configure log SHA256 `e4aa950a30a717f57f91b249d3da35bda826a56797bc53303222e1ad54e98b26`;
  `cmake --build local/build/native-root-cpu01-namehash-inline-embedded-nul-root-replay-20261022 --target test_name_hash`
  — exit 0, build log SHA256 `3824aae71f7dc603cfe566630fa5851b165752dca9161d196357302fa4f17df3`;
  `ctest --test-dir local/build/native-root-cpu01-namehash-inline-embedded-nul-root-replay-20261022 --output-on-failure -R '^name_hash$'`
  — exit 0, focused CTest passes 1/1, log SHA256
  `5fe0bb7c78bfdfcc1c1c9bd4ef15720004e8629ba95ac83986eb83c1f59d13a4`; diff-check log SHA256
  `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`.
- Root Trail post-change query `trail grep 'name_hash_inline' --in native/` exits 0 with 10 hits in
  3 symbols across 3 files; the added assertion resolves at
  `native/tests/test_name_hash.cpp:L43`. Log SHA256
  `059be0b5e001a38715923086888141aa58288b07786facd3d9353635cf19b102`.
- Folder-index generation/check both pass at 39,266 folders; log SHA256 values are
  `3896f1665051e57afc385d53f173f7ebb1ea40b624701b2361dd63cf71ce884d` and
  `30eb6827ab873ac5c68f847c31326fa5ba0155816ad3578d92a7b60d0c482247`. `trail build --check`
  passes with a synchronized 3,618-node / 1,240-file wiring graph; log SHA256
  `f7231c65a9ccdc9f1ec3de943e3fe9b3a2be977fdf687c86e771d1c9c416a2dc`. `git diff --check` exits
  0 with empty output (SHA256 `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`).
- Observation versus inference: the inline helper returns the prefix hash for the synthetic C string
  whose first NUL follows `abc`; the test pins this helper behavior. It does not demonstrate that any
  original game name has an embedded NUL or is required by the port.
- Remaining risk / acceptance: deterministic helper fixture only; original game-name import and game
  runtime behavior remain separate questions.
- Publication state: the claim is in `eaa9e0b405175b78ac35c756155ae525c8f71f20`; completion and the
  `store_zero32` claim are in pushed commit `cd4efb6`, fetch-verified at 0/0 divergence. Pre-push
  staged/full audits accepted 9/1,733 paths; post-commit explicit path/full-tree audits accepted
  9/1,733 paths. E316 records log hashes. User-owned `native/api/CMakeLists.txt` and
  `native/api/host/` remain untouched.
- Next exact action: publish the audit record, then dispatch and independently replay the
  `store_zero32` fixture.
- Task status and attempt count: `FIXTURE_VERIFIED`; one successful implementation attempt; zero
  failed tries.
