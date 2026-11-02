# Checkpoint — ConCmd manager `0x210b0` re-termination model made faithful

- Local: 2026-11-01 (America/Chicago); agent: worker (Sonnet 5.5) under the orchestrator
- Task: `CPU-01-CONCMD-MANAGER-210B0-01` (follow-up fix, no new task)
- Gate: `FIXTURE_VERIFIED` + `STATIC_ANALYSIS_VERIFIED` only. `engine_ready=0`; no boot, audio, save,
  Metal or run-time claim.

## Root cause

The committed model ended the constructor with `head.next = &head; head.prev = &head;` (a
"PROBE C: overfit to the emptying result"): it wrote the *result* the 0x20d80-built list happens to
produce instead of the stores the bytes perform. The in-flight edit replaced it with stores that
mirror the instructions. Decoded from the pinned ELF (sha256 `d0723afb…61d3`, image base 0):

| RVA | bytes | instruction | meaning |
|---|---|---|---|
| `0x2118d` | `488d15bcbcdd02` | `lea 0x2dfce50(%rip),%rdx` | `rdx = &head` |
| `0x21198` | `488b4110` | `mov 0x10(%rcx),%rax` | `rax = first->prev` (predecessor link) |
| `0x2119c..0x211a3` | `488d48f8 4885c0 480f44c8` | `lea -8(%rax),%rcx; test; cmove` | `rcx = tail = prev_link - 8` (0 on null) |
| `0x211a7` | `48895108` | `mov %rdx,8(%rcx)` | **store** `[tail+8] = &head` |
| `0x211ab` | `4883c108` | `add $8,%rcx` | `rcx = tail+8 = prev_link` |
| `0x211af` | `48890da2bcdd02` | `mov %rcx,0x2dfce58(%rip)` | **store** `head.prev = prev_link` (0x211af+7+0x2ddbca2 = `0x2dfce58`) |

`0x211a7/0x211af` are stores (opcode `89`, not `8b`), so the checker's *comments* ("mov rdx,[rcx+8]",
"mov rcx,[0x2dfce58]") read as loads, but the checker's byte expectations were already correct. When
`first->prev` is the head link (every list built by `0x20d80`), `tail` is the sentinel `&head - 8`,
so `[tail+8]` is `head.next` and the two stores yield `head = {&head, &head}` (the emptying result).
For any other `first->prev` the stores re-point `tail->next` at the head and set `head.prev` to the
tail link without emptying; the model now implements that general form.

## What was found when the task started

- The in-flight `concmd_manager_210b0.cpp` edit was correct; the checker and `ctest -R concmd` both
  passed against it in this tree (the reported failure did not reproduce). The checker's byte
  expectations were NOT wrong, so no expectation was changed. Only misleading comments were fixed.
- The fixture's "general form" section (6) could not distinguish the missing `tail->next` store
  (the tail's `next` was already `&head`) and so allowed a regression of that one store.

## Files changed (all owned by this task)

- `native/core/concmd_manager_210b0.cpp` — the in-flight faithful stores
  (`store_ptr(tail, +8, &head)`, sentinel case writes `head.next`, `head.prev = prev_link`). The
  `PROBE C` line is gone and must not return.
- `native/tests/test_concmd_manager_210b0.cpp` — new section **6b**: first element's `prev` is a node
  outside the drained chain, tail `next` poisoned, head `prev` stale; asserts `outside.next == &head`
  and `head.prev == &outside`, `head.next` untouched, not emptied.
- `tools/lift/check_concmd_manager_210b0.py` — instruction comments corrected to the real
  mnemonics; new pin that `0x211af` is the `89 0d` store form; native pins: no `PROBE`/`overfit` in the
  source, the two store statements present, section 6b present. (A concurrent edit by the
  orchestrator replaced the hard-coded "63 fragments" in the OK line with `len(FRAGMENTS)` = 62 and
  corrected the header's instruction count to 62; I removed the header's "one Ghidra basic block"
  wording, which contradicts the 4 branches.)
- `native/core/concmd_manager_210b0.hpp` — wording only (see above).

## Commands and results (SOTC_NATIVE_BUILD=local/build/orch-r22)

- `cmake --build local/build/orch-r22 -j 8` → exit 0.
- `ctest --test-dir local/build/orch-r22 -R concmd` → **3/3 passed** (`concmd_slots_298_299_301`,
  `fconcmd_object_slot306`, `concmd_manager_210b0`); with `lookup_21620`, 4/4.
- `python3 tools/lift/check_concmd_manager_210b0.py` → exit 0, 62 fragments, **225 mutants rejected**
  (223 before; the two new pin spans add the `89 0d` bytes).
- Sibling checkers, all exit 0: `check_fconcmd_handlers_slot306.py`, `check_fconcmd_object_slot306.py`,
  `check_lookup_21620.py` (202 mutants), `check_fctor_slots_298_299_301.py`. There is no
  `check_findconcmd*.py` in `tools/lift/`; `check_lookup_21620.py` is the FindConCmd (`0x21620`) checker.

## Source mutation probes (fixture sensitivity), each against a rebuilt `test_concmd_manager_210b0`

| probe | result |
|---|---|
| restore `head.next = head.prev = &head` (PROBE C) | test FAILED (caught) |
| drop the `store_ptr(tail, +8, &head)` | test FAILED (caught; needs 6b) |
| drop `head.prev = prev_link` | test FAILED |
| `head.prev = &head` instead of the tail link | test FAILED |
| drop the sentinel `head.next = &head` | test FAILED |

(Two probes first "passed" only because the mutated file did not compile under -Werror and the old
binary ran; they were redone with a compiling mutant and fail as shown.) Original source restored and
rebuilt afterwards.

## Remaining risks

- Decoding was done by hand from the bytes plus the checker's RIP-relative resolution; no
  disassembler was available for raw binary (`objdump -b binary` unsupported here). The fragment
  byte-for-byte pins are what the checker enforces.
- `0x211a7` pathological alias (`tail+8 == &head.prev`) is modelled in retail store order but untested.
- Not a run-time claim: whether the retail queue is ever populated at this point is unproven.

## Ready-to-paste HANDOFF / TASKS / EVIDENCE text

HANDOFF: "2026-11-01 worker: `0x210b0` re-termination made faithful (stores `[tail+8]=&head`,
`head.prev=prev_link`, sentinel case lands on `head.next`); PROBE C removed. Section 6b added to the
fixture; checker pins the stores and forbids `PROBE`/`overfit`. `ctest -R concmd` 3/3, checker exit 0
(225 mutants), sibling fconcmd/lookup/fctor checkers exit 0. FIXTURE_VERIFIED + STATIC_ANALYSIS_VERIFIED.
Next: orchestrator commits. Attempts: 1, solved."

TASKS: `CPU-01-CONCMD-MANAGER-210B0-01` — keep COMPLETE; note the follow-up fix (faithful tail stores, 225
mutants, fixture 6b).

EVIDENCE (amend E1073): "Re-termination `0x2118d..0x211af`: `lea rdx,&head`; `mov 0x10(%rcx),%rax`;
`lea -8(%rax),%rcx`; `mov %rdx,8(%rcx)` (store); `add $8,%rcx`; `mov %rcx,0x2dfce58(%rip)` (store).
Model stores both; emptying is a consequence when `first->prev == &head`, not an assignment.
Checker 225 mutants; fixture 6b pins both stores. Status FIXTURE_VERIFIED + STATIC_ANALYSIS_VERIFIED."
