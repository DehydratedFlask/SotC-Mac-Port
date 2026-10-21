# NEEDS_PRINCIPAL: APP-01-CORE-HOST-01 — bad-stdin child exit remains unresolved

- Owner / principal / date: `/root` (Codex integrator) / project principal / 2026-10-21.
- Requirement and failing gate: harden the native child-process host protocol and pipe tests.
  The `test_process_bad_stdin_exits_nonzero` fixture expects exit code 4 for a read error on
  stdin. The third targeted CTest run did not satisfy that assertion. G4 remains NOT RUN; the
  host still has no implemented engine and `start` returns `ENGINE_NOT_IMPLEMENTED`.
- Input SHA256, source pins, original RVA / file offset / image base: no game input or original
  executable was used; original RVA / offset / image base are not applicable. Published source base
  at escalation is commit `d3a1ba905b3db51ade880fbd66a89cfbd8d68c8d`. Private ARM64 sanitizer build
  artifacts:
  - `local/build/native/api/host/test_sotc_host_protocol` SHA256
    `5d875046423246b8fbbd750bc002f1806c5bd2f67db54869889b7dc262a9643a`.
  - `local/build/native/api/host/sotc_game_core` SHA256
    `8adf1b8aff1f7a0a0c844cb0a180ec699e74ddd13b8c6dc72d6f902a6cb8c1b9`.
- Reproducer / expected result: `test_process_bad_stdin_exits_nonzero()` calls
  `spawn_host(true)`, which opens `/dev/null` write-only on child fd 0, then reaps the child. The
  expected result is a normal exit with code 4 (`SessionEnd::InputError`). Run from the repository
  root with `ctest --test-dir local/build/native --output-on-failure -R '^sotc_host_protocol$'`.
  The test currently checks the wait status but does not print the actual status value on failure.
- Current tree changes and recovery location: `native/api/host/` remains uncommitted, including the
  worker protocol implementation and root test-harness fixes. The compile fix stores `reap(unused)`
  in a local before `WIFEXITED`; the output fixture now overrides `sync()` to fail on flush. Separate
  uncommitted CPU-01 work remains under `native/core/`, native CMake registration, the ahead-table
  test, and `tools/lift/check_ctor_slots.py`. No forbidden game inputs are staged. Logs are under
  `local/checkpoints/`, which is private and ignored.
- Index / graph: `python3 scripts/project_index.py` exited 0; `docs/FOLDERS.md` records 213
  checkpoints and 4 escalations. The first `trail build --check` after the test-fixture edit was
  STALE; rebuilding with `trail build --follow-nested-repos --only-dir scripts --only-dir tools
  --only-dir reference --only-dir native --only-dir launcher --only-dir tests` exited 0, and the
  follow-up `trail build --check` exited 0. Logs:
  `local/checkpoints/root-app01-core-host-escalation-trail-refresh.log` and
  `local/checkpoints/root-app01-core-host-escalation-trail-check.log`. `graft/` is a private cache.

| Failed try | Distinct hypothesis/change | Exact command / environment | Actual result | Log/evidence |
|---|---|---|---|---|
| 1 | Worker protocol patch would build the focused host test target. | `cmake --build local/build/native -j 8 --target test_sotc_host_protocol`; repository root; CMake Unix Makefiles, arm64, `SOTC_SANITIZE=ON`. | Exit 2 at `test_host_protocol.cpp:508`: Apple `WIFEXITED` macro cannot take temporary `reap(unused)`. The host library and child executable built; test did not compile. | `local/checkpoints/root-app01-core-host-build.log`; exact failure also recorded in `docs/checkpoints/2026-10-21-root-app01-core-host-build-failure.md`. |
| 2 | Store the wait status in a local so `WIFEXITED` receives an lvalue; then exercise the full focused test. | Build: `cmake --build local/build/native -j 8 --target test_sotc_host_protocol`; CTest: `ctest --test-dir local/build/native --output-on-failure -R '^sotc_host_protocol$'`; same ARM64 sanitizer build. | Build exit 0. CTest exit 8 at line 333: the `run_session(...)=OutputClosed` test-double assertion was false. | `local/checkpoints/root-app01-core-host-build-attempt2.log`, `local/checkpoints/root-app01-core-host-ctest-attempt2.log`; checkpoint `docs/checkpoints/2026-10-21-root-app01-core-host-output-test-failure.md`. |
| 3 | Make the failing output stream deterministic by returning `-1` from `FailBuf::sync()`; rerun focused test. | Build: `cmake --build local/build/native -j 8 --target test_sotc_host_protocol`; CTest: `ctest --test-dir local/build/native --output-on-failure -R '^sotc_host_protocol$'`; same ARM64 sanitizer build. | Build exit 0. CTest exit 8 at line 500: `WIFEXITED(st) && WEXITSTATUS(st) == 4` is false. The assertion does not print whether the process was signaled or its actual exit code. ASan also warned that no external symbolizer was found. | `local/checkpoints/root-app01-core-host-build-attempt3.log`, `local/checkpoints/root-app01-core-host-ctest-attempt3.log`. |

- Smallest failing fixture and how to run it: the final failure is in
  `native/api/host/tests/test_host_protocol.cpp::test_process_bad_stdin_exits_nonzero`; its
  `spawn_host(true)` child reads from a write-only `/dev/null` file descriptor. The full focused
  CTest above reaches this assertion after the in-process protocol tests and earlier child-process
  tests pass. The actual wait status is not reported, so a principal-led diagnostic should first
  capture `WIFEXITED`, `WIFSIGNALED`, and `WEXITSTATUS`/`WTERMSIG` without changing the expected
  acceptance behavior.
- Verified facts versus guesses: the host library and child executable build; the test target builds
  after the `WIFEXITED` lvalue fix; `run_session` separately classifies `badbit` or non-EOF `failbit`
  as `InputError`, and `main` maps that to exit 4. The bad-stdin child test does not observe exit 4.
  It is only a guess that opening `/dev/null` write-only yields a different iostream state on this
  environment; the exact child status has not been captured.
- Why previous approaches failed / missing information: try 1 was a test-compilation issue caused by
  the platform macro's lvalue requirement. Try 2 exposed that rejecting newline only in
  `stringbuf::overflow` did not make the stream fail in this fixture; replacing it with a failing
  `sync()` fixed that test. Try 3 reached an unresolved child-process status in the bad-stdin case.
  The smallest missing fact is the child's actual wait status and the exact iostream flags set by
  reading fd 0 when it is open write-only.
- Independent work that can continue: `CPU-01-NATIVE-SLOT24-47-01` is independent; its focused
  constructor test passed 1/1 and its three offline source checkers exited 0. Continue that task
  without changing host protocol behavior.
- Task register and HANDOFF checkpoint links: `docs/TASKS.md` marks this task `NEEDS_PRINCIPAL`
  with three failed tries; `HANDOFF.md` links this escalation. Earlier records are
  `docs/checkpoints/2026-10-21-root-app01-core-host-build-failure.md` and
  `docs/checkpoints/2026-10-21-root-app01-core-host-output-test-failure.md`.
- Principal reassessment / authorized next approach: none recorded yet. Do not make a fourth host
  change or rerun the targeted host check until the principal reviews the actual wait-status gap and
  records a new assessment here.
