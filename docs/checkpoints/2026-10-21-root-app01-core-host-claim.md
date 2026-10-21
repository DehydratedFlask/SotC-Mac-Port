# Checkpoint: APP-01-CORE-HOST-01 — claim and initial host-protocol audit

- Time / timezone: 2026-10-21 08:38 CDT (America/Chicago).
- Agent and integrator: `/root` is integrator; read-only review by `/root/review_host_proto`.
- Task / gate / status: `APP-01-CORE-HOST-01`, ACTIVE, attempt 0. G4 remains NOT RUN.
- Prerequisite evidence / input and source hashes: `APP-01-CORE-ABI-01` provides the versioned C
  ABI; current HEAD at claim is `57a5282`. Existing untracked `native/api/host/` work wraps that ABI
  over a versioned stdio protocol. The host process still relays `ENGINE_NOT_IMPLEMENTED`.
- Owned / changed files: root claim changes `HANDOFF.md`, `docs/TASKS.md`, and this checkpoint;
  implementation owner is the host-protocol worker for `native/api/host/protocol.{cpp,hpp}`,
  `main.cpp`, and `tests/test_host_protocol.cpp`. CMake registration remains under root integration.
- Hypothesis and expected result: advertise the configured input limit in hello, distinguish input
  stream errors from clean EOF, require the documented exit status in the closed-stdout child test,
  and make the parent pipe writer survive early child closure. The focused host protocol tests should
  pass after these corrections.
- Exact command, CWD and nonsecret environment:
  - CWD `/Volumes/Extra/SoC Mac Native`.
  - `TZ=America/Chicago date '+%Y-%m-%d %H:%M:%S %Z'` → exit 0, `2026-10-21 08:38:52 CDT`.
  - Read-only audit of the current host diff; no tests were run and no implementation files changed.
- Exit / observed result: audit identified four concrete gaps: hello reports the default limit even
  for non-default options; `badbit` is indistinguishable from EOF; a closed-stdout test expects 2 but
  accepts 0; and the harness writer has default SIGPIPE behavior. No tests/builds have run yet.
- Log, capture and REA evidence paths/IDs: audit findings are in the root-agent message and this
  checkpoint; no run logs exist. No original binary or REA evidence used.
- Observation versus inference / coverage limitations: the source currently runs an ABI host over
  stdio and has pipe tests (observed by the worker). The defect descriptions are code-review findings
  and still need regression tests. This is fixture/transport preparation only, not a Swift child
  process client, native startup, game boot, or G4 proof.
- Cumulative failed tries and distinct hypotheses: 0 failed tries; the worker has not changed code.
- Remaining risks / blockers: no game engine implementation; `start` returns
  `SOTC_ERR_ENGINE_NOT_IMPLEMENTED`. A real launcher child-process client still needs bounded
  framing, concurrent pipe draining, timeout/cancellation/kill/reap, and conservative state mapping.
- Next exact action: the host worker fixes only the listed host protocol/test files, then returns the
  exact diff and proposed focused check. Root will run the native build/CTest sequentially with the
  constructor task to avoid sharing the build directory concurrently.
- HANDOFF.md / TASKS / EVIDENCE / INSTALL_LOG / indexes updated: `HANDOFF.md` and `docs/TASKS.md`
  now claim the task. No evidence or installation facts changed; regenerate/check folder indexes and
  record test results before handoff.
