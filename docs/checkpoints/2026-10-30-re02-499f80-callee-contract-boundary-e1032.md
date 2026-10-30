# E1032 — `0x499f80` callee-contract boundary

- Date: 2026-10-30 00:11 CDT
- Agent: `/root` (integrator)
- Task: `RE-02-APPMAIN-499F80-BODY-LIFTABILITY-01`
- Status: `PARTIAL / STATIC_ANALYSIS_VERIFIED` for wrapper mapping only. No code or runtime change; 0 failed semantic hypotheses.

## Current evidence

At the clean published baseline `190395545d4a08ebd6704d22939ed141f2746092`, the E1031 Ghidra dossier (`ev_23dcf45d3ce46417d94d92bbf0a8492625dd9a679c07185e6b3e14a0780ba23a`) and pinned disassembly identify the direct calls in body `0x499f80`.

Four calls have matching native wrapper contracts in `native/core/alloc_wrappers_17070.{hpp,cpp}`:

- `0x17070`: tag 1, alignment 8, flags 1; called for the `0x50` object.
- `0x17090`: tag 1, alignment 8, flags 2; called twice for `0xa00` buffers.
- `0x170c0`: deallocate with tag 2; called for old buffer pointers.
- `0x170b0`: deallocate with tag 1; called for the old object.

These wrapper functions still delegate to host hooks; the pinned binary dispatchers `0x12820` and `0x12af0` have not been replaced by a complete engine allocator.

The unresolved lifecycle is the mutex field. In saved `all.dis`, `0x3b310` initializes mutex attributes, sets type 2, calls the mutex init import with name `CritSec`, and destroys the attributes. `0x3b380` jumps through `0xf8f8c0`, which the pinned import manifest maps to `scePthreadMutexDestroy` at GOT slot `0x1600250`, NID `2Of0f+3mhhE`. Existing `Slot414Hooks` only exposes an init hook for an 8-byte mutex handle; it has no paired destruction contract. Existing `RecursiveMutex` embeds the host `pthread_mutex_t`, sets `PTHREAD_PRIO_INHERIT`, and checks host error returns, so its layout and behavior are not an exact match.

The immutable `reference/ShadowOfTheColossus-PC` checkout is at `70bb43859cd7ff181a6d409540b757866184d60d`; its `runtime_mutex.c` maps type 2 to `PTHREAD_MUTEX_RECURSIVE`. This is an adapter implementation, not authoritative proof of the PS4 ABI. Trail and Graft have no indexed `reference/shadPS4/` source scope; no Sony SDK or other executable/runtime was installed.

## Product status and next action

`sotc_core_start()` still unconditionally reports `SOTC_ERR_ENGINE_NOT_IMPLEMENTED`; `engine_ready` remains false. No cold boot, real audio, save/reload, or game-driven Metal frame is established. G1 remains PARTIAL and G5 NOT RUN.

Next exact action if resumed: establish the native host representation for the `0x3b310`/`0x3b380` mutex lifecycle, including type 2, the 8-byte handle, initialization/destruction, and failure semantics. Then claim a bounded `0x499f80` lift using the existing allocation wrappers; keep `engine_ready` false until a real engine startup path is implemented.

Private report: `local/validation/orch-20261030/RE-02-499F80-CALLEE-CONTRACTS-E1032/REPORT.md`, SHA-256 `6b84bce0c404855007d893d1c0fbea96543d6103faeae8f151393568db6207b1`.

## Verification and publication

- `cmake -S native -B local/build/orch-r22`: exit 0.
- `cmake --build local/build/orch-r22 -j 12`: exit 0.
- `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure`: 487/487 passed.
- `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh`: 572 passed, 0 skipped, 0 failed.
- `trail build --check`: wiring graph in sync; 10,787 meaning nodes remain pending.
- `python3 scripts/project_index.py` and `python3 scripts/project_index.py --check`: 44,264/current.
- Staged publish audit: 6/6; all-path audit: 3,417; attribution audit and staged diff check passed.
- Commit and push results are visible in the Git history/remote ref and are reported in the final handoff.
- Logs: `local/validation/orch-20261030/STARTUP-STOP-E1032-PUBLICATION/`. No source files changed.
