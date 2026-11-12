# Checkpoint 2026-11-02: helper callee models, part 4 (CPU-01-HELPER-CALLEE-MODELS-04, worker helper-e)

Agent: helper-e worker (Claude Sonnet 5.5). Workers do not edit HANDOFF.md / docs/TASKS.md / docs/EVIDENCE.md; the integrator pastes the sections below.
Nothing was committed. Build dir `local/build/helper-e`. Attempt count: 1, no failed tries (no escalation). Labels: STATIC_ANALYSIS_VERIFIED +
FIXTURE_VERIFIED. No original RVA was executed; no boot, audio or save claim. `native/core/startup_ctors.*`, `ctor_slot_*`, count pins, survey / G1 /
entry_map / FOLDERS were not touched; no slot was registered. Only new files plus stanzas appended (`>>`) to `native/CMakeLists.txt`.

## Premise check (callees named for the remaining slots, re-measured from live files)

| address | what it is | status |
|---|---|---|
| 0x1580a0 | 192 B perspective-matrix builder (slot 408, call 0x5c8082, out cell 0x2fda41c) | **newly modelled** (`tanf` is an explicit host hook) |
| 0x274730 | 661 B bool console-variable ctor + the 143-table bool functor family (copy 0xc2a0, clone 0xc2c0, move 0xc2e0, destroy 0xc2f0, class 0x1594280); 14 direct callers | **newly modelled** |
| 0x49ca60 / 0x49cc60 | 352 B dword ranged console-variable ctors (slot 451, sites 0x49dbad / 0x49df7f); same body as the native float twin 0xc300 except six rip/call operands | **newly modelled** |
| 0x140b00 / 0x25ab0 | 322 B unbounded-int ctor twins of the native 0x3a980 (slot 534: 3 sites, slot 566: 1 site), each with a private seed class | **newly modelled** |
| 0x1f220 | 27 B ring-header leaf (slot 473, call 0x3571ae, object 0x2ef2ba8; nine direct callers in the image) | **newly modelled** |
| 0x249bf0 .. 0x3b9450 (48 getters, slot 498) | 48 x `mov eax, imm32 ; ret` (6 B) in 24 adjacent pairs; the slot builds 24 12-byte records at 0x2eec3b0 from them | **newly modelled** (getters + the 24 record stores) |
| 0x52a990, 0x17070, 0xf730, 0x27050, 0x323d0, 0x261b50 / 0x261f10, 0xc580, 0x3b310, 0xf2f0, 0x3a980, 0x257a0, 0x6aa00, 0x4c8f40, 0xa81700 | named by the survey for 408 / 427 / 446 / 454 / 497 / 501 / 503 / 511 | **already native** (earlier helper rounds); their slots are blocked slot-side, not callee-side |
| 0x3be2d0 + 0x3c0840 | 192 B RB-tree find-or-insert (strcmp import 0xf8f500) + 0x48 B node alloc via 0x17070, atomic SharedStr copy, rebalance 0x3c31b0 (692 B) / 0x3c3470 (662 B) / 0x3c3710 (544 B), error tail 0x3c3930 | **refused** (see below), unchanged from part 3 |
| 0xa38960 | 436 B blob parser (slot 495); calls unknown 0xa39eb0 / 0xa39fd0 and a memcpy-like 0xf8f470 | **refused** (see below) |
| 0x53fc0 (2974 B) + 17 x 397 B wrappers 0x6abb0 .. 0x6e4d0 | slot 554 helper family; the 397 B body (93 insns) is a new wrapper that builds a local arg block, calls a ranged-float base ctor and re-tags | **not modelled** (see below) |
| 0x6e800 / 0x6e9f0, 0x6ebe0 / 0x6ede0 | slot 554 twins: 323 B bodies equal to 0x3a980 and 352 B bodies equal to 0xc300 / 0xc580 (normalised, only branch targets differ) | **identified, not modelled** (see below) |

## What was added

| model | files | what it covers |
|---|---|---|
| 0x1580a0 | `native/core/helper_1580a0_perspective.{hpp,cpp}`, `native/tests/test_helper_1580a0.cpp`, `tools/lift/check_helper_1580a0.py` | perspective matrix, tanf as a hook; slot 408's call arguments (fov 0x3f490fdb, aspect 0x3fe38e39, near 0x3f000000, far 0x45834000, flag 1) pinned |
| 0x274730 | `native/core/helper_274730_bool_var.{hpp,cpp}`, `native/core/helper_bool_functor_tags.{hpp,cpp}`, `native/tests/test_helper_274730.cpp`, `tools/lift/check_helper_274730.py` | bool ctor; 143-tag family recomputed from relocations and tied to the native array both ways; 14 callers; owner bodies of slots 493 / 473 / 427 / 422 and 0x523bd0 hash-pinned; slot 493's first site pinned to atexit |
| 0x49ca60 / 0x49cc60 | `native/core/helper_49ca60_ranged_dword_var.{hpp,cpp}`, `native/tests/test_helper_49ca60.cpp`, `tools/lift/check_helper_49ca60.py` | `helper_ranged_dword_var_49ca60(var, variant, ...)`; A: min 0, max 10, value 1; B: min 0, max 4, value 0; refuses (nothing written) when the argument functor is not the ctor's own class or min/max is null |
| 0x140b00 / 0x25ab0 | `native/core/helper_140b00_unbounded_int_twin.{hpp,cpp}`, `native/tests/test_helper_140b00.cpp`, `tools/lift/check_helper_140b00.py` | reuses `helper_3a980_unbounded_int` and re-tags mid vptr + private seed class (0x15983c8 / 0x1595100) |
| 0x1f220 | `native/core/helper_1f220_ring_header.{hpp,cpp}`, `native/tests/test_helper_1f220.cpp`, `tools/lift/check_helper_1f220.py` | `ring_header_init_1f220(obj)`: qwords +0x00, +0x08, +0x20 = obj + 0x28, +0x10 = obj + 0x2628; nothing else written |
| slot 498 getters | `native/core/helper_498_member_hashes.{hpp,cpp}`, `native/tests/test_helper_498.cpp`, `tools/lift/check_helper_498.py` | `member_hash_getter_498(rva, value)` (48 getters), `fill_member_hash_cells_498` (24 records `{lo, hi, size}`, stride 12, 0x120 B at 0x2eec3b0) |
| pins | `tools/lift/gen_helper_pins_e.py`, generated `tools/lift/helper_callee_pins_e.py` | per-instruction pins from objdump (regenerate with `--write` only after re-reading the disassembly) |
| CMake | `native/CMakeLists.txt` (appended stanzas) | six `add_executable` + `add_test` pairs: `helper_1580a0`, `helper_274730`, `helper_49ca60`, `helper_140b00`, `helper_1f220`, `helper_498` |
| scratch | none kept in the repo | |

## Verification (this worker, 2026-11-02)

```
cmake -S native -B local/build/helper-e && cmake --build local/build/helper-e -j 12       # whole tree builds, -Werror targets clean
ctest --test-dir local/build/helper-e -j 12                                                # 633 / 633 passed (17 helper_* tests incl. the six new)
python3 -I tools/lift/check_helper_{1580a0,274730,49ca60,140b00,1f220,498}.py              # all exit 0, every mutant rejected (counts below)
python3 -I tools/lift/check_helper_<x>.py --tamper elf|native                              # exit 1 for every checker and mode
python3 -I scripts/audit_publish_paths.py --all                                            # PUBLISH PATH AUDIT OK (4110 paths)
```

| checker | ELF mutants | spec mutants | native mutants |
|---|---|---|---|
| check_helper_1580a0 | 150 | 17 | 39 |
| check_helper_274730 | 1018 | 38 | 186 |
| check_helper_49ca60 | 838 | 60 | 36 |
| check_helper_140b00 | 521 | 40 | 29 |
| check_helper_1f220 | 31 | 10 | 16 |
| check_helper_498 | 772 | 76 | 38 |

`tools/lift/run_all_checkers.sh` and `python3 scripts/project_index.py` were NOT run by this worker (they regenerate shared indexes); the integrator should run them.

## Which slots each model unblocks (callee side only; every slot still needs its own wiring)

- 408: 0x1580a0 was the cliff; the remaining callee 0xf2f0 is native.
- 422 / 427 / 473 / 493: 0x274730 and its bool functor family. 427 still needs 0x52a990 / 0xa81700 / 0x257a0 / 0x3a980 (native); 473 also needs 0x1f220 (now native). Slot 422's separate slot-side assertion failure (part 3) is unchanged.
- 451: 0x49ca60 and 0x49cc60.
- 534 and 566: 0x140b00 and 0x25ab0.
- 498: the 48 getters. Remaining: 0xc580 and 0x3a980 (native), the menu toggle ctors and slot-side wiring.
- 454, 497, 501, 503, 511: no unknown callee left (all named callees were already native: 0x17070, 0x3a980, 0xa81700, 0xf730, 0x257a0, 0x26800, 0xf2f0, 0x6aa00); they are slot-side or allocator-contract wiring.
- 446: still blocked by 0x3be2d0 / 0x3c0840 (refused).
- 495: still blocked by 0xa38960 (refused).
- 554: still blocked by 0x53fc0, the 397 B wrapper family and four unmodelled twins.

## Refusals and deferrals (with evidence; none is a failed try)

- **0x3be2d0 / 0x3c0840** (slot 446): a find-or-insert on a `map<const char*, T*>`. Faithful modelling needs the allocation contract (0x17070, 0x48 B node), the strcmp import (0xf8f500) as a hook, the atomic refcounted SharedStr copy, and the full red-black insert + rebalance (0x3c31b0 692 B, 0x3c3470 662 B, 0x3c3710 544 B, error tail 0x3c3930 / 0xf8fc10): about 2.5 KB of tree code not yet analysed per instruction. Refused rather than faked. An existing native RB insert (`owner_ctor_4a690`'s `0x4b3f0`) is a different tree with different node layout; reuse was not proven.
- **0xa38960** (slot 495, 436 B): parses a blob via two unknown callees (0xa39eb0 / 0xa39fd0) and 0xf8f470 (memcpy-like). New unknown mechanism; refused.
- **Slot 554 family**: (a) 0x53fc0 is 2974 B; (b) the 397 B body shared by 0x6abb0, 0x6af60, 0x6b2f0, 0x6b680, 0x6ba10, 0x6bda0, 0x6c130, 0x6c4c0, 0x6c850, 0x6cbe0, 0x6cf70, 0x6d300, 0x6d690, 0x6da20, 0x6ddb0, 0x6e140, 0x6e4d0 is identical across all 17 (normalised) and is NOT a twin of 0xc300 (93 vs 90 insns, local argument block, a call to a base ctor, then re-tag of the vptr and a store at +0x68); (c) 0x6e800 / 0x6e9f0 are equal to 0x3a980 (mid vptr 0x1596678 / 0x15966f0, class 0x1597348 / 0x15973e0) and 0x6ebe0 / 0x6ede0 are equal to 0xc300 / 0xc580 (mid 0x1596768 / 0x15967e0, class 0x1597478 / 0x1597510). The twins would follow the helper_140b00 / helper_49ca60 pattern, but modelling them without 0x53fc0 and the 397 B family unblocks nothing, so they are deferred.
- **0x274730 residual**: the model proves the constructor and the family; slot-side registration, value cell and atexit stay with the slot (as for the other helpers).
- **Not established by any of the above**: the meaning of the 0x1f220 object, of the 498 hash records, or of the bool/dword variable names (only layout and stores are proven); the aliasing and allocation-failure behaviour of the original (the models fail visibly, the original would fault).

## Paste-ready text

### HANDOFF.md (checkpoint)

2026-11-02 helper-e worker, task CPU-01-HELPER-CALLEE-MODELS-04, attempt 1 (no failed tries). Newly modelled and ELF-pinned: 0x1580a0 (slot 408 perspective builder), 0x274730 + the 143-table bool functor family (slots 422 / 427 / 473 / 493), 0x49ca60 / 0x49cc60 (slot 451, same body as 0xc300), 0x140b00 / 0x25ab0 (3a980 twins for slots 534 / 566), 0x1f220 (slot 473 ring header), and the 48 six byte getters + 24 records of slot 498. Targets `helper_1580a0`, `helper_274730`, `helper_49ca60`, `helper_140b00`, `helper_1f220`, `helper_498`; whole tree 633/633 ctests; six checkers exit 0 with all mutants rejected, `--tamper elf|native` exit 1; path audit OK (4110 paths). Refused: 0x3be2d0 / 0x3c0840 (RB insert), 0xa38960 (blob parser). Deferred: slot 554 family (0x53fc0, 17 x 397 B wrappers, four twins). Not run: `run_all_checkers.sh`, `project_index.py`. Labels: STATIC_ANALYSIS_VERIFIED, FIXTURE_VERIFIED. Next exact action: wire slots 408 / 451 / 534 / 566 / 473 / 493 / 498 to the new models (each registration, atexit and value cell is slot-side).

### docs/TASKS.md

`CPU-01-HELPER-CALLEE-MODELS-04` | helper-e | DONE for 0x1580a0 / 0x274730 / 0x49ca60 / 0x49cc60 / 0x140b00 / 0x25ab0 / 0x1f220 / slot-498 getters | open: 0x3be2d0 + 0x3c0840 (refused, RB insert), 0xa38960 (refused), slot 554 family (deferred) | attempts 1 | evidence E1250..E1257.

### docs/EVIDENCE.md

| E1250 | 2026-11-02 | 0x1580a0 (192 B) perspective builder, tanf NID-proven, seven direct callers re-derived; slot 408 call 0x5c8082 arguments pinned (out 0x2fda41c, fov 0x3f490fdb, aspect 0x3fe38e39, near 0x3f000000, far 0x45834000, flag 1). 150 + 17 + 39 mutants rejected. STATIC_ANALYSIS_VERIFIED + FIXTURE_VERIFIED. |
| E1251 | 2026-11-02 | 0x274730 (661 B) bool console-variable ctor; bool functor family of 143 tables recomputed from RELATIVE relocations (slots 0xc2a0 / 0xc2c0 / 0xc2e0 / 0xc2f0, class 0x1594280) and tied to the native array both ways; 14 direct callers; owner bodies of 493 / 473 / 427 / 422 and 0x523bd0 hash-pinned; slot 493's first site (0x277723, 0xaf B) pinned through atexit. 1018 + 38 + 186 mutants rejected. |
| E1252 | 2026-11-02 | 0x49ca60 / 0x49cc60 (352 B) are the same body as the native 0xc300 except six rip/call operands; A: table 0x15bfb20, class 0x15bfce8, min 0 max 10 value 1; B: table 0x15bfb98, class 0x15bfd80, min 0 max 4 value 0; slot 451 sites 0x49dbad / 0x49df7f pinned from the vptr lea to __cxa_atexit. 838 + 60 + 36 mutants rejected. |
| E1253 | 2026-11-02 | 0x140b00 (slot 534: sites 0x141bd1 / 0x141cd6 / 0x142406) and 0x25ab0 (slot 566: site 0x26181) are 322 B twins of 0x3a980 (differ in five rel32 windows); mid tables 0x1598240 / 0x1594fd0; private seed classes 0x15983c8 / 0x1595100 tied to the int functor bodies. 521 + 40 + 29 mutants rejected. |
| E1254 | 2026-11-02 | 0x1f220 (27 B, sha e1020c61...a7a1cc): four qword stores (+0x00 / +0x08 / +0x20 = obj + 0x28, +0x10 = obj + 0x2628); nine direct callers re-derived; slot 473 (0x355a00) call 0x3571ae passes 0x2ef2ba8 and is followed by atexit(0x354470). 31 + 10 + 16 mutants rejected. |
| E1255 | 2026-11-02 | Slot 498 (0x252ae0): 48 getters `b8 imm32 c3` (FDE 6) in 24 adjacent pairs; 24 call pairs walked instruction by instruction into 12 byte records (lo, hi, size) at 0x2eec3b0..0x2eec4d0; last pair has the canary load after the second call; native table tied row by row. 772 + 76 + 38 mutants rejected. |
| E1256 | 2026-11-02 | Slot 554 family re-measured: 17 x 397 B wrappers (0x6abb0 .. 0x6e4d0) share one normalised body, not a 0xc300 twin; 0x6e800 / 0x6e9f0 equal 0x3a980 and 0x6ebe0 / 0x6ede0 equal 0xc300 / 0xc580 (normalised); 0x53fc0 is 2974 B. Not modelled. STATIC_ANALYSIS_VERIFIED (equivalence by normalised disassembly only). |
| E1257 | 2026-11-02 | Refusals: 0x3be2d0 + 0x3c0840 (RB find-or-insert with allocation and rebalance 0x3c31b0 / 0x3c3470 / 0x3c3710), 0xa38960 (blob parser with unknown callees 0xa39eb0 / 0xa39fd0). No fake call, no emulation. |

### INSTALL_LOG.md

No install, download or dependency change.

## Risks

- The allocation-failure and refusal behaviour intentionally differs from the original (which would fault); a host that ignores `ok == false` would see a half-built variable.
- The 0x1f220 and 498 record meanings are not recovered; only byte layout, stores and call-site tables are proven.
- The slot-554 equivalences are normalised-disassembly comparisons, not yet per-instruction pinned.
- `std::atomic_ref` on `SharedStr` control words keeps the project's 8-byte-alignment convention.
- `run_all_checkers.sh` and `project_index.py` are not run; new checkers are not yet registered in the shared runner.
