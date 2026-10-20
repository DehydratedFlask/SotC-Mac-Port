# CPU-01-CTOR-METADATA-01 — constructor index provenance correction claim

- Date/time: claim 2026-10-19 23:29 CDT; completion 2026-10-19 23:34 CDT (America/Chicago).
- Integrator: `/root`; assigned owner: `/root/cpu_ctor_metadata`.
- Task ID: CPU-01-CTOR-METADATA-01; parent CPU-01-CTOR-PREFIX-01.
- Files changed for claim: `HANDOFF.md`, `docs/TASKS.md`, and this checkpoint. Completion changes
  are listed below.
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
## Completion checkpoint

- Agent ID: `/root/cpu_ctor_metadata`; integrator verification: `/root`.
- Files changed: the six owned files listed above, plus `HANDOFF.md`, `docs/TASKS.md`,
  `docs/EVIDENCE.md`, and this checkpoint.
- Worker commands and logs:
  - `cmake --build local/build/native --target test_vec_d32720 test_vec_d2ffe0` — passed;
    `local/validation/20261019T2321-cpu-01-ctor-prefix-01/ctor-metadata-build.log`.
  - `ctest --test-dir local/build/native -R '^(vec_d32720|vec_d2ffe0)$' --output-on-failure` —
    2/2 passed; `.../ctor-metadata-ctest.log`.
  - `python3 -I tools/lift/check_fd32720.py` — passed, index 119;
    `.../ctor-metadata-check-fd32720.log`.
  - `python3 -I tools/lift/check_fd2ffe0.py` — passed, index 120;
    `.../ctor-metadata-check-fd2ffe0.log`.
  - `python3 -I tools/lift/check_ctor_slots.py` — passed, 103 pairs;
    `.../ctor-metadata-check-ctor-slots.log`.
  - `git diff --check` — passed.
- Root independently reran the build, focused CTest, all three checkers, and whitespace check after
  the worker; each passed. Root's rerun output was not redirected to a log.
- Observation: six original-index metadata values changed from 115/116 to table-derived 119/120;
  both leaf tests and hash-bound checkers pass. Inference: the standalone fixture provenance now
  agrees with the original constructor table. No startup or runtime behavior is established.
- Risks: slot 10 at RVA `0xf09e60` remains unresolved; the prefix test validates bookkeeping only;
  G1 remains NOT PASSED. Failed tries: 0.
- Publication gates: `python3 scripts/project_index.py --check` passed (4,128 folders), `graft check`
  passed (wiring graph in sync), staged/all path audits passed (10/576 paths), `git diff --cached
  --check` passed, and `git fetch origin main` plus `git rev-list --left-right --count
  HEAD...origin/main` reported `0 0`.
- Exact publication action/result: `git commit -m "fix: correct leaf constructor indices"` created
  `fc1b278`; `git push origin main` succeeded (`fa859cf..fc1b278`).
- Observation vs inference: the metadata correction and its evidence are present on public `main`.
  This does not establish startup behavior or pass G1. Failed tries: 0.
- Exact next action: finish CPU slot-10 and bridge-member private analyses, then integrate those
  results independently.
- Task status: FIXTURE_VERIFIED and published.
