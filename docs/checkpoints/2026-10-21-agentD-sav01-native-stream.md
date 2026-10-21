# Checkpoint: SAV-01-NATIVE-STREAM-01 — fixture-level tagged-chunk stream container

- Time / timezone: 2026-10-21 (America/Chicago), macOS 27.0.1 arm64, Apple M4 Pro
- Agent: worker "agent D" (Claude Sonnet 5.5); integrator is the orchestrator session.
  `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `INSTALL_LOG.md` were **not** edited; nothing committed or pushed.
- Task / status: SAV-01-NATIVE-STREAM-01. **COMPLETE — FIXTURE_VERIFIED.** No claim of game save
  compatibility; G2 stays NOT RUN, G5 NOT RUN.
- Inputs (facts used, all STATIC_ANALYSIS_VERIFIED in earlier checkpoints, none re-derived here):
  `2026-10-20-root-sav01-producer-complete.md` (16-byte outer header, body-only hash, load checks),
  `2026-10-20-root-sav01-serializer-complete.md` (12-byte chunk header, name hash),
  `2026-10-20-root-sav01-blob-flush-complete.md` (stream bytes flushed verbatim, no re-encoding),
  `native/core/save_checksum.*`, `native/core/save_chunk_header.*`. No ELF/game bytes touched, no REA.
- Files: added `native/core/save_stream.{hpp,cpp}`, `native/tests/test_save_stream.cpp`,
  `native/tests/test_save_stream_file.cpp`, this file. Modified (additive) `native/CMakeLists.txt`
  (one `sotc_core` source, two test targets/tests) and `native/README.md` (two rows).

## What is implemented
- `SaveStreamWriter(descriptor_id, DuplicatePolicy, max_body_size)`: `append_chunk(name, tag, payload)` /
  `append_chunk_hashed`, `serialize()`. Output = `{u32 1, u32 fnv1a32(body, 0x811c9dc5), u32 body_size,
  u32 descriptor_id}` (little-endian) + chunks. Limit is clamped to `0x6400000`; failed appends leave
  the writer unchanged.
- `parse_save_stream(bytes, policy)`: checks in order truncated_header, bad_version (dword0 != 1),
  size_too_large (dword2 > `0x6400000`), truncated_body, trailing_bytes, bad_hash, then chunk-level
  truncated_chunk_header / bad_chunk_magic / chunk_overrun, then optional reader_duplicate_name.
  Chunks are copied out as `{name_hash, tag, payload bytes}`; payloads are never interpreted.
- `write_file_atomic` (`<path>.tmp`, write loop, F_FULLFSYNC-then-fsync, rename, best-effort dir fsync,
  temp unlinked on any failure) and `read_save_stream_file` (fstat size gate before reading, ENOENT
  distinguished, directory -> io_read).

## PROVED (from checkpoints) vs fixture policy vs UNKNOWN
- Proved/used: outer header shape and dword meanings, version 1, size limit `0x6400000` unsigned,
  hash = FNV-1a-32 over body only with seed `0x811c9dc5`, dword1/dword3 not checked at header time,
  chunk header layout and name hash, stream flushed verbatim.
- Fixture policy (NOT claims about the original loader): trailing bytes rejected; chunks must tile the
  body; hash checked before chunk parsing; duplicate names allowed by default (real serializer has
  loops, repeats unknown) with explicit opt-in reject; `payload_length` = bytes after the chunk header.
- UNKNOWN, deliberately not modelled: tag meanings, chunk names, all payload/blob contents (game-data
  blob tag `0xd` with u64 prefix, 48 bool chunks, options blob), whether the u64 prefix is inside
  `payload_length`, outer-header endianness (little-endian is an inference from x86-64 `fwrite`),
  byte-swap flag, how the original loader treats trailing bytes, power-loss durability.

## Commands (CWD `/Volumes/Extra/SoC Mac Native`)
- `cmake -S native -B local/build/native-agent-d -G Ninja` -> exit 0 (ASan+UBSan ON, default option)
- `cmake --build local/build/native-agent-d -j 8` -> exit 0, no warnings (`-Wall -Wextra -Werror`)
- `ctest --test-dir local/build/native-agent-d -j 8 --output-on-failure` -> exit 0, **268/268 pass**
  (includes `save_stream` 8.4 s, dominated by one 100 MiB at-limit round trip, and `save_stream_file` 0.2 s)
- Logs: `local/build/native-agent-d-{configure,build,ctest}.log` (private, ephemeral).
- Mutation sanity (scratchpad copies, not in the tree): removing the hash check, size-limit check,
  version check, trailing-byte check, chunk-overrun check, or hashing the header instead of the body
  each made `test_save_stream` fail (6/6 killed).

## Test coverage (2 ctest entries, ~16 + 10 test functions)
Hand-written wire bytes; round trip + re-serialization byte equality; empty body; boundary payloads;
duplicate policy (writer and reader); wrong versions incl. every XOR of each version byte; every
XOR (1..255) of each hash/size byte rejected; descriptor-id flips accepted and carried (proved
behavior); every body byte x 255 flips -> bad_hash; truncation at every offset (also for the empty
stream and for files); trailing byte; oversize (limit+1, 0x7fffffff, 0xffffffff header-only; exactly
limit round trip; writer refuses limit+1; payload > u32 refused); chunk errors with a correct outer
hash; file atomic overwrite, stale temp, missing dir, rename failure cleanup, unwritable directory
preserves the original, corrupt/truncated/sparse-oversize files.

- Cumulative failed tries: 0. Remaining risks: all payload semantics unknown; power-loss durability
  not tested; unwritable-directory test is skipped when run as root.
- Next exact action: integrator merges; a real serializer needs decoded payload producers (profile
  chunk writers from `0x548d30`, bool-chunk reader `0xa81180`) before any schema is added.
