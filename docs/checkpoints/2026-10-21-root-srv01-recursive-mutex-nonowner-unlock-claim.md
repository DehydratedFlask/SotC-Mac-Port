# SRV-01 recursive-mutex non-owner unlock — claim checkpoint

- Local date/time: 2026-10-21 23:16 CDT (America/Chicago).
- Agent: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`), integrator.
- Task: `SRV-01-RECURSIVE-MUTEX-NONOWNER-UNLOCK-FIXTURE-01`, child of
  `SRV-01-RECURSIVE-MUTEX-01` and `CPU-01-MUTEX-CONTRACT-01`.
- Status: `ACTIVE`; 0 failed implementation tries. No source/test changes have been made for this task.
- Scope: one synthetic test-only case in `native/tests/test_recursive_mutex.cpp`; while the main
  thread holds two recursive levels, a competing thread directly calls `unlock()` and must receive
  `EPERM`. Verify the owner still releases both recursion levels. Leave production files and CMake
  registration unchanged.
- Baseline build command: `cmake --build local/build/native --target test_recursive_mutex` — exit 0.
  Log `local/validation/srv01-recursive-mutex-nonowner-unlock/baseline-build.log`, SHA256
  `f823026168d3d6a293b9b62fca8edb44de342440ea39cfb6c85e2747a039eebe`.
- Baseline test command: `ctest --test-dir local/build/native -R '^recursive_mutex$' --output-on-failure`
  — exit 0, 1/1 passed. Log `local/validation/srv01-recursive-mutex-nonowner-unlock/baseline-test.log`,
  SHA256 `62b9e1d844bae30e303d39d20cf782dac596e6f996b27cf555ba9a74426bbcad`.
- Baseline hashes: test `native/tests/test_recursive_mutex.cpp`
  `656bcccc6c9bb11566b9920e02a68b13032efbb4ff816ca3285fce60925ea6d9`; implementation
  `native/core/recursive_mutex.cpp` `f61fb74be7c771b35203a9cf98d658fd3bba4d00c2c023e19404b37a4f18b6aa`;
  header `native/core/recursive_mutex.hpp`
  `f97aea6beb99aed0bc5348bb92dc70d059b965558d1eaef60c4c57600678d5df`.
- Observation: `RecursiveMutex::unlock()` directly returns `pthread_mutex_unlock()`'s status when
  initialized. Existing test helper calls `unlock()` only after a successful `try_lock()`; when
  `try_lock()` is `EBUSY`, its `unlock_rc` remains its default `EINVAL`, so current tests never make a
  wrong-owner unlock call. Apple's manual page specifies `EPERM` when the current thread holds no
  lock on the mutex: https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man3/pthread_mutex_unlock.3.html.
  This source-backed contract does not prove project wrapper behavior until the fixture passes.
- Files changed at claim: `HANDOFF.md`, `docs/TASKS.md`, and this checkpoint only. The test and
  implementation files remain unchanged. Existing user-owned `native/api/CMakeLists.txt` and
  `native/api/host/` changes remain untouched and unstaged.
- Remaining risks: this exercises the current local Darwin pthread implementation only. It does not
  test scheduler priority inheritance under contention or establish game mutex equivalence.
- Next exact action: add a direct non-owner unlock helper/case, assert `EPERM`, verify the owner can
  still unlock twice, rebuild `test_recursive_mutex`, and run the focused CTest; return the test
  change and logs to the root integrator for completion documentation and publication.
