# Checkpoint: CPU-01-NAMEHASH-INLINE-EMBEDDED-NUL-FIXTURE-01 — claimed

- Time / timezone: 2026-10-22 03:54 CDT (America/Chicago)
- Agent / integrator: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`);
  proposed implementation owner `/root/next_fixture_scout_2` after claim publication.
- Task / gate / status: `CPU-01-NAMEHASH-INLINE-EMBEDDED-NUL-FIXTURE-01`, child of
  `CPU-01-NAMEHASH-E71990-01` under `CPU-01`; `ACTIVE`; root baseline passed; zero implementation
  attempts and zero failed tries.
- Ownership / scope: only `native/tests/test_name_hash.cpp`. Add one synthetic assertion
  `CHECK(name_hash_inline("abc\0def") == 0x0b873285u);`. This pins the `const char*` inline helper's
  stopping at the first NUL. Do not add/import original game-name strings or change implementation,
  header, or CMake.
- Baseline source hashes: `test_name_hash.cpp`
  `c58326919ac3512d74448d0696dde70f4b42c25959a3c33b49abd42d0c4e3da5`; unchanged
  `name_hash.cpp`, `name_hash.hpp`, and `native/CMakeLists.txt` hashes
  `3d00d9dd2821801481b9a12e6c57fe4e7401e27fbe02af69ec2683d85cabe4a7`,
  `ca3bee89f348b67d128f9c25c8fba6793080b9bd1aeb64a8e1da190c0dd17b8e`, and
  `cd46d94590f58515152155658f609cd11ef58aeb9a45272f744402edcdeeb01f`.
- Root Trail evidence: `trail grep 'name_hash_inline' --in native/` exited 0 with 9 hits; current
  inline checks cover empty input, a vector, zero hash, and non-ASCII bytes but not an embedded NUL.
  Implementation resolves at `native/core/name_hash.cpp:L21-L27`; current test span at
  `native/tests/test_name_hash.cpp:L11-L46`. Log SHA256
  `c8b47c51c45b01953c4dc5bfc919dd41ec93f10ee041b9c8976580aba4fa3f17`.
- Exact root baseline commands (CWD `/Volumes/Extra/SoC Mac Native`; fresh AppleClang 21 / Ninja
  Debug build directory `local/build/native-root-cpu01-namehash-inline-embedded-nul-baseline-20261022`):
  `cmake -S native -B local/build/native-root-cpu01-namehash-inline-embedded-nul-baseline-20261022 -G Ninja -DCMAKE_BUILD_TYPE=Debug`
  — exit 0, configure log SHA256
  `62341cf32eeb8a65ac7bf6387f29ca86ca972620bac061834f354a1d0b8c89e0`;
  `cmake --build local/build/native-root-cpu01-namehash-inline-embedded-nul-baseline-20261022 --target test_name_hash`
  — exit 0, build log SHA256
  `014b4b81c925c9cc585a14190d3305ff331fe73196605ce4d46f1576385d0ce9`;
  `ctest --test-dir local/build/native-root-cpu01-namehash-inline-embedded-nul-baseline-20261022 --output-on-failure -R '^name_hash$'`
  — exit 0, focused CTest passes 1/1, log SHA256
  `f05df3f1a9693a5223382be8e837b860d42e550bc508f83868f0b685f08b0440`. Logs are under
  `local/validation/20261022-root-fixture-baselines/`.
- Expected constant check: applying the existing djb2-XOR recurrence to the three bytes in `abc`
  yields `0x0b873285`; command output log SHA256
  `1f6116358afc5242357aa4c6234d85bbbaadd7ef51b72bff968245df77110999`.
- Observation versus inference: the current tests leave this inline helper's NUL termination
  unpinned. The expected result follows the helper's C-string loop and the independently checked
  prefix hash. This remains a synthetic string fixture; original game strings are not involved.
- Remaining risk: verify the escaped literal and expected value in the changed test using the focused
  Darwin build and CTest. The separate question whether original game-name strings must be imported
  remains open and out of scope.
- Publication / dispatch: the claim will be published with the recursive-mutex completion. Do not
  edit the test until the claim commit is pushed and verified. Root owns all docs, independent replay,
  and publication. User-owned native API files stay untouched.
- Next exact action: publish the verified recursive-mutex completion and this claim; then dispatch
  the one-line test edit.
- Task status and attempt count: `ACTIVE`; zero implementation attempts and zero failed tries.
