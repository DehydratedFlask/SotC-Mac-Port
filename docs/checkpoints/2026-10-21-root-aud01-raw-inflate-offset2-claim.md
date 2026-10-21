# Checkpoint: AUD-01-CLASSIFIER-RAW-INFLATE-OFFSET2-FIXTURE-01 — claim

- Local date/time: 2026-10-21 12:31 CDT.
- Agent ID: `/root` (integrator).
- Task ID: `AUD-01-CLASSIFIER-RAW-INFLATE-OFFSET2-FIXTURE-01`.
- Status: `ACTIVE`; failed tries: 0.
- Files changed at claim time: shared task/handoff documentation only; no source or test edits.
- Scope: in `scripts/test_audio_headers.py`, add a synthetic 64-byte payload fixture compressed as
  raw DEFLATE (`wbits=-15`) and prefixed by two bytes; assert
  `decompress_probe()` reports `raw_inflate_offset2.ok` true and `out_len` 64. Do not change
  `scripts/classify_audio_headers.py`.
- Gap evidence: the live probe implements raw DEFLATE at offset 2 and the suite covers its
  trivial-output rejection plus successful raw offset 0 and successful zlib offset 2, but no
  successful raw offset-2 case is present.
- Prerequisite: `AUD-01-CLASSIFIER-RAW-INFLATE-OFFSET-00-FIXTURE-01` is `FIXTURE_VERIFIED`.
  No matching raw-offset-2 task is registered and the test file is clean and unassigned.
- Target command: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_audio_headers.py`.
- Acceptance limits: one synthetic classifier probe only; no real asset codec identity, asset
  decoding, playback, or game behavior claim.
- Remaining risks: no test has been implemented or run yet.
- Next exact action: after this claim is published, assign only `scripts/test_audio_headers.py` to
  `/root/review_host_proto`; root will replay the documented Python command.
