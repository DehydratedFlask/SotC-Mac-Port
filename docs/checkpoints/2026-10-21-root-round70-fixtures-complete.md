# Round 70 fixtures complete

Checkpoint time: 2026-10-21 16:23 CDT (America/Chicago). Integrator: `/root` (Codex desktop task
`01a15eab-88d5-7a83-a23b-cc55dc002eb6`). Claims were published in commit
`099136c56e4de5101b97112d634b5d86864c7425`; claim-doc synchronization commit
`bf96db6ebbaa897b9916f7b4bd5ebd4d57d634ed` is on `origin/main`.

## Completed tasks

| Task | Status / attempts | Files changed | Result |
|---|---|---|---|
| `DOC-FOLDER-INDEX-CHECK-DIRECTORY-OUTPUT-01` | `FIXTURE_VERIFIED`; one failed fixture expectation followed by one passing implementation attempt | `scripts/project_index.py`, `scripts/test_project_tools.py` | Check mode treats a directory at either generated output path as stale and reports a `ValueError`, rather than leaking `IsADirectoryError`. |
| `RE-02-DYNLINK-SPAN-NEGATIVE-OFFSET-SIZE-FIXTURE-01` | `FIXTURE_VERIFIED`; 0 failed tries | `scripts/test_ps4_dynlink.py` | Pins exact errors for negative span offset and length; production parser unchanged. |
| `APP-01-PRESENTATION-IDLE-HINT-ABSENCE-FIXTURE-01` | `FIXTURE_VERIFIED`; 0 failed tries | `launcher/Tests/LauncherCoreTests/LauncherModelTests.swift` | Pins `hint == nil` for the idle not-ready presentation; production model unchanged. |

Other changed files: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, generated `docs/FOLDERS.md`,
and this checkpoint. Existing `native/api/CMakeLists.txt` and untracked `native/api/host/` are
pre-existing user-owned state and are excluded.

The initial index fixture expected a directory under private `local/` to stale the public docs
index. That expectation was wrong: nested private inventory paths are intentionally omitted from
`docs/FOLDERS.md`. The failed run is retained as attempt 1. The corrected fixture expects only the
private JSON output to be stale for that case; the successful second run confirms both directory
cases report stale outputs without raising an OS read exception.

## Commands and evidence

### Project index

- Command: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_project_tools.py`
- Attempt 1: exit 1, 10 tests, one assertion failure for the private-path expectation. Log
  `local/validation/root-round70-scout/index-root-replay.log`, SHA-256
  `4759d653593a6f8adcdb4c01f1c7eff8319bfdaad7885056e8d1c649f42e5856`.
- Attempt 2: exit 0, 10 tests passed. Log
  `local/validation/root-round70-scout/index-root-replay-attempt2.log`, SHA-256
  `d4be35de3d82250986690334669c4d88ae35b2993f723c16c8c675d6299bc32e`.
- `scripts/project_index.py` SHA-256 `04bf99ac9c2027e663b88b0950544008a857e012ea3bab24dfcf678c13c33eb6`;
  `scripts/test_project_tools.py` SHA-256
  `dcf161b6994f01ac92b6abbf261298456017dd7bdfcc0daa5d26fb298aee5280`.

### Dynamic-link span

- Worker and root command: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_ps4_dynlink.py`.
- Worker: exit 0, 25 tests; log `local/validation/root-round70-scout/re-worker-attempt-1.log`,
  SHA-256 `eea909e80cac976e042df6bc3545b70fb14ff6a007d31081c1e4a83f66f1a024`.
- Root replay: exit 0, 25 tests; log `local/validation/root-round70-scout/re-root-replay.log`,
  SHA-256 `e88eaf6e50984f6551445a6c9e77e4ca9e63f0e93ed442e493d2682e576a4f19`.
- `scripts/test_ps4_dynlink.py` SHA-256
  `ca04e6fa132262c90643731ac607b9a3873303adeab6978443efd9132bbb6f95`; unchanged
  `scripts/ps4_dynlink.py` SHA-256
  `28c53257a6f2bd58522f123098d41ca0d884222330a1bf18ce9f8935fb45eff9`.

### Launcher presentation

- Worker and root command: `swift test --package-path launcher --scratch-path local/build/launcher --filter LauncherPresentationTests`.
- Worker: exit 0, 3 tests; log `local/validation/root-round70-scout/app-worker-attempt-1.log`,
  SHA-256 `b2b020e8e4b8385a5cfe03b6d3c3282089186aa07ddc658d8d7725fffb5b7bf7`.
- Root replay: exit 0, 3 tests; log `local/validation/root-round70-scout/app-root-replay.log`,
  SHA-256 `9fbd43ab3d9e2f9abdd46d11fdb246c4cc244e82719e3bef928e43f127cb1284`.
- `launcher/Tests/LauncherCoreTests/LauncherModelTests.swift` SHA-256
  `24ce0317362a36bf95106f59e4c58a48439d91489105e081ffe4ffb4378cfb98`; unchanged
  `launcher/Sources/LauncherCore/LauncherPresentation.swift` SHA-256
  `717e18a1a7452ef8be060a651714e7bf2c122c7df72c0660f919927516f52564`.

## Limits, risks, and next action

- These checks cover temporary-root project-index reporting, synthetic span range validation, and
  a pure Swift presentation value. They establish no ELF loading, native startup, live UI behavior,
  game boot, authentic game audio, real save/reload, or Metal gameplay.
- G0 remains `PARTIAL`; G1–G6 remain `NOT RUN`. No dependency or installation facts changed.
- Existing user-owned `native/api/CMakeLists.txt` changes and untracked `native/api/host/` remain
  outside this task and must not be staged or published.
- Generated project index refreshed successfully: `PYTHONDONTWRITEBYTECODE=1 python3 -I
  scripts/project_index.py` and the matching `--check` each exit 0 at 14,465 folders. Logs are
  `local/validation/root-round70-scout/completion-index-generate.log` (SHA-256
  `1a9a21d51e9ee96151bcc69b5e2f7c8002ec16088f14bbac447803156dd29b0d`) and
  `completion-index-check.log` (SHA-256
  `320c1c3b2c1d30d1937ddb7cba788ebe4633c25b8bd0619bba564b65acf66296`).
- The folder index was refreshed and checked again after creating the sync-publication log paths;
  both commands exit 0 at 14,465 folders. Logs `completion-sync-index-generate.log` and
  `completion-sync-index-check.log` each have SHA-256 `1a9a21d51e9ee96151bcc69b5e2f7c8002ec16088f14bbac447803156dd29b0d`
  and `320c1c3b2c1d30d1937ddb7cba788ebe4633c25b8bd0619bba564b65acf66296` respectively.
- `trail build` exits 0 with 3,570 nodes / 4,832 edges / 943 cards; it parsed 4 of 1,236 files and
  replayed 1,232 cached files. `trail build --check` exits 0 and reports the wiring graph in sync;
  the optional meaning layer remains unbuilt. Logs `completion-trail-build.log` and
  `completion-trail-check.log` have SHA-256
  `83ddf680e3ac9a0ee8d96b93c7cef403cb150f04569167ef9ab55e83589feb56` and
  `39f69c81098aed4346e55b9b92489d150501ea1641ffbf4b3a739e03aa3756f4` respectively.
- `git diff --check` and `git diff --cached --check` exit 0. The staged path audit
  `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/audit_publish_paths.py` accepts exactly 9 staged
  allowlisted paths (log SHA-256
  `a348e5e2bf95acd4f0a2a259fa7b9c693bdffa5c68b376a7bce78ccffe9ba1f5`). The full audit with
  `--all` accepts 1,645 paths (log SHA-256
  `5ae6fe8cfe64099e1a5c0640cefc94f64551573231d27c5dab4d2cb6922b82c0`). The staged set excludes
  both user-owned native/API changes.
- `git commit -m "fix: handle directory index outputs and add Round70 fixtures"` exits 0 and
  creates commit `5a2e8341d9432639609c18f639f49cb1d1155106`; command log
  `local/validation/root-round70-scout/completion-commit.log` has SHA-256
  `57d4e21c2d59df5caeab98588e94a735d3bd39909df6694bc92bcb2d8f34cd92`.
- `git push origin main` exits 0; push log SHA-256
  `b046e9a558bbcb7483b4da6b51cddf43454c832b87855fb568740b095e6cb60a`. Then `git fetch origin`
  exits 0 (empty output). The captured post-push commands report `HEAD` and `origin/main` both
  `5a2e8341d9432639609c18f639f49cb1d1155106`, divergence `0 0`, and only the user-owned native/API
  changes remain in the worktree. State log SHA-256
  `ab55f80110225d059f77f72e00d59d066a799f1cc7516c25a2945d2c6fbe6f28`.
- Fetch log `local/validation/root-round70-scout/completion-fetch.log` SHA-256
  `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`; staged audit log
  `completion-audit-staged.log` SHA-256
  `a348e5e2bf95acd4f0a2a259fa7b9c693bdffa5c68b376a7bce78ccffe9ba1f5`; full audit log
  `completion-audit-all.log` SHA-256
  `5ae6fe8cfe64099e1a5c0640cefc94f64551573231d27c5dab4d2cb6922b82c0`.
- Next task after publication: use read-only Graft scouts and the live task register to identify
  independent Round71 candidates; verify their contracts, then publish bounded claims before
  implementation.
