# Checkpoint: SRV-01-HOST-MAP-EMPTY-NULL-NONZERO-SIZE-FIXTURE-01 — fixture verified

- Time / timezone: 2026-10-22 01:51 CDT (America/Chicago)
- Agent and integrator: /root (Codex desktop task 01a15eab-88d5-7a83-a23b-cc55dc002eb6)
- Task / gate / status: SRV-01-HOST-MAP-EMPTY-NULL-NONZERO-SIZE-FIXTURE-01, child of verified
  SRV-01-HOST-MAP-EMPTY-NONNULL-ZERO-SIZE-FIXTURE-01; FIXTURE_VERIFIED, one successful
  implementation attempt and zero failed tries.
- Files changed for the fixture: one synthetic assertion in native/tests/test_host_map.cpp.
  No production code changed. Final test file SHA256:
  4a9f83894e5a40c3335dd2aa91520fc1f42fd36c8ea7bc0f74bedb8894130e4b. Unchanged
  native/core/host_map.hpp SHA256:
  283c3b8f0c98ccb48cb756e9e16ba4fd54c98babc1ea60b942f2cc2276c30bbe. Unchanged
  native/core/host_map.cpp SHA256:
  0ce60c6af1e2d4f9f91f5882326bf516f20dd5e58f936f99b0efc67cbb582f72.
- Hypothesis and observed result: HostMapping::empty() returns true when ptr is null even if size
  is nonzero. The fixture sets size to 1 on a default value and asserts empty(); it passes.
- Exact commands and results, CWD /Volumes/Extra/SoC Mac Native:
  - Worker: cmake --build local/build/native --target test_host_map — exit 0;
    ctest --test-dir local/build/native --output-on-failure -R '^host_map$' — exit 0, 1/1.
  - Root fresh configure: cmake -S native -B local/build/native-root-srv01 — exit 0.
  - Root fresh compile: cmake --build local/build/native-root-srv01 --target test_host_map —
    exit 0; the log shows the modified test source compiled and linked.
  - Root focused test: ctest --test-dir local/build/native-root-srv01 --output-on-failure
    -R '^host_map$' — exit 0, 1/1.
- Worker build/CTest log SHA256:
  0524da2e8cc2458ec343a44bc4b73f53179647b2df39031b43c2b170235a77a1 /
  21d56d120f2115f4b5e4af45a1581c07c4d09e7d4666a3a64f1fa9a953d5c568. Root configure/build/CTest
  log SHA256:
  c410a24b362f582c3563f2028f1e7e344eb0103a64e11bbdf55b4bf220726094 /
  536dc48403ffe77648d3d5b5852caebeff8323ef0109907e4c1d85b47ea475f2 /
  1887b4e3e5f063379c58c2553ca00650b9b9b6fcbde42031dadad0f927e0be25.
- Trail source query: trail grep 'null_ptr_nonzero_size' exits 0 and resolves the three assertion
  lines at native/tests/test_host_map.cpp:L24-L26. Query log:
  local/validation/20261022-root-fixture-baselines/srv01-host-map-empty-null-nonzero-complete-query.log,
  SHA256 3227350e55810e7c876e8f547bf8c35379c35b6478c48ab3af2bf49325828ecb. Trail build --check
  exits 0 with 3,617 nodes / 1,240 files; the deep meaning layer remains unbuilt. Combined project
  index generation/check and Trail check log:
  local/validation/20261022-root-fixture-baselines/srv01-host-map-empty-null-nonzero-complete-work.log,
  SHA256 2d5c71518778f654249d058c3ca0d1998438878dd7a04cae9f88cd90f0e0faf3. Project index is current
  at 24,690 folders.
- Observation versus inference / limits: the C++ accessor behavior is observed in a synthetic
  state. This does not exercise mmap, allocator integration, a native game binary, or game behavior.
- Remaining risks / blockers: no blocker to this fixture; all real-game acceptance gates remain as
  stated at the top of HANDOFF.md.
- Exact next action: commit and push only allowlisted source and documentation paths, with staged
  and full publication audits; then verify the pushed revision and publish the APP-01 claim before
  delegating its test.
- Publication status at this checkpoint: completion change is prepared for commit; publication
  facts will be recorded in a follow-up checkpoint after push.
