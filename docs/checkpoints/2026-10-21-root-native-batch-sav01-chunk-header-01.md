# Checkpoint: SAV-01-CHUNK-HEADER-01 — native codec fixture for the 12-byte chunk header

- Time / timezone: 2026-10-21 00:37 CDT (America/Chicago), macOS 27.0.1 arm64
- Agent and integrator: worker subagent `cpu01-native-batch` (deepseek-v4.1-flash-1017); integrator
  is the orchestrator session. `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/INDEX.md`,
  `ROADMAP.md`, root `README.md`, `launcher/`, `scripts/`, `.gitignore` were **not** edited here.
- Task / gate / status: SAV-01-CHUNK-HEADER-01. Status: **COMPLETE — FIXTURE_VERIFIED** (codec +
  tests pass). **No real-save compatibility is claimed**: this is an ordinary-C++ codec for the
  header shape only. G2 (save/load) NOT RUN.
- Prerequisite evidence / input and source hashes: the header layout and name-hash rule come from
  [docs/checkpoints/2026-10-20-root-sav01-serializer-complete.md](2026-10-20-root-sav01-serializer-complete.md)
  (STATIC_ANALYSIS_VERIFIED): 12 header bytes — little-endian `u32 name_hash`, `u16 magic 0xBEEF`,
  `u16 tag`, `u32 payload_length`; name hash = FNV-1a-32 seed `0x811c9dc5` over the name after
  per-character `_Getptoupper` mapping. No ELF bytes are consumed at run time by this fixture.
- Owned / changed files:
  - Added `native/core/save_chunk_header.hpp`, `native/core/save_chunk_header.cpp`.
  - Added `native/tests/test_save_chunk_header.cpp`.
  - Modified `native/CMakeLists.txt` (`sotc_core` source + `test_save_chunk_header` target/test) and
    `native/README.md` (core row + test row).
- Hypothesis and expected result: a 12-byte little-endian codec with `parse` → `{error, header}`,
  `encode` → 12 bytes, and an FNV-1a-32 uppercase-mapped name hash reproduces the decoded wire
  layout exactly and rejects truncation and bad magic. Expected: exact 12-byte roundtrip, truncation
  rejected at every length 0..11, wrong magic rejected, boundary payload lengths preserved.
- Exact command, CWD and nonsecret environment: CWD `/Volumes/Extra/SoC Mac Native`
  - `cmake -S native -B local/build/native-agent-a -G Ninja` → exit 0
  - `cmake --build local/build/native-agent-a -j 8` → exit 0
  - `ctest --test-dir local/build/native-agent-a --output-on-failure` → **263/263 pass**
    (261 before A5/A6, +2 here)
  - `ctest --test-dir local/build/native-agent-a -R '^save_chunk_header$' --output-on-failure`
    → 1/1 pass (included in the 263)
- Exit / observed result: all checks pass.
  - Wire layout asserted byte-for-byte: `{name_hash=0x11223344, tag=0x5566, payload=0x778899aa}`
    encodes to `44 33 22 11 EF BE 66 55 AA 99 88 77`; magic bytes `EF BE` at offsets 4..5.
  - Roundtrip preserves `name_hash`, `tag`, `payload_length`; trailing payload bytes are ignored.
  - Truncation at every length 0..11 returns `ChunkParseError::truncated_header` with a zeroed
    header; truncation is reported before a magic mismatch when both apply.
  - Wrong magic (`0x0000`, `0xBEEE`, `0xEFBE`, `0x1234`) returns `ChunkParseError::bad_magic`.
  - Zero payload length accepted; `u32`/`u16` maxima preserved
    (`FF FF FF FF EF BE FF FF FF FF FF FF`).
  - Name hash: `""` → `0x811c9dc5` (seed unchanged); implementation equals an independent
    byte-at-a-time FNV loop over the ASCII-uppercased name for `foobar`/`FOOBAR`/`FoObAr`,
    `abc`/`ABC`, `level`, `ProgressData`, `x`, `player_name_00`; case variants are equal and
    `abc`≠`abd`; non-ASCII bytes (`C3 A9`) are hashed unchanged and equal the reference loop.
- Log, capture and REA evidence paths/IDs: temp logs
  `/private/var/folders/vx/5z8skhb97w32g817rv7hbh940000gn/T/opencode/build-a56.log` and
  `verify-final.log` (ephemeral). No REA session, no game binary touched.
- Observation versus inference / coverage limitations:
  - Observation (from the serializer checkpoint): header field order, magic value, the FNV-1a-32
    seed and the uppercase mapping step.
  - Explicitly **not modelled**: `_Getptoupper` locale behaviour outside ASCII (non-ASCII bytes are
    hashed unchanged and this is documented in the header); tag semantics (the tag is carried as an
    opaque `u16`, and no `bad_tag` error exists because the observed tag values are inference);
    payload contents/length validation beyond the header (the codec never reads a payload).
  - Not verified: that any real save file produced by the game matches this codec. No save file was
    read or written by this task.
- Cumulative failed tries and distinct hypotheses: 0 failed tries.
- Remaining risks / blockers: the codec is a fixture, not a serializer. A future serializer must
  decide how the back-patched `payload_length` is written and how the stream's byte-swap flag is
  handled (both noted as unknown in the serializer checkpoint).
- Next exact action: integrator merges; a follow-up task can build the chunk writer on top of
  `encode_save_chunk_header` once the payload producers and the stream flush slot are decoded.
- HANDOFF.md / TASKS / EVIDENCE / INSTALL_LOG / indexes updated: **NOT updated by this worker**.
  `scripts/project_index.py` and trail refresh not run.
