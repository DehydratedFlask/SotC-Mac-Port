# Checkpoint: SRV-01-RECURSIVE-MUTEX-REENTRANT-TRYLOCK-FIXTURE-01 — fixture verified

- Time / timezone: 2026-10-22 03:58 CDT (America/Chicago)
- Agent / integrator: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`); worker
  `/root/next_fixture_scout_2` owned the single test-file edit.
- Task / gate / status: `SRV-01-RECURSIVE-MUTEX-REENTRANT-TRYLOCK-FIXTURE-01`, child of
  `SRV-01-RECURSIVE-MUTEX-01` and `CPU-01-MUTEX-CONTRACT-01`; `FIXTURE_VERIFIED`; one successful
  implementation attempt, zero failed tries.
- Files changed by implementation: only `native/tests/test_recursive_mutex.cpp`. After two successful
  recursive `lock()` calls, the test asserts that owner-side `try_lock()` succeeds and balances the
  extra recursion level with one successful `unlock()`. The implementation, header, and CMake files
  are unchanged. Existing user-owned `native/api/CMakeLists.txt` and `native/api/host/` are untouched.
- Final source hashes: `test_recursive_mutex.cpp`
  `ce7c58e17ae3297de0097db539303d1e7b895f97a2f6d547eba99e9f585ee702`; unchanged
  `recursive_mutex.cpp`, `recursive_mutex.hpp`, and `native/CMakeLists.txt` hashes
  `f61fb74be7c771b35203a9cf98d658fd3bba4d00c2c023e19404b37a4f18b6aa`,
  `f97aea6beb99aed0bc5348bb92dc70d059b965558d1eaef60c4c57600678d5df`, and
  `cd46d94590f58515152155658f609cd11ef58aeb9a45272f744402edcdeeb01f`.
- Worker commands (CWD `/Volumes/Extra/SoC Mac Native`; fresh Debug/Ninja directory
  `local/build/native-worker-srv01-recursive-mutex-reentrant-trylock-20261022`): configure, build
  `test_recursive_mutex`, and focused `recursive_mutex` CTest all exit 0; CTest passes 1/1. Log
  SHA256 values for configure/build/CTest/diff-check are
  `a5c471c9156cc328817045fa62c65509a9a765331df17eb8c32789144ea4d3bc`,
  `a096e0a973dbadb9eb1ff25a025a987751650d9521ae9ab4d38421d6b32a7e2e`,
  `de7c3fc95b8c9af3387323aae9805e16f1546baaae5afb581097252ab15171e0`, and
  `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`.
- Independent root commands (CWD `/Volumes/Extra/SoC Mac Native`; fresh Debug/Ninja directory
  `local/build/native-root-srv01-recursive-mutex-reentrant-trylock-root-replay-20261022`):
  `cmake -S native -B local/build/native-root-srv01-recursive-mutex-reentrant-trylock-root-replay-20261022 -G Ninja -DCMAKE_BUILD_TYPE=Debug`
  — exit 0, configure log SHA256
  `cc7ffd554e963779770aa548d57788e48b69e61c4d39dbbdab0d59eb7cd0a522`;
  `cmake --build local/build/native-root-srv01-recursive-mutex-reentrant-trylock-root-replay-20261022 --target test_recursive_mutex`
  — exit 0, build log SHA256
  `4d53ff4c6b2edca347ba5f398d33beeb2e92f735b5b601572cc20f8b07d3186c`;
  `ctest --test-dir local/build/native-root-srv01-recursive-mutex-reentrant-trylock-root-replay-20261022 --output-on-failure -R '^recursive_mutex$'`
  — exit 0, focused CTest passes 1/1, log SHA256
  `25e0c71d4f2fcc893b53a706e1a5e7323d67f4c8cbc61bd410076a2d64e40858`.
- Trail post-change query: `trail grep 'try_lock' --in native/` exits 0 with 6 hits in 4 symbols
  across 3 files; the new owner assertion resolves at `native/tests/test_recursive_mutex.cpp:L45`.
  Persisted query log SHA256 `8d98f5004808fa01b920a7d184cc7dced1df36303c04cce573e4e040a9e97c49`.
  `git diff --check` exits 0 with empty output (SHA256
  `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`).
- Folder-index generation and check both pass at 37,436 folders; log SHA256 values are
  `364ed7c4daf565d2d2b97ce82b98f6469eb0890a9f3736cc2454178f86db37f7` and
  `161630131adf1d25b70a7428bfe1c504af6ffbeca42d570e9b5a4d2282858f55`. `trail build --check`
  passes with a synchronized 3,618-node / 1,240-file wiring graph; the deep meaning layer remains
  unbuilt (log SHA256 `f7231c65a9ccdc9f1ec3de943e3fe9b3a2be977fdf687c86e771d1c9c416a2dc`).
- Observation versus inference: the current-thread recursive `try_lock()` succeeds on the local
  Darwin pthread implementation and its recursion level is released before the existing competing
  thread checks. This does not test scheduler priority inheritance under contention or game behavior.
- Remaining risk / acceptance: this is a deterministic local helper fixture. It makes no claim about
  game callsite reachability, scheduler policy, or game readiness.
- Publication state: the task claim is in pushed commit
  `1400c835ff74b1744c65ded8e0139cb2051b92fa`; completion changes and the next CPU fixture claim are
  ready for the root allowlisted audit, commit, push, and fresh fetch/divergence check. The existing
  API edits must remain unstaged.
- Next exact action: stage only allowlisted test and root-owned documentation paths, run staged and
  full publication audits, commit/push, and verify `origin/main` at 0/0 divergence.
- Task status and attempt count: `FIXTURE_VERIFIED`; one successful implementation attempt; zero
  failed tries.
