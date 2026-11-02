# Codec decisions: the two decoders the port must supply

**This is a decision record, not an implementation.** Nothing here decodes a member, vendors a
library, installs a tool or changes a gate. It states, for each of the two codecs the port needs,
what is needed, what is already pinned, the options with their real licensing and verification
consequences, and the acceptance test each option would have to pass.

Owner-facing summary: [§0](#0-decision-requested). Tasks:
`GPU-01-OODLE-DECODER-DECISION-01`, `AUD-01-ATRAC9-DECODER-DECISION-01`
(both `BLOCKED / WAITING_FOR_OWNER_DECISION` in [TASKS](TASKS.md)).

Labels used: `STATIC_ANALYSIS_VERIFIED` (pinned ELF code), `ARTIFACT_VERIFIED` (pinned asset
bytes), `FIXTURE_VERIFIED` (synthetic), `PASSED`/`PARTIAL`/`FAILED`/`NOT RUN` for tests, and
`INFERENCE` where a reading is not measured.

---

## 0. Decision requested

| # | Codec | Question for the owner | If the answer is "no" |
|---:|---|---|---|
| 1 | **Oodle** (`.csdr`) | Which route do you want for the compressed 56 % of the shader corpus: (a) a licensed Oodle SDK — linked into the app, or used only at import time — (b) a clean-room Mermaid/Kraken decoder, or (c) ship raw-only and reject compressed members visibly? | 19,320 members stay unreadable; no GPU task is blocked *today*, but ~67 % of distinct shader programs can never be lowered |
| 2 | **ATRAC9** (`.sxd2`) | Which route for streamed music: (a) vendor a permissively-licensed decoder, (b) clean-room, or (c) no streamed music? | The game has sound effects and ambient beds but **no music**, and G2 cannot be claimed for the audio subsystem |

Both answers are independent. Neither is a prerequisite for the work already in flight
(startup slots, MSL lowering shape, launcher packaging); see [§4](#4-what-the-port-can-do-today-without-either-decoder).

---

## 1. Why this record exists

Two investigations closed the "where is the decoder?" question for both codecs, with opposite
answers, and both leave the port holding the same requirement: **supply a decoder that is not in
this repository.**

### 1.1 The two gaps are not the same kind of gap

| | Oodle (`.csdr`) | ATRAC9 (`.sxd2`) |
|---|---|---|
| Is the decoder in the image? | **Yes** — a complete Oodle 2 LZ SDK is statically linked (`0x890000..0x8c0000`, 178 `oodle` labels, 24 `OodleLZ*` symbols, 11 codec-family names, 25 `v:\devel\projects\oodle2\core\*.cpp` paths) | **No** — the transform is behind `sceNgs2VoiceControl`, i.e. inside `libSceNgs2` (sysmodule `0xb`) |
| Why can the port not use it? | **Licensing**, not reachability. The bytes are ordinary x86 functions, but they are a third party's proprietary codec implementation | **Reachability**. There is nothing to use or copy: the image contains geometry arithmetic and an NGS2 parameter marshaller, no transform |
| Evidence | E1075, E1078, E1080 | E1069, E1074, E1076, E1077 |

That distinction is the whole point of this record. For ATRAC9 the investigation ended at a proved
negative. For Oodle it ended at a **licensing boundary that the investigation deliberately did not
cross**: the decoder is present, and translating or extracting it would reproduce a proprietary
implementation without a licence. It is not a shortcut we may take, and [§2.5](#25-excluded--not-an-option)
records it as excluded so a later task does not rediscover it as an "idea".

### 1.2 Boundaries this record does not cross

- It does not recommend, plan or approve any use of Oodle code taken from the game image or from a
  PS4 system library.
- It does not strip, disassemble-for-porting, or re-encode any part of the image into a shipped
  artefact. Reading the image to *understand the call contract* is what the existing evidence did;
  reproducing the codec body is a different act.
- It does not vendor anything. No file, binary or source was downloaded or added.
- It does not write `.csdr` or audio payload bytes into the repository, and no option below asks
  anyone to.
- It does not decide. The owner decides; the two `TASKS.md` rows stay `BLOCKED` until then.

---

## 2. Oodle — compressed `.csdr` shaders

### 2.1 What is needed

A decoder for an **Oodle 2 LZ stream**, decode types **6 (Kraken)** and **10 (Mermaid)**, operating
on a **single chunk containing a single quantum** per member. That is the entire functional surface:
every compressed member in the corpus is one chunk and one quantum (max `compLen` 20,380,
max `rawLen` 33,468, per-quantum raw cap `0x20000`), so no multi-chunk, seek-chunk or streaming path
is required. `STATIC_ANALYSIS_VERIFIED` + `ARTIFACT_VERIFIED` (E1080).

### 2.2 What is already pinned

An implementer needs **nothing further from the binary**. `STATIC_ANALYSIS_VERIFIED` unless marked.

| Item | Value |
|---|---|
| Container | `member = [Oodle stream][12-byte trailer]`; trailer = `{u64 tag; u32 {type:3 @ bits31..29, size:29}}` |
| Trailer tag | `0x1343534452` = bytes `52 44 53 43 13 00 00 00` — 34,392/34,392 members |
| Codec selector | the *stream* header, not the trailer type: byte 1 `0x0a` → decode type 10 → **Mermaid** (16,328); `0x06` → type 6 → **Kraken** (2,992). Names come from the image's own table `0x1295090` → name pointers `0x15ced00+8*8` → `"Kraken"` / `"Mermaid"` |
| Trailer types present | `0` = 15,072 (raw), `3` = 16,328, `4` = 2,992; type 3 ⇔ `8c0a`, type 4 ⇔ `8c06`, 1:1 with no exception; **no type 1, 2 or 5** |
| Decode contract (retail) | `OodleLZ_Decompress(member[0 … S-12), S-12, dst, rawLen = trailer & 0x1fffffff, fuzzSafe = 1, checkCRC = 0, verbosity = 0, decBufBase = 0, decBufSize = 0, fpCallback = 0, userData = 0, decoderMemory, decoderMemorySize, threadPhase = 3)` — **nothing stripped**, input starts at byte 0 |
| Stream header | bytes `[0,2)` block header (`0x8c` + type), `[2,5)` 3-byte chunk header (= `compLen-1`), `[5,8)` newLZ quantum header (`0x88xxxx`, mode 1), `[8,16)` **first 8 output bytes stored raw**, `[16, 5+compLen)` entropy/command streams |
| Closed-form corpus relations | `S = compLen + 17`; chunk field `= S-18`; quantum size `= S-20`; `compLen = S-17 < rawLen ≤ 0x20000` — all 19,320/19,320 |
| Decoded-content acceptance | the output is an OrbShdr record table parsed by `0xb0ced0` (allocates `0x88` B, `0xe4`-byte shader records) |
| Retail failure behaviour | **fail-open**: `Decompress` discards the codec's return value in `rax` and returns `size` regardless (`0x1aa42b`, `0x1aa452`). A port that wants fail-closed behaviour must check the codec itself |
| Threading | the retail call is `ThreadPhase_Unthreaded` under a critical section on `0x2ee6728` (shared decoder memory at `0x2ee6720`) — a native decoder may ignore both |

What remains genuinely open is **only** the entropy/command-stream grammar of bytes
`[16, 5+compLen)`, which is what an implementation supplies. It is not a missing fact about the
title; it is the decoder.

Corpus scale, for the cost side of the decision (`ARTIFACT_VERIFIED`): 34,291 `.csdr` members in
`pak0`+`pak1`+`pakN`, **19,320 compressed (56.34 %)** and **14,971 raw**; corpus-wide with
`cachedFiles.psarc` the raw count is 15,072. Distinct shader stems: **7,767 compressed, 3,799 raw,
only 34 in both** — the raw set is *not* a subset, so dropping compressed support drops whole
shaders rather than duplicate encodings.

### 2.3 Options

| Option | What you get | Licence consequence | Verification burden |
|---|---|---|---|
| **(a1) Licence the Oodle SDK and link it into the app** | Every compressed member decodes with the vendor's own implementation; lowest engineering risk | Requires a negotiated licence from Epic/RAD. The FSF reading of GPL-2.0 §2(b)/§7 is that a GPL-2.0-or-later work may not be *distributed* combined with a proprietary library whose terms add restrictions, and Oodle's will not be GPL-compatible. So a distributed build could not be GPL-2.0-or-later (**INFERENCE** — engineering reading, not legal advice) | Licence review + a stream-compatibility test: the title's streams are Oodle **2** V4 block headers, so the chosen SDK version must still decode them — that is a claim to test, not assume |
| **(a2) Licence the SDK for an offline import step only** | The shipped app contains no Oodle code; a licensed tool expands members once, into the user's **private** asset cache | Still needs a licence to *use* the SDK at all, and the licence must permit producing data for this use. Distribution stays clean because no Oodle code is distributed; the expanded bytes are game data and must stay in the private cache (never in the repo) | Licence review (ask Epic explicitly). Plus a coverage manifest: which of the 19,320 members were expanded, with hashes, and a fail-visible "unexpanded compressed member" path at load |
| **(b) Clean-room Mermaid/Kraken decoder** | No third-party code, no runtime dependency, GPL-2.0-or-later throughout | Compatible. But "clean room" has a hard precondition: the implementer must **not** study `0x8ac710`/`0x8ace10`/`0x8c1e60`/`0x89d3b0` or transliterate them. A decoder derived from those bodies is a derivative work of Oodle, not a clean-room one | Highest. There is no public Oodle format spec, and the only local oracle is the image we are not allowed to copy. Validation must therefore be **structural** (see §2.4) plus a second independent decoder or a licensed reference run; a decoder that merely produces plausible bytes is not acceptable |
| **(c) Do not support compressed shaders** | 14,971 raw members stay readable; 19,320 are rejected with an explicit error | None — this is the status quo | Trivial to verify: the reader already rejects them (E626). Cost: ~67 % of distinct shader programs (7,767 of 11,532 stems) can never be lowered, and per the measured sample 52 % of members (264/512) are compressed — the single largest ceiling in the shader path |

Note on (b): public reimplementations of the Oodle container exist in the wild, but each would need
its own licence review and none was inspected, downloaded or evaluated here. A vendored
reimplementation is a **fourth** route in practice — treat it as (b) with an upstream to attribute,
and apply the same acceptance test.

### 2.4 Acceptance test

`NOT RUN` — this is the test the implementing task must pass, stated now so the option cannot be
graded on a weaker one.

1. **Shape.** Input `member[0 : S-12)`; output buffer exactly `rawLen = trailer & 0x1fffffff` bytes.
2. **Raw prefix.** `out[0:8] == member[8:16)` and `u32le(out[0]) ∈ {1, 2}`. This is free and
   falsifies a decoder that consumes the wrong bytes as the first literal.
3. **Structural.** The full `rawLen` bytes parse as a `.csdr` record table through the same rules
   `0xb0ced0` uses (`0xe4`-byte records, count and stride in range, no overrun). Rendered as:
   a decompressed member that parses as a record table **with the exact `rawLen` bytes**.
4. **Whole-population, not a sample.** All 19,320 compressed members must decode: 16,328 Mermaid and
   2,992 Kraken, zero failures, zero length mismatches. Report the counts.
5. **Independent agreement.** If two implementations exist, their outputs must be byte-identical on
   the whole population; if only one exists, say so — "one decoder agrees with itself" is not
   evidence of correctness.
6. **Downstream.** At least the members used by the pinned material set must reach the existing GCN
   decoder unmodified; the decode must not be graded on a rewritten or truncated buffer.
7. **Fail-closed.** A truncated or corrupted member must produce a reported failure, not partial
   output — the retail code is fail-open (§2.2), so this is a port decision that must be deliberate.

**How the test differs by option.** **(a1)/(a2)** may use the licensed decoder as the reference the
other implementation is compared against, but items 1–4 still have to pass on the shipped path (a2
additionally owes the "every member was expanded" coverage manifest). **(b)** has no reference at
all, so items 2–4 carry the whole burden and item 5 must be reported as "no second implementation
exists" rather than skipped silently. **(c)** is graded on the rejection path instead: all 19,320
members must produce a named, non-silent error with zero bytes of output.

### 2.5 Excluded — not an option

| Excluded route | Why |
|---|---|
| Lift/translate `OodleLZ_Decompress` (`0x89d3b0`), `FUN_008c1e60`, `FUN_008cb610`, `FUN_008cbd30` or the Huffman/rANS helpers out of the game image into `native/` | Technically these are ordinary functions; **legally they are a proprietary third-party codec**. Reproducing them is the act the Oodle licence forbids, and it would put derived vendor code into a public GPL-2.0-or-later repository. This is the same act as extracting Oodle code, under a different name |
| Take the decoder from any Sony module (GPU driver, system library) | Forbidden target dependency, and the same licensing problem in a worse form |
| Ship an Oodle `.sprx`/`.prx`/binary blob as an "asset" | Disguising executable code as data; violates the asset-only import rule and the target |
| Commit a decompressed `.csdr` (or any expanded member) to the repository | Ships copyrighted game data; violates the payload rule. Expanded output belongs in the user's private cache |
| Treat "fuzzSafe = 1" as a licence argument | It is a decode-robustness flag, not a permission |

### 2.6 If the answer is (c)

The shader path stays honest rather than degraded: the reader rejects compressed members with a named
error, the corpus report states 19,320 unsupported, and G3 keeps its current PARTIAL status. Per
[VALIDATION](VALIDATION.md) G3, unknown or unsupported input must **fail** rather than render zero, so
"compressed member → explicit error" is an acceptable *product* answer even though it caps coverage.

---

## 3. ATRAC9 — streamed music

### 3.1 What is needed

A decoder for **ATRAC9** at the configuration the title actually uses: 48 kHz, 1/2/4/6 channels,
config words carrying the `0xFE` top byte, `256` samples per frame, `bits[15:5]+1` bytes per frame
across all channels, and `1 << ((cfg>>3)&3)` frames per stream block. For `music.sxd2` that is
**6 channels, 240 B/frame, 4 frames/block, 1024 samples/block, 960 B/block**, 179,549 blocks.

### 3.2 What is already pinned

| Item | Value |
|---|---|
| Selector | `WAVE` record format dword = `(0xff<<16) + (channels<<8) + format`; `format 0x42` → ATRAC9, dispatched by the 67-entry table at `0x12b5410` from `0x983ea0` |
| Corpus split | 830 `WAVE` records over five `.sxd1`: **566 ATRAC9**, 264 PSX/PS2 SPU ADPCM |
| Configuration source | `WAVE` record field `0x0A` (one `u32`, `count = 1`), located through arena offsets; **not** an id into any image table |
| `music.sxd1` config | **33/33 at `0xFE761DF0`**; the corpus uses six words total (`0xFE7007F0`, `0xFE740FE0`, `0xFE740FF0`, `0xFE7A17F0`, `0xFE761DE0`, `0xFE761DF0`) |
| Geometry for `music` | `bits[23:20] = 7` → 256 samples/frame; `bits[19:17] = 3` → 6 channels; `bits[15:5]+1 = 240` B/frame; `bits[4:3] = 2` → ×4 frames ⇒ **1024 samples / 960 B per block** |
| Asset agreement | derived block bytes equal the exact **gcd of every segment length** in each of the six config groups; whole payload `172,367,040 = 179,549 × 960` (0 remainder) |
| Rate | 48000 in every record (all five files); sample-rate index 7 |
| Duration (arithmetic) | `blocks × 1024 / 48000` = **3,830.4 s** of 6-channel material across the 33 waves |
| The interface a decoder must sit behind | `0x94b2c0` **cmd 0** (reads descriptor `+0x00`, `+0x04`, `+0xd8`, `+0xe8`, `+0xf8`, `+0x100`; never the payload pointer/length) and `0x9a8390` (per-channel block-offset function) |
| What is absent | the transform itself; no codec library is imported at all (34 libraries / 588 imports, no `libSceAudiocodec`, no `libSceAjm`) |

One fact is deliberately left as a decoder's job rather than a container claim: the payload entropy
census puts header-like bands at offsets 0, 242, 481 and 720 inside the 960-byte block — about
±2 bytes off the 240 grid. That phase offset is **unexplained** (E1076). A decoder must either explain
it or consume the block at exact 240-byte boundaries; it may not paper over it by re-aligning bytes.

### 3.3 Options

Licence statements below are **INFERENCE from the upstream projects' published licence terms**; none
was fetched, downloaded or vendored in this task. Re-verify at the pinned upstream commit before any
file lands ([ATTRIBUTION](ATTRIBUTION.md) rules 1–5).

| Option | What you get | Licence | Verification burden |
|---|---|---|---|
| **(a1) Vendor `libatrac9`** (Thealexbarney) | Full ATRAC9 decoder, written for exactly this family of streams; small, no platform dependency | **MIT** — GPL-compatible; keep the copyright header, add upstream/commit to `NOTICES.md` (created on the first vendored file) | Check it handles **6 channels** at 240 B/frame with the `0xFE` config we have (channel support must be verified, not assumed); then §3.4 |
| **(a2) Vendor FFmpeg's `libavcodec/atrac9dec.c`** | Single-file decoder, already the independent oracle this repo used for SPU ADPCM (E10 decoded a package ATRAC9 asset to finite PCM) | **LGPL-2.1-or-later**; LGPL-2.1 §3 permits converting to GPL-2.0-or-later, so it can be combined with this GPL-2.0-or-later work. Keep the file header, record the pinned commit, note any modification | Same, plus: FFmpeg is currently a **cross-check tool only** ("no FFmpeg binary ships with the port", AUDIO_FORMAT limits). Vendoring converts it into shipped code, so the licence review must cover the vendored file, not "FFmpeg is installed" |
| **(b) Clean-room ATRAC9** | No third-party code | Compatible | Strictly worse than (a1) for the same result: a permissive implementation exists, so clean-room buys nothing but risk. Only sensible if a licence review rejects both candidates |
| **(c) No streamed music** | SFX and ambient beds only; `music.sxd2`, `str_bgm`, `str_amb`, `str_boss`, `str_event` silent | None needed | Trivial to state, expensive to live with: 566 of 830 records lost, 1,714,959,920 B of the five `.sxd2` files, and the title has no music |

Not an option: **relying on the OS decoder.** G2 requires an explicit platform-decode query; ATRAC9
is not a format Apple documents for AudioToolbox/AVFoundation (**INFERENCE**; the query itself is
`NOT RUN`). Assume a native decoder is required.

### 3.4 Acceptance test

`NOT RUN` — the implementing task must pass all of these.

1. **Unit geometry.** One 960-byte stream block from a `music.sxd2` record start with config
   `0xFE761DF0` decodes to exactly **1024 samples × 6 channels** and consumes exactly **960 bytes**
   (4 frames × 240 B). Report the attempt count; a blind decode of an arbitrary window is not a test.
2. **Length closure.** The full `music.sxd2` payload decodes to exactly `179,549 × 1024 =
   183,858,176` samples per channel with **zero bytes left over** — the payload divides exactly, so
   any remainder means the geometry is wrong.
3. **Per-group agreement.** The other five config words must reproduce their derived block sizes
   (256/128/512/768/240 B) on real members; the asset-side gcd identity (§3.2) is the oracle.
4. **Independent agreement.** Compare against a second implementation (FFmpeg `atrac9`/`libatrac9`) on
   the same bytes and report bit-exactness or a stated tolerance, the way the `.bd` SPU ADPCM claim is
   reported (`PARTIAL`: correlation 0.99861, not bit-exact).
5. **Signal sanity.** No NaN/Inf; sample statistics clearly non-noise (the PCM-plausibility ratio
   `mean|Δ²|/mean|Δ|` was 1.75 for white noise and 0.2–0.6 for real PCM here); the six channels must
   not be identical.
6. **Phase explanation.** The 0/242/481/720 entropy bands must be accounted for by the decoder's own
   framing, not absorbed by shifting the input.
7. **Silence is failure.** A decoder that returns zeros, DC or repeated frames fails, even if the
   lengths match — [VALIDATION](VALIDATION.md) G2 lists "silence fallback, random PCM" as a fail.

Loop points, seeking and duration are **stream-layer** properties (`WAVE` record `+0x14`/`+0x18` are
`-1` in every `music` record, unproven) and are out of scope for the codec test.

**How the test differs by option.** **(a1)/(a2)** must also pass the licence/attribution review
before any of the above counts as a result — a decoder that decodes but has no `NOTICES.md` entry
has not shipped. **(b)** has no reference implementation, so item 4 becomes "no independent agreement
exists" and items 1–3 and 5–6 carry the whole burden. **(c)** is graded on a named absence: the audio
path must report "streamed music unsupported" rather than play silence, and G2 records the specific
missing codec.

### 3.5 Excluded

| Excluded route | Why |
|---|---|
| Call `libSceNgs2` (or any Sony module) at run time, or ship it | Forbidden target dependency; also the reason the codec is unavailable in the first place |
| Extract the transform from `libSceNgs2` | Forbidden dependency and the same third-party-code problem as Oodle |
| Treat the FFmpeg cross-check binary as the product path | The port ships no FFmpeg binary; converting that is option (a2), with attribution |

### 3.6 If the answer is (c)

Audio stays honest but incomplete: SFX banks (264 records + 25 `.hd`/`.bd` pairs) play, music does not,
and G2 keeps `NOT RUN` for the audio subsystem with the specific missing item named — "no streamed-music
codec" — rather than a generic gap.

---

## 4. What the port can do today without either decoder

Neither absence is a total block on the GPU or audio path. `ARTIFACT_VERIFIED` / `PARTIAL` as noted.

**GPU / shaders**

- **14,971 raw `.csdr` members** in `pak0`+`pak1`+`pakN` are readable today (15,072 corpus-wide with
  `cachedFiles.psarc`); the raw container reader is `ARTIFACT_VERIFIED` (E626, `GPU-01-CSDR-READER-01`).
- The raw members feed the existing GCN decoder: at the 512-member sample, **230 of 410 programs
  decode** (up from 200), with **0 lowered** to MSL because the per-shader ABI manifest and the
  lowering shape are still missing — separate blockers, not a codec one ([MSL_OFFLINE](MSL_OFFLINE.md),
  E1081).
- The pinned default material/shader pair, logo VS/PS, raw `.cmsh` geometry and the synthetic
  direct-Metal harness all work on the raw path (`GPU-02-*`, `GPU-03-*`, G3 PARTIAL).
- What is missing without Oodle is **reach**, not capability: 19,320 members and ~67 % of distinct
  shader stems are unreachable, and 52 % of the members in the measured 512-member sample are
  compressed.

**Audio**

- **264 PSX/PS2 SPU ADPCM `WAVE` records** and **25 `.hd`/`.bd` bank pairs** (25,921,328 body bytes,
  **1,620,083 frames**) are proven and decode to PCM; the decode matches FFmpeg `adpcm_psx` at
  correlation 0.99861 (`PARTIAL` sample-exactness).
- Containers (`SXDF`/`SXDS`, chunk grammar, `WAVE` offset table, tiling) are specified from bytes and
  from the pinned parser code, so a music path needs only the codec.
- `.sts_cp3` (290 × 120 B) are descriptors, not audio — no codec would change that.
- What is missing without ATRAC9 is **all music and BGM**: 566 of 830 records, five `.sxd2` files.

So: a person running the port today would hear sound effects and see raw-path shaders, and would
hear no music and lose two thirds of the shader programs. That is the honest baseline the decision
is being made against.

---

## 5. Licence mechanics for this repository (GPL-2.0-or-later)

The repository adopted **GPL-2.0-or-later** on 2026-11-01 ([LICENSE](../LICENSE),
[ATTRIBUTION](ATTRIBUTION.md), [PRIOR_ART](PRIOR_ART.md) §5.6). What that implies for each option
(**INFERENCE** — an engineering reading of the licence texts, not legal advice):

| Candidate | Licence | Distribute inside this GPL-2.0-or-later work? | Concrete obligation |
|---|---|---|---|
| Oodle SDK (Epic/RAD) | proprietary, negotiated | **No**, unless Epic grants GPL-compatible terms (GPL-2.0 §2(b) "licensed as a whole … under the terms of this License" and §7 "you may not distribute the Program at all") | Negotiate; or keep it out of the distributed artefact (option a2) |
| `libatrac9` | MIT | **Yes** | Keep the MIT notice; `NOTICES.md` entry with upstream, pinned commit, files, authors |
| FFmpeg `atrac9dec.c` | LGPL-2.1-or-later | **Yes**, via LGPL-2.1 §3 (relicense to GPL v2 or later) | Keep the file header; `NOTICES.md` entry; record modifications |
| Clean-room (either codec) | owner's own code | **Yes**, GPL-2.0-or-later | Record the clean-room provenance (what the implementer did and did not read) — otherwise the claim is unverifiable |
| An Oodle reimplementation from elsewhere | varies | depends entirely on its licence | Full review before vendoring; unlicensed code is never copied ([ATTRIBUTION](ATTRIBUTION.md) rule 4) |

Two mechanical points that are easy to get wrong:

1. **`NOTICES.md` does not exist yet.** It is created on the first vendored file ([ATTRIBUTION](ATTRIBUTION.md)
   rule 2). Whoever implements (a1)/(a2) creates it; a vendored decoder without a `NOTICES.md` entry
   fails the attribution rules.
2. **A vendored decoder is a dependency change**, so it also needs an `INSTALL_LOG.md`-style record of
   upstream identity, version/commit, checksum and removal boundary — same as any tool, except the
   code is now shipped rather than used.

---

## 6. Gate consequences

| Gate | Today | With Oodle (a1/a2/b) | With ATRAC9 (a1/a2/b) | With "no decoder" |
|---|---|---|---|---|
| G2 (audio fixtures) | `NOT RUN` | unchanged | music becomes decodable; still needs playback/voice/under-run work before a pass | SFX-only; cannot pass for the audio subsystem |
| G3 (direct Metal) | `PARTIAL` | removes the largest corpus ceiling (52 % of members); ABI manifest and lowering shape remain the blockers, so **G3 still cannot pass** | unchanged | coverage capped at raw members |
| G5/G6 | `NOT RUN` | prerequisite for full shader coverage | prerequisite for authentic music | vertical slice would ship without music |

Neither decision unblocks a gate on its own. Both are **coverage** decisions with licence content,
which is why they belong to the owner rather than to a worker.

---

## 7. How to re-check the facts in this document

All read-only; each re-derives its claims from the pinned ELF
(`local/re/eboot.elf`, SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`,
image base 0, RVA == address) and skips cleanly when private inputs are absent. **`NOT RUN` in this
task** — the counts below are the ones recorded in E1075/E1080/E1076 and were not re-executed here.

```sh
python3 -I tools/lift/check_csdr_loader_search.py            # loader search, Oodle SDK presence, .csdr registration
python3 -I tools/lift/check_csdr_oodle_dispatch.py           # 0x1aa2a0 six-case dispatch, single call site, tag semantics
python3 -I tools/lift/check_csdr_b0cc50_container.py         # member layout; --require-private for the corpus census
python3 -I tools/lift/check_atrac9_config_provenance.py      # config provenance, geometry, asset-side gcd identity
```

Private corpus probes (not publishable paths, listed so the recipe is reproducible):
`local/re/csdr_preheader_census.py`, `local/re/csdr_tail_census.py`, and
`local/validation/AUD-01-AUDIO-FORMAT-01/analysis/{check_sxd2_selector,check_atrac9_config}.py`.

Narrative sources: [CSDR_LOADER_SEARCH](CSDR_LOADER_SEARCH.md) §4.2–§4.3 (E1075/E1078),
[AUDIO_FORMAT](AUDIO_FORMAT.md) (E1069/E1074/E1076/E1077),
[the container checkpoint](checkpoints/2026-11-01-csdr-b0cc50-expansion.md) (E1080),
[the transform checkpoint](checkpoints/2026-11-01-aud01-atrac9-transform-not-in-image.md) (E1077).

---

## 8. Limits

- **No decode was attempted for either codec.** Every output-side number here (output length, sample
  count, block geometry) is derived from the pinned code and the pinned asset bytes, never from a
  decoded result. `NOT RUN` for both acceptance tests.
- The Oodle stream grammar below byte 16 is **not** described here and must not be inferred from this
  document; only the header fields and the closed-form container relations are pinned.
- Kraken and Mermaid are named from the image's own name table; the mapping is
  `STATIC_ANALYSIS_VERIFIED` for the table and strings, but "type 6 is exactly the Kraken codec"
  additionally relies on the vendor naming, and the Kraken/Mermaid **call split** inside
  `FUN_008c1e60` is not pinned (both codecs show identical container fields).
- ATRAC9 field *names* (frame bytes, superframe index, validation bit) are public-layout **INFERENCE**;
  the field *extractions* are observed. The ATRAC9 channel-element split is not recoverable from this
  arithmetic.
- Licence statements are **INFERENCE from published upstream terms**, not fetched or verified here.
  Nothing was installed, downloaded or vendored in this task.
- This document does not advance any gate, does not unblock either task, and does not constitute
  legal advice.
