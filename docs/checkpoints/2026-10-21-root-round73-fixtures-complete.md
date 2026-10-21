# Round 73 fixtures verified

Checkpoint time: 2026-10-21 16:57 CDT (America/Chicago). Integrator: `/root` (Codex desktop task
`01a15eab-88d5-7a83-a23b-cc55dc002eb6`). Tasks: `RE-02-DYNLINK-EXPORT-LIB-METADATA-FIXTURE-01`,
`APP-01-CORECLIENT-ABI-MISMATCH-FAILURE-COPY-FIXTURE-01`, and
`SRV-01-REGION-SPLIT-ZERO-LEAD-NO-MUTATION-FIXTURE-01`. Claim and dispatch history is in
[the claim checkpoint](2026-10-21-root-round73-fixture-claims.md).

## Changes

- `scripts/test_ps4_dynlink.py`: synthetic `DT_SCE_EXPORT_LIB` fixture now uses ID `0x789` and the
  existing `libFixture` string offset. The parser output is asserted to contain the exact name, ID,
  and encoded ID. Test SHA-256 `75225da11393fbef6ec40fc0f6351f4522e6aa86e0f975505dbf9fe90591cfaa`.
- `launcher/Tests/LauncherCoreTests/CoreClientTests.swift`: exact `.abiMismatch` failure text is
  asserted through the existing pure mapping. Test SHA-256
  `1e40e2483faeb114b5bbea05dba2aee2c6e9c3cb61e55783572c2a5639fa3fea`.
- `native/tests/test_region_split.cpp`: the zero-lead rejection case seeds both complete node
  records with different nonzero byte patterns and checks rejection preserves both records byte for
  byte. Test SHA-256 `8bc5a45cbb421536d3f6ed06ebd4972152df615e9f5e4813aaf7b532e7498c46`.

No production file changed. Unchanged production hashes: `scripts/ps4_dynlink.py`
`28c53257a6f2bd58522f123098d41ca0d884222330a1bf18ce9f8935fb45eff9`,
`launcher/Sources/LauncherCore/CoreClient.swift`
`9f2b214486f8c3ed81ead6a237f86e494c52ccd89a33c15a9fed60de533b73f5`,
`native/core/region_split.cpp`
`aeead9c2a91b49283e0924d1bfba5ac98bb56aee50a1cc1f320a6e6924d0021f`, and
`native/core/region_split.hpp`
`be19e19e04ea8cd7040a59058253d7f7ca9a596e46216320a512c786a1f66acb`.

## Worker and root verification

| Task | Exact command | Worker result / log | Independent root result / log |
|---|---|---|---|
| Dynlink export metadata | `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_ps4_dynlink.py` | Exit 0; 26 tests. `local/validation/root-round73-scout/data-worker.log`, SHA-256 `fb07fd984b5b57e9d18e9f7885b26996b77d69a1daa4987c709b832a849437b7`. | Exit 0; 26 tests. `data-root.log`, SHA-256 `d23d96c924c37a648bc6287f8392678d17683e13339bc83ac8ea851b1049dbe3`. |
| Launcher ABI mismatch copy | `swift test --package-path launcher --scratch-path local/build/launcher --filter CoreClientTests` | Exit 0; 22 tests (9 `NativeCoreClientTests`, 13 `CoreClientTests`). `app-worker.log`, SHA-256 `877e97322a25808583ff999d45b382fed858dd6c304501a341937c683cda944c`. | Exit 0; 22 tests (9 + 13). `app-root.log`, SHA-256 `13bdf678a39e22cdd9fb8f6d209e013721d1d54a191bf651fdaad7eca05c9f9a`. |
| Region split zero lead | `cmake --build local/build/native --target test_region_split`; `ctest --test-dir local/build/native -R '^region_split$'` | Build exit 0; CTest exit 0, 1/1. `srv-worker-build.log` / `srv-worker-ctest.log`, SHA-256 `8afdf348bbda2cd741eda1e7216d82fc246741da3b539ee41c7522696b30af26` / `de832824b5771d7ba01f37980e5378c357718704938e5ffed245328f89352ed5`. | Build exit 0; CTest exit 0, 1/1. `srv-root-build.log` / `srv-root-ctest.log`, SHA-256 `21a11a3b5ef65d6abb448e29a2eec6df2cfbe0621748e8e5ffed245328f89352ed5` / `72f9a935d1a69ef02dd9a3f592ddde5aadca6e2b8363754c03aa206827fd7d47`. |

The dynlink parser reports one synthetic export record with the asserted identity; this is fixture
output. The launcher test pins one existing copy mapping. The native helper rejects zero lead while
leaving both synthetic records unchanged. These observations do not establish Sony export-format
authority, loader behavior, live ABI mismatch handling, allocator integration, or game execution.
No failed implementation tries; each task has one successful implementation attempt.

## Scope and gates

Only the three claimed test files changed for implementation. `native/api/CMakeLists.txt` and
untracked `native/api/host/` are pre-existing user-owned changes and remain untouched and unstaged.
No install/dependency facts changed. G0 remains PARTIAL; G1–G6 remain NOT RUN. No native game
executable, cold boot, authentic game audio, real save/reload, or Metal gameplay is demonstrated.

Current task status: all three `FIXTURE_VERIFIED`; each has 1 successful attempt and 0 failed tries.
Index and graph refresh plus the pre-push checks passed. Next exact action: commit and push only the
three test files and allowlisted documentation, then scout independent Round74 tasks.

## Index and source graph refresh

After the three test edits and new completion checkpoint were present:

- `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/project_index.py`: exit 0; 14,468 folders indexed
  and `docs/checkpoints` now lists 384 entries. Log SHA-256
  `b8fde651bc090b143c0607ec67613ca52b902f336993b7fd221e7fd61a641f36`.
- `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/project_index.py --check`: exit 0, current at 14,468.
  Log SHA-256 `f13eeff12b22e2aa01a70e27c6f29f138992718cb3930a53033129c3f47e7d21`.
- `trail build`: exit 0, refreshed graph to 3,572 nodes / 4,837 edges / 943 cards. One changed
  source file was parsed; the remaining 1,235 files replayed from cache. Log SHA-256
  `2ff1164fac7f8f4b2dfef9f0eff3e2012fe4c363c5a284c578c5a0f8706fa223`.
- `trail build --check`: exit 0, graph in sync. The optional deep meaning layer remains unbuilt.
  Log SHA-256 `99342face5580b625b2480268e4e5b63ecfebc573b79c791cf004539b078027b`.

The folder index and code graph are observations of repository metadata only. The pre-push results
are recorded below; task status remains `FIXTURE_VERIFIED` with one successful attempt and zero
failed tries for each task.

## Pre-push checkpoint

Checkpoint time: 2026-10-21 16:59 CDT (America/Chicago). Agent: `/root`. Changed paths staged for
publication are only `HANDOFF.md`, `docs/EVIDENCE.md`, `docs/FOLDERS.md`, `docs/TASKS.md`, this
completion checkpoint, and the three claimed test files. The user-owned `native/api/CMakeLists.txt`
and untracked `native/api/host/` are excluded.

- `git diff --cached --check` and `git diff --check`: exit 0.
- `python3 -I scripts/audit_publish_paths.py`: exit 0, all 8 staged paths accepted. Log SHA-256
  `185969c37999dd8017de4f1d14d07acd8df13ea9f88d925d052c4ff217e3da95`.
- `python3 -I scripts/audit_publish_paths.py --all`: exit 0, all 1,651 tracked/staged paths
  accepted. Log SHA-256 `8f7008655a830da3058759df7198711c653dff57ab2b7146b864242b6964d73e`.
- Diff-check log SHA-256 `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`.
- `git fetch origin`: exit 0. Before commit, HEAD and `origin/main` were both
  `a1fa566838fbeed6caa71dc53eed499302cbc3a0`; divergence was `0 0`. Fetch/state log hashes:
  `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855` and
  `0b23f07661b26c16daa483a61de6e1ce18b642956cc610650d1087c093d4eb06`.

Observation: the staged audit contains no forbidden path, and no user-owned API files are staged.
Inference: none. All three tasks are `FIXTURE_VERIFIED`, each with one successful implementation
attempt and zero failed tries. Next exact action: commit and push this allowlisted set, fetch again,
and verify HEAD matches `origin/main`.
