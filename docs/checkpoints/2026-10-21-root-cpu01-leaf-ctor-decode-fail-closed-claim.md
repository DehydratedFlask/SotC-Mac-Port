# Checkpoint: CPU-01-LEAF-CTOR-DECODER-FAIL-CLOSED-FIXTURE-01 — claim

- Local date/time: 2026-10-21 12:23 CDT.
- Agent ID: `/root` (integrator).
- Task ID: `CPU-01-LEAF-CTOR-DECODER-FAIL-CLOSED-FIXTURE-01`.
- Status: `ACTIVE`; failed tries: 0.
- Files changed at claim time: shared task/handoff documentation only; the decoder and test
  source remain unchanged.
- Scope: add `tools/lift/test_leaf_ctor_decode.py` with one minimal image-like fixture whose
  `read()` returns 16 synthetic NOP bytes. Call `leaf_ctor_decode.decode(image, 0x4000)` and
  assert it raises `leaf_ctor_decode.DecodeError` with a message beginning
  `unrecognised encoding at 0x4000`. Do not change the decoder.
- Gap evidence: the decoder accepts `image, rva`, consumes bytes from `image.read`, and its
  documented strict fallback raises `DecodeError` for every unrecognised encoding. No matching
  task is registered and the new test path is unassigned.
- Prerequisite: `CPU-01-LEAF-CTOR-BATCH-34` is `FIXTURE_VERIFIED`.
- Target command: `PYTHONDONTWRITEBYTECODE=1 python3 -I tools/lift/test_leaf_ctor_decode.py`.
- Acceptance limits: one fail-closed synthetic opcode case only; no general decoder correctness,
  lifted-code execution, native runtime, or game behavior claim.
- Remaining risks: no test has been implemented or run yet. The new file must be included in
  project indexing and the source graph check.
- Next exact action: after this claim is published, assign only the new test file to
  `/root/review_host_proto`; root will replay the documented Python command.
