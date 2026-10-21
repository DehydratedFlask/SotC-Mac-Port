# Checkpoint: SRV-01-REGION-SPLIT-OVERFLOW-FIXTURE-01 — claim

- Local date/time: 2026-10-21 12:23 CDT.
- Agent ID: `/root` (integrator).
- Task ID: `SRV-01-REGION-SPLIT-OVERFLOW-FIXTURE-01`.
- Status: `ACTIVE`; failed tries: 0.
- Files changed at claim time: shared task/handoff documentation only; no test or production
  source change yet.
- Scope: add one case to `native/tests/test_region_split.cpp` using a free node with
  `start = UINT64_MAX - 3`, `size = 16`, and `lead = 8`; initialize the new node with sentinel
  bytes, snapshot both raw byte arrays, assert `split_free_at_lead` returns false, and assert both
  raw arrays remain unchanged. Do not change the implementation.
- Gap evidence: `native/core/region_split.cpp` explicitly rejects wraparound in
  `free_start + lead`; current tests cover zero lead and a successful split but not overflow.
  CMake already registers the focused `region_split` test. No matching task is registered and the
  target test file is unassigned.
- Prerequisite: `SRV-01` is `ACTIVE`; the existing split fixture is present.
- Target command: `ctest --test-dir local/build/native -R '^region_split$'`.
- Acceptance limits: malformed synthetic range arithmetic only; no allocator runtime or game
  behavior claim.
- Remaining risks: no test has been implemented or run yet.
- Next exact action: after this claim is published, assign only `native/tests/test_region_split.cpp`
  to `/root/slot23_raw_xrefs`; root will replay the focused CTest.
