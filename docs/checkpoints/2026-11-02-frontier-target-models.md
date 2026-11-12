# Checkpoint: body models for the four withdrawn frontier targets (E1100)

Local date 2026-11-02, worker under the orchestrator. Task: model the real targets of the eight call
sites withdrawn in E1095 (`2026-11-01-frontier-fallback-correction.md`): `0x47860`, `0x31900`,
`0x318f0`, `0x31520`. Status: **FIXTURE_VERIFIED** (models) + **STATIC_ANALYSIS_VERIFIED** (ELF facts);
no gate advanced, no evidence grade moved, no table row added. Attempts: 1 (no failed hypothesis).

## What the bodies are (from ELF bytes, checked by `check_frontier_target_models.py`)

All five are reached through the type-8 RELA'd slot qwords below. Instruction counts, direct calls and
indirect transfers come from the private listing `local/validation/registry-analysis-agent/all.dis`
(bisected, 478-probe pinned); byte layout and SHA-256 from the pinned ELF.

| Function | Size | Insns | Direct calls | Indirect transfers | Memory effects |
|---|---:|---:|---|---|---|
| `0x47860` | 0x80 | 45 | `0x3b570` (5 B `jmp` to PLT `0xf8fa20` = `sceKernelUsleep`) | none | reads `[this+0x70]` (lock pointer); writes `[this+0x28]` (qword); `lock cmpxchg` and `xchg` on `*lock` |
| `0x31900` | 0xb1 | 52 | `sceFiosOpIsDone`, `OpGetError`, `OpGetActualCount`, `OpDelete` (PLT `0xf8f7c0/b0/a0/90`) | `jmp *%rcx` (4-entry table at `0xf96ec8`), `call *0x50(%rax)` | state `+0x54`, op `+0x50`, error `+0x5c`, count `+0x58`, mode `+0x08` (dword 1 / qword 0), handle `+0x10`, cursor `+0x18`; reads flag byte `+0x0c & 8` |
| `0x318f0` | 8 | 3 | none | none | one dword read at `+0x5c`; no frame, no writes |
| `0x31520` | 0x38 | 18 | `sceFiosFHCloseSync` (PLT `0xf8f660`) | `call *0xb0(%rax)` | reads `+0x8` (dword), `+0x10`; writes `+0x10` (dword 0), `+0x8` (qword 0) |
| `0x319c0` (extra) | 0x3a | 20 | `sceFiosOpWait` (PLT `0xf8f7d0`) | `call *0xa8(%rax)`, tail `jmp *0xa0(%rax)` | reads `+0x50` only |

Semantics:

- **`0x47860`**: `void f(this, u64 value)` stores `value` at `this+0x28` under a spin lock. The lock is the
  qword pointed to by `[this+0x70]` (the pointer `0x13d60` writes as `this+0x60`; null means an unlocked plain
  store). 0 = free, all-ones = held; test-then-`lock cmpxchg`; each failed attempt runs `pause`; from attempt
  `0x2711` on it first calls `sceKernelUsleep(1)`; `lfence`, store, `sfence`, `xchg [lock],0`. No return value
  is defined by the body.
- **`0x31900`**: poll the outstanding `SceFiosOp` (`+0x50`, state `+0x54`). State 0 returns true at once. If
  `sceFiosOpIsDone` is false it returns false (state != 0). Otherwise it stores error/count, then by state:
  1 -> on error clears the handle, else mode dword = 1 and (flag bit 3) cursor = `vptr[0x50](this)`;
  2 -> clears the handle and mode qword; 3 and 4 -> cursor += sign-extended count; any other value -> nothing.
  The tail always clears the state, deletes the op and clears `+0x50`. Returns `state == 0`. State 4 is the
  value `0x31870`/`0x318b0` store (`0xFFFFFFFF00000004` at `+0x54`); the meaning of states 1/2/3 is inferred.
- **`0x318f0`**: `return *(i32*)(this+0x5c) == 0;` the op reported no error.
- **`0x31520`**: `vptr[0xb0](this)` (wait for completion), then if `[this+8] == 1`: `sceFiosFHCloseSync(0,
  [this+0x10])`, `[this+0x10] = 0`, `*(u64*)(this+8) = 0`.
- **`0x319c0`**: what `0x31520` reaches through `vptr+0xb0` under the `0x1595408` table: loop `vptr[0xa8]` /
  `sceFiosOpWait(+0x50)`, then tail-call `vptr[0xa0]`. Identical to the loop in `0x31310`.
- `0x316a0` (8 B, `vptr+0x50` under that table) is `mov edi,[rdi+0x10]; jmp sceFiosFHGetSize`; only its image
  fact is checked, it has no model.

## Correction found for `vtable_15953f8_slot18_31310`

The hook names `sub_init` / `positioned_read` / `plain_read` / `mode_one` were wrong. Real mapping:

| Old hook | Real target | New hook |
|---|---|---|
| `sub_init` (direct `call 0x31900`) | `0x31900`, the op poll | `poll_direct` |
| `positioned_read` (`vptr+0xa8`) | `0x31900`, the **same** poll | `poll_virtual` |
| `plain_read` (`vptr+0xa0`) | `0x318f0`, op-succeeded predicate (result discarded) | `succeeded` |
| `mode_one` (`vptr+0x18`) | `0x31520`, wait + `sceFiosFHCloseSync` | `close` |

Result flags `plain_read_called` / `mode_one_called` became `succeeded_called` / `close_called`;
`kVtable15953f8Slot18SubInitRva` became `kVtable15953f8Slot18PollRva` (value unchanged, `0x31900`). Behaviour
is unchanged. Related: `0x31870`/`0x318b0` are *op starters* (they store the op handle returned by
`sceFiosFHReadv`/`Preadv` at `+0x50`, which `0x31900` consumes); the "status" cell in
`vtable_15953f8_slot_a0_a8_fios_reads.hpp` is an op handle (note added, names kept). They are never called
from `0x31310`.

## Which rows could return (integrator's decision; NOT done)

The audit array (`kCorrectedVptrReads`) holds the values. Re-adding would use the read address as
`vtable + slot` (the convention of the kept `0x113c5` row), and every one is **table-level at best**: the
call register is a vptr reload, the strict predicate does not hold, and the E1095 hand argument (a)-(d) is not
machine-checked.

| Site(s) | Read address | As `vtable + slot` | Target | Body model |
|---|---|---|---|---|
| `0x11462` | `0x1594b48` | `0x1594ae8 + 0x60` | `0x47860` | `frontier_47860_locked_store` |
| `0x113d6` | `0x1595bc0` | `0x1595b60 + 0x60` | `0x47860` | same |
| `0x312ce`, `0x3133e` | `0x15954b0` | `0x15953f8 + 0xb8` | `0x31900` | `frontier_31900_fios_poll` |
| `0x312de`, `0x3134e` | `0x15954a8` | `0x15953f8 + 0xb0` | `0x318f0` | `frontier_318f0_fios_succeeded` |
| `0x312f0`, `0x31360` | `0x1595420` | `0x15953f8 + 0x28` | `0x31520` | `frontier_31520_fios_close` |

If rows return, update together: `vtable_resolved_entries.{hpp,cpp}` (+ `kResolvedVtableTarget_*Rva` constants),
its test and checker, `check_evidence_grades.py`, `INDIRECT_FRONTIER.md`, `G1_EVIDENCE.md`, and flip layer 3
and the `*_REINSTATED` probes of `check_frontier_target_models.py` and the "withdrawn" assertions of
`check_frontier_vptr_correction.py`, which both currently assert absence. Rows for `0x31900` / `0x318f0` /
`0x31520` also depend on the dispatch table being `0x15953f8` for the `0x1595408` address point; the
`0x1595b60` / `0x1594ae8` rows for `0x47860` are the same shape as the kept `0x113c5`.

## Files changed (this task only)

New: `native/core/frontier_47860_locked_store.{hpp,cpp}`, `frontier_31900_fios_poll.{hpp,cpp}`,
`frontier_318f0_fios_succeeded.{hpp,cpp}`, `frontier_31520_fios_close.{hpp,cpp}`,
`frontier_319c0_fios_wait.{hpp,cpp}`, `native/tests/test_frontier_target_models.cpp`,
`tools/lift/check_frontier_target_models.py`, this file.
Edited: `native/CMakeLists.txt` (stanza appended: `sotc_frontier_targets`, `test_frontier_target_models`, ctest
`frontier_target_models`), `native/core/vtable_15953f8_slot18_31310.{hpp,cpp}` (hook/flag/constant names and
prose), `native/tests/test_vtable_15953f8_fios_and_4b0.cpp` (renamed hooks), `native/core/vtable_15953f8_slot_a0_a8_fios_reads.hpp`
(comment note only). `docs/entry_map.json` already carries the new constants (another worker regenerated it
after my headers landed; `check_entry_map.py` passes: 1910 constants, 422 headers).
Not touched: `vtable_resolved_entries.*`, `startup_ctors.*`, `ctor_slot_*`, `debug_page_*`, HANDOFF/TASKS/EVIDENCE.

## Model design (fail-visible)

- Every Fios service and every virtual call is a hook. A hook that would be reached and is null reports
  `ok == false`, names it in `missing_hook`, and leaves the object untouched (hooks are checked before the first
  object write: `0x31900`, `0x31520`, `0x319c0`).
- Non-retail bounds, reported not hidden: `0x47860` gives up after `kFrontier47860MaxAttempts` (0x2711 + 64)
  failed attempts with `spin_overflow` (nothing stored, lock untouched); `0x319c0` after 1024 waits. A
  misaligned lock pointer is refused.
- Width fidelity: `0x31900` writes mode as a **dword** 1 but clears it as a **qword**; `0x31520` clears the mode as a
  qword (bytes 8..15); the compare in `0x31520` is a dword. State and op are re-read where the body re-reads them
  (after the virtual size call, after the virtual wait).
- Fences are modelled as seq_cst barriers (stronger than `lfence`/`sfence`); `pause` and `sceKernelUsleep` are
  optional hooks.

## Commands and results

| Command | Exit | Result |
|---|---:|---|
| `cmake -S native -B local/build/orch-r22`; `cmake --build local/build/orch-r22 --target test_frontier_target_models test_vtable_15953f8_fios_and_4b0` | 0 | builds clean under `-Wall -Wextra -Werror` |
| `ctest --test-dir local/build/orch-r22 -R "vtable\|fios\|frontier" -j 8` | 0 | **22/22 passed** (includes `frontier_target_models`, `vtable_15953f8_fios_and_4b0`) |
| `python3 -I tools/lift/check_frontier_target_models.py` | 0 | 5 bodies + `0x3b570`/`0x316a0` thunks, **478 mutation probes all rejected** |
| 15 hand mutations of the models (script in the scratchpad), each rebuilt and run against `test_frontier_target_models` | n/a | **15/15 rejected** (arm set, dword/qword widths, sign extension, error gate, missing-hook gate, mode compare, close attr, threshold `>=`, release, null-lock path, overflow bound, loop polarity); sources restored, test green after |
| `check_fios_slots_and_4b0`, `check_frontier_vptr_correction` (92 probes), `check_vtable_resolved_entries` (83/32), `check_evidence_grades`, `check_indirect_frontier`, `check_vtable_frontier2`, `check_alloc_site_context`, `verify_frontier_sweep` (24/24), `store_to_load_forwarding`, `check_entry_map` | 0 each | all OK, run one at a time |
| all 20 `tools/lift/check_vtable*.py`, one at a time | 0 each | all OK |
| `python3 -I tools/lift/check_g1_evidence_pack.py` | 1 | **FAILED, not from this change**: `docs/G1_EVIDENCE.md` is stale because the startup lane advanced (`registered_rows` 460 -> 465, remaining 113 -> 108). The frontier block (83 / 32 / 20, 41 + 42) is unchanged. Integrator regenerates after the peer lands |
| `python3 -I scripts/audit_publish_paths.py --all` | 0 | `PUBLISH PATH AUDIT OK (4110 path(s))` |

Not run: full `ctest` (543+), `run_all_checkers.sh`, `check_commit_attribution.py`, `project_index.py`
(integrator steps; the tree is shared with the startup lane). The 22/22 is a filtered ctest, not the full suite.

## Observations versus inference

- Observed: bytes, sizes, SHA-256, FDE ranges, call targets, PLT names, jump-table entries, slot qwords and
  single type-8 RELA, instruction counts, the `0x3b570 -> sceKernelUsleep` thunk and `0x316a0 -> sceFiosFHGetSize`.
- Inferred (not in the bytes): names of the object fields ("op", "state", "cursor", "handle", "mode"); the
  meaning of states 1/2/3 (open / close / transfer) from what each arm does and from state 4 = read as stored by
  `0x31870`/`0x318b0`; that `+0x8 == 1` means "handle open"; that `vptr+0x50` is `sceFiosFHGetSize` for the
  `0x1595408` table only (a derived object's table may differ, so it stays a hook).
- Unchanged assumption: the callers' vptr is `TABLE + 0x10` at the call (E1095 hand argument).

## Risks

- Models are fixtures against stated semantics; they are not bound into a runtime. No claim about Fios behaviour of
  the real game, loading, or boot follows.
- `0x47860` is only tested single-threaded (the pause hook releases the lock); the atomics use GCC builtins and
  are not exercised under real contention. Return value of the retail body is undefined and not modelled.
- Hook arity differs from the `0x31310` hooks (the new models take a `user` pointer and typed cells); a host binds
  them with thunks (the composition test `test_31310_composition` does this for all five).
- `check_frontier_target_models.py` layer 3 asserts the four targets stay out of the table; it must change in the
  same commit that reinstates any row.

## Next exact action

Integrator: paste the HANDOFF/TASKS/EVIDENCE text from the worker report, regenerate `G1_EVIDENCE.md` once the
startup peer lands, then run the standard sweep. Decision needed: reinstate the five table-level rows above (update
list in "Which rows could return").
