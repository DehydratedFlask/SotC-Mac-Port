# APP-01 archive status-code fixture — claim checkpoint

- Local date/time: 2026-10-21 23:10 CDT (America/Chicago).
- Agent: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`).
- Task: `APP-01-CORE-ARCHIVE-STATUS-CODES-FIXTURE-01`, child of `APP-01-CORE-ABI-01`.
- Status: `ACTIVE`; 0 failed implementation tries. No source or test change has been made yet.
- Scope: add one pure Swift fixture to `launcher/Tests/LauncherNativeCoreTests/NativeCoreClientTests.swift`
  for status values 0–3 (`.ok`, `.missing`, `.unreadable`, `.badMagic`) and an unknown value
  (`.unknown(value)`). Keep production files unchanged.
- Baseline command: `swift test --package-path launcher --scratch-path
  local/build/app01-core-archive-status-codes --filter NativeCoreClientTests` — exit 0, 9 tests in
  1 suite passed. Log: `local/validation/app01-core-archive-status-codes/baseline.log`, SHA256
  `b69e18fba32ce2938a2787bd892604c0727e7868965085893b730762ddef312e`.
- Baseline file hashes: test `launcher/Tests/LauncherNativeCoreTests/NativeCoreClientTests.swift`
  `fa151723cf23fc14f4d7f03ed09fff53d214770c8fb66194c1fff3cb2eb40485`; mapper
  `launcher/Sources/LauncherCore/CoreModels.swift`
  `183d681e8566cf2d8c19b2a484f6e4163869ec3eb34c231f92ed91314b824eda`; C header
  `native/api/sotc_core.h` `6b29da35ffb1a6bfb8fa58a5e1a06620285591eb733b166c7cde8232bfc8dac0`.
- Observation: the C header defines 0–3 as OK, missing, unreadable, and bad magic; the Swift initializer
  maps those same values and preserves unknown values. Baseline tests do not exercise every numeric
  decoder case directly. This is evidence about the current source and test coverage, not a live ABI
  behavior claim.
- Files changed at claim: `HANDOFF.md`, `docs/TASKS.md`, and this checkpoint only. Existing user-owned
  changes `native/api/CMakeLists.txt` and `native/api/host/` remain untouched and unstaged.
- Remaining risks: the native ABI is still an incomplete stub; this mapping fixture will not establish
  archive inspection correctness, gameplay readiness, or real game behavior.
- Next exact action: add the synthetic code-mapping assertions; rerun the focused suite; record final
  hashes and E242; run project-index and Trail checks; audit and publish only allowlisted paths.
