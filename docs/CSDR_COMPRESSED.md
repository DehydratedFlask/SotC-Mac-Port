# Compressed `.csdr` container (8c06 / 8c0a / cc**): structure stats, codec unknown

Status: **UNKNOWN codec**. This note records what a bounded probe measured and what it ruled out.
It does not claim a format, a decoder or a readable member share.

- Task: `GPU-01-MSL-OFFLINE-01` (compressed-container card).
- Prior state: E626 already records **19,320** full compressed-prefix members rejected corpus-wide by
  the structurally-first raw reader, with the container header described as opaque
  ([csdr reader checkpoint](checkpoints/2026-10-24-csdr-reader.md)).
- Probe: `tools/dev/csdr_compressed_probe.py` (self-test 8 checks, synthetic bytes only).

## Sample

Deterministic even spacing over the `(path, archive)`-sorted member list, 256 members across
`pak0`, `pak1`, `pakN`; **84** of them are `0x8c`/`0xcc`-prefixed `.csdr` (33 %). Of those:

| Prefix | Members | Size (min / median / max) | Payload entropy (min / median / max) |
|---|---:|---|---|
| `8c0a` | 74 | 69 / 21,428 / 4,189,544 B | 3.28 / 7.57 / 7.97 |
| `8c06` | 9 | 2,126 / 2,837 / 242,140 B | 7.77 / 7.79 / 7.95 |
| `cc0a` | 1 | 524,530 B | 7.85 |

Low-entropy outliers exist (`8c0a` down to 3.28): some members are mostly structured data, not
uniformly compressed.

## Header fields (first 16 bytes)

| Offset | Observation |
|---|---|
| `[0]` | `0x8c` (83/84) or `0xcc` (1/84) |
| `[1]` | `0x0a` (75/84) or `0x06` (9/84) — the only two second bytes seen |
| `[2:4]` big-endian | equals `floor(size / 256)` for **62/84** members; the rest are within one block of it. Consistent with a 256-byte block count. |
| `[4:8]` | varies per member; no correlation found with size, entropy or prefix |
| `[8:16]` | varies per member; no correlation found |

Example headers (hex): `8c0a001e18881216…`, `8c06000b6c880b6a…`, `8c06001425801423…`.
The `u16be[2:4] == size/256` relation is the only field rule that held up; treat it as a length hint,
not as a proven layout.

## Codec trials

For every member, five standard codecs were tried at offsets `0, 2, 4, 8, 12, 16, 20, 24, 32`:
zlib (window 15), raw deflate (-15), gzip (31), bzip2 and lzma — 45 attempts per member,
**3,780 attempts total, 0 successes** at any offset. No member starts with a recognised container
magic (zlib `78 01/9c/da`, lz4 frame `04 22 4d 18`, zstd `28 b5 2f fd`, xz, bzip2, lzma-alone,
snappy stream).

`lz4` and `zstd` libraries are **not installed** on this machine, so block-format lz4 and zstd
payloads are **NOT TESTED**, not "ruled out".

## What was ruled out

- **Raw OrbShdr inside the compressed members.** Two sampled members contain the bytes `ORB` at
  non-header offsets. Both were investigated: the surrounding bytes are high-entropy floats
  (`…c9 4b 42 db be 7a c3 b5 31 cf 3f 80 65 43 c3 7e 4f 52 42 26 ec 79…`) and
  `parse_raw_csdr` rejects every candidate span. The hits are coincidental, not embedded raw data.
- **A simple "skip N bytes then read raw" wrapper.** The raw reader rejects the payload from
  offsets 0, 16 and `ORB-16`.

## What this implies

1. The member is compressed or encrypted as a whole; the readable share cannot improve without a
   real decoder.
2. The `[2:4] == size/256` relation suggests a block count, so a block-structured codec is more
   likely than a single stream. That is a hypothesis, not evidence.
3. The authoritative next step is **not** more codec guessing: the game itself decompresses these
   members before use, so the decoder lives in the executable. The cheap way to find it is the
   `.csdr` loader path — the code that reads the 16-byte header, branches on the second byte, and
   calls the decompressor. That is a reverse-engineering card (`GPU-01-CSDR-DECOMPRESSOR-RE-01`),
   not a corpus-statistics card.

## Reproduction

```sh
python3 -I tools/dev/csdr_compressed_probe.py --self-test
python3 -I tools/dev/csdr_compressed_probe.py --sample 256 --out local/validation/csdr-compressed/s256
```

Private outputs (metadata only, no member bytes): `local/validation/csdr-compressed/s256/`
(`compressed_probe.json`, `SUMMARY.txt`).

## Limits

- Static container statistics only. No member was decompressed or executed; no GPU, G3 or game claim.
- Sample of 84 compressed members out of 19,320 corpus-wide; the field rules may not hold on the
  whole population.
- Header interpretation is descriptive; no field is proven by a decoder round-trip.
- `lz4`/`zstd` block formats untested on this host.
