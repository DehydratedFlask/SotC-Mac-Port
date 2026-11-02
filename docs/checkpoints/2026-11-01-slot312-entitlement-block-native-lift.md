# Startup slot 312 (`0xa46130`): characterized, modelled, registered

Date: 2026-11-01 (America/Chicago). Task: `CPU-01-STARTUP-SLOT-312-CHARACTERIZATION-01`
(the lift half of the same row). Ledger: E1068.

Status of the claims below: **FIXTURE_VERIFIED** for the native model and its failure paths,
**STATIC_ANALYSIS_VERIFIED** for the body/callee readings, **PASSED** for the build and CTest
counts. No runtime, boot, audio or save claim.

## Why this row moved

Slot 309 was registered on 2026-11-02, which made index 312 the production startup cliff
(`first_unrecovered_constructor_index()` returned 312, `first_unrecovered_constructor_rva()`
returned `0xa46130`). E1022 had left 312 with the caveat "at least 18 unresolved nested indirect
sites in selected descendants, no bounded lift or safe-omission proof". That caveat is now
**resolved by pinning the bodies the dispatches reach**, not by declaring the sites inert.

## Body characterization (pinned ELF `d0723afb…61d3`)

| Fact | Value |
|---|---|
| Table row | `0x2dd70e0 - 312*8 = 0x2dd6720` holds `0xa46130` |
| FDE extent | **1,250 B** `[0xa46130, 0xa46612)`; **1,249 B** reachable (last byte `0xa46611` is a `nop`) |
| Body sha256 | `33e64f61e940…` (1,250 B) / `327a08fc102a…` (1,249 B, matches E718) |
| Instructions | 238 reachable (matches E718) |
| Branches | **one**, `0xa465f8 jne 0xa4660c` → `__stack_chk_fail` (the canary compare) |
| Direct calls | 30 sites over 9 callees: `0xf590`×12, `0xf8f390`×8, `0x257a0`, `0xa45ba0`, `0xa45de0`, `0x26c10`, `0x26ea0`×3, `0xf180`×2, `0xf8f3a0` |
| Indirect calls | 2, both `callq *0x18(%rax)` at `0xa4626c` / `0xa46320` |
| Rip operands | 49 (8 stores), zero outside-image |
| Atexit | 8 records, pinned by site/dtor/object |

The block is the **entitlement / Platform Services title-info initializer**, the sibling of slot 310:
`titleskuid` (string variable, default `"DEFAULT"`), `titleAgeRestriction` (int, −1) and
`titleRegion` (int, 0), a menu group `"Platform Services"`/`"TitleInfo"`, three toggles
(`Sku Name` / `Age Restriction` / `Region`) and one 0x80 B record at `0x32f74f0` whose eight raw
stores are modelled without claiming field meanings. The neighbouring rodata also carries
`PS4EntitlementWorker.cpp` and the `ETitleRegion::*` names.

## E1022's 18 sites, accounted

Every one of them is a functor-vtable slot call of the ranged/unbounded variable template:

| Where | Sites | Targets |
|---|---|---|
| Row-local `callq *0x18(%rax)` | 2 | `0xc770` / `0xa45db0` (one `c3` each) |
| Class A wrapper + leaf (`0xa45ba0` + `0x3a980`) | 11 | `0xc720` / `0xc740` / `0xc760` / `0xc770` |
| Class B wrapper + leaf (`0xa45de0` + `0xa45fa0`) | 11 | `0xa45d60` / `0xa45d80` / `0xa45da0` / `0xa45db0` (byte-identical bodies, tag `0x15d18c0`) |
| Already native through `0x257a0` | 1 | `0x25aa0` (one `c3`) |
| Already native through `0x258f0` | 5 | `0x1595068` slots `0x25a50` / `0x25a70` / `0x25a90` / `0x25aa0` |

The four bodies are 20/20/9/1 B: copy stores `{src+0x10 -> dst+0x10, class tag -> dst+0x20}`,
clone stores the source tag at `dst+0x20` then the payload, move copies the payload, destroy is one
`c3`. The model therefore collapses the dispatch to its net state and **reports 11 collapsed calls per
int variable** rather than claiming to execute them — the same contract as
`core/ranged_bound_var_a4f4d0` for the three slot-309 instantiations.

## Modelled vs hooked

**Modelled (native, exact):** prologue/epilogue and canary guard; the `titleskuid` string variable
through the existing native `0x257a0`/`0x258f0` model with both stack-view releases and its atexit
record; the two int variables through `init_slot312_int_var` (net state of wrapper + leaf, value cell
= the object's own `+0xd0`, both bounds disengaged, final vptr, value/default read twice from the
caller's stack cell, `0x20d60` registration); the menu group through the native `0x26c10` model; the
three toggles through the native `0x26ea0` model with gate answers `{1,4}` / `{1,2}` / `{1,1}` (all
keep the default `{1.0, 10.0}` range); the record's eight stores and two empty views; all eight atexit
records in body order.

**Hook:** `Slot312Hooks::var_ctor` is an optional override with **no default in the struct**;
`default_slot312_hooks()` installs the native model. A null or failing hook stops the fragment at that
variable's stage before its atexit record. There is no hook that lacks a native implementation, which
is why the row was registered rather than left unregistered.

**Not modelled:** the five destructors (`0x23c90`, `0x18a60`, `0xa44a80`, `0x272d0`, `0xa44ac0`) and
the four values the argument blocks carry as **data** (`0x52dab0` / `0xa45dc0` at `q[1]`, `0xa44a40` /
`0xa44a60` at `q[3]`). They are pinned identity tags, recorded and never executed. Nothing consumes the
variables or the record yet, so no title/entitlement/PSN behaviour is claimed.

## An honest difference from slot 309

Slot 309's ranged variables point `+0x60` at a caller **stack temporary** (a dangling pointer after the
slot returns). Here the wrapper passes `this+0xd0`, so `+0x60` is a **self-pointer** and the object is
not left dangling. The model reproduces the self-pointer and does not "fix" anything.

## Files

* `native/core/ctor_slot_312_a46130.{hpp,cpp}` — the model (characterization in the header).
* `native/tests/test_ctor_slot_312.cpp` — 9 sections, fail-visible paths included.
* `tools/lift/check_ctor_slot_312.py` — 394 ELF + 145 native mutants, `--tamper elf|native` exit 1.
* `native/core/startup_ctors.{hpp,cpp}` — the production row.
* `native/tests/test_startup_ctor_coverage.cpp` — 451 / 314 / `0xa41810` / 122, pending 114, menu root
  68, atexit 288, plus slot-312 spot checks; `native/tests/test_startup_ctors.cpp` `expect[]` + 312.
* `tools/lift/check_ctor_slots.py`, `check_console_var_ctors.py`, `check_fctor_slots_296_297_300.py` —
  the three checkers that hard-coded 450 / 312 / 0xa46130.

## Verification (this window)

| Command | Result |
|---|---|
| `cmake -S native -B local/build/slot312` | exit 0 |
| `cmake --build local/build/slot312 -j 6` | exit 0, no warnings |
| `ctest --test-dir local/build/slot312 -j 6 --output-on-failure` | **532/532 PASSED** (533/533 on a re-run after a peer's `lookup_21620` test entered the tree) |
| `python3 -I tools/lift/check_ctor_slot_312.py` | OK, mutation self-test 394 + 145 |
| 96 `check_ctor_* / check_fctor_* / check_startup_* / check_slot*` checkers | 91 OK / 0 FAIL (89 + 2 in two batches), re-run after the peer edits landed |
| `python3 -I scripts/audit_publish_paths.py --all` | OK, 4,030 paths |
| `tools/lift/run_all_checkers.sh` | **NOT RUN** (out of scope by task instruction) |
| `python3 scripts/project_index.py` | **NOT RUN** (would regenerate `docs/INDEX.md` / `docs/FOLDERS.md`, out of scope) |

## Next exact action

The cliff is now index **314 / `0xa41810`** (4,329 B). Its body is much larger than 312's and should
get the same treatment: characterize the call inventory, pin every functor/vtable target, and only
then decide registered vs verified-unregistered.
