# Checkpoint: SAV-01-CHUNK-HEADER-TRAILING-FIELDS-FIXTURE-01 — claim

- Local date/time: 2026-10-21 12:20 CDT.
- Agent ID: `/root` (integrator).
- Task ID: `SAV-01-CHUNK-HEADER-TRAILING-FIELDS-FIXTURE-01`.
- Status: `ACTIVE`; failed tries: 0.
- Files changed at claim time: shared task/handoff documentation only; no test or production source change yet.
- Scope: in `native/tests/test_save_chunk_header.cpp`, extend the existing `parsed_long` check for a synthetic 20-byte span (12-byte header plus trailing payload) to assert `name_hash`, `tag`, and `payload_length` all equal the original `header` fields. No parser changes.
- Gap evidence: the live fixture currently checks successful parsing and payload length for `parsed_long`, while the parser decodes the three header fields from the first 12 bytes and ignores trailing bytes. The short 12-byte `parsed` roundtrip checks all fields, but the longer payload-bearing span does not.
- Prerequisites: `SAV-01-SERIALIZER-01` is `STATIC_ANALYSIS_VERIFIED` and `SAV-01-CHUNK-HEADER-01` is `FIXTURE_VERIFIED`; no matching trailing-fields task is registered.
- Target command: `ctest --test-dir local/build/native -R '^save_chunk_header$'`.
- Acceptance limits: synthetic header-codec behavior only; no serializer, real-save compatibility, or game save/load claim.
- Remaining risks: test has not been implemented or run yet.
- Next exact action: after the claim is published, assign only `native/tests/test_save_chunk_header.cpp` to `/root/slot23_raw_xrefs`; root will replay the focused CTest.
