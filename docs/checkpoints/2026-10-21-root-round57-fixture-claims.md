# Checkpoint: round 57 synthetic fixture claims

- Local date/time: 2026-10-21 13:08 CDT.
- Agent ID: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`).
- Task IDs: `APP-01-CORE-READINESS-QUERY-FAILURE-NOT-BUILT-FIXTURE-01`,
  `RE-02-DYNLINK-UNMAPPED-SYMBOL-TYPE-FIXTURE-01`, and
  `SRV-01-HOST-MAP-ROUNDUP-OVERFLOW-FIXTURE-01`.
- Files changed to record these claims: `HANDOFF.md`, `docs/TASKS.md`, and this checkpoint.
- Source/registry reconnaissance: three scoped `trail ask ... --source --in <launcher|scripts|native>/`
  queries exited 0; direct live-source/task checks also exited 0. Tool output is retained in this
  task conversation. No source edit, test run, game-data read, or runtime experiment was performed.
- Repository state at claim: `main` at `4439751`, matching `origin/main`; only the pre-existing
  `native/api/CMakeLists.txt` modification and untracked `native/api/host/` were dirty. Both are
  outside these tasks.

## APP-01-CORE-READINESS-QUERY-FAILURE-NOT-BUILT-FIXTURE-01

- Bounded owner/file: `/root/launcher_review`,
  `launcher/Tests/LauncherCoreTests/CoreClientTests.swift` only.
- Live observation: `CoreReadiness.nativeCoreStatus` returns a `.notBuilt` value that includes the
  `CoreFailure.message` when both validation and coverage are nil and `queryFailure` is present.
  Existing tests cover unqueried and coverage-only cases, but not this distinct branch.
- Planned case: use a synthetic failure message such as `status request failed`; assert the exact
  `.notBuilt` copy with that message and the Play-disabled sentence. Production code is out of scope.
- Exact focused command: `swift test --package-path launcher --scratch-path local/build/launcher
  --filter CoreClientTests`.
- Claim result: implementation/test run NOT RUN; no log exists yet. Evidence is the live source/test
  spans and this checkpoint.
- Remaining risk/inference limit: pure mapping only; no native query failure, UI runtime, or game
  behavior is demonstrated.
- Next exact action: after this claim is committed and pushed, add the fixture, run the focused
  command, and save output under `local/validation/`.
- Status/attempt count: `ACTIVE`; 0 failed tries.

## RE-02-DYNLINK-UNMAPPED-SYMBOL-TYPE-FIXTURE-01

- Bounded owner/file: `/root/review_host_proto`, `scripts/test_ps4_dynlink.py` only.
- Live observation: `parse` maps known `st_info & 15` values through `SYMBOL_TYPES` and otherwise
  stores the numeric type as a decimal string. The current synthetic ELF has a known symbol type
  and its existing parse case does not assert the type field.
- Planned case: make the synthetic helper accept a symbol type, parse an unknown type such as `3`,
  and assert the symbol `type` field is the string `"3"`. Preserve current fixture defaults; parser
  code is out of scope.
- Exact focused command: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_ps4_dynlink.py`.
- Claim result: implementation/test run NOT RUN; no log exists yet. Evidence is the live parser/test
  spans and this checkpoint.
- Remaining risk/inference limit: synthetic symbol parsing only; no live ELF, loader, or runtime
  symbol-resolution behavior is established.
- Next exact action: after this claim is committed and pushed, add the synthetic assertion, run the
  focused command, and save output under `local/validation/`.
- Status/attempt count: `ACTIVE`; 0 failed tries.

## SRV-01-HOST-MAP-ROUNDUP-OVERFLOW-FIXTURE-01

- Bounded owner/file: `/root/slot23_raw_xrefs`, `native/tests/test_host_map.cpp` only.
- Live observation: `map_anonymous_aligned` computes page-rounded size, returns `nullopt` when the
  rounded value wraps below the input, and does so before calling `mmap`. Existing tests cover
  small mappings and invalid region requests, not rounding overflow; target `test_host_map` and
  CTest `host_map` are registered.
- Planned case: assert `map_anonymous_aligned(~std::uint64_t{0}, 0x100000)` returns no mapping.
  Production code and CMake are out of scope.
- Exact commands: `cmake --build local/build/native --target test_host_map` and
  `ctest --test-dir local/build/native -R '^host_map$'`.
- Claim result: implementation/test run NOT RUN; no log exists yet. Evidence is the live source/test
  and CMake registration plus this checkpoint.
- Remaining risk/inference limit: arithmetic guard only; no successful host mapping, allocator, or
  game behavior is established by this case.
- Next exact action: after this claim is committed and pushed, add the synthetic case, run both
  commands, and save output under `local/validation/`.
- Status/attempt count: `ACTIVE`; 0 failed tries.

## Shared checkpoint

- Observations are limited to existing source, tests, CMake registration, and task/evidence rows.
- No changes were made to original game files, reference clones, production files, or the user's
  pre-existing `native/api/` work.
- Next exact action: regenerate indexes, audit and publish the claim, then assign the three distinct
  test files.
