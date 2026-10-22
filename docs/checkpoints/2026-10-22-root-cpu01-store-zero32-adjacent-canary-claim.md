# Checkpoint: CPU-01-STORE-ZERO32-ADJACENT-CANARY-FIXTURE-01 — claimed

- Time / timezone: 2026-10-22 04:05 CDT (America/Chicago)
- Agent / integrator: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`);
  proposed implementation owner `/root/next_fixture_scout_3` after claim publication.
- Task / gate / status: `CPU-01-STORE-ZERO32-ADJACENT-CANARY-FIXTURE-01`, child of active `CPU-01`;
  `ACTIVE`; root baseline passed; zero implementation attempts and zero failed tries.
- Ownership / scope: only `native/tests/test_store_zero32.cpp`. Add one synthetic three-element
  `std::uint32_t` array case: call `store_zero32` on the middle element, then assert the left and
  right canaries retain their initialized values and the middle element is zero. Do not change the
  helper, header, CMake, or original game data.
- Baseline source hashes: `test_store_zero32.cpp`
  `25cbfe3e22417604cc699a80fcd00b5ecc05107c8e5ccf5880e967c705229a12`; unchanged
  `store_zero32.cpp`, `store_zero32.hpp`, and `native/CMakeLists.txt` hashes
  `fd87c3ad39a28d870b7f503a9edb799e2749a10dc5c1a99cf760f9182a88573c`,
  `66d6205b9d1563a323cd09dfd59cd72b1b653292ea4eca8f15f82d8036684d2f`, and
  `cd46d94590f58515152155658f609cd11ef58aeb9a45272f744402edcdeeb01f`.
- Root Trail evidence: `trail grep 'store_zero32' --in native/` exits 0 with 7 hits in 5 symbols
  across 3 files. The helper resolves at `native/core/store_zero32.cpp:L5-L7`; the current test's two
  cases are in `native/tests/test_store_zero32.cpp:L8-L17`. Trail did not index the CMake target
  spelling; the live registration was verified at `native/CMakeLists.txt:L385-L387`. Trail log
  SHA256 `38dd8281a29b8aeb9fcac0e7fc1b876012f11515174a2b8aa6c95c33dafadd01`.
- Exact root baseline commands (CWD `/Volumes/Extra/SoC Mac Native`; fresh Debug/Ninja directory
  `local/build/native-root-cpu01-store-zero32-canary-baseline-20261022`):
  `cmake -S native -B local/build/native-root-cpu01-store-zero32-canary-baseline-20261022 -G Ninja -DCMAKE_BUILD_TYPE=Debug`
  — exit 0, configure log SHA256 `b59ce91eef3b31788a9fada1f68f188e30e5047650cebb0cc6445cc5385c87fa`;
  `cmake --build local/build/native-root-cpu01-store-zero32-canary-baseline-20261022 --target test_store_zero32`
  — exit 0, build log SHA256 `3f54d22bd36ec78c0214fbc2f59a2702cabb614691eba8e790907f607cc646a9`;
  `ctest --test-dir local/build/native-root-cpu01-store-zero32-canary-baseline-20261022 --output-on-failure -R '^store_zero32$'`
  — exit 0, focused CTest passes 1/1, log SHA256
  `b4807b32d886fd7b9d3717be83626cbf45f99ad9db762363f34e21705934a010`. Logs are under
  `local/validation/20261022-root-fixture-baselines/`.
- Observation versus inference: current checks show zeroing for two standalone values, but do not
  pin that neighboring words remain unchanged. A three-word array fixture would pin the helper's
  local write boundary; this does not show that a game callsite reaches the helper.
- Remaining risk: this is a synthetic local helper fixture. No game callsite, game runtime behavior,
  or broader memory-safety claim is established.
- Publication / dispatch: the claim is in pushed commit `cd4efb6`, fetch-verified at 0/0 divergence.
  The test remains unedited pending the root's publication audit-record commit. Root owns docs,
  independent replay, and publication; user-owned `native/api` changes stay untouched.
- Next exact action: push the completion audit record, then dispatch the single-file test edit.
- Task status and attempt count: `ACTIVE`; zero implementation attempts; zero failed tries.
