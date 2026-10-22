# Checkpoint: SRV-01-ALLOCATOR-ALIGN64K-UINT32-WRAP-FIXTURE-01 — claimed

- Time / timezone: 2026-10-22 03:22 CDT (America/Chicago)
- Agent and integrator: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`);
  proposed implementation owner `/root/next_fixture_scout_3` after claim publication.
- Task / gate / status: `SRV-01-ALLOCATOR-ALIGN64K-UINT32-WRAP-FIXTURE-01`, child of active
  `SRV-01`; `ACTIVE`; root baseline complete; zero implementation attempts and zero failed tries.
- Ownership / scope: only `native/tests/test_allocator_region.cpp::main()`. Add one assertion in the
  existing alignment block: `CHECK(align_up_64k(0xffff0001u) == 0u);`. No implementation, header,
  CMake, or other test changes.
- Baseline hashes: test `b3b2dd3c339338304f5bef6b3fa7b716dfeead3e97f36d3718a8fade3a56bd4d`; unchanged
  `native/core/allocator_region.hpp`, `native/core/allocator_region.cpp`, and
  `native/CMakeLists.txt` hashes `6bc5fc4091d6a5ce8411d0beb0020fa07e5e93d2846c9169d7081d9a03c3696a`,
  `7ea6e93c79f43dfbad5bb447008a39c77136633f3afe97dde493d57dd69c9da8`, and
  `cd46d94590f58515152155658f609cd11ef58aeb9a45272f744402edcdeeb01f`.
- Exact root baseline commands (CWD `/Volumes/Extra/SoC Mac Native`; AppleClang 21 / Ninja; fresh
  build directory `local/build/native-root-srv01-align64k-wrap-20261022`):
  `cmake -S native -B local/build/native-root-srv01-align64k-wrap-20261022 -G Ninja -DCMAKE_BUILD_TYPE=Debug`
  — exit 0, log SHA256
  `b0e1f031dcb5ad19417469e7a28c2ca71dc87249bf5a38bc02ea5b5ec315fd49`;
  `cmake --build local/build/native-root-srv01-align64k-wrap-20261022 --target test_allocator_region`
  — exit 0, log SHA256
  `f1914c0780655641268b3f1148f04492888b239ba611b39b15f200d770e7c30a`;
  `ctest --test-dir local/build/native-root-srv01-align64k-wrap-20261022 --output-on-failure -R '^allocator_region$'`
  — exit 0, focused CTest 1/1 passed, log SHA256
  `c8f6c359f7ab4cbb75911a9897cdcd236c10581790a005025d9a4893104e8afa`. Logs are under
  `local/validation/20261022-root-fixture-baselines/`.
- Trail evidence: `trail grep "align_up_64k"` returned the helper at
  `native/core/allocator_region.hpp:L50-L52` and current alignment checks in
  `native/tests/test_allocator_region.cpp:L18-L22`. The helper adds `0xffffu` in `uint32_t` then
  masks with `0xffff0000u`; for `0xffff0001u` that addition wraps to zero. This is an observation
  about the current helper, not a claim about allocator invocation or game usage.
- Folder index: `python3 scripts/project_index.py` exits 0 and writes the inventory at 31,946
  folders; log SHA256 `444d60085795ec43532e888829d17f2e2a4b6c2d8f3cdc084d3062b46e2002e5`.
  `python3 scripts/project_index.py --check` exits 0 with the index current at 31,946 folders;
  log SHA256 `f796d8c71d085086826055fa5a156e32d5c301e906c9e4421e4d84c61f5f579b`.
  `trail build --check` exits 0 at 3,618 nodes / 1,240 files; the wiring graph is current and the
  meaning layer remains unbuilt; log SHA256
  `f7231c65a9ccdc9f1ec3de943e3fe9b3a2be977fdf687c86e771d1c9c416a2dc`.
  `git diff --check` exits 0; log SHA256
  `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`.
- Claim publication is pending. `INSTALL_LOG.md` is unchanged. The existing user-owned
  `native/api/CMakeLists.txt` edit and untracked `native/api/host/` are not part of this task.
- Next exact action: finish project index / Trail checks and publish this claim using explicit
  allowlisted paths; only then send the worker the one-file implementation task.
