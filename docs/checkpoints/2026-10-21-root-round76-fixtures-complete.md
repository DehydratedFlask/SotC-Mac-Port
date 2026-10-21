# Round 76 fixtures verified — test-only evidence

- Local time / timezone: 2026-10-21 17:44 CDT (America/Chicago).
- Agent / integrator: `/root`; Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`.
- Tasks / status / tries: `APP-01-CORE-DIAGNOSTICS-EMPTY-SUBSYSTEM-LIST-FIXTURE-01`, `DATA-02-PSARC-BLOCK-TABLE-WIDTH-ALIGNMENT-FIXTURE-01`, and `SAV-01-NATIVE-STREAM-FILE-EXACT-LIMIT-FIXTURE-01` are `FIXTURE_VERIFIED`; each has one successful implementation attempt and zero failed tries.
- Prerequisites: `APP-01-DIAGNOSTICS-SUMMARY-01`, `DATA-02-PSARC-PARTIAL-READ-01`, and `SAV-01-NATIVE-STREAM-01` are `FIXTURE_VERIFIED` in `docs/TASKS.md`.
- Acceptance gates: G0 remains `PARTIAL`; G1–G6 remain `NOT RUN`. No game behavior or save compatibility is claimed.
- Files changed for this batch: `launcher/Tests/LauncherCoreTests/LauncherModelTests.swift`, `scripts/test_psarc.py`, and `native/tests/test_save_stream_file.cpp`; documentation updated in `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, generated `docs/FOLDERS.md`, and this checkpoint. No production source changed. No dependency/install change; `INSTALL_LOG.md` is unchanged.

## Results

### APP-01 empty-subsystem diagnostics row

- Owner: `/root/launcher_review`; root independently reviewed and replayed.
- Change: one synthetic `CoreCoverageReport` with `subsystems: []` asserts exact output row `  Subsystems: none reported`.
- Baseline: `swift test --package-path launcher --scratch-path local/build/launcher --filter CoreReadinessTests` passed 4 tests before the change; baseline log `local/validation/root-round76-claims/launcher-diagnostics-baseline.log` SHA256 `d7427440101473c420e58e20fca384a358665cc583c9d3df4b0e03c6a5494b72`.
- Worker and root commands: `swift test --package-path launcher --scratch-path local/build/launcher --filter CoreReadinessTests`; each exited 0 with 5 tests in 1 suite. Worker/root logs `local/validation/round76-app01-empty-subsystems/worker.log` and `root-review.log`, SHA256 `125b00a12fd3b39ac23427aa657f7e65ddfd54d8ebe9aebb6e237698c2b87394` / `8d62be4b39978762ffc22cd18479c21054fed9ed532b05680a995871c5bfd257`.
- Source identity: test SHA256 `66160fdeb49b79762e2c3141c7e0877fd840873e1710584decc9710ee27eb3c0`; unchanged `launcher/Sources/LauncherCore/CoreReadiness.swift` SHA256 `fea0fec1c0635b8d6ab3978db2a817a1dd47d0a8d87045aada68f820ce64e2eb`.
- Observation and limit: the pure formatter output includes the empty-list row. No live core coverage query, UI display, or game behavior was exercised.

### DATA-02 PSARC partial block-table entry

- Owner: `/root/review_host_proto`; root independently reviewed and replayed.
- Change: build a one-member synthetic archive with block size 257, increase big-endian header `toc_size` by one byte, and assert exact `PsarcError("block table not a multiple of width")`.
- Baseline: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_psarc.py` passed 37 tests before the change; baseline log `local/validation/root-round76-claims/psarc-baseline.log` SHA256 `bba5ab93a9265bdd21b18d2898204ac2add994b44ea727b35dc2b9e70ae8324e`.
- Worker and root command: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_psarc.py`; each exited 0 with 38 tests. Worker/root logs `local/validation/round76-psarc-width-alignment/worker.log` and `root-review.log`, both SHA256 `e688ca1bae9f6cdf8817cb78963001b370e2596ae6b57d0a2fd5dc6646b5d2be`.
- Source identity: test SHA256 `1339ea4676ebba9b21bafe0dd857867738a4589c452184713b97d3dd4fb0789f`; unchanged parser SHA256 `39f2c748db013637aba8144205dc60b7e5b1eb49ee0f641960783a1e2c140485`.
- Observation and limit: the malformed synthetic TOC reaches the fixed-width alignment guard. No real archive, overlay, asset, or game behavior was exercised.

### SAV-01 exact maximum file-reader limit

- Owner: `/root/round68_native_scout`; root independently reviewed and replayed.
- Change: extend `test_oversize_file()` with a sparse zero-filled file of exactly `kSaveStreamHeaderSize + kSaveStreamMaxBodySize`; `read_save_stream_file` returns `SaveStreamError::bad_version` after passing its file-size gate.
- Baseline: `cmake --build local/build/native --target test_save_stream_file` exited 0 (log SHA256 `e498f88e3ddd30adf6b222cc60decde2b2ecab59b135e58bc2e629ee7f923598`). `ctest --test-dir local/build/native -R '^save_stream_file$' --output-on-failure` exited 0, 1/1 passed (baseline log SHA256 `af9ee511e63032d882014def01b662e0c13c76573693ca075f4d4ad33dadbcd1`). Both baseline logs are under `local/validation/root-round76-claims/`.
- Worker/root commands: `cmake --build local/build/native --target test_save_stream_file` exited 0; worker/root build logs SHA256 `23b802e59005a3cb8e43be3619a0ead486d249860d2c858aa477800b8d2e50b6` / `e498f88e3ddd30adf6b222cc60decde2b2ecab59b135e58bc2e629ee7f923598`. Then `ctest --test-dir local/build/native -R '^save_stream_file$' --output-on-failure` exited 0, 1/1 passed; worker/root CTest logs SHA256 `51933e1a5e061183007439f754edf4b200ac87220685b94d8439604e43d209a1` / `e01962de6a8e92f46169417de6271862a6fdfce424576f5ab418276e13e1ec03`.
- Source identity: test SHA256 `ffc9a985c134383ae1f3c8af8f6a448b6ab7a8e00113b7c9fb306c03f4504617`; unchanged `native/core/save_stream.cpp` SHA256 `7bc9763539dfa6fdd40f26d3cd9c1dd4c0d06e50bbd3af87d0c7b1f2cb1c0c7e`; unchanged `native/core/save_stream.hpp` SHA256 `b2a0f296b850f094151e4d53ef7cf08f62268b3358cd9aa1fdfb41244a7dbe83`.
- Toolchain evidence: `local/validation/root-round76-completion/toolchain.log` records arm64, macOS 27.0.1 build 26A434, Swift 6.4, CMake/CTest 4.4.4, Xcode 27.1 build 27A9275, and Apple Clang 21.0.0; log SHA256 `194a3a6615165086304d29470c8b4deccaa9c52c2dd20d6c008d893eb971da15`.
- Observation and limit: the zero-version sparse file is intentionally invalid. This tests only the exact file-size boundary and validation order, not valid save parsing, persistence, compatibility, or game save/load. It has roughly 100 MiB logical/read size.

## Remaining risks and next action

- All changes are additive tests in the three claimed files. `git diff --check` passes. There were no failed hypotheses or abandoned attempts.
- No real archive or original game save data was read or required. No proprietary files or binaries were touched.
- Index check: `python3 scripts/project_index.py` exited 0 and indexed 14,475 folders; `python3 scripts/project_index.py --check` exited 0 and reported current. The first check after adding new private validation logs reported stale `local/indexes/folders.json`; the index was regenerated after all current log paths existed, then rechecked successfully. Earlier output is retained at `local/validation/root-round76-completion/project-index-check-initial-stale.log` and `project-index-check.log`.
- Trail check: `trail build --check` first reported the launcher and native test changes as stale (exit 1). `trail build` exited 0, rebuilt the local ignored graph to 3,578 nodes and 4,853 edges, then `trail build --check` exited 0 and reported the wiring graph in sync. The optional deep meaning layer remains unbuilt; `graft/` is a local ignored cache and is not staged.
- Toolchain: the local toolchain capture is `local/validation/root-round76-completion/toolchain.log`, SHA256 `194a3a6615165086304d29470c8b4deccaa9c52c2dd20d6c008d893eb971da15`; it records arm64, macOS 27.0.1 build 26A434, Swift 6.4, CMake/CTest 4.4.4, Xcode 27.1 build 27A9275, and Apple Clang 21.0.0.
- Publication preflight: `git diff --cached --check` exited 0. `python3 -I scripts/audit_publish_paths.py` exited 0 and accepted 9 staged paths (output `local/validation/root-round76-completion/audit-staged.log`); `python3 -I scripts/audit_publish_paths.py --all` exited 0 and accepted 1,657 paths (output `local/validation/root-round76-completion/audit-all.log`). The existing user-owned native API modification and untracked host directory were not staged.
- Publication remains pending for this completion checkpoint. Exact next action: commit the staged allowlisted files, push `origin main`, fetch, and confirm `HEAD == origin/main`; then record the publication result in `HANDOFF.md` and this checkpoint.
