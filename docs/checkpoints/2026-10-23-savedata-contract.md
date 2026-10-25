# Checkpoint — SaveData transaction contract

- Local: 2026-10-23 (America/Chicago)
- Agent: analysis-and-design worker
- Task: `SAV-01-CONTRACT-01`
- Gate: **STATIC_ANALYSIS_VERIFIED**; not implemented

## First slice

One progress slot, `USR-DATA` only. The progress payload is a flat 15,576-byte buffer
(`0x3260bb8`, `0x3cd8`), not a field-by-field serializer, so the container can be byte-exact while
the blob stays opaque behind one hook.

- `save_load`: read `%s/USR-DATA` -> version 1 -> size <= 0x6400000 -> FNV(body) == dword1 ->
  descriptor 4 -> decode -> unmount.
- `save_store`: encode -> header `{1, hash, size, 4}` -> mount -> atomic write -> metadata/icon ->
  umount-with-backup.
- `save_check_backup`, `save_restore_backup`, `save_enumerate`.
- `SaveStore` + `SaveIoOps` (POSIX default) so partial-write/rename failures are deterministic.
- `ProgressV1 { game_data[0x3cd8], unlock[257], flags[48], words[5], booleans[3], floats[2], ids }`.
- `SaveResult` is the game's own 1-based ordinal table: 1 success, 2 canceled, 3 no-space,
  4 not-found, 5 mount, 6 corrupt, 7 generic.
- Reuses `save_stream`, `save_chunk_codec`, `save_checksum`, `save_chunk_header`.

## Corrections

`ESaveResult` **is** an ordinal table (1-based); `FUN_00a4a600` contains the `DialogOpen` callsite
`0xa4a903`, so it is the dialog-builder hook, not a leaf.

## Limits

No real save exists in the workspace; the blob semantics are unknown (round-trip only). The `0x530`
metadata, the progress writer's chunk order, the tag-4-vs-8 question, and the SaveIcon/Delete shapes
remain open. The original's unbounded BUSY retries and delete-before-write must not be copied.
**No save/load works claim.**
