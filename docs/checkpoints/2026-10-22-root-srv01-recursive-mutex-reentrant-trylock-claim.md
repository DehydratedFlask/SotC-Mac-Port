# Checkpoint: SRV-01-RECURSIVE-MUTEX-REENTRANT-TRYLOCK-FIXTURE-01 — claimed

- Time / timezone: 2026-10-22 03:45 CDT (America/Chicago)
- Agent / integrator: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`);
  proposed implementation owner `/root/next_fixture_scout_2` after claim publication.
- Task / gate / status: `SRV-01-RECURSIVE-MUTEX-REENTRANT-TRYLOCK-FIXTURE-01`, child of
  `SRV-01-RECURSIVE-MUTEX-01` and `CPU-01-MUTEX-CONTRACT-01`; `ACTIVE`; baseline passed; zero
  implementation attempts and zero failed tries.
- Ownership / scope: only `native/tests/test_recursive_mutex.cpp::main()`. While the current thread
  already holds the recursive mutex twice, add `CHECK(mutex.try_lock() == 0);` followed by
  `CHECK(mutex.unlock() == 0);` to balance the added recursion level. No implementation, header,
  CMake, or other test changes.
- Source hashes before implementation: `test_recursive_mutex.cpp`
  `9203b9d055ae46c775c733386056bbab778639ddc9eccd85f53bc293a62c8610`; unchanged
  `recursive_mutex.cpp`, `recursive_mutex.hpp`, and `native/CMakeLists.txt` hashes
  `f61fb74be7c771b35203a9cf98d658fd3bba4d00c2c023e19404b37a4f18b6aa`,
  `f97aea6beb99aed0bc5348bb92dc70d059b965558d1eaef60c4c57600678d5df`, and
  `cd46d94590f58515152155658f609cd11ef58aeb9a45272f744402edcdeeb01f`.
- Root Trail evidence: `trail grep 'try_lock' --in native/` exited 0 and returned 5 hits in 4
  symbols across 3 files. It found only uninitialized `EINVAL` checks and a competing thread's
  `EBUSY`/post-unlock checks in the test; the owner-side recursive success path is absent. The
  wrapper delegates to `pthread_mutex_trylock`. Persisted output SHA256
  `55f94ecca80d16622aa499aaf5d1fa3b76f3c8a59f0ba5e6a968b62ec3e3fbc9`.
- Exact root baseline commands (CWD `/Volumes/Extra/SoC Mac Native`; fresh AppleClang 21 / Ninja
  Debug build directory `local/build/native-root-srv01-recursive-mutex-reentrant-trylock-baseline-20261022`):
  `cmake -S native -B local/build/native-root-srv01-recursive-mutex-reentrant-trylock-baseline-20261022 -G Ninja -DCMAKE_BUILD_TYPE=Debug`
  — exit 0, configure log SHA256
  `e88623602346d4f959c0e9569474214370719334e86780271479a5efe8fee2d7`;
  `cmake --build local/build/native-root-srv01-recursive-mutex-reentrant-trylock-baseline-20261022 --target test_recursive_mutex`
  — exit 0, build log SHA256
  `d502ed29d2f17fec40174fe665f02ef9ab8b634d3f73542589c00c004de9c93f`;
  `ctest --test-dir local/build/native-root-srv01-recursive-mutex-reentrant-trylock-baseline-20261022 --output-on-failure -R '^recursive_mutex$'`
  — exit 0, focused CTest 1/1 passed, log SHA256
  `17cde73cf0586c3e0f4c67e870225aafb3080957051668d79a9ca7236d45b6aa`. Logs are under
  `local/validation/20261022-root-fixture-baselines/`.
- Observation versus inference: current-thread recursive success is the proposed behavior to pin;
  the passing baseline only shows current tests do not fail before the new assertion. This does not
  characterize scheduler priority inheritance under contention or game behavior.
- Remaining risk: run the assertion on the target Darwin pthread implementation and verify the
  extra recursive acquisition is released before the existing competing-thread assertions.
- Next exact action: after the claim and completion publication is verified on `origin/main`, dispatch
  `/root/next_fixture_scout_2` to edit only the test file and run the focused target/build checks.
  Root retains docs, integration, and publication ownership. Existing user-owned
  `native/api/CMakeLists.txt` and untracked `native/api/host/` stay outside scope.
- Task status and attempt count: `ACTIVE`; zero implementation attempts and zero failed tries.
