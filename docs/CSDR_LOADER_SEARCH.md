# Where is the `.csdr` loader? (round 2: decompiler sweep, previous "no loader" withdrawn)

Task: `GPU-01-CSDR-DECOMPRESSOR-RE-01`. Question: does the pinned executable contain the code that
reads the `.csdr` container header, branches on the compressed prefix, and decompresses the member?

**Verdict: the previous "no loader exists in this executable" answer is withdrawn.** It rested on
three premises that this round falsified:

| Previous premise | Status now | Evidence |
|---|---|---|
| "the executable imports no decompression entry point, so it has no decompressor" | **FALSE** | The image statically links a complete **Oodle 2 LZ SDK** *and* **zlib 1.2.3**. Static linkage is why no import exists. §3 |
| "the executable contains no `csdr`/`OrbShdr` string; raw members must be handed to something else as opaque blobs" | **FALSE** | `.csdr`, `_shipping.csdr`, `_neo.csdr`, `_neoshipping.csdr`, the `csdr` 4CC, a `shaderTypeToLoad` property and an `EShaderTypeToLoad` enum are all in the image, with a variant-suffix table and a registered loader callback. §3.2 |
| "no `cmp $0x8c`/`$0x8c0a` exists, therefore nothing branches on the prefix" | **NOT DISCRIMINATING** | An exhaustive sweep of every scalar operand of all 3,661,345 instructions finds the prefix in **no** encoding at all. That is what a block header decoded as a bitfield looks like — so the predicate could never have found a loader, and its absence excludes nothing. §2 |

What this round established instead: the image contains a **generic, type-tagged Oodle decompression
dispatcher** — `FUN_001aa2a0`, six cases on the top three bits of its first argument, two of which
call `OodleLZ_Decompress` with different `fuzzSafe` policy (§4.1) — plus the `.csdr` naming,
variant-suffix table and registered loader callback (§3.2). At the end of that round the two were not
yet connected.

**Round 3 — the edge is closed (§4.2).** `FUN_001aa2a0` is
`bpe::Compression::Decompress(const CompressionHeader &, const void *, size_t, void *, size_t)` from
`Core/Util/Compression.cpp`; its only caller is `ResourceFactory::CreateResource` (`0x147160`); and the
`CompressionHeader` it is handed is the **last four bytes of the resource file**, checked against an
8-byte tag that sits immediately before them. Every one of the 19,320 compressed-prefix `.csdr`
members carries type **3** (`8c0a`) or **4** (`8c06`) in that trailer word, and both types dispatch to
`OodleLZ_Decompress` at `0x1aa426`; the 15,072 raw members carry type **0** and take the `memcpy`
case. **So yes: the one Oodle call site does expand `.csdr` members.** Nothing was decompressed —
this is a call-graph and container-format result (§4.2(f) states what it does not cover).

Details below are separated into what was **established** and what was **not**. Everything is
`STATIC_ANALYSIS_VERIFIED` against `local/re/eboot.elf`
(SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`, image base 0) unless a
row says otherwise. **No guest instruction was executed, no member was decompressed, and nothing
here is a rendering claim.**

---

## 1. Re-derived corpus facts (this round's own measurement)

Independent re-derivation over the three pinned archives, reading only each member's first 24 bytes
and its TOC size. Counts only; no member bytes are stored.

| Fact | Value |
|---|---|
| `.csdr` members in `pak0`+`pak1`+`pakN` | **34,291** (matches the figure the task carried) |
| compressed-prefix members | **19,320** = **56.34 %** |
| raw (`0100`/`0200`) members | **14,971** = 43.66 % |
| distinct 2-byte prefixes in the whole corpus | **exactly 4**: `8c0a` 16,328 · `0100` 8,077 · `0200` 6,894 · `8c06` 2,992 |
| `cc*` prefixes | **0** in every archive scanned — the earlier report's single `cc0a` member is not in any of the six |
| per archive (compressed / raw) | `pak0` 2,270 / 1,039 · `pak1` 1,442 / 1,070 · `pakN` 15,608 / 12,862 |
| other archives | `pak2` and `pak3` hold **no** `.csdr`; `cachedFiles.psarc` holds **101**, all raw `0100` |

### 1.1 Header fields, whole population (not a 84-member sample)

| Field | Rule | Holds |
|---|---|---|
| `[0]` | always `0x8c` | 19,320 / 19,320 |
| `[1]` | only `0x0a` (16,328) or `0x06` (2,992) | 19,320 / 19,320 |
| `u16be[2:4]` | `== floor(size / 256)`, or exactly one less | 18,009 exact, 1,311 one less, **0 otherwise** |
| `u16be[6:8]` | `== size - 20` | **19,320 / 19,320** |
| `u32be[8:12]` | **exactly 2 distinct values** over the whole population: `0x01000000` (4,238) and `0x02000000` (15,082) — a single bit in the top byte, i.e. a flag word | 19,320 / 19,320 |
| `u32be[12:16]` | **exactly 4 distinct values**: `0x00000000` (4,008), `0x01000000` (226), `0x02000000` (15,084), `0x03000000` (2) — a two-bit flag word | 19,320 / 19,320 |
| `u32be[16:20]` | an offset-shaped value, median `0x00107000`, sparse outliers up to `0x4057…` | — |
| member size | min 1,412 · median 4,736 · max 20,397 B (`8c0a` median 5,278; `8c06` median 3,041) | — |

Two cross-checks, both `STATIC_ANALYSIS_VERIFIED`:

- **The header carries no compression ratio, and its middle is flags, not sizes.** `u16be[6:8]` is the
  *file* length minus 20, `u16be[2:4]` is a count of 256-byte units of the same file, and `[8:16]`
  are two small flag words (§ table). A decompressor therefore has no explicit output length in this
  header; it must infer one (e.g. `u16be[2:4] * 256`) or take it from the caller. This contradicts
  the previous note's framing of `[2:4]` as a "block count hint" — on this corpus it is a *size* in
  256-byte units with no ratio information attached.
- **No `.csdr` member begins with `R`/`r`**, so the resource-descriptor tag in §3.2 is not a file
  magic for this corpus.

### 1.2 The `_shipping` variant is a duplicate, and the old "50/50 across builds" argument was void

Cross-tabulating member name against container class gives four buckets with **exactly equal
counts and identical size distributions**:

| | `.csdr` | `_shipping.csdr` |
|---|---:|---:|
| compressed | 9,660 | 9,660 |
| raw | 7,536 | 7,536 |

There are **no** `_neo.csdr` or `_neoshipping.csdr` members at all. Sampling 11,274
`X.csdr`/`X_shipping.csdr` pairs shows **11,272 byte-identical** and 2 different. The earlier
note's "compressed vs readable splits 135/118 among `_shipping` and 129/130 among non-shipping,
therefore the compressed half is live" was measuring this duplication: the split is a property of
the *shader*, reproduced under both names, not evidence about build variants.

Distinct shader stems: 7,767 compressed, 3,799 raw, **34 in both** — the two classes are almost
disjoint by name and interleaved in the same directories (e.g. both appear under
`materials/!cookedshaders/version57/<material>/_ps4/`). So the compressed set is a *different
population of shader programs*, not a second copy of the raw set. Whether those programs are ever
selected is still open (§6).

---

## 2. Predicate sweep 1 — every scalar operand of every instruction

Script: `tools/re/ghidra/sweep_constants.py`. Run through `tools/re/ghidra/run_dump.py` against the
persistent project `local/ghidra/persistent-20261020` (`/eboot.elf`, `x86:LE:64:default`, base 0).

| Coverage | Value |
|---|---|
| instructions visited | **3,661,345** |
| inside a function body | 3,558,451 |
| **not** inside any function body | **102,894** (2.8 % — this is the coverage exception to carry forward) |
| memory blocks | 9 |
| functions | 38,845 |

Predicates (every encoding the prefix could plausibly be compared as): `0x8c`, `0xcc`; `0x8c0a`,
`0x8c06`, `0xcc0a`, `0xcc06`; the little-endian 16-bit forms `0x0a8c`, `0x068c`, `0x0acc`, `0x06cc`;
the 32-bit forms `0x8c0a0000`/`0x8c060000`/`0xcc0a0000`/`0xcc060000`; `0x8c0a`, `0x8c06`; the
resource tag candidates `0x52445343` (`RDSC`), `0x72647363` (`rdsc`), `0x43534452` (`CSDR`),
`0x63736472` (`csdr`); `0x42524f` (`ORB`), `0x5342524f` (`ORBS`), `0x5362724f` (`OrbS`).

| Predicate | Hits | Reading |
|---|---:|---|
| `0x8c0a`, `0x8c06`, `0xcc0a`, `0xcc06` | **0** | the 16-bit prefix is never an immediate |
| `0x8c0a0000`, `0x8c060000`, `0xcc0a0000`, `0xcc060000` | **0** | …in no 32-bit half either |
| `0x8c` (`0x8C` same set) | **≥60** (capped) | **every** hit is a struct displacement `[reg+0x8c]` (`CMP/MOV/VMOVSS/IMUL/IMM`); not one is a value compare |
| `0xcc` | ≥60 (capped) | same shape |
| `csdr` (`0x63736472`) | **2** | the two resource-type registrations, §3.2 |
| `RDSC` / `rdsc` / `CSDR` / `ORB` / `ORBS` / `OrbS` | **0** | the descriptor tag is stored as a wider immediate (§3.2) |
| count-only reference counts | `0x100` 3,771 · `0x14` 8,215 · `0x16` 857 | not discriminating |

**Reading:** the prefix is never compared, in any encoding, anywhere in 3.66 M instructions. A loader
that validates `0x8c` cannot exist in the shape the previous search assumed, and a loader that hands
the bytes to an LZ decoder would never need such a compare. The predicate is therefore **vacuous for
this question** in both directions.

---

## 3. What the executable *does* contain

### 3.1 A complete, statically linked Oodle 2 LZ SDK

| Observation | Value |
|---|---|
| symbol-table labels matching `oodle` (case-insensitive) | **178** |
| …matching `OodleLZ` | **24** |
| API labels present | `OodleLZ_Decompress`, `OodleLZ_Compress`, `OodleLZ_Compressor_to_DecodeType`, `OodleLZ_CompressContext_Alloc/_Free/_Reset`, `OodleLZ_CompressWithContext`, `OodleLZA/LZB/LZBLW/LZNA/LZNib_CompressFast_*`, `rrHuffman_PackCodeLensNew/_UnPackCodeLensNew`, `rrLZHLW_CompressFast/_CompressNormal_New/_CompressVeryFast`, `newLZ_decode_chunk_phase1/2`, `LZBLW_DecodeOneQuantum`, `LZNib_DecodeOneQuantum` |
| codec family names | `LZHLW`, `LZNIB`, `LZB16`, `LZBLW`, `LZNA`, `Kraken`, `Mermaid`, `BitKnit`, `Selkie`, `Hydra`, `Leviathan` |
| embedded source paths | **25 distinct** `v:\devel\projects\oodle2\core\*.cpp` — the whole codec set, including `newlz.cpp`, `newlzf.cpp`, `newlzhc.cpp`, `lza.cpp`, `lzb.cpp`, `lzblw.cpp`, `lzna.cpp`, `lznib.cpp`, `oodlebitknit.cpp`, `rrrans64dual.cpp`, `rrhuffmandecode.cpp`, `rrhuffman.cpp`, `rrlzh.cpp`, `rrlzhlw.cpp`, `rrlzh_lzhlw_shared.cpp`, `oodlelzcompressors.cpp`, `oodlelzpub.cpp` |
| `OODLE ERROR` / `bad parameter` assertion strings | **99** + **13** |
| seekable/chunked API evidence | `OodleLZ_GetCompressedStepForRawStep`, `seekChunkLen`, `seekChunksIndependent`, `st: %d x %d seekchunks` |

The decompressor entry this task was looking for **is in the image**: `OodleLZ_Decompress` at RVA
`0x89d3b0` — 1,676-byte Ghidra body, 332 decompiled lines, 13 callees, and the `__FUNCTION__` tag
`"OodleLZ_Decompress"` plus its three parameter asserts (`decodeTo != (0)`, `compBuf != (0)`,
`threadPhase == …ThreadPhase1 || …ThreadPhase2`) referenced from inside its own body. Its heavy
lifting is `FUN_0089bcb0` (5,841 B).

**Also present: zlib 1.2.3** — `deflate 1.2.3 Copyright 1995-2005 Jean-loup Gailly` at `0xfae520`
and `inflate 1.2.3 Copyright 1995-2005 Mark Adler` at `0xfb3c60`, with the `zerr` strings
(`need dictionary`, `stream end`, `file error`, `stream error`). Both libraries are statically
linked, which is exactly why the import table shows no decompression entry point.

### 3.2 The executable knows `.csdr` by name, and registers a loader for it

| Item | Address / value |
|---|---|
| string `.csdr` | `0x12cfc1c` (referenced by `LEA` at `0xb0cbe2`, inside `FUN_00b0cb80`) |
| strings `_shipping.csdr`, `_neo.csdr`, `_neoshipping.csdr` | `0x12cfbf1`, `0x12cfc00`, `0x12cfc0a` |
| variant-suffix table (`.data.rel.ro`) | `0x15d5820` = index 0 → `NULL`, 1 → `_shipping.csdr`, 2 → `_neo.csdr`, 3 → `_neoshipping.csdr` |
| property name | `shaderTypeToLoad` at `0x12cfbe0` |
| enum | `EShaderTypeToLoad` at `0x12cfcd2`; values `Debug`=0, `Shipping`=1, `PS4Neo_Debug`=2, `PS4Neo_Shipping`=3 (built by the string→int table in `FUN_00b0d580`, `0xb0d580`) |
| resource type 4CC | `csdr` (`0x63736472`) registered at `FUN_00b0d3e0` call site `0xb0d464` |
| sibling type | `cslt` (`0x63736c74`) at `FUN_00b0e3a0` call site `0xb0e403`; the same function registers the `ShaderList` string, so the two are associated (inference, not proven) |
| `csdr` descriptor tag | 8-byte constant **`0x1343534452`** → byte image `52 44 53 43 13` (`RDSC` + `0x13`). *(Corrected in §4.2(e): this line previously read `0x0000000134534452` / `…43 01`; `movabsq $0x1343534452` at `0xb0d420`, and the corpus trailer agrees byte for byte.)* |
| `csdr` descriptor suffix | `_ps4` (`0x12cfc29`) |
| `csdr` loader callback | `FUN_00b0cc50` (624 B) stored into the descriptor at `0xb0d464`'s call |
| path builder | `FUN_00b0cb80` (201 B): reads `table[shaderTypeToLoad]`, concatenates with the literal `.csdr` through `FUN_000102b0`, stores the result |

`FUN_00b0cc50` is a **two-phase** accessor (`*param_2` is the phase): phase 0 derives the variant
index from the `shaderTypeToLoad` global `*DAT_03305d80` combined with a boolean from
`FUN_0003f4e0()` — modes 2 and 3 double the index and add the Neo flag, giving exactly the 0..3
table domains — then calls `FUN_00b0cb80` to build the path; phase 1 returns the request field at
`+0x70`. Because the corpus has no `_neo`/`_neoshipping` members (§1.2), variants 2 and 3 resolve to
nothing in this build, so the shipped build is the non-Neo one.

**This is the `.csdr` *loader*, in the sense the previous note said did not exist**: the executable
derives the file name including the variant suffix, opens it as resource type `csdr`, and asks a
registered callback for it. What it does **not** yet show is the byte-level expansion (§4, §6).

---

## 4. Predicate sweep 2 — byte-level `call`/`jmp rel32` census (disassembler-independent)

Why a second sweep: a Ghidra reference query answers "did the disassembler record a reference", not
"does a call exist". This is the E1065 failure mode, and it bit this task.

Method: decode every `E8`/`E9` rel32 in the image-mapping segments directly from the file bytes, map
each source to a VA, keep the ones whose target lands inside `0x890000..0x8c0000` (the Oodle code
region), then classify each source against Ghidra's coverage.

| Metric | Value |
|---|---|
| image bytes scanned | 48,053,336 |
| candidate rel32 sites | 315,268 |
| in-window sites | 1,307 |
| distinct in-window targets | 585 |
| Ghidra-reported inbound call targets into the same window | 109 |
| **sites Ghidra has no reference for** | **> 476** |

Two independent implementations agree on the decisive case: `tools/re/ghidra/sweep_rel32_calls.py`
does the same decode from Ghidra's own memory (77,245,752 initialised block bytes, 352,535 candidate
sites, which is larger because Ghidra's blocks include zero-filled ranges that hold no code) and
reports, for the window `0x89d3b0..0x89d3b1`, **1 in-window site, 1 distinct target,
`0x1aa426`, class `OUTSIDE`**.

Most of the extra sites are mid-instruction false positives (a `0xE8` byte inside another
instruction's immediate), which is why the classification step matters: a site is only real when the
source is an instruction boundary. The important case is the one that is **not** a false positive:

### 4.1 `OodleLZ_Decompress` has exactly one inbound call, inside a Ghidra hole

`tools/re/ghidra/probe_containing.py` on that address still reports the E1065 shape, so the hole was
made visible with `tools/re/ghidra/probe_disasm_range.py` (disassembles in memory only; the project
is never saved):

```
0x1aa351  (undecoded by auto-analysis; body hole between FUN_001aa2a0's two ranges)
…
0x1aa3fb  MOV RDX, [RBP-0x60]
0x1aa3ff  MOV R9D, 0
0x1aa405  MOV RDI, R15
0x1aa408  MOV RSI, R12
0x1aa40b  MOV RCX, RBX
0x1aa40e  PUSH 0x3
0x1aa410  PUSH qword ptr [0x02ee6748]
0x1aa416  PUSH qword ptr [0x02ee6720]
0x1aa41c  PUSH 0x0            (×5)
0x1aa426  CALL 0x0089d3b0     ; == OodleLZ_Decompress
0x1aa42b  ADD RSP, 0x40       ; 8 stack slots
```

That is a 14-argument call — six register arguments plus eight stack slots — which matches the
14-parameter signature Ghidra infers for `FUN_0089d3b0` and the public Oodle
`OodleLZ_Decompress(compBuf, compBufLen, rawBuf, rawLen, fuzzSafe, checkCRC, verbosity,
decBufBase, decBufSize, threadPhase, callback, userData…)` shape. The two non-zero stack slots are
`.bss` globals `0x02ee6748` and `0x02ee6720`, both initialised in `FUN_001aa2a0`'s prologue (from
`FUN_0089b980(0xffffffff,-1)` and `FUN_00017090()` respectively).

`0x89d3b0` has **no other inbound reference of any kind** in the image: no `E8`/`E9` rel32 anywhere
else targets it, and the 8-byte little-endian value `0x000000000089d3b0` occurs **zero** times, so
there is no function-pointer table entry for it either.

#### The hole is the switch body of `FUN_001aa2a0`

Ghidra's decompilation of `FUN_001aa2a0` ends at an unrecovered indirect jump
(`WARNING: Could not recover jumptable at 0x001aa34f. Too many branches`). The table it failed to
recover is a relative-offset switch table at **`0xfadd4c`** — the same address the decompiled C uses:

| tag = `*param_1 >> 29` | case entry | lands at | what it does |
|---:|---|---|---|
| 0 | `0x1aa35c` | `0x1aa35c` | log/lookup path, then `CALL 0xf8f470` (thunk), then the epilogue |
| 1 | `0x1aa3dc` | `0x1aa3dc` | `CALL 0x1b9130`, then the epilogue |
| 2, 3, 4 | `0x1aa351` | `0x1aa351` | `MOV R8D,1` → falls into the Oodle call |
| 5 | `0x1aa3f5` | `0x1aa3f5` | `MOV R8D,0` → falls into the Oodle call |

(six real entries plus two trailing words holding `0x3f800000`, i.e. the next table's data, which is
why the guard is `5 < tag`). So `0x1aa351..0x1aa42e` **is** part of
`FUN_001aa2a0`: the function's true body is `0x1aa2a0..0x1aa468` (456 B), of which Ghidra claimed
only 235 B in two ranges, and the epilogue at `0x1aa42f` restores `RBX/R12/R13/R14/R15/RBP` — the
same registers the case bodies use, which is why the truncated decompilation showed them unset.

`FUN_001aa2a0` therefore **is a type-tagged decompression dispatcher**: the tag is the top three bits
of its first argument, and tags 2/3/4 and 5 both call `OodleLZ_Decompress`, differing only in `R8D`
(`1` vs `0`). Reading the six register arguments against the public Oodle parameter order gives
`compBuf = R15`, `compBufSize = R12`, `rawBuf = [RBP-0x60]`, `rawLen = RBX`, `R8D = fuzzSafe`,
`R9D = checkCRC = 0` — **that mapping is an inference from the public signature order, not proven by
this analysis**; what is proven is the 14-argument, 8-stack-slot call shape above.

**What this does and does not prove.** It proves a *generic, tag-dispatched Oodle decompression entry
point exists in game-reachable code* — not a library-internal one — and that Ghidra's "0 callers" was
a coverage artefact hiding four of that function's six cases. On its own this paragraph does **not**
prove the entry point is reached from `.csdr`; §4.2 closes that edge with the caller, the trailer
format and a whole-corpus census.

### 4.2 The owner is `bpe::Compression::Decompress`, and it *is* the `.csdr` expander

Everything below is `STATIC_ANALYSIS_VERIFIED` against the pinned image and the pinned corpus. **No
member was decompressed, no guest instruction ran, and nothing here is a rendering claim.**

#### (a) The function: extent from the bytes and from `.eh_frame`

| Fact | Value |
|---|---|
| entry (`push rbp; mov rbp,rsp; push r15,r14,r13,r12,rbx; sub rsp,0x38`) | `0x1aa2a0` |
| single epilogue `0x1aa42f..0x1aa463` (restores `RBX/R12/R13/R14/R15/RBP`, stack-cookie compare, `ret` at `0x1aa463`) | `0x1aa42f` |
| cold tail `call __stack_chk_fail` (`0x1aa464`, 5 B) | ends `0x1aa469` |
| **`.eh_frame` FDE** (file offset `0x135cccc`: `length 0x20`, `cie_ptr 0x178`, `pc_begin 0x1aa2a0`, `pc_range 0x1ca`) | **`[0x1aa2a0, 0x1aa46a)` = 458 B** |
| hot body, prologue → `ret` | **452 B** |

So the extent is **not** only inferred from control flow: an FDE really does claim the address — it
starts at `0x1aa2a0` and spans 458 bytes, which *is* `0x1aa351..0x1aa42e` plus the prologue, the
other four case bodies and the epilogue. What Ghidra lacked was not the boundary but the body:
auto-analysis never followed the switch table at `0xfadd4c`, so `FUN_001aa2a0` was split into two
discontiguous ranges totalling 235 B and the four case bodies (`0x1aa351`, `0x1aa35c`, `0x1aa3dc`,
`0x1aa3f5`) were never decoded. The `0x1aa2a0..0x1aa468` / 456 B figure in §4.1 counts to two bytes
inside the last instruction; the measured FDE range is 458 B and the last instruction ends at
`0x1aa469`.

#### (b) The identity: `bpe::Compression::Decompress`, `Core/Util/Compression.cpp`

The body carries its own assert metadata, all three literals referenced from inside the function:

| Address | String |
|---|---|
| `0xfadd6c` | `d:\dev\cp10\Main\Code\BPE\Source\Core\Util\Compression.cpp` |
| `0xfadda7` | `static size_t bpe::Compression::Decompress(const bpe::Compression::CompressionHeader &, const void *, size_t, void *, size_t)` |
| `0xfade25` | `Extracting a non-compressed buffer, use Compression::GetUncompressedData() instead!` |

The same translation unit supplies `0x1aa0b0` (`CompressionHeader::CompressionHeader()`, `*h = 0`),
`0x1aa210` (`0x1fffffff < *h`, the "is compressed" test), `0x1aa260` (uncompressed size), and
`0x1aa160`, which maps the *same* type field onto compressors (`0x1b4130` for type 1, `0x89ed10` for
types 2–5) — the compression side of the same wrapper. So the one Oodle call site in the image lives
in the engine's **generic compression wrapper**, not in Oodle's own code and not in a shader-specific
reader.

#### (c) The six cases, resolved to codecs

| type = `*h >> 29` | case entry | what it does |
|---:|---|---|
| 0 | `0x1aa35c` | logs `0xfade25`, then `memcpy(dst, compBuf, *h & 0x1fffffff)` (`0xf8f470` is the `memcpy` thunk) — **stored, uncompressed** |
| 1 | `0x1aa3dc` | `0x1b9130(dst, &len, compBuf, compLen)` → `inflateInit_(&strm, "1.2.3", 0x58)` + `inflate(&strm, 4)` — **zlib 1.2.3** |
| 2, 3, 4 | `0x1aa351` | `MOV R8D,1` → `OodleLZ_Decompress(..., fuzzSafe = 1)` |
| 5 | `0x1aa3f5` | `MOV R8D,0` → `OodleLZ_Decompress(..., fuzzSafe = 0)`; `0x1aa260` case 5 also asks Oodle (`0x89dad0`) for the size instead of masking it out of the header |

The argument mapping is now **proven**, not inferred from the public signature: the case body loads
`RDI = R15` (incoming `RSI`), `RSI = R12` (incoming `RDX`), `RDX = [RBP-0x60]` (incoming `RCX`),
`RCX = RBX` (`*h & 0x1fffffff`), `R9D = 0`, and pushes eight slots whose last-pushed (first stack
argument) is `0`; the two non-zero slots are `[0x2ee6720]` (`decoderMemory`, an allocation from
`FUN_00017090`) and `[0x2ee6748]` (`decoderMemorySize`, `FUN_0089b980(-1,-1)`), and the top slot is
`3` = Oodle's `ThreadPhase_Unthreaded`. `checkCRC` is 0.

#### (d) The single caller is `ResourceFactory::CreateResource`

`0x1aa2a0` has exactly **one** direct call in the image, at `0x147257`, inside `FUN_00147160` =
**`ResourceFactory::CreateResource(uintptr)`** (own assert metadata: `0xfa97d1`
`void ResourceFactory::CreateResource(uintptr)`, `0xfa944a`
`…\Engine\Resource\ResourceFactory.cpp`). The decompiled body is unambiguous:

```c
lVar1 = *param_2;                              /* ResourceWrapper                       */
lVar6 = FUN_0014c000(param_2);                 /* compBuf = loader+0x90  (whole file)    */
lVar9 = param_2[0x10];                         /* compLen = loader+0x80  (= filelen-12)  */
if ((*(byte *)(lVar2 + 0x70) & 8) != 0) {      /* wrapper flags                          */
  if (FUN_001aa210(param_2 + 0x1e)) {          /* IsCompressed(loader+0xf0)              */
    uVar7 = FUN_001aa260(param_2 + 0x1e);      /* rawLen = trailing word & 0x1fffffff    */
    lVar8 = FUN_00012820(uVar7, 1, *(u32 *)(lVar2 + 0x54), 0);   /* 8-byte aligned alloc */
    lVar9 = FUN_001aa2a0(param_2 + 0x1e, lVar6, lVar9, lVar8, uVar7);
```

The expanded buffer is then wrapped by `FUN_0004d470(&buf, owner, data, size, flag)` (which stores
`&PTR_LAB_01595dc0` as the buffer object's vtable) and handed to `FUN_00144880`, which builds the
resource through the type's object ctor — for `csdr` the descriptor's `+0x78` entry `FUN_00b0d4a0` →
`FUN_00b0ced0` (allocates 0x88 B and parses a table of `0xe4`-byte shader records). `FUN_0008dc30` at
`0x1473ac` is that buffer object's destructor, not part of the data path.

#### (e) Where `loader+0xf0` comes from: a 12-byte trailer, proven on the whole corpus

`ResourceFactory::PrepareNextResourceToLoad` (`0x146ea0`) reaches `FUN_0014aee0(loader, typeEntry)`,
which opens the file named by the type's `+0x48` callback — for `csdr` that is `FUN_00b0cc50`, and the
call is visible in raw bytes at `0x14ba2f` (`callq *0x48(%rcx)` with `rcx = *(r15+8)`, the type
entry). `movq $0xc, 0xe0(%rbx)` at `0x14af2d` then sets a **12-byte** tail for any type whose
descriptor flags carry bit 3; the `csdr` descriptor sets that bit (`orb $0x8` at `0xb0d447`).

`FUN_0014b980` (read completion) executes, verbatim:

```
0x14b9da  leaq 0xe8(%r15),%rax      ; dst = &loader+0xe8
0x14bc35  movq 0xe0(%r15),%r12      ; 12
0x14bc50  subq %r12,%rsi
0x14bc53  addq 0x90(%r15),%rsi      ; src = buffer + (bytesRead - 12)
0x14bc5a  callq 0xf8f470            ; memcpy(dst, src, 12)
0x14bc63  movq 0xe8(%r15),%rsi      ; file tail tag
0x14bc6c  movq 0x28(%rax),%rdi      ; typeEntry->tag
0x14bcb4  subq %r12,%r13
0x14bcb7  movq %r13,0x80(%r15)      ; payloadSize = filelen - 12
```

So the last 12 bytes of every resource file are `{u64 tag; u32 CompressionHeader}`, and the 8-byte
prefix of that tail is checked against the registered tag. An independent, whole-population census
(`local/re/csdr_tail_census.py` over `scripts/psarc.py`; **34,392** members = the 34,291 of §1 plus
the 101 `cachedFiles.psarc` members) gives:

| Measurement | Result |
|---|---|
| members whose bytes `[len-12, len-4)` equal the tag `0x1343534452` | **34,392 / 34,392** |
| trailing word type field | type **0** = 15,072 · type **3** = 16,328 · type **4** = 2,992 |
| prefix ⇔ type | `0100` (8,178) and `0200` (6,894) → **0**; `8c0a` (16,328) → **3**; `8c06` (2,992) → **4** — 1:1, no exception |
| type 0: `size == len-12` | **15,072 / 15,072** |
| type 3/4: `size > len-12` | **19,320 / 19,320** (pak1 ratio: type 3 min 1.135 / median 1.343 / max 1.804; type 4 1.311 / 1.389 / 1.643) |

**Correction to §3.2 and E1072:** the descriptor tag is **`0x1343534452`**, byte image
`52 44 53 43 13` — `movabsq $0x1343534452` at `0xb0d420`. The earlier `0x0000000134534452` /
`…43 01` dropped a `3`. The corpus trailer and the image constant now agree byte for byte.

#### (f) The answer, and the argument characterisation the task asked for

**Yes — the one `OodleLZ_Decompress` call site expands `.csdr` members**, on the following chain,
every link direct and byte-verified:

`.csdr` member → `ResourceFactoryResourceLoader` reads it → 12-byte tail copied to `loader+0xe8`
(tag + `CompressionHeader`) → `payloadSize = filelen-12` → `CreateResource` (`0x147160`) sees
`IsCompressed(loader+0xf0)` → `Compression::Decompress` (`0x1aa2a0`) → type 3/4 → case `0x1aa351`
→ `CALL 0x89d3b0` at `0x1aa426`.

| Question | Answer |
|---|---|
| which argument is the compressed buffer | `RDI` = the loader's read buffer `loader+0x90`, i.e. the member's bytes `[0, filelen-12)` — **from offset 0** |
| which is the output | `RDX` = a fresh `FUN_00012820(size, 1, 8, 0)` allocation, exact size, no slack |
| where the output length comes from | the **trailing** word: `rawLen = RCX = *h & 0x1fffffff` (bytes `[filelen-4, filelen)`) |
| do the leading header fields feed it | **No.** `u16be[2:4]`, `u16be[6:8]`, `[8:16]` and `[16:20]` are never read by `Decompress`, by its caller, or by `IsCompressed`/`GetUncompressedSize`; they are ordinary bytes inside the `compBuf` range. The only field the path reads is the trailing word |

**What is deliberately left open.** Bytes `[0:16)` of a compressed member are *structured*, not
codec noise: over pak1's 1,442 compressed members `[8:12]` takes exactly two values and `[12:16]`
two (co-varying), and `[6:8]` equals `len-20` for every member in the corpus (§1.1). Since the
compiled caller passes the range from byte 0, whether the Oodle stream begins at byte 0 or after
that header is a **codec-format** question: it cannot be settled without an Oodle implementation, and
it is not needed to place the call on the path. A future decoder must reproduce whatever the retail
caller does, and this document does not guess which.

**Not claimed:** that any specific shader program is requested at run time (§1.2's "is the
compressed set live" question is untouched); that any member decompresses; anything about rendering.
The §6.6 "sector-structured container" hypothesis is **withdrawn as an explanation of the compressed
members**: the trailing word names a codec and the dispatcher sends types 3/4 unconditionally to
`OodleLZ_Decompress`.

---

## 5. What the GNM command-buffer machinery does *not* do

`docs/GPU_COMMAND_SURFACE.md` §6 describes the title's own PM4 emitters: 293 functions in
`0x865fa0..0x8852a0` writing type-3 headers into one growable buffer (`+0x08` end, `+0x10` cursor,
`+0x18` grow callback, `+0x20` argument, `+0x28` flags). Checked this round:

- The growable-buffer machinery **copies payload dwords** (`payload_dwords == count + 1`, `memcpy`
  for `inline_data` families) and never resizes or reinterprets them. Nothing in the grounded table
  carries a whole program image: the largest fixed payload is 6 dwords and the only unbounded family
  (`0x10`) is used for 1..63-dword inline data, far below the 1.4–20 KB of a `.csdr` member. So the
  command buffer is not the copy/expand path this task hypothesised it might be.
- The shader bytes reach the driver through `libSceGnmDriver` imports (`sceGnmSetPsShader350`,
  `sceGnmSetVsShader`, `sceGnmUpdatePsShader350`, …), which are `jmp *GOT` thunks at
  `0xf903e0..0xf90a40`. Those thunks are external and are **not** part of this search's address
  space; whether the game hands them a raw or an expanded buffer is unresolved (§6).

So the command buffer is **not** the copy/expand path this task hypothesised it might be
(`STATIC_ANALYSIS_VERIFIED` for the writer census, which is a re-read of the already-grounded table;
the "no shader blob" statement is a structural reading of the same table, not a new sweep).

---

## 6. How a loader could still have been missed (the honest residual)

The two sweeps above are strong but neither is a proof of absence. These were the concrete remaining
holes, in the order they had to be closed. **Items 1, 3 and 6 are now closed by §4.2**; the rest
stand, and the end-to-end path is `STATIC_ANALYSIS_VERIFIED` but never executed.

1. ~~**`FUN_001aa2a0`'s own callers and tag semantics.**~~ **CLOSED (§4.2).** Exactly one caller,
   `ResourceFactory::CreateResource` at `0x147257`; the tag is `bpe::Compression::CompressionHeader`
   `{type:3, size:29}` and the six cases are None / zlib / Oodle fuzzSafe=1 (×3) / Oodle fuzzSafe=0.
2. **102,894 instructions outside any function body** (§2) plus the undecoded ranges that
   `probe_disasm_range.py` exposes. Any loader that lives entirely in such a range is invisible to
   `getFunctionContaining`, to `getReferencesTo`, and to the decompiler. The rel32 census in §4 was
   immune to this for *call edges*, but a loader reached only through a `jmp` into a table, or
   reached by `call rax` after an indirect load, would still hide.
3. ~~**Indirect calls.**~~ **CLOSED for this path (§4.2).** The per-resource-type pointer is real —
   the loader calls `typeEntry+0x48` (`0x14ba2f`) to get the file name — but it produces a *path*,
   not a decompressor. The decompression is a direct `CALL 0x89d3b0` from a direct call of
   `0x1aa2a0`, so no link on the `.csdr` → Oodle chain is indirect.
4. **`.bss` globals.** The two stack arguments come from `0x02ee6720`/`0x02ee6748`, which have no
   file image. Their initialisers are known (`FUN_001aa2a0`), but any *other* writer would be
   invisible to a byte scan of the image. §4.2 identifies them as `decoderMemory` and
   `decoderMemorySize`; their *values* are still runtime state.
5. **A second decompressor.** Oodle and zlib are both present; nothing rules out a third,
   title-specific block codec written in-house. No sweep in this round searched for one, because
   there is no predicate for "a codec I have not seen". §4.2 narrows this: the `.csdr` trailer names
   only types 0/3/4, and those map to `memcpy` and Oodle, so a second codec would have to be inside
   `OodleLZ_Decompress` itself.
6. ~~**The `[0]==0x8c` byte may not be a codec tag at all.**~~ **WITHDRAWN (§4.2).** The hypothesis
   was that `[8:16]`'s flag words and `[16:20]`'s offset-sized value would suit a sector-structured
   container. The trailer settles it in the other direction: every `8c0a` member carries type **3**
   and every `8c06` member type **4**, and the dispatcher sends those to `OodleLZ_Decompress`
   unconditionally, so the compressed payload is a codec stream, not a per-256-byte-unit transform.
   What remains unexplained is where inside the member the stream *starts* (see §4.2(f)).

---

## 7. Next steps (bounded, in order)

The former steps 1–3 are done (§4.2): the caller is known, the descriptor's pointer turned out to be
a *path* producer rather than a decompressor, and the codec question is decided in Oodle's favour.
What remains is deliberate and separate work:

1. **A decoder is now a bounded task with a known interface.** Input: `member[0 : len-12]`; output:
   exactly `trailer & 0x1fffffff` bytes; codec: `OodleLZ_Decompress` type 3/4 (`fuzzSafe = 1`),
   `memcpy` for type 0. The unanswered codec-format question is §4.2(f): does the stream start at
   byte 0 or after the 16-byte header. Deciding it needs a *working* Oodle implementation, which this
   repository does not have; a from-scratch one is a separate, much larger task.
2. **Do not re-derive the container.** `.csdr` = `[leading header][body][12-byte trailer]`, trailer =
   `{u64 tag 0x1343534452; u32 {type:3,size:29}}`, `body length = u16be[6:8] + 20 = len`, is now
   measured on 34,392 / 34,392 members. Further header archaeology (what `[8:12]`, `[12:16]`,
   `[16:20]` mean) does **not** unblock decompression and should not be scheduled ahead of it.
3. **Runtime liveness is a different task.** Nothing here shows that a compressed program is ever
   requested. That needs a dynamic observation (or the shipping/NEO variant selection traced through
   `shaderTypeToLoad`), and it is not answerable from the image alone.
4. `lz4`/`zstd` stay **NOT TESTED** and are now known to be irrelevant for this container: the
   trailer names the codec.

---

## 8. Reproduction

```sh
export JAVA_HOME=/opt/homebrew/Cellar/openjdk@25/25.0.4.1/libexec/openjdk.jdk/Contents/Home
export PATH="$JAVA_HOME/bin:$PATH"
export GHIDRA_INSTALL_DIR=/Users/dehydratedflask/Applications/ghidra_12.1.4_PUBLIC

# 1. Whole-image scalar sweep (3,661,345 instructions, ~4 min).
./local/venv-pyghidra/bin/python tools/re/ghidra/run_dump.py \
    --project-dir local/ghidra/persistent-20261020 --project-name SotC \
    --script tools/re/ghidra/sweep_constants.py --out /dev/null \
    > local/validation/csdr-loader/sweep.txt

# 2. Symbol / string census for Oodle and .csdr.
./local/venv-pyghidra/bin/python tools/re/ghidra/run_dump.py \
    --project-dir local/ghidra/persistent-20261020 --project-name SotC \
    --script tools/re/ghidra/probe_csdr_refs.py --out /dev/null \
    0x12cfc1c 0x12cfbe0 0x1294b2c

# 3. Inbound call census into the Oodle region, and the hole at 0x1aa351.
./local/venv-pyghidra/bin/python tools/re/ghidra/run_dump.py \
    --project-dir local/ghidra/persistent-20261020 --project-name SotC \
    --script tools/re/ghidra/sweep_region_calls.py --out /dev/null 0x890000 0x8c0000
./local/venv-pyghidra/bin/python tools/re/ghidra/run_dump.py \
    --project-dir local/ghidra/persistent-20261020 --project-name SotC \
    --script tools/re/ghidra/probe_disasm_range.py --out /dev/null 0x1aa3fb 0x1aa442

# 4. The switch table Ghidra could not recover, at 0xfadd4c (relative int32 offsets).
python3 -I - <<'EOF'
import struct
d = open("local/re/eboot.elf", "rb").read()
BASE, OFF = 0xfadd4c, 0xfadd4c + 0x4000     # PHDR 0: file offset = VA + 0x4000
for i in range(6):
    rel = struct.unpack_from("<i", d, OFF + 4 * i)[0]
    print(i, hex((BASE + rel) & 0xffffffff))
EOF

# 5. Corpus facts (§1).
python3 -I local/re/csdr_prefix_census.py           # 34,291 / 19,320, prefix histogram
python3 -I local/re/csdr_variant_crosstab.py       # 9,660 / 7,536 per name variant
python3 -I local/re/csdr_header_fields.py          # field rules
python3 -I tools/lift/check_csdr_loader_search.py  # pins the claims above

# 6. The caller chain of §4.2 (read-only Ghidra decompilation of six bodies).
./local/venv-pyghidra/bin/python tools/re/ghidra/run_dump.py \
    --project-dir local/ghidra/persistent-20261020 --project-name SotC \
    --script tools/re/ghidra/dump_functions.py \
    --out local/validation/csdr-decompressor-re/dossier-1.txt \
    0xb0d3e0 0xb0cc50 0xb0cb80 0x146ea0 0x147160 0x146b60

# 7. The trailing-word census of §4.2(e) — whole corpus, counts only.
python3 -I local/re/csdr_tail_census.py 1 local/validation/csdr-decompressor-re/tail-census.json
```

The FDE of §4.2(a) is read straight out of `.eh_frame` (no tool needed):

```sh
python3 -I - <<'EOF'
import struct
d = open("local/re/eboot.elf", "rb").read()
o = 0x135cccc                                   # FDE, found through .eh_frame_hdr's search table
ln = struct.unpack_from("<I", d, o)[0]
pcrel = struct.unpack_from("<i", d, o + 8)[0]
rng = struct.unpack_from("<I", d, o + 12)[0]
pc = pcrel + (o + 8 - 0x4000)                   # .eh_frame is in PT_LOAD 0: file = VA + 0x4000
print(hex(ln), hex(pc), hex(rng))               # 0x20 0x1aa2a0 0x1ca
EOF
```

Project hygiene: the persistent project was only ever **opened read-only** — no re-import, no
re-analysis, no save. Ghidra did rewrite its own bookkeeping (`SotC.rep/~index.*`,
`SotC.rep/project.prp`) on open/close, which is normal; the program database
(`SotC.rep/idata/00/`) still carries its 2026-10-20 timestamp, so the analysis is unchanged.
`probe_disasm_range.py` and `probe_callgraph.py` alter the in-memory program only.

Private outputs (counts and addresses only, no member bytes):
`local/validation/csdr-loader/` (`sweep.txt`, `probe-strings.txt`, `rel32-oodle.txt`,
`callgraph-*.txt`, `dossier-*.txt`), plus `local/re/csdr_prefix_census.json`,
`local/re/csdr_variant_crosstab.json`, `local/re/csdr_header_fields.json`,
`local/re/rel32_inbound.json`.

For §4.2: `local/re/csdr_tail_census.py` (and `csdr_trailer_probe.py`, its first, sampling pass),
`local/validation/csdr-decompressor-re/tail-census.json`, and the eight read-only Ghidra dossiers
`dossier-1.txt … dossier-8.txt` in the same directory. No member byte and no decoded shader text is
stored in any of them — the census writes header words and counts only.

## 9. Limits

- Static analysis of one executable plus a corpus census. No guest code executed, no member
  decompressed, no GPU, G3 or rendering claim.
- §2 covers instructions Ghidra has; **102,894 instructions are outside any function body** and the
  sweep still visits them, but an entirely undecoded range (like `0x1aa351..0x1aa42e`) contributes
  no instructions to the sweep at all until it is force-disassembled.
- §4's rel32 census is disassembler-independent for *direct* call edges only. Indirect calls, `jmp`
  tables and function pointers are outside its reach.
- The Oodle/zlib identification is by embedded strings and symbol labels. No Oodle API was called and
  no codec behaviour was reproduced.
- §4.2 is a **call-graph and container-format** result. No member was decompressed: the Oodle call
  site is placed on the path from bytes and from a corpus census, not by running it. "The `.csdr`
  member expands" therefore means "the retail code feeds it to `OodleLZ_Decompress`", not "it
  decodes".
- §4.2(e)'s census reads member bytes but stores none: the JSON holds the tag match, the 3-bit type,
  the 29-bit size and per-archive counts.
- §4.2(f) leaves the codec's *stream start offset* open on purpose. The compiled caller passes
  `member[0 : len-12]`; whether those leading 16 bytes are codec input or a header the codec skips is
  not decidable without an Oodle implementation.
- §6.6 is **withdrawn**; the earlier text is kept above only so the correction is auditable.
- The claim that compressed members are "live" is **not** re-established here; §1.2 only shows the
  previous argument for it was invalid.
