# Checkpoint: SRV-01-REGION-SPLIT-SUCCESS-PRESERVE-FREE-NODE-METADATA-FIXTURE-01 — complete

- Time / timezone: 2026-10-22 02:41 CDT (America/Chicago)
- Agent and integrator: implementation `/root/next_fixture_scout_2`; independent replay and docs
  `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`).
- Task / gate / status: `SRV-01-REGION-SPLIT-SUCCESS-PRESERVE-FREE-NODE-METADATA-FIXTURE-01`,
  child of `SRV-01`; `FIXTURE_VERIFIED`, one successful implementation attempt, zero failed tries.
- Prerequisite / hashes: claim and baseline are in
  [claim checkpoint](2026-10-22-root-srv01-region-split-metadata-claim.md) and E289. Final test
  `native/tests/test_region_split.cpp` SHA256
  `dbcb9faf352f978eb93b05249c55789975ed0adfbf141fdc6c50a73bf7d9b589`; unchanged
  `native/core/region_split.cpp` / `native/core/region_split.hpp` SHA256
  `aeead9c2a91b49283e0924d1bfba5ac98bb56aee50a1cc1f320a6e6924d0021f` /
  `be19e19e04ea8cd7040a59058253d7f7ca9a596e46216320a512c786a1f66acb`.
- Owned / changed source: one synthetic success-path case in `native/tests/test_region_split.cpp`.
  It seeds a free node, snapshots its raw bytes, calls `split_free_at_lead` with `lead = 0xf000`,
  checks capacity and size equal `lead`, and compares bytes before `+0x50` and from `+0x60` to the
  snapshot. Production code and build files are unchanged.
- Exact worker commands, CWD `/Volumes/Extra/SoC Mac Native`:
  `cmake --build local/build/native --target test_region_split` -> exit 0;
  `ctest --test-dir local/build/native --output-on-failure -R '^region_split$'` -> exit 0, 1/1.
  Worker build/CTest log SHA256 values are `8afdf348bbda2cd741eda1e7216d82fc246741da3b539ee41c7522696b30af26` /
  `e8dd7345de124aead1d4c9331b5f5ce0c18082458c7826a9b4058a9ef73e4636`.
- Exact independent root commands, CWD `/Volumes/Extra/SoC Mac Native`, AppleClang 21 / Ninja:
  `cmake -S native -B local/build/native-root-srv01-region-split-metadata -G Ninja -DCMAKE_BUILD_TYPE=Debug`
  -> exit 0; `cmake --build local/build/native-root-srv01-region-split-metadata --target test_region_split`
  -> exit 0; `ctest --test-dir local/build/native-root-srv01-region-split-metadata --output-on-failure -R '^region_split$'`
  -> exit 0, 1/1; `git diff --check` -> exit 0. Configure/build/CTest log SHA256 values are
  `705cd533d6e5e544db6b3e232415e6338b064d559d1e1e54161cdf26c7c93372` /
  `a2e0444fc66938c8d3c531e6bb9f3fe7b6abb0fd8d7aa6be4be546e466c7b691` /
  `21f581fe0a97124663be20cb01f0c8c623fded50496b8907ff7d749eda8f59e2` under
  `local/validation/20261022-root-fixture-baselines/`.
- Failed tooling invocation: the first root CTest wrapper exited 1 with a Python
  `SyntaxError: unterminated string literal` while generating its log-write expression; CTest did
  not start. The corrected wrapper ran the same CTest arguments and passed. Record:
  `local/validation/20261022-root-fixture-baselines/srv01-region-split-metadata-root-ctest-wrapper-error.log`,
  SHA256 `dbc37de4c33edbe920809dee34777938f8df291c5ea9cd4c4bc6a9dc6627d173`.
- Observation versus inference / limits: the focused fixture passes and the compared free-node raw
  bytes outside capacity/size are unchanged in this synthetic call. This is helper-level evidence
  only; it does not establish allocator-runtime behavior, a native game allocator, or game behavior.
- Remaining risks: no allocator integration or runtime test. No blocker for this fixture.
- Next exact action: `/root/next_fixture_scout_2` is implementing only the mapped-error assertions
  in `native/tests/test_save_stream.cpp` under the published claim; root will independently replay
  its focused CTest when the worker returns.
- HANDOFF / TASKS / EVIDENCE updated. `python3 scripts/project_index.py` and its `--check` pass at
  29,507 folders. The first check reported stale `local/indexes/folders.json` because its wrapper
  created a new local log after generation; after precreating the log paths, regeneration and check
  passed. Initial and corrected check log SHA256 values are `c806373a4a3407f5886096b07df4c504c1fadf17f65c11d6570670463dddc724` /
  `ad1be16ea5a959e42131be310e05c46b01a0300322b53740f76168205a891e95`.
  `trail build --only-dir scripts --only-dir tools --only-dir reference --only-dir native --only-dir launcher --only-dir tests`
  exits 0 and `trail build --check` exits 0 at 3,618 nodes / 1,240 files; Trail build/check log
  SHA256 values are `c90e62e21aa307c9cd00f2f1b81618df36eb320f3ab2a748902dff63bd30d9d7` /
  `37f2a18c24b237dcd0e456bc8f019ff693cfb1df7b052283e2d6ac0d3d50527a`. The project-index generator
  log SHA256 is `a3df43b97c2f04ba2d6d1a579547a66bc0ec9c140c423cbf88d6e0b6077a2814`; root
  `git diff --check` log SHA256 is
  `09442f290a984958f6d87d36dff2195b89c003c833523d2f182d698023f9f5c9`. No install occurred.
- Publication: commit `1f194e4ef9389d33520f57a01d2439db5535d8f6` pushed `origin/main` with the new
  save-stream claim. Staged/full and post-push explicit/full audits accepted 8/1,721 paths; pre- and
  post-push fetch checks showed 0/0 divergence. See E294 for publication log hashes. User-owned
  `native/api/CMakeLists.txt` and `native/api/host/` remained unstaged.
