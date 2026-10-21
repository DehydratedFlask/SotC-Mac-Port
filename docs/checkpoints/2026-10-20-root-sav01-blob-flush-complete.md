# SAV-01-BLOB-FLUSH-01 — 2026-10-20, `/root`

`STATIC_ANALYSIS_VERIFIED`. Fourth function-level SAV-01 step: how the chunk stream becomes the saved bytes, and what the game-data blob producer is. Worker (Agent subagent) used the relocation table, stored records, the pinned ELF and 2 new REA 6.7.0 / Ghidra 12.1.4 `function` queries (`0x7e5bb0`: exit 0, 365.2 s, `ev_0b7dee3de5b8599bfb5491e6aaa349fd8f428a012fcb7a8221848c90acf7bcf8`; `0x4d420`: exit 0, 358.1 s, `ev_047da3a7d8b4871ed6a6b4a76d42ee611b40693e56e8fb368ded8ef46e943148`; pristine snapshot, no retry; both matched the offline decode). Root replayed `verify_blobflush.py` (exit 0; worker reports 127/127 verifier mutations caught) and `shasum -c` (0 bad lines over 38 files). Private dir `local/re/evidence/sav01-blob-flush-01/` (verifier `56e75e02…b31c`, `verification.json` `fe08e853…1e82`, `SHA256SUMS` `8526323f…c514`). Nothing is native; no save/load behavior is claimed.

## Flush mechanism (byte + relocation verified)
- The stream is a three-class chain: a base class with an abstract flush slot at `[vtbl+0x10]`, a vector-sink class (vtable `0x1595d98`) that implements it, and the chunk-stream class (vtable `0x15d2460`) that inherits it. Slot `[vtbl+0x10]` resolves to `0x4d420` in both tables.
- The Save request builds the stream on the stack with a `0x1000`-byte staging buffer. `0x4d5c0` is the primitive writer: it copies into the staging buffer, counts total bytes at `[+0x28]`, and calls the virtual flush when the buffer fills. `0x4d420` grows a byte vector at `[+0x30]` (via resize `0x4bca0`, zero-filling the new tail) and appends the staged bytes. `0x4d2c0` flushes staging and returns the vector start; `0x4d320` returns the total byte count. Chunk end calls `0x4d2c0` and back-patches the u32 length. Chunk-header writes carry no byte-swap.
- `FUN_00a56080` passes (data, size) to `FUN_00a55450`, which allocates a `{len, buf}` record, `memcpy`s the bytes verbatim, and stores it at `[obj+0x60]` with the size at `[obj+0x10]` — no re-encoding on flush.

## Blob facts
- `0x7e5bb0` writes nothing to the stream: it is a 14-byte accessor returning `.bss` address `0x3260bb8` and size `0x3cd8` (the game-data blob). `0x7ddb60` is the same for the options blob: `0x325f310`, size `0x9c` (39 int32 values).
- The game-data chunk is: tag `0xd`, a u64 length (byte-swapped only if the stream flag is set), then the raw bytes — 15596 bytes total. The progress-object serializer `0x543f40` does: copy blob, mode 1, `0x541ba0` (writes 48 one-byte bool chunks, 624 bytes, before game data), a snapshot overlay of three ranges (size/offset (100,0), (588,2500), (1,15388)), then the chunk. The loader at `0x5435f0` (observed) requires tag `0xd` and a stored length equal to `0x3cd8`.

## Unknown
- Game-data byte semantics and how the 48 flags map to blob bits; whether the flag-sync step changes the saved copy (the copy is taken before it runs); writers of the blob data; the unreferenced restore routine `0x543ef0`; the other class that shares `0x4d420`.

## Next bounded step
Decode the bool-chunk reader `0xa81180` and the remaining profile chunk writers (`0x548d30` onward), then the load-side deserializer. Not claimed: any native serializer, any save/load working. G0 PARTIAL, G1 NOT PASSED, G2-G6 NOT RUN.
