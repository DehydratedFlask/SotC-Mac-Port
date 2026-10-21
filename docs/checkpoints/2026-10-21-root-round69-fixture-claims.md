# Round 69 fixture claims

**Checkpoint:** 2026-10-21 15:56 CDT (America/Chicago)
**Integrator:** `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`)
**Task status:** Three bounded test-only tasks claimed; 0 failed tries and 0 implementation attempts.
**Starting revision:** `c15b27a2a868fb2d4bf124e647e8c0b68f2d8daa` (`HEAD` at scout start).
**Files changed:** `HANDOFF.md`, `docs/TASKS.md`, `docs/FOLDERS.md`, this checkpoint.

## Claims and baselines

| Task | Owned file | Claimed assertion | Baseline result |
|---|---|---|---|
| `APP-01-TERMINATION-REQUESTED-STOP-CLEAN-EXIT-PRECEDENCE-FIXTURE-01` | `launcher/Tests/LauncherCoreTests/LauncherLifecycleTests.swift` | Extend the requested-Stop precedence fixture with `reason: .exit`, `status: 0`; `LauncherLifecycle.classifyTermination` must still return `.requestedStop`. Keep the existing signal case. | `swift test --package-path launcher --scratch-path local/build/launcher --filter LauncherLifecycleTests` — exit 0; 11 tests across 2 suites. Log `local/validation/root-round69-scout/app-baseline.log`, SHA-256 `96cf945658cffd5df9f624d79872d9c4c76c8255d8dfcf73bbe88a4d628310d5`. |
| `RE-02-DYNLINK-ELF-MAGIC-BYTE-FIXTURE-01` | `scripts/test_ps4_dynlink.py` | Add an otherwise-valid synthetic ELF identity with a corrupted magic byte to the program-header identity cases; assert exact `ValueError("expected little-endian x86-64 ELF")`. | `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_ps4_dynlink.py` — exit 0; 24 tests. Log `local/validation/root-round69-scout/re-baseline.log`, SHA-256 `506387472b939c6ac23436d79e196f7cbbef7d8293e44dbb633a4f129d3dcce7`. |
| `DATA-02-PSARC-PREFIX-READ-TRUNCATED-TAIL-01` | `scripts/test_psarc.py` | Build a synthetic 48-byte member and truncate its final compressed block by one byte; assert `read(1, 5) == b"A" * 5` while a full `read(1)` raises `PsarcError("truncated block")`. | `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_psarc.py` — exit 0; 35 tests. Log `local/validation/root-round69-scout/data-baseline.log`, SHA-256 `d3ada05f7287e5238de561edfc38695b4be4f33abe3317c5ea19639f20a46fe7`. |

Current test and production SHA-256 pairs at claim time:

- APP test `launcher/Tests/LauncherCoreTests/LauncherLifecycleTests.swift`:
  `30beed9c664e10d91785c89d70b3705a4cbf26a17333dbf7c0df958021a796bc`; unchanged production
  `launcher/Sources/LauncherCore/LauncherLifecycle.swift`:
  `358e78ae1de70957fd01100e9969916ab4c6c81a0e52365484251087d8c248c4`.
- RE test `scripts/test_ps4_dynlink.py`:
  `3c2d318c64ed5a0dca12faabb1a155ff3e688d9626fb50a0f49f2627d00af217`; unchanged production
  `scripts/ps4_dynlink.py`:
  `28c53257a6f2bd58522f123098d41ca0d884222330a1bf18ce9f8935fb45eff9`.
- DATA test `scripts/test_psarc.py`:
  `e873256364378ab3b98e8bc683b6441d418d68a03e662f5463bc6ef7341057fb`; unchanged production
  `scripts/psarc.py`:
  `39f2c748db013637aba8144205dc60b7e5b1eb49ee0f641960783a1e2c140485`.

## Ownership and limits

After claim publication, `/root/launcher_review`, `/root/review_host_proto`, and
`/root/round68_data_scout` each own only the named existing test file. Root owns shared documents,
diff review, independent replay, evidence updates, and publication. No production change, dependency
change, installation, or `INSTALL_LOG.md` update is proposed. The task register has no matching
task for any of the three assertions. The PSARC invalid-UTF-8 exception behavior remains excluded
pending the user's earlier preference.

These are deterministic value/parser fixtures only. The APP case does not test process-stop timing,
the RE case does not load a live ELF or execute relocations, and the DATA case does not read a real
archive. None advances G0–G6. G0 remains PARTIAL; G1–G6 remain NOT RUN. No native game boot, actual
game audio, real save/reload, or Metal gameplay is demonstrated.

**Observation:** The pre-edit baselines pass; the current production/test hashes match the live
files. The task-specific Graft queries located each source/test span. The region-list scout found
no nonredundant candidate. A second native scout proposed testing
`carve_pages_available(1, 0x10001)`, but that function has no production callers (only its test main)
and E164 already covers partial-page arithmetic in a related helper; root deferred it rather than
turning an unsupported direct input into a task contract. Its baseline build and focused CTest
passed 1/1; logs `local/validation/root-round69-scout/native2-build-baseline.log` and
`native2-ctest-baseline.log` have SHA-256 `864e6f0bdf507d73c1178c9078acbe53c24725d2383621db31416c2de9c14544`
and `f1a8714dffcc4593004f670a5ffaa1b79830c1cdcabc1b90b92e25d27674439f`. No native files changed.
**Inference:** Each proposed test should pin deterministic behavior already implemented in the
unchanged production helpers. A passing result will be limited to synthetic/value-level behavior.
**Remaining risks:** No evidence here validates live child process timing, executable loading,
archive data, allocator integration, game execution, or any acceptance gate. Existing user-owned
`native/api/CMakeLists.txt` and untracked `native/api/host/` remain untouched and excluded.
**Next exact action:** Publish these claims before dispatch, then send each worker its single-file
test task; review the diff and independently replay all three focused suites.

## Claim index preflight

- `python3 -I scripts/project_index.py` exited 0 and indexed 14,464 folders; log
  `local/validation/root-round69-scout/claim-project-index.log`, SHA-256
  `d50f1e79e0646836436a43040406e5720bad32092b6cfd2c86a8278065bfe400`.
- The first `python3 -I scripts/project_index.py --check` exited 1 with
  `Stale indexes: local/indexes/folders.json` because its output log was created after the
  generation command, changing the indexed immediate-file count. After all current checkpoint and
  log files existed, generation and check were rerun: 14,464 folders indexed; `--check` reports
  current. Final check log `local/validation/root-round69-scout/claim-project-index-check.log`,
  SHA-256 `d5e6cc672715890d5e489357e2e1b56a23d94cb7fa5cb5bd68717f5613fe72bd`.
- `trail build --check` exited 0 and reported the wiring graph in sync at 3,567 nodes; log
  `local/validation/root-round69-scout/claim-trail-check.log`, SHA-256
  `adef6f08d0303f55da890c62f01a6c70d0dabc1b3a30630d8a792c026809c704`. The optional meaning
  layer remains unbuilt.
- Final `git fetch origin` exited 0. `HEAD == origin/main ==
  c15b27a2a868fb2d4bf124e647e8c0b68f2d8daa`; divergence is `0 0`. Ref log
  `local/validation/root-round69-scout/claim-ref-state.log`, SHA-256
  `60973051249078ba83a5a1f2009d2dbef5ad0801f867bde6118ab078949e8e37`; status log
  `local/validation/root-round69-scout/claim-git-status.log`, SHA-256
  `3a8d270c1d6c80c9044a228f6b5b8d0d5f9f974e737b3c20fd6d5c5414f18684`.
- Fetch log `local/validation/root-round69-scout/claim-fetch-final.log` is empty (exit 0),
  SHA-256 `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`.
- `git diff --cached --check` and `git diff --check` both exited 0; combined log
  `local/validation/root-round69-scout/claim-diff-check.log` is empty, SHA-256
  `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`.
- `python3 -I scripts/audit_publish_paths.py` exited 0 and accepted the 4 staged allowlisted
  documentation paths; log SHA-256 `54f9eaa74aec4c90e67c8e6d94d54b0c4c903120c1a075b0512e74b3e72b3e3b`.
  `python3 -I scripts/audit_publish_paths.py --all` exited 0 and accepted 1,642 paths; log
  SHA-256 `43946cd74d58eb2f9ddcd6a11c4ea930d90c9d9caddf972f9b48244bb63288fb`.
- Immediately before the claim commit, staged paths are only `HANDOFF.md`, `docs/FOLDERS.md`,
  `docs/TASKS.md`, and this checkpoint. Existing user-owned `native/api/CMakeLists.txt` and
  untracked `native/api/host/` are unstaged and untouched.

## Claim publication and dispatch

**Updated:** 2026-10-21 16:00 CDT (America/Chicago). Claim publication is complete; worker dispatch
is the next action.

- The four allowlisted claim paths were committed as
  `4fabeb0ebbc759a3d9bec424e8f39d48893fc83e` (`docs: claim Round 69 fixture batch`) and pushed to
  `origin/main`. Push exited 0 and advanced `c15b27a..4fabeb0`; log
  `local/validation/root-round69-scout/claim-push.log`, SHA-256
  `651366b515a95f42e0bf612957fdfe517e56fcd6f009f55cd64ad18978a94ebd`.
- Commit log `local/validation/root-round69-scout/claim-commit.log` SHA-256
  `894e68bc87c4ee4983393fdf7820945e6294172f8c364a43435b86765d241429`; it records the claim
  commit and four documentation paths.
- After `git fetch origin` exited 0, `HEAD == origin/main ==
  4fabeb0ebbc759a3d9bec424e8f39d48893fc83e` and `git rev-list --left-right --count
  HEAD...origin/main` returned `0 0`. Publication-state log SHA-256
  `7577b4fb22909d284634d7d1f8778db997be81006691eae840edd11400e09cdb`.
- Post-push status log `local/validation/root-round69-scout/claim-git-status.log` SHA-256
  `bafebe0544ec1663fc5c60d5555079052f1efbe617d7957be47744c6262d25c7` shows only the pre-existing
  user-owned `native/api/CMakeLists.txt` modification and untracked `native/api/host/`; neither was
  staged, modified, or included in the claim commit.
- The staged publish audit accepted the four claim paths; the full audit accepted 1,642 paths.
  `git diff --cached --check`, `git diff --check`, and the final project-index check all exited 0.
  Their logs and hashes are listed in the preflight section above.

**Next exact action:** Send the three already-bounded single-file test assignments to the named
workers. Root remains responsible for diff review, independent test replay, E207–E209, and the
completion publication.
