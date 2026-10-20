# CPU-01-CTOR-METADATA-01 — constructor index provenance correction claim

- Date/time: 2026-10-19 23:29 CDT (America/Chicago).
- Integrator: `/root`; assigned owner: `/root/cpu_ctor_metadata`.
- Task ID: CPU-01-CTOR-METADATA-01; parent CPU-01-CTOR-PREFIX-01.
- Files changed in claim: `HANDOFF.md`, `docs/TASKS.md`, and this checkpoint. No implementation
  files have been edited yet.
- Failed tries: 0.

## Prerequisite evidence and bounded scope

The hash-bound constructor table checker verifies that RVA `0xd32720` is at original table index
119 and RVA `0xd2ffe0` is at index 120. `graft grep "0xd32720"` and
`graft grep "0xd2ffe0"` show the remaining 115/116 metadata in each native header, test JSON dump,
and private ELF checker. Those values conflict with the original table even though the standalone
leaf behavior fixtures themselves pass.

Update only those original-index annotations and expected JSON dump fields. Preserve all source and
destination cell addresses, test behavior, vector-copy/survey positions and function semantics.
Owned files: `native/core/vec_d32720.hpp`, `native/core/vec_d2ffe0.hpp`,
`native/tests/test_vec_d32720.cpp`, `native/tests/test_vec_d2ffe0.cpp`,
`tools/lift/check_fd32720.py`, and `tools/lift/check_fd2ffe0.py`.

## Acceptance and limits

Build `test_vec_d32720` and `test_vec_d2ffe0`, run focused CTests
`ctest --test-dir local/build/native -R '^(vec_d32720|vec_d2ffe0)$' --output-on-failure`, and run
`python3 -I tools/lift/check_fd32720.py` plus `python3 -I tools/lift/check_fd2ffe0.py`. Also rerun
`python3 -I tools/lift/check_ctor_slots.py` to preserve the table cross-check. Failed tries: 0.

This is a provenance correction only. It does not change either recovered function, establish
startup execution, or pass G1. Root verifies the results and integrates shared docs before commit.
Current status: ACTIVE; next action: make the exact metadata edits and run the listed checks.
