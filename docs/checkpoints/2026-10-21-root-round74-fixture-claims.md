# Round 74 fixture claims

Checkpoint time: 2026-10-21 17:11 CDT (America/Chicago). Integrator: `/root` (Codex desktop task
`01a15eab-88d5-7a83-a23b-cc55dc002eb6`). Claim parent is `f7e1cf9c418c2435283635ad1f1ad732ff095848`.
Claim register rows are in [TASKS](../TASKS.md); current project gates remain recorded in
[HANDOFF](../../HANDOFF.md).

## Shared state and scope

The live repository was checked after read-only scouts used Graft. `HEAD` and `origin/main` were
both `f7e1cf9c418c2435283635ad1f1ad732ff095848`; divergence was `0 0`. The only pre-existing
worktree changes are the user-owned `native/api/CMakeLists.txt` modification and untracked
`native/api/host/`; they are outside every claim and remain untouched.

Five tasks are claimed before implementation. Each owns one test file; production files are
read-only. Exact task IDs, owners, prerequisite state, baseline commands and acceptance checks:

| Task | Owner / sole file | Parent state | Claim-time baseline | Acceptance |
|---|---|---|---|---|
| `DATA-02-PSARC-TOC-UPPER-BOUND-FIXTURE-01` | `/root/review_host_proto`; `scripts/test_psarc.py` | `DATA-02` ACTIVE; `DATA-02-PSARC-PARTIAL-READ-01` FIXTURE_VERIFIED | `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_psarc.py`: exit 0, 36 tests | Build the existing valid synthetic archive, set `toc_size` to one byte past file length, and assert exact `PsarcError('TOC size inconsistent with entry count/file size')`. No real-archive convention claim. |
| `RE-02-DYNLINK-CTOR-RELOCATED-SENTINEL-FIXTURE-01` | `/root/review_host_proto`; `scripts/test_ps4_dynlink.py` | `RE-02-DYNLINK-01` FIXTURE_VERIFIED | `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_ps4_dynlink.py`: exit 0, 26 tests | A relocated all-ones word at slot `0x1008` with addend `0x4242` is emitted as an entry; the next unrelocated all-ones word at `0x1000` is the sentinel. Assert exact structured output. No ELF-loader or runtime claim. |
| `APP-01-CORE-DIAGNOSTICS-FAILURE-ROWS-FIXTURE-01` | `/root/launcher_review`; `launcher/Tests/LauncherCoreTests/LauncherModelTests.swift` | `APP-01-DIAGNOSTICS-SUMMARY-01` FIXTURE_VERIFIED | `swift test --package-path launcher --scratch-path local/build/launcher --filter CoreReadinessTests`: exit 0, 3 tests | Assert exact ordered `CoreDiagnostics.coreSection` rows when `.io` query failure and `.badState` last-request failure are both present. Formatter output only. |
| `APP-01-CORECLIENT-IO-BADSTATE-COPY-FIXTURE-01` | `/root/launcher_review`; `launcher/Tests/LauncherCoreTests/CoreClientTests.swift` | `APP-01-CORE-ABI-01` PARTIAL | `swift test --package-path launcher --scratch-path local/build/launcher --filter CoreClientTests`: exit 0, 13 tests | Assert exact `.io` and `.badState` strings from the existing pure mapping. No live ABI/runtime claim. |
| `SRV-01-REGION-LIST-SINGLETON-UNLINK-FIXTURE-01` | `/root/round68_native_scout`; `native/tests/test_region_list.cpp` | `SRV-01` ACTIVE; dependencies `RE-02`, `DATA-02` | `cmake --build local/build/native --target test_region_list`: exit 0; `ctest --test-dir local/build/native -R '^region_list$'`: exit 0, 1/1 | Construct a sentinel-headed list with one node and free count 1. After unlink, assert `sentinel.next == nullptr`, the removed node's `next` points to itself, its `prev` remains the sentinel per current helper behavior, and free count is 0. Synthetic intrusive-list behavior only. |

## Baseline files and identities

The test files and production sources below were SHA-256 checked at claim time. No implementation
edits had started; these are the baseline identities.

| Task | Test SHA-256 | Read-only production SHA-256 | Root baseline log(s), SHA-256 |
|---|---|---|---|
| PSARC upper TOC | `scripts/test_psarc.py` `1eda5a3ff072729536afad21da53fa8267f5774589c63a550378c6aab40e0816` | `scripts/psarc.py` `39f2c748db013637aba8144205dc60b7e5b1eb49ee0f641960783a1e2c140485` | `local/validation/root-round73-scout/round74-psarc-baseline.log` `cd7b78e99b62c807bda666fe7e6a11c9e4bec7774d6805941a57a85100190bc4` |
| Relocated ctor sentinel | `scripts/test_ps4_dynlink.py` `75225da11393fbef6ec40fc0f6351f4522e6aa86e0f975505dbf9fe90591cfaa` | `scripts/ps4_dynlink.py` `28c53257a6f2bd58522f123098d41ca0d884222330a1bf18ce9f8935fb45eff9` | `local/validation/root-round73-scout/round74-dynlink-baseline.log` `fb742cec914f6d036dcde929ba737876b1d95237b2006c04c0316ee097d4a4c2` |
| Diagnostics failure rows | `LauncherModelTests.swift` `66e8f7c7f98465d3845cd038966934da5c32025371304c5ec6e24b4ad2a80c6a` | `CoreReadiness.swift` `fea0fec1c0635b8d6ab3978db2a817a1dd47d0a8d87045aada68f820ce64e2eb` | `round74-launcher-readiness-baseline.log` `a46ddf7f87b41e91ea45bdbae5a03fe967f48a4f95b854011869a286a4a7a5d9` |
| CoreClient copies | `CoreClientTests.swift` `1e40e2483faeb114b5bbea05dba2aee2c6e9c3cb61e55783572c2a5639fa3fea` | `CoreClient.swift` `9f2b214486f8c3ed81ead6a237f86e494c52ccd89a33c15a9fed60de533b73f5` | `round74-launcher-coreclient-baseline.log` `c7758d8c099351bd64d79bfa25385d593c3e95297e3a5089b16a8beec5aa7bdb` |
| Singleton region unlink | `native/tests/test_region_list.cpp` `3ca51b59317206c2fb76c417a140e0917788094dc1e4ab7d9e417dfc7dd7193c` | `region_list.cpp` `a042ec5af910f31579c43b1897d6d35e5e535c25da0b031f32e7973f1c8e3bda`; header `region_list.hpp` `3f36189b27178b4ebea6135067e96c29d4839be9e255cddb6e51cd4b7f65fb69` | `round74-native-region-list-build-baseline.log` `5309b8f8a5ee9a5cdbad8e067efbda1b99f8294aa79e644d5a4c61c017631bc0`; `round74-native-region-list-ctest-baseline.log` `7042082a6ea56737a63daf3074b63901911693c3cfa3a4a1abafb1defbf045a3` |

All log paths above are under `local/validation/root-round73-scout/` and excluded from publication.
The script baselines ran 36 and 26 tests; launcher filters ran 3 and 13 tests; native build and
focused CTest passed 1/1. No baseline failed. Baseline runs are not implementation attempts.

## Scout findings, observations, and remaining risks

The parser scout rejected no additional test-only options, but the two chosen cases cover distinct
unasserted branches. The launcher scout ruled out the already-covered script-stub case and provided
two separate fixture locations. The native scout preferred singleton unlink over a host-map overflow
fixture because the latter's `nullopt` result is also used for operating-system mapping failure.

Observation: all five claimed baselines pass, test and production hashes match the live worktree,
and the selected assertions target existing behavior without requiring a production edit.
Inference: none. Each task has zero failed implementation tries and zero implementation attempts.
G0 remains PARTIAL; G1–G6 remain NOT RUN. Next exact action: finish folder-index and path audits,
publish the claim set, fetch and verify the remote tip, then let each owner implement only its
allowlisted test file.

## Pre-dispatch publication checkpoint

Checkpoint time: 2026-10-21 17:12 CDT (America/Chicago). Agent: `/root`. Staged paths are only
`HANDOFF.md`, `docs/FOLDERS.md`, `docs/TASKS.md`, and this claim checkpoint. No source or test file
is staged; the user-owned API changes are excluded.

- `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/project_index.py`: exit 0; 14,468 folders indexed
  and the public index now records 385 checkpoint files. Log `pubsync-index-generate.log`, SHA-256
  `c0c4d7d50babd839833e54a1f7374e4ac76ebb16eec2555cce6ae22512464bec`.
- `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/project_index.py --check`: exit 0, current at
  14,468. Log `pubsync-index-final-check.log`, SHA-256
  `82a06e9d8c3e42e0a5d6e750144f973f757db4f956b5dd4d3ef97bc50a0e8ccb`.
- `trail build --check`: exit 0; source graph in sync at 3,572 nodes / 4,837 edges / 943 cards;
  deep meaning layer remains unbuilt. Log `pubsync-trail-check.log`, SHA-256
  `bc77b1c82984f85ace632af8382d9800cc4015579fe1bea7373a8bcb082499f3`.
- `git diff --cached --check && git diff --check`: exit 0. Log `pubsync-diff-check.log`, SHA-256
  `8051da3aaf5ed31b3c200f7693a6133f151dc4acede082e677690f7918b918d8`.
- `python3 -I scripts/audit_publish_paths.py`: exit 0; 4 staged paths accepted. Log
  `pubsync-audit-staged.log`, SHA-256
  `50ca23fef09fdaeeb493776ee8824f54cd4dbb76d9973e4d683ded5a9849a5ac`.
- `python3 -I scripts/audit_publish_paths.py --all`: exit 0; 1,652 paths accepted. Log
  `pubsync-audit-all.log`, SHA-256
  `51c4f017bf9a75129d0d3d1d567a2092595b85159f0eb4bc3527618446e0e09c`.
- `git fetch origin`: exit 0. Before commit `HEAD` and `origin/main` both equal
  `f7e1cf9c418c2435283635ad1f1ad732ff095848`, divergence `0 0`. Logs `pubsync-fetch.log`,
  `pubsync-state.log`, and `pubsync-divergence.log`, SHA-256 `8051da3aaf5ed31b3c200f7693a6133f151dc4acede082e677690f7918b918d8`,
  `f1fb09576036c30d2f435eb5bc8c673ba77d051de18a443179465b33d0845602`, and
  `231d953f3e206b69f61bb624fcd69294244ee7ad6bf019bfa1f8f965c5328f82`.
- `git status --short --branch` confirms only those four staged docs plus the excluded user-owned API
  changes. Log `pubsync-status.log`, SHA-256
  `3df8e05785b4e7d9d050467fe247dce34653351dd27f90f71ec81e694fd48819`.

All root baselines above pass; no code acceptance claim is made before dispatch. Next exact action:
commit and push the four allowlisted claim paths, verify the fetched remote tip, then dispatch the
five isolated test-file tasks.

## Claim publication checkpoint

Checkpoint time: 2026-10-21 17:13 CDT (America/Chicago). Agent: `/root`. Claim commit
`820bb128adfb3c9a03271c33a704a3ae2682aed1` (`docs: claim Round 74 fixture tasks`) pushed successfully, moving `main` from
`f7e1cf9` to `820bb12`. `git fetch origin` exited 0. Post-fetch `HEAD` and `origin/main` both equal
`820bb128adfb3c9a03271c33a704a3ae2682aed1`; divergence is `0 0`. A second `git push origin main` exited 0 with `Everything
up-to-date`.

- Fetch log `local/validation/root-round73-scout/pubsync-fetch.log`, SHA-256
  `8051da3aaf5ed31b3c200f7693a6133f151dc4acede082e677690f7918b918d8`.
- Post-fetch state log `pubsync-state.log`, SHA-256
  `17d7a401ee7e40b2831681a65ccb186d25fa616e5e4fc8143e0c2346958c562b`; divergence log
  `pubsync-divergence.log`, SHA-256
  `231d953f3e206b69f61bb624fcd69294244ee7ad6bf019bfa1f8f965c5328f82`.
- Confirmation push log `pubsync-final-push.log`, SHA-256
  `6a296371679d55bc9e67c365fd337f2114bb1b68e3e56f995601143a8d29c005`; commit log
  `pubsync-commit.log`, SHA-256 `dd8fce92c706d5973e95a865fd3ac943fcf98ba4ae15eefb710fd80e5b6409df`.
- Post-push status log `pubsync-status.log`, SHA-256
  `e0335ec91a749d2e476cb33be39e2b906e147877be8468e0ce4758917953e836`, shows only the pre-existing
  user-owned `native/api/CMakeLists.txt` modification and `native/api/host/` untracked path.
- `python3 -I scripts/project_index.py --check`: exit 0, 14,468 folders. `trail build --check`:
  exit 0, wiring graph in sync. Logs `pubsync-index-final-check.log` and `pubsync-trail-check.log`,
  SHA-256 `82a06e9d8c3e42e0a5d6e750144f973f757db4f956b5dd4d3ef97bc50a0e8ccb` and
  `bc77b1c82984f85ace632af8382d9800cc4015579fe1bea7373a8bcb082499f3`.

Observation: the published claim commit is the fetched remote tip and the worktree contains no
claimed source/test edits. Inference: none. All five tasks remain ACTIVE with zero implementation
attempts and zero failed tries. Next exact action: dispatch the two script tests, two launcher tests,
and native region-list test to their assigned owners; each agent must remain within its single test
file.
