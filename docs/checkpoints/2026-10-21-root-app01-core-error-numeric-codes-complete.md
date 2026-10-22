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
- `python3 scripts/project_index.py` exited 0 and indexed 22,871 folders; the `--check` command also
  exited 0 and reported current. Logs are `local/validation/app01-core-error-codes/project-index.log`
  (SHA256 `8d5ab2b644ff52d743e7ec8a84f88b770674926bbac4c6b459efe11aabb8e84c`) and
  `project-index-check.log` (SHA256 `6a99a8e988f6e88561323cc056d6d9558b95a6423ea49aec4c0aee24c9e780eb`).
- Initial `trail build --check` exited 1 because the new Swift fixture was missing from the source
  graph. `trail build` exited 0 (3,604 nodes, 4,922 edges, 946 cards; 1 of 1,239 files parsed), and
  final `trail build --check` exited 0 with `graph check: OK`. The deep layer remains unbuilt. Logs
  `trail-check-initial.log`, `trail-build.log`, and `trail-check-final.log` have SHA256
  `d039704fbe88fee493d94c5ecaa9b40f544a43d2afc31ca1088be01e28f22dc2`,
  `b9f27d3de7ffadde9b0f95f66d1d2f8a3f614fb865cf636c2840d1d8fbe6906c`, and
  `c911e78de75303012722025cfeec383b0c71c0900a0647f5a9ef294ad46dee3b`.
- `git diff --cached --check` exited 0; empty-output log SHA256 is
  `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`. Staged
  `python3 -I scripts/audit_publish_paths.py` exited 0 for 8 allowed paths (log SHA256
  `b863587d7776a37c833f648970cd229dce67bf503b33c5d21441801c0dda1176`); `--all` exited 0 for 1,679
  allowed paths (log SHA256 `7dae2943f273cb747b8cb15f240151c6ae26d0b8b0269eb4bf6e57f060aecad7`).
- Pre-commit `git fetch origin main` exited 0; `git rev-list --left-right --count HEAD...origin/main`
  returned `0 0`. Commit `0ae3bcabf96cd115fdaf40cf4aa79cef6e22794e` was pushed; post-push fetch exited 0
  and divergence remained `0 0`. Push/fetch/divergence/status logs and hashes are recorded under
  `local/validation/app01-core-error-codes/`. Post-push status shows only the pre-existing user-owned
  `native/api/CMakeLists.txt` and untracked `native/api/host/` changes. The commit included the two
  fixtures and their shared documentation only.
- Remaining risks: the native core is still a stub; no live ABI, engine, launcher runtime, or game
  behavior is demonstrated. User-owned native API changes remain uncommitted and excluded.
- Next exact action: claim `APP-01-SYSTEM-CHECKS-REJECT-32BIT-MACHO-01`, then add and run its synthetic
  i386 classification fixture if the live task board remains clear.
