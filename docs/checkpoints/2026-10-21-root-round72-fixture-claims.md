# Round 72 fixture claims

Claim checkpoint time: 2026-10-21 16:37 CDT (America/Chicago). Integrator: `/root` (Codex desktop
task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`). Claim base: commit
`6e517b91a849c48a2775edce346c9539abb8c7e6` (`origin/main`).

Three independent bounded test-only tasks are claimed before implementation. No code/test file has
been edited for these claims. `/root` owns `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, the
project index, and checkpoints; each worker owns only the one test file listed below.

## Claims

| Task | Owned file | Hypothesis and acceptance | Status / attempts |
|---|---|---|---|
| `DATA-02-PSARC-EMPTY-MEMBER-READ-FIXTURE-01` | `scripts/test_psarc.py` | The existing synthetic roundtrip archive contains `('z', b'')`; add `self.assertEqual(a.read(3), b'')` to pin an unbounded read of a zero-size member. This differs from a zero-prefix read of a nonempty member and from manifest-only zero-size coverage. | ACTIVE; 0 failed tries |
| `APP-01-CORE-DIAGNOSTICS-STOPPING-STATE-FIXTURE-01` | `launcher/Tests/LauncherCoreTests/LauncherModelTests.swift` | A synthetic `CoreStatus(state: .stopping, lastError: .ok, assetsValidated: false, assetsReady: false, engineImplemented: false, ctor: .notLinked)` should produce the exact row `  Core state: stopping; last error: OK`. This is formatter output only. | ACTIVE; 0 failed tries |
| `SRV-01-FREE-LIST-CARVE-LEAD-EXCEEDS-FREE-SIZE-FIXTURE-01` | `native/tests/test_free_list_carve.cpp` | `try_carve_fit(0x20001000, 0x1000, 1, 0x10000)` should return no fit: alignment produces lead `0xf000`, greater than free size `0x1000`, while request size itself fits. This pins the pure helper's bound check only. | ACTIVE; 0 failed tries |

Graft/Trail scouts identified the target spans, current coverage, and adjacent task IDs; root verified
the live code/test state and ran fresh baselines. No matching task/evidence IDs were found. No user
choice is needed because each task pins one existing, explicit synthetic/helper contract.

## Baselines and claim-time hashes

| Scope | Exact command | Result / log |
|---|---|---|
| PSARC | `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_psarc.py` | Exit 0; 36 tests. `local/validation/root-round72-scout/psarc-baseline.log`, SHA-256 `7de6e7ef645548fd64e1f5519031fde8b3621d15c98ca997d8f107920596cb80`. |
| Launcher diagnostics | `swift test --package-path launcher --scratch-path local/build/launcher --filter CoreReadinessTests` | Exit 0; 3 tests in 1 suite. `local/validation/root-round72-scout/launcher-baseline.log`, SHA-256 `b2737773c170f7e55b098a8fe706b023730cd43bb7a602108dd12f5ae1468ea6`. |
| Free-list carve | `cmake --build local/build/native --target test_free_list_carve`; then `ctest --test-dir local/build/native -R '^free_list_carve$'` | Build exit 0; focused CTest exit 0, 1/1 passed. Logs `free-list-carve-baseline-build.log` SHA-256 `864e6f0bdf507d73c1178c9078acbe53c24725d2383621db31416c2de9c14544` and `free-list-carve-baseline-ctest.log` SHA-256 `3e68d0037317133fde230ae08fb257e72e357a0ff6f445f77dfbc03a148bf9f9`. |

Claim-time hashes:

- `scripts/psarc.py`: `39f2c748db013637aba8144205dc60b7e5b1eb49ee0f641960783a1e2c140485`.
- `scripts/test_psarc.py`: `23dc33b3e06ed08fc13139a5691232f7a95492ce7d1034f924449c44fcb8bd43`.
- `launcher/Sources/LauncherCore/CoreReadiness.swift`: `fea0fec1c0635b8d6ab3978db2a817a1dd47d0a8d87045aada68f820ce64e2eb`.
- `launcher/Tests/LauncherCoreTests/LauncherModelTests.swift`: `24ce0317362a36bf95106f59e4c58a48439d91489105e081ffe4ffb4378cfb98`.
- `native/core/free_list_carve.cpp`: `a5d0d660cb49b73499c27b42724afc6d57a8735a01ce07017d11599b3e8b1630`.
- `native/tests/test_free_list_carve.cpp`: `58c7f6a88eea2801050b00a345ea5b9478be9823be51317a7ce263782fed2f56`.

## Limits, risks, and next action

The assertions cover one synthetic PSARC member, one pure diagnostic formatter value, and scalar
carve-fit math. They establish no real game archive behavior, live status polling/UI, original
allocator integration, executable behavior, game boot/audio/save, or Metal rendering. G0 remains
PARTIAL; G1–G6 remain NOT RUN. No install/dependency facts changed. Existing user-owned
`native/api/CMakeLists.txt` and `native/api/host/` remain untouched and excluded.

## Claim publication checkpoint

At 2026-10-21 16:39 CDT, claim docs and the Round71 checkpoint sync were published as
`df1af7d60f48c4b51222f6fb7705f93fa7eaae1c` (`docs: claim Round 72 fixture tasks`). Before commit,
`git diff --cached --check`, the staged path audit (exactly the five allowlisted docs paths shown
above in the commit), and the full path audit (1,648 allowlisted paths) passed. The claim commit and
push logs are `local/validation/root-round72-scout/claim-commit.log` (SHA-256
`a095f0a223c0c6a8b75597dacc4b67b8afbfc435fee514853f4646a8b4ae928c`) and
`claim-push.log` (SHA-256 `5d98261ae24b6c0582404f99eaa365a55e2d3f8b0d11e0ae014da08544f44de2`).
Project index regeneration and check both passed at 14,467 folders; logs are
`claim-index-generate.log` (SHA-256
`1c39b93eb3985f6f6f4972ccd3fcdf293b5ee79108366233397a5adc813ce328`) and
`claim-index-check.log` (SHA-256
`91c2a7d757519063ca3a43308f042f15372887ed7c2e1641a11ae64b1c789561`). After push,
`HEAD == origin/main == df1af7d60f48c4b51222f6fb7705f93fa7eaae1c` (divergence 0/0); exact command
`git rev-parse HEAD origin/main`. These observations verify only documentation publication and path
hygiene, not any fixture implementation or game acceptance gate.

## Dispatch checkpoint

At 2026-10-21 16:40 CDT, `/root` dispatched the three independently claimed single-file edits with
`collaboration.followup_task`: `review_host_proto` owns only `scripts/test_psarc.py`,
`launcher_review` owns only `launcher/Tests/LauncherCoreTests/LauncherModelTests.swift`, and
`round68_native_scout` owns only `native/tests/test_free_list_carve.cpp`. `collaboration.list_agents`
reported all three workers running. The shared-worktree snapshot `git status --short` showed only the
pre-existing user-owned `native/api/CMakeLists.txt` modification and untracked `native/api/host/`;
no worker file edit had landed at that observation. Each worker was instructed not to change docs,
stage, commit, or touch `native/api`, and to save its exact command output under
`local/validation/root-round72-scout/`. This confirms dispatch only; results and tests are pending.

Before dispatch, docs sync commit `963adb58cfbaab654d1d11b663cbfb6405768073` was pushed after the
Round72 claim commit. It records the publication details above. `python3 -I scripts/project_index.py`
and `--check` passed at 14,467 folders; `trail build --check` reported the wiring graph current at
3,572 nodes (see private log
`local/validation/root-round72-scout/sync-trail-check.log`, SHA-256
`99342face5580b625b2480268e4e5b63ecfebc573b79c791cf004539b078027b`). Staged audit, full audit, and
cached diff check passed. Logs: `sync-commit.log` SHA-256
`11cc170168ac17d3c8eb1ed4191ba796f8cefe44af9e7dc1119d754ff1be9e18`, `sync-push.log` SHA-256
`d697185a03e41faf7e03ff77c3859b73a0ebc0656dcba3449a7eac5327599122`, index generation/check hashes
are in the publication section, staged audit `sync-audit-staged.log` SHA-256
`090e018131e0b80e555a8ea5ccea066dd5b31a603e9cf3693b69bbd81b7acc39`, full audit
`sync-audit-all.log` SHA-256
`05190addd752e71e9bc5b389b24638b0796e5dfe1719c2a26a87e222b4ae6993`, diff check was empty (SHA-256
`e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`). After push,
`HEAD == origin/main == 963adb58cfbaab654d1d11b663cbfb6405768073`, divergence 0/0. State log
`sync-state.log` SHA-256 `bafebe0544ec1663fc5c60d5555079052f1efbe617d7957be47744c6262d25c7`.

## Implementation and independent replay checkpoint

At 2026-10-21 16:41 CDT, `/root` reviewed each worker diff and independently reran the targeted
checks. All three edits match their claims, touch only the assigned test file, and leave production
hashes unchanged. Each task has one successful implementation attempt and zero failed tries.

| Task | Worker and root result | Hashes and scope |
|---|---|---|
| `DATA-02-PSARC-EMPTY-MEMBER-READ-FIXTURE-01` | Exact assertion `self.assertEqual(a.read(3), b'')`. Worker and root ran `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_psarc.py`; exit 0, 36 tests each. | Worker/root logs `local/validation/root-round72-scout/psarc-worker.log` SHA-256 `1209b09247e7955d5dde23ffba55c0b27be4f05d191acd5f46c82a59e0e20446`, `psarc-root.log` SHA-256 `55f0679f8ab195999d310c536f1ab8e01ba88b6408d3359e7a36c827845c1e3e`. Test SHA-256 `1eda5a3ff072729536afad21da53fa8267f5774589c63a550378c6aab40e0816`; parser unchanged `39f2c748db013637aba8144205dc60b7e5b1eb49ee0f641960783a1e2c140485`. Synthetic archive fixture only. |
| `APP-01-CORE-DIAGNOSTICS-STOPPING-STATE-FIXTURE-01` | Synthetic `.stopping`/`.ok` status asserts exact `  Core state: stopping; last error: OK` output. Worker and root ran `swift test --package-path launcher --scratch-path local/build/launcher --filter CoreReadinessTests`; exit 0, 3 tests each. | Worker/root logs `launcher-worker.log` SHA-256 `2d0963e47a89fc98bd132ce9e95b9fd54c59690ecc2fc8d62a1ca1613188a3c1`, `launcher-root.log` SHA-256 `2a29a65efab7b27f42494e59d51f612ce310e7d0f25757a86e33cf0ab609bf8d`. Test SHA-256 `66e8f7c7f98465d3845cd038966934da5c32025371304c5ec6e24b4ad2a80c6a`; production unchanged `fea0fec1c0635b8d6ab3978db2a817a1dd47d0a8d87045aada68f820ce64e2eb`. Pure formatter output only. |
| `SRV-01-FREE-LIST-CARVE-LEAD-EXCEEDS-FREE-SIZE-FIXTURE-01` | Added the claimed `lead=0xf000 > free_size=0x1000` rejection. Worker/root each ran `cmake --build local/build/native --target test_free_list_carve` and `ctest --test-dir local/build/native -R '^free_list_carve$'`; both builds exit 0 and both CTest runs pass 1/1. | Worker build/CTest logs SHA-256 `2a8cc4e37fa84a48b9e6ffe94aa21b4c89362d2eaad928ebc97d7e99046f6ee7` / `cd9d560f619a65f7982878b1d49c44d2beb7cb0a0c8a32be4e083ed97dac7ea2`; root logs `864e6f0bdf507d73c1178c9078acbe53c24725d2383621db31416c2de9c14544` / `3e68d0037317133fde230ae08fb257e72e357a0ff6f445f77dfbc03a148bf9f9`. Test SHA-256 `359ca51ecb8362097a687a640e2307c39d1caf4bd7898b57f1706fe3af913c64`; production unchanged `a5d0d660cb49b73499c27b42724afc6d57a8735a01ce07017d11599b3e8b1630`. Pure helper arithmetic only. |

Root `git diff --check` passed. The tests establish only synthetic archive, formatter, and carve-fit
helper behavior. They do not establish real game archives, launcher runtime, allocator integration,
game boot/audio/save-load, or Metal rendering. G0 remains PARTIAL; G1–G6 remain NOT RUN. No install
or dependency facts changed. `native/api/CMakeLists.txt` and `native/api/host/` remain user-owned and
untouched.

Status: all three tasks are `FIXTURE_VERIFIED`; one successful implementation attempt and zero failed
tries each.

## Index and source-graph checkpoint

At 2026-10-21 16:43 CDT, `/root` added the completion checkpoint, regenerated and checked the project
index, rebuilt the Trail graph, and checked it. The folder index is current at 14,467 folders. Trail
reports 3,572 nodes, 4,837 edges, and 943 cards; the wiring graph is current and the optional deep
meaning layer remains unbuilt. The first `project_index.py --check` failed with `Stale indexes:
local/indexes/folders.json` because the check log was opened after generation. The failure was
preserved in `complete-index-check-initial-failed.log` (SHA-256
`31dad00ffd5208458ee89a329fea70b0fa13494e525ddd8a7e837c7614a2266b`). Precreating all private log
paths, then running `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/project_index.py` and
`PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/project_index.py --check`, both exited 0. This was an
index freshness issue only, not a failed fixture attempt. Generation/check log SHA-256s:
`1c39b93eb3985f6f6f4972ccd3fcdf293b5ee79108366233397a5adc813ce328` /
`91c2a7d757519063ca3a43308f042f15372887ed7c2e1641a11ae64b1c789561`. `trail build` and
`trail build --check` both exited 0; logs `complete-trail-build.log` SHA-256
`93fe515a6cb08377da9ce861ea8fd7f0786aec7a7ae6f9f647ccaf7b2b6bce44`, `complete-trail-check.log`
SHA-256 `99342face5580b625b2480268e4e5b63ecfebc573b79c791cf004539b078027b`. Root diff-check log
`complete-diff-check.log` is empty (SHA-256
`e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`).

Next exact action: run staged/full path audits, commit and push the three test files, E216–E218, task
status, handoff, generated index, and this completion checkpoint. Then scout the next independent
bounded tasks.
