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

Next exact action: refresh/check the folder index, publish these claims, verify `HEAD == origin/main`,
then dispatch the three single-file edits. Status is ACTIVE for each task; 0 failed tries.
