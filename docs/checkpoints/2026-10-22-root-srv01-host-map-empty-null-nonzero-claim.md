# Checkpoint: SRV-01-HOST-MAP-EMPTY-NULL-NONZERO-SIZE-FIXTURE-01 — claimed

- Time / timezone: 2026-10-22 01:45 CDT (America/Chicago)
- Agent and integrator: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`)
- Task / gate / status: `SRV-01-HOST-MAP-EMPTY-NULL-NONZERO-SIZE-FIXTURE-01`, child of
  verified `SRV-01-HOST-MAP-EMPTY-NONNULL-ZERO-SIZE-FIXTURE-01`; `ACTIVE`, implementation not
  started, 0 failed tries.
- Prerequisite evidence / input and source hashes: parent claim/completion records are in
  `docs/checkpoints/2026-10-21-root-srv01-host-map-empty-nonnull-zero-claim.md` and
  `docs/checkpoints/2026-10-21-root-srv01-host-map-empty-nonnull-zero-complete.md`; E251/E252 hold
  the parent test evidence/publication. Live header `native/core/host_map.hpp` SHA256
  `283c3b8f0c98ccb48cb756e9e16ba4fd54c98babc1ea60b942f2cc2276c30bbe`; unchanged implementation
  `native/core/host_map.cpp` SHA256
  `0ce60c6af1e2d4f9f91f5882326bf516f20dd5e58f936f99b0efc67cbb582f72`; baseline test file
  `native/tests/test_host_map.cpp` SHA256
  `d55b45455b3b19a365fb7f035146d48fd75f7b6b7564073bafeed5faf9f50926`.
- Owned / changed files: one synthetic assertion in the existing `main()` in
  `native/tests/test_host_map.cpp`; no production change.
- Hypothesis and expected result: `HostMapping::empty()` returns true for a null `ptr` even when
  `size` is nonzero. The new local `HostMapping` state should set `size = 1`, leave `ptr == nullptr`,
  and assert `empty()`. The current fixture already covers non-null pointer with zero size.
- Exact baseline commands, CWD `/Volumes/Extra/SoC Mac Native`:
  `cmake --build local/build/native --target test_host_map` -> exit 0;
  `ctest --test-dir local/build/native --output-on-failure -R '^host_map$'` -> exit 0, 1/1.
- Exit / observed result: the target built and focused CTest passed 1/1 before any source change.
  Logs are `local/validation/20261022-root-fixture-baselines/srv01-host-map-empty-null-nonzero-baseline-build.log`
  (SHA256 `c6a4b5ca13ce84a970f60a6231a0c43038e207bf62d80ce0e8e2926e5a0a94d4`) and
  `local/validation/20261022-root-fixture-baselines/srv01-host-map-empty-null-nonzero-baseline-ctest.log`
  (SHA256 `50c4307e2702737affb26b07026016663a300e6e324d4a6a81cae7f6819c269d`).
- Log, capture and REA evidence paths/IDs: the baseline logs above. Trail source query identified
  `HostMapping::empty()` at `native/core/host_map.hpp:L24` and the existing test body at
  `native/tests/test_host_map.cpp:L15-L95`; no REA or game input is involved.
- Observation versus inference / coverage limitations: `empty()` is directly defined as
  `ptr == nullptr || size == 0`; the proposed state isolates the first operand. Normal map paths
  do not produce this fixture state. It does not test mmap, allocator integration, or game behavior.
- Cumulative failed tries and distinct hypotheses: 0 implementation tries / 0 failed tries.
- Remaining risks / blockers: defensive synthetic accessor coverage only. No blocker to the bounded
  assertion.
- Index and Trail checks: `python3 scripts/project_index.py` and its `--check` exit 0 at 24,080
  folders. `trail build --check` exits 0 at 3,617 nodes / 1,240 files; deep meaning layer remains
  unbuilt. Combined output is
  `local/validation/20261022-root-fixture-baselines/srv01-host-map-empty-null-nonzero-work.log`,
  SHA256 `f316c907a52968f44209ea90e70e3fa991fc37562263ed834d97ae3cb12ff929`. `git diff --check`
  passed.
- Claim commit `b33b4bfbb60c746853e6a2ca1bd5a983e92825c3` was pushed. Staged/full audits accepted
  6/1,711 paths; fresh fetch/divergence was 0/0; post-push explicit/full audits accepted 6/1,711.
  Publication log is `local/validation/20261022-root-fixture-baselines/srv01-host-map-empty-null-nonzero-publication.log`,
  SHA256 `973ec1915bf00cee11e855c9df0be06f914e869152f2355a552398aaccb05005`.
- Worker `/root/next_fixture_scout_2` added only the planned assertion. Worker target build and
  focused CTest exit 0, with 1/1 test passing. Worker build/CTest log SHA256 values are
  `0524da2e8cc2458ec343a44bc4b73f53179647b2df39031b43c2b170235a77a1` /
  `21d56d120f2115f4b5e4af45a1581c07c4d09e7d4666a3a64f1fa9a953d5c568`.
- Root independently configured `local/build/native-root-srv01`, built target `test_host_map`
  from source, and ran focused `host_map` CTest; all commands exit 0 and CTest passes 1/1.
  Configure/build/CTest log SHA256 values are
  `c410a24b362f582c3563f2028f1e7e344eb0103a64e11bbdf55b4bf220726094`,
  `536dc48403ffe77648d3d5b5852caebeff8323ef0109907e4c1d85b47ea475f2`, and
  `1887b4e3e5f063379c58c2553ca00650b9b9b6fcbde42031dadad0f927e0be25`.
- Completion source hashes: test
  `4a9f83894e5a40c3335dd2aa91520fc1f42fd36c8ea7bc0f74bedb8894130e4b`; unchanged header
  `283c3b8f0c98ccb48cb756e9e16ba4fd54c98babc1ea60b942f2cc2276c30bbe`; unchanged implementation
  `0ce60c6af1e2d4f9f91f5882326bf516f20dd5e58f936f99b0efc67cbb582f72`.
- Completion commit `ddcb75292a9be2fa543f756c6dfa69eda574b8cf` was pushed with staged/full audits
  accepting 8/1,713 paths; fresh fetch/divergence was 0/0 and post-push explicit/full audits
  accepted 8/1,713. Shared publication log SHA256:
  `97f60b5f2e7b6ce49891d753977e4bc6902ac5f679addabea8ece099613c72f1`.
- Next exact action: continue the claimed APP-01 task and the active region-list fixture; see the
  current handoff for their owners and status.
- HANDOFF.md / TASKS / EVIDENCE / INSTALL_LOG / indexes updated: HANDOFF, TASKS, and EVIDENCE now
  record the claim; `INSTALL_LOG.md` has no change. `docs/FOLDERS.md` is regenerated and current.
