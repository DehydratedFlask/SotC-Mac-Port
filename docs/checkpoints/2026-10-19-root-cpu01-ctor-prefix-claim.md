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
