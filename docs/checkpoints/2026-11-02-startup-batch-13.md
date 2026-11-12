# Startup batch 13 (CPU-01-STARTUP-BATCH-13), 2026-11-02

Agent: startup-ctor lane worker (Claude Sonnet 5.5). No commit, tag or push was made. HANDOFF.md, docs/TASKS.md and
docs/EVIDENCE.md were NOT edited; the paste-ready text is at the end (ids E1230-E1235; the EVIDENCE tail was E1220-E1226 when
this was written, re-check it before pasting).

## Result

Coverage 527/573 -> **531/573** (42 remain). Cliff 370 (0x720b50) -> **390 (0x691c50)**.
Slots registered: 370, 469, 513, 508 (chain extended in startup_ctors.cpp; 370 and 469 were wired as plain extensions of the
generator, 513 and 508 needed new row kinds, see below).
Three-try rule: no try was burned on any registered slot; the refused slots were refused at triage, before any native code was written
(no escalation file needed, no slot has three failed hypotheses).

Pins moved: coverage test `slots.size()==531`, cliff 390/0x691c50, 42 remaining; pending 515 -> 720, debug-menu roots 278 -> 337,
atexit 934 -> 1203. `DEBUG_PAGE_BATCH_13 = {370, 469, 508, 513}` added to check_ctor_slots.py (batch 12 set unchanged; the union and
index-set digest were refreshed with `startup_gen/fixdigest.py`).

## Answer to the cliff question (slot 370)

The generator plus the existing `helper_620e10` lint-wrapper table wires 370 mechanically (52,949 B: bool, bounded and unbounded floats,
local-int wrapper sites, string variable, console commands, name holder, name cells, menu rows). No new mechanism was needed.

## Generator extensions (tools/lift/startup_gen)

`page.account()` still requires the whole-body call/atexit token stream to equal the concatenation of the row grammars (hard
AssertionError, never a silent drop).

| row kind | ELF shape | grammar | unlocks |
|---|---|---|---|
| `sent` (trailing sentinel container, 0x25..) | tracked store sequence ending in the atexit; bytes hash-pinned | `A` | 469 |
| `zcont` (zero-filled 0x20 B container) | 0x1c5090 plus a byte window of the init sequence, hash-pinned | `A` | 513 |
| `recs` (named-record registrar `0xa81700`) | `register_named_record_a81700(cell, record, name, kind, flag, alloc)`; registry cell 0x32fd398, guard 0x33053f0, atexit 0xa81b80 recorded once as an identity tag | `Qf`xN `+A` | 513 |
| `ufloat` with run-time-loaded initial value | 4-byte `movl (global)` of an unwritten address becomes a symbolic `('mut', addr, 4)` | `UiRffA` / `UfiRfA` | 508 |
| `lint` with description text | `<wrapper>fifA` | | 513 |

New honest-input mechanism for 508: the model exposes `Objects::source_bits[K]`, an explicit input field, defaulting to the initial image
of the ELF (zero for .bss, e.g. 0x2ee8820). The host supplies the value in effect at run time. The generated test, header comment and
checker all label this as an INPUT, not a constant of the body.

Tracker change (`track.py`): `addq`/`subq` on a symbolic non-stack value now yields `None` instead of raising a `TypeError`
(this is also what let 468/524/568 report their real blocker, below).

AllocDispatchContract: the registrar needs a host allocator. `StartupState` carries `RecordRegistryCell record_registry{}` and the
wiring lambda for 513 sets `record_alloc = default_slot513_record_alloc()`, a host malloc stand-in (never freed here; it is a host input,
not a model of the guest allocator). A missing allocator or registry reports `ok == false` (tested).

## Slots

| slot | RVA | size | content | status |
|---|---|---|---|---|
| 370 | 0x720b50 | 52,949 B | full debug page (bool / floats / lint sites / string var / commands / holder / cells / menu) | STATIC_ANALYSIS_VERIFIED + FIXTURE_VERIFIED; checker 4010 operand pins |
| 469 | 0x394a20 | 3,658 B | bool, bounded float, menu rows, sentinel container; `external_target_0 = &s.slot518.var` | same; 357 pins; 858 ELF + 106 native mutants rejected |
| 513 | 0x1d0770 | 4,050 B | 13 named records, ints, bools, described lint, menu rows, zero container | same; 351 pins; 983 ELF + 113 native mutants rejected |
| 508 | 0x2241e0 | 3,252 B | ranged and unbounded ints, bools, unbounded floats with run-time-loaded initial values | same; 307 pins; 759 ELF + 87 native mutants rejected |

Each has `native/core/ctor_slot_<n>_<rva>.{hpp,cpp}`, `native/tests/test_ctor_slot_<n>.cpp` and `tools/lift/check_ctor_slot_<n>.py`
(`--tamper elf` and `--tamper native` rejection probes verified; mutation self-test is fail-closed).

## Refusals (documented, not stubbed)

| slot | verdict | evidence |
|---|---|---|
| 424 | refused | not a sentinel container. About 26 KB of inline static-table initialisation (hundreds of string records at 0x2eff8e0.., about 285 mov/vmov and 584 `$0` stores) plus an atexit 0x54dbb0 whose rsi is unresolved. Needs a real table-initialisation model. |
| 497 | refused | unmodelled helpers (see the earlier subagent transcript, grep "497"); no helper_a81700 / helper_8d650 form covers its sites. |
| 468, 524, 568, 510 | refused | same shape, inline-functor console commands: `0x20af0` (flags 1/2) followed by an inline vptr store (0x1594dc0+0x10), a ymm functor copy into obj+0x60, `0x20540`, and a local callee (e.g. 0x16a380). There is no `call [0x1594dc0+0x20]` register-once, so the `cmd` grammar's assertion fails. A genuinely new mechanism. |
| 451 | refused | needs four things at once: late-release `W iRffA` uint (the unbounded-int helper `helper_init_unbounded_int_var_site` releases the description early only; I prototyped an `early_description_release` flag and **reverted it** because no registered slot exercises it), bool rows with real descriptions (0x7b/0x7d bytes, `BRffA`: both views released after register; the generator and `init_bool_var_literal` callers assume empty description), late lint `<49ca60>iRffA`, and trailing `<3b310>A<3b310>AA`. |
| 446 | refused | the same described late bool (`BRffA`, descriptions 59/87 bytes) plus `NN<3be2d0>ffA` command shapes. |
| 422 | refused | unknown direct call 0x274730 between rows. |
| 462 | refused | `toge` text view unresolved. |
| 530 | refused | ufloat/float call with `r8` (`f_r8`) unresolved. |
| 570 | refused | mixed new kinds (leading `AAA` atexits, `N` rows, ...). |

Shared helper change: none kept. `native/core/helper_int_var_sites.{hpp,cpp}` were briefly edited for 451 and restored byte for byte.

## Verification (build dir local/build/startup-b13)

| command | result |
|---|---|
| `cmake --build local/build/startup-b13 -j 12` | exit 0, no warnings |
| `ctest --test-dir local/build/startup-b13 -j 12 --output-on-failure` | 621/621 passed |
| `check_ctor_slot_370/469/513/508.py` (+ `--tamper elf|native`) | all OK, tamper probes rejected |
| `SOTC_NATIVE_BUILD=local/build/startup-b13 tools/lift/run_all_checkers.sh` | 830 passed, 0 skipped, 0 failed |
| `tools/lift/check_evidence_grades.py` | OK |
| `gen_entry_map.py` / `--check` | 2157 constants -> 1476 distinct guest RVAs in 494 headers (364 duplicates), OK |
| `g1_evidence_pack.py --write` / `--check`, `check_g1_evidence_pack.py` | OK |
| `survey_startup_gaps.py --write-doc` | HEAD 460/573, working tree 531/573, 42 open |
| `python3 -I scripts/audit_publish_paths.py --all` | PUBLISH PATH AUDIT OK (4110 paths) |
| `python3 scripts/project_index.py` | 85898 folders indexed |

Docs regenerated: docs/entry_map.json, docs/G1_EVIDENCE.md, docs/STARTUP_GAP_SURVEY.md, docs/FOLDERS.md.

## Risks and caveats

- 508 input fields default to the ELF image (zero for .bss). A real host must supply the run-time values; nothing here claims they are zero at run time.
- The registry/allocator for 513 is a host malloc stand-in, not a model of the guest allocator; leaks are intentional in the fixture.
- `track.py` now yields `None` for symbolic non-stack add/sub; any previously accepted page with such an add would now hard-fail rather than mis-track (all registered slots still pass).
- Batch-12/13 set split in `check_ctor_slots.py` is a bookkeeping boundary only.
- Everything is STATIC_ANALYSIS_VERIFIED / FIXTURE_VERIFIED: no guest code was executed, and no boot, audio or save claim is made.
- Refused slots are triage verdicts from the static tracker; none got a three-try escalation.

## Next exact action

Cliff slot 390 (0x691c50). The most valuable generator extension is the described late `BRffA` bool plus late lint (unlocks 451 and probably 446);
second is the inline-functor console command (468/524/568/510, four slots).

## Paste-ready text

HANDOFF checkpoint: 2026-11-02, startup-ctor lane (Claude Sonnet 5.5), task CPU-01-STARTUP-BATCH-13. Files: native/core/ctor_slot_{370,469,513,508}_*,
native/tests/test_ctor_slot_*.cpp, tools/lift/check_ctor_slot_*.py, tools/lift/startup_gen/*, startup_ctors.{hpp,cpp}, count pins. Commands as in the
Verification table. Coverage 531/573 (STATIC_ANALYSIS_VERIFIED + FIXTURE_VERIFIED), cliff 390 (0x691c50); pins pending 720, menu roots 337, atexit 1203.
Refused with documentation: 424, 497, 468, 524, 568, 510, 451, 446, 422, 462, 530, 570. Attempts: 1 per registered slot (all passed first try), no escalation.
Next: described late bool + late lint for 451/446, then inline-functor commands.

TASKS: CPU-01-STARTUP-BATCH-13 done (531/573); remaining 42, next cliff slot 390.

EVIDENCE rows:
| E1230 | 2026-11-02 | Startup batch 13: slot 370 (52,949 B) wired by the generator plus helper_620e10 table, no new mechanism; checker 4010 pins (STATIC_ANALYSIS_VERIFIED + FIXTURE_VERIFIED) |
| E1231 | 2026-11-02 | Slot 469: trailing sentinel-container row (`A`), 357 pins, mutation self-test 858 ELF + 106 native rejected |
| E1232 | 2026-11-02 | Slot 513: `recs` rows via register_named_record_a81700 (host allocator stand-in) plus `zcont` row, 351 pins, 983 ELF + 113 native mutants rejected |
| E1233 | 2026-11-02 | Slot 508: run-time-loaded ufloat initial values as an explicit `source_bits` input field (ELF image default), 307 pins, 759 ELF + 87 native mutants rejected |
| E1234 | 2026-11-02 | Batch 13 refusals: 424 (26 KB inline table init), 468/524/568/510 (inline-functor commands), 451/446 (described late bool, late lint), 422, 462, 530, 570, 497; static triage, no native code |
| E1235 | 2026-11-02 | Batch 13 totals: coverage 531/573, 42 remain, cliff 370 -> 390 (0x691c50); pending 720, menu roots 337, atexit 1203; ctest 621/621, run_all_checkers 830 passed, publish audit OK |
