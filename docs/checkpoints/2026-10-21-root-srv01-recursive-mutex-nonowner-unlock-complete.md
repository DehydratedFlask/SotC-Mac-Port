# SRV-01 recursive-mutex non-owner unlock — completion checkpoint

- Local date/time: 2026-10-21 23:22 CDT (America/Chicago).
- Agent: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`), integrator.
- Task: `SRV-01-RECURSIVE-MUTEX-NONOWNER-UNLOCK-FIXTURE-01`, child of
  `SRV-01-RECURSIVE-MUTEX-01` and `CPU-01-MUTEX-CONTRACT-01`.
- Status: `FIXTURE_VERIFIED`; 1 successful implementation attempt, 0 failed implementation tries.
- Files changed: `native/tests/test_recursive_mutex.cpp`, `HANDOFF.md`, `docs/TASKS.md`,
  `docs/EVIDENCE.md`, and this checkpoint. Production mutex implementation, header, and CMake target
  were unchanged. User-owned `native/api/CMakeLists.txt` and `native/api/host/` remained untouched.
- Baseline build command: `cmake --build local/build/native --target test_recursive_mutex` — exit 0.
  Log `local/validation/srv01-recursive-mutex-nonowner-unlock/baseline-build.log`, SHA256
  `f823026168d3d6a293b9b62fca8edb44de342440ea39cfb6c85e2747a039eebe`.
- Baseline test command: `ctest --test-dir local/build/native -R '^recursive_mutex$' --output-on-failure`
  — exit 0, 1/1 passed. Log `local/validation/srv01-recursive-mutex-nonowner-unlock/baseline-test.log`,
  SHA256 `62b9e1d844bae30e303d39d20cf782dac596e6f996b27cf555ba9a74426bbcad`.
- Worker final build/test commands are the same; build exits 0 and focused CTest exits 0, 1/1 passed.
  Logs `nonowner-build.log` and `nonowner-test.log`, SHA256 `d90f78e13fff2c5e916fb0636e4c93c67c9755c978098510f22ab9ad579b4e34` and
  `db705bb5fa406b5e590b5357cb8a3a0a3f9658661e9433df936ddb257c8ccd90`.
- Root independently replayed both commands; build exits 0 and CTest exits 0, 1/1 passed. Logs
  `root-build.log` and `root-test.log`, SHA256 `f823026168d3d6a293b9b62fca8edb44de342440ea39cfb6c85e2747a039eebe` and
  `99d275b0c40454544b610f3ab134aea4f2f59a514f39fbac31b01b50a33a6d00`.
- Final test SHA256: `9203b9d055ae46c775c733386056bbab778639ddc9eccd85f53bc293a62c8610`; unchanged implementation
  SHA256 `f61fb74be7c771b35203a9cf98d658fd3bba4d00c2c023e19404b37a4f18b6aa`; unchanged header SHA256
  `f97aea6beb99aed0bc5348bb92dc70d059b965558d1eaef60c4c57600678d5df`.
- Observation: while the owning thread held two recursive levels, a second thread directly invoked
  `unlock()` and observed `EPERM`; the owner then released both levels. The targeted test remained
  passing on root replay. Apple's pthread manual documents `EPERM` when the current thread does not
  hold the mutex: https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man3/pthread_mutex_unlock.3.html.
  This is local Darwin evidence, not a scheduler priority-inheritance measurement.
- `git diff --check -- native/tests/test_recursive_mutex.cpp` passed. No production behavior changed.
- Remaining risks: this does not test scheduler priority inheritance under contention, original game
  mutex equivalence, or game behavior. User-owned native API changes remain uncommitted and excluded.
- Next exact action: claim `APP-01-SYSTEM-CHECKS-REJECT-32BIT-MACHO-01`, then add and run its synthetic
  i386 classification fixture if the live task board remains clear.
