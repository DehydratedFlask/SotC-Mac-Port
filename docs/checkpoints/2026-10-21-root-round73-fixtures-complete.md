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

## Post-push publication checkpoint

Checkpoint time: 2026-10-21 17:01 CDT (America/Chicago). Agent: `/root`. Completion commit
`e5d3e40c308679a356998e506441eba2c013e17a` (`test: verify Round 73 export, ABI, and split
fixtures`) contains the eight allowlisted paths listed above. `git push origin main` completed
successfully; push log SHA-256 `47fd26c3d7597f2280dcca24e0770c3a0b5a37b1c0482ba6f5368f31f7798b62`.
Commit display log SHA-256 `16943abd04d0d97a137453276012968084fe92f8ae05453a4ba85674c4c28d62`.

- `git fetch origin`: exit 0; fetch log SHA-256
  `be87f0e1cb8b7e9cc5faeaeb535f1c7436d2a516681757485b7f9903e5bad082`.
- Post-push `git status --short --branch` and `git rev-parse HEAD origin/main` report both at
  `e5d3e40c308679a356998e506441eba2c013e17a`, divergence `0 0`; state log SHA-256
  `084b5f9ef97b98ba1ae143c7f5efbae77c34b19b8e509a157238d8283febd96e`.
- The only remaining worktree entries are the pre-existing user-owned `native/api/CMakeLists.txt`
  modification and untracked `native/api/host/`; both remain untouched and excluded.

Observation: the published completion commit is the fetched remote tip and the worktree's only
non-project entries are the same user-owned API changes. Inference: none. All Round73 tasks remain
`FIXTURE_VERIFIED`; G0 remains PARTIAL and G1–G6 remain NOT RUN. Next exact action: scout independent
Round74 fixture candidates from the live task register and Graft source graph, then publish bounded
claims before implementation.

## Publication-record sync pre-push checkpoint

Checkpoint time: 2026-10-21 17:03 CDT (America/Chicago). Agent: `/root`. Staged paths are only
`HANDOFF.md` and this completion checkpoint; Round73 code and task status do not change.

- `git diff --cached --check && git diff --check`: exit 0. Log
  `local/validation/root-round73-scout/pubsync-diff-check.log`, SHA-256
  `8051da3aaf5ed31b3c200f7693a6133f151dc4acede082e677690f7918b918d8`.
- `python3 -I scripts/audit_publish_paths.py`: exit 0, both staged paths accepted. Log
  `local/validation/root-round73-scout/pubsync-audit-staged.log`, SHA-256
  `048c125e781dcc057b488025ffb7ebe1728125bb8895bb6ad3ec809b7348235c`.
- `python3 -I scripts/audit_publish_paths.py --all`: exit 0, 1,651 tracked paths accepted. Log
  `local/validation/root-round73-scout/pubsync-audit-all.log`, SHA-256
  `ca1d0c4d76717454cecedc3459f6da14bdc0317a6a5b8bc1027be17abb4ac6d5`.
- `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/project_index.py --check` first returned exit 1
  because newly captured private logs changed the indexed file count. After all log files were
  present, `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/project_index.py` returned exit 0 and
  rewrote only the excluded private inventory; final `... project_index.py --check` returned exit
  0 at 14,468 folders. Logs `pubsync-index-check.log`, `pubsync-index-generate.log`, and
  `pubsync-index-final-check.log` have SHA-256 `143ff08d424879bb92a4b2bfe16518225ba8501c4b6358c40a21e72abce1df5f`,
  `c0c4d7d50babd839833e54a1f7374e4ac76ebb16eec2555cce6ae22512464bec`, and
  `82a06e9d8c3e42e0a5d6e750144f973f757db4f956b5dd4d3ef97bc50a0e8ccb` respectively.
- `trail build --check`: exit 0; wiring graph in sync at 3,572 nodes, 4,837 edges and 943 cards;
  deep meaning layer remains unbuilt. Log `pubsync-trail-check.log`, SHA-256
  `bc77b1c82984f85ace632af8382d9800cc4015579fe1bea7373a8bcb082499f3`.
- `git fetch origin`: exit 0. Before this docs-only sync, `HEAD` and `origin/main` both equal
  `e5d3e40c308679a356998e506441eba2c013e17a`, divergence `0 0`. Logs `pubsync-fetch.log`,
  `pubsync-state.log`, and `pubsync-divergence.log` SHA-256 `8051da3aaf5ed31b3c200f7693a6133f151dc4acede082e677690f7918b918d8`,
  `cd03b266fe41ec4b19e03929c09be9ee43c26b17efca54d8e2a74dc6ea4c0ca9`, and
  `231d953f3e206b69f61bb624fcd69294244ee7ad6bf019bfa1f8f965c5328f82`.
- Status log `pubsync-status.log`, SHA-256
  `e3afb40b96bbf2634779387fab03f5db9c2bfa8ae812b148d41d2c53b17be8a3`, confirms only the two
  staged docs plus the excluded pre-existing user-owned `native/api/CMakeLists.txt` and untracked
  `native/api/host/`.

Observation: both required path audits pass and no user-owned API path is staged. Inference: none.
Round73 tasks remain `FIXTURE_VERIFIED`, G0 remains PARTIAL and G1–G6 remain NOT RUN. Next exact
action: commit and push this two-document publication record, fetch and verify the new remote tip,
then begin independent Round74 scouting.

## Publication-record sync post-push checkpoint

Checkpoint time: 2026-10-21 17:04 CDT (America/Chicago). Agent: `/root`. Commit
`4e7d0fc84a491b746b0efe54a2ec87def8903790` (`docs: record Round 73 publication`) pushed the
two-document sync. The first push exited 0 and moved `main` from `e5d3e40` to `4e7d0fc`; a
post-fetch `git push origin main` also exited 0 with `Everything up-to-date`. Confirmation log
`local/validation/root-round73-scout/pubsync-final-push.log`, SHA-256
`6a296371679d55bc9e67c365fd337f2114bb1b68e3e56f995601143a8d29c005`.

- `git fetch origin`: exit 0. Post-push `git rev-parse HEAD origin/main` reports both
  `4e7d0fc84a491b746b0efe54a2ec87def8903790`; divergence is `0 0`. Logs
  `pubsync-state.log` and `pubsync-divergence.log`, SHA-256
  `405f83e20b2d50e755dff5da8b9619dbc0e488baac8bfcbca066d79cb4e8225f` and
  `231d953f3e206b69f61bb624fcd69294244ee7ad6bf019bfa1f8f965c5328f82`.
- `git status --short --branch`: only pre-existing user-owned `native/api/CMakeLists.txt` and
  untracked `native/api/host/` remain. Log `pubsync-status.log`, SHA-256
  `e0335ec91a749d2e476cb33be39e2b906e147877be8468e0ce4758917953e836`.
- After all local logs were present, `python3 -I scripts/project_index.py` exited 0 and
  `python3 -I scripts/project_index.py --check` exited 0 at 14,468 folders. Logs
  `pubsync-final-index-generate.log` and `pubsync-final-index-check.log`, SHA-256
  `b8fde651bc090b143c0607ec67613ca52b902f336993b7fd221e7fd61a641f36` and
  `f13eeff12b22e2aa01a70e27c6f29f138992718cb3930a53033129c3f47e7d21`.
- The docs sync was limited to `HANDOFF.md` and this checkpoint. It changed no source or test file;
  Round73 statuses and all gate labels remain unchanged.

Observation: `4e7d0fc` is the fetched remote tip; only the same user-owned API changes remain in the
worktree. Inference: none. Round73 is published and all three tasks remain `FIXTURE_VERIFIED`; G0
remains PARTIAL and G1–G6 remain NOT RUN. Next exact action: inspect unclaimed task rows and use Graft
to scout separate fixture candidates before publishing bounded Round74 claims.
