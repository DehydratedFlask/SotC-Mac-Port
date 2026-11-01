# Where is the `.csdr` loader? (bounded search of the pinned executable)

Task: `GPU-01-CSDR-DECOMPRESSOR-RE-01`. Question: does the pinned executable contain the code that
reads the 16-byte `.csdr` header, branches on `0x8c`/`0xcc` and `0x06`/`0x0a`, and decompresses the
member?

**Result: no such site was found in `eboot.elf`. The evidence points to the PS4 system GPU driver
as the consumer of shader blobs, not to the game executable. This is a negative result, not a
decoded format.**

## Search space (all negative)

Inputs: `local/re/eboot.elf` (SHA-256 `d0723afb…61d3`) and the saved disassembly
`local/validation/registry-analysis-agent/all.dis` (SHA-256 `9c196254…107e83`, 6,612,510 lines).

| Search | Hits | Interpretation |
|---|---|---|
| `cmpq $0x8c` | 3 | all three are loop bounds (`cmpq $0x8c, %rbx/%rcx/-0xe0(%rbp)` after `addq $0x1c`), not a file-magic compare |
| `cmpb $0x8c` / `cmpb $0xcc` | 0 | no byte-magic compare |
| `cmpl $0x8c0a` / `$0x8c06` / `$0xa8c` / `$0x68c` | 0 | the 16-bit prefix is never compared as an immediate, in either endianness |
| `cmpl $0x8c00` | 2 | inside shader-token/enum switches (`0x8c00`, `0x1400`, `0x8800`, `0x800`), not file bytes |
| `and $0xfc` | 1 | unrelated bit mask |
| immediate `0x42524f`, `0x4f5242`, `0x5362724f` (`"ORB"`, `"OrbS"`) | 0 | the executable contains **no `OrbShdr` magic constant at all** |
| string `csdr`, `OrbShdr`, `orb` | 0 relevant | `strings` returns only unrelated tokens (`5orB`, `Orbit ...`) |
| imports: `inflate`, `deflate`, `uncompress`, `lz4`, `zstd`, `lzma`, `crc32` | 0 | the executable imports **no decompression entry point** |

## What this means

1. **The executable does not parse `.csdr` itself.** It has no `OrbShdr` magic, no prefix compare
   and no decompression import. Raw `.csdr` members must therefore be handed to something else as
   opaque blobs — the natural candidate is `libSceGnmDriver` (`sceGnmSetPsShader350`,
   `sceGnmSetVsShader`, `sceGnmUpdatePsShader350`, …), which is a PS4 system module and is **not**
   part of this repository's artifacts.
2. **The compressed prefixes are consistent with a driver-side cache.** A GPU driver that accepts
   shader binaries can also accept a compressed form of the same binary; nothing in the game code
   needs to know the difference.
3. **This blocks the earlier plan** of finding a decompressor by reading the executable. Without
   the system module (which this project must not ship or require), the codec has to be recovered
   from the data itself or from published format knowledge, not from local code.

## Structural facts that still stand

From `docs/CSDR_COMPRESSED.md` (independent of this search):

- byte0 `0x8c` (83/84) or `0xcc`; byte1 only `0x0a` (75) or `0x06` (9);
- `u16be[2:4] == floor(size/256)` for 62/84 — a 256-byte block count;
- `u16be[6:8] == size - 20` for the single-blob members checked (16-byte header + 4);
- payload entropy 7.5–8.0 in most members, 3.28 in one — so not every member is uniformly compressed;
- no standard codec decodes any member; `lz4`/`zstd` block formats remain **NOT TESTED**.

## Reachability data points

1. **The numeric suffix is not a literal in the executable.** Five sampled compressed members'
   suffixes (`…_2421303609.csdr`) appear neither as little-endian 32-bit constants nor as ASCII
   digits anywhere in `eboot.elf`. Shader selection is computed (a name/path hash or a table
   lookup), so a file's presence in the archive cannot be read as "this file is loaded".
2. **Compressed members are not a dead duplicate class.** Over the 512-member coverage sample:
   - 264 compressed, 248 readable;
   - 77 of the 264 compressed members share a basename stem with a readable member
     (`depthfadeout`, `shadow`, `depthfadeoutinvlodfade`, …), but 121 stems appear **only**
     compressed;
   - splitting by build variant, compressed vs readable is **135 / 118** among `_shipping` members
     and **129 / 130** among non-shipping ones.

   If the compressed set were a stale duplicate of the raw set, it would not split ~50/50 inside
   both the shipping and non-shipping variants. Treat the compressed half as **live** until the
   loader proves otherwise, which makes the missing decompressor a real coverage loss rather than
   dead data.

## Next steps (bounded, in order)

1. **Confirm the driver hand-off.** Trace one GNM shader-registration call site in the executable
   and show that the pointer passed is the file buffer with no in-place transform. That converts
   the "driver consumes it" hypothesis into an observed call shape.
2. **Check whether compressed members are reachable at all.** If the engine only ever loads raw
   members by name/hash, the compressed set may be dead data from a different build, and the
   readable-share ceiling is not 52 % but 0 % lost. This is cheaper than decoding and should come
   first.
3. Only if (2) shows the compressed set is live: recover the format from data (block structure
   suggested by the 256-byte count) or from published material; do not guess a codec.

## Limits

- Static search of one executable and one saved disassembly. No runtime observation.
- Absence of a compare in `eboot` is strong evidence but does not prove the driver parses the
  format; it only proves the game does not branch on the prefix itself.
- The saved disassembly is objdump-derived; a decoder-derived sweep could in principle find a site
  this text search missed, though immediate constants would appear in both.
