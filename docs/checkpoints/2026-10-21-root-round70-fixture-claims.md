# Round 70 fixture and edge-fix claims

**Checkpoint:** 2026-10-21 16:16 CDT (America/Chicago)
**Integrator:** `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`)
**Starting revision:** `2f75c173efeda74ec0f1ccd272a9f43e2cfae9cb` (Round69 publication sync; `HEAD == origin/main`).
**Files changed for claims:** `HANDOFF.md`, `docs/TASKS.md`, `docs/FOLDERS.md`, and this checkpoint.
**Implementation status:** No source or test implementation has started. All three tasks have zero failed tries.

Claim commit `099136c56e4de5101b97112d634b5d86864c7425` (`docs: claim Round 70 fixture and index
tasks`) is pushed. After `git fetch origin` exited 0, `HEAD == origin/main ==
099136c56e4de5101b97112d634b5d86864c7425`, with divergence `0 0`. Push log
`claim-push.log`, SHA-256 `e62ab693123eed87d4c74d345d7a9bed0b1e76ccb000fc6b10df5ed37e5e7843`; state
log `claim-publish-state.log`, SHA-256
`632ab1f2ce354ec20ade17a4239a70d1e6d7e31121c5f5a6eada2227a6ca39b4`. Existing user-owned
`native/api/CMakeLists.txt` and untracked `native/api/host/` were not staged.

## Claimed tasks and owners

| Task | Scope and owner | Planned targeted check |
|---|---|---|
| `DOC-FOLDER-INDEX-CHECK-DIRECTORY-OUTPUT-01` | Treat a directory occupying either generated index output path as stale in check mode. Root owns `scripts/project_index.py` and `scripts/test_project_tools.py`. | `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_project_tools.py` |
| `RE-02-DYNLINK-SPAN-NEGATIVE-OFFSET-SIZE-FIXTURE-01` | Assert exact rejection for negative `span` offset and length. `/root/review_host_proto` owns only `scripts/test_ps4_dynlink.py`. | `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_ps4_dynlink.py` |
| `APP-01-PRESENTATION-IDLE-HINT-ABSENCE-FIXTURE-01` | Assert that the idle presentation model has no recovery hint. `/root/launcher_review` owns only `launcher/Tests/LauncherCoreTests/LauncherModelTests.swift`. | `swift test --package-path launcher --scratch-path local/build/launcher --filter LauncherPresentationTests` |

## Live baselines and evidence

- **Project index:** Graft located `generate` at `scripts/project_index.py:59-77` and the current
  missing-output test at `scripts/test_project_tools.py:111-123`. The read-only check's baseline
  command `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_project_tools.py` exits 0 with 9
  tests; log `local/validation/root-round70-scout/index-baseline.log`, SHA-256
  `0490208e5a254f2f2e6e0f70f7a5ee55157accf27688d6887c0492cc0a5b37e0`. The reproducible private
  probe `PYTHONDONTWRITEBYTECODE=1 python3 -I local/validation/root-round70-scout/index-directory-repro.py`
  exits 0 after observing `IsADirectoryError` for both `docs/FOLDERS.md` and
  `local/indexes/folders.json`; script SHA-256
  `3ec9f7881ee30478d06ee4c1f2e84a81971f23aa177762420c27d481d20be8bf`, output log
  `index-directory-repro.log` SHA-256
  `7ea57f81c313c4e5521463906bd697e6736ea285cd91d5d599a901a12042338f`. Baseline indexer SHA-256
  `394ec07b93f1120e4fe2f5459cc0663672ad0e2165ab87afaa568bacdf40b9d9`; test SHA-256
  `986821a09679e2080ee653d9911ef8c032b61f1b3cc43f1af18fe31bcbc594f4`. The intended fix is to
  classify non-file output destinations as stale before reading; tests will preserve the
  directory and assert the exact stale-output error.
- **Dynlink:** Graft located `span` at `scripts/ps4_dynlink.py:27-30`. Its explicit negative
  offset/size guards are not covered by the existing end-overflow fixture. Baseline command
  `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_ps4_dynlink.py` exits 0 with 24 tests; log
  `re-baseline.log`, SHA-256
  `84a7ab403af2322bcfe6ed0618c774e421b8a5f5727f8ebb8e27ca6605d61cd1`. Baseline parser SHA-256
  `28c53257a6f2bd58522f123098d41ca0d884222330a1bf18ce9f8935fb45eff9`; test SHA-256
  `f7f8b8415ecede7f957d69b369a8565596383fed1dec3f99d99bc496d46ec952`. Planned synthetic
  assertions use `d.span(b'abc', -1, 1)` and `d.span(b'abc', 0, -1)`, pinning the respective exact
  `out of bounds` errors.
- **Launcher presentation:** Graft located the `.idle` mapping in
  `launcher/Sources/LauncherCore/LauncherPresentation.swift:33-37` and its existing fixture in
  `launcher/Tests/LauncherCoreTests/LauncherModelTests.swift:446-452`. The test covers blocker
  copy, accessibility value, and neutral tone but not `hint`. Baseline command
  `swift test --package-path launcher --scratch-path local/build/launcher --filter LauncherPresentationTests`
  exits 0 with 3 tests in 1 suite; log `app-baseline.log`, SHA-256
  `ed91c45e6bdf932f3db3c833a33ca3b89558cb87136118f03e63a0f6d491ffaf`. Baseline presentation
  source SHA-256 `717e18a1a7452ef8be060a651714e7bf2c122c7df72c0660f919927516f52564`; test SHA-256
  `51f1f531998316a91cc52a86f4695a43285d277eed3d19b35299abdf6742d0cc`. Planned assertion is
  `presentation.hint == nil`, limited to the pure model, not rendered UI or VoiceOver.

## Decisions, limits, and next action

- Observed: all three baseline suites pass; a directory at either generated-index output path
  currently raises `IsADirectoryError`; source and test hashes match the live checkout. Inference:
  the directory cases are invalid generated outputs and should follow check mode's existing
  stale-output reporting contract.
- A PSARC interior-empty manifest token was deferred because the supported format policy is not
  established. The prior invalid-UTF-8 exception preference remains pending and is not claimed.
- **Index preflight:** An initial `python3 -I scripts/project_index.py` exited 0 and reported
  14,465 folders; the following `python3 -I scripts/project_index.py --check` exited 1 with
  `Stale indexes: local/indexes/folders.json`. A second refresh/check pair repeated that result.
  The failure output is retained in `local/validation/root-round70-scout/claim-project-index-check-final.log`,
  SHA-256 `31dad00ffd5208458ee89a329fea70b0fa13494e525ddd8a7e837c7614a2266b`. The live
  `inventory` function counts immediate files under private paths too; writing a new redirected
  check log after generation changed the indexed count in `local/validation/root-round70-scout`.
  This was an indexing-workflow issue, not a failed implementation try. After reserving the
  expected log paths, final `python3 -I scripts/project_index.py` exited 0 with 14,465 folders
  (log `claim-project-index.log`, SHA-256
  `1a9a21d51e9ee96151bcc69b5e2f7c8002ec16088f14bbac447803156dd29b0d`) and
  `python3 -I scripts/project_index.py --check` exited 0, current at 14,465 (log
  `claim-project-index-check.log`, SHA-256
  `320c1c3b2c1d30d1937ddb7cba788ebe4633c25b8bd0619bba564b65acf66296`).
- `trail build --check` exited 0; the wiring graph is in sync at 3,568 nodes, 4,829 edges, and
  943 cards. Log `claim-trail-check.log`, SHA-256
  `3290230a999a0955146a4ad9bf1bd358fd2e4177737331da17d29ca7cf9ae37e`. The optional meaning
  layer remains unbuilt.
- **Claim publication preflight:** `git fetch origin` exited 0; `HEAD == origin/main ==
  2f75c173efeda74ec0f1ccd272a9f43e2cfae9cb`, divergence `0 0`. State log
  `claim-prepush-state.log`, SHA-256
  `6d2c955ec167557838cb6cc5d5902a1c1274a335c5021eeab5194162f057a51b`. The staged audit exited 0
  for 4 allowlisted files; log `claim-audit-staged.log`, SHA-256
  `60b06ebb3ca214b8b997e50beb76bfcfd0f69e4aa7a093e0facfc57d11793f59`. The full audit exited 0
  for 1,644 paths; log `claim-audit-all.log`, SHA-256
  `7abeb299fcceb90079287055105cd47deb9064ba7ee58a401251ba5d729a77a5`. Both
  `git diff --cached --check` and `git diff --check` exited 0; their empty logs have SHA-256
  `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`.
- These fixtures and the project-index check-mode fix do not advance runtime gates. G0 remains
  PARTIAL; G1-G6 remain NOT RUN. No live launcher UI, native game behavior, ELF loading, real
  archive, game audio, save/reload, or Metal gameplay is established.
- Exact next action: dispatch the two claimed test-file tasks and implement the root-owned indexer
  case.
