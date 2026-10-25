# Checkpoint — independent replay of the private-fixture test suite

- Local: 2026-10-23 ~00:10 CDT (America/Chicago)
- Agent: orchestrator round-3 verification worker (non-Claude model) + integrator replay
- Task: `INT-VALIDATION-FIXTURE-REPLAY-01` — attempt 1, success
- Gate: no gate promoted; this adjudicates a `NEEDS_PRINCIPAL` claim on another lane.
- Evidence: E521

## Why

The parallel-foundations lane recorded `INT-VALIDATION-PARALLEL-FOUNDATIONS-01` as
`NEEDS_PRINCIPAL` after three attempts, claiming "suite with private fixtures is 325/327,
FAILED", and E515 attributed the two failures to stale constructor expectations plus "missing
per-test fixture environments". Both hypotheses needed an independent check on the current tree,
which now contains the E518 test fix.

## OBS

- Fixtures present: `local/fixtures/slot23_e71990_names.bin` (331 B) and
  `local/fixtures/inputtable_551140_names.bin` (15 889 B); CMake assigns them to 12 tests via
  `ENVIRONMENT` properties.
- Fresh configure + build: 1080 targets, clean.
- `ctest --test-dir local/build/verify-fixtures`: **326/326 passed**, 35.58 s.
- Test count is **326**. The 325/327 claim does not reproduce on this tree.
- `registration_thunks`, `clothing_physical_mesh_parameters`, `startup_ctors` all pass, both
  individually and inside the suite.
- Fixture resolution: `resolve_*_fixture_path()` probes the env var first, then
  `local/fixtures/`, `../local/fixtures/`, … from the CTest working directory, so fixtures are
  found even without env vars during normal `ctest` runs.
- With env vars unset **and** a working directory where no relative probe matches: 4 tests
  hard-fail (`startup_ctors`, `slot23_names_fixture`, `registration_thunks_ahead`,
  `appmain_init_ctor_98a90_slice`) and 8 tests print `SKIP … (private fixture missing)` and exit 0
  after validating the fail-visible path.

## INF

The 8 silent-SKIP tests are a deliberate fail-visible design (they assert the missing-fixture
error path). The residual risk is that a truly missing fixture reduces coverage without failing
the suite when both the env var is absent and no relative probe matches. That does not occur under
`ctest` from the build directory; a configure-time fixture-presence check would be the fix if it
ever mattered, but fixtures are private by policy and public clones must still build, so the SKIP
is retained deliberately.

## Verdict

The suite is honestly green (326/326) on this tree; the `NEEDS_PRINCIPAL` blocker for
`INT-VALIDATION-PARALLEL-FOUNDATIONS-01` is stale with respect to this tree — its two failing
tests were exactly the stale constructor expectations fixed in E518. Remaining INT-VALIDATION
work is the actual integration acceptance scope, not a red suite.

## Next exact action

Re-run the full suite and publish-path audit from a clean build at the tip when the next
integration checkpoint is recorded; if `INT-VALIDATION-PARALLEL-FOUNDATIONS-01` is still open,
close the stale failure hypothesis and record the 326/326 replay against the new tip.
