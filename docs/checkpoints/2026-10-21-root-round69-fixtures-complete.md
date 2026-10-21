# Round 69 fixtures verified

**Checkpoint:** 2026-10-21 16:06 CDT (America/Chicago)
**Integrator:** `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`)
**Task status:** Three test-only tasks verified; each has 1 successful implementation attempt and 0 failed tries.
**Starting revision:** `e669b7c35dd0fa155e962a57957a5da686be87e2` (`HEAD == origin/main` after claim publication).
**Files changed:** Three existing test files, `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`,
`docs/FOLDERS.md`, the Round 69 claim checkpoint, and this completion checkpoint.

The three task claims were published before implementation: claim commit
`4fabeb0ebbc759a3d9bec424e8f39d48893fc83e`, followed by claim-document sync
`e669b7c35dd0fa155e962a57957a5da686be87e2`. Before the completion commit, `git fetch origin`
exited 0; `HEAD == origin/main == e669b7c35dd0fa155e962a57957a5da686be87e2`, with divergence `0 0`.
The fetch/ref record is `local/validation/root-round69-scout/completion-fetch.log`, SHA-256
`5e6746cc2b6ea420d86418571fe2f55cb9e710f1f85f67d68b78d01694b433ad`.

## Completed tasks

| Task | Test-only change | Worker and independent root result |
|---|---|---|
| `APP-01-TERMINATION-REQUESTED-STOP-CLEAN-EXIT-PRECEDENCE-FIXTURE-01` | In the requested-Stop classifier fixture, assert that `.exit` with status `0` still maps to `.requestedStop`, retaining the signal case. | `swift test --package-path launcher --scratch-path local/build/launcher --filter LauncherLifecycleTests` — worker and root each exit 0; **11 tests across 2 suites**. Worker log `local/validation/root-round69-scout/app-worker-attempt-1.log`, SHA-256 `a00a406024ec7485cd6105c0ed65d387a5b9d432f7a289636cc740cd426da34a`; root log `app-root-replay.log`, SHA-256 `515a703a8fafa74f9f6a06a391e56fc387a843f886386c6b0498cc52b10b887b`. |
| `RE-02-DYNLINK-ELF-MAGIC-BYTE-FIXTURE-01` | Add a wrong-magic byte to the otherwise-valid synthetic ELF identity cases and assert exact `ValueError("expected little-endian x86-64 ELF")`. | `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_ps4_dynlink.py` — worker and root each exit 0; **24 tests**. Worker and root logs are byte-identical at `local/validation/root-round69-scout/re-worker-attempt-1.log` and `re-root-replay.log`, SHA-256 `84a7ab403af2322bcfe6ed0618c774e421b8a5f5727f8ebb8e27ca6605d61cd1`. |
| `DATA-02-PSARC-PREFIX-READ-TRUNCATED-TAIL-01` | Truncate the last compressed block of a synthetic 48-byte member; a 5-byte prefix read succeeds and a full read raises `PsarcError("truncated block")`. | `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_psarc.py` — worker and root each exit 0; **36 tests**. Worker log `local/validation/root-round69-scout/data-worker-attempt-1.log`, SHA-256 `52266fe37b604d668df03f8619c2e8e4c4a38159cba833d92afb6e521c6074df`; root log `data-root-replay.log`, SHA-256 `79397a6199dd7efb9f454f96e92b80c58ef342776f65d90fe6fa49770a68a2e4`. |

## Source and test identity

- APP test `launcher/Tests/LauncherCoreTests/LauncherLifecycleTests.swift` SHA-256
  `78c5910e0a014e290feedc4859a3e2b36126faef70cbc7483929cf178fc49a01`; unchanged production
  `launcher/Sources/LauncherCore/LauncherLifecycle.swift` SHA-256
  `358e78ae1de70957fd01100e9969916ab4c6c81a0e52365484251087d8c248c4`.
- RE test `scripts/test_ps4_dynlink.py` SHA-256
  `f7f8b8415ecede7f957d69b369a8565596383fed1dec3f99d99bc496d46ec952`; unchanged production
  `scripts/ps4_dynlink.py` SHA-256
  `28c53257a6f2bd58522f123098d41ca0d884222330a1bf18ce9f8935fb45eff9`.
- DATA test `scripts/test_psarc.py` SHA-256
  `23dc33b3e06ed08fc13139a5691232f7a95492ce7d1034f924449c44fcb8bd43`; unchanged production
  `scripts/psarc.py` SHA-256
  `39f2c748db013637aba8144205dc60b7e5b1eb49ee0f641960783a1e2c140485`.

## Index and validation preflight

- `python3 -I scripts/project_index.py` exited 0 and indexed 14,464 folders;
  `python3 -I scripts/project_index.py --check` exited 0 and reported current. Logs
  `local/validation/root-round69-scout/completion-project-index.log` and
  `completion-project-index-check.log` have SHA-256
  `d50f1e79e0646836436a43040406e5720bad32092b6cfd2c86a8278065bfe400` and
  `d5e6cc672715890d5e489357e2e1b56a23d94cb7fa5cb5bd68717f5613fe72bd`.
- `trail build --follow-nested-repos --only-dir scripts --only-dir tools --only-dir native
  --only-dir launcher --only-dir reference` exited 0 and refreshed the wiring graph to 3,568
  nodes, 4,829 edges, and 943 cards. `trail build --check` exited 0 and reports the graph in sync.
  Logs `completion-trail-build.log` and `completion-trail-check.log` have SHA-256
  `7ca302b5be0f3b1b3ab9e448ca4d7ed53ba33b1eb4a48473d4272ec2fa14efd2` and
  `3290230a999a0955146a4ad9bf1bd358fd2e4177737331da17d29ca7cf9ae37e`. The optional deep
  meaning layer remains unbuilt.

Root reviewed each diff: APP adds only the clean-exit tuple assertion; RE adds only the corrupted
magic identity case; DATA adds only the two prefix/full-read assertions for the truncated-tail
fixture. No production file changed. `git diff --check` passes. No install or dependency facts
changed; `INSTALL_LOG.md` was not edited.

## Final prepublication checks

- `git fetch origin` exited 0. `HEAD == origin/main == e669b7c35dd0fa155e962a57957a5da686be87e2`;
  `git rev-list --left-right --count HEAD...origin/main` reports `0 0`. The record is
  `local/validation/root-round69-scout/completion-publish-state.log`, SHA-256
  `6abf56c1b4e4c911971ba7339c80b94348bbf2805b9d38b84cb48a64cee70afd`.
- `python3 -I scripts/audit_publish_paths.py` exited 0 for the 9 explicitly staged allowlisted
  paths. `python3 -I scripts/audit_publish_paths.py --all` exited 0. Both commands are silent on
  success; their empty logs have SHA-256
  `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`.
- `git diff --cached --check` and `git diff --check` both exited 0 with no output. The unstaged
  `native/api/CMakeLists.txt` modification and untracked `native/api/host/` remain excluded.
- Local date/time and integrator: 2026-10-21 16:06 CDT, `/root`; task status: all three tasks
  `FIXTURE_VERIFIED`, each with 1 successful implementation attempt and 0 failed tries. The
  observations and limits above are unchanged; G0 remains PARTIAL and G1–G6 remain NOT RUN.

## Evidence limits and remaining risks

Observations: all six worker/root focused runs passed; all three production hashes match the claim
baselines; test changes remain in their assigned files. Inference: these fixtures pin deterministic
behavior already present in the pure lifecycle classifier and synthetic parsers. They establish no
live child-process timing, executable loading, relocation behavior, real archive compatibility,
audio, allocator integration, or game runtime. G0 remains PARTIAL; G1–G6 remain NOT RUN. There is no
native game executable, cold boot, actual game audio, real save/reload, or Metal gameplay evidence.

The region-list scout found no untested insertion/unlink case. A suggested nonaligned free-list
page-budget assertion was deferred because the helper has no production callers and E164 already
covers related partial-page arithmetic; no unsupported direct-input behavior was registered. The
PSARC invalid-UTF-8 exception preference remains pending and that candidate is not part of this
batch. Existing user-owned `native/api/CMakeLists.txt` and untracked `native/api/host/` remain
untouched and excluded from publication.

**Next exact action:** Commit and push the allowlisted Round 69 files, verify `HEAD == origin/main`,
then scout the next independent batch.
