# Checkpoint: APP-01-CORE-HOST-01 — protocol fixes ready for verification

- Time / timezone: 2026-10-21 08:47 CDT (America/Chicago).
- Agent and integrator: host protocol implementation by `/root/review_host_proto`; `/root` integrates.
- Task / gate / status: `APP-01-CORE-HOST-01`, ACTIVE, attempt 0. G4 NOT RUN.
- Prerequisite evidence / input and source hashes: `APP-01-CORE-ABI-01` provides the versioned C
  ABI. This branch is at `e40c1f7`; `sotc_game_core` remains a protocol host and reports
  `SOTC_ERR_ENGINE_NOT_IMPLEMENTED` for start.
- Owned / changed files: worker changed only `native/api/host/protocol.cpp`,
  `native/api/host/protocol.hpp`, `native/api/host/main.cpp`, and
  `native/api/host/tests/test_host_protocol.cpp`. Root owns all handoff/task/evidence/index updates.
- Hypothesis and expected result: reporting the configured line limit, surfacing input errors,
  requiring the documented child exit code, and ignoring SIGPIPE in the parent test harness will
  make the existing stdio protocol failures explicit and testable.
- Exact command, CWD and nonsecret environment: CWD `/Volumes/Extra/SoC Mac Native`.
  Root ran `git diff --check` → exit 0 after the worker reported its four-file patch. No build or
  CTest command has run yet. The proposed focused commands are
  `cmake --build local/build/native -j 8 --target test_sotc_host_protocol`, followed by
  `ctest --test-dir local/build/native --output-on-failure -R '^sotc_host_protocol$'`.
- Exit / observed result: static diff check passed. The source changes add a configured max-line
  argument to `Host::handle_line`, an `InputError` result for bad/fail without EOF and process exit
  4, hello limit 64 regression coverage, a bad-stdin case, strict exit-2 assertion for closed stdout,
  and SIGPIPE/EPIPE handling in the test writer.
- Log, capture and REA evidence paths/IDs: none yet. This checkpoint is an implementation handoff;
  tests remain to run.
- Observation versus inference / coverage limitations: these are protocol-process and pipe-test
  fixes only. The proposed bad-stdin child test still needs runtime confirmation on this libc++
  environment. There is no Swift child-process `CoreClient`, game engine startup, boot, or G4 proof.
- Cumulative failed tries and distinct hypotheses: 0 failed tries; no host tests have run.
- Remaining risks / blockers: build/test pending. A future launcher client still needs bounded
  framing, concurrent pipe draining, timeout/cancellation/kill/reap, malformed/EOF mapping, and
  status handling that cannot report running for `ENGINE_NOT_IMPLEMENTED`.
- Next exact action: explicitly reconfigure the existing ARM64 sanitizer build; then run the host
  target and focused CTest. The source graph was rebuilt with its existing scopes preserved and
  `native`, `launcher`, and `tests` added; `trail build --check` passes. Record real build and test
  outputs.
- HANDOFF.md / TASKS / EVIDENCE / INSTALL_LOG / indexes updated: handoff and task status updated;
  no durable evidence or installation facts changed. Refresh `docs/FOLDERS.md` for this checkpoint.
