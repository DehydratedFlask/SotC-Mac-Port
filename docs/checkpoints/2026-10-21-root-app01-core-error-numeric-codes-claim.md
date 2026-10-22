# APP-01 CoreErrorCode numeric mapping — claim checkpoint

- Local date/time: 2026-10-21 23:18 CDT (America/Chicago).
- Agent: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`), integrator.
- Task: `APP-01-CORE-ERROR-NUMERIC-CODES-ALL-VALUES-FIXTURE-01`, child of `APP-01-CORE-ABI-01`.
- Status: `ACTIVE`; 0 failed implementation tries. No new test file or production change has been made.
- Scope: create `launcher/Tests/LauncherCoreTests/CoreErrorCodeMappingTests.swift` with direct assertions
  for C ABI error codes 0–8 and unknown values 9 and `UInt32.max`. Keep production code unchanged.
- Baseline command: `swift test --package-path launcher --scratch-path
  local/build/app01-core-error-codes --filter CoreClientTests` — exit 0, 13 tests in 1 suite passed.
  Log `local/validation/app01-core-error-codes/baseline.log`, SHA256
  `2d8ab37ead356940cc36000c9e52ad72b13465d459f04a2f15f8c4f7b745195a`.
- Baseline file hashes: `launcher/Sources/LauncherCore/CoreModels.swift`
  `183d681e8566cf2d8c19b2a484f6e4163869ec3eb34c231f92ed91314b824eda`; C enum
  `native/api/sotc_core.h` `6b29da35ffb1a6bfb8fa58a5e1a06620285591eb733b166c7cde8232bfc8dac0`;
  related existing test `launcher/Tests/LauncherCoreTests/CoreClientTests.swift`
  `7dfd2f4d50479d0cfb208643b8d0cac0dd91a1f2742f4e4a1c0371ad84548c92`.
- Observation: the C header assigns values 0–8 in order from `SOTC_OK` through `SOTC_ERR_INTERNAL`;
  Swift maps those same values to the nine named `CoreErrorCode` cases and preserves other values as
  `.unknown(code)`. Existing numeric tests directly assert only error codes 3, 4, and 99. This is
  current source/test evidence, not a live ABI contract demonstration.
- Files changed at claim: `HANDOFF.md`, `docs/TASKS.md`, and this checkpoint only. The new Swift test
  file and both production files remain unchanged. User-owned native API changes remain untouched.
- Remaining risks: the ABI implementation is still an incomplete stub; no live C-to-Swift transport,
  engine behavior, or game behavior is established by this mapping fixture.
- Next exact action: add a standalone test that pins all nine cases plus unknown preservation; run the
  new suite and existing `CoreClientTests`; record final hashes, E244, and a completion checkpoint.
