# Audio container and codec status (AUD-01-AUDIO-FORMAT-01)

Status: **SXD/SShd containers now specified from bytes and code; the streamed music codec is
UNKNOWN.** This note proves the container grammar of the sound banks and stream descriptors,
proves that the `.hd`/`.bd` sound-effect banks are PSX/PS2 SPU ADPCM, and records what the evidence
does **not** establish about `sound/shadow/*.sxd2` (the music/ambience/event stream payload).
No decoded game audio is claimed. No member bytes were extracted to disk.

Labels used: `ARTIFACT_VERIFIED` (pinned bytes measured here), `STATIC_ANALYSIS_VERIFIED` (pinned
ELF code), `FIXTURE_VERIFIED` (synthetic), `PARTIAL`, `UNKNOWN`, `NOT RUN`.

## Scope and method

- All members were resolved through the corrected MD5(path) PSARC mapping with
  `scripts/psarc.py` and decoded **in memory only**; no member payload was written into the
  repository. The FFmpeg cross-check used a synthetic `VAGp` wrapper plus one 32 KiB
  window in a private, git-ignored directory; both files were deleted after the run, and no
  decoded waveform was stored.
- Container fields were checked against the pinned plaintext ELF, not guessed. The executable
  contains a real `SXDF`/`TONE`/`NAME` parser and an `SShd` bank registrar; both are cited by
  virtual address below (ELF SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`).
- The SPU ADPCM decode was cross-checked against an independent implementation (FFmpeg
  `adpcm_psx`, see below). Codec trials follow the [`CSDR_COMPRESSED.md`](CSDR_COMPRESSED.md)
  style: exact attempt counts, exact results, no "ruled out" beyond what was actually run.
- Archive size was re-checked as 12,838,532,222 bytes. The pakN SHA-256
  (`559f7a3fa66bc652d36c5f1a885b351e4aeb17dadbba06a0006520bfd325be73`) is the E1009 pin and was
  **not** re-streamed in this task; member hashes below were recomputed here and match.

## Members examined (`ARTIFACT_VERIFIED`)

| Member | Archive / TOC | Bytes | Decoded SHA-256 |
|---|---|---:|---|
| `sound/shadow/music.sxd1` | pakN / 27715 | 9,872 | `b925ca03cb6dddf8e9d3448c24a2e2eeb0ac595067cd3bbf25087ec3e6692a94` |
| `sound/shadow/music.sxd2` | pakN / 37843 | 172,367,136 | `3ffb2b86dba5dfadb7052247b5a7393b499d2b4a8d9b87c476a49379586c7881` |
| `sound/shadow/str_bgm.sxd1` | pakN / 61642 | 2,461,232 | `5b7aabb76442a57368ecff9b539c63e04c00ff7539af73cb3d52936a53d468c3` |
| `sound/shadow/str_bgm.sxd2` | pakN / 17150 | 514,125,344 | `51989ccf144405898714ca9fec200a7a0cfeb1dd7811ec7b90d2ed595da451e8` |
| `sound/shadow/bridge.sxd` | cachedFiles / 2581 | 144 | `9c288d5c8f9836ca8bc44676db159212b4b48e6fdf229e9d388cc7d3cc90a75e` |
| `cp3shadow/sound/se/bridge.hd` | pak3 / 7880 | 2,026 | `a95728f569f78dfcd2e4857630353e89e6a3815fe7de906311d0bbadfea7a818` |
| `cp3shadow/sound/se/bridge.bd` | pak3 / 6864 | 715,872 | `9dc1efa9c723d88a4ae8f0b9f9aaec4f392a9e5436a31d5e41c837ee6399d4b1` |
| `cp3shadow/sound/se/com_sys.hd` | pak2 | 1,916 | `d90ec91455b26c16a877f8324919f13fb4d0365ee1c892d07712ea0dad68887c` |
| `cp3shadow/sound/se/com_sys.bd` | pak2 | 112,768 | `cc3af4a54cbd52d28522f3745a1538d469507226b33b65228ac9c37db1537ec3` |
| `cp3shadow/sound/test_adpcm/b_info_01.sts_cp3` | cachedFiles / 13300 | 120 | `0c3a40c38366dacda236c4f22c82434cc591ce3c1109b6ea6c60bdb13b607687` |
| `cp3shadow/sound/test_adpcm/b_ev_02.sts_cp3` | cachedFiles / 28733 | 120 | `64c8f3e9cf34ceb2689ec294624689c5c216af1a4fda84c66ba464f3e51f93dc` |

Corpus context for the same families (metadata only, from the corrected 86-row audio manifest):
5 `.sxd1` + 5 `.sxd2` pairs; 26 `.sxd`; 25 `.hd` + 25 `.bd` pairs (25,921,328 body bytes,
1,620,083 ADPCM frames); 290 `.sts_cp3` (120 B each).

## PROVEN — `SXDF` container (`*.sxd`, `*.sxd1`)

`ARTIFACT_VERIFIED` + `STATIC_ANALYSIS_VERIFIED`.

The loader reads a 96-byte header (`0x98597f`: `mov edx,0x60`), tests the first dword
(`0x98598c`: `cmp dword [rbp-0x90],0x46445853` = `SXDF`), uses the u32 at header offset `0x08` as
the file size (allocated at `0x9859a4`, stored at `0x985c3d`), copies the 64-byte name from `0x20`
(`0x985b99`) and the 16-byte id from `0x10` (`0x985bba`).

| Offset | Type | Meaning (proven by use or by value) |
|---:|---|---|
| `0x00` | 4 s | `SXDF` |
| `0x04` | u16le | 4 in every observed file |
| `0x06` | u16le | varies per file; purpose unknown |
| `0x08` | u32le | this file's byte size (equals the file length in all observed files) |
| `0x0C` | u32le | companion `.sxd2` byte size, or 0 when there is no companion |
| `0x10` | 16 B | id shared with the companion `.sxd2` (same 16 bytes in both files) |
| `0x20` | 64 B | NUL-padded base name (`music`, `str_bgm`, `bridge`, …) |

Header size check: `music.sxd1` 9,872 = 0x2690 at `0x08`; `0x0C` = 172,367,136 = exact
`music.sxd2` length. The same `self`/`companion` relation holds for all five `.sxd1` files
(`str_amb` 15,614,576 / 375,729,632; `str_bgm` 2,461,232 / 514,125,344; `str_boss` 48,928 /
286,412,736; `str_event` 57,248 / 366,325,072) and for `bridge.sxd` (144 / 0).

### Chunk grammar

From `0x60` the file is a list of `{char tag[4]; u32le payload_size; payload}`. The loader scans
`next = pos + 8 + payload_size` (`0x985c60`–`0x985c76`) and compares tags `TONE` (`0x985c65`) and
`NAME` (`0x985d15`). Observed chunk sets:

| File | Chunks in order |
|---|---|
| `bridge.sxd` | `TONE` |
| `music.sxd1` | `TONE REQD NAME WAVE SEQD LVAR LVRN RXFD RXFN` |
| `str_bgm.sxd1` | `TONE REQD NAME WAVE SEQD LVAR LVRN RXFD RXFN DATA` |
| `str_amb.sxd1` | `… WAVE SEQD LVAR LVRN RXFD RXFN TBLD DATA` |
| `str_event.sxd1` | `… WAVE SEQD EXTR LVAR LVRN RXFD RXFN` |

`NAME` payload is a string table (`0x9b0` in `music.sxd1`). `TONE`, `SEQD`, `LVAR`, `LVRN`,
`RXFD`/`RXFN` payloads are timing/level/randomisation tables; their record layouts were not
decoded and are not claimed.

### `WAVE` records declare the stream segments

`ARTIFACT_VERIFIED` for `music.sxd1` (full tiling), `PARTIAL` for the four other `.sxd1` files.

`WAVE` header: `u32le count` at `+0x0C` (33 in `music.sxd1`, 133 in `str_bgm.sxd1`), `u32le`
`table_bytes` at `+0x10` (= 4 × count in both files checked), then a `count`-entry u32 table, then
the wave records. The table values are **not** record offsets: in `music.sxd1` they advance by 72
while the records advance by 76, so their meaning is `UNKNOWN`. Records are located by their
`(u32 flags, u32 sample_rate)` prefix; in `music.sxd1` they are 76 bytes apart (the last one 4
bytes shorter, ending exactly at the next chunk).

Each record begins with an 8-byte `(u32 flags, u32 sample_rate)` pair. Every record in all five
`.sxd1` files carries **48000** at `+0x04`; the count of `48000` occurrences equals the declared
`WAVE` count for `music`, `str_amb`, `str_bgm` and `str_boss`, with one extra (non-record) hit in
`str_event`.

| Record field | Observed meaning |
|---|---|
| `+0x00` u32 | type/flags, seen as `0x00ff0642`, `0x00ff0242`, `0x00ff0121`, `0x00ff0221`, `0x00ff0142`, `0x00ff0442`; not decoded |
| `+0x04` u32 | sample rate = 48000 in every record |
| `+0x10` u32 | per-record value (hash-like); not decoded |
| `+0x14`,`+0x18` s32 | `-1` in every `music` record (loop fields, unproven) |
| `+0x1C` u32 | segment length in **bytes** |
| `+0x20` u32 | segment start offset in the companion `.sxd2` |

`music.sxd1` tiling, checked exactly: first `(offset, length) = (0x60, 0x9BEB00)`, next
`(0x9BEB60, 0x4DDF00)`, and so on for all 33 records; every record starts where the previous one
ends, and the last end is **172,367,136** = the exact `music.sxd2` length. That is a byte-exact
proof that the `.sxd2` file is the concatenated payload these records describe.

## PROVEN — `SXDS` container (`*.sxd2`)

`ARTIFACT_VERIFIED` for header/geometry; payload codec **UNKNOWN**.

| Offset | Type | Observation |
|---:|---|---|
| `0x00` | 4 s | `SXDS` |
| `0x04` | u16le | 4 in every observed file |
| `0x06` | u16le | 0 in every observed file |
| `0x08` | u32le | 33 / 125 / 127 / 172 / 53 for music / amb / bgm / boss / event; **not** the `WAVE` count (33/414/133/194/56) — meaning unknown |
| `0x0C` | u32le | payload size = file length − 96, true for **5/5** |
| `0x10` | 16 B | same id as the companion `.sxd1` |
| `0x20` | 64 B | NUL-padded base name |

Payload geometry (`music.sxd2`): 172,367,040 bytes = 960 × 179,549 exactly. The payload is a
sequence of 960-byte records; the first ~6 bytes of each record have low entropy (byte 0 ≈ 1.0 bit,
byte 1 ≈ 1.2 bits, bytes 2–5 2.2–5.5 bits) and byte 6 onward ≈ 7.6–7.9 bits. The first two bytes of
the first records are `0x32 0x93` and drift slowly (`0x33 0x13`, …) deeper in the file. The same
960-byte stride with a low-entropy prefix appears in `str_amb`, `str_bgm` and `str_boss`;
`str_event` does not follow it. Remainders modulo 960 differ per file (0, 128, 480, 496, 896), so
960 is an observed record stride, not a proven container rule.

**No `SXDS` literal exists in the pinned executable** (case-sensitive search: 0 hits for the
bytes `SXDS`, which also covers the immediate `0x53445853`), so no simple magic check is visible.

## PROVEN — `SShd` + body sample banks (`*.hd` + `*.bd`) are SPU ADPCM

`ARTIFACT_VERIFIED` + `STATIC_ANALYSIS_VERIFIED` + independent decoder cross-check.

- `.hd` layout, verified on **25/25** pairs: `u32le file_size` at `0`, `u32le body_size` at `4`
  (equals the paired `.bd` length), `0` at `8`, `SShd` at `0x0C`. The registrar tests exactly that
  dword (`0xb1f013`: `cmp dword [r14+0xc],0x64685353`).
- `.bd` layout: 16-byte frames, 28 samples per frame, `byte0` = `filter<<4 | shift`,
  `byte1` = predictor high byte, `bytes 2..15` = 28 signed 4-bit nibbles (high nibble first).
  Decode is `sample = ((nibble<<12) >> shift) + ((hist1*c0 + hist2*c1 + 32) >> 6)` with the PSX/PS2
  coefficients `{0:(0,0), 1:(60,0), 2:(115,-52), 3:(98,-55), 4:(122,-60)}`, then clamp to int16.
  Note that filter 0 has no base-prediction term: its samples are shifted absolute nibbles. Adding
  `hist1` unconditionally was tested and rejected — that variant saturates to a constant output
  (mean |Δ1| 0.2 vs 385 on `bridge.bd`).
- Corpus check over all **25** `.bd` members, **1,620,083** frames: every frame header has
  `shift <= 12` and `filter <= 4`, **0 violations**; all five filters occur
  (0: 49,910; 1: 162,339; 2: 911,961; 3: 164,493; 4: 331,380). Random or non-ADPCM bytes would
  violate the 5-of-16 filter rule roughly 69 % of the time, so the framing is not a coincidence.
- Cross-check: decode 2,048 frames (57,344 samples) of `bridge.bd` and the same window through
  FFmpeg's independent `adpcm_psx` decoder (FFmpeg 9.0.2, wrapped in a synthetic `VAGp` header in a
  private temp dir): correlation **0.99861**, 0.73 % bit-exact samples, mean absolute difference
  347 on a ±32,768 signal. Same codec and same framing; the residual is a rounding/state convention
  difference inside the recursive filter, not a structural mismatch. The FFmpeg path is a
  **cross-check only**; no FFmpeg binary ships with the port.
- Supporting engine evidence: the executable carries `SndVab`, `makeSpuLayoutInfo`,
  `adpcmManager.cpp`, `B_adpcmOpenReq`, `:Sg2ExStAdpcmOpen error %d` (file offsets 0x1277913,
  0x127799a, 0x127779a, 0x1277653, 0x1277662) — a PS2-style VAB/SPU ADPCM manager.

This proves the **sound-effect bank** path decodes to PCM. It does not cover `*.sxd2`, and it does
not prove the `.hd` sample directory (names, per-sample offsets, loop points): that layout is not
decoded here.

## Observed only — `EXST` / `.sts_cp3` (120 B)

`ARTIFACT_VERIFIED` byte structure, **no audio payload**.

All 290 `.sts_cp3` members live under `cp3shadow/sound/test_adpcm/` and are 120 bytes. They begin
with `EXST` (`0x54535845`), then `u16 0`, `u16 2`, `u32 48000` at `+0x08`, `u32 1` at `+0x0C`,
a 32-bit value at `+0x14`, and a 32-bit value repeated at `+0x18` and `+0x3C`; every other
byte is zero in the three members inspected (fields occupy `0x00`-`0x3F`, the rest is empty). The suffix is produced by the `"%s_cp3"` format
string (file offset 0x1277160).

Interpretation: a 120-byte **descriptor** (48 kHz, field value 1, two repeated counters), not
audio. It is consistent with an ADPCM test item, but the directory name is a hint, not evidence of
codec, and no sampling of these files can produce PCM. `UNKNOWN`: units of `+0x14`/`+0x18`.

## UNKNOWN — the `SXD2` stream codec

This is the title's music/ambience/boss/event audio payload (1.7 GB across five files). **The
codec has not been established.** What was measured:

| Test on `music.sxd2` (1 MiB window, offset 0 of the decoded payload) | Attempts | Result |
|---|---:|---|
| PCM plausibility, s16le (mono and stereo interpretations) | 2 | mean&#124;Δ2&#124;/mean&#124;Δ1&#124; = **1.75** (white-noise value ≈ 1.75; real PCM here reads 0.2–0.6). Not PCM. |
| 16-byte SPU/VAG frame framing | 1 | no low-entropy header position (all 16 phases 7.91–7.98 bits); only **24.55 %** of 65,536 aligned frames pass the `shift<=12, filter<=4` rule (uniform random bytes give 25.39 %; real `.bd` gives 100 %). Not SPU ADPCM. |
| zlib(w15), raw DEFLATE, gzip, bzip2, lzma at offsets 0,2,4,8,12,16,20,24,32 | **45** | **0 successes** (`error`/`OSError`/`LZMAError` at every offset) |
| MPEG-1 Layer III frame chain | **1** | 37 candidate sync words, **0** chained frames → not MP3 |
| Container signature at payload offset 0 (RIFF, OggS, fLaC, ID3, ADX, `xff`, MP4) | 7 | 0 at offset 0; four later 4-byte coincidences (`OggS`, `ADX`, `MP4`, `ID3`) do not carry the fields those containers require |
| ATRAC9 | — | **NOT TESTED** (see below) |

Entropy of the payload is 7.97–7.98 bits/byte, uniform across 16/32/48/64/128/256-byte phases, so
there is no byte-aligned PCM, no small-frame ADPCM, and no uncompressed container.

**Why ATRAC9 is a candidate but not a result.** The executable is Sony's `sndx` middleware
(source path `…\ps4sndx20170803_4500_ico_sotc_no_irreverb\runtime\source\sndx\…`, file offset
0x12b9530) and it does contain AT9 voice bookkeeping (`VOICE_ALLOC_AT9 %d/%d`, file offset
0x12b061f) plus stream APIs (`sceSndxInstanceWriteStreamData`, 0x12b558a). But:

- the title imports **no** `libSceAudiocodec`/`libSceAjm`/ATRAC library symbol (checked against
  `local/re/imports.json`, 588 imports);
- the `SXDS` payload has no proven AT9 frame alignment, frame size or 2-byte AT9 config;
- a blind ATRAC9 decode of an arbitrary window would not be a falsification test — it would fail
  whether or not the data is AT9.

So ATRAC9 stays **NOT TESTED**, not "ruled out".

### Exact next measurements

1. **Find the consumer, not another codec guess.** The `.sxd1` paths are passed to the resource
   loader at `0x6b1d84` (`$/sound/shadow/music.sxd1`, file offset 0x1273700). Trace the object that
   receives that resource to the call site of the sndx instance stream API
   (`sceSndxInstanceWriteStreamData`, file offset 0x12b558a). The producer of that buffer is the
   decoder; its switch on the `WAVE` record type field (`0x00ff0642` / `0x00ff0242` / `0x00ff0121`
   / …) is the codec selector. This is a static call-chain task on the pinned ELF, no payload read.
2. **Map the type field to the engine enum.** `sndx_player_wave.c` reports
   `Unknown wave type (%d)` / `Unknown waveform data (%d)` (file offsets 0x12b9598, 0x12b55b0).
   Recover the branch that produces those, and the numeric values it accepts; compare with
   `0x00ff0*` observed in `WAVE` records.
3. **Only then attempt a decode**, on one 960-byte record, with a real configuration word and an
   independent decoder (FFmpeg `atrac9` or `libatrac9`), and report the attempt count.
4. **Channel count / duration.** Nothing in this task proves the stream is mono or stereo. The
   `+0x20` byte lengths and the 48,000 Hz declarations give a candidate duration only once the
   bytes-per-sample ratio is known.

### Other unknowns and their next measurement

| Unknown | Next measurement |
|---|---|
| `SXDS` `+0x08` field (33/125/127/172/53) | Read the first access to payload offset 0 in the consumer found in step 1; test against all five files |
| 960-byte record prefix fields (bytes 0–5) | Dump headers for one file and correlate with the `WAVE` type field; if it is a bit-packed stream, test bit offsets 0–7 for a constant sync |
| `SShd` sample directory (names/offsets/loops) | Complete the registrar at `0xb1f013`; the tables at `+0x10..0x24` are relative offsets and `+0x34`/`+0x38` become u16-array pointers |
| `.sxd` `DATA` chunk codec (26 banks, 313 MB) | Same method as `SXD2`; `com_sys.sxd` DATA has 7.37 bits/byte and no 16-byte framing, so it is not the `.bd` format |
| Whether the game reaches any of this at runtime | No runtime evidence exists; a decoded buffer alone would not prove playback |

## Reproduction

Private, read-only probes (metadata and structure only; no payload written):

```sh
python3 -I local/validation/AUD-01-AUDIO-FORMAT-01/analysis/phase_b_structure.py
python3 -I local/validation/AUD-01-AUDIO-FORMAT-01/analysis/phase_c_family.py
python3 -I local/validation/AUD-01-AUDIO-FORMAT-01/analysis/phase_j_wave_census.py
python3 -I local/validation/AUD-01-AUDIO-FORMAT-01/analysis/phase_k_probe.py
python3 -I local/validation/AUD-01-AUDIO-FORMAT-01/analysis/phase_i_ffmpeg_check.py
```

Bank inspector (synthetic self-test, then a real pair):

```sh
python3 -I tools/dev/se_bank_inspect.py --self-test
python3 -I tools/dev/se_bank_inspect.py --hd "…/cp3shadow/sound/se/bridge.hd" \
    --bd "…/cp3shadow/sound/se/bridge.bd" --json
```

## Limits

- Container/record facts are `ARTIFACT_VERIFIED` on the members listed; the `WAVE` record tiling is
  exact only for `music.sxd1` (the other four files were censused but their record table is not
  fully decoded).
- The `.bd` PCM claim is `ARTIFACT_VERIFIED` for framing and `PARTIAL` for sample-exactness: the
  decode matches FFmpeg's independent `adpcm_psx` at correlation 0.99861, not bit-for-bit.
- The stream codec is **UNKNOWN**. 45 decompression probes and one MP3 chain test failed; ATRAC9
  was deliberately not attempted. A future decode must not be reported as "the title's audio"
  without a decoded, validated sample and a runtime path.
- No sample rate, channel count or loop semantics are claimed for `*.sxd2`.
- G2 (decoded/mixed audio fixtures) is **not** advanced to a pass by this note; it adds a proven
  bank codec, a proven stream container, and a precise blocker for the music codec.
