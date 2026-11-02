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

What this round **did** establish, and what it did **not**, is separated below. Everything is
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
| `cc*` prefixes | **0** in this corpus (the earlier report's single `cc0a` member is not in these archives) |
| per archive (compressed / raw) | `pak0` 2,270 / 1,039 · `pak1` 1,442 / 1,070 · `pakN` 15,608 / 12,862 |
| other archives | `pak2` and `pak3` hold **no** `.csdr`; `cachedFiles.psarc` holds **101**, all raw `0100` |

### 1.1 Header fields, whole population (not a 84-member sample)

| Field | Rule | Holds |
|---|---|---|
| `[0]` | always `0x8c` | 19,320 / 19,320 |
| `[1]` | only `0x0a` (16,328) or `0x06` (2,992) | 19,320 / 19,320 |
| `u16be[2:4]` | `== floor(size / 256)`, or exactly one less | 18,009 exact, 1,311 one less, **0 otherwise** |
| `u16be[6:8]` | `== size - 20` | **19,320 / 19,320** |
| `u32be[8:12]` | one constant bit pattern across the population (`0x02000000`, min `0x01000000`) | 1 distinct value in a 2,000-member sample |
| `u32be[12:16]` | a second constant-ish flag word (`0x00000000`, `0x02000000`, `0x03000000` seen) | median `0x02000000` |
| `u32be[16:20]` | an offset-shaped value, median `0x00107000`, sparse outliers up to `0x4057…` | — |
| member size | min 1,412 · median 4,736 · max 20,397 B (`8c0a` median 5,278; `8c06` median 3,041) | — |

Two consequences, both `STATIC_ANALYSIS_VERIFIED`:

- **The header carries no compression ratio.** `u16be[6:8]` is the *file* length minus 20, and
  `u16be[2:4]` is a count of 256-byte units of the same file. A decompressor therefore has no
  explicit output length in this header; it must infer one (e.g. `u16be[2:4] * 256`) or take it from
  the caller. This contradicts the previous note's framing of `[2:4]` as a "block count hint" — it
  is a *size* in 256-byte units, not a hint.
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
| embedded source paths | `v:\devel\projects\oodle2\core\oodlelzcompressors.cpp`, `oodlelzpub.cpp`, `rrhuffmandecode.cpp`, `rrlzh.cpp`, `rrlzh_lzhlw_shared.cpp`, `templates\rrnew.h` |
| `OODLE ERROR` / assertion strings | 105 |
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
| sibling type | `cslt` (`0x63736c74`, "shader list") at `FUN_00b0e3a0` call site `0xb0e403` |
| `csdr` descriptor tag | 8-byte constant `0x0000000134534452` → byte image `52 44 53 43 01` (`RDSC` + version 1) |
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

**What this does and does not prove.** It proves the decompressor is *linked into reachable code* —
one call site exists and Ghidra's "0 callers" was a coverage artefact. It does **not** prove that
call site expands `.csdr` members: the enclosing function is not resolved (`0x1aa351..0x1aa42e` is
still not claimed by any function entry, and the neighbouring `FUN_001aa2a0` is a tag-dispatch
allocator whose low-29-bits-as-size accounting and `FUN_000326f0`/`FUN_00032710` lock pair do not
look like an asset reader). **This is the one open edge, and it is the next task's first target.**

---

## 5. What the GNM command-buffer machinery does *not* do

`docs/GPU_COMMAND_SURFACE.md` §6 describes the title's own PM4 emitters: 293 functions in
`0x865fa0..0x8852a0` writing type-3 headers into one growable buffer (`+0x08` end, `+0x10` cursor,
`+0x18` grow callback, `+0x20` argument, `+0x28` flags). Checked this round:

- The growable-buffer machinery **copies payload dwords** (`payload_dwords == count + 1`, `memcpy`
  for `inline_data` families) and never resizes or reinterprets them. It has no shader-blob path:
  the 711 grounded writer sites cover 27 opcodes, none of which carries a whole program image.
- The shader bytes reach the driver through `libSceGnmDriver` imports (`sceGnmSetPsShader350`,
  `sceGnmSetVsShader`, `sceGnmUpdatePsShader350`, …), which are `jmp *GOT` thunks at
  `0xf903e0..0xf90a40`. Those thunks are external and are **not** part of this search's address
  space; whether the game hands them a raw or an expanded buffer is unresolved (§6).

So the command buffer is **not** the copy/expand path this task hypothesised it might be
(`STATIC_ANALYSIS_VERIFIED` for the writer census, which is a re-read of the already-grounded table;
the "no shader blob" statement is a structural reading of the same table, not a new sweep).

---

## 6. How a loader could still have been missed (the honest residual)

The two sweeps above are strong but neither is a proof of absence. These are the concrete remaining
holes, in the order they should be closed:

1. **The unresolved call site.** `0x1aa426 → 0x89d3b0` exists and is proven; the function that owns
   it is not identified. Because the owner is unknown, "is this the asset path" is unanswered. This
   is the only *positive* lead and must be closed first.
2. **102,894 instructions outside any function body** (§2) plus the undecoded ranges that
   `probe_disasm_range.py` exposes. Any loader that lives entirely in such a range is invisible to
   `getFunctionContaining`, to `getReferencesTo`, and to the decompiler. The rel32 census in §4 was
   immune to this for *call edges*, but a loader reached only through a `jmp` into a table, or
   reached by `call rax` after an indirect load, would still hide.
3. **Indirect calls.** The image has 499 indirect call sites in one closure alone
   (`docs/INDIRECT_FRONTIER.md`). A decompressor invoked through a per-resource-type function
   pointer — which is exactly the shape §3.2 shows for `csdr` (`local_90 = FUN_00b0cc50` in the
   descriptor) — would appear as a data reference to a descriptor, not as a call to `0x89d3b0`.
   This is now the *most likely* hiding place, and it is why §3.2 matters more than §4.
4. **`.bss` globals.** The two stack arguments come from `0x02ee6720`/`0x02ee6748`, which have no
   file image. Their initialisers are known (`FUN_001aa2a0`), but any *other* writer would be
   invisible to a byte scan of the image.
5. **A second decompressor.** Oodle and zlib are both present; nothing rules out a third,
   title-specific block codec written in-house. No sweep in this round searched for one, because
   there is no predicate for "a codec I have not seen".
6. **The `[0]==0x8c` byte may not be a codec tag at all.** §1.1 shows the header carries no
   compression ratio and that `[8:16]` look like flag words with `[16:20]` an offset. A
   sector-structured container (e.g. per-256-byte-unit transform) would have exactly these fields
   and would fit the 7.5–8.0 payload entropy better than a codec does. **This is a hypothesis, not a
   finding**, and it changes what to look for: a sector loop, not a bitstream reader.

---

## 7. Next steps (bounded, in order)

1. **Resolve `0x1aa351..0x1aa42e`.** Find the entry that reaches it (the jump table at `0xfadd4c`
   indexed by `*param_1 >> 29` is the nearest candidate) and decompile the owner of the
   `OodleLZ_Decompress` call. One question only: is `compBuf` an archive member buffer.
2. **Follow the `csdr` descriptor's function pointer.** `FUN_00b0d3e0` stores `FUN_00b0cc50` and
   `&DAT_02ed1fb0` into the descriptor; find every consumer of that descriptor and read what it does
   with the bytes it gets back. This is the path §6.3 predicts.
3. Only then decide between "the member is Oodle-compressed" and "the member is a sector-structured
   container". Do not write a decoder before that decision: `lz4`/`zstd` block formats are still
   **NOT TESTED**, and a codec guess would repeat the 3,780-attempt dead end.

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

# 4. Corpus facts (§1).
python3 -I local/re/csdr_prefix_census.py           # 34,291 / 19,320, prefix histogram
python3 -I local/re/csdr_variant_crosstab.py       # 9,660 / 7,536 per name variant
python3 -I local/re/csdr_header_fields.py          # field rules
python3 -I tools/lift/check_csdr_loader_search.py  # pins the claims above
```

Private outputs (counts and addresses only, no member bytes):
`local/validation/csdr-loader/` (`sweep.txt`, `probe-strings.txt`, `rel32-oodle.txt`,
`callgraph-*.txt`, `dossier-*.txt`), plus `local/re/csdr_prefix_census.json`,
`local/re/csdr_variant_crosstab.json`, `local/re/csdr_header_fields.json`,
`local/re/rel32_inbound.json`.

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
- §6.6 is a hypothesis about the container shape, explicitly not a finding.
- The claim that compressed members are "live" is **not** re-established here; §1.2 only shows the
  previous argument for it was invalid.
