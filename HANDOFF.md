# SoC Mac Native — current handoff

Updated: 2026-10-20 23:55 CDT (America/Chicago). Integrator: orchestrator session
(`deepseek-v4.1-flash-1017`). Previous transfer record:
[docs/checkpoints/2026-10-20-root-next-agent-handoff.md](docs/checkpoints/2026-10-20-root-next-agent-handoff.md);
older history: `docs/checkpoints/2026-10-20-handoff-archive-through-1f7c8e8.md`.

**Project remains incomplete.** G0 PARTIAL; G1–G6 NOT RUN. No native game executable, boot,
authentic game audio, real progress save/relaunch/load, or Metal gameplay is demonstrated.

## Start here

Read [INDEX](docs/INDEX.md), [ROADMAP](ROADMAP.md), [TASKS](docs/TASKS.md), [EVIDENCE](docs/EVIDENCE.md),
then the latest checkpoints:
- [Slot-10 descriptor consumer + native slot 10](docs/checkpoints/2026-10-20-root-cpu01-ctor10-descriptor-consumer-complete.md)
- [Leaf ctor batches 25–29](docs/checkpoints/2026-10-20-root-cpu01-leaf-ctor-batches-25-29-complete.md)

## Current verified state

- Native tests: **252/252 pass** (`cmake --build local/build/native -j 8 && ctest --test-dir
  local/build/native --output-on-failure`), ASan+UBSan on, Mach-O arm64.
- Constructor-table checker: `python3 -I tools/lift/check_ctor_slots.py` verifies **104**
  RVA/index pairs; slots 0–10 are recovered; slot 11 is the explicit blocker.
- Launcher: 19 Swift tests pass (`swift test --package-path launcher --scratch-path
  local/build/launcher`). Python tool tests pass except `test_project_tools.py`, which needs to be
  run from `scripts/` (pre-existing `-I` import behavior, not a regression).
- RE-01-PERSISTENT-01: the saved Ghidra baseline reopens read-only `-noanalysis` (38,845
  functions, 9 blocks). See `local/checkpoints/root-20261020-reopen-baseline.txt`.
- Slot 10 / RVA `0xf09e60` now has demonstrated semantics: it publishes the `ClothingPreviewParam`
  vtable address point `0x15fb7c0` into global `0x3356678`, constructs/destroys a stack-local
  instance, creates a recursive + priority-inherit mutex, and registers its cleanup. Consumer
  `FUN_00ed5a90` installs the published pointer as an object vptr; factory `FUN_00ed59a0`
  allocates `0xb0` bytes at alignment 8.
- Native counterpart: `native/core/clothing_preview_param.{hpp,cpp}` plus a slot-10 callback and
  `StartupState`-owned `RecursiveMutex`. `run_constructors` now propagates `callback_failed`.

## Blocker

Slot 11 / RVA `0x00f09160` is the next unresolved original constructor. The sparse runner must
keep the explicit null slot 11 before later leaves. Slots 12/13 are `0xf06340`/`0xf058f0`.
Do not claim engine startup or G1 from slot-10 registration.

## Settled decisions (do not re-ask)

- `StartupState` RAII ownership for the slot-10 mutex — implemented.
- Testable `LauncherCore` lifecycle helper and a system alert for folder-import errors — keep.
- Offline analysis of the local `eboot.bin` is allowed; no original/Sony executable may ship or run.

## Next exact work

1. Claim bounded slot-11 recovery (`0x00f09160`) using the saved read-only Ghidra baseline and
   `tools/lift/check_ctor_slots.py`; or continue AUD-02/SAV-01 static tracks.
2. Keep `0xd71f60` NEEDS_PRINCIPAL (3 failed tries) and do not retry without principal
   reassessment.
3. Any new native module needs a README row, a test, and CMake registration.
4. Before publishing: `python3 scripts/project_index.py`, `trail build --check`,
   `python3 -I scripts/audit_publish_paths.py --all`, then commit allowlisted paths and
   `git push origin main`.

## Parallel-agent state

- A leaf-ctor subagent completed batches 25–29 (27/27, all manifest entries now disposed).
- An AUD-02-WAITLOOP-01 subagent is running; treat its checkpoint as pending until it reports.
- The persistent Ghidra project `local/ghidra/persistent-20261020` must never be re-imported or
  overwritten while in use; read-only `-noanalysis` reopening is the supported path.

## Private evidence pointers

- Consumer packet: `local/re/evidence/cpu01-ctor10-descriptor-consumer-01/`
  (`report.md`, `verify.py`, `verification.json` 45/45, `ghidra-*.txt`, `SHA256SUMS`).
- Reopen log: `local/checkpoints/root-20261020-reopen-console.log`.
