# E1019 — Slot-309 save-consumer callback recheck and boot triage

Date/time: 2026-10-29 21:24 CDT. Agent: `/root`.
Task: `CPU-01-STARTUP-SLOT-309-CHARACTERIZATION-01`.
Status: `PARTIAL / STATIC_ANALYSIS_VERIFIED`; one bounded constructor-to-consumer guard recheck and live boot-task triage; zero failed semantic hypotheses.

## Scope and starting state

This continues game-start work only; VoiceOver/accessibility is out of scope. The worktree started clean on `main` at `6f3c77250f8757c82ae8e360cca1a1bd0a20a83c`. The pinned ELF is `local/re/eboot.elf`, SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`.

## Callback guard result

The exact bounded disassembly command was:

```sh
objdump -D -Mintel --start-address=0xa4d9c0 --stop-address=0xa4de42 local/re/eboot.elf > local/validation/orch-20261029/CPU-01-STARTUP-SLOT-309-SAVECONSUMER-01/consumer-disassembly-rerun.txt 2>&1
```

It exited 0; the output SHA-256 is `f508850930053a828c3c335a768af1fed124c19a6d8edfe6c9f7208089ad5c48`. The existing structural replay at `0xa4d9c0` is SHA-256 `b0d859cfb5c347d6bbdc39f53687023152d6d4fee1a387d3fcd5bc47e6995a18` and binds the 1,154-byte / 266-instruction body to the pinned ELF.

Observed constructor-to-consumer path:

- Slot-309 root `0xa505e0` sets the global address `0x32f79e8` and passes a zeroed 32-byte block as argument 7 to `0xa4f4d0`.
- `0xa4f4d0` copies that block and skips its conditional self-pointer rewrite because the fourth qword is zero. Nested helper `0xa4f690` copies the zero descriptor into the global object's `+0x80..+0x98` fields.
- Consumer `0xa4d9c0` loads `[object+0x80]` and `[object+0x88]`. When both are zero, its branch at `0xa4de03` skips `call rax` at `0xa4de23`. Thus the indirect call has no target for the constructor-initialized state, provided no later mutation occurs first.
- The separate `+0xc0` member callback at `0xa47d45` resolves to `0xa4f4c0` (`ret`) for this constructed object. It is not the target of `0xa4de23`; the enclosing cleanup path through `0x20b50` and complete object ownership remain unresolved.

A literal search for the fixed field addresses `0x32f7a68`, `0x32f7a70`, `0x32f7a78`, `0x32f7a80`, and `0x32f7aa8` in the saved disassembly found no hits (exit 1). This only rules out those exact address literals in that input; it does not exclude writes through aliases or prove runtime ordering. The private recheck is `local/validation/orch-20261029/CPU-01-STARTUP-SLOT-309-SAVECONSUMER-01/RECHECK-A4DE23.md`.

## Live startup status and task triage

Graft lookup plus live-source inspection confirmed that `run_constructors` iterates the supplied sparse recovered span and stops on a null callback or initialization failure; it does not enforce consecutive original indices. The current call is in `measure_ctor_prefix`, which runs on a synthetic `StartupState` for diagnostics. `sotc_core_start()` still unconditionally returns `SOTC_ERR_ENGINE_NOT_IMPLEMENTED`; this runner behavior does not establish a safe game-start bypass or identify slot 309 as the sole blocker.

The live status-column parse found 0 READY, 10 ACTIVE, 4 BLOCKED, and 15 WAITING_FOR_DEPENDENCY rows. The only active bounded startup item with a newly supported edge is this slot-309 characterization task. Private read-only triage: `local/validation/orch-20261029/BOOT-TRIAGE-E1019/REPORT.md`. No alternate native game-start path or safe slot-309 bypass was demonstrated.

## Result and remaining risks

This resolves the `0xa4de23` callback gate only for the constructor's zero-initialized state, conditional on no intervening write. It does not resolve alias writers/order before either consumer, other indirect paths, the complete constructor/destructor ownership contract, or whether any consumer path runs at runtime. Slot 309 remains unregistered and G1 remains PARTIAL. No game code ran; cold boot, actual game audio, real save/load after relaunch, gameplay Metal, and forbidden-runtime/module absence remain unverified. The current game is not running.

Next exact action: use the pinned bytes and saved disassembly to trace only concrete writers or mutation paths into `0x32f79e8+0x80..+0x98` before the two consumer handoffs; stop if no runtime-order edge can be established, then close constructor/destructor ownership before considering any native wiring.

No source or launcher code changed. Repository checks: configure/build exit 0; CTest 487/487; lift checkers 572 passed, 0 skipped, 0 failed; Trail graph in sync with 10,787 meaning-tier nodes pending; project index 44,245/current; staged-path audit 6/6; all-path audit 3,406; attribution and `git diff --check` passed. The private result summary is `local/validation/orch-20261029/CPU-01-STARTUP-SLOT-309-CALLBACK-RECHECK-E1019/result-publication/attempt-01/CHECKS.md`. These are harness and publication checks only; no game gate advances.
