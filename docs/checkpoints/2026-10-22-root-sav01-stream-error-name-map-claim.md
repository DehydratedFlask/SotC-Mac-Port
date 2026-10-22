# Checkpoint: SAV-01-NATIVE-STREAM-ERROR-NAME-MAP-FIXTURE-01 — claimed

- Time / timezone: 2026-10-22 02:41 CDT (America/Chicago)
- Agent and integrator: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`);
  implementation assigned to `/root/next_fixture_scout_2` after publication.
- Task / gate / status: `SAV-01-NATIVE-STREAM-ERROR-NAME-MAP-FIXTURE-01`, child of
  `SAV-01-NATIVE-STREAM-01`; `ACTIVE`, implementation not started, zero failed tries.
- Prerequisite: parent behavior and explicit unknowns are documented in
  [SAV-01-NATIVE-STREAM-01](2026-10-21-agentD-sav01-native-stream.md). This fixture pins only the
  diagnostic-name mapper and cannot establish real save compatibility.
- Baseline hashes: `native/tests/test_save_stream.cpp` SHA256
  `412615c0eb82feaa4c075869663a1ea980aaaf98e0533e8e274accefb3984189`; unchanged
  `native/core/save_stream.cpp`, `native/core/save_stream.hpp`, and `native/CMakeLists.txt` SHA256
  `7bc9763539dfa6fdd40f26d3cd9c1dd4c0d06e50bbd3af87d0c7b1f2cb1c0c7e`,
  `b2a0f296b850f094151e4d53ef7cf08f62268b3358cd9aa1fdfb41244a7dbe83`, and
  `cd46d94590f58515152155658f609cd11ef58aeb9a45272f744402edcdeeb01f`.
- Owned / changed file: only `native/tests/test_save_stream.cpp`, inside `test_error_names()`. Keep
  existing checks for `bad_hash` and `none`; add exact expected strings for the other 19
  `SaveStreamError` enum values and assert an out-of-range enum returns `unknown`. Do not change
  production or build files.
- Hypothesis: each declared `SaveStreamError` maps to its matching enum spelling, while values not
  in the switch map to `unknown`.
- Exact baseline commands, CWD `/Volumes/Extra/SoC Mac Native`:
  `cmake --build local/build/native --target test_save_stream` -> exit 0;
  `ctest --test-dir local/build/native --output-on-failure -R '^save_stream$'` -> exit 0, 1/1.
  Root baseline log SHA256 values are `23deaffbeeb4acf4a71198d95c9d2a4416cda166ba68c397a0b6458615776bae` /
  `91520118d36d217bf9ca2580a68eb6b404767bd62aeca0662879437623766f06` under
  `local/validation/20261022-root-fixture-baselines/`.
- Evidence reviewed: Trail located `test_error_names()` at `native/tests/test_save_stream.cpp:L364-L367`
  and `SaveStreamError` at `native/core/save_stream.hpp:L47-L73`. Live source maps all 21 enum values
  and returns `unknown` after the switch at `native/core/save_stream.cpp:L37-L61`. The task ID was
  absent from the task/evidence/handoff registers at claim time.
- Observation versus inference / limits: baseline passes and the current mapper has explicit string
  cases for all enum values plus a fallback. The new test will pin this mapping only; it does not
  test stream parsing, file I/O, save compatibility, or game progress.
- Failed tries: zero. No implementation has started.
- Remaining risks / blockers: no blocker to the single-file assertion fixture; payload semantics and
  real-save compatibility remain unknown as documented by the parent task.
- Next exact action: publish this claim, then send `/root/next_fixture_scout_2` only the
  `native/tests/test_save_stream.cpp` test-file edit and focused build/CTest request.
- HANDOFF / TASKS / EVIDENCE updated; `INSTALL_LOG.md` has no change. `python3 scripts/project_index.py`
  and `--check` pass at 29,507 folders. Trail was rebuilt with explicit `scripts`, `tools`,
  `reference`, `native`, `launcher`, and `tests` scopes; `trail build --check` passes at 3,618 nodes /
  1,240 files. Index and Trail log hashes are in the region-split completion checkpoint. No install
  occurred.
