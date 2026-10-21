# Round 66 bounded fixture claims

Checkpoint: 2026-10-21 15:06 CDT (America/Chicago)
Integrator: `/root` — Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`
Task: claim three independent fixture-only tasks before implementation.

## Candidates and bounded ownership

| Task / owner | Existing file and exact planned assertion | Parent | Claim-time digests |
|---|---|---|---|
| `APP-01-SYSTEM-CHECKS-CORE-NONEXECUTABLE-FILE-FIXTURE-01` / `/root/launcher_review` | In `AssetValidatorTests.swift`, create a temporary regular `SoCGameCore` file, set POSIX mode explicitly to `0644`, and assert `SystemChecks.nativeCore(executable:)` returns `.notBuilt` with exact message `NOT BUILT: SoCGameCore is not executable. Play disabled until a native SoCGameCore exists`. Only edit this existing test file. | `APP-01`; existing core-readiness tests | Test `launcher/Tests/LauncherCoreTests/AssetValidatorTests.swift`: `b5d61ccad0b50131c6ebbca85d3bebd3d70884b6dbe952358bb4b59a520ee7b4`. Production `launcher/Sources/LauncherCore/SystemChecks.swift`: `cf3d6ce5dd5663b10291c5c53169aecb2ef02bc7912960e29af2789e60a3b0d8`. |
| `RE-02-DYNLINK-SYMBOL-NAME-OFFSET-BOUNDS-FIXTURE-01` / `/root/review_host_proto` | In `dynamic_import_elf_fixture()`, mutate symbol index 1's 32-bit `st_name` field to equal `DT_SCE_STRSZ`, keep the string table unchanged, and assert exact `ValueError` text `out of bounds: {strsz:#x}+0x1 / {strsz:#x}`. Only edit this existing test file. | `RE-02-DYNLINK-01` | Test `scripts/test_ps4_dynlink.py`: `84926a6eb368fd9f86e1c034b89e968f9af6a1f48e3ded85807133faa55fffbf`. Production `scripts/ps4_dynlink.py`: `28c53257a6f2bd58522f123098d41ca0d884222330a1bf18ce9f8935fb45eff9`. |
| `SRV-01-ARENA-HIGH-WATER-COUNTER-PAIR-INDEPENDENCE-FIXTURE-01` / `/root/slot23_raw_xrefs` | In `test_arena_high_water.cpp`, seed used/peak pairs at offsets `+0x90/+0x98` with `0x100/0x105` and `+0x80/+0x88` with `0x200/0x300`; call `arena_add_used_high_water(arena, 0x10)`; assert resulting pairs are `0x110/0x110` and `0x210/0x300`. Only edit this existing test file. | `SRV-01` | Test `native/tests/test_arena_high_water.cpp`: `18fcb987050b87f924b43482fbde7ed62474fbf8688e12e09902918dac1bf5c9`. Production `native/core/arena_high_water.cpp`: `d3c489cff878f3c2c64ebc95a9abaa1d2aa4d20aceac08669784a8079605b213`; header `native/core/arena_high_water.hpp`: `beb7cc7eb1540c7f5f910f0289b7fcb3052b266928b118b629ced1aaf3597eba`. |

Only the three listed existing test files are assigned; no production code or build files may change,
and workers may not commit. Root owns all shared documentation and replay logs. No task has an
implementation attempt or failed try yet. The candidate IDs were checked against `docs/TASKS.md`
and `docs/EVIDENCE.md` and were absent.

## Graft and live-source checks

Root used Graft before source inspection. Queries covered the implemented `SystemChecks.nativeCore`
file-executable guard and adjacent tests; PS4 dynlink `parse()` string-table bounds, the existing
unterminated-name fixture, and `dynamic_import_elf_fixture()` layout; and `arena_add_used_high_water`
with the current native fixture. Relevant live spans: `SystemChecks.swift:L31-L55`,
`AssetValidatorTests.swift:L71-L97`, `ps4_dynlink.py:L27-L30` and `L74-L124`,
`test_ps4_dynlink.py:L47-L123` and `L190-L201`, `arena_high_water.cpp:L30-L35`, and
`test_arena_high_water.cpp:L26-L67`. Scouts separately checked task/evidence registries and
reported these single-file boundaries.

Observed behavior: `nativeCore` checks `FileManager.isExecutableFile(atPath:)` before Mach-O
inspection; `parse()` routes each symbol name through a bounded string-table span check; and the
arena helper updates `+0x90/+0x98` and `+0x80/+0x88` through separate calls. These are the behavior
targets for synthetic regression fixtures. No real executable, original ELF, allocator mapping,
or game behavior is claimed.

## Focused baseline runs

| Task | Exact command and result | Root log and SHA-256 |
|---|---|---|
| APP | `swift test --package-path launcher --scratch-path local/build/launcher --filter AssetValidatorTests`; exit 0, 9 tests in 1 suite. | `local/validation/root-round66-scout/app01-system-checks-nonexecutable/baseline.log` — `60cb5ff82ceca0365cc23e73c3b65efe667393d5bb04efb707e23695390c8132` |
| RE | `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_ps4_dynlink.py`; exit 0, 22 tests. | `local/validation/root-round66-scout/re02-dynlink-name-offset/baseline.log` — `5b04ea09c0319911715acdac442b8d8f4a06920fd3eeb2892aea92d3bfddf09c` |
| SRV build | `cmake --build local/build/native --target test_arena_high_water`; exit 0. | `local/validation/root-round66-scout/srv01-arena-counter-pair/build.log` — `77e4505908cd48ed3528c6226ab924f2fe4268ae3bbde75d402114a9e0b12329` |
| SRV test | `ctest --test-dir local/build/native -R '^arena_high_water$'`; exit 0, 1/1 passed. | `local/validation/root-round66-scout/srv01-arena-counter-pair/ctest.log` — `f72d4752e9bf2bed44feb87f43a5b038fe1d5c8e9c6a326ca86af3bcc9129a9f` |

The fixtures establish only temporary-file readiness classification, synthetic string-table offset
validation, and synthetic counter arithmetic. They do not demonstrate cold boot, actual game audio,
real save/reload, Metal rendering, or any G0–G6 runtime gate. Existing user-owned
`native/api/CMakeLists.txt` and `native/api/host/` remain outside the task.

## Pre-publication checks

- `trail build --check` exited 0 and reports the 3,562-node wiring graph is current; optional
  meaning layer is unbuilt. Log `local/validation/root-round66-scout/trail-check.log`, SHA-256
  `7272b1c7540b89c4719537381397fa04a6dea83c0062f4a37015dece476ff497`.
- `python3 scripts/project_index.py` exited 0 and indexed 14,458 folders; log
  `local/validation/root-round66-scout/project-index.log`, SHA-256
  `3e77876fa27a3c1df46b37f73c98fb06452d949b4b264425671caa9020b490d3`.
- `python3 scripts/project_index.py --check` exited 0 and reported current; log
  `local/validation/root-round66-scout/project-index-check.log`, SHA-256
  `70bdd6fb6debdd05a49cdf519242d4392d4e1cf0a702497d66d0f89fe6c68b48`.
- `python3 -I scripts/audit_publish_paths.py` accepted the 4 staged allowlisted claim paths; log
  `local/validation/root-round66-scout/audit-staged.log`, SHA-256
  `9fe1ed915e72cc744e0a311c0775b3092657f51f566bce2bb64a1a2018d4519c`.
- `python3 -I scripts/audit_publish_paths.py --all` accepted 1,636 paths; log
  `local/validation/root-round66-scout/audit-all.log`, SHA-256
  `23c13e13762c46c1f000e92423c0de34a9ca799290f2544a2fb328d84bd46427`.
- `git diff --cached --check` and `git diff --check` exited 0 with empty output; logs
  `local/validation/root-round66-scout/diff-check-cached.log` and
  `diff-check-working.log`, each SHA-256
  `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`.
- `git fetch origin main` exited 0; log `local/validation/root-round66-scout/remote-ref.log`,
  SHA-256 `58e4bb2ebd9bd713bf3332d4cad1251aeb0b51fb26a8b06f30b3aaa431df79b3`.
- `docs/FOLDERS.md` was regenerated after the new checkpoint and private Round66 validation paths
  existed. `INSTALL_LOG.md` is unchanged; no dependencies or installs changed.
- Before claim publication, `HEAD == origin/main == 0a7f5b0671c5f878edf15569bd409de099265c02`.
  The user-owned `native/api/CMakeLists.txt` modification and untracked `native/api/host/` remain
  unstaged and excluded.

## Task status and next action

All three rows are `ACTIVE`; attempts: 0 implementation and 0 failed for each. Exact next action:
the claim commit has been pushed and matched `origin/main`; dispatch each worker its exact one-file
task and fixed private log path after publishing this post-push record.

## Claim publication

- `git commit -m "docs: claim Round66 launcher, dynlink, arena fixtures"` exited 0 and created
  `9bb85cc4299134fb230165a70891dfc135034aa1`. Commit output
  `local/validation/root-round66-scout/commit.log`, SHA-256
  `4d8b0f22b2b7035890cda5af39335b5f487763b1e7dd1be9a99607d62091f657`.
- `git push origin main` exited 0; push log `local/validation/root-round66-scout/push.log`, SHA-256
  `ccf0884f7dfa7271f94d20bb26005c6d70999a63bad49a1d55e245d72901fb09`.
- Post-push `HEAD == origin/main == 9bb85cc4299134fb230165a70891dfc135034aa1`; worktree status
  retains only the user-owned `native/api/CMakeLists.txt` modification and untracked `native/api/host/`.
  State log `local/validation/root-round66-scout/state.log`, SHA-256
  `463e5fd3e38487ff1738ad0b900f55ca480b229461c59e15e54f93a0de74ccec`.
- No implementation prompt has been sent yet. Exact next action: publish this post-push record and
  then dispatch the three scoped workers.
