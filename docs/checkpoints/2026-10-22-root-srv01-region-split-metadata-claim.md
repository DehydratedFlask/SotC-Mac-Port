# Checkpoint: SRV-01-REGION-SPLIT-SUCCESS-PRESERVE-FREE-NODE-METADATA-FIXTURE-01 — claimed

- Time / timezone: 2026-10-22 02:28 CDT (America/Chicago)
- Agent and integrator: claim `/root` (Codex desktop task
  `01a15eab-88d5-7a83-a23b-cc55dc002eb6`); implementation assigned to `/root/next_fixture_scout_2`
  at 2026-10-22 02:34 CDT.
- Task / gate / status: `SRV-01-REGION-SPLIT-SUCCESS-PRESERVE-FREE-NODE-METADATA-FIXTURE-01`,
  child of `SRV-01`; `ACTIVE`, implementation not started, zero failed tries.
- Prerequisite evidence / input and source hashes: baseline test
  `native/tests/test_region_split.cpp` SHA256
  `8bc5a45cbb421536d3f6ed06ebd4972152df615e9f5e4813aaf7b532e7498c46`; unchanged helper,
  header, and byte checker SHA256 `aeead9c2a91b49283e0924d1bfba5ac98bb56aee50a1cc1f320a6e6924d0021f`,
  `be19e19e04ea8cd7040a59058253d7f7ca9a596e46216320a512c786a1f66acb`, and
  `1daebc41b71c98215ddf8438b948c9c629b7a61e45fc0b67a4182f42149405de`.
- Owned / changed files: planned single-file fixture in `native/tests/test_region_split.cpp`;
  no source file has been edited for this claim.
- Hypothesis and expected result: a successful `split_free_at_lead(free, neu, lead)` changes only
  the existing free node's capacity at `+0x50` and size at `+0x58` to `lead`; every other byte
  remains as seeded. Existing success coverage checks selected fields but not this raw-byte
  preservation property.
- Exact command, CWD and nonsecret environment: CWD `/Volumes/Extra/SoC Mac Native`; Graft query
  `region_split successful split preserves existing free-node metadata besides capacity and size,
  and current tests for region_split`; baseline commands
  `cmake --build local/build/native --target test_region_split` and
  `ctest --test-dir local/build/native --output-on-failure -R '^region_split$'`.
- Exit / observed result: both baseline commands exit 0; focused CTest passes `region_split` 1/1.
  No fixture implementation has been attempted.
- Log, capture and REA evidence paths/IDs: claim research note
  `local/validation/20261022-root-fixture-baselines/srv01-region-split-metadata-root-claim.log`,
  SHA256 `0b3ab03162c15626f8ad0e4c37766f1d59610a3c67bb696c461f90da71db7a5b`; baseline build and
  CTest logs SHA256 `21a11a3b5ef65d6abb448e29a2eec6df2cfbe0621748eea0a75d2448d65bf0ab` and
  `d41d55e6ded933628acbcd5f1655a1ca649a1549d99e8082ba06b37572f04639`.
- Observation versus inference / coverage limitations: Graft points to the helper at
  `native/core/region_split.cpp:L5-L28` and the existing test at
  `native/tests/test_region_split.cpp:L10-L118`. Scout review indicates the success path writes
  the new node and the existing free node's size/capacity; the proposed fixture will pin the
  remaining bytes. This is synthetic helper behavior only.
- Cumulative failed tries and distinct hypotheses: zero implementation attempts and zero failed
  tries.
- Remaining risks / blockers: no blocker to this narrow fixture; no runtime or allocator
  integration is exercised.
- Initial next action at claim time: publish this claim with the APP-01 completion using
  allowlisted audits, then delegate only `native/tests/test_region_split.cpp` to
  `/root/next_fixture_scout_2`.
- Publication: commit `6441c02473a6c64b14246075258bf7400f3ac2bc` pushed to `origin/main` with the
  APP-01 completion. Staged/full audits accepted 7/1,719 paths; fresh fetch showed 0/0 divergence;
  post-push explicit/full audits accepted 7/1,719. Audit log SHA256 values are
  `d5d4c08339281a22ca058b28d9e97e88560659fb680e115313700a3846ca4a4b` /
  `4379a1e23cbede0ba946693bd4327279b48e29440497fa95879e716122bc1d74` and are recorded in E291.
- Next exact action: receive the worker's test and focused CTest result, then independently replay
  the focused CTest from the current shared checkout.
- HANDOFF.md / TASKS / EVIDENCE / INSTALL_LOG / indexes updated: HANDOFF, TASKS, EVIDENCE, and
  `docs/FOLDERS.md` are updated; `python3 scripts/project_index.py` and its check pass at 28,897
  folders including 448 checkpoint files; `trail build --check` passes at 3,618 nodes / 1,240
  files. Combined log `local/validation/20261022-root-fixture-baselines/app01-srv01-claim-index-trail.log`
  SHA256 `4290efc2b8ace98dfd871aed2bdca43cf65b63211257105b75183e331bcf4cd3`. No install occurred.
- Outcome: the worker's implementation and independent root replay are complete; see
  [completion checkpoint](2026-10-22-root-srv01-region-split-metadata-complete.md). This claim
  checkpoint preserves the baseline and assignment state; its original next action is superseded.
