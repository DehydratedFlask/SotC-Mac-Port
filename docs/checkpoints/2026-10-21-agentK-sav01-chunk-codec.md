# Checkpoint: SAV-01-CHUNK-CODEC-01 — typed chunk codec, spec tables, descriptor gate (fixture level)

- Time / timezone: 2026-10-21 (America/Chicago), macOS 27.0.1 arm64
- Agent: worker "agent K" (Claude Sonnet 5.5); integrator is the orchestrator session.
  `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `INSTALL_LOG.md` were **not** edited; nothing committed or pushed;
  no Ghidra/REA; no ELF or game byte read. `startup_ctors.*` and `check_ctor_slots.py` untouched.
- Task / status: SAV-01-CHUNK-CODEC-01. **COMPLETE — FIXTURE_VERIFIED.** No claim of real-save compatibility; G2 stays
  NOT RUN. Cumulative failed tries: 0 (one test bug found and fixed on the first run: a span returned from
  `decode_blob_chunk` aliasing a destroyed temporary; ASan caught it, library unchanged).
- Facts used (all from `2026-10-21-agentH-sav01-profile-chunks.md`, not re-derived here): little-endian only; bool chunk =
  tag 13 + 1 byte; blob chunk = tag 13 + u64 prefix **inside** `payload_length`; varint string (tag 12); f32 (tag 10);
  4-byte values (tags 8 and 4); first-match lookup by name hash, retagged/missing chunk keeps the default; empty body and
  overlong record are original UB; header dword 3 = class descriptor id (profile `0xd`, progress `4`) checked by class
  loaders; 33 profile + 13 progress + 48 flag rows, 257 indexed hashes, enum value sets E1..E7.
  Two spot cross-checks done with a standalone script (no game data): the 257 indexed hashes reproduce agentH's
  first/second/last values and SHA-256 digest `9fa2d6ad...e1d9`; the 33+257+48 hashes are pairwise distinct (338 unique).

## Files
- Added `native/core/save_chunk_codec.{hpp,cpp}`, `native/tests/test_save_chunk_codec.cpp`, this checkpoint.
- Modified (additive, re-read just before editing): `native/CMakeLists.txt` (one `sotc_core` source line after
  `core/save_stream.cpp`; one test target/test right after `save_stream_file`),
  `native/README.md` (two rows).

## What is implemented
- `PayloadReader` / `PayloadWriter`: bounded LE `u8/u32/u64/f32`, 7-bit varint (<= 5 bytes, rejects values over u32,
  accepts non-canonical zero padding), `read_bytes` borrow. Every read checks `remaining()` first; no read past the payload.
- Encoders/decoders per kind: bool, word32 tag 8, word32 tag 4 (separate function), f32 (NaN bits preserved), varint string,
  decimal enum text with value-set membership, u64-prefixed blob (optional required length), u64-prefixed u32 array
  (prefix must equal 4N). `ReadMode::strict` (default) requires the payload to be consumed exactly; `ignore_trailing`
  mirrors the original's sub-stream read for original-save import.
- `lookup_chunk` (first match; `found` / `missing` / `tag_mismatch`; a retagged first match is not retried against a later
  duplicate, like the original). `ParsedSaveStream::find_first` from the container is reused.
- `constexpr` tables (accessors return spans): `profile_named_chunk_specs()` (33), `progress_named_chunk_specs()` (13),
  `flag_specs()` (48, hash + flag index), `indexed_chunk_hash/spec(i)` (257, `static_assert`ed against agentH's h_0/h_1/h_256).
  `PayloadKind` = Bool8, Word32 (tag 8), **Word32Tag4Unresolved** (tag 4), F32, Str, EnumStrDecimal, BlobU64, ArrayU32U64.
- `append_chunk_by_spec` / `decode_chunk_by_spec` / `read_chunk_by_spec` (`ChunkValue` variant); wrong alternative, enum
  value outside its set, fixed-length blob mismatch are errors and leave the writer unchanged.
- Flag-bit helpers `get_flag_bit` / `set_flag_bit` (out of range refused, never wrapped).
- Loader gate, separate from the container: `check_descriptor_id(header, expected)` and `validate_for_loader(bytes, expected)`
  — container checks first, then descriptor (the original checks it before reading any chunk), then `empty_body`.
  Chunk overrun / truncated header / bad magic / bad hash are the container's errors surfaced as `container_rejected`.
- `NameHashPolicy`: `ascii_upper` (default, == `save_chunk_name_hash`), `ascii_upper_and_slash` (adds the verified
  backslash-to-slash step of the original hash loop, which `save_chunk_name_hash` does not model), `verbatim`.

## Proved vs fixture policy vs unknown
- **Used as proved (from agentH):** layouts above; descriptor ids; tables; value sets; first-match; hash incrementality.
- **Fixture policy (not claims):** strict exact-consumption of payloads; non-canonical varints accepted; decimal text only
  (digits, any length with value <= 0xFFFFFFFF, leading zeros ok, no sign/space — original `_Stoul` is more permissive);
  unknown enum text is an error, not "first table value"; strict container tiling/no trailing bytes; "import mode"
  (tail leniency, stop at first non-`0xBEEF`) not implemented; progress table is tag/payload class only.
- **UNRESOLVED / inference, not modelled:** tag 4 vs tag 8 (kept as two kinds; a chunk with the other tag is rejected by
  the spec reader; signedness left to callers); name-hash case folding = ASCII toupper is **inference** (policy + test,
  no claim); flag-bit mapping is OBSERVATION-READING; the 48 flag chunks belong to the progress writer in the original, the
  "profile-like" test body combines them with the profile rows synthetically; original enum name tables not embedded (user
  decision pending); semantics of every field.

## Commands (CWD `/Volumes/Extra/SoC Mac Native`)
- `cmake -S native -B local/build/native-agent-k -G Ninja` -> exit 0 (ASan+UBSan on)
- `cmake --build local/build/native-agent-k -j 8` -> exit 0, no warnings (`-Wall -Wextra -Werror`)
- `ctest --test-dir local/build/native-agent-k -j 8 --output-on-failure` -> exit 0, **294/294 pass**, incl. `save_chunk_codec`
  (1 ctest, 13 test functions, ~276 CHECKs) and the existing `save_stream*` / `save_chunk_header` / `save_checksum*`.
  (One earlier full run showed `clothing_physical_mesh_parameters` failing while another agent was mid-edit; the final run passes.)
- Logs: `local/build/native-agent-k-{configure,build,ctest}.log` (private, ephemeral).
- Mutation sanity (scratchpad copies of the codec, compiled with ASan/UBSan against the unchanged test): 15/15 killed —
  bool `!=0`, tag-8 reader using tag 4, descriptor check removed, empty-body check removed, varint 5th-byte guard removed,
  blob size guard removed, retag-continues lookup, strict trailing check removed, decimal sign accepted, E1 set widened,
  array prefix==4N check removed, flag bit order reversed, fixed blob length ignored, backslash fold removed, varint
  continuation bit dropped.

## Test coverage (all synthetic)
Hand-written wire bytes for every primitive and for whole chunks inside the container (bool, u32, string, enum, f32, blob
with `payload_length = 8 + size`, 156-byte param blob = `0xa4`, game-data record = 15596 bytes); varint 0/127/128/300/16383/16384/
2^21/2^28/u32 max, 6-byte and 5th-byte-overflow rejection; every strict payload prefix rejected for every decoder, plus trailing byte
(strict vs ignore); tag mismatch for every decoder; tag 4 vs 8 separation and exactly one unresolved row per table;
decimal-only text (empty, letters, sign, space, overflow, max u32); value-set membership for E1..E7; spec counts (33/13/48/257),
per-kind counts (7/3+1/8/1/13), anchors, tag-vs-kind consistency, 351 (33+13+48+257) pairwise-distinct hashes, independent FNV fold of the
indexed hashes plus a Python-computed digest `0x11ef85a7`; flag bit mapping; first-match duplicates, retag not retried, writer
reject policy; failed appends leave the writer unchanged, writer size limit; synthetic full body (33 named + 257 indexed + 48 bool
chunks + a 0x3cd8-byte game-data-shaped blob whose bit i mirrors flag i) with Python-computed goldens (body 20747 bytes, body hash
`0xea9615e8`, whole image FNV `0xd29e50bd`), per-chunk decode, byte-identical re-serialisation from decoded values, truncation at every
offset, one trailing byte, dword-3 bit flips (container accepts, gate rejects); empty body, overrun by one in first/second chunk,
declared length `0xffffffff`, short tail, bad magic, overrun with wrong hash (bad_hash first), descriptor-before-empty-body ordering.

## Remaining risks / next exact action
- No real save was read; every format claim rests on agentH's static analysis. G2 stays NOT RUN.
- `Decoded<span>` from `decode_blob_chunk` aliases the chunk; callers must keep the chunk alive (documented).
- Next: user decision on enum-name import (data tables vs decimal-only); optional "import mode" tail leniency for original
  saves; a typed `ProfileV1`/`ProgressV1` struct once field semantics are decoded; real-save golden vectors only after a legitimately
  owned local save is available (never committed).

## Recommended TASKS.md row
`| SAV-01-CHUNK-CODEC-01 | Typed chunk readers/writers, constexpr spec tables (33 profile, 13 progress, 48 flags, 257 indexed), loader-level descriptor gate | SAV-01-NATIVE-STREAM-01, SAV-01-PROFILE-CHUNKS-01 | FIXTURE_VERIFIED (294/294 ctest, 15/15 mutations killed; tag 4/8 and name-hash case folding stay UNRESOLVED/inference; no real-save claim; see docs/checkpoints/2026-10-21-agentK-sav01-chunk-codec.md) / agentK | 0 |`
