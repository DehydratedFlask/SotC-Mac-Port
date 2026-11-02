# Audio container and codec status (AUD-01-AUDIO-FORMAT-01)

Status: **SXD/SShd containers now specified from bytes and code; the streamed music codec is
identified as ATRAC9 by the title's own dispatch table, its per-record configuration word is
recovered, and its stream-block geometry is derived — but no ATRAC9 payload has been decoded.**
This note proves the container grammar of the sound banks and stream descriptors, proves that the
`.hd`/`.bd` sound-effect banks are PSX/PS2 SPU ADPCM, and (E1069) proves from pinned code which
field of a `WAVE` record selects the streamed-audio codec and which codec each observed value names.
E1074 then recovers *where* the ATRAC9 configuration comes from and *what it says*: it is field
`0x0A` of each `WAVE` record (`music.sxd1` is 33/33 at `0xFE761DF0`), and the derived values give
960 bytes and 1024 samples per stream block for the 6-channel music. It also records what the
evidence still does **not** establish about `sound/shadow/*.sxd2`: the codec's internal frame layout
and any decoded sample. No decoded game audio is claimed. No member bytes were extracted to disk.

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
the wave records. **Correction (E1069):** the `count`-entry table *is* a record-offset table, but
each entry is **relative to its own address**, so `record_k = &entry[k] + entry[k]`. That resolves
the earlier "advance by 72 while the records advance by 76" observation: the stride only looks like
72 because `entry[k]` moves 4 bytes per entry, and it reproduces every record start in
`music.sxd1` exactly (33/33, 76-byte stride). The field labelled `table_bytes` above is entry 0, and
`4 × count` is what it happens to equal. Records are located by that table, not by scanning for the
`(u32 flags, u32 sample_rate)` prefix; in `music.sxd1` they are 76 bytes apart (the last one 4 bytes
shorter, ending exactly at the next chunk). In the four other `.sxd1` files the record stride is
variable (48/56/68/76/96/116/124/140/144/168 all occur), which the table handles and a scan does not.

Each record begins with an 8-byte `(u32 flags, u32 sample_rate)` pair. Every record in all five
`.sxd1` files carries **48000** at `+0x04`; the count of `48000` occurrences equals the declared
`WAVE` count for `music`, `str_amb`, `str_bgm` and `str_boss`, with one extra (non-record) hit in
`str_event`.

**Correction (E1069) to the row offsets below.** The field offsets in this table are counted from
the format dword, which is what the earlier probe keyed on. The true record base — the one the
offset table and the sndx consumer (`0x987990`) use — is 4 bytes earlier, at the flags dword, so
every offset below is `record + 0x04 + <offset shown>`. All the *measured values* and the tiling
proof are unchanged; only the labels shift. The flags dword itself is `0x1`/`0x3`/`0x5` across the
830 records of the five files, `0x3` in **all 33** `music.sxd1` records, and bit 1 is the sndx
`stream` bit tested at `0x981640` (`testb $0x2,(%r13)`), so every `music.sxd1` wave is a streamed
wave. The `+0x00` dword below is the record's `+0x04`.

| Record field | Observed meaning |
|---|---|
| `+0x00` u32 | **the wave-format dword** (E1069, was "type/flags, not decoded"): low byte = codec/format selector, byte 1 = channel count, byte 2 = `0xFF` in all 830 records, byte 3 = 0. Observed `0x00ff0642` / `0x00ff0242` / `0x00ff0442` / `0x00ff0142` / `0x00ff0221` / `0x00ff0121` / `0x00ff0120` = format `0x42` ch 6/2/4/1, format `0x21` ch 2/1, format `0x20` ch 1. Mapping to codecs in the "wave-format dispatch" section below |
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

`ARTIFACT_VERIFIED` for header/geometry. The payload codec is **ATRAC9** — not from these bytes but
from the `.sxd1` format byte the engine dispatches on; see "RESOLVED — the wave-format dispatch"
below (E1069). Nothing in the `SXDS` container itself selects or validates a codec: there is still no
`SXDS` literal in the image, so the header is not checked by the engine.

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

**Partial resolution (E1069, extended by E1074).** The *selector* is now known from pinned code and
the codec it names is **ATRAC9**; the per-record **configuration word** and the derived stream-block
geometry are recovered in "RESOLVED — where the ATRAC9 configuration comes from" below. The payload
itself is still undecoded: the codec's internal frame layout and one validated decode remain open.
The 45 decompression probes and the MP3 chain test recorded below were run **before** the selector
was known and none of them should be read as a codec result; they are kept as the negative record.
Jump to "RESOLVED — the wave-format dispatch" for the new evidence.

What was measured:

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

**Why ATRAC9 was a candidate before E1069.** The executable is Sony's `sndx` middleware
(source path `…\ps4sndx20170803_4500_ico_sotc_no_irreverb\runtime\source\sndx\…`, file offset
0x12b9530) and it contains AT9 voice bookkeeping (`VOICE_ALLOC_AT9 %d/%d`, file offset 0x12b061f)
plus stream API names. But at that point:

- the title imports **no** `libSceAudiocodec`/`libSceAjm`/ATRAC library symbol (checked against
  `local/re/imports.json`, 588 imports);
- the `SXDS` payload had no proven AT9 frame alignment, frame size or 2-byte AT9 config;
- a blind ATRAC9 decode of an arbitrary window would not be a falsification test — it would fail
  whether or not the data is AT9.

So ATRAC9 stayed **NOT TESTED**, not "ruled out". E1069 supplies the missing evidence from the code
side instead of from a decode attempt; the decode is still NOT RUN.

### RESOLVED — the wave-format dispatch (E1069)

`STATIC_ANALYSIS_VERIFIED` on the pinned ELF (`d0723afb…61d3`), re-derived by
`local/validation/AUD-01-AUDIO-FORMAT-01/analysis/check_sxd2_selector.py` (**49 checks, all PASS,
exit 0**; that script failed nine of its own author's expectations before it went green, so it is not
vacuous). Every address in this subsection is an RVA; the image base is 0, so RVA == address.

**1. The consumer chain, and why the old target was wrong.** All five streamed banks are requested
by name from one routine: `0x6b1d50` — the `adpcmManager` initialiser from the translation unit whose
path literal is at `0x12737a0` (`…\manager\sound\src\adpcmManager.cpp`), building five 0x788-byte
channel objects with 0xb0000-byte buffers. It builds a `{char* ptr; size_t len; …}` name object from
`0x1273700` (`$/sound/shadow/music.sxd1`), `0x127371a`, `0x1273736`, `0x1273752`, `0x127376f` and
calls the name-keyed resource lookup `0xa5eea0` five times (`0x6b1d9f`, `0x6b1dde`, `0x6b1e1d`,
`0x6b1e5c`, `0x6b1e9b`), releasing each key with the shared release `0xf590`. `0xa5eea0` compares
the name with the `strcasecmp` thunk `0xf8f500` against a container of 0x38-byte entries at
`manager+0x28..+0x30`.

The other end is `SoundSystemSndx` (`…\Sound\SoundSystemSndx.cpp`, string at `0x12c4ea0`).
`0xa5c690` tests the path with `strstr(path, ".sxd1")` (`0xa5c6c2` → `0xf8fc80`), builds the
companion path with the `".sxd2"` literal (`0xa5c6d6`, `0x12c4ec1`), and calls
`0x9892d0(sxd1buffer, sxd2path, 0, 0)` (`0xa5c703`). `0x9892d0` is a lock wrapper around `0x985900`,
the SXD registrar that reads the 96-byte header, tests `SXDF` (`0x98598c`), allocates the declared
size (`0x98599c`), scans the chunk list and stores the `WAVE` payload at `object+0x108`
(`0x985f79`–`0x9860xx`). Because the `.sxd2` argument is non-NULL, `0x986952`–`0x986990`
(`testq %rdx,%rdx` / `je`) sets **stream mode 2** at `stream+0x138` and copies the `.sxd2` path into
a 0x100-byte field at `stream+0x140`; with no such argument it sets mode 0 instead. So the `.sxd2`
is consumed as a **file-backed stream**, not by an application-supplied writer.

**`sceSndxInstanceWriteStreamData` is dead code in this title.** `0x9678d0` has no `e8`/`e9`
rel32 transfer anywhere in the image, no `lea`/`mov` RIP-relative reference, no `R_X86_64_RELATIVE`
addend equal to it, and no raw little-endian qword equal to it; its worker `0x9909e0` has exactly one
caller, `0x967973`, which is inside that same dead wrapper, and
`sceSndxInstanceWriteStreamTermination` (`0x967a70`) is likewise unreferenced. The literal
`sceSndxInstanceWriteStreamData` (`0x12b158a`) occurs once, as the trace-name argument of
`0x984700` at `0x967a21`. **The earlier plan of tracing "the producer of the stream buffer" through
this API cannot be executed: there is no such producer in this image.** That is the concrete reason
the previous three attempts at this chain produced nothing.

**2. `Unknown wave type (%d)` is not the codec selector.** The string (`0x12b5598`, source literal
`…\sndx_player_wave.c` at `0x12b5530`) has exactly **one** xref, `0x980b6d`. It is the default arm of
a 3-way test in `0x9805d0`:

```
980816  mov  r8d, [r13+0x28]      ; the value that gets printed as %d
98081a  cmp  r8d, 2  / je 0x980aa1
980824  cmp  r8d, 1  / je 0x9809dc
98082e  test r8d, r8d / jne 0x980b5f   ; -> "Unknown wave type (%d)"
```

So sndx's **wave type** is a 3-value source-kind enum accepting only **0, 1, 2**, and it cannot
carry any `0x00ff0*` value. `Unknown waveform data (%d)` (`0x12b55b0`, three xrefs: `0x980bb5`,
`0x983284`, `0x98330f`) is the "lookup returned NULL" message (`0x980860 je 0x980bb5`), not a switch
default either. Neither string is on the codec path.

**3. The actual selector: the low byte of the `WAVE` format dword.** `0x983ea0`
(`sndx_player_wave.c`, the per-wave geometry setup called from `0x980b53` and `0x981631`) does:

```
983ec2  movzx r8d, byte [rbx+0x579]     ; format byte
983eca  cmp   r8, 0x42 / ja 0x983ff7     ; default -> "Unsupported wave format (%d)"
983ed4  lea   rax, [0x12b5410]
983edb  movsxd rcx, [rax+r8*4] ; add rcx, rax ; jmp rcx   ; 67-entry self-relative table
```

`0x579` is filled from `[wave_descriptor+4]&0xff` at `0x9808a6` (and from `+0x57a` = byte 1 as the
channel count at `0x9808ac`); `0x987990` is the self-relative wave-record lookup
(`record = &entry[k] + entry[k]`) into the registered `WAVE` table at `object+0x60` with
`count` at `object+0x68`. That is the same table the `.sxd1` `WAVE` chunk carries, so the format
byte the dispatcher reads **is** `WAVE_record[+0x04] & 0xff`.

Accepted values and the frame geometry each one sets (`0x594` = block bytes, `0x590` = block
samples; `0x57a` = channel count):

| Format byte | Handler | Geometry | Reading |
|---:|---|---|---|
| `0x00`,`0x01` | `0x983ef0` | `channels × 1` B | 8-bit linear PCM |
| `0x04`,`0x05` | `0x983ef9` | `channels × 2` B | 16-bit linear PCM |
| `0x08`,`0x09` | `0x983f04` | `channels × 3` B | 24-bit linear PCM |
| `0x0C`,`0x0D`,`0x10`,`0x11` | `0x983ee4` | `channels × 4` B | 32-bit linear (two codes, e.g. int and float — not distinguished) |
| `0x14`,`0x15` | `0x983f10` | `channels × 8` B | 64-bit linear |
| `0x20`,`0x21` | `0x983f2f` | `channels × 16` B per **28** samples, offsets re-aligned to 28 | **PSX/PS2 SPU ADPCM** — the same 16 B / 28 samples rule proved for `.bd` above |
| `0x22` | `0x98402a` | `channels × 4` B per **6** samples | unidentified; not used by any observed `.sxd1` record |
| `0x28`, `0x30`–`0x35` | `0x9840f8` | geometry taken from fields preset elsewhere | unidentified; not used by any observed `.sxd1` record |
| `0x42` | `0x98413e` | ATRAC9 setup (see below) | **ATRAC9** |
| everything else in `0x00`–`0x42` (44 values) | `0x983ff7` | — | `Unsupported wave format (%d)` |

Every one of the 67 table entries lands inside the dispatcher's own 0x430-byte body, so the table is
complete and not truncated.

**4. Format `0x42` is ATRAC9, from the branch body.** `0x98413e` fetches a configuration block with
`0x987950(id=[rbx+0x5f8], inline=&[rbx+0x5e0])` (`0x984145`–`0x984158`), then:

- `bextr` bit 20 of the first config dword selects a block-sample count from the u16 table at
  `0x12b5700` = `[0, 64, 0, 0, 128, 0, 0, 256]` (`0x98417d`–`0x984196`);
- `1 << ((cfg >> 3) & 3)` is stored as a block multiplier at `0x58b` (`0x98416a`–`0x984177`);
- the per-channel configuration words (`0x984223`–`0x984290`) each have to satisfy
  `word & 0xFF000000 == 0xFE000000` (`0x984246`–`0x984255`); each tested word contributes
  `bits[5]` and a byte from the table at `0x12b5720` to an accumulator written to `0x594`
  (`0x98426b`–`0x98427c`), i.e. the ATRAC9 branch *computes* the block's byte count from the
  configuration. A word that fails the `0xFE` test logs
  **`Invalid ATRAC9 configure data (%08X)`** (`0x12b5660`, line 0x145 of `sndx_player_wave.c`,
  xref `0x98429e`).
**Inference (labelled).** A top byte of `0xFE` on a 32-bit configuration word is the published
ATRAC9 configuration signature — that part is general knowledge, not something these bytes prove.
What the bytes *do* prove is the identity of the branch: it is `sndx_player_wave.c`'s only arm that
reports a codec by name (`ATRAC9`), it is the only arm that consumes a separate configuration block
at all, and it is the only arm whose failure text is a codec name. No other arm in the 67-way
dispatch mentions any codec.

**5. The observed-type → codec mapping.** The offset-table census (re-run in the checker) reaches
exactly the declared `WAVE` count in **all five** files — 33 / 414 / 133 / 194 / 56, total **830**
records — where the old prefix-scan census found only 33/87/82/162/51. Those 830 records carry
exactly three format bytes:

| `+0x00` dword observed | format | channels | records | where | codec |
|---|---:|---:|---:|---|---|
| `0x00ff0642` | `0x42` | 6 | 415 | all 33 of `music.sxd1`, 87 `str_amb`, 82 `str_bgm`, 162 `str_boss`, 51 `str_event` | **ATRAC9** |
| `0x00ff0242` | `0x42` | 2 | 131 | 47 / 50 / 29 / 5 | **ATRAC9** |
| `0x00ff0442` | `0x42` | 4 | 3 | 1 / 2 | **ATRAC9** |
| `0x00ff0142` | `0x42` | 1 | 17 | 17 (`str_amb`) | **ATRAC9** |
| `0x00ff0121` | `0x21` | 1 | 230 | 229 `str_amb`, 1 `str_boss` | SPU ADPCM |
| `0x00ff0221` | `0x21` | 2 | 33 | 33 (`str_amb`) | SPU ADPCM |
| `0x00ff0120` | `0x20` | 1 | 1 | 1 (`str_bgm`) | SPU ADPCM |

566 records are ATRAC9 and 264 are SPU ADPCM. The `.sxd2` payload of **`music.sxd1`
(172,367,136 B) is therefore ATRAC9, 6 channels**. The five `.sxd2` files total **1,714,959,920 B**.

**6. What this does and does not settle.** It settles the *selector* and names the codec: the
`0x00ff0*` dword is `(0xff << 16) | (channels << 8) | format`, `format = 0x42` is ATRAC9, and the
placement of the format byte in `WAVE_record[+0x04]` is proved twice — once from the writer
(`0x9808a6`) and once from the reader's dispatch (`0x983ec2`). It does **not** settle:

- **where the ATRAC9 configuration block comes from.** `0x5f8` (a config resource id) is
  zero-initialised per voice at `0x980a34`, and the two inline words are `0x5e0`/`0x5e4`
  (`0x980a3f`, `0x980a5b`); nothing in the `WAVE` record carries a 0xFE config word. This is the
  single missing input for a decode.
- **the frame geometry.** It is derived from that config block (block samples 0/64/128/256, block
  multiplier 1/2/4/8), not from the record, so the 960-byte payload stride cannot yet be mapped to
  ATRAC9 frames. Note that 960 is not obviously an ATRAC9 frame size; it may be a container/stream
  block rather than a codec frame, which is exactly why a blind decode was refused.
- **any decode.** ATRAC9 remains **NOT RUN** — see next measurement 1.

### RESOLVED — where the ATRAC9 configuration comes from (E1074)

`STATIC_ANALYSIS_VERIFIED` on the same pinned ELF, re-derived by
`local/validation/AUD-01-AUDIO-FORMAT-01/analysis/check_atrac9_config.py` (**48 checks, all PASS,
exit 0**) and, independently and reproducibly from the repo, by
`tools/lift/check_atrac9_config_provenance.py` (**65 checks PASS, 40/40 mutation probes detected**,
E1076), plus `ARTIFACT_VERIFIED` for the `.sxd1` field census. Every address is an RVA; the image
base is 0. **The configuration is per-record data inside the `.sxd1` file, not an id into any table
in the image.**

*Corrections made by the E1076 re-derivation (the sections below are already amended):* the table at
`0x12b5720` gives the **channels the word contributes**, not "frames per superframe"; a 960-byte
stream block is **four 240-byte frames** (one `0x594` frame = all channels of one 256-sample column),
not "four ~240-byte superframes"; and the per-channel-bitrate remark was an inference and is removed.
Bit-field **names** (frame bytes, superframe index, validation bit) are the public ATRAC9 layout and
are labelled INFERENCE; the field **extractions** (`bextr` starts/lengths) are observed.

**1. `0x987950` is not a lookup — it is a base-relative pointer fixup.**
`0x987950`: `48 85 f6 / 74 07 / 48 03 77 20 / 48 89 f2 / 48 89 d0 / c3` =
`return id ? [ctx+0x20] + id : inline`. Its inverse `0x987940`:
`31 c0 / 48 85 f6 / 74 07 / 48 2b 77 20 / 48 89 f0 / c3` = `return ptr ? ptr - [ctx+0x20] : 0`.
So the `[wave+0x5f8]` argument is an **offset** into the allocation whose base is `[ctx+0x20]`, and
the `0x5e0` pair is only the fallback for `offset == 0`. Both helpers are called (never taken as a
value — no raw qword anywhere in the file equals either address) from **52** sites in
`0x940000..0x9a0000`; they are a general arena-pointer idiom, not codec code.

`ctx` is `[wave+0xa0]`. That same object is the one the wave-record lookup uses: `0x98084e` loads
`rdi = [obj+0xa0]` and calls `0x987990` (the self-relative `WAVE` table at `ctx+0x60`, count at
`ctx+0x68`), and `0x980896` stores `record - [ctx+0x20]` at `[obj+0x580]`. The records and the
configuration payload therefore live in the **same arena** as the `WAVE` table.

**2. The writer: `WAVE`-record field `0x0A`.**
The wave setup routine `0x980620` (the routine that also writes `0x579`, `0x580` and `0x5f8` for
this object) parses a field list that starts at `record+0x28` and is entered only when
`record[+0x00] & 1` (`0x9809aa`: `testb $0x1,(%r15)`; every one of the 830 records has bit 0 set).
The loop (`0x980c30`) reads `{u8 id; u8 count; u16 size}` from `r14`, bounds `id` at `0x14`, and
dispatches through a **21-entry self-relative table at `0x12b5274`** (`movslq (%r12,%rcx,4),%rcx /
add %r12,%rcx / jmp *%rcx`); it advances `r14 += 2 + size`, so `size` counts the two size bytes.

| Field id | Handler | Stores | Source |
|---:|---|---|---|
| `0x0A` | `0x980f29` | `[obj+0x5f8] = 0x987940(ctx, entry+4)`, `[obj+0x600] = min(count,16)` | **the ATRAC9 configuration word** |
| `0x0C` | `0x980f78` | `[obj+0x5e8] = 0x987940(ctx, entry+4)`, `[obj+0x601] = min(count,16)` | second config block (per-channel descriptors) |
| `0x0F` | `0x98106f` | `[obj+0x5f0] = 0x987940(ctx, entry+4)`, `[obj+0x602] = min(count,32)` | third config block |
| all other ids in `0x00`–`0x14` | various | gain / filter / loop / per-channel fields; `0x08`, `0x09`, `0x10` are no-ops (`0x9811f0`) | not decoded here |

The per-wave reset block (`0x980772`–`0x9807a5`) clears `0x5f8`, `0x600`, `0x5e0`..`0x5ef`,
`0x5f0`, `0x601`, `0x602` before the dispatch, so a format-`0x42` wave with **no** field `0x0A`
would read `0x5e0 = 0`, fail the `0xFE` test at `0x984246` and log the ATRAC9 error. No such record
exists in the corpus (566/566 ATRAC9 records carry field `0x0A`; no non-ATRAC9 record carries it).

**Where the setup reads its descriptor from (`spec[+0x28]`, E1076).** `0x980620(obj, spec)` is a
virtual method (vtable `0x2dd0900`, slot at `0x2dd09c8`) and `spec[+0x28]` selects the *source* of the
config pair, so the WAVE record is only one of three:

| Kind | Site | Config source | Count |
|---:|---|---|---|
| 0 | `0x980f39` | WAVE record field `0x0A` in the `.sxd1` arena (all 566 corpus records) | `min(count,16)` |
| 1 | `0x9809dc` | inline words copied from `spec+0x38` / `spec+0x3c` | 2 |
| 2 | `0x980aa1` | one inline word from `spec+0x40` | 1 |

The producers of kind-1 and kind-2 specs are **unresolved** (the call is virtual; the one
`callq *0xc8` candidate found belongs to a different class). They matter only for content that is not
a WAVE record; every format-`0x42` record in the five `.sxd1` files is kind 0.

**Writer census of `[obj+0x5f8]`.** Exactly one store of a record-derived value (`0x980f39`) plus the
per-wave reset (`0x980772`..`0x9807a5`); the only reader that consumes it as ATRAC9 geometry is the
arm at `0x984153`, with one more consumer, the stream path (`0x983b0f`), which forwards the pointer to
`0x94b2c0`. The field loop's descriptor semantics are `{u8 id; u8 count; s16 size}`, advance
`2 + size`, ids bounded by `0x14`, and `size == 0` ends the list.

**3. The payload is one 32-bit word.** Field `0x0A` is `{id 0x0a; count 1; size 6; u32 word}` — six
bytes from `+0x02`, i.e. the two size bytes plus exactly the word at `entry+4`; `count` is 1 in every
record, so `0x600 = 1` and the arm consumes one word. In `music.sxd1` record 0 (file offset `0xCD4`)
the entry is at `+0x3C` and the word is at `+0x40`.

| `.sxd1` | `0xFE761DF0` | `0xFE761DE0` | `0xFE740FF0` | `0xFE740FE0` | `0xFE7A17F0` | `0xFE7007F0` | ATRAC9 records |
|---|---:|---:|---:|---:|---:|---:|---:|
| `music.sxd1` | **33** | – | – | – | – | – | 33 |
| `str_amb.sxd1` | 87 | – | 47 | – | 1 | 17 | 152 |
| `str_bgm.sxd1` | 82 | – | 50 | – | – | – | 132 |
| `str_boss.sxd1` | 57 | 105 | 21 | 8 | 2 | – | 193 |
| `str_event.sxd1` | 7 | 44 | 5 | – | – | – | 56 |
| **total** | **266** | **149** | **123** | **8** | **3** | **17** | **566** |

**`music.sxd1` is 33/33 at `0xFE761DF0`.** No other configuration word occurs; field `0x0C` (the
second block, `0x5e8`/`0x601`) is **absent from all 830 records**, so the ATRAC9 arm always takes the
inline-descriptor path at `0x9841e6` (`{count = channels, multiplier = 1}`).

**4. The configuration word decodes to the geometry the arm then computes.**
`bextr` control `0x414` = start 20, **len 4** → `bits[23:20]` indexes `0x12b5700`
`[0,64,0,0,128,0,0,256]`; control `0x311` = start 17, **len 3** → `bits[19:17]` indexes `0x12b5720`
`[1,2,2,6,8,4,0,0]`; control `0xb05` = start 5, **len 11** → `bits[15:5] + 1` is the byte count;
`1 << ((cfg >> 3) & 3)` (`0x98416a`) is the block multiplier. (`bextr` encodes start in bits 7:0 and
the bit count in bits 15:8. The widths above are the ones that make all six words self-consistent:
read with a count of 4 at bit 17 the same word gives index 11 into an 8-entry table, and the 3-bit
read gives 3 → 6 frames, which equals the channel count in all six groups — independently confirmed
in §5. The 4- and 5-bit reads at bit 20 agree for every word here, because bit 24 is the low bit of
the `0xFE` tag byte and is always 0.)

| Config word | ch | `bits[23:20]` | samples/frame | `bits[19:17]` | channels contributed | `bits[15:5]+1` = `0x594` frame B (all ch) | `bits[4:3]` | mult | `0x592` samples/block | `0x596` bytes/block |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| `0xFE7007F0` | 1 | 7 | 256 | 0 | 1 | 64 | 2 | 4 | 1024 | **256** |
| `0xFE740FE0` | 2 | 7 | 256 | 2 | 2 | 128 | 0 | 1 | 256 | **128** |
| `0xFE740FF0` | 2 | 7 | 256 | 2 | 2 | 128 | 2 | 4 | 1024 | **512** |
| `0xFE7A17F0` | 4 | 7 | 256 | 5 | 4 | 192 | 2 | 4 | 1024 | **768** |
| `0xFE761DE0` | 6 | 7 | 256 | 3 | 6 | 240 | 0 | 1 | 256 | **240** |
| `0xFE761DF0` | 6 | 7 | 256 | 3 | 6 | 240 | 2 | 4 | 1024 | **960** |

*Observed:* the table at `0x12b5720` (`[1,2,2,6,8,4,0,0]`) is indexed by `bits[19:17]` and the arm's
inner loop at `0x984240` accumulates its entries until the running count reaches the record's channel
count; in all six rows the table value equals the channel count. It is therefore a "channels
contributed by this word" table (the prior label "frames/superframe" is withdrawn). The shared
finaliser `0x9840f8` writes `0x592 = mult × samples` and `0x596 = mult × bytes`
(`0x984108`–`0x984119`), and the stream path divides a byte position by `0x596` to get a block index
(`0x983977`); `0x594` is the unscaled frame byte count.

*INFERENCE (public ATRAC9 layout, not read from the binary):* the 8-bit `0xFE` tag, the 4-bit
sample-rate index (7 = 48 kHz — consistent with every record's rate dword `48000`), the 3-bit channel
configuration, a validation bit (bit 16, zero in the whole corpus), the 11-bit `frame bytes - 1`
field, and the 2-bit superframe index with reserved low bits `[2:0] = 0` (also zero throughout). Under
that reading the multiplier is the number of frames in a superframe (`1 << n`, n = 0 or 2 here). Per
channel frame bytes (`0x594 / channels`) would be 64/64/48/40 for 1/2/4/6 channels; no bitrate claim is
made from them, because the ATRAC9 channel-element split is not recoverable from this arithmetic.

**Frame geometry summary** (geometry the code computes; decoder agreement is NOT RUN):

| Channels | Samples / frame | Bytes / frame (all ch) | Frames / stream block | Samples / block | Bytes / block | Records |
|---:|---:|---:|---:|---:|---:|---:|
| 1 | 256 | 64 | 4 | 1024 | 256 | 17 |
| 2 | 256 | 128 | 1 or 4 | 256 or 1024 | 128 or 512 | 8 + 123 |
| 4 | 256 | 192 | 4 | 1024 | 768 | 3 |
| 6 | 256 | 240 | 1 or 4 | 256 or 1024 | 240 or 960 | 149 + 266 |

**5. Independent confirmation from the assets (no decode).** For every
`(format, channels, configuration)` group in the five `.sxd1` files, the derived `0x596` value
divides **every** segment length in that group and equals the group's gcd exactly:

| group | records | gcd of segment lengths | derived `0x596` |
|---|---:|---:|---:|
| 1 ch / `0xFE7007F0` | 17 | 256 | 256 |
| 2 ch / `0xFE740FE0` | 8 | 128 | 128 |
| 2 ch / `0xFE740FF0` | 123 | 512 | 512 |
| 4 ch / `0xFE7A17F0` | 3 | 768 | 768 |
| 6 ch / `0xFE761DE0` | 149 | 240 | 240 |
| 6 ch / `0xFE761DF0` | 266 | **960** | **960** |

The whole `music.sxd2` payload is 172,367,040 B = **179,549 × 960** exactly, which is also the
figure E1069 measured from entropy alone. The 960-byte "record" seen in the payload is therefore
**one stream block of four 240-byte frames** (`0x594` = 240 B each, 256 samples per frame; the
four-frame grouping is the `bits[4:3]` multiplier), not a single frame. Additional asset facts
(`ARTIFACT_VERIFIED`, E1076): the maximum `start + length` over the records equals the `.sxd2` size in
all five files; only `music` tiles its `.sxd2` exactly; record starts are **not** aligned to the
superframe size; sample-rate index 7 coincides with 48000 in every ATRAC9 record; and field `0x0A`
occurs only in format-`0x42` records. A byte-phase entropy census
over 7,680,000 payload bytes (8,000 blocks) agrees: counting phases whose entropy is below 7 bits,
periods 16/20/24/30/32/40/48/60/64/80/96/192/256 give **0**, period 240 gives **7** (phases 0–5 at
4.40–6.98 bits and phase 154), and period 960 gives **46**, whose deep minima are at 0–5
(0.94–5.61 bits), 242–245 (0.74), 481–484 (0.84), 720–723 (0.92) and 959 (5.98) with weaker bands at
150–159, 392–397, 631–635 and 870–875. That is **four header-like regions per 960-byte block at a
239–243-byte spacing**. The geometry says 240-byte frames, yet the entropy minima sit at 0, 242, 481,
720 (about ±2 bytes off a 240 grid); that phase offset is **unexplained** and is not used for any
claim. A decode attempt must therefore start at exact 240-byte boundaries and also try the observed
offsets. **No payload byte and no decoded sample is recorded here.**

**6. Call-target enumeration of the ATRAC9 arm (`0x98413e`–`0x9842cf`).** Four `call`s and one
`jmp`, no import and no decode:

| Site | Target | Role |
|---|---|---|
| `0x984153` | `0x987950` | configuration lookup (`id = [obj+0x5f8]`, inline `&[obj+0x5e0]`) |
| `0x9841bb` | `0x987950` | second block lookup (`id = [obj+0x5e8]`, `rdx = 0`); dead for this corpus |
| `0x9842b1` | `0x983cf0` | message logger, failure arm only |
| `0x9842ca` | `0xf8f3a0` | stack-canary failure handler |
| `0x984299` | `0x9840f8` (jmp) | shared finaliser that folds the multiplier into `0x592`/`0x596` |

So **the arm is geometry only**: it validates and measures the configuration and returns 0; it
neither decodes nor calls out to a decoder. The descriptor it produces is handed on by the stream
path (`0x983970`–`0x983b0f`), which passes `base+0x5f8`/`0x600`, `base+0x5e8`/`0x601` and
`base+0x5f0`/`0x602` to `0x94b2c0` (call at `0x983b0f`); what that callee does with them is outside
this task.

**7. Nothing about this is in the image.** None of the six words occurs as raw bytes anywhere in the
pinned ELF (0 hits each), and the R-only RELRO program header (`0x1594000`, filesz `0x6d190`)
contains **no** `0xFE`-top dword. There is therefore no "default" ATRAC9 configuration to fall back
on: the only source is the record.

**8. What is still not settled.** No decode was attempted and none is claimed. The arm's arithmetic
gives 256 samples and 240 bytes per column and 1024 samples / 960 bytes per stream block (four
columns) **for the 6-channel music configuration**; whether a decoder's own frame size matches those
numbers bit-for-bit is a property of the decoder, not of these bytes, and remains unverified. Whether
`0x94b2c0` (or an NGS2 rack) ultimately performs the ATRAC9 transform is untouched: the literals
`maxAtrac9Decoders` (RVA `0x12ac371`) and `VOICE_ALLOC_AT9 %d/%d` (RVA `0x12ac61f`) exist in the
image, but `maxAtrac9Decoders` has **no** RIP-relative xref and nothing here proves NGS2 is the
decoder. Duration is now derivable as `blocks × 0x592 / 48000` (for `music.sxd2`: 179,549 × 1024 /
48,000 = 3,830.4 s of 6-channel material across the 33 waves), but that is arithmetic on the code's
own bookkeeping, not a measured duration.

### Exact next measurements

1. ~~**Has ATRAC9 been ruled out as "reached only through a system library"?**~~ **ANSWERED (E1074).**
   `0x983ea0`'s ATRAC9 arm calls only `0x987950` (twice), `0x983cf0` (message logger) and `0xf8f3a0`
   (stack canary), and tail-jumps to `0x9840f8`; it contains no decode and no import. The geometry is
   consumed later by the stream path, which hands the configuration pointers to `0x94b2c0`. Where the
   transform itself happens is still open.
2. ~~**Recover the ATRAC9 configuration word.**~~ **ANSWERED (E1074).** `0x987950` is
   `id ? [ctx+0x20] + id : inline`, and `[wave+0x5f8]` is written by the record field parser from
   field `0x0A`. `music.sxd1` is 33/33 at `0xFE761DF0`; the corpus uses six words, listed above.
3. **Attempt a decode** of one payload unit, with a real configuration word and an independent
   decoder (FFmpeg `atrac9`/`libatrac9`), reporting the attempt count. Use `0xFE761DF0` for
   `music.sxd2` and a **960-byte stream block** as the unit (it is exactly 179,549 blocks); the
   240-byte frame is the finer unit given by `bits[15:5]+1`. The entropy census puts minima about
   ±2 bytes off the 240 grid (unexplained), so try exact 240-byte boundaries first, then the observed
   242/481/720 offsets. Handle payload in memory only, and report the attempt count.
4. ~~**Channel count / duration.**~~ **RESOLVED structurally (E1074).** Channel count is the format
   dword's second byte; the configuration supplies `0x592` samples per stream block (1024 for
   `music`), so duration is `blocks × 0x592 / 48000`. For `music.sxd2` that is 3,830.4 s of
   6-channel material over the 33 waves — arithmetic on the code's own bookkeeping, not a decode.

### Other unknowns and their next measurement

| Unknown | Next measurement |
|---|---|
| `SXDS` `+0x08` field (33/125/127/172/53) | Read the first access to payload offset 0 in the registrar `0x985900` after the chunk scan; test against all five files |
| 960-byte block prefix fields (bytes 0–5) | The block is now known to be four ~240-byte sub-units whose byte phase drifts (E1074); next, disassemble `0x94b2c0` (the callee the stream path hands `base+0x5f8`/`0x600` to) and check whether it re-derives a bit-level frame boundary |
| Where the ATRAC9 transform executes | `0x94b2c0` is a local function with 13 callers; `maxAtrac9Decoders` (`0x12ac371`) and `VOICE_ALLOC_AT9 %d/%d` (`0x12ac61f`) exist but the first has no RIP-relative xref. Trace `0x94b2c0` and the NGS2 rack configuration; nothing here proves NGS2 is the decoder |
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

Wave-format selector (E1069; re-derives every address, byte and table entry cited above from the
pinned ELF, and re-runs the `.sxd1` format census when the private descriptors are present):

```sh
python3 -I local/validation/AUD-01-AUDIO-FORMAT-01/analysis/check_sxd2_selector.py
```

ATRAC9 configuration provenance (E1074; re-derives the two pointer helpers, the 21-entry field table,
the four writers, the arm's call targets, both geometry tables, the "no configuration in the image"
result, and — when the private descriptors are present — the 566-record field census and the
block-size/gcd identity for all six configuration groups):

```sh
python3 -I local/validation/AUD-01-AUDIO-FORMAT-01/analysis/check_atrac9_config.py
```

Helper used to walk the pinned disassembly while tracing (`fde` / `str` / `xref` subcommands):

```sh
python3 -I local/validation/AUD-01-AUDIO-FORMAT-01/analysis/trace_sxd2.py fde 0x983ea0
```

Bank inspector (synthetic self-test, then a real pair):

```sh
python3 -I tools/dev/se_bank_inspect.py --self-test
python3 -I tools/dev/se_bank_inspect.py --hd "…/cp3shadow/sound/se/bridge.hd" \
    --bd "…/cp3shadow/sound/se/bridge.bd" --json
```

## Limits

- Container/record facts are `ARTIFACT_VERIFIED` on the members listed. Since E1069 the `WAVE`
  record table is fully decoded for **all five** `.sxd1` files (self-relative offset table, 33/414/
  133/194/56 records = the declared counts), so the old "record table not fully decoded for the
  other four files" limit is withdrawn; the byte-exact contiguous tiling claim is still only
  demonstrated for `music.sxd1`.
- The `.bd` PCM claim is `ARTIFACT_VERIFIED` for framing and `PARTIAL` for sample-exactness: the
  decode matches FFmpeg's independent `adpcm_psx` at correlation 0.99861, not bit-for-bit.
- The stream **selector** is proved and names **ATRAC9** (`STATIC_ANALYSIS_VERIFIED`), and the
  per-record **configuration word** plus the derived stream-block geometry are now recovered
  (E1074), but no ATRAC9 payload has been decoded: the codec's internal frame layout and a validated
  sample are still missing, and ATRAC9 stays **NOT RUN**. The 45 decompression probes and one MP3
  chain test predate the selector and are negative results only. A future decode must not be
  reported as "the title's audio" without a decoded, validated sample and a runtime path.
- Channel count is claimed per record from the format dword's second byte (and the ATRAC9/SPU
  dispatch reads that same byte); sample rate is 48,000 for every record. Duration is now
  *derivable* (`blocks × 0x592 / 48000`) but is arithmetic on the code's bookkeeping, not a measured
  value; loop semantics remain unclaimed.
- The configuration word is located and its byte content is quoted, but its codec-internal field
  meanings are only established through the arithmetic at `0x98413e`–`0x984119`; the labels "frame"
  and "superframe" here are shorthand for what that arithmetic produces (a `bits[15:5]+1` byte count
  and a `bits[19:17]` count that equals the channel count), not decoder-verified structures.
- The `sceSndxInstanceWriteStreamData` route is **refuted**, not merely untested: that function is
  unreachable in this image. Do not re-open the "trace the stream-buffer producer" plan.
- G2 (decoded/mixed audio fixtures) is **not** advanced to a pass by this note; it adds a proven
  bank codec, a proven stream container, a proved codec selector, a recovered per-record ATRAC9
  configuration and a derived stream-block geometry. The decode itself is still absent.

