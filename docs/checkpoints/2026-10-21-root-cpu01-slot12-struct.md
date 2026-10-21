# Checkpoint: CPU-01-SLOT12-STRUCT-01 — slot 12 (`0xf06340`) is a descriptor+heap-mutex+atexit initializer

- Time / timezone: 2026-10-21 00:00 CDT (America/Chicago), macOS 27.0.1 arm64
- Agent and integrator: orchestrator session (`deepseek-v4.1-flash-1017`), integrator-owned record.
- Task / gate / status: CPU-01-SLOT12-STRUCT-01. **STATIC_ANALYSIS_VERIFIED** for the structural
  facts below; **NOT LIFTED**, not registered in `startup_ctors`. G1 still not passed.
- Prerequisite evidence / input and source hashes: private `local/re/eboot.elf`
  sha256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3` (re-asserted by the
  verifier); constructor table `0x2dd70e0`, 573 entries, sha256 `5917cb85…3ab28` (checker-pinned);
  slot-10/slot-11 dossiers for the same descriptor family.
- Owned / changed files: `tools/lift/analyze_slot_struct.py` (new reusable reachability dumper),
  `tools/lift/verify_slot12_13.py` (new replay verifier, covers slot 12 and 13), this checkpoint.
- Hypothesis and expected result: slot 12 follows the slot-10/11 pattern — publish a descriptor
  pointer into a global, create a once-guarded recursive+priority-inherit mutex, register cleanup.
  Expected: a straight-line 1-return body with `__cxa_atexit` and the `0xb30950` mutex initializer.
- Exact command, CWD and nonsecret environment: CWD `/Volumes/Extra/SoC Mac Native`
  - `python3 -I tools/lift/analyze_slot_struct.py 0xf06340 --label slot-12`
  - `python3 -I tools/lift/verify_slot12_13.py` (exit 0)
  Read-only ELF decode plus one `llvm-objdump` pass per block start through
  `scripts/disasm_range.sh`; nothing executed, no Ghidra/REA session.
- Exit / observed result (all re-derived by the verifier):
  - Body `0xf06340…0xf0640d`, **205 bytes**, sha256
    `a16f79f44e9fa082e8e7a25d300210dd6538aa4f5f1341d66a6e30769b3dcb53`,
    49 reachable instructions, **1** return, **0** indirect transfers.
  - Direct calls in order: `0xe1eda0` `0xe1ede0` `0xb30a10` `malloc`(`0xf90210`, GOT
    `0x16006f8`) `0xb30950` `__cxa_atexit`(`0xf8f390`, GOT `0x15fffb8`) `__stack_chk_fail`
    (`0xf8f3a0`, GOT `0x15fffc0`).
  - RIP-relative references: `0x15fe518` (pointer to the stack guard), `0x15f9ef0` (vtable base;
    `+0x10` is published), `.bss` globals `0x3355f88` and `0x3355f98`, destructor `0xe44a10`,
    atexit dso word `0x1604218`.
  - `0xb30a10` is `lea rax,[rip+…]; ret` returning `0x12d2d1c`; the dword there is **`0x10`**, and
    that value is passed to `malloc`.
  - `0xb30950` (112 bytes) calls `pthread_mutexattr_init`, `pthread_mutexattr_settype`,
    `pthread_mutexattr_setprotocol`, `pthread_mutex_init`, `pthread_mutexattr_destroy` — i.e. the
    same recursive + priority-inherit mutex contract already recovered natively as
    `SRV-01-RECURSIVE-MUTEX-01`.
  - `0xe44a10` (31 bytes) calls the `pthread_mutex_destroy` stub `0xb309d0` and tail-calls `free`.
- Observation versus inference / coverage limitations:
  - Observation: every byte range, call, global and contract above, re-derived from the pinned ELF.
  - Inference: the published `0x15f9ef0+0x10` address point is a vtable for the stack-local object
    constructed at `rsp+8` by `0xe1eda0`/`0xe1ede0`; the class identity is **not** established.
  - Not verified: the class behind vtable `0x15f9ef0`; the consumer of `0x3355f88`/`0x3355f98`;
    whether `0xf030e0`-style singleton creation applies here (it does for slot 13, not slot 12);
    ordering/once-guard interaction with slot 11; runtime behaviour. No native function written.
- Cumulative failed tries and distinct hypotheses: 0 failed tries. One tool defect was found and
  fixed during this task: a linear-sweep body walk ran into the next function (reported 1229
  bytes) because a noreturn `__stack_chk_fail` block fell through into padding; the reachability
  walk plus a noreturn-call list corrected it. Body SHA in an earlier scratch run (1229-byte span)
  must not be quoted.
- Remaining risks / blockers: slot 11 (`0xf09160`) remains the first unresolved slot, so slots 12
  and 13 are unreachable at runtime regardless of this analysis. Nothing here proves the slot-11
  "reflection subsystem" dependency, and it does not release that blocker.
- Next exact action: recover the class behind vtable `0x15f9ef0` and the consumers of the two
  `.bss` globals, or (bounded alternative) apply the same reachability dossier to slots 14–19.
- HANDOFF.md / TASKS / EVIDENCE / INSTALL_LOG / indexes updated: by the integrator in this session.

## Note on adjacency

`0xf06340` ends at `0xf0640d`; the next function starts at `0xf06410`, which the slot-11 dossier
calls the class constructor of `ClothingPhysicalMeshParameters`. This is consistent, not a
contradiction: `0xf06340` is a *static registration thunk* that constructs a transient descriptor,
publishes its address point, allocates a 16-byte heap mutex, initializes it and registers the
destroy+free destructor. It never calls `0xf06410`.
