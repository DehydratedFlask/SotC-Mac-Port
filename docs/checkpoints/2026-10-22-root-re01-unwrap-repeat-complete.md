# Checkpoint: RE-01 identical-output SELF unwrap repeat fixture

- Local date/time: 2026-10-22 00:25 CDT (America/Chicago)
- Integrator / agent: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`); implementation worker `/root/next_bounded_scout`.
- Task ID: `RE-01-UNWRAP-IDENTICAL-ELF-REPEAT-FIXTURE-01`
- Status: `FIXTURE_VERIFIED`; 1 successful implementation attempt, 0 failed tries.
- Files changed: `scripts/test_project_tools.py`, `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, the claim checkpoint, this completion checkpoint, and generated `docs/FOLDERS.md`. `scripts/unwrap_self.py` and the pinned reference clone are unchanged. Existing user-owned `native/api/CMakeLists.txt` and `native/api/host/` remain out of scope.

## Change and verification

- Added `ToolChecks.test_identical_plaintext_self_repeat_is_stable`. It constructs the same synthetic plaintext SELF used by existing tests, calls `unwrap()` twice with the same source and destination, and asserts the second returned record, ELF output bytes, and identity-sidecar bytes match the first call.
- Baseline `scripts/test_project_tools.py` SHA-256: `dcf161b6994f01ac92b6abbf261298456017dd7bdfcc0daa5d26fb298aee5280`; final SHA-256: `598cc65d59fc3f4461f4c348235ff928fefdf1a1ae1a1e935276511829e4cfae`.
- Unchanged helper `scripts/unwrap_self.py` SHA-256: `165333d29aaab4fb2529a796bdaa796574402245e65579b85ecabee3784fc0d8`.
- Baseline command `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_project_tools.py`: exit 0, 10 tests; log `local/validation/20261022-root-fixture-baselines/re01-unwrap-repeat-baseline.log`, SHA-256 `811eabda5dc298da232b3c7953fbe433089645edff6f53063d91016c4136c0a2`.
- Worker ran `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_project_tools.py`: exit 0, 11 tests; log `local/validation/20261022-root-fixture-baselines/re01-unwrap-repeat-worker.log`, SHA-256 `95a6f805e0fd51b14c71539d53f6a60a6371670e512bdb12578522a8d3f1df29`.
- Root independently ran the same command: exit 0, 11 tests; log `local/validation/20261022-root-fixture-baselines/re01-unwrap-repeat-root.log`, SHA-256 `c31e811c47f992d66e20f9b41cd43f0634edac76521e17dbdc11a485fe59c061`.
- `git diff --check`: exit 0.

## Indexes and evidence boundary

- Graft command `trail ask "identical plaintext SELF repeated unwrap stable sidecar" --source --in scripts/` exits 0 and resolves the unchanged `scripts/unwrap_self.py:L10-L31` helper and the new method at `scripts/test_project_tools.py:L47-L67`; query output log SHA-256 `82bfaa9c91325778094f63b9c8ac6a2fc34cc48752ce12bde234c19d25b9308a`.
- `trail build --check` exits 0 with the wiring graph in sync at 3,610 nodes / 1,240 files. Initial and final check logs SHA-256 `67d8530e493d39dc77e5345ccbb25ba0da7701ad12b479fa479c1d964f5979e4`; the deep meaning layer is not built.
- `python3 scripts/project_index.py`: exit 0, 24,078 folders indexed; log `local/validation/20261022-root-fixture-baselines/re01-unwrap-repeat-project-index-generate-final.log`, SHA-256 `a79b0d25fced0d9c18dda3386413644761318dd915da578a4bbe36f239469b0c`.
- `python3 scripts/project_index.py --check`: exit 0, 24,078 folders current; log `local/validation/20261022-root-fixture-baselines/re01-unwrap-repeat-project-index-check-final.log`, SHA-256 `2e24f86275b84b7efc28ec198e984a6698bc3584a72c960f1b280bdaa45c1b67`.
- Observation: the second identical helper call returns the same synthetic record and rewrites bytes identical to the first ELF and identity sidecar.
- Inference and task boundary: this pins wrapper repeatability for one deterministic synthetic plaintext SELF, source path, and destination path. It does not establish real SELF behavior, executable loading, code execution, concurrency, crash recovery, or game behavior. Production code is unchanged.
- Remaining risk: no real package, loader, runtime, or game acceptance gate was exercised.

## Publication and next action

- Claim commits `8020974b0d77aca07143026ab717a2c44c9f9740` and `af40659` were pushed and fetch-verified before implementation. Completion audit, commit, push, and post-push verification will be appended after publication.
- Exact next action: review `/root/next_independent_task_scout`'s read-only candidate, verify its live file/spec/task gap, and publish a claim only if it is distinct. Leave the pending Mach-O CPU and PSARC negative-limit choices untouched.
