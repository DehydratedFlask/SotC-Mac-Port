# NEEDS_PRINCIPAL: INT-VALIDATION-PARALLEL-FOUNDATIONS-01 — full native suite

- Owner/principal/date: /root / /root, 2026-10-22 22:24 CDT (America/Chicago).
- Requirement: full integrated native tests must pass with required private fixtures. Do not
  remove assertions, skip tests, or promote game gates from fixtures.
- Source base ff7b186; no original binary executes. Original fixture hashes: slot23
  9bc9512093dd54b4d87fa05f4eda649dc804f0f3a67ea5f959e376df509ee771,
  inputtable 55f76754a86e1044d5ab2cc80a4fda8851409e3f607a00b945c8e7fd9e9b713c.
- Current changes: save writer/test, independent release module/test/checker, audio CFG tool;
  root CMake integration. No startup_ctors or existing constructor semantic edits.
- Private logs/hashes/binary identities: managed worktree
  local/validation/root-parallel-foundations-20261022/manifest.json.

All below use ASAN_OPTIONS=detect_leaks=0, UBSAN_OPTIONS=halt_on_error=1 and command
`ctest --test-dir local/build/root-parallel-foundations-20261022 --output-on-failure -j 8`.
Configure Debug/Ninja SOTC_SANITIZE=ON and full 1082-step build passed beforehand.

| Failed try | Hypothesis/change | Exact additional environment | Actual result | Log |
|---|---|---|---|---|
| 1 | Integrated build is ready for complete suite | none | exit8; 321/327, six failures including missing private fixtures | ctest.log |
| 2 | Exported original fixture paths satisfy loaders | SOTC_SLOT23_NAMES_FIXTURE=/Volumes/Extra/SoC Mac Native/local/fixtures/slot23_e71990_names.bin; SOTC_INPUTTABLE_NAMES_FIXTURE=/Volumes/Extra/SoC Mac Native/local/fixtures/inputtable_551140_names.bin (quoted full paths) | exit8; same six failures; CTest ENVIRONMENT properties override exports with managed-worktree paths | ctest-with-fixtures.log |
| 3 | Verified private copies at CMake-prescribed local/fixtures paths satisfy all six | copied only these two files after hash checks; standard env above | exit8; 325/327, four fixed; two constructor expectations still fail | ctest-private-fixtures-local.log |

Smallest current reproducer: same ctest command with
`-R '^(clothing_physical_mesh_parameters|registration_thunks)$'`.
Expected pass; actual assertions: test_clothing_physical_mesh_parameters.cpp:205
`!r.ok() && !r.callback_failed`; test_registration_thunks.cpp:64
`r.blocked_on_rva == 0xe71990 && !r.callback_failed`.

Verified: first four failures were fixture-location issues, not changed startup code. Two remaining
failures occur outside the edited behavior. Inference only: expectations may be stale from the
now-registered ctor prefix; baseline comparison is required before that conclusion.
Independent save review and release/audio verification can continue. HANDOFF and TASKS record
this escalation; G0/G1 PARTIAL and G2–G6 NOT RUN remain unchanged.

## Principal reassessment before a fourth approach

/root authorizes one distinct targeted clean-baseline isolation: extract ONLY tracked native/
from ff7b186 into ignored scratch; configure Debug/Ninja ASan/UBSan; copy the same hash-verified
fixtures into the baseline's local/fixtures; build/run only the two failing tests. Preserve all
prior logs and attempt counts. This establishes pre-existing failure versus regression; it does
not change acceptance. Do not rerun the same full suite again until baseline diagnosis yields
an actionable fix. If baseline also fails, claim a separate bounded test-contract reconciliation
only after validating actual implemented ctor behavior, without touching Drive-owned startup code.
