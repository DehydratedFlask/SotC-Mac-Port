# SoC Mac Native — current handoff

Updated: 2026-10-21 12:17 CDT (America/Chicago). Integrator: /root (Codex desktop task
`01a15eab-88d5-7a83-a23b-cc55dc002eb6`). Previous transfer record:
[docs/checkpoints/2026-10-20-root-next-agent-handoff.md](docs/checkpoints/2026-10-20-root-next-agent-handoff.md);
older history: `docs/checkpoints/2026-10-20-handoff-archive-through-1f7c8e8.md`.

**Project remains incomplete.** G0 PARTIAL; G1–G6 NOT RUN. No native game executable, boot,
authentic game audio, real progress save/relaunch/load, or Metal gameplay is demonstrated.

## Session addendum 2026-10-21 round 40 (`/root`)

- Completed `APP-01-DIAGNOSTICS-CONFLICTING-DUPLICATE-01`; worker and root each pass 7/7 focused
  tests. The fixture proves a later missing duplicate cannot make an all-ready report startable.
  E150 and its completion checkpoint record both logs and hashes.
- Completed `APP-01-CORE-SUBSYSTEM-UNKNOWN-CODE-FIXTURE-01`; worker and root each pass 82 tests
  across 12 suites. The unknown-code assertion is test-only; `CoreModels.swift` is unchanged.
  E151 and its completion checkpoint record both logs and hashes.
- The three launcher fixtures from rounds 38–40 are synthetic test coverage. No real game
  acceptance gate changed. Existing `native/api/CMakeLists.txt` and `native/api/host/` remain
  untouched and unpublished.
- Exact next action: regenerate/check indexes and the source graph, publish the two fixture
  completions and round-38 publication record, then select another bounded unowned task.

## Session addendum 2026-10-21 round 39 (`/root`)

- Published short-magic fixture completion, E149, and two separate fixture claims in commit
  `d7f471fca29bf7ddd380d44efa5cea6d47d8c628` (`test: cover short PSARC magic in launcher report`);
  push succeeded and `HEAD` matched `origin/main`. See the
  [round 38 publication checkpoint](docs/checkpoints/2026-10-21-root-round38-publication.md).
- Pre-push verification passed: project index current at 14,372 folders; trail graph current at
  3,538 nodes; staged audit accepted 8 paths; full audit accepted 1,550 paths; staged diff check
  passed. Only allowlisted documentation and the short-magic Swift test were committed.
- `APP-01-ASSET-VALIDATOR-SHORT-MAGIC-FIXTURE-01` is `FIXTURE_VERIFIED`; the diagnostics
  conflicting-duplicate and unknown-subsystem-code tasks are `ACTIVE`, each with zero failed
  tries and a distinct test-file owner.
- Actual game acceptance gates remain open. Existing `native/api/CMakeLists.txt` and
  `native/api/host/` remain untouched and unpublished.
- Exact next action: receive both workers' results, review their distinct diffs, replay each
  focused test suite, and publish accurate outcomes.

## Session addendum 2026-10-21 round 38 (`/root`)

- Completed `APP-01-ASSET-VALIDATOR-SHORT-MAGIC-FIXTURE-01`. Worker and root each pass the
  focused launcher suite (6/6); only `AssetValidationReportTests.swift` changed, while
  `AssetValidator.swift` stayed unchanged. E149 and the completion checkpoint record both logs
  and hashes.
- Claimed `APP-01-DIAGNOSTICS-CONFLICTING-DUPLICATE-01`, `ACTIVE`, zero failed tries. Add a
  synthetic `ReadinessReport` with the four required subsystem rows READY plus a later duplicate
  assets row marked missing; assert the report is not startable and the summary does not claim
  Play is ready. Claim:
  [conflicting duplicate diagnostics](docs/checkpoints/2026-10-21-root-app01-diagnostics-conflicting-duplicate-claim.md).
- Claimed `APP-01-CORE-SUBSYSTEM-UNKNOWN-CODE-FIXTURE-01`, `ACTIVE`, zero failed tries. Add the
  missing pure-Swift fallback assertion `CoreSubsystem.Status(code: 99) == .unknown(99)` in
  `CoreClientTests.swift`. Claim:
  [unknown subsystem code](docs/checkpoints/2026-10-21-root-app01-core-subsystem-unknown-code-claim.md).
- `/root/review_host_proto` owns only `DiagnosticsSummaryTests.swift`; `/root/launcher_review`
  owns only `CoreClientTests.swift`. Both changes begin after these claims are published. Root
  will replay the focused suites.
- No actual game acceptance gate changed. Existing `native/api/CMakeLists.txt` and untracked
  `native/api/host/` remain untouched and unpublished.
- Exact next action: regenerate/check indexes, path-audit the exact documentation publication,
  push the short-magic completion and both claims, then release the two test files to their owners.

## Session addendum 2026-10-21 round 37 (`/root`)

- Recorded publication of the NUL-filename completion and the raw-DEFLATE task-status correction
  in commit `ae2ee8d8daf8330a461cc5e1330309c2c80c7e24` (`docs: reconcile published fixture task
  states`); push succeeded and HEAD matched `origin/main`. See the
  [round 35 publication checkpoint](docs/checkpoints/2026-10-21-root-round35-publication.md).
- Claimed `APP-01-ASSET-VALIDATOR-SHORT-MAGIC-FIXTURE-01`, `ACTIVE` with zero failed tries. Add
  one test in `launcher/Tests/LauncherCoreTests/AssetValidationReportTests.swift`: create the
  existing six-file valid fixture, replace `pak0.psarc` with `Data("PS".utf8)`, and assert the
  sole problem is `pak0.psarc is not a PSARC archive` while `missingArchives` remains empty. No
  production code changes or real archive data. Claim:
  [short PSARC magic](docs/checkpoints/2026-10-21-root-app01-asset-validator-short-magic-claim.md).
- `/root/slot23_raw_xrefs` may edit only the test file after this claim is published. Root owns
  shared docs and will run `swift test --package-path launcher --filter AssetValidationReportTests`.
- Actual game acceptance gates remain open. Existing host-runtime worktree paths remain untouched
  and unpublished.
- Exact next action: run `python3 scripts/project_index.py`, verify indexes, path-audit, then commit
  and push the round 35 publication record and short-magic claim.

## Session addendum 2026-10-21 round 36 (`/root`)

- Published `AUD-PUBLISH-GIT-NUL-FILENAME-FIXTURE-01` completion in commit
  `e9b1a15526e247401bb1622cfd077853ba9b7a31` (`test: preserve newlines in staged path audit
  names`); push succeeded and HEAD matches `origin/main`. See the
  [round 34 publication checkpoint](docs/checkpoints/2026-10-21-root-round34-publication.md).
- Corrected the stale `docs/TASKS.md` status for `AUD-01-CLASSIFIER-RAW-INFLATE-OFFSET-00-FIXTURE-01`:
  E145 and its completion checkpoint already recorded `FIXTURE_VERIFIED`, while the task row still
  said `ACTIVE`. This is a documentation reconciliation; no code or test result changed.
- `/root/slot23_raw_xrefs` is scouting another bounded test-only task in a distinct module. No
  implementation begins until root checks the candidate and publishes a claim.
- Pre-push checks for the prior commit passed: folder index current at 14,235; source graph current
  at 3,537 nodes; staged audit accepted 7 paths and full audit accepted 1,544 paths. Only the
  pre-existing `native/api/CMakeLists.txt` modification and untracked `native/api/host/` remain.
- Actual game acceptance gates remain open.
- Exact next action: review the scout's candidate and claim one unowned, bounded task if available.

## Session addendum 2026-10-21 round 35 (`/root`)

- Completed `AUD-PUBLISH-GIT-NUL-FILENAME-FIXTURE-01`, `FIXTURE_VERIFIED` with zero failed tries.
  `/root/review_host_proto` and root each pass `PYTHONDONTWRITEBYTECODE=1 python3 -I
  scripts/test_audit_publish_paths.py` (8/8). A mocked staged Git result containing one embedded
  newline is returned as one path; the fixture also asserts `git diff --cached --name-only -z`
  and `cwd=audit.ROOT`. Test SHA256 is
  `2759f5b9f554388ca7fd95ab2ee6457d16af995d3f0e86bbe85458422e65d2ac`; production audit
  `scripts/audit_publish_paths.py` is unchanged at
  `2ea984cfb38329f4a9ec0b46300b0513998a1a642d224572906cee531653bba3`. E148 and the
  [completion checkpoint](docs/checkpoints/2026-10-21-root-aud-publish-git-nul-filename-complete.md)
  record worker/root logs and hashes.
- The claim publication for this task is recorded at [round 33](docs/checkpoints/2026-10-21-root-round33-publication.md).
  `trail build` parsed the test and rebuilt 3,537 nodes / 4,758 edges; `trail build --check` passed.
- Actual game acceptance gates remain open. Only the pre-existing `native/api/CMakeLists.txt`
  modification and untracked `native/api/host/` remain outside the task.
- Exact next action: update and check the folder index, then path-audit, commit, and push the fixture
  completion and publication record.

## Session addendum 2026-10-21 round 34 (`/root`)

- Published the claim for `AUD-PUBLISH-GIT-NUL-FILENAME-FIXTURE-01` in commit
  `c640c20b9bf48f720a5e4e8d87e82bd45d798e59` (`docs: claim NUL-safe audit filename fixture`);
  the push succeeded and HEAD matches `origin/main`. See the
  [round 33 publication checkpoint](docs/checkpoints/2026-10-21-root-round33-publication.md).
- The task remains `ACTIVE` with zero failed tries. `/root/review_host_proto` now owns only
  `scripts/test_audit_publish_paths.py`; root owns shared docs and will replay the focused test.
- Pre-push checks passed: project index current at 14,234 folders; source graph current at 3,536
  nodes; staged-path audit accepted 5 paths and full-path audit accepted 1,542 paths. The only
  unrelated dirty paths remain the pre-existing `native/api/CMakeLists.txt` and `native/api/host/`.
- Actual game acceptance gates remain open.
- Exact next action: review the worker diff and root-replay
  `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_audit_publish_paths.py`.

## Session addendum 2026-10-21 round 33 (`/root`)

- Recorded the prior completion publication in commit `33099216c0777f517d4087faaf68b8cb61181bc8`
  (`docs: record PSARC bounds publication`); `git push origin main` succeeded and local HEAD
  matches the remote. See the [round 32 publication checkpoint](docs/checkpoints/2026-10-21-root-round32-publication.md).
- Claimed `AUD-PUBLISH-GIT-NUL-FILENAME-FIXTURE-01`, `ACTIVE` with zero failed tries. Add one
  synthetic test in `scripts/test_audit_publish_paths.py` that mocks the staged-name Git command
  to return `b"scripts/name\nwith-newline.py\0"`, then confirms `git_names("staged")` returns
  exactly one path containing the embedded newline and uses `--name-only -z` from `audit.ROOT`.
  Do not change the production audit. Claim: [NUL-delimited filename](docs/checkpoints/2026-10-21-root-aud-publish-git-nul-filename-claim.md).
- `/root/review_host_proto` has been asked to implement only the test module after this claim is
  published. Root owns shared docs and will independently replay
  `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_audit_publish_paths.py`.
- Actual game acceptance gates remain open; the only unrelated dirty paths remain the pre-existing
  `native/api/CMakeLists.txt` modification and untracked `native/api/host/`.
- Exact next action: run `python3 scripts/project_index.py`, verify folder and source indexes,
  path-audit the publication, then commit and push the round 32 publication record and task claim.

## Session addendum 2026-10-21 round 32 (`/root`)

- Published `DATA-02-PSARC-BLOCK-SIZE-RANGE-01` completion in commit
  `cf5096b0c775104739f871c40da8b698618219b6` (`test: cover PSARC block-size validation bounds`);
  `git push origin main` succeeded and HEAD matches `origin/main`. See the
  [round 31 publication checkpoint](docs/checkpoints/2026-10-21-root-round31-publication.md).
- Pre-push checks passed: `python3 scripts/project_index.py --check` reports 14,234 folders
  current; `trail build --check` reports 3,536 nodes in sync; `git diff --check` passes; the staged
  path audit accepted 7 paths and the full tracked-path audit accepted 1,539 paths.
- The worktree retains only the pre-existing `native/api/CMakeLists.txt` modification and untracked
  `native/api/host/`. Actual game acceptance gates remain open.
- `/root/review_host_proto` is scouting one additional test-only task in a disjoint module; it is
  read-only and must not edit before a bounded claim is recorded and published.
- Exact next action: review the scout's candidate against Graft, live source, and `docs/TASKS.md`,
  then claim one bounded task before assigning any edit.

## Session addendum 2026-10-21 round 31 (`/root`)

- Completed `DATA-02-PSARC-BLOCK-SIZE-RANGE-01`, `FIXTURE_VERIFIED` with zero failed tries.
  `slot23_raw_xrefs` and root each pass `PYTHONDONTWRITEBYTECODE=1 python3 -I
  scripts/test_psarc.py` (32/32). Starting from a valid synthetic PSARC, the fixture mutates
  header bytes 24–27 to `0` and `16,777,217`; both are rejected with the exact
  `PsarcError('unsupported entry/block size')`. Test SHA256 is
  `0a922203a17cfe7454cbf5f1f8e5787413511320d6bd4a54415c205360263dfc`; parser
  `scripts/psarc.py` is unchanged at
  `39f2c748db013637aba8144205dc60b7e5b1eb49ee0f641960783a1e2c140485`. E147 and the
  [completion checkpoint](docs/checkpoints/2026-10-21-root-data02-psarc-block-size-range-complete.md)
  record the worker/root log hashes and synthetic-only limit.
- The prior publication is recorded at [round 29](docs/checkpoints/2026-10-21-root-round29-publication.md).
  `trail build` parsed the changed test and rebuilt 3,536 nodes / 4,756 edges; subsequent
  `trail build --check` passed. The project folder index became stale after the root test log was
  created; regenerate and check it before the next push.
- The only excluded worktree paths remain the pre-existing `native/api/CMakeLists.txt` modification
  and untracked `native/api/host/`. Actual game acceptance gates remain open.
- Exact next action: run `python3 scripts/project_index.py`, verify it with `python3 scripts/project_index.py --check`, then stage only the PSARC test and its task/evidence/checkpoint records, audit, commit, and push.

## Session addendum 2026-10-21 round 30 (`/root`)

- Published the zero-length mapping fixture and the bounded PSARC block-size claim in commit
  `13b1b3ab64ed29ee4c3992d6edf27e85a8646ff9` (`test: pin dynlink empty ranges and claim PSARC
  size bounds`) to `origin/main`; the remote accepted the push and HEAD matches origin/main.
  See [publication checkpoint](docs/checkpoints/2026-10-21-root-round29-publication.md).
- Before the push, `python3 scripts/project_index.py` wrote 14,233 folder entries;
  `python3 scripts/project_index.py --check` reported them current; `trail build --check` reported
  3,535 nodes in sync; and `git diff --check` passed. `python3 -I scripts/audit_publish_paths.py`
  accepted all 7 staged paths, and `python3 -I scripts/audit_publish_paths.py --all` accepted
  1,537 paths. The new checkpoint and task claim are now published.
- `DATA-02-PSARC-BLOCK-SIZE-RANGE-01` remains `ACTIVE`, with `/root/slot23_raw_xrefs` assigned
  only `scripts/test_psarc.py`; the worker has been asked to add the two synthetic invalid-header
  cases, run the focused suite, and save a private log. Root owns shared docs and will replay the
  focused suite before marking completion.
- Only the pre-existing `native/api/CMakeLists.txt` modification and untracked `native/api/host/`
  remain in the worktree outside the commit. No actual game acceptance gate advanced.
- Exact next action: review the worker's `scripts/test_psarc.py` diff and independently run
  `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_psarc.py`.

## Session addendum 2026-10-21 round 29 (`/root`)

- Completed `RE-02-DYNLINK-MAPPED-ZERO-LENGTH-01`, `FIXTURE_VERIFIED` with zero failed tries.
  `launcher_review` and root each pass `PYTHONDONTWRITEBYTECODE=1 python3 -I
  scripts/test_ps4_dynlink.py` (21/21). The new synthetic assertion confirms that an empty range
  at `0x1010`, the first segment's one-past-end address, is considered mapped. Test SHA256 is
  `75f971b5834fd95f2b16b92133aa999d3f77211e73d74834b6f88da6dfe12e07`; parser
  `scripts/ps4_dynlink.py` is unchanged at
  `28c53257a6f2bd58522f123098d41ca0d884222330a1bf18ce9f8935fb45eff9`. E146 and the
  [completion checkpoint](docs/checkpoints/2026-10-21-root-re02-dynlink-mapped-zero-length-complete.md)
  record the worker/root log hashes and synthetic-only limit.
- Claimed `DATA-02-PSARC-BLOCK-SIZE-RANGE-01`, `ACTIVE` with zero failed tries. The scoped test
  mutates the block-size field in a valid synthetic archive to `0` and `16,777,217`, then asserts
  the exact `PsarcError('unsupported entry/block size')` for both invalid bounds. Only
  `scripts/test_psarc.py` may change; production parser and game archives are out of scope. Claim:
  [PSARC block-size range](docs/checkpoints/2026-10-21-root-data02-psarc-block-size-range-claim.md).
- Root owns shared documentation. After this claim checkpoint is published, assign only
  `scripts/test_psarc.py` to `slot23_raw_xrefs`; worker and root must save separate logs in the
  private validation folder. Actual game acceptance gates remain open.
- Exact next action: run `python3 scripts/project_index.py`, verify folder and source indexes, then
  path-audit, commit, and push the zero-length completion plus the new bounded claim.

## Session addendum 2026-10-21 round 11 (`/root`)

- Completed three independent, synthetic test-only tasks: `DATA-02-OVERLAP-MAP-JSON-CONSTANTS-01`
  (worker `launcher_review`, `scripts/test_archive_overlap_map.py` only),
  `AUD-01-XFF-EXACT-PATH-CARDINALITY-01` (worker `slot23_raw_xrefs`,
  `scripts/test_audio_bank_header.py` only), and `DOC-FOLDER-INDEX-SYMLINK-01`
  (worker `review_host_proto`, `scripts/test_project_tools.py` only). All are FIXTURE_VERIFIED with
  zero failed tries; root owns this handoff and the project/task records.
- Their prerequisites are complete: the archive-overlap map artifact is verified; XFF identity
  verification is fixture-verified; and the folder-index admin-directory fixture is fixture-verified.
  No production parser/index code or game data is in scope.
- Claims: [JSON constants](docs/checkpoints/2026-10-21-root-data02-overlap-map-json-constants-claim.md),
  [XFF exact paths](docs/checkpoints/2026-10-21-root-aud01-xff-exact-path-cardinality-claim.md),
  [folder-index symlinks](docs/checkpoints/2026-10-21-root-doc-folder-index-symlink-claim.md).
- Acceptance commands: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_archive_overlap_map.py`,
  `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_audio_bank_header.py`, and
  `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_project_tools.py`, respectively. Workers must
  save logs in private `local/validation/` paths; root independently replayed them. Worker/root
  results are 19/19, 18/18, and 7/7; E134-E136 and three completion checkpoints record hashes,
  logs, observations, and synthetic-only limits.
- Claim-time repository state: `main`, HEAD equals `origin/main` at `667a88c`; only the previously
  escalated `native/api/CMakeLists.txt` and untracked `native/api/host/` are dirty. Those paths stay
  outside all three tasks and all commits. No game gate advances from these fixtures.
- Claim publication: commit `8723be0eba56ba3ed675dc73b7379edc6f2391eb` is pushed to `origin/main`;
  staged and full path audits passed for 6 staged paths and 1,509 tracked paths. Root verified
  HEAD equals `origin/main`; only the excluded host escalation paths remain dirty.
- Checkpoint-index correction: after private checkpoint logs were added, two
  `python3 scripts/project_index.py --check` runs reported stale `local/indexes/folders.json`;
  the index was regenerated after all log paths existed, and the final check passes with 14,214
  folders. This affects only the private index; `docs/FOLDERS.md` remains current.
- Source-graph checkpoint: `trail build` exited 0 and parsed the changed source; `trail build --check`
  exited 0 with wiring graph in sync (3,527 nodes / 4,739 edges). The deep meaning layer remains
  unbuilt (0% complete); see private logs in `local/checkpoints/root-round11-claims/`.
- Final folder-index checkpoint: `python3 scripts/project_index.py` and
  `python3 scripts/project_index.py --check` both exited 0 with 14,217 folders; generated
  `docs/FOLDERS.md` includes the three completion records, and the private recursive index is current.
  Logs: `local/checkpoints/root-round11-claims/{index-final.log,index-final-check.log}`.
- Immediate next action: continue with another bounded unclaimed task from the read-only scout, if
  available. Preserve the slot-23/Ghidra blocker, three-try host escalation, and all game/runtime
  acceptance gates.

## Session addendum 2026-10-21 round 12 (`/root`)

- Published round 11's three synthetic test-only completions: overlap-map JSON constants, XFF exact
  path cardinality, and folder-index symlink exclusion. Commit `219f261ce102aabe6050a3ffa63ff1e4f7114856`
  is on `origin/main`; root replay counts were 19/19, 18/18, and 7/7. E134-E136 and the three
  completion checkpoints record private logs and hashes. No production behavior or game gate changed.
- Graft located `name_by_hash()` at `scripts/ps4_dynlink.py:155-170` and its `build()` call at
  `:212`. The live function hashes identifier-like tokens from selected text files; the caller uses
  results only to label unresolved import spellings. `scripts/test_ps4_dynlink.py` tests `nid()` and
  synthetic ELF parsing but has no `name_by_hash()` coverage.
- Claimed `RE-02-DYNLINK-NID-NAME-HASH-FIXTURE-01`, ACTIVE with zero failed implementation tries.
  Bounded scope is a synthetic temporary text tree and direct assertions that a requested known NID
  resolves to its token while an absent NID is omitted. Test-only edit to
  `scripts/test_ps4_dynlink.py`; no parser/build changes or real source-tree/game behavior claim.
- Claim checkpoint: [name-by-hash fixture](docs/checkpoints/2026-10-21-root-re02-dynlink-name-by-hash-claim.md).
  Prerequisite `RE-02-DYNLINK-01` is FIXTURE_VERIFIED. Acceptance command:
  `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_ps4_dynlink.py`.
- Claim publication: commit `fe7446f58d87bad444c3cec18196d9ac09442184` is pushed to `origin/main`;
  staged audit accepted four paths, full tracked-path audit accepted 1,513 paths, and root verified
  HEAD equals `origin/main`. `python3 scripts/project_index.py --check` reports 14,218 folders
  current; `trail build --check` reports the wiring graph in sync (3,527 nodes / 4,739 edges), with
  the deep meaning layer still unbuilt. Private command logs are in
  `local/checkpoints/root-round12-dynlink-claim/`.
- Next exact action: assign only `scripts/test_ps4_dynlink.py` to `launcher_review`, independently
  replay the focused suite, record the fixture result, and publish completion. Keep
  `native/api/CMakeLists.txt` and `native/api/host/` excluded; preserve all emulation/module and real
  game acceptance gates.

## Session addendum 2026-10-21 round 13 (`/root`)

- Completed `RE-02-DYNLINK-NID-NAME-HASH-FIXTURE-01`, now FIXTURE_VERIFIED with zero failed
  implementation tries. Worker `launcher_review` and root each pass
  `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_ps4_dynlink.py` (19/19). The test creates a
  temporary `fixture.cpp` containing `synthetic_export_name`, asserts the requested NID resolves to
  that token, and asserts an absent requested NID is omitted. E137 and
  [the completion checkpoint](docs/checkpoints/2026-10-21-root-re02-dynlink-name-by-hash-complete.md)
  retain log/test/source hashes and the spelling-only boundary.
- Only `scripts/test_ps4_dynlink.py` changed in the worker implementation; production
  `scripts/ps4_dynlink.py` is unchanged. `trail build --check` passes with the new test indexed
  (3,528 nodes; deep meaning layer remains unbuilt). No import semantics, loader, or game gate is
  established.
- Completion publication: commit `cfebea9848583ed1df9124a869e99379e65f988a` is pushed to
  `origin/main`; staged audit accepted six paths and full tracked-path audit accepted 1,514 paths.
  Root verified HEAD equals `origin/main`; the folder index reports 14,219 folders current and
  `trail build --check` passes with 3,528 nodes. Next exact action: continue with a bounded unclaimed
  task from the independent scouts. Keep the existing host escalation and slot-23/Ghidra blocker
  unchanged.

## Session addendum 2026-10-21 round 14 (`/root`)

- Claimed two independent synthetic test-only tasks: `DATA-02-PSARC-BLOCK-TABLE-WIDTH-01` and
  `DOC-FOLDER-INDEX-CHECK-READONLY-01`, both ACTIVE with zero failed implementation tries. Claims
  and dependencies are recorded in `docs/TASKS.md`; detailed evidence is in the two claim
  checkpoints named below.
- Graft confirms `Psarc._load()` computes `self.width = max(1, ((self.block_size - 1).bit_length() + 7) // 8)` and decodes fixed-width big-endian block entries. The synthetic PSARC builder in `scripts/test_psarc.py` fixes `BS = 16` and serializes `blocks` with `bytes(blocks)`, so existing fixtures exercise only one-byte entries. Planned fixture uses block size 257 and a 256-byte raw payload whose table value is 256 (`0x0100`), asserting width 2 and successful roundtrip.
- `project_index.generate(root, check=True)` skips directory creation, checks both generated files, and raises `ValueError` when either is missing. Existing test coverage calls `generate(root)` first, so it does not prove the empty-root check avoids writes. Planned test asserts exact stale-index error and confirms neither `docs/` nor `local/` is created.
- Claim checkpoints: [PSARC block width](docs/checkpoints/2026-10-21-root-data02-psarc-block-table-width-claim.md) and [folder-index check mode](docs/checkpoints/2026-10-21-root-doc-folder-index-check-readonly-claim.md). Dependencies: `DATA-02-PSARC-PARTIAL-READ-01` is FIXTURE_VERIFIED; folder-index task has no prerequisite. Acceptance commands are `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_psarc.py` and `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_project_tools.py`.
- Worker assignments after claim publication: `launcher_review` owns only `scripts/test_psarc.py`; `slot23_raw_xrefs` owns only `scripts/test_project_tools.py`. Root retains sole ownership of handoff/task/evidence records. Claim commit `f427b2bb412379a54feb7e5f3af27420420a31da` and publication-record commit `95642f017d58c2fccaf1cd2398a32077230a5d8d` are on `origin/main`; staged audits accepted five paths and then three follow-up paths, full tracked-path audits accepted 1,516 files, the folder index reports 14,222 folders current, and `trail build --check` passes with 3,528 nodes. Root verified HEAD equals `origin/main`. Preserve the escalated native host paths outside both tasks and commits.

## Session addendum 2026-10-21 round 15 (`/root`)

- Completed `DATA-02-PSARC-BLOCK-TABLE-WIDTH-01`, FIXTURE_VERIFIED: the builder now emits width-sized table entries for configurable block size; the synthetic block-size-257 fixture uses `bytes(range(256))`, asserts width 2 and parsed value 256 (`0x0100`), then verifies exact readback. Worker and root each pass `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_psarc.py` (31/31). E138 and [completion checkpoint](docs/checkpoints/2026-10-21-root-data02-psarc-block-table-width-complete.md) record hashes/logs. Production `scripts/psarc.py` is unchanged.
- Completed `DOC-FOLDER-INDEX-CHECK-READONLY-01`, FIXTURE_VERIFIED: an empty temporary root in check mode raises the exact two-output stale-index error and creates neither `docs/` nor `local/`. Worker and root each pass `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_project_tools.py` (8/8). E139 and [completion checkpoint](docs/checkpoints/2026-10-21-root-doc-folder-index-check-readonly-complete.md) record hashes/logs. Production `scripts/project_index.py` is unchanged.
- Graph checkpoint: `trail build --check` first reported stale after these two new test methods; `trail build` exited 0 and rebuilt 3,530 nodes / 4,746 edges; subsequent `trail build --check` exited 0. Deep meaning layer remains unbuilt. Stale output, build, and passing check are under `local/checkpoints/root-round13-claims/`.
- Completion publication: commit `42af4f281e2d310024bd2ce1f99a3784bae2371e` is on `origin/main`;
  staged audit accepted eight paths and the full tracked-path audit accepted 1,518 paths. Root
  verified HEAD equals `origin/main`. `python3 scripts/project_index.py --check` reports 14,222
  folders current; `trail build --check` passes with 3,530 nodes / 4,746 edges. Next exact action:
  continue with the independent scout's candidate if still unclaimed. All real game acceptance gates
  remain unchanged.

## Session addendum 2026-10-21 round 28 (`/root`)

- Published the round 27 raw-DEFLATE fixture and zero-length mapped-range claim in commit `fd601388d2205cf7403e3796260d6fa7d51b40fd` (`test: cover raw inflate offset zero and claim empty range`); `git push origin main` exited 0 and both `HEAD` and `origin/main` resolve to that commit. See [publication checkpoint](docs/checkpoints/2026-10-21-root-round27-publication.md).
- Pre-commit checks passed: `python3 scripts/project_index.py` and `python3 scripts/project_index.py --check` (14,232 folders current); `trail build --check` (3,535 nodes in sync; deep meaning layer not built); `git diff --check`; `git diff --cached --check`; `python3 -I scripts/audit_publish_paths.py` (8 staged allowlisted paths); and `python3 -I scripts/audit_publish_paths.py --all` (1,534 paths). Post-push `git status --short --branch` showed `main` synchronized; only the pre-existing native host worktree paths remain outside the commit.
- `AUD-01-CLASSIFIER-RAW-INFLATE-OFFSET-00-FIXTURE-01` is `FIXTURE_VERIFIED`; `RE-02-DYNLINK-MAPPED-ZERO-LENGTH-01` is `ACTIVE`. Both have zero failed tries. Actual game acceptance gates remain open.
- `launcher_review` now owns only `scripts/test_ps4_dynlink.py` for the zero-length boundary fixture. `slot23_raw_xrefs` has been asked to scout one more distinct test module. Next exact action: review the dynlink test patch, root-replay `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_ps4_dynlink.py`, and record the result.

## Session addendum 2026-10-21 round 27 (`/root`)

- Completed `AUD-01-CLASSIFIER-RAW-INFLATE-OFFSET-00-FIXTURE-01`, `FIXTURE_VERIFIED` with zero failed tries. Worker and root each pass `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_audio_headers.py` (21/21). A 64-byte synthetic payload compressed with raw DEFLATE selects `raw_inflate_offset0` and decodes to 64 bytes; test SHA256 `accbc01ef1b55b7f832d84c0ece73eb2bab97200bdb05f305c35604be8e8c7c1`; classifier unchanged at `624a51336859bab8aef2e7e7880b16d1282c9e883f459de05c8e3c1c4320cd77`. E145 and the [completion checkpoint](docs/checkpoints/2026-10-21-root-aud01-classifier-raw-inflate-offset0-complete.md) record both log hashes.
- Claimed `RE-02-DYNLINK-MAPPED-ZERO-LENGTH-01`, `ACTIVE` with zero failed tries. Graft and live source review confirm that `Image.mapped()` includes a zero-length range at the first segment's one-past-end address `0x1010`; no existing test pins this. Add one synthetic assertion in `scripts/test_ps4_dynlink.py`, without changing parser behavior. Claim: [zero-length mapped range](docs/checkpoints/2026-10-21-root-re02-dynlink-mapped-zero-length-claim.md).
- `slot23_raw_xrefs` completed only `scripts/test_audio_headers.py`; `launcher_review` will own only `scripts/test_ps4_dynlink.py` after this claim is published. Root owns shared documentation.
- At this checkpoint the next action was to publish E145 and the zero-length claim; publication and worker assignment completed in round 28 above.

## Session addendum 2026-10-21 round 26 (`/root`)

- Published the round 25 RELRO fixture and raw-DEFLATE claim in commit `62e2b677eee5ae4136d9c9bc672eff6e2e825566` (`test: verify RELRO image reads and claim raw inflate branch`); `git push origin main` exited 0 and both `HEAD` and `origin/main` resolve to that commit. See [publication checkpoint](docs/checkpoints/2026-10-21-root-round25-publication.md).
- Pre-commit checks passed: `python3 scripts/project_index.py` and `python3 scripts/project_index.py --check` (14,231 folders current); `trail build --check` (3,534 nodes in sync; deep meaning layer not built); `git diff --check`; `git diff --cached --check`; `python3 -I scripts/audit_publish_paths.py` (7 staged allowlisted paths); and `python3 -I scripts/audit_publish_paths.py --all` (1,531 paths). The post-push `git status --short --branch` showed `main` synchronized; only the pre-existing `native/api/CMakeLists.txt` modification and untracked `native/api/host/` remain outside the commit.
- `RE-02-DYNLINK-RELRO-IMAGE-FIXTURE-01` is `FIXTURE_VERIFIED`; `AUD-01-CLASSIFIER-RAW-INFLATE-OFFSET-00-FIXTURE-01` is `ACTIVE`. Both have zero failed tries. All actual game acceptance gates remain open.
- At this checkpoint the next action was to review and replay the raw-DEFLATE fixture; this completed in round 27 above.

## Session addendum 2026-10-21 round 25 (`/root`)

- Completed `RE-02-DYNLINK-RELRO-IMAGE-FIXTURE-01`, `FIXTURE_VERIFIED` with zero failed tries. Worker and root each pass `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_ps4_dynlink.py` (21/21). The synthetic `PT_SCE_RELRO` segment maps and reads its exact bytes; test SHA256 `dbac8eeb97a1fe98e7c9e5c3a16bfe4b133127214f5da5eda1849aa3ccb3efec`; parser unchanged at `28c53257a6f2bd58522f123098d41ca0d884222330a1bf18ce9f8935fb45eff9`. E144 and the [completion checkpoint](docs/checkpoints/2026-10-21-root-re02-dynlink-relro-image-complete.md) record both log hashes.
- Claimed `AUD-01-CLASSIFIER-RAW-INFLATE-OFFSET-00-FIXTURE-01`, `ACTIVE` with zero failed tries. Graft located the raw DEFLATE offset-0 probe branch with no matching test. Add one synthetic `wbits=-15` success fixture in `scripts/test_audio_headers.py`; codec identity and playback remain out of scope. Claim: [raw DEFLATE offset-0 fixture](docs/checkpoints/2026-10-21-root-aud01-classifier-raw-inflate-offset0-claim.md).
- `launcher_review` completed only `scripts/test_ps4_dynlink.py`; `slot23_raw_xrefs` will own only `scripts/test_audio_headers.py` after claim publication. Root owns the shared documentation.
- At this checkpoint the next action was to publish E144 and the new claim; publication and worker assignment completed in round 26 above.

## Session addendum 2026-10-21 round 23 (`/root`)

- Published the round 22 CRLF fixture and RELRO claim in commit `91d58a255b0e94dc271de24d43a1fd2ec0bc76c0` (`test: verify CRLF manifest records and claim RELRO coverage`); `git push origin main` exited 0 and both `HEAD` and `origin/main` resolve to that commit. See [publication checkpoint](docs/checkpoints/2026-10-21-root-round22-publication.md).
- Pre-commit checks passed: `python3 scripts/project_index.py` and `python3 scripts/project_index.py --check` (14,230 folders current); `trail build --check` (3,533 nodes in sync; deep meaning layer not built); `git diff --check`; `git diff --cached --check`; `python3 -I scripts/audit_publish_paths.py` (7 staged allowlisted paths); and `python3 -I scripts/audit_publish_paths.py --all` (1,528 paths). The post-push `git status --short --branch` showed `main` synchronized; only the pre-existing `native/api/CMakeLists.txt` modification and untracked `native/api/host/` remain outside the commit.
- `DATA-02-ARCHIVE-MANIFEST-CRLF-01` is `FIXTURE_VERIFIED` with zero failed tries. `RE-02-DYNLINK-RELRO-IMAGE-FIXTURE-01` is `ACTIVE` with zero failed tries. All real-game acceptance gates remain open.
- At this checkpoint the next action was to review and replay the RELRO fixture; this completed in round 25 above.

## Session addendum 2026-10-21 round 22 (`/root`)

- Completed `DATA-02-ARCHIVE-MANIFEST-CRLF-01`, `FIXTURE_VERIFIED` with zero failed tries. Worker and root each pass `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_archive_overlap_map.py` (20/20). The synthetic two-record CRLF fixture preserves exact paths and sizes; test SHA256 `07ab59c0500a99ad13d3d0c15d63e525a06061ccef34fe734ab5e76b796665b7`; parser unchanged at `25889f7ff85db9154ba209bed4913b1bb9e1a627e8a984ef7c11580b24e982d1`. E143 and [completion checkpoint](docs/checkpoints/2026-10-21-root-data02-manifest-crlf-complete.md) record both logs.
- Claimed `RE-02-DYNLINK-RELRO-IMAGE-FIXTURE-01`, `ACTIVE` with zero failed tries. Scout's Graft search found the `PT_SCE_RELRO` constant/header filter but no matching test. Add one synthetic `Image` fixture that checks mapped/read on a `PT_SCE_RELRO` segment; do not change parser behavior. Claim: [RELRO image fixture](docs/checkpoints/2026-10-21-root-re02-dynlink-relro-image-claim.md).
- `review_host_proto` completed the CRLF test file; `launcher_review` will own only `scripts/test_ps4_dynlink.py` after this claim is published. Root owns all task/evidence/handoff documentation.
- At this checkpoint the next action was to publish E143 and the RELRO claim; publication and worker assignment completed in round 23 above.

## Session addendum 2026-10-21 round 21 (`/root`)

- Published the round 20 batch in commit `d19067e952736880d643b9d521d9eaaa5316b455` (`test: record mapped-range fixture and manifest CRLF claim`); `git push origin main` exited 0 and both `HEAD` and `origin/main` resolve to that commit. See [publication checkpoint](docs/checkpoints/2026-10-21-root-round20-publication.md).
- Pre-commit checks: `python3 scripts/project_index.py` and `python3 scripts/project_index.py --check` exited 0 (14,230 folders indexed/current); `trail build --check` exited 0 (wiring graph in sync; the optional deep layer is not built); `git diff --cached --check` exited 0; `python3 -I scripts/audit_publish_paths.py` exited 0 for 9 staged allowlisted paths; `python3 -I scripts/audit_publish_paths.py --all` exited 0 for 1,525 paths. Logs are under `local/checkpoints/root-round20-claim/`.
- Post-commit `git status --short --branch` showed `main` synchronized with `origin/main`. The only remaining changes are the pre-existing `native/api/CMakeLists.txt` modification and untracked `native/api/host/`; neither was staged or published. A post-commit staged audit reported no staged files (exit 2); the staged audit immediately before commit passed.
- `RE-02-DYNLINK-MAPPED-RANGE-FIXTURE-01` and `DOC-ASSETS-AUD-01-XFF-COVERAGE-COUNT-01` are `FIXTURE_VERIFIED`; `DATA-02-ARCHIVE-MANIFEST-CRLF-01` is claimed `ACTIVE`. All have zero failed tries. Actual game boot/audio/save-load/Metal acceptance gates remain open.
- `review_host_proto` now owns only `scripts/test_archive_overlap_map.py` for the claimed CRLF fixture. Next exact action: review its patch, root-replay `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_archive_overlap_map.py`, and record the result.

## Session addendum 2026-10-21 round 20 (`/root`)

- Completed `RE-02-DYNLINK-MAPPED-RANGE-FIXTURE-01`, FIXTURE_VERIFIED with zero failed tries. The synthetic two-segment fixture asserts a contained mapped range is true, while a range crossing the first segment end and a nonzero range starting in the gap are false. Worker and root each pass `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_ps4_dynlink.py` (20/20); test SHA256 `f779cc8f7983c20efed84f2c17f35415f8daa718f8a812599986747b3b261695`; production parser unchanged at `28c53257a6f2bd58522f123098d41ca0d884222330a1bf18ce9f8935fb45eff9`. E141 and [completion checkpoint](docs/checkpoints/2026-10-21-root-re02-dynlink-mapped-range-complete.md) record both log hashes. No live ELF or loader claim.
- Completed `DOC-ASSETS-AUD-01-XFF-COVERAGE-COUNT-01`, FIXTURE_VERIFIED with zero failed tries. `docs/ASSETS.md` now accurately says “18 synthetic test methods”; the count command returns 18 and the test suite passes 18/18. `docs/ASSETS.md` SHA256 `ca0e0e495ef02a88a56a52589dda8d790c018162ff009d9ee5463558e9b85f87`; E142 and [completion checkpoint](docs/checkpoints/2026-10-21-root-doc-assets-aud01-xff-count-complete.md) record worker/root logs. This is a documentation count correction only; no codec or playback claim.
- Claimed `DATA-02-ARCHIVE-MANIFEST-CRLF-01`, ACTIVE with zero failed tries. The parser has an explicit CRLF strip at `scripts/audit_archive_overlap_map.py:66`; current tests cover LF and blank records but not CRLF. Add one synthetic two-record CRLF fixture to `scripts/test_archive_overlap_map.py`. Claim: [CRLF manifest fixture](docs/checkpoints/2026-10-21-root-data02-manifest-crlf-claim.md). After its claim publication, `review_host_proto` will own only that test file; root owns all documentation.
- At this checkpoint the next action was to publish E141/E142 and the CRLF claim; publication completed in round 21 above.

## Session addendum 2026-10-21 round 19 (`/root`)

- Claimed `DATA-02-ARCHIVE-MANIFEST-CRLF-01`, ACTIVE with zero failed tries. Graft's `trail grep "line.endswith" --in scripts/` found the parser's explicit CRLF branch at `scripts/audit_archive_overlap_map.py:66`. Live tests in `scripts/test_archive_overlap_map.py` use LF records and do not cover CRLF. Add a synthetic two-record CRLF fixture that asserts exact path/size output; parser behavior and real manifest interpretation are out of scope. Parent `DATA-02-ARCHIVE-OVERLAP-MAP-01` is ARTIFACT_VERIFIED. Claim: [CRLF manifest fixture](docs/checkpoints/2026-10-21-root-data02-manifest-crlf-claim.md).
- The implementation owns only `scripts/test_archive_overlap_map.py`; it is separate from round 18's `scripts/test_ps4_dynlink.py` and `docs/ASSETS.md`. `review_host_proto` will own the file after publication; root retains all task/evidence/handoff documentation.
- Acceptance command: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_archive_overlap_map.py`. Expected result: CRLF-delimited JSONL records parse to their exact declared paths and sizes. No test has run at claim time; this does not establish archive contents or runtime behavior.
- Next exact action: publish the claim, then assign its test-only file to `review_host_proto` and root-replay the focused suite.

## Session addendum 2026-10-21 round 18 (`/root`)

- Claimed `RE-02-DYNLINK-MAPPED-RANGE-FIXTURE-01`, ACTIVE with zero failed tries. The scout's Graft query `trail grep "mapped" --in scripts/` found `Image.mapped()` and only one existing direct test assertion, for a mapped BSS-tail range. `two_segment_image()` already supports checking a range contained in one segment, a range crossing its end into a gap, and a nonzero range beginning in the gap. Add test-only assertions in `scripts/test_ps4_dynlink.py`; do not change parser/runtime behavior. Dependencies `RE-02-DYNLINK-01` and `RE-02-DYNLINK-SEGMENT-SPAN-01` are FIXTURE_VERIFIED. Claim: [mapped-range fixture](docs/checkpoints/2026-10-21-root-re02-dynlink-mapped-range-claim.md).
- Claimed `DOC-ASSETS-AUD-01-XFF-COVERAGE-COUNT-01`, ACTIVE with zero failed tries. `docs/ASSETS.md` says “six synthetic fixtures,” while `scripts/test_audio_bank_header.py` currently has 18 test methods (`rg -c '^    def test_' ...` → 18). Correct the prose to “18 synthetic test methods” in `docs/ASSETS.md` only. XFF characterization, reader identity, and exact-path-cardinality dependencies are FIXTURE_VERIFIED. Claim: [XFF test-count documentation correction](docs/checkpoints/2026-10-21-root-doc-assets-aud01-xff-count-claim.md).
- Both claim tasks are file-disjoint from each other and the completed audio-header fixture. After this claim publication, `launcher_review` will own only `scripts/test_ps4_dynlink.py`; `slot23_raw_xrefs` will own only `docs/ASSETS.md`. Root retains sole ownership of `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, and checkpoints. Their acceptance commands and no-runtime-claim boundaries are recorded in the claim files.
- Claim publication: `3709a5da33f066cbb6936935545f86ec73ccd409` is on `origin/main`; `git push origin main` exited 0 and advanced `517defa..3709a5d`. This commit publishes E140, both claim records, and both active task rows. Staged audit accepted eight paths; full tracked-path audit accepted 1,522 paths. Folder index check reports 14,227 current; `trail build --check` passes with 3,531 nodes; `git diff --check` passes. HEAD and `origin/main` both resolve to this commit. The prior audio-claim publication record is `517defab025c6e2c8ef84330d4fbe86fd4fda5eb`. Logs are under `local/checkpoints/root-round18-claims/`. Only the preexisting escalated `native/api/CMakeLists.txt` and `native/api/host/` remain outside the published scope.
- Next exact action: dispatch the two published tasks to their assigned workers, then independently replay their acceptance commands and publish completion evidence.

## Session addendum 2026-10-21 round 17 (`/root`)

- Completed `AUD-01-CLASSIFIER-ZLIB-OFFSET-02-FIXTURE-01`, FIXTURE_VERIFIED with zero failed tries. `scripts/test_audio_headers.py` adds a 64-byte synthetic payload compressed with zlib and prefixed with `b'\x00\x01'`; it asserts successful `zlib_offset2` selection and exact decoded length. Worker and root each pass `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_audio_headers.py` (20/20); test SHA256 `888a6abcf077d6a3d9f861bf7d80acb163540fbf873d7b288b6fb45beaca8c45`; classifier source unchanged at SHA256 `624a51336859bab8aef2e7e7880b16d1282c9e883f459de05c8e3c1c4320cd77`. E140 and [completion checkpoint](docs/checkpoints/2026-10-21-root-aud01-classifier-zlib-offset2-complete.md) record both log hashes. Synthetic branch behavior only; no real codec, game asset, or playback claim.
- Next exact action: complete the next independent bounded task after confirming no file-ownership overlap; all game-level audio and playback gates remain open.

## Session addendum 2026-10-21 round 16 (`/root`)

- Claimed `AUD-01-CLASSIFIER-ZLIB-OFFSET-02-FIXTURE-01`, ACTIVE with zero failed tries. Graft found `decompress_probe()`'s `zlib_offset2` branch in `scripts/classify_audio_headers.py:81-127`; live test coverage accepts zlib at offset 0 but has no offset-2 regression. Add one synthetic prefixed-zlib fixture to `scripts/test_audio_headers.py`; production code and real archive interpretation are out of scope. The root independently checked that no matching task row existed before the claim.
- Acceptance command: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_audio_headers.py`. Expected assertion: a two-byte prefix followed by `zlib.compress(payload)` is reported as `zlib_offset2` and returns the expected decoded length. No test has run at claim time. Claim checkpoint: [zlib offset-2 fixture](docs/checkpoints/2026-10-21-root-aud01-classifier-zlib-offset2-claim.md). Root retains sole ownership of `HANDOFF.md`, `docs/TASKS.md`, and evidence records; `review_host_proto` will own only `scripts/test_audio_headers.py` after claim publication. The fixture demonstrates synthetic classifier behavior only, not codec identity, game asset use, or playback.
- Claim publication: `6685c7f73b800e49b9905cc1b2e289b20a7f087c` is on `origin/main`; `git push origin main` exited 0 and advanced `59dad26..6685c7f`. Staged audit accepted four paths; full tracked-path audit accepted 1,519 paths. `python3 scripts/project_index.py --check` reports 14,224 folders current, `trail build --check` passes with 3,530 nodes / 4,746 edges, and `git diff --check` passes. HEAD and `origin/main` both resolve to the claim commit. Publication logs are under `local/checkpoints/root-round14-aud01-claim/`. Next exact action: dispatch `scripts/test_audio_headers.py` to `review_host_proto`, then root replays and publishes the result.

## Session addendum 2026-10-21 round 10 (`/root`)

- `RE-01-UNWRAP-INPUT-SIDECAR-COLLISION-01` is FIXTURE_VERIFIED with zero failed implementation
  tries. Worker and root each pass 6/6; E132 records the expected pre-guard failure, both passing
  logs and source hashes. Completion and claim records were pushed in `c8dd0b3`; no game gate advanced.
- AUD-PUBLISH-SECRET-NAME-FIXTURE-01 is FIXTURE_VERIFIED with zero failed tries. Worker and
  root each pass 7/7 (E133); the test-only cases cover nested .ENV.local and CREDENTIALS.txt
  names. Completion and evidence were pushed in 8d463ab; the production audit stayed unchanged.
- Graft confirmed the deny regex in `scripts/audit_publish_paths.py:30-42`; current tests cover
  nested forbidden components and file extensions but no `.env` or `credentials` names. Both
  prerequisite audit tasks are FIXTURE_VERIFIED.
- Acceptance command: PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_audit_publish_paths.py.
  Worker and root each pass 7/7; source graph is rebuilt and in sync after the new test method.
  The completion checkpoint records private log and source hashes.
- Next exact action: select and claim another bounded, non-escalated task. Preserve escalated host paths and the slot-23 blocker.
  then record E133 and publish the Task B completion checkpoint. Escalated host paths remain out.
- The secret-name claim checkpoint was reviewed, contains no credentials, and is published in c8dd0b3 despite the .gitignore:76 *secret* rule. The worker's test-only scope is now complete.
  `c8dd0b3` despite the `.gitignore:76` `*secret*` rule. The test-only work is assigned to
  `launcher_review` and is in progress.

## Session addendum 2026-10-21 round 9 (`/root`)

- Completed and pushed `DOC-FOLDER-INDEX-ADMIN-DIR-01` in commits `94a8cbc` and `6811f52`.
  Worker and root each pass the synthetic nested `.git`/`__pycache__` check 5/5; E131 records the
  log hashes. No source behavior or live inventory changed.
- Claimed `RE-01-UNWRAP-INPUT-SIDECAR-COLLISION-01`, ACTIVE with zero failed implementation tries.
  `unwrap()` can write `destination.with_suffix(".identity.json")` over the source if that sidecar
  aliases the input; the task adds a pre-write same-file guard and a synthetic regression.
  Claim: [docs/checkpoints/2026-10-21-root-re01-unwrap-input-sidecar-collision-claim.md](docs/checkpoints/2026-10-21-root-re01-unwrap-input-sidecar-collision-claim.md).
- Graft located the unguarded sidecar write at `scripts/unwrap_self.py:27` and showed callers from
  the three existing wrapper tests plus the CLI. Prerequisites `RE-01` and
  `RE-01-ELF-PHDR-TRUNCATION-FIXTURE-01` are satisfied.
- Acceptance command: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_project_tools.py`. Worker
  will first record a pre-guard failing collision assertion, then add the guard and rerun; the
  expected red fixture is not counted as a failed implementation try. No test has run for this task.
- Claim commit `90a5d99` is pushed to `origin/main`; `git push origin main` exited 0 and advanced
  `6811f52..90a5d99`. Root verified `HEAD` and `origin/main` both resolve to
  `90a5d99413dfe53236828f3f963121ac7045186c`. Next exact action: assign
  `scripts/unwrap_self.py` and `scripts/test_project_tools.py` to `slot23_raw_xrefs`. Root remains
  the sole integrator for handoff/task/evidence documentation. Keep all user game files read-only
  and host escalation files unstaged.

## Session addendum 2026-10-21 round 8 (`/root`)

- `DATA-02-PSARC-STORED-BLOCK-TRUNCATION-01` completion was committed and pushed as `fb28481`.
  This updates the completion checkpoint's previous next-action entry; E130 and the synthetic-only
  limits remain as recorded.
- Claimed `DOC-FOLDER-INDEX-ADMIN-DIR-01`, ACTIVE with zero failed tries. Add one temp-root test in
  `scripts/test_project_tools.py` proving nested `.git` and `__pycache__` trees do not make
  `docs/FOLDERS.md` stale. No source/index behavior change or live index regeneration is in scope.
  Claim: [docs/checkpoints/2026-10-21-root-doc-folder-index-admin-dir-claim.md](docs/checkpoints/2026-10-21-root-doc-folder-index-admin-dir-claim.md).
- Graft located the ignored-directory pruning in `scripts/project_index.py:27-40` and check mode
  in `:59-77`; the existing stability test covers `.DS_Store` and new folders but not these
  administrative directories. Prerequisite: none.
- Acceptance command: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_project_tools.py`. Worker and
  root each pass 5/5. E131 and the completion checkpoint record the exact log hashes and test-only
  limits. `slot23_raw_xrefs` owned only `scripts/test_project_tools.py`; root remained the sole
  `HANDOFF.md` integrator.
- Claim commit `51f6549` is pushed to `origin/main`; `git push origin main` exited 0 and advanced
  `fb28481..51f6549`. Root verified `HEAD` and `origin/main` both resolve to
  `51f654923eb76ba2d10c6fcf88f222089ded84e9`. `slot23_raw_xrefs` is now assigned only
  `scripts/test_project_tools.py`; implementation and acceptance result are pending.
- During implementation, `trail build --check` reported the expected stale graph after the test file
  gained its new test symbol. `trail build` then exited 0 and the subsequent check passed; the graph
  remains in sync. No production code changed. Keep escalated `native/api/CMakeLists.txt` and
  `native/api/host/` outside this task and all commits.
- Final folder index after adding the completion checkpoint: `python3 scripts/project_index.py`
  exited 0 with 14,208 folders; `docs/checkpoints` now reports 236 files. `trail build --check`
  exited 0 with the wiring graph in sync (deep layer remains unbuilt).
- Publication gates pass: the staged path audit accepts 7 allowlisted files; the full tracked path
  audit accepts 1,502 paths; `git diff --cached --check` is clean. Escalated host files remain
  unstaged. Completion commit `94a8cbc` was pushed to `origin/main`; the push advanced
  `51f6549..94a8cbc`. `HEAD` and `origin/main` both resolve to
  `94a8cbc922a32ab37d8215cbac16d98c6f56077f`.
- Next exact action: select and claim another bounded, non-escalated project task. Preserve the
  three-try host escalation and the slot-23/Ghidra blocker; no game gate advanced from this fixture.

## Session addendum 2026-10-21 round 7 (`/root`)

- Completed and pushed `RE-02-DYNLINK-PHDR-ENTRY-SIZE-01` in commit `35370fc`. The test-first
  `e_phentsize=55` fixture failed before the guard; worker and root focused runs pass 18/18 after
  it. E128 and the completion checkpoint record synthetic-only limits.
- Completed `DATA-02-OVERLAP-MAP-UNSHARED-SIZE-VALIDATION-01`, FIXTURE_VERIFIED: all member-map
  values must be exact nonnegative integers, including unshared paths. The `-1` and `True` cases
  failed before the change; worker and root each pass 18/18 after it. Claim and completion records:
  [claim](docs/checkpoints/2026-10-21-root-data02-overlap-map-unshared-size-claim.md),
  [completion](docs/checkpoints/2026-10-21-root-data02-overlap-map-unshared-size-complete.md).
- Graft located the size check inside only the shared-path loop at
  `scripts/audit_archive_overlap_map.py:96-138`; callers are `build_report`, synthetic tests, then
  `main`. Existing contract says exact nonnegative integers, but unshared values bypass it.
- Claim commit `f1a98c5` is pushed to `origin/main`; worker `launcher_review` now owns the two claimed
  files for a test-first unshared negative/bool fixture and whole-map validation. No implementation
  result is integrated yet.
- Root reviewed the two-file diff and independently replayed
  `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_archive_overlap_map.py`: exit 0, 18/18.
  E129 and private log hashes are recorded in the completion checkpoint. No archive data or game
  behavior is in scope.
- Commit `c0c7177` is pushed to `origin/main`; it includes the whole-map validation, two synthetic
  unshared-size cases, and E129. No real manifest/archive behavior is claimed.
- Completed `DATA-02-PSARC-STORED-BLOCK-TRUNCATION-01`, FIXTURE_VERIFIED: a 16-byte synthetic raw
  block truncated by one byte raises the exact decoded-size `PsarcError`. Worker and root each pass
  the full PSARC suite 30/30; parser source is unchanged. Claim and completion records:
  [claim](docs/checkpoints/2026-10-21-root-data02-psarc-stored-block-truncation-claim.md),
  [completion](docs/checkpoints/2026-10-21-root-data02-psarc-stored-block-truncation-complete.md).
- Graft confirms `Psarc.read` has an existing raw block-size check after `f.read(block_size)` and
  existing tests cover compressed truncation only. The fixture should assert the exact decoded-size
  mismatch on a truncated raw block; no parser source change is in scope.
- Claim commit `f41b202` is pushed to `origin/main`; `slot23_raw_xrefs` now owns only
  `scripts/test_psarc.py` for the synthetic raw-block truncation fixture. Worker result is pending.
- Root reviewed the test-only diff and independently replayed
  `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_psarc.py`: exit 0, 30/30. E130 and exact
  private log hashes are recorded in the completion checkpoint. No real archive/payload claim.
- Publication follow-up: the completion files were committed as `fb28481` and pushed to
  `origin/main`; current HEAD and `origin/main` both resolve to that commit. The commit included
  the test, E130, task/handoff updates, generated folder index, and completion checkpoint.

## Session addendum 2026-10-21 round 6 (`/root`)

- Completed and published `AUD-01-XFF-READER-IDENTITY-FIXTURE-01`: commits `14fcc86` and
  `1260c96` are on `origin/main`; worker and root both pass 16/16 synthetic tests; E127 records
  the test-only path expectation correction and unchanged parser hash.
- Completed `RE-02-DYNLINK-PHDR-ENTRY-SIZE-01`, FIXTURE_VERIFIED: reject ELF64 program headers
  whose `e_phentsize` is not 56. The synthetic `e_phentsize=55` assertion failed before the guard;
  worker and root each pass 18/18 after it. Claim and completion records:
  [claim](docs/checkpoints/2026-10-21-root-re02-phdr-entsize-claim.md),
  [completion](docs/checkpoints/2026-10-21-root-re02-phdr-entsize-complete.md).
- Graft located `program_headers` at `scripts/ps4_dynlink.py:46-52`; it decodes records with a
  56-byte stride but does not check ELF header `e_phentsize`. Its callers are parser → build → main.
  Existing synthetic tests cover class/data/machine and truncated tables, but not this field.
- Claim commit `bad72a6` was pushed to `origin/main`; `slot23_raw_xrefs` owns the two claimed
  source/test files. The worker is adding a synthetic test, recording the pre-guard failure, then
  adding the narrow check and rerunning the focused suite. No code result is integrated yet.
- Root reviewed the two-file diff and independently replayed
  `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_ps4_dynlink.py`: exit 0, 18/18. E128 and
  exact private log hashes are recorded in the completion checkpoint; only synthetic parser
  behavior is demonstrated.
- Next exact action: refresh indexes, path-audit the staged source/test/docs, commit, and push the
  task completion. No live ELF, loader, or game milestone is implied.

## Session addendum 2026-10-21 round 5 (`/root`)

- Completed `AUD-01-XFF-READER-IDENTITY-FIXTURE-01`, FIXTURE_VERIFIED with one failed fixture
  attempt: synthetic tests cover full summary/archive/member identity, manifest mismatch, and reader
  closure on success/error. Worker changed only `scripts/test_audio_bank_header.py`; parser source
  was unchanged. Worker retry 2 and root replay each passed 16/16. Completion record:
  [docs/checkpoints/2026-10-21-root-aud01-xff-reader-identity-claim.md](docs/checkpoints/2026-10-21-root-aud01-xff-reader-identity-claim.md).
- The worker's first focused run, `python3 -I scripts/test_audio_bank_header.py`, failed 1 of 16
  tests because the success fixture expected a resolved summary path, while `inspect_members`
  preserves the supplied summary path verbatim. The mismatch/error cleanup test passed. Evidence:
  `local/validation/aud01-xff-reader-identity-fixture-01/attempt-1.log`; the incorrect expected
  value was test-only. The one-line fixture correction passed 16/16 on worker retry 2; root replay
  is pending. No parser source changed.
- Graft located the reader identity checks at `scripts/audio_bank_header.py:101-198`; its only
  indexed references are the definition and CLI call. The accepted fixture will mock `Psarc` and
  use temporary archive/summary/manifest files, so it does not read a game archive or invoke zlib.
- Root reviewed the diff and replayed `python3 -I scripts/test_audio_bank_header.py`: exit 0,
  16/16, log `local/validation/aud01-xff-reader-identity-fixture-01/root-replay.log` (SHA256
  `40e7e701b0fb7520dec152943e03896c5835fbb8b2ebe4c5ca0e04b62aeaeb2b`). Test SHA256
  `c5a09bccf054d0cd186144a22b602cb2777993857cd87d5ad3846cc04e230912`; parser SHA256 remains
  `bdad7d192b0ecdb929daf5147a139e7545fd5c546f01ed4ffb4005f2cea9f9c0`.
- Commit `14fcc86` (`test(audio): verify inspect_members identity and cleanup`) was pushed to
  `origin/main`; this checkpoint's final documentation update will be committed and pushed
  separately. The task is FIXTURE_VERIFIED with no real archive, codec, playback, or G1 claim.
- Next exact action: claim `RE-02-DYNLINK-PHDR-ENTRY-SIZE-01` as a bounded synthetic parser guard
  and regression fixture, then delegate its two owned files after the claim is published.

## Session addendum 2026-10-21 round 4 (`/root`)

- Resumed `APP-01-LAUNCHER-CORECLIENT-01` for a bounded live Mac UI and accessibility inspection
  of the already-built launcher. No source change or game-core launch is in scope. Claim record:
  [docs/checkpoints/2026-10-21-root-app01-launcher-runtime-claim.md](docs/checkpoints/2026-10-21-root-app01-launcher-runtime-claim.md).
- The existing launcher checkpoint records 89 passing Swift tests and an app-process smoke start,
  but no observed window, VoiceOver, Finder launch, or signed bundle. The executable currently
  exists at `local/build/launcher-agent-e/out/Products/Debug/SoCLauncher`.
- The first native UI inventory was blocked: `cua.getState()` reports the Mac is locked and needs
  manual unlock. No app was launched and no UI state was captured. The user has been asked to
  unlock it; the task remains PARTIAL with no failed implementation tries. Private result:
  `local/checkpoints/root-app01-launcher-ui-cua-blocked.log`.
- Next exact action after the user unlocks the Mac: bind the existing executable with CUA, capture
  its AX state and screenshot, and inspect visible readiness/state. Do not interpret the UI as game
  boot, audio, saving, or G4 evidence. VoiceOver, Finder launch, and signing remain NOT RUN.

## Session addendum 2026-10-21 round 3 (`/root`)

- Claimed `CPU-01-NATIVE-SLOT24-47-01`, status ACTIVE, attempt 0. Review and verify the existing
  uncommitted native counterpart for constructor slots 24–47, with a test-only ahead table that
  cannot bypass unresolved production slot 23. Claim record:
  [docs/checkpoints/2026-10-21-root-cpu01-native-slot24-47-claim.md](docs/checkpoints/2026-10-21-root-cpu01-native-slot24-47-claim.md).
- Claim-time checkout: `main` at `57a5282`, matching `origin/main`. An unfinished native diff was
  already present before this task under `native/core/`, CMake registrations, a new ahead
  constructor fixture, and a separate `native/api/host/` implementation. Root has since reviewed
  and corrected comments/test scope and the slot-table checker; host protocol work was separately
  claimed below.
- Graft located the ahead-table symbol in `native/core/startup_ctors.{hpp,cpp}`, its state in
  `registration_thunks.hpp`, and the focused test. The test-only range is kept outside production.
- Current CPU-01 result: `check_ctor_slots.py`, `verify_ctor_family.py`, and `check_ctor_thunks.py`
  exited 0; `test_registration_thunks_ahead` built and focused CTest passed 1/1 under the ARM64
  ASan/UBSan build. The test asserts production stops at index 23; no G1 or game startup claim.
  The first missing-target build failure and CMake recovery are recorded in the linked checkpoint.
- First-build checkpoint: `check_ctor_slots.py`, `verify_ctor_family.py`, and `check_ctor_thunks.py`
  exited 0. The focused build command exited 2 before compilation because Make has no rule for
  `test_registration_thunks_ahead`; this was failed try 1, with no native test run. The cache was
  then confirmed to point to this checkout and explicitly reconfigured as recorded below. See
  [the checkpoint](docs/checkpoints/2026-10-21-root-cpu01-slot24-47-build-target-failure.md).
- Also claimed `APP-01-CORE-HOST-01`, status ACTIVE, attempt 0, for the existing native host
  protocol. A parallel read-only audit found the hello limit can disagree with its configured
  parser limit, input errors are returned as clean EOF, the closed-stdout test accepts the wrong
  exit code, and the parent test writer can receive SIGPIPE. Worker owns only `native/api/host/`
  sources/tests; root remains the sole integrator for handoff/task/evidence files. Record:
  [docs/checkpoints/2026-10-21-root-app01-core-host-claim.md](docs/checkpoints/2026-10-21-root-app01-core-host-claim.md).
- Host task is now `NEEDS_PRINCIPAL` after three failed targeted tries. Do not change or rerun the
  host task until the principal records a reassessment; see
  [the escalation](docs/escalations/APP-01-CORE-HOST-01.md). `start` still returns
  `ENGINE_NOT_IMPLEMENTED`; G4 remains NOT RUN.
- Host worker implementation is now ready in the four assigned `native/api/host/` files. It adds
  configured hello limits, distinguishes stream read errors from EOF, tightens the exit-code check,
  and handles SIGPIPE/EPIPE in the parent pipe writer. The initial implementation checkpoint was
  written before root's build verification. See
  [the implementation checkpoint](docs/checkpoints/2026-10-21-root-app01-core-host-implementation.md).
- CMake was reconfigured for this `native/` tree with Unix Makefiles, arm64, and `SOTC_SANITIZE=ON`.
  The source graph was rebuilt with the original scopes preserved plus `native`, `launcher`, and
  `tests`; `trail build --check` passes and Graft resolves the new ahead-table test file. See
  [the failed-build checkpoint](docs/checkpoints/2026-10-21-root-cpu01-slot24-47-build-target-failure.md).
- `CPU-01-NATIVE-SLOT24-47-01` is now FIXTURE_VERIFIED. Full native build exited 0 and 295/295
  CTests passed with only the escalated `sotc_host_protocol` test excluded. See
  [the completion checkpoint](docs/checkpoints/2026-10-21-root-cpu01-slot24-47-complete.md) and
  evidence row E120. Slot 23 remains the production blocker; G1 is unchanged.
- Claimed `CPU-01-SLOT23-CONSUMERS-01` for a name-free trace of the slot-23 table and consumers.
  Current status is PARTIAL: a decoded pointer path passes a table pointer to `0xe70af0`, while a
  separate reachable path hashes an object-row pointer and scans a per-object hash list. The
  pointer consumer has 31 unresolved indirect transfers, and Ghidra 12.1.4 timed out during
  whole-program auto-analysis at 330 seconds, so string necessity and function ownership remain
  unknown. No names, native data, or registration were added. See
  [the claim](docs/checkpoints/2026-10-21-root-cpu01-slot23-consumers-claim.md) and
  [the partial checkpoint](docs/checkpoints/2026-10-21-root-cpu01-slot23-consumers-partial.md).
- `DATA-02-PAKN-TOC-DIGEST-UNIQUENESS-01` is ARTIFACT_VERIFIED: a root-owned metadata aggregate
  over the pakN manifest found 64,108 rows and 64,108 unique TOC values, with no repeated value
  groups. Two executions were byte-identical; input SHA256 remained
  `ee7c927e8ae3b28b20d61bb460c0c3c6c783cf0a7242df366f968949d5d21075`. This establishes
  uniqueness in this manifest only; digest semantics remain UNKNOWN. See E122 and the
  [completion checkpoint](docs/checkpoints/2026-10-21-root-data02-pakn-toc-digest-complete.md).
- Next exact action: claim `DATA-02-ARCHIVE-OVERLAP-TOC-DIGEST-01` for a separate aggregate of
  TOC-value equality across exact-path overlaps in the seven existing manifests, then independently
  replay it from root. Preserve only aggregate counts; no payload or digest-semantics claim.
- `DATA-02-ARCHIVE-OVERLAP-TOC-DIGEST-01` is ARTIFACT_VERIFIED: root compared the TOC field only
  across exact-path overlaps in all 21 pairs from the seven private manifests. Two executions
  matched byte-for-byte and the shared-path total matched E77: 40,216 occurrences, 4,479 equal
  values and 35,737 different values. These are metadata comparisons; digest meaning, content
  equality, overlay precedence and runtime use remain UNKNOWN. See E123 and the
  [completion checkpoint](docs/checkpoints/2026-10-21-root-data02-overlap-toc-digest-complete.md).
- `DATA-02-PSARC-DECODED-LENGTH-MISMATCH-01` is FIXTURE_VERIFIED: added a synthetic compressed
  member whose declared length is one byte shorter than its decoded final block. The exact existing
  error (`block 4 decoded to 16 bytes, expected 15`) is asserted; `python3 -I scripts/test_psarc.py`
  passed 27/27. Parser behavior is unchanged; see E124 and the
  [completion checkpoint](docs/checkpoints/2026-10-21-root-data02-psarc-decoded-length-complete.md).
- `DATA-02-TOC-VALUE-DISTINCT-PATH-REUSE-01` is ARTIFACT_VERIFIED: root independently replayed the
  aggregate across seven hash-bound manifests (170,451 rows). Of 130,235 distinct values, 35,737
  occur on multiple exact paths (71,474 row associations); 4,479 exact-path/value associations
  repeat across archives (8,958 rows), matching E123. Worker runs and identities match byte-for-byte;
  aggregate reports contain no identifiers. Semantics remain UNKNOWN; see E126 and the
  [completion checkpoint](docs/checkpoints/2026-10-21-root-data02-toc-distinct-path-reuse-complete.md).
- `DATA-02-MANIFEST-JSON-SYNTAX-01` is FIXTURE_VERIFIED: malformed nonblank JSONL now raises
  `PsarcError` with the file and 1-based line. The old parser produced the expected two
  `JSONDecodeError` test errors; the fixed parser passed 29/29, including a blank-line test preserving
  the previous behavior. Root replay passed; see E125 and the
  [completion checkpoint](docs/checkpoints/2026-10-21-root-data02-manifest-json-syntax-complete.md).
- Next exact action: continue `CPU-01-SLOT23-CONSUMERS-01` with a bounded name-free trace of the
  `0xe70af0` indirect targets and object/vtable source. Preserve unresolved targets and do not
  import original strings or register slot 23 without the principal's decision. The PSARC
  negative-limit behavior question remains open; do not change that API behavior yet.
- After reconfigure, `test_registration_thunks_ahead` built and its focused CTest passed 1/1.
  The host library and child executable build. The first host target build failed at line 508
  because `WIFEXITED` received temporary `reap(unused)`; storing status in a local made the target
  compile. Focused CTest then failed at line 333 because the output-failure fixture did not make the
  stream fail. After making flush fail deterministically, try 3 reached the bad-stdin process test,
  which did not exit with expected code 4. The actual wait status was not printed; the task is
  escalated and must not be retried without principal reassessment. See
  [the host build checkpoint](docs/checkpoints/2026-10-21-root-app01-core-host-build-failure.md)
  and [the output-fixture checkpoint](docs/checkpoints/2026-10-21-root-app01-core-host-output-test-failure.md),
  plus [the escalation](docs/escalations/APP-01-CORE-HOST-01.md).

## Session addendum 2026-10-21 round 2 (orchestrator)

- Native tests **294/294**; all **287** `tools/lift` checkers pass; launcher **80+9** tests pass.
- **Slot 11 is no longer the blocker.** Reflection scope showed the 74-record tree is lazy (vtable getters only);
  slot 11 registered with `LazyParamDefTree` returning `schema_not_available`. Slots 12–19 and 21 registered
  (registration-only). Checker verifies **118** RVA/index pairs. **First unresolved: index 23, RVA `0xe71990`**
  (needs original name strings; hash is djb2-XOR, see native `name_hash`) — open decision: import original names as data?
- Save: `save_chunk_codec` typed codec; profile-chunk facts in `docs/checkpoints/2026-10-21-agentH-sav01-profile-chunks.md`.
  Unresolved: tag 4 vs 8; case-folding in name hash is inference.
- `LauncherModel` now routes through `CoreClient`; no runtime UI proof yet.
- Next: decide slot-23 name-string import; slots 24–47 family (descriptor_mutex_atexit); 6 tail-call leaf bodies; child-process core client.

## Session addendum 2026-10-21 (orchestrator, 4 parallel workers)

- Batch 34: 12 more call-free leaf ctors lifted (unregistered). Native tests now **281/281**.
- `native/core/save_stream`: opaque-payload tagged-chunk container + atomic file I/O (fixture only).
- `native/api/` C ABI 1.0 (`add_subdirectory(api)` now in `native/CMakeLists.txt`) + Swift `CoreClient`/`NativeCoreClient`;
  start() reports ENGINE_NOT_IMPLEMENTED. `LauncherModel` is not yet routed through `CoreClient`.
- Name hash for `0xe71990` is djb2-XOR at `0xe70aa0` (the ctor itself has no call); native `name_hash` added.
  Lifting the 14-record table still needs a decision on importing original names as data.
- Vtable `0x15f9ef0` mapped (222 slots; slot 12 = `0xf06330`).
- Known pre-existing checker failures (3 of 279): `check_fb747f0.py`, `check_fb76950.py` (tests absent from CMake),
  `check_math_ctor_clones.py` (asserts distinct clone cells, contradicts shared-range finding). Not fixed.
- Next: route `LauncherModel` via `CoreClient`; remaining 7 true leaves; fix the 3 checkers; slot 11 is still the blocker.

## Start here

Read [INDEX](docs/INDEX.md), [ROADMAP](ROADMAP.md), [TASKS](docs/TASKS.md), [EVIDENCE](docs/EVIDENCE.md),
[CTOR_CLASSES](docs/CTOR_CLASSES.md), then the latest checkpoints:
- [Constructor family map (573 slots)](docs/checkpoints/2026-10-21-root-cpu01-ctor-family-map.md)
- [Slot 10–47 family verification](docs/checkpoints/2026-10-21-root-cpu01-slot10-47-family.md)
- [Slot 12 structure](docs/checkpoints/2026-10-21-root-cpu01-slot12-struct.md),
  [slot 13](docs/checkpoints/2026-10-21-root-cpu01-slot13-struct.md),
  [slots 14–19](docs/checkpoints/2026-10-21-root-cpu01-slot14-19-family.md)
- Native batches [30](docs/checkpoints/2026-10-21-root-native-batch-30.md),
  [31](docs/checkpoints/2026-10-21-root-native-batch-31.md),
  [32](docs/checkpoints/2026-10-21-root-native-batch-32.md),
  [33 (analysis-only)](docs/checkpoints/2026-10-21-root-native-batch-33.md),
  [SAV chunk header](docs/checkpoints/2026-10-21-root-native-batch-sav01-chunk-header-01.md),
  [SAV checksum edges](docs/checkpoints/2026-10-21-root-native-batch-sav01-checksum-edge-01.md),
  [slot 21 constant table](docs/checkpoints/2026-10-21-root-cpu01-ctor21-const-table.md),
  [static-cleanup thunks](docs/checkpoints/2026-10-21-root-cpu01-static-cleanup-thunks.md),
  [family native helper](docs/checkpoints/2026-10-21-root-cpu01-family-native-helper.md)

## Current verified state

- Native tests: **265/265 pass** (`cmake --build local/build/native -j 8 && ctest --test-dir
  local/build/native --output-on-failure`), ASan+UBSan on, Mach-O arm64.
- Constructor-table checker: `python3 -I tools/lift/check_ctor_slots.py` verifies **108**
  RVA/index pairs; slots 0–10 are recovered; slot 11 is the explicit blocker. Lifted-but-unregistered
  gaps 50/70/82/147 are now registered and asserted.
- Launcher: **42 Swift tests / 7 suites** (`swift test --package-path launcher --scratch-path
  local/build/launcher`); adds tested accessibility copy, diagnostics text, ordered asset problems
  and lifecycle copy. Still no core ABI, no runtime UI/VoiceOver/Finder proof, G4 NOT PASSED.
- Project scripts: **103 Python tests** (`cd scripts && python3 -I -m unittest discover -s . -p
  'test_*.py'`). `python3 -I -m unittest <module>` cannot import on this interpreter — use
  `discover -s .` or the direct script path.
- RE-01-PERSISTENT-01: the saved Ghidra baseline still reopens read-only `-noanalysis`
  (38,845 functions, 9 blocks). See `local/checkpoints/root-20261020-reopen-baseline.txt`.

## What changed in this session (31 bounded tasks)

Constructor structure (all offline, all replayed by a verifier):
- **`docs/CTOR_CLASSES.md`** classifies all 573 table entries by reachable body: 227
  `leaf_vec_copy`, 12 `guarded_const`, **34 `descriptor_mutex_atexit` in contiguous runs 10–19 and
  24–47**, 300 `other`, 0 `undecoded`.
- Every one of the 34 family slots publishes **two `.bss` globals 0x10 apart**, one `malloc(0x10)`
  sized from `[0x12d2d1c]`, one mutex init through `0xb30950`
  (`pthread_mutexattr_init/settype/setprotocol/init/attr_destroy`), and exactly one
  `__cxa_atexit(0xe44a10, &global, 0x1604218)`; `0xe44a10` is `pthread_mutex_destroy` + `free`.
- Slots 12–19 are *registration thunks*, not class constructors. Slot 12's 205-byte body ends at
  `0xf0640d`; the slot-11 class constructor `0xf06410` starts immediately after and is never called
  by it.
- Six guarded-constant constructors (`0xdd91b0`, `0xdd7e30`, `0xdd4210`, `0xd39b00`, `0xd366d0`,
  `0xd33b50`) are now lifted with all 9 guarded cells and their unguarded tails; they share one
  guarded cell range, so only one of them can do meaningful work at runtime.
- Three AVX ctors (`0xd8fa40`, `0xd86ed0`, `0xd5a280`) are lifted; `0xd5a280` fills a **2048-byte**
  int32 identity table (the loop counter counts dwords).
- Batch 33 (`0xe71990`, `0xdf94c0`, `0xde0160`) is analysis-only on purpose: `0xe71990` needs the
  unknown name-hash function and original strings; `0xdf94c0` and `0xde0160` call `_FSin`/`memset`
  and a `ret`-stub `__cxa_atexit` cleanup.
- Slot 21 (`0xee1130`) is lifted as a byte-exact 128-byte constant table (15 stores into
  `0x33540f0..0x3354170`; the checker models four register/vector reloads). Cell meanings are
  unknown, so the table stays untyped.
- **15 further entries (515, 522, 523, 525, 527, 528, 531, 533, 542, 546, 547, 549, 551, 564, 565)
  are 33-byte static-destructor registration thunks**: zero one `.bss` cell, then tail-call
  `__cxa_atexit(cell, destructor, 0x1604218)`. Verified as a family; not lifted because the
  destructors are unresolved (`0x1bdae0` alone calls `0x1bede0` and tail-jumps `0x170b0`).
- `DescriptorRegistration` is the tested native lifetime model for the 34-slot family (one-shot
  publish, owned recursive+PI mutex, explicit errors) — deliberately not the original heap block or
  atexit ordering, and not registered.

Coverage statement: **every constructor-table index 0–149 now has a native lift or a documented
dossier**, and slots 21/515/522/523/525/527/528/531/533/542/546/547/549/551/564/565 are covered by
the two families above. Outside that prefix, 54 call-free `other` bodies remain unlifted (several
are the same 16-byte copy or YMM-store idioms already lifted in batches 32/33) plus the ~240 `other`
bodies that contain calls.

Services/parsers:
- New native `core/save_chunk_header.{hpp,cpp}` models the 12-byte tagged-chunk header with exact
  wire bytes and truncation/magic errors; new checksum edge vectors (one case NOT RUN by design).
- 11 new synthetic parser fixtures across PSARC, overlay/manifest JSONL, PS4 dynlink, audio header
  classification and XFF; two real defects fixed (`psarc.load_members_jsonl` error type/line number;
  `classify_audio_headers.family_of` input handling).

Tooling correctness (do not repeat these mistakes):
- A fixed-window byte scan over-reported 49 family slots; `0xf53de0`'s reachable body has no calls.
- A linear sweep ran from `0xf06340` into the next function because a `__stack_chk_fail` block falls
  through into padding.
- An `objdump` line parser that assumes a trailing space drops any instruction whose bytes fill the
  byte column, silently truncating the rest of the block (`0xdd7e30` 319 vs 396 bytes,
  `0xdd4210` 334 vs 486). Split on the tab.
- `scripts/disasm_range.sh` uses `--adjust-vma`, so its `#` target comments are wrong; re-derive
  targets from the raw displacement bytes.

## Current blocker

Slot 23 / RVA `0x00e71990` remains the first unresolved production constructor. It writes a 14-row
name/hash registry at `0x3353f40`; the accessors accept 0–12 and row 13 is a hash-zero sentinel. A
decoded call sequence passes a pointer from `0xe70a60` to `0xe70af0`, while the reachable
`0xe5cd78` path hashes an object-row pointer and scans per-object hashes in `0xe715d0`. Ghidra
timed out before function evidence was available; `0xe70af0` still has 31 unresolved indirect
transfers, so the string consumer's semantics and reachability remain open. Keep original strings
private and do not register slot 23 or claim startup/G1. Slot 11's fixture is unchanged.
See [the partial slot-23 checkpoint](docs/checkpoints/2026-10-21-root-cpu01-slot23-consumers-partial.md).

## Settled decisions (do not re-ask)

- `StartupState` RAII ownership for the slot-10 mutex — implemented.
- Testable `LauncherCore` lifecycle helper and a system alert for folder-import errors — keep.
- Offline analysis of the local `eboot.bin` is allowed; no original/Sony executable may ship or run.
- The shared descriptor helper models the 34-slot family; slots 24–47 remain in a test-only ahead
  table outside `recovered_constructors()` until production reaches and resolves slot 23.

## Next exact work

1. Continue `CPU-01-SLOT23-CONSUMERS-01` with a bounded, name-free trace of `0xe70af0`'s indirect
   calls and object/vtable source. Ghidra 12.1.4's 330-second startup timeout is recorded; do not
   treat the local decoder as equivalent Ghidra evidence. Once semantics are understood, take the
   data-only import choice to the principal before changing native code.
2. Identify the class/interface behind vtable `0x15f9ef0` (slot 12) and the forwarding callees
   `0xf011f0`, `0xefe410`, `0xefd140`, `0xefad20`, `0xef8600`, `0xf030e0` (slots 13–19).
3. Preserve `0xd71f60` NEEDS_PRINCIPAL (3 failed tries) and do not retry without principal
   reassessment.
4. Any new native module needs a README row, a test, and CMake registration; any new parser fixture
   needs a failing-before proof where it claims a defect fix.
5. Before publishing: `python3 scripts/project_index.py`, `trail build --check`,
   `python3 -I scripts/audit_publish_paths.py --all`, then commit allowlisted paths and
   `git push origin main`.

## Parallel-agent state

- Native worker completed batches 30–33 plus the two SAV-01 C++ fixtures (263/263, nine checkers,
  `local/build/native-agent-a` scratch tree remains private/regenerable).
- Parser worker completed 8 DATA-02/RE-02 fixtures; audio/archive worker completed 3 AUD-01/DATA-02
  fixtures (103 tests total).
- Launcher worker completed 4 APP-01 fixtures (42 tests / 7 suites).
- The persistent Ghidra project `local/ghidra/persistent-20261020` must never be re-imported or
  overwritten while in use; read-only `-noanalysis` reopening is the supported path.

## Private evidence pointers

- Family manifest: `local/re/work/ctor-classes.json` (per-slot span, digest, calls, class).
- Consumer packet: `local/re/evidence/cpu01-ctor10-descriptor-consumer-01/`
  (`report.md`, `verify.py`, `verification.json` 45/45, `ghidra-*.txt`, `SHA256SUMS`).
- Reopen log: `local/checkpoints/root-20261020-reopen-console.log`.
