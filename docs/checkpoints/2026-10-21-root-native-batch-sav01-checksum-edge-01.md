# Checkpoint: SAV-01-CHECKSUM-EDGE-01 — boundary tests for `core/save_checksum`

- Time / timezone: 2026-10-21 00:37 CDT (America/Chicago), macOS 27.0.1 arm64
- Agent and integrator: worker subagent `cpu01-native-batch` (deepseek-v4.1-flash-1017); integrator
  is the orchestrator session. `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/INDEX.md`,
  `ROADMAP.md`, root `README.md`, `launcher/`, `scripts/`, `.gitignore` were **not** edited here.
- Task / gate / status: SAV-01-CHECKSUM-EDGE-01. Status: **COMPLETE — FIXTURE_VERIFIED**, with one
  explicitly **NOT RUN** case (a span of `SIZE_MAX` bytes; see limitations). `core/save_checksum`
  semantics were **not** changed.
- Prerequisite evidence / input and source hashes: existing `native/core/save_checksum.{hpp,cpp}`
  (`fnv1a32(std::span<const std::byte>, std::uint32_t seed)`, a single pass with
  `hash = (hash ^ byte) * 0x01000193`) and the existing `native/tests/test_save_checksum.cpp`.
  No ELF or game data involved.
- Owned / changed files:
  - Added `native/tests/test_save_checksum_edges.cpp`.
  - Modified `native/CMakeLists.txt` (`test_save_checksum_edges` target + `save_checksum_edges`
    test) and `native/README.md` (test row).
  - `native/core/save_checksum.{hpp,cpp}` unchanged.
- Hypothesis and expected result: the existing checksum is a plain byte-at-a-time FNV-1a-32; its
  edges (empty span, single byte, seed sensitivity, a multi-chunk-sized span, and chunked
  composition) should all agree with an independent reference loop. Expected: all pass, no
  implementation change needed.
- Exact command, CWD and nonsecret environment: CWD `/Volumes/Extra/SoC Mac Native`
  - `cmake -S native -B local/build/native-agent-a -G Ninja` → exit 0
  - `cmake --build local/build/native-agent-a -j 8` → exit 0
  - `ctest --test-dir local/build/native-agent-a --output-on-failure` → **263/263 pass**
    (261 before A5/A6, +2 here)
  - `ctest --test-dir local/build/native-agent-a -R '^save_checksum_edges$' --output-on-failure`
    → 1/1 pass (included in the 263)
- Exit / observed result: all checks pass.
  - Empty span with the production seed `0x811c9dc5` returns the seed; an empty span at a valid
    non-null pointer returns the seed without dereferencing.
  - Single byte: `0x61` → `0xe40c292c` (published FNV-1a-32 vector for `"a"`); `0x00` differs from
    `0x61` and equals the reference loop.
  - Published multi-byte vector: `"foobar"` → `0xbf9cf968` (FNV reference vectors for 32-bit FNV-1a,
    offset basis `0x811c9dc5`; also checked through the in-test reference loop).
  - Seed sensitivity: same bytes under seeds `0x811c9dc5` and `0x12345678` give different results,
    and the second equals the reference loop with that seed.
  - Large span: 4 MiB (`1 << 22`) equals the independent byte-by-byte loop, and also equals the
    running-seed composition of consecutive 4093-byte sub-spans (so the implementation is
    chunking-transparent; it does not actually chunk).
  - Type bound asserted: `kLargeSpan < SIZE_MAX / 2` and `SIZE_MAX > large.size()`.
- Log, capture and REA evidence paths/IDs: temp logs
  `/private/var/folders/vx/5z8skhb97w32g817rv7hbh940000gn/T/opencode/build-a56.log`,
  `verify-final.log` (ephemeral). No REA session, no game binary touched.
- Observation versus inference / coverage limitations:
  - Observation: the implementation is a single pass with no index arithmetic, so the largest
    feasible span exercises the same path as a huge one; there is no separate chunk boundary.
  - **NOT RUN**: a span whose size is `std::numeric_limits<std::size_t>::max()`. Such a span is not
    representable over real memory in a test process, and calling `fnv1a32` on it would read
    unmapped memory. This is recorded as untested rather than asserted, and the test file says so
    in its header comment. The largest span the test actually allocates is 4 MiB.
  - Not verified: behaviour under a caller-provided span that aliases the destination of a future
    serializer (no such code exists yet).
- Cumulative failed tries and distinct hypotheses: 0 failed tries.
- Remaining risks / blockers: none for the checksum itself; the save format still has no serializer
  or loader, so G2 remains NOT RUN.
- Next exact action: integrator merges; no follow-up needed for this task.
- HANDOFF.md / TASKS / EVIDENCE / INSTALL_LOG / indexes updated: **NOT updated by this worker**.
  `scripts/project_index.py` and trail refresh not run.
