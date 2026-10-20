# CPU-01-CTOR-PREFIX-01 — fail-fast original constructor prefix claim

- Date/time: 2026-10-19 23:16 CDT (America/Chicago).
- Integrator: `/root`; implementation owner after publication: `/root/cpu_leaf_survey`.
- Task ID: CPU-01-CTOR-PREFIX-01; parent CPU-01.
- Files changed in claim: `HANDOFF.md`, `docs/TASKS.md`, and this checkpoint. No native code or
  checker changed at claim time.
- Failed tries: 0.

## Prerequisite evidence and bounded scope

Graft navigation used `graft callers run_constructors --depth all`; ambiguous duplicate definitions
produced no reliable cross-file caller graph. `graft grep "run_constructors"` then showed the two
definitions in `native/core/startup_ctors.cpp` and `.hpp`, and three call sites in
`native/tests/test_startup_ctors.cpp`.

For private ELF SHA-256
`d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`, the original constructor
table contains 573 entries at `0x2dd70e0`, with sentinel at `0x2dd5ef8`. The raw table words match
the private manifest and have SHA-256
`5917cb85db190e9ef93ce5e06d5b6596d40b6c3623166e37ed7c6c3c3cd3ab28`. The first unregistered slot
is index 10, RVA `0x2dd7090`, target `0xf09e60`. The 255-byte target body hash is
`fde1ac223a4c084da0a1a9d04804bee8ba8e12be0ac46e1a8e98a925ac4ffcc7`.

Agent disassembly shows allocator, registration, stack-check, and indirect-call edges. This supports
stopping at the gap, but complete service semantics remain unproven. The task owns only
`native/core/startup_ctors.hpp`, `native/core/startup_ctors.cpp`,
`native/tests/test_startup_ctors.cpp`, and new `tools/lift/check_ctor_slots.py`.

## Acceptance and limits

Represent original slot gaps and make the runner stop on the first unresolved original slot. The
fixture must report exactly ten completed callbacks and `blocked_on_rva == 0xf09e60`. Correct
registered indices for `0xd32720` and `0xd2ffe0` to 119 and 120. The private-input checker must
verify registered RVA/index pairs against the pinned table/hash.

Exact commands:

1. `cmake --build local/build/native --target test_startup_ctors`
2. `ctest --test-dir local/build/native -R '^startup_ctors$' --output-on-failure`
3. `python3 -I tools/lift/check_ctor_slots.py`

This proves runner bookkeeping against a private static table, not the actual startup chain. It does
not implement the blocked constructor's services, prove game startup reachability, or pass G1.
Failed implementation tries remain 0. Next exact action: publish the claim, then assign the listed
paths and request exact results/logs.

## Completion checkpoint — 2026-10-19 23:26 CDT

- Integrator: `/root`; implementation owner: `/root/cpu_leaf_survey`; task: CPU-01-CTOR-PREFIX-01.
- Files changed: `native/core/startup_ctors.hpp`, `native/core/startup_ctors.cpp`,
  `native/tests/test_startup_ctors.cpp`, `tools/lift/check_ctor_slots.py`, `docs/FOLDERS.md`,
  `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, and this checkpoint. No private game inputs
  or build outputs are staged.
- Root independently ran `cmake --build local/build/native --target test_startup_ctors` (exit 0),
  `ctest --test-dir local/build/native -R '^startup_ctors$' --output-on-failure` (exit 0, 1/1),
  `python3 -I tools/lift/check_ctor_slots.py` (exit 0), and `git diff --check` (exit 0). The checker
  reports ELF SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`, constructor
  table start `0x2dd70e0`, count 573, table SHA-256
  `5917cb85db190e9ef93ce5e06d5b6596d40b6c3623166e37ed7c6c3c3cd3ab28`, 103 verified registrations,
  and first unresolved slot index 10 / RVA `0xf09e60`. Worker logs are
  `local/validation/20261019T2321-cpu-01-ctor-prefix-01/01-cmake-build.log`, `02-ctest.log`,
  `03-check-ctor-slots.log`, and `commands.txt`. Root's independent command output was not redirected
  to a file.
- Project index refresh/check reported 4,128 folders and current. `graft check` passed; Graft found
  the new checker queryable. The worker's early stale-index check is retained and attributed to
  logs created after refresh; it is not counted as an implementation failure.
- Observation: the runner exits its recovered prefix at the first explicit null callback; the
  checker matches all listed original RVA/index pairs against the exact private table. Inference:
  the test prevents the fixture runner from silently skipping an unresolved constructor. It does
  not show that original runtime startup executes this harness or that slot 10 has known semantics.
- Remaining risks: RVA `0xf09e60` service behavior, constructor dependencies after the gap, allocator/
  atexit semantics, actual startup reachability, and all G1 criteria remain open. G1 is NOT PASSED;
  G2-G6 are NOT RUN. Failed implementation tries: 0. Task status: FIXTURE_VERIFIED.
- Next exact action: claim a bounded offline analysis of `0xf09e60`; implement nothing for this slot
  until its behavior and required native contract are established.
