# Checkpoint: CPU-01-FAMILY-NATIVE-HELPER-01 — tested native lifetime model for the 34-slot family

- Time / timezone: 2026-10-21 01:05 CDT (America/Chicago), macOS 27.0.1 arm64
- Agent and integrator: orchestrator session (`deepseek-v4.1-flash-1017`), integrator-owned record.
- Task / gate / status: CPU-01-FAMILY-NATIVE-HELPER-01. **FIXTURE_VERIFIED**. Nothing registered in
  `startup_ctors`; G1 not passed; slot 11 still blocks the runner.
- Prerequisite evidence / input and source hashes: CPU-01-SLOT10-47-FAMILY-01 (34 slots verified
  against the pinned ELF), `docs/CTOR_CLASSES.md`, `tools/lift/verify_ctor_family.py`, the settled
  `StartupState` RAII decision from slot 10, and `SRV-01-RECURSIVE-MUTEX-01`.
- Owned / changed files: NEW `native/core/descriptor_registration.{hpp,cpp}`,
  NEW `native/tests/test_descriptor_registration.cpp`, `native/CMakeLists.txt`,
  `native/README.md`, this checkpoint.
- Hypothesis and expected result: one small native type can carry the family's *native* lifetime
  contract — publish a descriptor once, own a recursive + priority-inherit mutex, tear down
  deterministically — while deliberately not reproducing the original heap block, the `0x10` size
  word or the `__cxa_atexit` ordering. Expected: a one-shot `publish()` with explicit errors, a
  recursive mutex usable afterwards, and real cross-thread exclusion.
- Exact command, CWD and nonsecret environment: CWD `/Volumes/Extra/SoC Mac Native`
  - `cmake --build local/build/native -j 8` → 0
  - `ctest --test-dir local/build/native` → **264/264 pass** (ASan+UBSan, arm64)
  - `ctest --test-dir local/build/native -R '^descriptor_registration$'` → 1/1 pass
  - Negative control: removed the `descriptor == nullptr` guard, rebuilt, ran the focused test →
    exit 8 (failure), restored the file, rebuilt, focused test passes again.
- Exit / observed result: API `DescriptorRegistration::publish(const void*)` returning
  `DescriptorPublishError {none, null_descriptor, already_published, mutex_init_failed}`; the mutex
  is initialized before the registration is marked published, so a failed `pthread_mutex_init`
  cannot leave a half-published state. Tests assert: default unpublished state; null rejected
  without mutation; second publish does not overwrite the first descriptor; two registrations are
  independent; the mutex is recursive (double lock/unlock on one thread); and a second thread
  blocks until the first unlocks.
- Observation versus inference / coverage limitations:
  - Observation: the native behaviour above, and the 34 original bodies/contracts it models
    (previous checkpoint, replayed by `verify_ctor_family.py`).
  - Inference: that a shared helper is the right shape for the family. It is justified by the
    uniformity of the 34 bodies (same mutex, same cleanup, same two-global publication), not by a
    traced consumer.
  - Not verified / deliberately not modelled: the original 16-byte heap block and its size word
    `[0x12d2d1c] = 0x10`; the `__cxa_atexit(dtor, &global, 0x1604218)` ordering; the virtual
    `call [rbx]` that slots 14/16/17/18 perform after publication; class identities; which slot may
    run first (the six guarded clones share guard cells). No original address is dispatched.
- Cumulative failed tries and distinct hypotheses: 0 failed implementation tries; one deliberate
  negative control (expected failure) recorded above.
- Remaining risks / blockers: this helper cannot be registered while slot 11 is unresolved, and it
  is not a substitute for the per-class descriptor contents. Registering more than one of the six
  guarded clones would also be pointless because they share guard cells.
- Next exact action: when slot 11 is resolved, register the family through this helper behind a test
  that proves original slot order, and extend it only for the virtual-dispatch sub-shape once that
  target is identified.
- HANDOFF.md / TASKS / EVIDENCE / INSTALL_LOG / indexes updated: by the integrator in this session.
