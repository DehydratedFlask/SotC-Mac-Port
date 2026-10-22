# Checkpoint: SRV-01-ALLOCATOR-ALIGN64K-UINT32-WRAP-FIXTURE-01 — fixture verified

- Time / timezone: 2026-10-22 03:32 CDT (America/Chicago)
- Agent / integrator: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`);
  implementation worker `/root/next_fixture_scout_3`.
- Task / gate / status: `SRV-01-ALLOCATOR-ALIGN64K-UINT32-WRAP-FIXTURE-01`, child of `SRV-01`;
  `FIXTURE_VERIFIED`; one successful implementation attempt, zero failed tries.
- Files changed: only `native/tests/test_allocator_region.cpp` for source. One assertion was added:
  `CHECK(align_up_64k(0xffff0001u) == 0u);`. Allocator implementation/header and build files remain
  unchanged. Root owns this documentation checkpoint.
- Final hashes: test `9a5a9789a961d4094bc63ed74665fc77d75b2ad827cfba6ddf0f933e345b233c`;
  unchanged `allocator_region.hpp` / `allocator_region.cpp` / `native/CMakeLists.txt`
  `6bc5fc4091d6a5ce8411d0beb0020fa07e5e93d2846c9169d7081d9a03c3696a` /
  `7ea6e93c79f43dfbad5bb447008a39c77136633f3afe97dde493d57dd69c9da8` /
  `cd46d94590f58515152155658f609cd11ef58aeb9a45272f744402edcdeeb01f`.
- Worker validation: `cmake -S native -B local/build/native-worker-srv01-align64k-wrap-20261022 -G Ninja`
  and `cmake --build local/build/native-worker-srv01-align64k-wrap-20261022 --target test_allocator_region`
  both exited 0; `ctest --test-dir local/build/native-worker-srv01-align64k-wrap-20261022 -R '^allocator_region$' --output-on-failure`
  passed 1/1. Worker build/CTest/diff-check log SHA256 values are
  `1d200a421365852715482a7f4ab9e0db06dc54d5946eb90783238c215b7349c0` /
  `eeb86db5e2aafa47305bd4bb6d0ac4a7c868d542b6ca3a8b623af1e0d76a83cc` /
  `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`. The worker's configure
  command exited 0; a separate configure log was not captured.
- Independent root validation used fresh Debug/Ninja directory
  `local/build/native-root-srv01-align64k-wrap-root-replay-20261022`:
  `cmake -S native -B local/build/native-root-srv01-align64k-wrap-root-replay-20261022 -G Ninja -DCMAKE_BUILD_TYPE=Debug`
  exited 0 (log SHA256 `38c6b82dfa02552333ddff9bd5b63c96c2093ce20b535fa8b3bcebd4bf1b6cc0`);
  `cmake --build local/build/native-root-srv01-align64k-wrap-root-replay-20261022 --target test_allocator_region`
  exited 0 (log SHA256 `610e8aad5ea4cdede5543c968d192d9994413c30d71decec300e0a20884add38`);
  `ctest --test-dir local/build/native-root-srv01-align64k-wrap-root-replay-20261022 --output-on-failure -R '^allocator_region$'`
  passed 1/1 (log SHA256 `a38ac5c721d37148fec365d9e2d91eac7950524afbfdc59a3a98ed51e16d1b0e`). All logs are under
  `local/validation/20261022-root-fixture-baselines/`.
- `git diff --check` exited 0 in worker and root; worker's diff-check log SHA256 is recorded above.
  Exact diff is one added assertion. The pre-existing `native/api/CMakeLists.txt` edit and untracked
  `native/api/host/` were not touched or staged.
- Observation vs inference: focused test success establishes the synthetic helper returns zero for
  this 32-bit input. It does not establish allocator runtime usage, input reachability, or game
  behavior. Production code, CMake, and headers are unchanged.
- Remaining risk: no runtime allocator or game integration claim is made; broader native gates are
  unchanged and incomplete.
- Next exact action: publish this allowlisted fixture and, in parallel, the separately claimed
  `SRV-01-REGION-LIST-NULL-HEAD-SLOT-INSERT-FIXTURE-01` baseline/ownership docs; root retains
  integrator responsibility.
- Task status and attempt count: `FIXTURE_VERIFIED`; one successful implementation attempt; zero
  failed tries.
