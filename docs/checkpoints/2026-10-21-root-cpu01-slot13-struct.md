# Checkpoint: CPU-01-SLOT13-STRUCT-01 — slot 13 (`0xf058f0`) publishes a singleton and a heap mutex, then tail-calls `__cxa_atexit`

- Time / timezone: 2026-10-21 00:05 CDT (America/Chicago), macOS 27.0.1 arm64
- Agent and integrator: orchestrator session (`deepseek-v4.1-flash-1017`), integrator-owned record.
- Task / gate / status: CPU-01-SLOT13-STRUCT-01. **STATIC_ANALYSIS_VERIFIED**; **NOT LIFTED**, not
  registered in `startup_ctors`. G1 still not passed.
- Prerequisite evidence / input and source hashes: private `local/re/eboot.elf` sha256
  `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`; constructor table
  `0x2dd70e0` (573 entries, sha256 `5917cb85…3ab28`); `tools/lift/verify_slot12_13.py`.
- Owned / changed files: `tools/lift/analyze_slot_struct.py` (shared with CPU-01-SLOT12-STRUCT-01),
  `tools/lift/verify_slot12_13.py`, this checkpoint.
- Hypothesis and expected result: slot 13 is the same descriptor+mutex+atexit family as slots
  10–12, but obtains its object from a singleton creator instead of constructing it on the stack.
  Expected: a short body that stores two globals, allocates and initializes a 16-byte mutex, and
  exits through an `__cxa_atexit` tail call (no `ret` of its own).
- Exact command, CWD and nonsecret environment: CWD `/Volumes/Extra/SoC Mac Native`
  - `python3 -I tools/lift/analyze_slot_struct.py 0xf058f0 --label slot-13`
  - `python3 -I tools/lift/verify_slot12_13.py` (exit 0; also re-checks slot 12)
- Exit / observed result:
  - Body `0xf058f0…0xf0593f`, **79 bytes**, sha256
    `d021c860c421a25ee64f18d6007daa897d963d4a9be62d51928061b0eee6675b`,
    21 reachable instructions, **0** returns, **1** tail call.
  - Direct calls: `0xf030e0` (singleton creator), `0xb30a10` (returns `0x12d2d1c`, whose dword is
    `0x10`), `malloc` (`0xf90210`), `0xb30950` (pthread mutex init with type+protocol attributes).
  - Tail call: `jmp 0xf8f390`, the `__cxa_atexit` thunk (GOT `0x15fffb8`, NID `tsvEmnenz48`), with
    `rdi = 0xe44a10`, `rsi = 0x3355480` (address of the global holding the mutex pointer),
    `rdx = 0x1604218`.
  - Published `.bss` globals: `0x3355470` (singleton pointer), `0x3355480` (heap-mutex pointer).
  - `0xf030e0` (276 bytes) itself calls `0xe1eda0` and `0xe1ede0` — the same ctor/helper pair used
    by slots 10, 11 and 12.
- Observation versus inference / coverage limitations:
  - Observation: body, calls, tail-call target, globals and mutex size, all from pinned bytes.
  - Inference: `0xf030e0` is a singleton getter/creator (it constructs via `0xe1eda0`/`0xe1ede0`
    and returns a pointer) — its caching/guard semantics were not traced.
  - Not verified: the class identity behind `0xf030e0`; consumers of `0x3355470`/`0x3355480`; why
    the destructor is shared with slot 12; runtime behaviour; ABI/ordering.
- Cumulative failed tries and distinct hypotheses: 0 failed tries. The first walk followed the
  `__cxa_atexit` tail call into the import thunk and reported a nonsensical 563,878-byte span; the
  tool now stops at tail calls through `ff 25` thunks. The wrong span is not quoted anywhere.
- Remaining risks / blockers: slot 11 is still the first unresolved slot, so this analysis changes
  no runtime behaviour. Registration without slot 11 would violate the strict-order contract.
- Next exact action: resolve the `0xf030e0` singleton contract (guard cell + allocation size) and
  the class behind it, then decide whether slots 12/13 can share one native "descriptor + heap
  mutex + atexit" helper type.
- HANDOFF.md / TASKS / EVIDENCE / INSTALL_LOG / indexes updated: by the integrator in this session.
