# Checkpoint: oodle-prereqs - what a native Mermaid/Kraken decoder author needs from the pinned executable

- Time / timezone: 2026-11-01, local
- Agent and integrator: worker (static-analysis) under the orchestrator; the integrator owns `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`
- Task / gate / status: Oodle decoder prerequisites; label **STATIC_ANALYSIS_VERIFIED**. No decoder is written or vendored. Not a boot/audio/save gate.
- Prerequisite evidence / input and source hashes: `local/re/eboot.elf` sha256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3` (image base 0, RVA == address); FDE table `EH_FRAME_HDR 0x150c08c`, 55,790 entries (authoritative function extents); builds on `check_csdr_oodle_dispatch.py` (E1075 family).
- Owned / changed files: new `tools/lift/check_oodle_prereqs_newlz.py`, new `docs/checkpoints/2026-11-01-oodle-prereqs.md`; private evidence under `local/validation/oodle-prereqs-20261101/` (gitignored).
- Hypothesis and expected result: the four open questions (decoder-memory cells, type routing, size constants, function map) are answerable from the bytes alone and can be pinned so that any byte or relocation change fails the checker.
- Exact command, CWD and nonsecret environment: workspace root; `python3 -I tools/lift/check_oodle_prereqs_newlz.py [--no-mutate|--tamper|--archives|--print-map]`.
- Exit / observed result: see "Gate results" below.
- Log, capture and REA evidence paths/IDs: `local/validation/oodle-prereqs-20261101/` (`run-full.txt`, `run-tamper.txt`, `run-nomutate.txt`, `run-archives.txt`, Ghidra dossiers `dossier-*.txt`, decompiles `d*.c`, disassembly `disasm-*.txt`, `fde-out.txt`, `census-out.txt`).
- Observation versus inference / coverage limitations: separated in each section; the function map has a Label column (O observed, I inferred, `-` not examined).
- Cumulative failed tries and distinct hypotheses: 0 failed tries for this task. Two doc corrections found (below).
- Remaining risks / blockers: see "NOT RUN / open".
- Next exact action: a clean-room implementer (who has NOT read this file's section 4 detail, see the caveat) writes the decoder from the public format description and the interface facts in section 2/3, and uses this checker's constants only as acceptance numbers.
- HANDOFF.md / TASKS / EVIDENCE / INSTALL_LOG / indexes updated: not by this worker (rule); paste-ready text is in the worker report.

## Clean-room caveat (read first)

`docs/CODEC_DECISIONS.md` asks the decoder implementer not to study `0x8ac710`, `0x8ace10`, `0x8c1e60`, `0x89d3b0`.
This record was produced by reading those bodies. Section 2 and 3 therefore contain **interface facts only** (field
positions, sizes, ranges, constants, call routing), not control flow, tables, or algorithms. An implementer who wants to
stay clean-room should not read sections 2 and 3 until the orchestrator decides whether this worker's contamination is
acceptable; the addresses in the table and the cell/size analysis (section 1) do not describe any decoder algorithm.

## 1. Decoder-memory cells and `0x89dad0`

Observations (bytes pinned by the checker; whole-image reference census over E8/E9/Jcc/lea/RIP-disp/q64/d32/RELA):

| Cell | Role | Where touched |
|---|---|---|
| `0x2ee6720` | decoder-memory pointer | init/free in `0x1aa2a0` (inline), dead `0x1aa0f0`/`0x1aa160`, dtor `0x1aa070`, static ctor `0x1aa510`, stack arg at `0x1aa418` |
| `0x2ee6740` | init flag byte | `0x1aa2d2` (inline init), dead copies |
| `0x2ee6748` | size, int64 (sign-extended int) | `0x1aa2f5` store, stack arg at `0x1aa412` |
| `0x2ee6728` | 24 B rdtsc **stopwatch/profile object, not a lock** | `0x1aa324`, `0x1aa432`, `0x1aa497`, `0x1aa4b7`, ctor `0x1aa53c` |
| `0x2ee6750` | bytes-decoded counter | `0x1aa445`, `0x1aa4a3` (reset), `0x1aa4cf` (read) |

- All five cells are `.bss`: inside a segment's memsz, beyond filesz, so no initial bytes exist (checker enforces `ValueError` on a file read).
- **Initialisation.** The only live initialiser is inline in `bpe::Compression::Decompress` (`0x1aa2a0`, `0x1aa2cf..0x1aa31e`): if flag `[0x2ee6740]` is 0 then set it to 1 *first*, `size = 0x89b980(-1, -1)`, store `[0x2ee6748]`, `ptr = alloc(0x17090)` (engine allocator wrapper), and free the previous pointer via `0x170c0` if it differs. `0x1aa0f0` (Init) and `0x1aa160` carry the same init but have **zero references** (census: no E8/E9/Jcc/lea/q64/d32/RELA). `0x1aa510` is the static ctor (table entry `0x2dd60b8`): zeroes the pointer, `__cxa_atexit(0x1aa070)`, constructs the stopwatch. `0x1aa070` frees the cell at exit.
- **Size.** `0x89b980(type,rawLen)` = `0x89b990(type,rawLen,NULL) + 0x10`. With `(-1,-1)` type is clamped to compressor id 6 (state-size leaf `0x90c100` = `0x4d18`), `+0xa8` base, and because `rawLen < 0` the scratch term is the full `0x40000`: `0x4d18 + 0xa8 + 0x40000 + 0x10 = 0x44dd0` (281,040 B). The checker derives this from the immediates. Leaves for id 7 (`0x927c00`) = `0x2cdc`, id 10 (`0x9359d0`) = `0x3ea8`; both are below id 6, so the universal size covers them. Ids 0/1 use `0x8a0480` sums and were not evaluated. newLZ ids (8,9,11,12,13; mask `0x3b00`) are `0xa8 + min(2*rawLen+0x420, 0x40000)` (+0x10 in the wrapper), at most `0x400b8 <= 0x44dd0`.
- **Use.** `0x1aa426` calls `0x89d3b0` with 14 arguments matching the public `OodleLZ_Decompress` signature (`fuzzSafe` = 1 for tags 2/3/4, 0 for tag 5; `checkCRC=0`, `verbosity=0`, `decBufBase=0`, `decBufSize=0`, no callback, `decoderMemory=[0x2ee6720]`, `decoderMemorySize=[0x2ee6748]`, `threadPhase=3`).
- **Wrapper contract (`0x89d3b0`).** A supplied decoder memory must be at least `stateSize + 0x10`; it is aligned `(ptr+0xf) & -16` and `size - 0x10` is used. With NULL memory it uses alloca when `size < [0x2dce800]` (`0x1000`), else the hook `[0x2dce810]` (`0x93a8d0`). The `0x80000` minimum (`0x89f410`) applies only to threadPhase 1/2 (not reached with phase 3). Hook cells: `0x2dce810`->`0x93a8d0` (alloc), `0x2dce818`->`0x93a8e0` (free, not in the map), `0x2dce820`->`0x93a8f0` (log), `0x2dce828`->`0x93a9b0` (assert).
- **`0x89dad0` DecodeBufferSize(rawLen, flag)** = `rawLen + (flag ? 0x100 : 0x10)`. `DecodedSize` tag 5 tail-jumps to it (`0x1aa290`) with the +0x10 variant; tags 2/3/4 return exactly `rawLen`. Ghidra has no function there, the FDE does (20 B). A native decoder's output buffer therefore needs `rawLen + 0x10` for tag 5 only.

Inference:
- **Thread-safety.** The lazy init is unguarded and racy: no mutex, flag set before the size/pointer are valid, a second thread can read a not-yet-valid pointer/size or double-allocate (the `0x170c0` free of the old pointer shows the author expected re-entry). The single shared buffer is also the scratch memory of the decode, so two concurrent decodes would corrupt each other. Nothing in the statically examined callers serialises it (caller-level serialisation NOT examined). The profile stopwatch is not a lock.
- **Native consequence.** Use per-call or per-thread scratch of at least `0xa8 + min(2*raw+0x420, 0x40000)` (Kraken/Mermaid), 16-byte aligned; no process-global buffer is required. `0x44dd0` is a safe upper bound only for this engine's universal cell, not a format requirement.

## 2. Which decode type reaches which function (interface facts)

Observations:
- Decode type (byte 1 of the 2-byte header, ctx+0x34, value `<= 0xc`) indexes the dword table `0x1295090` = `[1,2,4,5,6,7,8,0,-1,-1,9,10,13]` -> compressor id; ids name the 14 relocation pointers at `0x15ced00` (LZH, LZHLW, LZNIB, None, LZB16, LZBLW, LZA, LZNA, Kraken, Mermaid, BitKnit, Selkie, Hydra, Leviathan). Types 8 and 9 map to -1 and branch to the invalid-type block `0x89c656`.
- Driver `0x89bcb0` dispatches through the 13-entry rel32 table `0x1294528`: **type 6 (Kraken) -> `0x8c1e60`** (call `0x89c57e`), **type 10 (Mermaid) -> `0x8db8a0`** (call `0x89c64c`), type 12 (Leviathan) -> `0x8cddc0` (call `0x89c443`). Each has exactly one caller. The other types go to non-newLZ code (not examined).
- So the second header byte is the decode type: `0x0a` = 10 = Mermaid, `0x06` = 6 = Kraken. First byte `0x8c` (low nibble 0xc is required).
- **`0x8c1e60` is the Kraken decoder only.** It does not serve both codecs. This corrects E1075, `CODEC_DECISIONS` section 2.2 and the b0cc50 checkpoint wording that grouped them.
- Archive census (counts only, `--archives`): `.csdr` Decompress tag 3 <-> header prefix `8c0a` (Mermaid) x16,328; tag 4 <-> `8c06` (Kraken) x2,992; tag 0 (memcpy) x15,072 carries no codec header; no other pairing. No member starts with a type-12 header.

Header field facts (positions only):
- byte0: low nibble must be `0xc`; version = `((b0 >> 4) & 3) | 4`; bit6 = stored/memcpy flag; bit7 = restart/keyframe flag. byte1: `& 0x7f` = decode type (> 12 invalid); bit7 = quantum-CRC enable. A new header is read whenever `(offset & 0x3ffff) == 0`, i.e. per 0x40000 chunk.
- Newlz recursion/depth guard 999.

Inference: the decode-type -> codec name mapping agrees with the public Oodle compressor ids (Kraken 8 / Mermaid 9 / Leviathan 13 in the ids; types 6/10/12 in the header numbering). Not cross-checked against any Oodle source (none is available or used).

## 3. Quantum, chunk and scratch constants

| Constant | Value | Meaning (observed unless marked) |
|---|---|---|
| chunk / window | `0x40000` | new 2-byte header every chunk; offset mask `0x3ffff` |
| per-quantum raw cap | `0x20000` | Kraken and Mermaid quantum |
| scratch | `min(2*raw + 0x420, 0x40000)` | state-size term for newLZ ids; slop `0x420` |
| non-newLZ quantum | `0x4000` | ids outside the newLZ family |
| newLZ state base | `0xa8` | added to the id leaf / scratch |
| buffer slack | `0x10` (flag 0: `0x10`; flag 1: `0x100`) | `0x89dad0` and `0x89b980` |
| quantum header (compressed) | 3 bytes, big-endian | bit31 compressed flag; compLen = `(be32 >> 8) & 0x7ffff`; mode bits 27..30 (Mermaid uses a shifted extract of the same field) |
| quantum header (non-compressed branch) | top nibble 0 = stored (data at +3); nibbles 2 and 4 = 5-byte header | compLen `be32(hdr+1) & 0x3ffff`, rawLen-1 by bit extract; nibble 4 passes a flag to the entropy decoder `0x8f47d0` |
| Kraken phase 2 | mode 0 -> `0x8cbd30`, mode 1 -> `0x8cb610`, else "bad chunk_type" | |
| newLZ QH (`0x8ace10`) | 3 bytes, 18-bit value, special `0x3ffff` (stored), compLen = value+1, optional 24-bit CRC | |
| masks | fuzz-safe `0x3f13`; newLZ ids `0x3b00` (8,9,11,12,13); newLZ types `0x51 >> (type-6)` (6,10,12) | |
| threaded minimum | `0x80000` | threadPhase 1/2 only |
| alloca threshold | `[0x2dce800] = 0x1000` | NULL decoder memory |

Inference: the 0x3ffff "special" value and the stored (memcpy) semantic of the quantum header are read from the control flow; their behaviour on malformed input is not characterised.

## 4. Function map of the newLZ family and its wrapper chain

Extents are FDE `address_range` values; every row also pins the sha256 of its FDE extent in the checker. Label: O observed, I inferred, `-` not examined.

| RVA | FDE bytes | Name | Role | Label |
|---|---|---|---|---|
| `0x1aa070` | 36 (0x24) | Decoder-memory cell dtor (atexit) | frees [0x2ee6720] if non-null, clears it; registered by 0x1aa510 | O |
| `0x1aa0f0` | 92 (0x5c) | Decoder-memory Init() [UNREFERENCED] | same lazy init as inline in 0x1aa2a0; zero references in the whole-image census | O |
| `0x1aa160` | 159 (0x9f) | Size/bound helper by tag [UNREFERENCED] | same lazy init, then tag 2..5 -> 0x89ed10, 1 -> 0x1b4130, 0 -> identity (GetCompressedBound-like, by inference) | O/I |
| `0x1aa2a0` | 458 (0x1ca) | bpe::Compression::Decompress | tag switch; the ONLY live initialiser of cells 0x2ee6720/40/48; calls 0x89d3b0 once | O |
| `0x1aa490` | 29 (0x1d) | Decode stats reset | stopwatch reset(0x2ee6728) + [0x2ee6750]=0 (callers 0x144320, 0x149390) | O |
| `0x1aa4b0` | 90 (0x5a) | Decode throughput getter | bytes [0x2ee6750] / stopwatch time (caller 0x1451a0; profiler UI, by inference) | O/I |
| `0x1aa510` | 56 (0x38) | Static ctor for the cells | [0x2ee6720]=0; __cxa_atexit(0x1aa070,&cell); stopwatch ctor(0x2ee6728,0); pointer in a ctor table at 0x2dd60b8 | O |
| `0x17090` | 20 (0x14) | Alloc wrapper | esi=1 edx=8 ecx=2 ; jmp 0x12820 (engine allocator) | O |
| `0x170c0` | 10 (0xa) | Free wrapper | esi=2 ; jmp 0x12af0 (engine free) | O |
| `0x89b980` | 16 (0x10) | DecoderMemorySize(type,rawLen) | 0x89b990(type,rawLen,NULL)+0x10; called with (-1,-1) at 0x1aa2ea | O |
| `0x89b990` | 287 (0x11f) | DecoderStateSize(type,rawLen,*outState) | per-compressor state + scratch; newLZ ids add min(2*rawLen+0x420,0x40000) | O |
| `0x89bcb0` | 5841 (0x16d1) | Decode driver (chunk/quantum loop) | oodlelzcompressors.cpp; header parse, 0x40000 chunk window, switch to the codec by decode type | O |
| `0x89d3b0` | 1676 (0x68c) | OodleLZ_Decompress wrapper (14 args) | single call site 0x1aa426; validates fuzz-safe mask, sizes/aligns decoder memory, loops 0x89bcb0 | O |
| `0x89dad0` | 20 (0x14) | DecodeBufferSize(rawLen,flag) | rawLen + (flag ? 0x100 : 0x10); FDE-only (Ghidra has no function here) | O |
| `0x89f410` | 6 (0x6) | threaded-phase minimum | returns 0x80000; reached only when threadPhase is 1/2 | O |
| `0x8ac710` | 778 (0x30a) | Block-header parse (2-byte) | reads byte0 (low nibble 0xc) and byte1; fills ctx+0x30..0x44 | O |
| `0x8acb90` | 422 (0x1a6) | Non-newLZ quantum-header parse | used for ids outside {8,9,12}; raw cap 0x4000 | O |
| `0x8ace10` | 426 (0x1aa) | newLZ chunk quantum-header parse (3 bytes) | 18-bit compLen-1, special value 0x3ffff, optional 24-bit crc | O |
| `0x89fc90` | 1968 (0x7b0) | 24-bit quantum checksum | compared with the QH crc field when checkCRC != 0 (role by inference) | I |
| `0x8c1780` | 166 (0xa6) | Zero-fill (ptr,size) | called with 0x10/0x20/0x88/0x98 on contexts | O |
| `0x8c1aa0` | 25 (0x19) | tiny guard | 25 B leaf (role not examined) | - |
| `0x8c1ac0` | 18 (0x12) | tiny guard | 18 B leaf, called at wrapper exit with the context pointer (role not examined) | - |
| `0x8c1b20` | 20 (0x14) | assert-fail handler | (file,line,func,expr) -> 0x93a9b0; nonzero result -> int 0x41 | O |
| `0x8c1e60` | 12534 (0x30f6) | Kraken chunk decoder | newlz.cpp; decode type 6 / compressor id 8; sole caller 0x89c57e | O |
| `0x8cb610` | 1817 (0x719) | Kraken phase-2 (mode 1) | newLZ_decode_chunk_phase2; call 0x8c47f7 | O |
| `0x8cbd30` | 1862 (0x746) | Kraken phase-2 (mode 0) | newLZ_decode_chunk_phase2; call 0x8c4844 | O |
| `0x8cddc0` | 13328 (0x3410) | Leviathan chunk decoder | newlz.cpp; decode type 12 / id 13; sole caller 0x89c443; no .csdr member starts with a type-12 header | O |
| `0x8d6e00` | 1507 (0x5e3) | Leviathan helper | leaf called only by 0x8cddc0 (role not examined) | - |
| `0x8d73f0` | 1522 (0x5f2) | Leviathan helper | leaf called only by 0x8cddc0 | - |
| `0x8d79f0` | 1633 (0x661) | Leviathan helper | leaf called only by 0x8cddc0 | - |
| `0x8d8060` | 4569 (0x11d9) | Leviathan helper | 4,523 B leaf called only by 0x8cddc0 | - |
| `0x8d9240` | 1178 (0x49a) | Leviathan helper | leaf called only by 0x8cddc0 | - |
| `0x8db8a0` | 5439 (0x153f) | Mermaid chunk decoder | newlzf.cpp; decode type 10 / compressor id 9; sole caller 0x89c64c | O |
| `0x8ee200` | 6034 (0x1792) | Mermaid parse loop variant | newlzf_decode_parse_outer.inl; local_d0 != 0 and sub-mode != 1 | O |
| `0x8ef9a0` | 5775 (0x168f) | Mermaid parse loop variant | as above; local_d0 != 0 and sub-mode == 1 | O |
| `0x8f1030` | 5694 (0x163e) | Mermaid parse loop variant | 'mp >= window_base' check; local_d0 == 0 and sub-mode != 1 | O |
| `0x8f2670` | 5323 (0x14cb) | Mermaid parse loop variant | 'mp >= window_base' check; local_d0 == 0 and sub-mode == 1 | O |
| `0x8f3b40` | 587 (0x24b) | Mermaid off24 reader | newlzf_sse4.cpp; 2 calls from 0x8db8a0 | O |
| `0x8f47d0` | 3251 (0xcb3) | Shared entropy sub-stream decoder | called by Kraken (6x), Leviathan (7x) and Mermaid (6x); arg includes the nibble==4 flag | O |
| `0x8a04c0` | 298 (0x12a) | Entropy primitive | callee of 0x8f47d0 only (not examined) | - |
| `0x8a1700` | 7226 (0x1c3a) | Entropy primitive | 7,150 B callee of 0x8f47d0 only (not examined) | - |
| `0x8a0480` | 59 (0x3b) | Size leaf for ids 0/1 | used by 0x89b990 (not examined) | - |
| `0x90c100` | 6 (0x6) | State size leaf, id 6 | mov eax,0x4d18 ; ret | O |
| `0x927c00` | 6 (0x6) | State size leaf, id 7 | mov eax,0x2cdc ; ret | O |
| `0x9359d0` | 6 (0x6) | State size leaf, id 10 | mov eax,0x3ea8 ; ret | O |
| `0x93a7d0` | 18 (0x12) | 18 B leaf beside the hooks | role not examined | - |
| `0x93a7f0` | 18 (0x12) | 18 B leaf beside the hooks | role not examined | - |
| `0x93a8d0` | 14 (0xe) | Alloc hook | target of the cell 0x2dce810 | O |
| `0x93a8f0` | 178 (0xb2) | Log hook | target of the cell 0x2dce820 (level,file,line,msg) | O |
| `0x93a9b0` | 40 (0x28) | Assert hook | target of the cell 0x2dce828 | O |
| `0x326a0` | 79 (0x4f) | Stopwatch ctor | byte tag + rdtsc start | O |
| `0x326f0` | 22 (0x16) | Stopwatch start | rdtsc | O |
| `0x32710` | 39 (0x27) | Stopwatch stop | rdtsc, accumulates | O |
| `0x32740` | 165 (0xa5) | Stopwatch read |  | O |
| `0x32a00` | 22 (0x16) | Stopwatch reset |  | O |

Not in the map: hook `0x93a8e0` (free), `0x89ed10`, `0x1b4130` (non-Oodle tag paths).

## Gate results

Run from the workspace root (exit codes and counts recorded by the checker; see `run-*.txt`):

- `python3 -I tools/lift/check_oodle_prereqs_newlz.py`: exit 0; 574 byte mutants + 14 relocation mutants + 7 detector probes + 2 whole-image injection probes, all rejected/detected; 55 map functions, 53 byte pins, 31 call pins.
- `--tamper`: exit 1 (`FAIL DecodeBufferSize(rawLen,flag) (0x89dad0) sha256`).
- `--no-mutate`: exit 0. `--archives --no-mutate`: exit 0 (cross-tab above).
- Label: **STATIC_ANALYSIS_VERIFIED** (bytes only; guest bytes were never executed).

## Corrections to earlier documents

1. `0x8c1e60` is Kraken-only (type 6); Mermaid is `0x8db8a0` (type 10); Leviathan is `0x8cddc0`. E1075, `CODEC_DECISIONS` section 2.2 and the b0cc50 checkpoint grouped it with both codecs.
2. `0x2ee6728` is a 24 B rdtsc profiling stopwatch (`0x326f0`/`0x32710` start/stop), not a critical section; the "critical section / lock" reading is withdrawn.
3. Quantum headers are 3 bytes in the compressed branch (not 4).

## NOT RUN / open

- State sizes for ids 0 and 1 (`0x8a0480` sums) and the non-newLZ driver paths.
- Roles of Leviathan helpers `0x8d6e00..0x8d9240`, entropy primitives `0x8a04c0`/`0x8a1700`, `0x8c1aa0`/`0x8c1ac0`, `0x93a7d0`/`0x93a7f0` (marked `-`).
- Caller-level serialisation of `Decompress` (whether the engine ever decodes on two threads).
- No dynamic execution, no comparison against decoded output; this is static analysis only.
- The inference of the 24-bit CRC function (`0x89fc90`) and the stats consumers.
