# Round 65 bounded fixture claims

Checkpoint: 2026-10-21 14:56 CDT (America/Chicago)
Integrator: `/root`
Task: claim three independent test-only fixtures
Files changed at claim: `docs/TASKS.md`, `HANDOFF.md`, this checkpoint, and generated
`docs/FOLDERS.md` after project indexing. No test or production source is edited.

## Candidate claims

Root used Graft to locate each existing test and production behavior, checked the task/evidence
registries for duplicate IDs, verified the task prerequisites, and independently ran each focused
baseline. The live files are the claim-time source of truth.

| Task / owner | Existing file and exact assertion | Parent / focused command | Claim-time digests |
|---|---|---|---|
| `APP-01-CORE-ABI-SAME-MAJOR-MINOR-ORDER-FIXTURE-01` / `/root/launcher_review` | Add to `abiVersionPacksAndOrders()` the assertion `CoreABIVersion(major: 1, minor: 2) < CoreABIVersion(major: 1, minor: 3)`. The current assertion compares different major values, so this covers the same-major minor ordering branch. | `APP-01-CORE-ABI-01`; `swift test --package-path launcher --scratch-path local/build/launcher --filter CoreClientTests` | Test `launcher/Tests/LauncherCoreTests/CoreClientTests.swift`: `4a195ab23e77acaf3307a6d3beac16b71cfe96427711848cac37632bb4df1bd0`. Production `launcher/Sources/LauncherCore/CoreModels.swift`: `183d681e8566cf2d8c19b2a484f6e4163869ec3eb34c231f92ed91314b824eda`. |
| `DATA-02-PSARC-TRAILING-NUL-TERMINATOR-FIXTURE-01` / `/root/review_host_proto` | Add a synthetic member with `manifest_paths=['one.bin', '']`; assert `archive.read(0) == b'one.bin\0'` and `archive.paths() == ['one.bin']`. This exercises the existing split/filter behavior without claiming a real archive convention. | `DATA-02`; `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_psarc.py` | Test `scripts/test_psarc.py`: `90cabb77649f9607409705f47dc92ef76bbc7dc081c053bda694a0dc00bb8794`. Production `scripts/psarc.py`: `39f2c748db013637aba8144205dc60b7e5b1eb49ee0f641960783a1e2c140485`. |
| `SRV-01-ALLOCATOR-REGION-MAX-ENDPOINT-FIXTURE-01` / `/root/slot23_raw_xrefs` | Add `auto end = make_region_view(~0ull - 15, 15); CHECK(end && end->end() == ~0ull);` alongside existing valid/zero/overflow region cases. The helper accepts a non-wrapping `start + size` exactly equal to the maximum unsigned endpoint. | `SRV-01`; `cmake --build local/build/native --target test_allocator_region`; `ctest --test-dir local/build/native -R '^allocator_region$'` | Test `native/tests/test_allocator_region.cpp`: `9f5c6ea43cc9da4c47d4ca520e1f8471eefc945ea65dbdbf9bef63c7aa4f91fa`. Production `native/core/allocator_region.cpp`: `7ea6e93c79f43dfbad5bb447008a39c77136633f3afe97dde493d57dd69c9da8`. |

Only these three existing test files are delegated. Workers may not change production, shared docs,
or build configuration and may not commit. Root owns review, independent replay, evidence, and
publication. All three rows are `ACTIVE` before implementation prompts. Failed tries and
implementation attempts: 0 for each.

## Baseline runs

| Task | Exact result | Root log and SHA-256 |
|---|---|---|
| APP | Exit 0; 22 tests across 2 suites (9 `NativeCoreClientTests` + 13 `CoreClientTests`). | `local/validation/root-round65-scout/appabi-same-major/baseline.log` — `d1f20b93e969554d1162cd7c3fb148e195bd68e963aeba0e3cb1565af2c2a4b2` |
| DATA | Exit 0; 34 tests, `OK`. | `local/validation/root-round65-scout/data02-psarc-terminal-nul/baseline.log` — `f4867d20c9d449119a1ba8ec4e99d65d39592d1db408ab66f3c8e07ae934c0b6` |
| SRV build | Exit 0; `test_allocator_region` target built. | `local/validation/root-round65-native-region-endpoint-scout/build.log` — `40d4c8c5d57a58d1b3e380e966efa8c1c6a87b1bcbb7f7f81bfb16abb4efc181` |
| SRV test | Exit 0; focused CTest 1/1 passed. | `local/validation/root-round65-native-region-endpoint-scout/ctest.log` — `db3f8595523db6fc5b31eb9aefa526d405f1831c9339c52335ac5a5fa830669c` |

## Graft and live task checks

Root ran:

```text
trail ask "Find existing bounded test-only gaps for open APP-01, DATA-02 or RE-02, and native support tasks in docs/TASKS.md. Prefer existing test files and deterministic assertions that exercise implemented behavior without changing production code." --source
trail ask "Locate the existing CoreABIVersion ordering implementation and its test fixture, including the same-major minor-order case." --source
trail grep "abiVersionPacksAndOrders"
trail ask "Locate allocator region creation, end-boundary arithmetic, and current native tests around zero size and uint64 overflow." --source
trail ask "Locate the PSARC fixture builder, Psarc.paths and entry read behavior for a manifest with a trailing NUL terminator." --source
```

The three exact candidate IDs were absent from `docs/TASKS.md` and `docs/EVIDENCE.md`. Existing
task cards confirm APP-01-CORE-ABI-01, DATA-02, and SRV-01 are active parents. The candidate tests
cover only pure Swift comparison, synthetic manifest parsing, and endpoint arithmetic. They do not
establish a real archive convention, actual memory mapping, native game execution, boot, audio,
save/reload, or Metal rendering.

## Claim state and publication boundary

- `python3 scripts/project_index.py` exited 0 and indexed 14,450 folders; final log
  `local/validation/root-round65-scout/project-index-final.log`, SHA-256
  `f03b0b5c677ce9f43465e4232051095307dc8ca0a226f318a95e5f3e52aee897`. Then
  `python3 scripts/project_index.py --check` exited 0 and reported current; final log
  `local/validation/root-round65-scout/project-index-check-final.log`, SHA-256
  `7078a2abc8e35bce9eab451acbbeb16cbc0852fad0a5d5ec54be6f470b214048`.
- The first `project_index.py --check` exited 1 with `Stale indexes: local/indexes/folders.json`;
  log `local/validation/root-round65-scout/project-index-check.log`, SHA-256
  `31dad00ffd5208458ee89a329fea70b0fa13494e525ddd8a7e837c7614a2266b`. Observation: the final
  index stayed stale until all new validation log paths existed. Inference, verified against
  `scripts/project_index.py`: the private JSON includes per-folder file counts, and the first
  check-log file was created after its preceding generation. After pre-creating remaining log
  destinations, rerunning generation and check passed. This was an index sequencing issue, not a
  fixture attempt; all three task attempts remain 0.
- `trail build --check` exited 0; the 3,561-node wiring graph is in sync. The optional meaning
  layer is not built. Log `local/validation/root-round65-scout/trail-check.log`, SHA-256
  `1e5c9b39c6d925a3581160c7641ef89b609c316dc4cfe0b41c653651f418a213`.
- Before claim publication, `HEAD == origin/main == 7d72af668f0d3d2cb90410b855884c0ce1e7484f`.
- Pre-existing user-owned `native/api/CMakeLists.txt` remains modified and `native/api/host/`
  remains untracked. Neither path is claimed or staged.
- No dependency, configuration, production implementation, or installer changed; `INSTALL_LOG.md`
  remains unchanged.
- The claim commit must be audited, committed, and pushed before worker prompts. Root is the sole
  owner of `HANDOFF.md` and shared documentation during these tasks.

## Claim publication preflight

At 14:56 CDT, only these four allowlisted paths were staged: `HANDOFF.md`, `docs/FOLDERS.md`,
`docs/TASKS.md`, and this claim checkpoint. Existing user-owned `native/api/CMakeLists.txt` and
untracked `native/api/host/` remain unstaged and excluded.

- `python3 -I scripts/audit_publish_paths.py` exited 0 and accepted 4 staged paths; log
  `local/validation/root-round65-scout/audit-staged.log`, SHA-256
  `1769bcaf5027ed0d2765ff26dcc71a753d88677595a047f4851dcfb09ea79145`.
- `python3 -I scripts/audit_publish_paths.py --all` exited 0 and accepted 1,634 paths; log
  `local/validation/root-round65-scout/audit-all.log`, SHA-256
  `a1223d0b4a28d54265dd10922ad8c05a9913c10acae20decbf3f42404194ba35`.
- `git diff --cached --check` and `git diff --check` exited 0 with empty outputs; both logs have
  SHA-256 `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855` at
  `local/validation/root-round65-scout/diff-check.log` and `working-diff-check.log`.
- `git fetch origin main` exited 0; log `local/validation/root-round65-scout/remote-ref.log`,
  SHA-256 `58e4bb2ebd9bd713bf3332d4cad1251aeb0b51fb26a8b06f30b3aaa431df79b3`. Immediately before
  claim publication, `HEAD == origin/main == 7d72af668f0d3d2cb90410b855884c0ce1e7484f`.

Exact next action: commit and push the four audited claim paths, verify `HEAD == origin/main`, then
dispatch the three test-only workers with their pre-created private log destinations.
