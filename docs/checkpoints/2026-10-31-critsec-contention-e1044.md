# CritSec sidecar contention repair — E1044

- **Local time / agent:** 2026-10-31 01:09 CDT, `/root` integrating `/root/prior_work_audit`
- **Task:** `RE-02-CRITSEC-CONTENTION-01`
- **Status:** `COMPLETE / FIXTURE_VERIFIED`; failed implementation hypotheses: 0.
- **Files:** `native/core/critsec_3b310.cpp`, `native/core/critsec_3b310.hpp`, `native/tests/test_critsec_3b310.cpp`, plus `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/INDEX.md`, and this checkpoint. Worker did not edit CMake, `INSTALL_LOG.md`, or Git state.

## Change

`Sidecar` now lives at a stable heap address owned by `std::unique_ptr`; an initialized `pthread_mutex_t` is never copied. The registry lock protects lookup, lease counts, and closing state only. Each lock/unlock/destroy operation takes a per-call lease before releasing the registry lock, so a blocked waiter does not prevent its owner from entering unlock. Destroy uses a nonblocking mutex probe, rejects active/held state while retaining the token, blocks new leases only after reaching an exclusive state, and clears the opaque 8-byte token after successful pthread destruction.

The fixture covers recursive acquisition, a held-mutex destroy rejection, and a two-thread owner/waiter/unlock sequence. It uses promise/future handshakes and bounded waits; CTest did not hang.

## Verification

Worker commands (all exited 0):

```sh
cmake --build local/build/orch-r22 --target test_critsec_3b310 -j 12
ctest --test-dir local/build/orch-r22 -R '^critsec_3b310$' --output-on-failure
python3 tools/lift/check_critsec_3b310_3b380.py
```

The worker's focused CTest passed 1/1 in 0.29 s and the checker returned `OK`. The integrator then ran:

```sh
ctest --test-dir local/build/orch-r22 -R '^critsec_3b310$' --repeat until-fail:20 --output-on-failure
```

All 20 repetitions passed (2.65 s total). Worker logs and exact commands: `local/validation/critsec-e1044/`; repeat output: `local/validation/critsec-e1044-integrator/repeat-ctest.log`.

## Evidence boundary and remaining risks

- **Observed:** stable sidecar ownership in the source; lease and closing checks; focused fixture and byte checker pass; repeated test pass.
- **Inference:** pthread type value 2 maps to `PTHREAD_MUTEX_RECURSIVE` for this host model. The pinned PS4 contract and retail contention behavior remain unproven.
- `Slot414Hooks` carries the destroy hook, but `init_slot414_57b9b0` only records the `0x3b380` atexit identity; actual teardown callback execution remains outside this task. `StartupState` RAII ownership remains unchanged.
- Full CTest and the full lift-checker sweep were **NOT RUN** here. G0–G6 do not advance; no game behavior is inferred.

## Next exact action

Track the separate `0x499f80` failure-path ownership limitation in E1046; keep any host rollback semantics distinct from observed retail behavior.
