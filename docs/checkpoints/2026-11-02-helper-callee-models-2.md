# Checkpoint 2026-11-02: helper callee models, part 2 (CPU-01-HELPER-CALLEE-MODELS-02, worker helper-c)

Agent: helper-c worker (Claude Sonnet 5.5). Workers do not edit HANDOFF.md / docs/TASKS.md / docs/EVIDENCE.md; the integrator pastes the
sections below. Nothing was committed. Build dir `local/build/helper-c`. Attempt count: 1, no failed tries. Labels: STATIC_ANALYSIS_VERIFIED +
FIXTURE_VERIFIED. No original RVA was executed; no boot, audio or save claim.

## Premise check (batch 9 "unmodelled helper" list)

Batch 9 listed 391 (0xf2f0), 561 (0xc580/0x3a980), 469 (0x27050), 401 (0x620e10), 501, 508, 352 as "unmodelled helper each". Re-measured:

| slot | helper named | status |
|---|---|---|
| 561, 508, 352 | 0xc580, 0x3a980, 0x6aa00, 0xc300 | already native (helper-b, `helper_{c580,3a980,6aa00}`); survey template shape "pure" |
| 469, 501 | 0x27050, 0x26800, 0x26ea0, 0x26c10 | already native inside slots 326 / 306 / 299 (`init_slot326_label_item_26800`, the slider ctor); survey shape "pure" |
| 391 | 0xf2f0 (40 B borrowed-cstr view: `{ptr, strlen, 0, 0}`) | already native (`slot335_borrowed_str`, used by slots 306/307/313/316/333/335); survey: "2 functor fix-up call(s)" only |
| **401** | **0x620e10, 0x6211d0** (435 B each) | **the only truly unmodelled helpers; modelled now** |

So the survey's remaining flag for 401 ("+ 2 unmodelled helper(s)") is the only one that needed a new body. The other six slots are blocked by their own
slot-level shapes (indirect calls, functor tags, long bodies), not by a helper; this worker did not fake a helper for them.

## What was added (new files only; startup_ctors.*, ctor_slot_*, count pins, survey/G1/entry_map/FOLDERS untouched; no slot registered)

| helper | size | shape | unblocks |
|---|---|---|---|
| 0x620e10 / 0x6211d0 and 24 twins ("local ranged-int variable wrapper family") | 26 x 435 B wrapper + 26 x 351 B inner | table-driven `helper_local_int_var_ctor(wrapper_rva, ...)`: the 0xc580 ranged-int object with a private class pair (mid vptr, functor class, final vptr, dtor), `[this+0xd0] = [this+0x68] = *init`, registration 0x20d60; only the row's OWN functor class is admitted as caller tag | slot 401 (both variables); table facts for 363, 370, 398, 399, 422, 446, 497, 513 (callers still need their own tag proof) |

Files: `native/core/helper_620e10_local_int_var.{hpp,cpp}`, `native/tests/test_helper_620e10.cpp`, `tools/lift/check_helper_620e10.py`,
`native/CMakeLists.txt` (stanza appended at the very end: `target_sources(sotc_core ...)`, `test_helper_620e10`, ctest `helper_620e10`).

API: `kLocalIntVarClasses` (26 ELF-derived rows `{wrapper, inner, mid vptr, functor class, final vptr, dtor, slot3, slot6}`), `find_local_int_var_class[_by_final]`,
`helper_local_int_var_ctor`, `local_int_var_menu_target` (slot3 = slot6 = 1, so the 0x26db0 / 0x26ea0 range stays {1.0, 10.0}), `helper_26db0_named_item_local_int`,
`helper_init_local_int_var_site` (slot 401 order: wrapper, `call [tag+0x18]`, `f590(name)`, `__cxa_atexit(dtor, obj)`; description is NOT released; the 7ec160 order is selectable)
and the two observed slot-401 sites `kSlot401PhaseBlendUseSite` (`cp10_phaseBlendUse`, init 0, range [0,2], flags 1, functor tag 0x15cc318, atexit 0x6112e0, object 0x300d0c0) and
`kSlot401PhaseBlendTypeSite` (`cp10_phaseBlendTypeOverride`, init 0, range [1,4], flags 1, tag 0x15cc3b0, atexit 0x611320, object 0x300d198). The slot owner must call
`helper_init_local_int_var_site(var, kSlot401..., empty_string_f730(), registry)` and check `HelperRefusal::none` / `ok`. Slot 401 also calls 0x26db0 (toggles) later; use
`helper_26db0_named_item_local_int` for toggles over these variables.

What the checker proves (byte equalities, not names): every wrapper equals the hash-pinned wrapper 0x7ec160 outside 7 rel32 windows; every inner equals the hash-pinned 0xc580 outside 6 windows;
each operand resolves to the row; the four functor bodies of each class equal the int bodies outside the class operand; final slots 0/2/3/6 and mid slots 2/3/5/6; slot 3/6 bodies are
`mov eax,1; ret`; slot 401's two call sites (names, lengths, init/range/flags, f730 description, push order, destroy, f590, atexit) as instruction pins and as four hash-pinned runs.

## Commands and results (2026-11-02)

```sh
cmake -S native -B local/build/helper-c                                              # exit 0
cmake --build local/build/helper-c -j 12 --target test_helper_620e10                 # clean under -Werror, 0 warnings
cmake --build local/build/helper-c -j 12 --target test_helper_{c580,7ec160,233ec0,3a980,6aa00,26db0}   # clean
ctest --test-dir local/build/helper-c -R '^helper_' --output-on-failure              # 7/7 passed
python3 -I tools/lift/check_helper_620e10.py                    # OK: 18 operand pins + family + slot-401 sites; 2325 ELF + 66 native mutants all rejected
python3 -I tools/lift/check_helper_620e10.py --no-mutate --tamper elf      # exit 1
python3 -I tools/lift/check_helper_620e10.py --no-mutate --tamper native   # exit 1
python3 -I scripts/audit_publish_paths.py --all                                      # PUBLISH PATH AUDIT OK (4110 paths; new files untracked)
```

## Paste-ready EVIDENCE.md rows (re-check the tail first; I used E1180+)

- E1180 STATIC_ANALYSIS_VERIFIED: `check_helper_620e10.py` pins the 26-member local ranged-int wrapper family: wrapper == 0x7ec160 outside 7 rel32 windows, inner == 0xc580 outside 6 windows,
  operands resolved to the ELF-derived rows, functor bodies == int bodies 0xc720..0xc770 outside the class operand, final slots 3/6 `mov eax,1; ret`.
- E1181 STATIC_ANALYSIS_VERIFIED: slot 401 (0x621530, 3799 B) calls 0x620e10 / 0x6211d0 exactly once each (byte-pattern census); sites pinned: tags 0x15cc318 / 0x15cc3b0, names
  `cp10_phaseBlendUse` (18) / `cp10_phaseBlendTypeOverride` (27), init 0, ranges [0,2] / [1,4], flags 1, f730 description, destroy, f590(name), atexit 0x6112e0 / 0x611320, objects 0x300d0c0 / 0x300d198.
- E1182 FIXTURE_VERIFIED: ctest `helper_620e10` (every row constructed, own-tag admission, foreign/family/garbage tag and non-member RVA refused with nothing written, both slot-401 sites, 0x26db0 over a family variable).
- E1183 STATIC_ANALYSIS_VERIFIED: batch-9 "unmodelled helper" list re-measured: only 401 needed new helper bodies (see table above).

## Paste-ready HANDOFF / TASKS text

CPU-01-HELPER-CALLEE-MODELS-02: the batch-9 helper list was mostly stale (561/508/352/469/501/391 helpers are already native). New: `helper_620e10_local_int_var` models the 26-wrapper
local ranged-int family and slot 401's two variables (checker 2325 + 66 mutants rejected, tamper probes exit 1). Wiring into slot 401 is NOT done (another worker owns startup_ctors / ctor_slot_*);
the slot owner calls `helper_init_local_int_var_site` with `empty_string_f730()`. Remaining blockers of 401 are outside this helper (the rest of its 3799 B body: 23 atexit records, 0x26db0 toggles).
Status: done, attempt 1.

## Risks

- The wrapper family claim rests on byte equality to hash-pinned template bodies; the functor re-wrap is modelled by net effect, not run. The census of wrapper calls in slot 401 is a byte-pattern scan, not a CFG proof.
- Only the row's own functor class is admitted. A caller passing the shared int-family tag (stores 0x15944a0) would leave a different class constant in the object; this is refused rather than guessed. The 24 non-401 rows have no per-site proof here.
- `sotc_core` is shared with other workers' in-flight edits; it built clean when measured (first build 18.9 s).
- The slot-401 name readings (phase blend) are inference from strings only.
