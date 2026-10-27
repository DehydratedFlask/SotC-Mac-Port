# Slot 306 native assembly (CPU-01-STARTUP-SLOT-306-ASSEMBLE-01)

Date: 2026-10-26. Orchestrator: GLM-5.3 `/root` (integrator). The module `native/core/ctor_slot_306_a633b0.{hpp,cpp}`
and its 676-line fixture `native/tests/test_ctor_slot_306.cpp` were inherited **already implemented and wired** from the
previous (undocumented) session; this run verified them, added the missing ELF-tie checker, fixed the stale row-count
pin, and ran the gates. No original code was executed, loaded or relocated at any point.

## What the fragment is

Native recovery of constructor-table index 306 / RVA `0xa633b0` (body `[0xa633b0,0xa654bd)`, `0x210d` = 8,461 B):
the `sndx_` sound-engine debug block — 46 objects in construction order (1 root pointer cell, 3 ranged int vars,
5 ranged float vars, 11 bool vars, 3 console commands, 23 debug-menu items), assembled from the E698/E699/E700 helper
modules. The production row `{0xa633b0, 306, ...}` in `startup_ctors.cpp` calls `init_slot306_a633b0(...)` and sets
`init_failed` when `slot306_result.ok` is false (fail-visible). Handler RVAs, destructor RVAs and vtable/functor tags
are identity tags only; nothing is executed through an original RVA. `kSlot306Gaps` is empty.

## This run's changes

1. `tools/lift/check_fctor_slots_296_297_300.py`: expected production-row count 317 → **318** (slot 306 now
   registered), order check extended `304 < 306`. Re-run exit 0: `OK ... 318 rows`.
2. `tools/lift/check_ctor_slot_306.py` (**new**, referenced by the module header but missing until now): pins the ELF
   SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`; the constructor-table word at file
   offset `0x2dda750` (`b033a60000000000` → `0xa633b0`); the body extent/SHA-256 (`f8f707c7f80129cb…`, 0x210d B);
   the two walker-omitted direct calls (`0xa637ae→0xf8f390` `__cxa_atexit`, `0xa643ad→0xc300`; E690/E695); all 22
   `sndx_` console names exactly once, 19 menu captions/paths + `"MenuSeparator"` present as NUL-terminated strings;
   the 46 object globals strictly increasing, mapped, not file-backed (BSS); the native `kSlot306Table` row-by-row
   against an independent compact copy of the semantics table; module constants; per-kind counts; menu-target
   well-formedness; the production row + fail-visible wiring; `StartupState` members; CMake registration.
   Mutation sanity: a tampered expected row (one global RVA changed) makes the checker exit 1
   (`FAIL table row #8 diverges from the pinned construction table`), proving it is fail-visible, not a rubber stamp.

## Gates (combined dirty tree, this run)

- `cmake -S native -B local/build/orch-r22` exit 0; `cmake --build local/build/orch-r22 -j 12` exit 0.
- Full CTest **401/401** (includes `ctor_slot_306` 1/1); log `local/validation/orch-20261026/ctest-full.log`.
- `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh`: **486 passed, 0 skipped, 0 failed**
  (484 before this run +1 for the new checker; the 317-row failure fixed). Log
  `local/validation/orch-20261026/lift-checkers-after-fix.log`.
- Trail rebuild, commit-attribution and staged-path audits NOT RUN at this checkpoint (they run before publication).

## Observations vs inference

- OBSERVED: every byte pin above reads the pinned ELF; the counts and hashes match E676/E690/E697.
- INFERRED: nothing new — the construction semantics were established by the inherited E697 checkpoint and its
  private simulator evidence; this run adds verification, not new semantics.
- The inherited implementation's own claims (46 objects built, pending-list/atexit/root-list side effects) are
  exercised by the inherited fixture; this run did not re-derive them.

## Status and remaining work

- Task **CPU-01-STARTUP-SLOT-306-ASSEMBLE-01**: **FIXTURE_VERIFIED + STATIC_ANALYSIS_VERIFIED** at the assembly
  scope (all 46 objects build on the native path; fail-closed stages named). Attempt 0 of this run; 0 failed
  hypotheses. G1 stays **PARTIAL** (slots beyond 306 remain; this is not a runtime/gate claim).
- Handler bodies `0xa5bf20/0xa5bf50/0xa5c020` and the category vars of engine init `0xa5cd90` are separate lanes
  (`CPU-01-SLOT306-HANDLERS-NATIVE-01`, `CPU-01-SLOT306-CATEGORY-VARS-01`) — in progress this run.
- Next exact action: integrate the handler/category lanes, run full gates + publish audits, record EVIDENCE rows,
  and ask the owner about commit/push and F590 acceptance.
