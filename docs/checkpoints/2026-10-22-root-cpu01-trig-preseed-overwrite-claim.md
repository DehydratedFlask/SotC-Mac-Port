# Claim checkpoint: CPU-01-TRIG-CONSTANTS-PRESEEDED-STATE-OVERWRITE-FIXTURE-01

- Local date/time: 2026-10-22 07:09 CDT (America/Chicago). Claim owner/integrator: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`); proposed implementation worker: `/root/fixture_candidate_scout_c`.
- Task / parent / status: `CPU-01-TRIG-CONSTANTS-PRESEEDED-STATE-OVERWRITE-FIXTURE-01`, child of `CPU-01`; `ACTIVE`, zero implementation attempts and zero failed tries. No matching task row was present in `docs/TASKS.md` at claim time.
- Bounded ownership: worker may change only `native/tests/test_trig_constants.cpp`. Add a test case that seeds a second `TrigConstants` with sentinel mask words and cosine, calls `init_trig_constants`, then asserts the exact initialized mask and the existing one-ULP `cosl` oracle for cosine. Root owns shared docs, root replay, review, and publication. Production code, headers, CMake, API/Graft paths, references, and game files are out of scope.
- Baseline HEAD `6581d40d82ae2d3efc52eeaf227f3db730ac592d`. Test/helper/header/CMake SHA256: `940e9b385bf0f3d3d7e48689a55bef4e0191ab74ffeed9677177233e5df4929f` / `8768369133d034244d3c1ba1ad1ec0265242c3cc34ab835fc990e70234147e6f` / `7ca4267565ad0805c58856ba81663543fd622ebdb0007adae60d588018ceeb87` / `cd46d94590f58515152155658f609cd11ef58aeb9a45272f744402edcdeeb01f`.
- Graft query `graft grep "init_trig_constants" --in native/` returned 4 hits in 4 symbols. It located the helper at `native/core/trig_constants.cpp:L9-L12`, the declaration, the existing test, and a recovered-constructor registration. Live-source inspection confirms the helper writes `xyz_mask = {0xffffffffu, 0xffffffffu, 0xffffffffu, 0}` and `cos_3_degrees = libm::fsin(..., 1)`; current test initializes only a zero-initialized struct. Graft log SHA256 `30924ddeea759a498d008a72fae03c7be07d0a6e2ce50bf9cfcc08ee3908da5e`.
- Root baseline commands, all exit 0:
  1. `cmake -S native -B local/build/native-root-cpu01-trig-preseed-baseline-20261022 -G Ninja -DCMAKE_BUILD_TYPE=Debug`
  2. `cmake --build local/build/native-root-cpu01-trig-preseed-baseline-20261022 --target test_trig_constants`
  3. `ctest --test-dir local/build/native-root-cpu01-trig-preseed-baseline-20261022 --output-on-failure -R '^trig_constants$'`
  Baseline CTest passed 1/1. Configure/build/CTest log SHA256: `0dd747c4b53a1aead2e966649875c642fb406b9a64ec7d05486338313ca920b5` / `bbd6852e09f5547eb98831eef72362455de77975deb6fb3f6df5dffad658d244` / `700020f062f7773c09eefb00b427a5cde895e137fdb00524637164a9c4de421d`.
- Worker acceptance commands:
  1. `cmake -S native -B local/build/native-worker-cpu01-trig-preseed-20261022 -G Ninja -DCMAKE_BUILD_TYPE=Debug`
  2. `cmake --build local/build/native-worker-cpu01-trig-preseed-20261022 --target test_trig_constants`
  3. `ctest --test-dir local/build/native-worker-cpu01-trig-preseed-20261022 --output-on-failure -R '^trig_constants$'`
  At claim time these implementation commands have not run. Root will replay in `local/build/native-root-cpu01-trig-preseed-verify-20261022/` after worker completion.
- Observation versus inference / limitation: this would verify synthetic helper initialization from a non-default struct state only. It does not establish constructor reachability, game startup state, or gameplay behavior.
- Exact next action: publish this claim with the verified symlink-reader fixture, then dispatch the one-test change.
