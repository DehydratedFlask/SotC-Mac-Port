# APP-01 archive status-code fixture — completion checkpoint

- Local date/time: 2026-10-21 23:14 CDT (America/Chicago).
- Agent: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`).
- Task: `APP-01-CORE-ARCHIVE-STATUS-CODES-FIXTURE-01`, child of `APP-01-CORE-ABI-01`.
- Status: `FIXTURE_VERIFIED`; 1 successful implementation attempt, 0 failed implementation tries.
- Files changed: `launcher/Tests/LauncherNativeCoreTests/NativeCoreClientTests.swift`,
  `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/FOLDERS.md`, and this checkpoint.
  Production mapper/header were unchanged. User-owned `native/api/CMakeLists.txt` and
  `native/api/host/` changes remained unstaged and excluded.
- Baseline command: `swift test --package-path launcher --scratch-path
  local/build/app01-core-archive-status-codes --filter NativeCoreClientTests` — exit 0, 9 tests in
  1 suite passed. Log `local/validation/app01-core-archive-status-codes/baseline.log`, SHA256
  `b69e18fba32ce2938a2787bd892604c0727e7868965085893b730762ddef312e`.
- Final command: the same focused command — exit 0, 10 tests in 1 suite passed. Log
  `local/validation/app01-core-archive-status-codes/root.log`, SHA256
  `a9b146ae9f185e97006a94e12e6dc973cd447650790d22e8dc288ddc6947e3ec`.
- Final test SHA256 `0d19847c6c94ece17113295bf97119da58d0b710ca45327e24d0cd19aee00b4b`;
  unchanged mapper SHA256 `183d681e8566cf2d8c19b2a484f6e4163869ec3eb34c231f92ed91314b824eda`;
  unchanged C header SHA256 `6b29da35ffb1a6bfb8fa58a5e1a06620285591eb733b166c7cde8232bfc8dac0`.
- Observation: `archiveStatusNumericCodesMapToAbiCases` now directly asserts codes 0–3 map to `.ok`,
  `.missing`, `.unreadable`, and `.badMagic`; 9 and `UInt32.max` map to `.unknown` with the original
  value. Both the baseline and final focused suite pass. This is a pure Swift mapping fixture against
  the current ABI declarations; it does not exercise ABI transport or archive inspection.
- `git diff --check` passed. `python3 scripts/project_index.py` exited 0 and wrote the inventory at
  21,670 folders; `python3 scripts/project_index.py --check` exited 0 and reported it current.
- The first `trail build --check` exited 1 as expected because the test method was newly added.
  `trail build` exited 0 and rebuilt the ignored local graph with 3,600 nodes, 4,916 edges, and
  945 cards (1 of 1,238 files parsed; the remainder replayed from cache). Final `trail build --check`
  exited 0 with `graph check: OK`. The deep concept layer remains unbuilt; this verifies the source
  wiring graph only. Final check output is preserved at
  `local/validation/app01-core-archive-status-codes/trail-check.log`, SHA256
  `f1d022759653a2973a680acddf3eec1178913ac9b06b7c6031deb07febcac7c2`.
- `python3 -I scripts/audit_publish_paths.py` exited 0 for the 6 staged allowlisted paths (log
  `local/validation/app01-core-archive-status-codes/staged-audit.log`, SHA256
  `902b436b1e3ab52ddd31a3b8d2fe2d6b3840f2bf55b441f831bc719272f207e6`).
  `python3 -I scripts/audit_publish_paths.py --all` exited 0 for 1,674 allowed paths (log
  `local/validation/app01-core-archive-status-codes/all-audit.log`, SHA256
  `aa31c83dbefff1546dba3a3056795f3eea3c5c7c9d8514960fe797e4ad3b0418`).
- The first project-index check exited 1 because subsequent private validation logs in the already
  existing directory made `local/indexes/folders.json` stale. Re-running `python3 scripts/project_index.py`
  exited 0 (`21,670 folders indexed; written`), and a final `python3 scripts/project_index.py --check`
  exited 0 (`21,670 folders indexed; current`). The stale-check output is at
  `local/validation/app01-core-archive-status-codes/project-index-check.log`, SHA256
  `31dad00ffd5208458ee89a329fea70b0fa13494e525ddd8a7e837c7614a2266b2`.
- `git diff --cached --check` exited 0; empty output SHA256 is recorded at
  `local/validation/app01-core-archive-status-codes/diff-check.log`, SHA256
  `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`.
- Remaining risks: the native implementation remains an incomplete stub, and no live ABI, filesystem,
  game-readiness, UI, runtime, or game behavior is demonstrated.
- Next exact action: claim and complete another independent bounded task outside PSARC and these
  launcher test files; retain the pending user choice for PSARC negative-limit behavior.
