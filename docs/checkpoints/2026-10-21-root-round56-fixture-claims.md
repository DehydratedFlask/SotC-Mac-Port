# Checkpoint: round 56 synthetic fixture claims

- Local date/time: 2026-10-21 13:00 CDT.
- Agent ID: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`).
- Task IDs: `APP-01-DIAGNOSTICS-UNKNOWN-ERROR-CODE-FORMAT-FIXTURE-01`,
  `AUD-01-XFF-ARCHIVE-SUMMARY-BYTE-IDENTITY-FIXTURE-01`, and
  `SRV-01-EXACT-FIT-CONSUME-FINALIZE-FIXTURE-01`.
- Files changed to record these claims: `HANDOFF.md`, `docs/TASKS.md`, and this checkpoint.
- Claim evidence command: `trail ask "Where is unknown diagnostic code formatting tested or
  implemented, and where does the audio bank header inspector reject summary byte-count mismatch
  before opening PSARC?" --source --in launcher/` followed by the same query scoped to `scripts/`;
  exit 0. Native candidate was independently traced by `/root/slot23_raw_xrefs` using Graft before
  live source confirmation. Registry/source confirmation commands `rg -n ... docs/TASKS.md
  docs/EVIDENCE.md HANDOFF.md`, `sed -n ...` for the cited implementation/test spans, and
  `git status -sb` all exited 0. Tool output is retained in this task conversation; no game data was
  read or changed.
- Repository state at claim: `main` at `d3c902d1e8679f4cf99224e62cd7f6b856b5385a`, matching
  `origin/main`; only the pre-existing `native/api/CMakeLists.txt` modification and untracked
  `native/api/host/` were dirty. Both are outside these tasks.

## APP-01-DIAGNOSTICS-UNKNOWN-ERROR-CODE-FORMAT-FIXTURE-01

- Bounded owner/file: `/root/launcher_review`,
  `launcher/Tests/LauncherCoreTests/LauncherModelTests.swift` only.
- Live observation: `CoreDiagnostics.code` in `launcher/Sources/LauncherCore/CoreReadiness.swift`
  has an explicit `.unknown(let raw)` branch returning `UNKNOWN(raw)`. Existing launcher fixtures
  assert unknown subsystem formatting but no direct error-code formatter assertion was found.
- Planned assertion: `.unknown(99)` formats as `UNKNOWN(99)`; no production change is requested.
- Exact focused command: `swift test --package-path launcher --scratch-path local/build/launcher
  --filter LauncherModelTests`.
- Claim result: implementation/test run NOT RUN; no log exists yet. Evidence is the live source span
  above and this checkpoint.
- Remaining risk/inference limit: the assertion covers formatter output only, not UI presentation,
  ABI behavior, or game execution.
- Next exact action: after this claim is committed and pushed, add the assertion in the owned test
  file, run the focused command, and save the output under `local/validation/`.
- Status/attempt count: `ACTIVE`; 0 failed tries.

## AUD-01-XFF-ARCHIVE-SUMMARY-BYTE-IDENTITY-FIXTURE-01

- Bounded owner/file: `/root/review_host_proto`, `scripts/test_audio_bank_header.py` only.
- Live observation: `scripts/audio_bank_header.py::inspect_members` compares the sidecar's `bytes`
  value with `archive_path.stat().st_size` before constructing `Psarc(archive_path)`; the current
  synthetic fixture supplies an archive, summary, manifest, and mock reader.
- Planned case: change only the summary byte count, assert the exact
  `archive summary byte count <reported> != live <size>` error, and assert `Psarc` was not called.
  No parser change is requested.
- Exact focused command: `PYTHONDONTWRITEBYTECODE=1 python3 -I
  scripts/test_audio_bank_header.py`.
- Claim result: implementation/test run NOT RUN; no log exists yet. Evidence is the live source span
  above and this checkpoint.
- Remaining risk/inference limit: sidecar identity validation only; no live archive, codec, audio
  playback, or game behavior is established.
- Next exact action: after this claim is committed and pushed, add the synthetic case, run the
  focused command, and save the output under `local/validation/`.
- Status/attempt count: `ACTIVE`; 0 failed tries.

## SRV-01-EXACT-FIT-CONSUME-FINALIZE-FIXTURE-01

- Bounded owner/file: `/root/slot23_raw_xrefs`,
  `native/tests/test_exact_fit_finalize.cpp` only.
- Live observation: the test separately checks `try_carve_fit` exact-node arithmetic and calls
  `exact_fit_finalize` for a partial-size request. The finalizer writes the size, arena flag,
  `flag_b`, context pointer, and sentinel; the CMake executable and CTest entry already exist.
- Planned case: initialize a synthetic node for the exact request, pass the exact request through
  the finalizer, and assert the resulting fields. No implementation/CMake change is requested.
- Exact commands:
  `cmake --build local/build/native --target test_exact_fit_finalize` and
  `ctest --test-dir local/build/native -R '^exact_fit_finalize$'`.
- Claim result: implementation/test run NOT RUN; no log exists yet. Evidence is the live source
  spans above and this checkpoint.
- Remaining risk/inference limit: combined synthetic helper coverage only; no allocator integration,
  runtime behavior, or game behavior is established.
- Next exact action: after this claim is committed and pushed, add the synthetic combined-path case,
  run both commands, and save outputs under `local/validation/`.
- Status/attempt count: `ACTIVE`; 0 failed tries.

## Shared checkpoint

- Observations are limited to existing source, tests, task rows, and CMake registration. Fixture
  outcomes remain unverified until worker and root runs complete.
- No changes were made to game inputs, reference clones, production implementation files, or the
  user's pre-existing `native/api/` work.
- Next exact action: regenerate indexes, audit allowlisted staged paths, commit/push the claims, then
  assign the three distinct test files for implementation.
