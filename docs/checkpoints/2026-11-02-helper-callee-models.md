# Checkpoint 2026-11-02: helper callee models (CPU-01-HELPER-CALLEE-MODELS-01)

Agent: helper-b worker (Claude Sonnet 5.5). Workers do not edit HANDOFF.md / docs/TASKS.md / docs/EVIDENCE.md; the
integrator pastes the sections below. Nothing was committed. Attempt count: 1 (no failed tries).

## Premise correction

All five helper callees already had native bodies (`init_ranged_int_var_c580`, `init_unbounded_int_var_worker_3a980`,
`init_unbounded_float_var_6aa00`, `init_bounded_float_var_c300`, `init_debug_menu_item_26db0`, `init_string_var_257a0`).
The blocker for slots 430 / 504 / 555 was the caller functor tag: each helper accepts only functor tags it has proven,
and the slots pass tags no helper had admitted. 366/411 call 0x26db0 with bool targets and are not blocked by it.
0x257a0 is self-contained and already modelled (documentation only, no new file).

## What was added (all new files; startup_ctors.*, ctor_slot_*, count pins, survey/G1/entry_map untouched)

| helper | size | shape | unblocks |
|---|---|---|---|
| 0xc580 ranged int ctor | 0x15f B | `helper_c580_ranged_int`: admits any int-family functor tag, else refuses with nothing written | slot 430 (tag 0x15c8450) |
| 0x3a980 unbounded int ctor | 0x142 B | `helper_3a980_unbounded_int`: same admission, normalises class to 0x15944a0 | slot 555 (tag 0x1595e60) |
| 0x6aa00 unbounded float / 0xc300 bounded float | 0x142 / 0x160 B | `helper_6aa00_unbounded_float`, `helper_c300_bounded_float`, `init_float_var_static_family` (float-family admission) | slot 504 (tag 0x159ba68) |
| 0x26db0 named menu item | 0xef B | `helper_26db0_named_item_{bool,int,float,string}`: explicit target class; unknown class refuses, item untouched | 366/411 already unblocked; adapter only |
| 0x257a0 string var | 0x14c B (+0x258f0, 0x157 B) | already modelled (CPU-01-STRING-VAR-257A0-01) | n/a |

Functor family: a class table whose four relocated qwords are exactly the four family bodies (int
0xc720/0xc740/0xc760/0xc770, float 0xc4a0/0xc4c0/0xc4e0/0xc4f0). The ELF has 37 int and 68 float tables; the native
arrays `kIntFunctorTags` / `kFloatFunctorTags` in `helper_functor_tags.cpp` equal the ELF set in both directions.
The stored final class is independent of the caller tag as long as it is a family member.

Files: `native/core/helper_functor_tags.{hpp,cpp}`, `helper_c580_ranged_int.*`, `helper_3a980_unbounded_int.*`,
`helper_6aa00_unbounded_float.*`, `helper_26db0_named_item.*`; `native/tests/helper_test_util.hpp`,
`test_helper_{c580,3a980,6aa00,26db0}.cpp`; `tools/lift/helper_callee_lib.py`, `helper_callee_pins.py`,
`check_helper_{c580,3a980,6aa00,26db0}.py`; `native/CMakeLists.txt` (stanza appended at the end: library
`sotc_helper_callees`, four tests `helper_c580/3a980/6aa00/26db0`).

## Commands and results (2026-11-02)

```sh
cmake -S native -B local/build/helper-b
cmake --build local/build/helper-b -j 12 --target test_helper_c580 test_helper_3a980 test_helper_6aa00 test_helper_26db0   # clean, -Werror
ctest --test-dir local/build/helper-b -R '^helper_' --output-on-failure      # 4/4 passed
python3 -I tools/lift/check_helper_c580.py    # OK, 399 ELF + 35 spec + 67 native mutants rejected
python3 -I tools/lift/check_helper_3a980.py   # OK, 408 ELF + 35 spec + 60 native mutants rejected
python3 -I tools/lift/check_helper_6aa00.py   # OK, 680 ELF + 43 spec + 101 native mutants rejected
python3 -I tools/lift/check_helper_26db0.py   # OK, 165 ELF + 23 spec + 33 native mutants rejected
python3 -I tools/lift/check_helper_<x>.py --no-mutate --tamper elf|native   # exit 1 for all 8 runs
python3 -I scripts/audit_publish_paths.py --all    # PUBLISH PATH AUDIT OK (4110 paths)
```

## Paste-ready EVIDENCE.md rows

- E1110 STATIC_ANALYSIS_VERIFIED: `check_helper_c580.py` pins 0xc580 (0x15f B, sha a93bc0f3...), int functor bodies
  0xc720..0xc770 and slot 430 (0x529df0): exactly one call to 0xc580 (0x529f02), the only family table loaded is
  0x15c8450, tag clone/destroy slots are the family bodies.
- E1111 STATIC_ANALYSIS_VERIFIED: `check_helper_3a980.py` pins 0x3a980 (0x142 B) and slot 555 (0x527f0), tag 0x1595e60.
- E1112 STATIC_ANALYSIS_VERIFIED: `check_helper_6aa00.py` pins 0x6aa00 (0x142 B), 0xc300 (0x160 B), float bodies and
  slot 504 (0x23f4d0), tag 0x159ba68.
- E1113 STATIC_ANALYSIS_VERIFIED: `check_helper_26db0.py` pins 0x26db0 (0xef B), class slots 3/6 of the four variable
  classes (results 1/0, 1/2, 1/3, 1/4) and the two 0x26db0 calls in slots 366 (0x744ac0) and 411 (0x591710), bool class only.
- E1114 FIXTURE_VERIFIED: `helper_c580`, `helper_3a980`, `helper_6aa00`, `helper_26db0` ctest fixtures.
- E1115 STATIC_ANALYSIS_VERIFIED: ELF functor families (37 int, 68 float) equal the native tag arrays both ways.

## Paste-ready HANDOFF / TASKS text

CPU-01-HELPER-CALLEE-MODELS-01: models and checkers done for 0xc580, 0x3a980, 0x6aa00/0xc300, 0x26db0 (0x257a0 was
already modelled). Wiring into slots 430 / 504 / 555 is NOT done: the slot owner must call
`helper_c580_ranged_int` (tag 0x15c8450), `helper_6aa00_unbounded_float` (0x159ba68), `helper_3a980_unbounded_int`
(0x1595e60) and check `HelperRefusal::none`. Status: done, attempt 1.

## Risks

- Family admission proves the tag's four slots are the family bodies (class-slot equivalence); the per-site tag is
  verified only for slots 430 / 504 / 555. Other slots passing a family tag still need their own call-site proof.
- Checker pins come from the same pinned ELF as the facts they check; a behavioural equality proof of the native bodies
  is not claimed (fixtures check the stated semantics only).
- The call-site census (one call, one family lea) is a byte-pattern scan, not a CFG proof.
- `sotc_core` is shared with another worker's in-flight edits and may transiently fail to build.
- No Metal/boot/audio/save claims are made by this work.
