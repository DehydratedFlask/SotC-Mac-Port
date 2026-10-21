# Checkpoint: SAV-01-PROFILE-CHUNKS-01 — profile chunk writers/loader, generic chunk lookup, bool chunks, outer-header checks

- Time / timezone: 2026-10-21 (America/Chicago), macOS 27.0.1 arm64
- Agent: worker "agent H" (Claude Sonnet 5.5); integrator is the orchestrator session. **Analysis only.**
  `native/`, `launcher/`, `tools/lift`, `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `INSTALL_LOG.md` were **not**
  edited; nothing committed or pushed. Ghidra/REA were **not** used (held by another agent).
- Task / status: SAV-01-PROFILE-CHUNKS-01. **STATIC_ANALYSIS_VERIFIED** for every claim labelled OBSERVATION-VERIFIED
  below (byte-level verifier, mutation-tested with 47 in-memory mutations); claims labelled OBSERVATION-READING come from the offline
  disassembly only; claims labelled INFERENCE are not proven. No native code, no save/load behaviour, no real-save test is
  claimed. G2 stays NOT RUN. Tries: one sub-question (tag 4 vs tag 8) reached three falsified hypotheses and is recorded UNRESOLVED as instructed; no other sub-question failed; the task as a whole is complete, so no `NEEDS_PRINCIPAL` escalation file was created.
- Input: plaintext ELF `local/re/eboot.elf` (SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`,
  65,214,308 bytes, identified in `docs/EXTRACTION.md`); relocation table and `local/re/imports.json` for vtable/thunk identity.
  All addresses are image RVAs (LOAD 0 maps file offset `0x4000` to vaddr `0`).
- Tools: Xcode `llvm-objdump` (x86-64, offline) + python3 byte scanners. No emulator, no guest execution.

## 1. Answers at a glance

| Question | Answer | Status |
|---|---|---|
| (a) profile chunk writer `0x548d30` (+5 helpers `0x54aca0/0x54ad70/0x54ae40/0x54af10/0x54afe0`) | 33 named chunks + 257 indexed chunks (290 per save); hash, tag, payload layout in section 4 | OBSERVATION-VERIFIED |
| (a) profile loader `0x545d30` (+ enum helper `0x548b90`) | reads the same 33 names + 257 indexed; one chunk is read and discarded; field sinks in section 4 | OBSERVATION-VERIFIED (tags, names, payload classes); sinks OBSERVATION-READING |
| (a) generic reader `0xa81180` | linear first-match scan by name hash over top-level chunks; payload becomes a bounded sub-stream | OBSERVATION-VERIFIED |
| (a) bool chunks (writer `0x541ba0` -> `0x7e4b50`; (de)serialiser `0x7e6b50`) | 48 one-byte chunks, tag 13, no length prefix; flag index i is also bit i of the game-data blob | OBSERVATION-VERIFIED |
| (a) endianness | **little-endian on disk**: writers consult a swap flag that is 0 from construction and never set (only two unreferenced setters exist); readers have no byte swap at all | OBSERVATION-VERIFIED |
| (b) is the blob's u64 length prefix inside `payload_length`? | **Inside.** `payload_length = bytes written between chunk-begin and chunk-end`, and the prefix is written between them; the reader parses the prefix out of the payload sub-stream | OBSERVATION-VERIFIED (two independent sides) |
| (c) trailing bytes | file level: loader reads exactly `0x10 + dword2` bytes, anything after is never read; body level: scan stops at first record whose magic is not `0xBEEF`, bytes after the last chunk are ignored; no bounds clamp on an overlong record; empty body dereferences NULL | OBSERVATION-VERIFIED |
| (c) outer-header dword 3 | **validated, but by the class loaders, not the file loader**: `[obj+0x14] == descriptor id`, mismatch returns 6 before any chunk is read. Profile id `0xd`, progress id `4` | OBSERVATION-VERIFIED |
| tag meaning | tags 2..11 are fixed-width/float codes and 12 = string, 13 = byte payload; sizes observed from a jump table; signedness of 2..5 vs 6..9 is inferred | sizes OBSERVATION-VERIFIED, types INFERENCE |
| tag 4 vs tag 8 (both 4 bytes) | **UNRESOLVED** (three hypotheses falsified, see section 9) | UNRESOLVED |

Corrections to earlier checkpoints (from the bytes): (1) the 257-iteration indexed loop is in **both** the progress writer
`0x543f40` and the profile writer `0x548d30` (two code variants there, one executes); the profile writer also copies from
the progress singleton's array. (2) The earlier "flag-sync step" unknown in `blob-flush` is resolved: see section 5.
(3) "27 sites plus 5 helpers" for the profile object is confirmed: 27 begin-sites (25 fixed + 2 loop variants), the 5 helpers
serve 8 enum-string chunks.

## 2. Method and evidence

Two independent derivations of every table: (1) the llvm-objdump text pipeline (`work/*.asm`, `work/*.py`), (2) the
raw-byte scanner in `verify.py` (finds `E8 rel32` call sites, `lea r,[rip+d32]`, `mov [rbp/rsp+d],reg|imm32`, `mov edx,imm32`,
`cmp eax,imm`, jump table, relocation table; recomputes every name hash). Cross-check at build time: 22/22
profile writer sites, 28/28 profile reader sites and 48/48 bool rows agree. `verify.py`: **232 checks, 0 failed**;
`mutation_test.py`: **47/47** in-memory byte mutations are caught.

Private evidence directory: `local/re/evidence/sav01-profile-chunks-01/` (never published; it contains disassembly text).

| File | SHA-256 |
|---|---|
| `verify.py` | `93ce511e4e40a2e13cb8a083368343e4d8750b110ba55ffccdb7b8eedcf83998` |
| `claims.json` | `cb1344f8bcd84fc54b1235628fc3438c229cc427e93005f4603f034f5ef77ba4` |
| `build_claims.py` | `b5830e3d9138228075a80cb9b3240b54b8775b484f1e3ffe2e5deceedebd3ad0` |
| `mutation_test.py` | `9f91dbdffbc75b67cbb55160cdd10f535ae7a94021e5476392ed6c6af4c53863` |
| `verification.json` | `38904e7359641c0938a48e303ce86ee3638e2cbfabc82e0ba261084a7504e37a` |
| `mutation_results.json` | `016a246cdc1492b06b7b2ec302baec7069fc8088036b7cb0c2fa673c40bf2389` |
| `SHA256SUMS` (54 files) | `349688160709574722e40fa42f755b7511ea39c1ece93943e1309062e7c4219a` |

Labels: **OBSERVATION-VERIFIED** = re-derived by `verify.py` from ELF bytes (table rows, tags, payload classes, hashes, call-site
sets, negative scans, byte-exact signatures of the key code ranges). **OBSERVATION-READING** = read from the disassembly,
not re-derived by script (field sinks, getter/setter addresses). **INFERENCE** = not proven.

## 3. Wire format recap (what the bytes say)

### 3.1 Outer header (16 bytes, little-endian dwords; unchanged from earlier checkpoints, plus new facts)

| dword | meaning | checked by |
|---:|---|---|
| 0 | version, must be 1 | file loader (`0xa4910a`) |
| 1 | FNV-1a-32 of the body (seed `0x811c9dc5`) | file loader |
| 2 | body length, must be `<= 0x6400000` (unsigned) | file loader |
| 3 | **class descriptor id**: profile `0xd`, progress `4` (static initialisers at `0x52f112`, `0x52f19b`) | **class loaders** `0x545d30` / `0x542880` via `0xa515a0`: `[obj+0x14] == *[obj]`, mismatch -> return 6, nothing is read |

Success of a class loader is the return value 1 (the caller compares `== 1`). Observed error values of the file loader:
`0x809f000b` for a short header read, `0x809f000f` for wrong version / oversize / short body / hash mismatch.

### 3.2 Chunk record (12-byte header + payload)

`name_hash:u32 | magic:u16 = 0xBEEF | tag:u16 | payload_length:u32 | payload[payload_length]`, all little-endian.

- Chunk-begin `0xa810b0(stream, name, tag)`: hashes the name (seed `0x811c9dc5`), writes the header with
  `payload_length = 0xFFFFFFFF`, and remembers the stream byte count *before* the header in `[stream+0x50]`.
- Chunk-end `0xa81130(stream)`: flushes staging, then back-patches `payload_length = total_bytes_now - start - 12` into the vector.
  The patch is a plain 32-bit store (a stream larger than 4 GiB would wrap; the format limit is `0x6400000`).
  Only one chunk can be open at a time (single start slot, reset to -1).
- Chunk lookup `0xa81180(out_reader, stream, name, &tag_out)`: hash the name, take the vector start, then loop:
  (1) magic at +4 must be `0xBEEF`, else stop = not found; (2) hash at +0 equal -> found, first match wins;
  (3) next record = `rec + payload_length + 12`; if it equals the end pointer exactly -> not found; if its magic is not `0xBEEF` -> not found.
  Found: `*tag_out = tag`, reader = memory stream over `(rec+12, payload_length)` with position 0.
  Not found: `*tag_out = 1` (never a valid tag) and an empty reader. Callers compare the tag and skip the read on mismatch,
  so **a missing, retagged or truncated chunk leaves the destination at its prior/default value**.
- No bounds clamp: an overlong `payload_length` makes the scan walk past the body (original out-of-bounds read); a zero-length body
  makes the vector start NULL and the first magic read dereferences NULL (original crash). A native reader must reject both.

### 3.3 Name hash `0x162020`

`h = 0x811c9dc5; for each byte c: c = toupper(c) via _Getptoupper table; if c == '\\' then c = '/'; h = (c ^ h) * 0x01000193`
(32-bit). Verified: loop shape, the `0x5c -> 0x2f` substitution, the prime, seed at both call sites (`0xa810d4`, `0xa81197`).
**INFERENCE (not proven):** the `_Getptoupper` table is the C-locale ASCII `toupper` (names are ASCII, so only A-Z/a-z matter).
No precomputed hash constants exist in the image to cross-check (ad-hoc `scan_hash_constants.py`, not part of `verify.py`: all 46 names under upper/lower/no folding, 0 hits).

### 3.4 Payload primitives

| tag | element size (jump table `0x1016a00` used by `0x543ba0`) | type reading |
|---:|---|---|
| 2,3,4,5 | 1,2,4,8 | INFERENCE: i8,i16,i32,i64 |
| 6,7,8,9 | 1,2,4,8 | INFERENCE: u8,u16,u32,u64 |
| 10, 11 | 4, 8 | INFERENCE: f32, f64 |
| 12 | varint-prefixed string | OBSERVED: 7-bit varint byte length (LSB group first, bit 7 = continue), then the bytes, not NUL-terminated |
| 13 | u64-prefixed blob | OBSERVED by `0x543ba0` when the chunk is a blob; **the 1-byte bool chunks also use tag 13 with no prefix** |

Only tags 4, 8, 10, 12, 13 occur in the profile and progress writers. Writers of 2/4/8-byte values test the stream swap flag
(`cmp byte [stream+8],0` + `bswap`); byte payloads, varint strings and raw blob bytes are never swapped. The swap flag is set to 0 in the
base stream constructor (`0x4d52a`); the only other byte stores to `[reg+8]` in all stream/serialiser code are two setters at `0x4d940/0x4d950`
that have no call, jump, `lea` or relocation reference. The reader primitives `0x8ea20/0x8eae0/0x8eba0` and the whole profile loader
contain no `bswap`. So the on-disk format is native little-endian by construction.

Enum-string chunks (tag 12, written by the 5 helpers): the writer walks a table of 24-byte entries `{name*, alias*, i32 value}` (terminated by a null
name), writes the entry name for the value, and falls back to **decimal text** when the value is not in the table. The reader
`0x548b90` (and inline equivalents) accepts decimal text (`_Stoul`, base 10) or a case-insensitive match (`strcasecmp`) against
name or alias, and **an unknown string resolves to the first table entry's value**. Because decimal text is accepted, a native writer
may emit decimal without embedding the original names; a native reader that must import original saves needs the name tables (policy
decision, not made here).

## 4. Profile object (descriptor id `0xd`, vtable `0x15c9830`: writer slot `+0x50` = `0x548d30`, loader slot `+0x48` = `0x545d30`)

Chunks in **writer order** (the loader reads in a different order; order does not matter for lookup). 33 named chunks, then the indexed block.
Counts: 7 bool/u8, 4 four-byte ints (3 x tag 8, 1 x tag 4), 8 floats, 1 blob, 13 strings/enum-strings, plus the 257 indexed enum-strings.
All 290 hashes are pairwise distinct (verified), so first-match lookup is unambiguous.

| # | hash | tag | payload | writer | reader site(s) | binding (disassembly reading) |
|---:|---|---:|---|---|---|---|
| 1 | `0x31542cc5` | 13 | u64 LE len + bytes | W 0x548da9 | 0x546087 | param blob: u64 len must equal 0x9c (39 x i32) else load aborts, code 6; blob VA 0x325f310 |
| 2 | `0x6a83b721` | 12 | varint len + bytes | W 0x548f35 via 0x54aca0 | 0x54623e | enum-string, table E1 {1,2,3,4}; getter 0x7cd8f0 / setter 0x7ccd20 (called when value != 0) |
| 3 | `0x087a6c73` | 13 | u8 | W 0x548f7d | 0x545f93 | bool; getter 0x7cdc70 / setter 0x7cd970 |
| 4 | `0xf7154f95` | 13 | u8 | W 0x548ff3 | 0x545dd9 | bool; obj+0x47 |
| 5 | `0x6ea5bf45` | 13 | u8 | W 0x54908d | 0x545eb6 | bool; obj+0x48 (writer first forces it to 1 if it is 0 and query 0x7dcfd0() > 0) |
| 6 | `0xf99a0ae3` | 8 | 4 bytes LE | W 0x5492d5 | 0x546520 | i32; getter 0xa56fa0 / setter 0xa56f70 on the singleton at global 0x2eff448 |
| 7 | `0x8b3584f3` | 8 | 4 bytes LE | W 0x549347 | 0x5465d8 | i32; obj+0x34 |
| 8 | `0xf068962f` | 8 | 4 bytes LE | W 0x5493bc | 0x546443, 0x54666b | i32; obj+0x38 (reader looks the chunk up twice, both store obj+0x38; first also calls 0x4c4830) |
| 9 | `0xcb27377e` | 4 | 4 bytes LE | W 0x549432 | 0x546705 | i32 (tag 4); obj+0x3c |
| 10 | `0x37c9d370` | 13 | u8 | W 0x5494aa | 0x54679f | bool; obj+0x46 |
| 11 | `0xb0ddb3f5` | 12 | varint len + bytes | W 0x54967d | 0x5469b7 | string label of enum obj+0x40 (table E2 {0,1,2,3}); reader reads and DISCARDS it |
| 12 | `0xec13f330` | 13 | u8 | W 0x5496f6 | 0x5483b6 | bool; obj+0x44 |
| 13 | `0x0c226ce0` | 13 | u8 | W 0x549776 | 0x54687c | bool; obj+0x45 |
| 14 | `0x16d0eb49` | 12 | varint len + bytes | W 0x5498d2 via 0x54ad70 | 0x546b9f via 0x548b90 | enum-string, table E3 {0,1,2}; global int 0x1604994, default 2 |
| 15 | `0xdeda656c` | 12 | varint len + bytes | W 0x5499e9 via 0x54ad70 | 0x546cbb via 0x548b90 | enum-string, table E3; global int 0x1604998, default 2 |
| 16 | `0x0bfe9eef` | 12 | varint len + bytes | W 0x549b00 via 0x54ad70 | 0x546dd7 via 0x548b90 | enum-string, table E3; global int 0x160499c, default 2 |
| 17 | `0xad062d8f` | 12 | varint len + bytes | W 0x549bed via 0x54ae40 | 0x546ede | enum-string, table E4 {0,1}; global int 0x16049a0, default 1 |
| 18 | `0x60a41121` | 12 | varint len + bytes | W 0x549d01 via 0x54ad70 | 0x54714c via 0x548b90 | enum-string, table E3; global int 0x16049a4, default 2 |
| 19 | `0x92eaa017` | 12 | varint len + bytes | W 0x549e1a via 0x54af10 | 0x547284 | enum-string, table E5 {0,1,2}; global int 0x2f063a8, default 0 |
| 20 | `0x5b6713de` | 10 | f32 LE | W 0x549e5b | 0x547431 | f32; global 0x16049a8, default 1.0 |
| 21 | `0xce3222c6` | 10 | f32 LE | W 0x549ef1 | 0x5474fb | f32; float behind [[0x2efc9e0]+0xa60]; reader keeps it in a local (default 1.0) |
| 22 | `0x2985559e` | 10 | f32 LE | W 0x549f79 | 0x547597 | f32; [[0x2efc9e0]+0xc10]; local default 1.0 |
| 23 | `0x77d20933` | 10 | f32 LE | W 0x54a001 | 0x547633 | f32; [[0x2efc9e0]+0xb38]; local default 1.0 |
| 24 | `0x97b66846` | 12 | varint len + bytes | W 0x54a185 via 0x54afe0 | 0x5477d0 | enum-string, table E6 {0,1,2,3}; int at [[0x2efc9e0]+0xe88] |
| 25 | `0x1478d8e2` | 13 | u8 | W 0x54a1c6 | 0x5479f3 | bool; global byte 0x2dcbfb5 |
| 26 | `0x60da6c9e` | 10 | f32 LE | W 0x54a2da | 0x547c62 | f32; float reached through an object returned by a virtual call |
| 27 | `0x63c5981f` | 10 | f32 LE | W 0x54a37f | 0x547de0 | f32; getter 0x70a680 / setter 0x70a690 |
| 28 | `0x6bfe3e86` | 10 | f32 LE | W 0x54a40e | 0x547e89 | f32; getter 0x70a6a0 / setter 0x70a6b0 |
| 29 | `0x8bfceba4` | 12 | varint len + bytes | W 0x54a4cd | 0x547f36 | string (std::string, from 0x50f620(0x558330(),3,[0x2efc8c8])); reader: 0x50f770 then 0x5582e0 |
| 30 | `0xe85a6984` | 10 | f32 LE | W 0x54a53d | 0x548039 | f32; getter 0x5583a0 / setter 0x5583b0 |
| 31 | `0xdc913ac6` | 12 | varint len + bytes | W 0x54a5c2 | 0x5480f6 | string (0x50f620(0x558390(),3,[0x2efc8cc])); reader: 0x50f770, 0x558340 |
| 32 | `0xb1b4ea0c` | 12 | varint len + bytes | W 0x54a69f | 0x547acd | enum-string over a runtime registry (lookup 0x52b8f0), decimal fallback; value 0xa2e9f0(global 0x32f4fe0, 0) |
| 33 | `0x9a50a3bc` | 12 | varint len + bytes | W 0x54a771 | 0x547b92 | same as above, index 1 |
| 34 | 257 hashes (see below) | 12 | varint len + bytes | W 0x54a995 (variant A) or 0x54abe3 (variant B) | 0x5487df | name = prefix + decimal index 0..256; value = enum-string (table E7 {0,1,2}); source obj+0x4c+4*i |


Enum tables (value sets; all names omitted): E1 `0x2eff4d0` {1,2,3,4}, E2 `0x2eff680` {0,1,2,3}, E3 `0x2eff550` {0,1,2}, E4 `0x2eff5c0` {0,1},
E5 `0x2eff610` {0,1,2}, E6 `0x2efcb70` {0,1,2,3}, E7 `0x2eff460` {0,1,2} (each followed by a null-name sentinel entry; the value sets are
re-derived by `verify.py` from the init code bytes).

**Indexed block (257 chunks, tag 12 enum-string, table E7).** Chunk name = an 8-character fixed prefix followed by the decimal index `i` = 0..256.
Because FNV-1a is incremental, the hashes are reproducible without publishing the prefix: state after the prefix `S0 = 0x0b45d343`, then
`h_i = fold(S0, decimal(i))` with `h = (h ^ ch) * 0x01000193` per digit. First/last: `h_0 = 0x31ebde09`, `h_1 = 0x30ebdc76`, `h_256 = 0x79b2a7bc`;
SHA-256 of the 257 newline-joined lower-case `0x%08x` values: `9fa2d6ad71546d7515f9f0c3f84434d30443b1c523696dce651eed1f87d3e1d9`. Source array: `obj+0x4c + 4*i` (257 x i32 = 0x404 bytes, ending at the `+0x450` field).
Writer variant A (taken when any of three string keys is present in a global map at `0x2eea478`) writes the entries as they are; variant B
first replaces each zero entry by the matching entry of the progress singleton's array (`[0x2eff448]+0x48+4*i`) (OBSERVATION-READING). The loader
(one loop) stores the parsed enum value for every found tag-12 chunk and leaves the entry unchanged otherwise; a found-but-unparsable string stores the
table's first value.

**Profile loader behaviour (OBSERVATION-VERIFIED unless noted).**
1. First step: class descriptor check (`0xa515a0`), mismatch -> return 6 with nothing read.
2. Reads three leading byte chunks (two stored straight into the object, one handed to a setter at once), then the param blob: tag must be 13 and the **u64 prefix must equal `0x9c`**, otherwise
   the load aborts with return 6 *after* those earlier effects already happened (no rollback). A matching blob is read with one `0x9c`-byte read into the blob at `0x325f310`.
3. Before the helper-driven enum reads the loader resets seven global settings to defaults (ints at `0x1604994/98/9c = 2`, `0x16049a0 = 1`, `0x16049a4 = 2`, `0x2f063a8 = 0`, float `0x16049a8 = 1.0`) (signature-verified, values OBSERVATION-READING).
4. Chunk `0x10174c3`'s string is read and destroyed unread (signature-verified).
5. Success sets `obj+0x32 = 1` (the writer also sets it at its end) and returns 1 (OBSERVATION-READING for the `+0x32` store; the return value 1 is in the signature-verified range).

## 5. Bool chunks (progress-side flag sync; `0x541ba0`, called by the progress writer `0x543fcf` and loader `0x5436f2`)

Each call `0x7e4b50(stream, name, flag_index)` does: `live = (0x7dbba0(flag_index) != 0)`; run the unified (de)serialiser `0x7e6b50` on `live`
(mode at `[stream+0x58]`: 1 = write, 2 = read; write = begin tag 13, **one byte**, end; read = find, tag must be 13, one byte, `!= 0`); then set or clear
**bit `flag_index & 7` of byte `flag_index >> 3` of the game-data blob at `0x3260bb8`** to match the (possibly just-read) value (OBSERVATION-READING for the bit
arithmetic; call structure OBSERVATION-VERIFIED). This resolves the earlier "how the 48 flags map to blob bits" unknown: flag i is blob bit i.
A missing/retagged chunk keeps the live value. Indices form three runs of 16: `0x78..0x87`, `0xa5..0xb4`, `0xd9..0xe8`. 48 records x 13 bytes = 624 bytes, written before the game-data chunk.

| order | flag idx | name hash |
|---:|---:|---|
| 1 | `0x7d` | `0x10a37023` |
| 2 | `0x7c` | `0x927711d9` |
| 3 | `0x78` | `0xfd9dc6cd` |
| 4 | `0x80` | `0xba3e8241` |
| 5 | `0x79` | `0x95f710eb` |
| 6 | `0x7e` | `0x11a371b6` |
| 7 | `0x86` | `0xec9dae76` |
| 8 | `0x85` | `0x4a87cd7a` |
| 9 | `0x7a` | `0xc01c3c76` |
| 10 | `0x83` | `0xd9c55811` |
| 11 | `0x81` | `0x85f0e1e4` |
| 12 | `0x82` | `0x5189c431` |
| 13 | `0x7b` | `0xd281cf5a` |
| 14 | `0x84` | `0x7cadc4ef` |
| 15 | `0x7f` | `0x12a37349` |
| 16 | `0x87` | `0xc187e05f` |
| 17 | `0xa5` | `0xb4de1ce1` |
| 18 | `0xa6` | `0x9409ff84` |
| 19 | `0xa7` | `0xf3265c03` |
| 20 | `0xa8` | `0xaa10c206` |
| 21 | `0xa9` | `0xc503c3b5` |
| 22 | `0xaa` | `0xf837dbf8` |
| 23 | `0xab` | `0xe96c15a7` |
| 24 | `0xac` | `0xca6e6e9a` |
| 25 | `0xad` | `0x229511f9` |
| 26 | `0xae` | `0xfa59a5b5` |
| 27 | `0xaf` | `0xdf66a406` |
| 28 | `0xb0` | `0x1ec1f7a7` |
| 29 | `0xb1` | `0x2d8dbdf8` |
| 30 | `0xb2` | `0xea33fee1` |
| 31 | `0xb3` | `0x73e4cf02` |
| 32 | `0xb4` | `0x287c3e03` |
| 33 | `0xd9` | `0x1a3c4083` |
| 34 | `0xda` | `0xd13ea686` |
| 35 | `0xdb` | `0x0140810d` |
| 36 | `0xdc` | `0x633609a8` |
| 37 | `0xdd` | `0xba217017` |
| 38 | `0xde` | `0xd7285f9a` |
| 39 | `0xdf` | `0xf4490491` |
| 40 | `0xe0` | `0x96b5972c` |
| 41 | `0xe1` | `0x5cfdc54b` |
| 42 | `0xe2` | `0x6424d235` |
| 43 | `0xe3` | `0x23dbcdee` |
| 44 | `0xe4` | `0x440c13ab` |
| 45 | `0xe5` | `0x12f1808c` |
| 46 | `0xe6` | `0x2f1ce079` |
| 47 | `0xe7` | `0xaae7f6c2` |
| 48 | `0xe8` | `0x2ede573f` |

## 6. Progress object (descriptor id `4`, vtable `0x15c97a8`: writer `0x543f40`, loader `0x542880`) - table only

13 named chunks plus the same 257-chunk indexed block (source: the progress object's own array `obj+0x48+4*i`; this is the array the profile writer's variant B copies from), plus the 48 bools above. The loader requires the
game-data chunk (`0xf24056ef`) to be tag 13 with u64 prefix `0x3cd8` (prior checkpoint). Chunk `0x2a708ecf` (tag 4) is an element count N and chunk
`0xfb61bc89` (tag 13) is a u64-prefixed array of N raw LE u32 (prefix = 4N, elements not swapped); the loader checks `prefix == 4N` via `0x543ba0`.
Field semantics of the progress chunks are **not decoded**.

| name hash | tag | payload | writer | reader |
|---|---:|---|---|---|
| `0xf24056ef` | 13 | u64 LE len + bytes | `0x54405e` | `0x543671` |
| `0x8c36c89b` | 8 | 4 bytes LE | `0x5440e3` | `0x543335` |
| `0x9b650877` | 8 | 4 bytes LE | `0x544150` | `0x542a31` |
| `0x0ccea4ff` | 13 | u8 | `0x5441bd` | `0x542ad8` |
| `0x1d2abf79` | 13 | u8 | `0x544239` | `0x54292b` |
| `0xbf73d5a9` | 13 | u8 | `0x5442b5` | `0x542bb9` |
| `0xe2922fb0` | 8 | 4 bytes LE | `0x544339` | `0x542ca2` |
| `0x79108b37` | 8 | 4 bytes LE | `0x544428` | `0x543936` |
| `0xd57202c7` | 8 | 4 bytes LE | `0x5444a3` | `0x543ac4` |
| `0x0acace09` | 10 | f32 LE | `0x54451a` | `0x543771` |
| `0x025bf0e9` | 10 | f32 LE | `0x5445b0` | `0x543860` |
| `0x2a708ecf` | 4 | 4 bytes LE | `0x5448fe` | `0x54312e` |
| `0xfb61bc89` | 13 | u64 LE len + bytes | `0x54497c` | `0x543440` |

## 7. Findings (b) and (c) in detail

**(b) The u64 length prefix is inside `payload_length` (OBSERVATION-VERIFIED, two sides).** Writer: the param-blob site (`0x548da9`..`0x548de9`) calls
the raw writer with size 8 (prefix) and then with the blob bytes, both between chunk-begin and chunk-end; chunk-end computes `total - start - 12`, so
`payload_length = 8 + 0x9c = 0xa4` for that chunk. The progress game-data chunk is the same shape: `payload_length = 8 + 0x3cd8`, record = 12 + 8 + 15576 = 15596 bytes, which matches the earlier
"15596 bytes total". Reader: `0xa81180` builds the sub-stream over exactly `payload_length` bytes and the caller then reads the u64 from that sub-stream (`0x8eba0`).
agentD's `payload_length` definition (bytes after the 12-byte header) is therefore correct.

**(c) Trailing bytes and header dword 3 (OBSERVATION-VERIFIED).**
- *File level.* The load path reads exactly 0x10 header bytes and then exactly `dword2` body bytes; between the body `fread` and `fclose` the only calls are the
  allocator, buffer helpers, `fread`, `fclose` and the hash (no `ftell/fseek/feof/...` thunk occurs anywhere in the load/save function; two `fread` calls in total).
  Bytes after `0x10 + dword2` are never examined.
- *Body level.* The class loader is handed a stream over exactly `dword2` bytes (`0xa57365`, slot `+0x48`, result compared with 1). Lookup is by name, so
  non-chunk bytes after the last chunk are ignored (scan stops at the first record without magic `0xBEEF`), duplicates resolve to the first record, and chunks
  that are absent or have a different tag are skipped. The original tolerates garbage tails and unknown chunks; it does **not** require the chunks to tile the body.
- *Differences to agentD's fixture `parse_save_stream`:* it rejects trailing bytes and requires chunks to tile the body (stricter than the original); duplicates are accepted by default
  (original: first wins); payload bytes are copied out as opaque vectors; the typed readers added later must be bounded to `payload_length` exactly like the original's sub-stream. Keep strict mode for
  native-written files; an optional "import" mode matching the original semantics is needed only for original saves.
- *Dword 3.* Not checked by the file loader (earlier checkpoint), but both class loaders call `0xa515a0` first and refuse with 6. Profile `0xd`, progress `4`; the Save request stores `*descriptor` as dword 3
  (`0xa56118`) and the Load request stores the descriptor pointer in `[obj]` (`0xa55940`).

## 8. Proposed typed native schema (NOT implemented; for the integrator)

Container (extends `native/core/save_stream`): keep the 16-byte header and 12-byte chunk header; add (1) `find_first(name_hash)` with the original first-match semantic and a bounded
payload span; (2) schema-level check `header.dword3 == descriptor_id` (profile `0xd`, progress `4`); (3) typed payload readers over a payload span that **fail on short reads** (the original silently
leaves the destination uninitialised) for `u8/bool`, `i32/u32`, `f32`, `u64-prefixed bytes`, `varint-string`; (4) optional import-mode tail leniency; (5) little-endian only, no swap flag.

```cpp
enum class ChunkTag : std::uint16_t { I8=2, I16=3, I32=4, I64=5, U8=6, U16=7, U32=8, U64=9, F32=10, F64=11, Str=12, Bytes=13 };   // sizes OBSERVED, signedness INFERRED
enum class PayloadKind : std::uint8_t { Bool8, I32, U32, F32, Str, EnumStr, BlobU64, ArrayU32U64 };
struct ChunkSpec { std::uint32_t name_hash; ChunkTag tag; PayloadKind kind; };          // constexpr tables: 33 profile, 13 progress, 48 flags
constexpr std::uint32_t kProfileDescriptor = 0xd, kProgressDescriptor = 4;
constexpr std::uint32_t kIndexPrefixState = 0x0b45d343;                   // h_i = fold(kIndexPrefixState, decimal(i)), i in [0,257)

struct ProfileV1 {                                   // field names are neutral on purpose; map by chunk hash
  std::array<std::int32_t, 39> params{};             // 0x31542cc5: u64 len must be 0x9c; 39 x i32 LE
  std::array<bool, 7>          bools{};              // 0x087a6c73 0xf7154f95 0x6ea5bf45 0x37c9d370 0x0c226ce0 0xec13f330 0x1478d8e2
  std::array<std::int32_t, 4>  ints{};               // 0xf99a0ae3 0x8b3584f3 0xf068962f (tag 8), 0xcb27377e (tag 4)
  std::array<float, 8>         floats{};             // tag-10 chunks in table order (defaults 1.0f for four of them)
  std::array<std::uint8_t, 8>  enums{};              // table rows 2,14-19,24 (+ label row 11: written, read back and discarded)
  std::array<std::string, 4>   strings{};            // 0x8bfceba4 0xdc913ac6 plus two registry-name strings 0xb1b4ea0c 0x9a50a3bc
  std::array<std::uint8_t, 257> unlock_state{};      // indexed block, enum 0..2
};
```
Read policy: absent / wrong tag / unparsable -> keep default (enum text unknown -> table's first value); blob length mismatch on the param chunk -> hard error;
no rollback guarantees to be copied. Write policy: emit chunks in the writer order of section 4, LE, `payload_length` = bytes after the 12-byte header (prefix included for blobs),
enum values as decimal text unless the name-import decision is taken. Golden vectors must be synthetic (built from this spec), never derived from game data.

## 9. Unresolved, inferred, not attempted

- **UNRESOLVED - tag 4 vs tag 8.** Hypotheses tried and falsified: H1 different writer primitive (both use the same 4-byte swap-aware write), H2 different reader primitive (both `0x8eae0`), H3 different size in the generic size query (both 4).
  Signedness remains INFERENCE from the 2..5 / 6..9 grouping. No impact on the byte layout.
- INFERENCE: `_Getptoupper` is ASCII `toupper` (section 3.3). A real save file or an offline look at the libc module would confirm; neither was used (scope).
- Not decoded: semantics of the field sinks/getters/setters in the table (only addresses recorded), the consumer of the three float locals and the enum E6 value (`0x526590`), the progress object's chunk meanings, the dynamic registry behind chunks `0xb1b4ea0c`/`0x9a50a3bc`,
  the three-key map that selects the indexed-block variant, the meaning of the return value 6 beyond "failure".
- Not claimed: any game save compatibility, any native serializer, any real-save round trip. The original reader's undefined behaviour on empty bodies and overlong records is documented, not reproduced.

Failed or abandoned hypotheses (none reached three on one sub-question): (1) `0xa47d60` is the load function - wrong (it is the mount/search path; the loader is in `0xa48280`); (2) the first name-extraction heuristic mis-paired ~6 sites (tool bug, replaced by slot/length pairing with a strlen cross-check);
(3) tag 4 vs 8 (above).

## 10. Commands (CWD `/Volumes/Extra/SoC Mac Native/local/re/evidence/sav01-profile-chunks-01`; all exit 0)

```sh
python3 -I build_claims.py      # exit 0  (two-pipeline cross-check asserts)
python3 -I verify.py            # exit 0  (232 checks, 0 failed)  -> verification.json
python3 -I mutation_test.py     # exit 0  (47/47 caught) -> mutation_results.json
python3 -I render_tables.py     # exit 0
shasum -a 256 -c SHA256SUMS     # exit 0  (54 files)
python3 -I scan_hash_constants.py  # exit 0  (0 hits)
python3 -I make_checkpoint.py   # exit 0  (writes this document)
```

Files changed by this task: only `local/re/evidence/sav01-profile-chunks-01/` (private) and this checkpoint. `scripts/project_index.py` and the trail refresh were not run (no indexed tree changed).

## 11. Recommended TASKS.md row and next native step

`| SAV-01-PROFILE-CHUNKS-01 | Decode profile chunk writers/loader, generic chunk lookup, bool chunks, outer-header dword 3 and trailing-byte handling | SAV-01-BLOB-FLUSH-01, SAV-01-LOADER-01 | STATIC_ANALYSIS_VERIFIED (232 byte checks, 47/47 mutations caught; 33+257 profile chunks, 48 bool chunks, 13+257 progress chunks tabulated; u64 prefix inside payload_length; dword 3 = descriptor id checked by class loaders; tag 4/8 distinction UNRESOLVED; see docs/checkpoints/2026-10-21-agentH-sav01-profile-chunks.md) / agentH | 0 task-level (sub-question tag 4 vs 8: 3 falsified hypotheses, recorded UNRESOLVED) |`

Next bounded native step (`SAV-01-CHUNK-CODEC-01`, fixture only): add `native/core/save_chunk_codec.{hpp,cpp}` with bounded typed readers/writers (`u8`, `u32`, `f32`, `u64`-prefixed bytes, varint string, enum-string with decimal fallback) over a payload span, `find_first`
on the existing parsed chunk list, and constexpr spec tables (hash, tag, kind) from this checkpoint; tests are synthetic golden vectors: u64 prefix inside `payload_length`, first-match duplicates, tag-mismatch default, short-read rejection, empty body,
overlong record, tail bytes, descriptor-id gate (`0xd`/`4`), varint boundaries (127/128/16383/16384), LE only. Decision needed from the user before enum import: embed the original enum name tables as data, or accept decimal-only on both sides.
