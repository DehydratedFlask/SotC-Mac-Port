# Round 71 fixtures complete

Checkpoint time: 2026-10-21 16:34 CDT (America/Chicago). Integrator: `/root` (Codex desktop task
`01a15eab-88d5-7a83-a23b-cc55dc002eb6`). The claims were published before dispatch in commits
`bb1f58628a85dbea0331e8195d792a49193d02d8` and claim-doc sync `3a866fa106f43d7cc4bff3ec46fc1dac3cd6cbcc`.

## Completed tasks

| Task | Status / attempts | Files changed | Result |
|---|---|---|---|
| `APP-01-SYSTEM-CHECKS-CORE-MISSING-PATH-FIXTURE-01` | `FIXTURE_VERIFIED`; one successful attempt, zero failed tries | `launcher/Tests/LauncherCoreTests/AssetValidatorTests.swift` | Missing `SoCGameCore` path yields the exact expected `.notBuilt` message. Production `SystemChecks.swift` unchanged. |
| `RE-02-DYNLINK-MULTI-NEEDED-MODULE-FIXTURE-01` | `FIXTURE_VERIFIED`; one successful attempt, zero failed tries | `scripts/test_ps4_dynlink.py` | Synthetic ELF with two different needed-module IDs parses both and reports the second version `[3, 4]`. Production parser unchanged. |
| `SRV-01-REGION-SPLIT-OVERSIZE-LEAD-NO-MUTATION-FIXTURE-01` | `FIXTURE_VERIFIED`; one successful attempt, zero failed tries | `native/tests/test_region_split.cpp` | Lead `0x10001` against free size `0x10000` is rejected and both synthetic node records remain byte-identical. Production helper unchanged. |

The only implementation changes are the three assigned test files. Existing user-owned
`native/api/CMakeLists.txt` and untracked `native/api/host/` are untouched and must remain excluded.

## Commands and evidence

| Scope | Exact command | Worker result | Root replay |
|---|---|---|---|
| Launcher | `swift test --package-path launcher --scratch-path local/build/launcher --filter AssetValidatorTests` | Exit 0, 11 tests; `app-worker.log`, SHA-256 `f59d46065726d6c471acf1984b8cfcbe3686bdb87e9726763df02bd53b1c5964`. | Exit 0, 11 tests; `app-root-replay.log`, SHA-256 `dceaf027e8ce2f066adb16eaf1a36c4cd3c3c20b9150695661463b7971ddf7a3`. |
| Dynlink parser | `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_ps4_dynlink.py` | Exit 0, 26 tests; `dynlink-worker.log`, SHA-256 `473c6d30ed868640c6e7a4f94593f5cdb6107a7f5776101318861ccab070ba80`. | Exit 0, 26 tests; `dynlink-root-replay.log`, SHA-256 `1bdefdd928b8e7365d8aae40791acfe542c28ed6e9f684cede822c670ee39484`. |
| Region split | `cmake --build local/build/native --target test_region_split`; `ctest --test-dir local/build/native -R '^region_split$'` | Build exit 0, CTest exit 0 (1/1); logs `region-split-worker-build.log` SHA-256 `8afdf348bbda2cd741eda1e7216d82fc246741da3b539ee41c7522696b30af26` and `region-split-worker-ctest.log` SHA-256 `e8dd7345de124aead1d4c9331b5f5ce0c18082458c7826a9b4058a9ef73e4636`. | Build exit 0, CTest exit 0 (1/1); logs `region-split-root-build.log` SHA-256 `21a11a3b5ef65d6abb448e29a2eec6df2cfbe0621748eea0a75d2448d65bf0ab` and `region-split-root-ctest.log` SHA-256 `99b27dcfb2a018511cac66cff281133728de81b4262a7b49d9183c75b4a44a84`. |

Logs are under `local/validation/root-round71-scout/`. Final edited-file hashes:

- `launcher/Tests/LauncherCoreTests/AssetValidatorTests.swift`: `f10fce129f002573b34b2e99a8c3fb1fba32d84d08472ed12f9dce34a63c41e1`.
- `scripts/test_ps4_dynlink.py`: `653a53a67891995fdcdb2ac85abe5407836ac6cda574241f3de41dd0d9342e10`.
- `native/tests/test_region_split.cpp`: `679b8880c754b428d1cac1f01b08baff964210554fd7355f8e73a3e9b6a296fc`.

Production hashes are unchanged: `SystemChecks.swift` `cf3d6ce5dd5663b10291c5c53169aecb2ef02bc7912960e29af2789e60a3b0d8`, `ps4_dynlink.py` `28c53257a6f2bd58522f123098d41ca0d884222330a1bf18ce9f8935fb45eff9`, and `region_split.cpp` `aeead9c2a91b49283e0924d1bfba5ac98bb56aee50a1cc1f320a6e6924d0021f`.

## Observation, limits, and next action

Observed: all focused worker and root checks pass; exact assertions match the claimed cases, and no
production source changed. Inference is limited to those unit/helper inputs. These fixtures do not
demonstrate ELF loading, native core execution, launcher runtime behavior, original allocator
integration, game boot, authentic audio, save/reload, or Metal gameplay.

G0 remains PARTIAL; G1–G6 remain NOT RUN. No install or dependency facts changed. Attempt counts are
one successful implementation attempt and zero failed tries per task.

## Index and graph refresh

At 2026-10-21 16:33 CDT, `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/project_index.py` exited 0
and wrote the index at 14,466 folders; log SHA-256
`7a011c9c98b920104871e7cee6a0c371391892a025c329d853d8c830c2ca0d99`. The matching `--check`
exited 0 at 14,466 folders; log SHA-256
`cfda4d504c8a996ed5e632aa4f3299497abf52178d627dc583bd192c496e3886`.

`trail build` exited 0 and refreshed the wiring graph to 3,572 nodes, 4,836 edges, and 943 cards;
the build parsed 3 files and replayed 1,233 from cache. Log SHA-256
`81d0b209033030ede42ba16b426352ba460c0440537bdff8a74880c639b6cf6d`. `trail build --check`
exited 0 and reported the wiring graph in sync; optional meaning summaries remain unbuilt. Log
SHA-256 `99342face5580b625b2480268e4e5b63ecfebc573b79c791cf004539b078027b`.

`git diff --check` on the three test files and completion docs exited 0; its empty log SHA-256 is
`e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`. The claim checkpoint records
an earlier private folder-index check failure and its successful regeneration/recheck; no task
test or implementation attempt failed.

## Publication

At 2026-10-21 16:33 CDT, the staged path audit accepted exactly 9 allowlisted paths; log SHA-256
`243e9d14434207e9f5a27dd3f8837ab7e7c74c3af6fff8d5b6f2a8c22f9dbc8b`. The full path audit accepted
1,647 paths; log SHA-256 `bcef1df30de2f6ae70602e3e3fdbbb6416eb416c435da7a635aae3a34f68917f`. Staged
`git diff --cached --check` exited 0 with empty output (SHA-256
`e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`).

These checks include only the three test files, task/evidence/handoff/index documents, and Round71
checkpoints. The pre-existing user-owned `native/api/CMakeLists.txt` and untracked `native/api/host/`
were unstaged and excluded.

Completion commit `6e517b91a849c48a2775edce346c9539abb8c7e6` (`test: verify Round 71 launcher,
dynlink, and split fixtures`) exited 0; commit log SHA-256
`27d961e2e807dcef4fe01f46083836ae02829113421f102b34ac458246ff39a2`. `git push origin main`
exited 0; push log SHA-256 `c14c88915525a59d6939e02d7ca7e8289c2b9a8e9bbdde3af04dde8605d9bcc5`. After
`git fetch origin`, `HEAD == origin/main == 6e517b91a849c48a2775edce346c9539abb8c7e6`, divergence
`0 0`; fetch log is empty (SHA-256
`e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`), and state log SHA-256 is
`623d3494bdff76f2d1d74ee46fa94c13148c567ea0c9d651bd042cac5f1e3562`. The only remaining worktree
changes are `native/api/CMakeLists.txt` and `native/api/host/`, preserved as user-owned state and
excluded from publication.

The bounded Round72 claims in `docs/TASKS.md` and [their claim checkpoint](2026-10-21-root-round72-fixture-claims.md)
were published before implementation in `df1af7d60f48c4b51222f6fb7705f93fa7eaae1c`. Next exact
action: dispatch the three single-file tasks. Round71 fixtures do not advance a game acceptance gate.
