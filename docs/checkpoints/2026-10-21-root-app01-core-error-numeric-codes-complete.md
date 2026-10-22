# APP-01 CoreErrorCode numeric mapping — completion checkpoint

- Local date/time: 2026-10-21 23:22 CDT (America/Chicago).
- Agent: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`).
- Task: `APP-01-CORE-ERROR-NUMERIC-CODES-ALL-VALUES-FIXTURE-01`, child of `APP-01-CORE-ABI-01`.
- Status: `FIXTURE_VERIFIED`; 1 successful implementation attempt, 0 failed implementation tries.
- Files changed: `launcher/Tests/LauncherCoreTests/CoreErrorCodeMappingTests.swift`, `HANDOFF.md`,
  `docs/TASKS.md`, `docs/EVIDENCE.md`, and this checkpoint. Production mapper and C header were
  unchanged. User-owned `native/api/CMakeLists.txt` and `native/api/host/` remain untouched.
- Baseline command: `swift test --package-path launcher --scratch-path
  local/build/app01-core-error-codes --filter CoreClientTests` — exit 0, 13 tests passed. Log
  `local/validation/app01-core-error-codes/baseline.log`, SHA256
  `2d8ab37ead356940cc36000c9e52ad72b13465d459f04a2f15f8c4f7b745195a`.
- New-suite command: `swift test --package-path launcher --scratch-path
  local/build/app01-core-error-codes --filter CoreErrorCodeMappingTests` — exit 0, 1 test passed. Log
  `local/validation/app01-core-error-codes/new-suite.log`, SHA256
  `9c6ddb3528da09c8693456084e4f37f047b2a7091045b5212daabb48ea3d2072`.
- Root replay command: `swift test --package-path launcher --scratch-path
  local/build/app01-core-error-codes --filter CoreClientTests` — exit 0, 13 tests passed. Log
  `local/validation/app01-core-error-codes/root-replay.log`, SHA256
  `43dd856e39c0f0f88ed047bc63ea69d8c4fe15b25a33246ae84eeaa53eba7836`.
- Final test SHA256 `8c20d1cd062e1b449cde25de12302cc80279e7a46ef3b2265f3489fc75f9c234`; unchanged
  `CoreModels.swift` SHA256 `183d681e8566cf2d8c19b2a484f6e4163869ec3eb34c231f92ed91314b824eda`; unchanged
  `sotc_core.h` SHA256 `6b29da35ffb1a6bfb8fa58a5e1a06620285591eb733b166c7cde8232bfc8dac0`.
- Observation: numeric values 0–8 map to `.ok`, `.invalidArgument`, `.badState`, `.assetsNotReady`,
  `.engineNotImplemented`, `.io`, `.bufferTooSmall`, `.abiMismatch`, and `.internalError`. Values 9
  and `UInt32.max` map to `.unknown` preserving the original value. New and existing focused suites
  pass. This is a pure decoder fixture against current declarations, not a live ABI transport test.
- Remaining risks: the native core is still a stub; no live ABI, engine, launcher runtime, or game
  behavior is demonstrated. User-owned native API changes remain uncommitted and excluded.
- Next exact action: claim `APP-01-SYSTEM-CHECKS-REJECT-32BIT-MACHO-01`, then add and run its synthetic
  i386 classification fixture if the live task board remains clear.
