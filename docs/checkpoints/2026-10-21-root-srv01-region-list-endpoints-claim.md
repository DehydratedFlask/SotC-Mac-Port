# Checkpoint: SRV-01-REGION-LIST-ENDPOINT-UNLINK-FIXTURE-01 — claim

- Local date/time: 2026-10-21 12:31 CDT.
- Agent ID: `/root` (integrator).
- Task ID: `SRV-01-REGION-LIST-ENDPOINT-UNLINK-FIXTURE-01`.
- Status: `ACTIVE`; failed tries: 0.
- Files changed at claim time: shared task/handoff documentation only; no source or test edits.
- Scope: in `native/tests/test_region_list.cpp`, add synthetic head and tail removal cases using
  `wire_chain`. For head removal, assert the new head's `prev` is null, the removed head's `next`
  self-loops, and `free_count` decrements. For tail removal, assert the new tail's `next` is null,
  the removed tail's `next` self-loops, and `free_count` decrements. Keep production unchanged.
- Gap evidence: the live `list_unlink` implementation handles null `prev`/`next` endpoints, but
  the existing test removes only the middle node of A–B–C.
- Prerequisite: `SRV-01` is `ACTIVE`; the proposed task is not registered and the target test file
  is clean and unassigned.
- Target command: `ctest --test-dir local/build/native -R '^region_list$'`.
- Acceptance limits: synthetic intrusive-list pointer behavior only; no allocator integration or
  live-game claim.
- Remaining risks: no test has been implemented or run yet.
- Next exact action: after this claim is published, assign only `native/tests/test_region_list.cpp`
  to `/root/slot23_raw_xrefs`; root will rebuild and replay the focused CTest.
