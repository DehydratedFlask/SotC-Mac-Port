# Checkpoint 2026-11-01: the `.csdr` member layout behind `FUN_00b0cc50`

Worker task: read-only static analysis of the pinned `local/re/eboot.elf`
(sha256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`, image base 0, file offset =
VA + 0x4000 in all three image-mapping phdrs) against the real `.csdr` members. Nothing was loaded,
relocated or executed; no decompressor was written or run. Try count for this task: 0 failed tries.

Question: `FUN_00b0cc50` (624 B) is the `.csdr` loader callback; it is characterised but its byte-level
expansion is not. What exact on-disk member layout does the shipped code expect?

Gate labels: layout, parsers and tables `STATIC_ANALYSIS_VERIFIED`; per-member relations
`ARTIFACT_VERIFIED` (all 34,392 members); entropy-stream decode of any member `NOT RUN`.

## Headline

1. **`FUN_00b0cc50` does not expand anything and never reads member bytes.** It is the descriptor's
   candidate-file-name callback: a two-phase accessor that reads `shaderTypeToLoad`, the Neo flag
   (`0x3f4e0`), two static-cell helpers (`0x14c4a0`, `0x14c3a0`), builds the path with
   `FUN_00b0cb80` (201 B) and releases temporaries through `0xf590`. Its body references no archive
   buffer. The expansion happens in the generic resource path (`FUN_00147160` ->
   `bpe::Compression::Decompress` `FUN_001aa2a0` -> `OodleLZ_Decompress` `FUN_0089d3b0`), traced by the
   sibling checkpoint [csdr-1aa2a0-callers](2026-11-01-csdr-1aa2a0-callers.md).
2. **The compressed member is an Oodle LZ stream (Kraken or Mermaid) with a 12-byte RDSC trailer.** The
   layout below was derived from the ELF's own block/chunk/quantum parsers and then confirmed for
   **19,320 of 19,320** compressed members and **15,072 of 15,072** raw members.
3. **The codec is recognised, not decoded.** Header bytes `8c 06` / `8c 0a` are an Oodle V4 block header
   for decode type 6 (Kraken) / 10 (Mermaid); the next 6 bytes are the newLZ chunk and quantum headers;
   bytes `[8, 16)` are the first 8 output bytes stored raw. Bytes `[16, 5 + compLen)` are the LZ
   entropy/command streams, which this task does not decode.

## Layout (compressed form)

Member length `S` (as stored in the PSARC; the PSARC layer is already undone by `scripts/psarc.py`).
`compSize` as the ELF passes it to Oodle is `S - 12`: `FUN_0014b980` stores `request[0x80] =
request[0x78] - 12` and the trailer into `request+0xe8` (see the sibling checkpoint, section 3); the
decoder therefore sees bytes `[0, S - 12)` and never the trailer.

| Offset | Size | Field | Value in corpus | ELF evidence |
|---:|---:|---|---|---|
| 0 | 1 | block byte 0 | `0x8c` (19,320/19,320) | `FUN_008ac710`: `0x8ac719` load, `0x8ac721 cmp 0xc` low nibble must be `0xC`; `bextr 0x204` (bits 4-5) `| 4` must equal 4 (header type 4); `bextr 0x106` bit 6 -> `hdr[3]` ("memcpy" log text in `FUN_0089bcb0`), `shr 7` bit 7 -> `hdr[4]` ("reset"). `0x8c` = nibble `C`, type bits 0, memcpy 0, reset 1 |
| 1 | 1 | block byte 1 | `0x0a` Mermaid (16,328) / `0x06` Kraken (2,992) | `0x8ac753..0x8ac763`: bit 7 -> `hdr[5]` (checksum flag, 0), low 7 bits -> `hdr[1]` = decode type. Table `0x1295090` (u32 x 13 = `1,2,4,5,6,7,8,0,-1,-1,9,10,13`) maps type 6 -> compressor 8 (name ptr `0x15ced00+8*8` -> `"Kraken"` `0x1295036`) and type 10 -> 9 (`"Mermaid"` `0x129503d`) |
| 2 | 3 | chunk header, big endian | `compLen - 1`; bits 18..23 (`b2 >> 2`) are 0 in all 19,320 | `FUN_008ace10` (426 B): `0x8ace57..0x8ace75` builds `(b0 & 3) << 16 \| b1 << 8 \| b2`; `0x8ace77 cmp 0x3ffff` is the special-value escape (selector `b0 >> 2`); `0x8aceeb inc edx` -> length = field + 1; flags `b0 >> 2 & 1` / `& 2`; the optional 3-byte CRC is read only when `r13d` (checksums) is non-zero (`0x8acf08`), so with byte-1 bit 7 = 0 no CRC bytes exist. Reached through the selector `mov dl,0x51; shr dl,cl` (`0x89c252..0x89c26b`): types 6, 10 and 12 take `call 0x8ace10` (`0x89c2a0`), 7/8/9/11 take `call 0x8acb90` (`0x89c2a7`) |
| 5 | 3 | newLZ quantum header, big endian | `0x88xxxx`: bit 23 = 1 (compressed), bits 19..22 = mode 1, bits 0..18 = `compLen - 3` (19,320/19,320; the size equals `compLen - 3` because the 3-byte header itself is part of `compLen`) | `FUN_008c1e60` (12,474 B, source string `newlz.cpp`, `newLZ_decode_chunk_phase2`): `0x8c1ef0` dword load, `bswap` (`0x8c1ef6`), `js` on bit 31 (`0x8c1efa`) = compressed flag; `shr 8; and 0x7ffff` (`0x8c1f83`, `0x8c1f87`) = 19-bit size; `bextr 0x41b` (`0x8c1faa`) = bits 19..22 = mode; `cmp [rbp-0xec],2; jnc error` (`0x8c2055`, `0x8c205c`) rejects mode > 1, and the decompile dispatches mode 0 -> `0x8cbd30`, mode 1 -> `0x8cb610`; `"bad chunk_type"` otherwise. Per-quantum raw cap `0x20000`, per-chunk `0x40000` |
| 8 | 8 | first 8 output bytes, raw | little-endian `u32` pair, first dword in `{1, 2}` for all 19,320 (same domain as the first dword of the 15,072 raw members) | `0x8c20ac cmp qword [rbp-0x108],0` (output offset 0 only), `0x8c20b6 mov rcx,[r12+3]`, `0x8c20bb add r12,0xb` (source advances 3 + 8), `0x8c20c2 mov [rdx],rcx` store to output; the LZ phase later starts at `out + 8` for offset 0 (`+ bVar44 * 8` in the `0x8cbd30` / `0x8cb610` calls) |
| 16 | `compLen - 11` | LZ entropy and command streams | not decoded | `FUN_008cb610` / `FUN_008cbd30` and the entropy helpers; **out of scope** (needs an Oodle-equivalent decoder) |
| `5 + compLen` | 12 | RDSC trailer | `52 44 53 43 13 00 00 00` + `u32 LE` (19,320/19,320) | descriptor `FUN_00b0d3e0`: `movabs rax,0x1343534452` at `0xb0d420` (bytes `52 44 53 43 13 00 00 00`); loader compares the first 8 bytes (sibling checkpoint, section 3) |

Closed-form relations (all 19,320 compressed members): `S = compLen + 17`; chunk field `= S - 18`;
quantum size `= S - 20`; `compLen = S - 17 < rawLen <= 0x20000` (max observed rawLen 33,468, max observed
compLen 20,380, so every compressed member is exactly one chunk and one quantum).

Trailer word (`u32 LE` at `S - 4`): `tag << 29 | size`, `tag = word >> 29`. The consumer
(`FUN_001aa2a0`, sibling checkpoint section 2) passes `size` as the `rawLen` argument of
`OodleLZ_Decompress`, so for a compressed member `size` **is** the decoded length (`STATIC_ANALYSIS_VERIFIED`
for the call shape; the decoded length was never produced by running the decoder). Corpus pairing:
`8c0a` <-> tag 3 (16,328) and `8c06` <-> tag 4 (2,992), both exactly. Tag 3/4 both reach the same Oodle block
in the dispatch; the tag value is not what selects Kraken versus Mermaid (that is byte 1, decode type).

## Layout (raw form)

15,072 members begin with a `u32 LE` of 1 (8,178) or 2 (6,894), end with the RDSC trailer, and have tag 0 and
`size == S - 12` in all 15,072: the payload is the plain OrbShdr container and `S - 12` is its length. Per the
sibling checkpoint, tag 0 never reaches `FUN_001aa2a0` from the resource path (the loader hands the raw
buffer on), so these are consumed as stored.

Examples (first 16 bytes and the 12 trailer bytes; header bytes only, no payload kept):

| Form | S | first 16 bytes | trailer |
|---|---:|---|---|
| raw (tag 0) | 1,916 | `02000000 02000000 90020000 00000102` | `5244534313000000 70070000` |
| Mermaid | 8,707 | `8c0a 0021f1 8821ef 0200000002000000` | `5244534313000000 782d0060` |
| Kraken | 3,029 | `8c06 000bc3 880bc1 0200000002000000` | `5244534313000000 34110080` |

Check the Mermaid row: chunk `0x0021f1 = 8,689 = S - 18`, quantum size `0x21ef = 8,687 = S - 20`, trailer word
`0x60002d78` = tag 3, rawLen 11,640 > compLen 8,690.

## Is it Oodle? Evidence and its limit

* The ELF contains the Oodle 2 SDK statically (`v:\devel\projects\oodle2\core\...` assert strings,
  `rrlzh_lzhlw_shared.cpp`, `newlz.cpp`, `"Kraken"`, `"Mermaid"`, `"OODLE ERROR : ..."`), and exactly one call
  to `FUN_0089d3b0` (`OodleLZ_Decompress`), at `0x1aa426` (sibling checkpoint; E1075).
* The block-header parser `FUN_008ac710` accepts `0x8c 0x0a` and `0x8c 0x06` as type-4 headers with decode types 10
  and 6, the table maps them to Mermaid and Kraken, and `FUN_008ace10` / `FUN_008c1e60` consume the next six
  bytes with exactly the field widths the corpus has. Every one of 19,320 members satisfies all of those
  relations at once, with zero exceptions; these are exact arithmetic relations between three independent fields and the file length, not
  a pattern that a non-Oodle container would reproduce by chance.
* **Limit.** This establishes the container and the stream header, not the bit-exact semantics of the bytes
  after offset 16. "Oodle-compatible stream header recognised" is `STATIC_ANALYSIS_VERIFIED` +
  `ARTIFACT_VERIFIED`. "A member decodes to a valid OrbShdr" is `NOT RUN`.

## Corrections to earlier notes

* `docs/CSDR_COMPRESSED.md` ("codec UNKNOWN", `u16be[2:4]` = size/256 block count, `cc0a` row) is superseded:
  bytes `[2:5]` are the 3-byte chunk header (length - 1), not a block count (size/256 only looked right because
  the field is about size); the codec is Oodle Kraken/Mermaid; no `0xcc`-prefixed `.csdr` exists in any of
  `pak0`, `pak1`, `pakN`, `cachedFiles`, `pak2`, `pak3` or the patch `cachedfiles.psarc` (head-two-bytes census:
  base `8c0a` 16,328, `8c06` 2,992, `0100` 8,178, `0200` 6,894; patch `0100` 101), so the `cc0a` row does not
  reproduce. `pak2` / `pak3` contain no `.csdr` members at all. The doc was not edited by this worker.
* The descriptor tag constant is `0x1343534452` (bytes `52 44 53 43 13 00 00 00`).
  `docs/CSDR_LOADER_SEARCH.md` already carries the correction (line 174, section 4.2(e)). The private helper
  `local/re/csdr_trailer_probe.py` (lines 6, 24, 25) still has `0x0000000134534452` ("RDSC + version 1");
  it is not in the publish allowlist and was not edited. `tools/lift/check_csdr_loader_search.py` does not pin
  the wrong value.

## Observations versus inference

Observations: every pinned byte/call/lea/string/table in the checker; the 34,392-member census; all closed-form
relations above.

Inference (flagged): (a) that the LZ phase begins at offset 16 and the entropy sub-headers there follow the
standard newLZ layout (not parsed); (b) that mode 1 is the only quantum mode the shipped data uses because
the encoder always picks it (the ELF also accepts 0); (c) that Kraken (type 6) and Mermaid (type 10) share
`FUN_008c1e60` as their chunk decoder: only the shared quantum-header shape is pinned, the Kraken/Mermaid
call split is not (both codecs show identical container fields in the corpus); (d) that no code path
outside the traced `0x1aa426` call reads the trailer words (the sibling census shows a single reader).

## What blocks a byte-exact expansion

* Decoding bytes `[16, 5 + compLen)` needs the newLZ entropy/command-stream grammar (the 12.5 KB
  `FUN_008c1e60` plus `FUN_008cb610`, `FUN_008cbd30`, the Huffman/rANS helpers). That is a decoder
  implementation, which this task excluded; the native replacement is a separate task (a clean-room
  Kraken/Mermaid decoder validated against the ELF's behaviour, or offline decompression of the 19,320
  members at asset-import time).
* There is no way to prove `rawLen` bit-exact without running a decoder: the claim rests on the call shape
  (`rawLen = size`) and on the raw-member identity `size == S - 12`.
* Variant/hash resolution of which `.csdr` the callback asks for is covered by `CSDR_LOADER_SEARCH.md`; this
  task did not extend it.

## Reproduction

```sh
python3 -I tools/lift/check_csdr_b0cc50_container.py --require-private    # exit 0
python3 -I tools/lift/check_csdr_b0cc50_container.py --no-corpus          # exit 0 (ELF only)
python3 -I tools/lift/check_csdr_b0cc50_container.py                      # exit 0; SKIP lines if inputs absent
```

Verified outputs (private logs): `local/validation/csdr-b0cc50/checker-full.txt` (exit 0: 71 public probes, 47
byte pins, 5 calls, 4 leas, 8 strings, 6 body hashes, tables, 174 ELF mutants rejected, corpus 34,392 =
19,320 + 15,072) and `checker-negative.txt` (private inputs absent: exit 0 SKIP / exit 1 with
`--require-private`; corrupted ELF pin exit 1; corrupted corpus pin exit 1; relaxed profile exit 1 with
3 surviving probes named). The Ghidra dossiers (`dossier-oodle-hdr.txt`, `dossier-qh.txt`, `dossier-newlz.txt`,
`dossier-helpers.txt`) and the instruction listings (`insns1.txt`..`insns3.txt`) are in the same directory;
regenerate with `tools/re/ghidra/run_dump.py` (see `docs/GHIDRA_PIPELINE.md`).

Sample size: the whole `.csdr` population of the four base archives (34,392 members) was read in memory by
the checker and by the scan scripts; only header bytes (first 16) and trailers (last 12) were examined, and
nothing was written to disk. The three example rows above are the only member bytes recorded.
