# Round 73 fixture claims

Claim checkpoint time: 2026-10-21 16:49 CDT (America/Chicago). Integrator: `/root` (Codex desktop
task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`). Claim base: `be28fc159481a6265e2e23f9cc0970b0621fea41`.

Three independent synthetic/test-only tasks are claimed before implementation. `/root` owns
`HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, the project index, and checkpoints. Each worker
owns only the one test file named below. No source implementation changes are claimed.

## Claims

| Task | Owner and file | Hypothesis and acceptance | Status |
|---|---|---|---|
| `RE-02-DYNLINK-EXPORT-LIB-METADATA-FIXTURE-01` | `/root + review_host_proto`; `scripts/test_ps4_dynlink.py` | Give the fixture's existing `DT_SCE_EXPORT_LIB` entry a nonzero synthetic ID and the existing `libFixture` string offset; assert the parsed export map contains the exact synthetic `{name, id, enc}` record. The existing fixture has a zero-valued export entry and tests module/library/import metadata but no export record. | ACTIVE; 0 implementation attempts, 0 failed tries |
| `APP-01-CORECLIENT-ABI-MISMATCH-FAILURE-COPY-FIXTURE-01` | `/root + launcher_review`; `launcher/Tests/LauncherCoreTests/CoreClientTests.swift` | Assert `CoreClientMapping.failureText(for: .abiMismatch) == "The game core was built for a different interface version."`. This pins one existing pure Swift mapping; it does not change copy or ABI behavior. | ACTIVE; 0 implementation attempts, 0 failed tries |
| `SRV-01-REGION-SPLIT-ZERO-LEAD-NO-MUTATION-FIXTURE-01` | `/root + round68_native_scout`; `native/tests/test_region_split.cpp` | Replace the all-zero node setup for the existing lead-zero rejection with two distinct nonzero byte patterns; assert the call rejects and both complete `RegionNode::raw` arrays remain byte-for-byte unchanged. | ACTIVE; 0 implementation attempts, 0 failed tries |

The three owners edit different test files. The task register and evidence table contained no matching
fixture IDs/coverage. Root independently inspected the current source/test spans and searched the
live task/evidence register; no material API or user-copy choice is involved.

## Baselines and claim-time hashes

| Scope | Exact command | Result / log |
|---|---|---|
| Dynlink export parser | `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_ps4_dynlink.py` | Exit 0; 26 tests. `local/validation/root-round73-scout/data-baseline.log`, SHA-256 `9562de2a6ec65628118dd9062f412650a6749b4ed9a39313346fdca45cecb775`. |
| Launcher core client | `swift test --package-path launcher --scratch-path local/build/launcher --filter CoreClientTests` | Exit 0; 9 `NativeCoreClientTests` and 13 `CoreClientTests` passed (22 total). `app-baseline.log`, SHA-256 `b62dea62652cac4a1b2d32434ca90343d8c808e2a6d321a6b6d0906bcc7fe608`. |
| Region split | `cmake --build local/build/native --target test_region_split`; then `ctest --test-dir local/build/native -R '^region_split$'` | Build exit 0; focused CTest exit 0, 1/1 passed. Logs `srv-baseline-build.log` SHA-256 `21a11a3b5ef65d6abb448e29a2eec6df2cfbe0621748eea0a75d2448d65bf0ab` and `srv-baseline-ctest.log` SHA-256 `b375c10499380371acc89985e8d8caeb36895dba02f44cd4f17b02350fcadcb4`. |

Claim-time hashes:

- `scripts/ps4_dynlink.py`: `28c53257a6f2bd58522f123098d41ca0d884222330a1bf18ce9f8935fb45eff9`.
- `scripts/test_ps4_dynlink.py`: `653a53a67891995fdcdb2ac85abe5407836ac6cda574241f3de41dd0d9342e10`.
- `launcher/Sources/LauncherCore/CoreClient.swift`: `9f2b214486f8c3ed81ead6a237f86e494c52ccd89a33c15a9fed60de533b73f5`.
- `launcher/Tests/LauncherCoreTests/CoreClientTests.swift`: `f0447d854f89d56b7c165462d5a83680be676ffad8f5d9f949b0b372134b370f`.
- `native/core/region_split.cpp`: `aeead9c2a91b49283e0924d1bfba5ac98bb56aee50a1cc1f320a6e6924d0021f`.
- `native/core/region_split.hpp`: `be19e19e04ea8cd7040a59058253d7f7ca9a596e46216320a512c786a1f66acb`.
- `native/tests/test_region_split.cpp`: `679b8880c754b428d1cac1f01b08baff964210554fd7355f8e73a3e9b6a296fc`.

## Scope limits and next action

These claims pin one synthetic ELF parser output, one pure Swift string mapping, and one pure native
helper's no-mutation contract. They establish no Sony export-format authority, loader behavior, live
ABI mismatch, allocator integration, game execution, or game acceptance gate. G0 remains PARTIAL;
G1–G6 remain NOT RUN. No install/dependency facts changed. User-owned `native/api/CMakeLists.txt`
and `native/api/host/` remain untouched and excluded.

The claim commit is published and verified below. Next exact action: publish the post-claim
documentation sync, then dispatch the three single-file edits. Status is ACTIVE for each task; 0
failed tries.

## Claim publication checkpoint

Checkpoint time: 2026-10-21 16:52 CDT (America/Chicago). Agent: `/root`. Task IDs: the three
Round73 IDs above. Files changed and published: `HANDOFF.md`, `docs/FOLDERS.md`, `docs/TASKS.md`,
and this checkpoint. No source or test file has changed; the user-owned `native/api/CMakeLists.txt`
and untracked `native/api/host/` remain untouched.

- `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/project_index.py`: exit 0, 14,468 folders indexed;
  generated index updated. Log `local/validation/root-round73-scout/claim-index-generate.log`,
  SHA-256 `b8fde651bc090b143c0607ec67613ca52b902f336993b7fd221e7fd61a641f36`.
- `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/project_index.py --check`: exit 0, 14,468 folders
  indexed and current. Log SHA-256 `f13eeff12b22e2aa01a70e27c6f29f138992718cb3930a53033129c3f47e7d21`.
- `trail build --check`: exit 0, wiring graph in sync at 3,572 nodes / 4,837 edges / 943 cards;
  deep meaning layer remains unbuilt. Log SHA-256
  `99342face5580b625b2480268e4e5b63ecfebc573b79c791cf004539b078027b`.
- `git diff --cached --check` and `git diff --check`: exit 0. The staged publish-path audit passed
  exactly 4 allowlisted paths; the full audit passed 1,650 paths. Log hashes: staged
  `e790379bfbd7f2fa92bc8b663f59ffb600074cc1c05e501c8dc2df9b92380fd6`, full
  `18ae020dfedd6f77c515205cfd74f0d1502898ed1d7fe2ae89a31cabe2d711d3`, diff
  `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`.
- Before commit, `git fetch origin` succeeded and `git rev-list --left-right --count
  HEAD...origin/main` returned `0 0`. Claim commit `01e683bcc2e172e409f9d65fd06f6e54ff6f8acf`
  was pushed to `origin/main`. Post-push fetch succeeded; the state log records matching HEAD and
  origin refs with divergence `0 0`. Fetch/state/commit/push log hashes:
  `be87f0e1cb8b7e9cc5faeaeb535f1c7436d2a516681757485b7f9903e5bad082`,
  `ba3abf892a1b48512313670b3c7628cc4608e4a9fa5c769dbcb0e6bb7d5a0a98`,
  `6565bf483c28d03234650965f1ca47277ef334e11b1b4c46f4b2b0b07334a473`, and
  `0dc59667638fc78ebbca2f9b638bffdc909e4ed68ee7d7ca69abcfb976f8b649` respectively.

Observation: repository/task/evidence state contains no duplicate claim for these three synthetic
fixtures; each baseline passed and only the claim/documentation paths were staged. Inference: none of
these fixtures advances a game acceptance gate. Remaining risks are limited to the declared fixture
scope until implementation and independent replay. Current status is ACTIVE for each task, 0
implementation attempts, 0 failed tries. Next exact action: publish this documentation sync, then
dispatch each worker to edit only its claimed test file and run the focused check.
