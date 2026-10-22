# Checkpoint: SAV-01-CHUNK-CODEC-UNKNOWN-PAYLOAD-KIND-FIXTURE-01 — claimed

- Time / timezone: 2026-10-22 03:07 CDT (America/Chicago)
- Agent and integrator: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`);
  implementation assigned to `/root/next_fixture_scout_2` after claim publication.
- Task / gate / status: `SAV-01-CHUNK-CODEC-UNKNOWN-PAYLOAD-KIND-FIXTURE-01`, child of
  `SAV-01-NATIVE-STREAM-01`; `ACTIVE`, baseline complete, worker implementation in progress, zero
  failed tries.
- Ownership / scope: only `native/tests/test_save_chunk_codec.cpp`, inside
  `test_decode_by_spec_every_kind()`. Add a synthetic chunk whose tag matches a `ChunkSpec` with
  `kind = static_cast<PayloadKind>(0xff)` and assert `decode_chunk_by_spec()` returns
  `CodecError::wrong_value_type`. Do not modify production or build files.
- Baseline hashes: test `5ba469fc86ea36ee8992bcff31040b7f149d5725f1a137faafdc06a1c4289b42`; unchanged
  `native/core/save_chunk_codec.cpp`, `native/core/save_chunk_codec.hpp`, and
  `native/CMakeLists.txt` hashes `e9c3f01b321377fccebc2feab24ada7238ad2ce1aa0d992277d503b97978598d`,
  `aabdd64ab2ce3cbf8d359572e1b2de958e984ddc4aac6925ec8337253a76f88a`, and
  `cd46d94590f58515152155658f609cd11ef58aeb9a45272f744402edcdeeb01f`.
- Exact baseline commands, CWD `/Volumes/Extra/SoC Mac Native`, AppleClang 21 / Ninja; all exit 0:
  `cmake -S native -B local/build/native-root-sav01-unknown-payload -G Ninja -DCMAKE_BUILD_TYPE=Debug`;
  `cmake --build local/build/native-root-sav01-unknown-payload --target test_save_chunk_codec`;
  `ctest --test-dir local/build/native-root-sav01-unknown-payload --output-on-failure -R '^save_chunk_codec$'`
  -> 1/1 passed. Configure/build/CTest log SHA256 values are
  `cd9426fc421f8e81b68a94a0cfd364e01702b613c1238bd98cc88384a89692f3`,
  `dbe6c7f49092d9fe8b2a27d9f109dc4eff9c12ab30c383b527cf9e715a849c59`, and
  `5825c1c9b5038417d7d4cd694d58c4fbfe3e4563ce06164e5a6a1c86bc8fde6e` under
  `local/validation/20261022-root-fixture-baselines/`.
- Trail evidence: `trail grep "test_decode_by_spec_every_kind"` locates the test at
  `native/tests/test_save_chunk_codec.cpp:L828-L879` and its call at L902; log SHA256
  `be50150e488e82a963a3283e92383cb40c362f8eea3724e17902a982a832443d`. A separate live-file read
  confirmed that the decoder checks `chunk.tag == spec.tag`, switches on `spec.kind`, and returns
  `wrong_value_type` after the switch. The existing generic test covers all eight declared kinds,
  truncation, trailing bytes, and retagging, but no out-of-range kind.
- Project-index generation/check pass at 30,727 folders; log SHA256 values are
  `403d44d64815f53a47df64617bebef0735cffa18bf2b8645d90a419113a2e02a` /
  `d5e19dfbd22ab166fb742ea11b91cfc006c793570e62bf3f99165d8def053e90`. `trail build --check`
  passes at 3,618 nodes / 1,240 files, with the deep meaning layer not built; log SHA256
  `f7231c65a9ccdc9f1ec3de943e3fe9b3a2be977fdf687c86e771d1c9c416a2dc`. `git diff --check` exits
  0; log SHA256 `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`.
- Observation versus inference / limits: a matching tag avoids the earlier `tag_mismatch` return;
  an invalid underlying enum value currently reaches the decoder's terminal error. The enum comment
  calls `wrong_value_type` a writer error, while this decoder fallback uses it. The test would pin
  that current synthetic behavior only, not save compatibility or game behavior.
- Failed tries: zero. No implementation edits or dependency changes. `INSTALL_LOG.md` unchanged.
- Remaining risk: no unresolved implementation choice for the one assertion; root will replay the
  changed focused target after the worker returns.
- Claim publication: commit `ff63a179c0c9000a6a9a71472741c5657ffee9ef` is pushed to `origin/main`;
  pre/post fetch divergence was 0/0; explicit audits accepted 5 paths and full audits accepted
  1,723 tracked paths. See E298 for hashes. User-owned `native/api/CMakeLists.txt` and
  `native/api/host/` remain unstaged.
- Current source state: `/root/next_fixture_scout_2` has started the one-file change in
  `native/tests/test_save_chunk_codec.cpp`; the diff is unverified and the worker has not returned
  its targeted check result. This is one implementation attempt in progress; failed tries remain 0.
- Next exact action: wait for the worker's focused build and `save_chunk_codec` CTest result, then
  independently configure/build and run the same focused CTest from a fresh root build directory.
