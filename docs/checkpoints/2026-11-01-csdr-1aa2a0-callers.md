# Checkpoint 2026-11-01: callers of FUN_001aa2a0 (Oodle dispatch) and the `.csdr` header word

Worker task: read-only static analysis of the pinned `local/re/eboot.elf`
(sha256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`, image base 0, file
offset = VA + 0x4000 in all three image-mapping phdrs). Nothing was loaded, relocated or executed.
No failed tries were spent on this task (try count 0).

Gate label for the whole checkpoint: `STATIC_ANALYSIS_VERIFIED` for the call graph, switch table and
loader wiring; `ARTIFACT_VERIFIED` (counts only) for the archive trailer census; `NOT RUN` for any actual
Oodle/zlib decode of a member.

## Answers

### 1. Who calls FUN_001aa2a0?  Exactly one site.

`FUN_001aa2a0` is `bpe::Compression::Decompress`: FDE `0x1aa2a0 + 0x1ca` (end `0x1aa46a`), 458 bytes,
body sha256 `fe721cceb...583734`.

| class | result |
|---|---|
| `E8` call rel32 (any byte offset, all three segments) | `0x147257` only |
| `E9` jmp rel32 | none |
| `0F 8x` jcc rel32 | none |
| `lea reg,[rip+disp]` (REX optional) | none |
| 64-bit literal of `0x1aa2a0` in the image bytes | none |
| 32-bit literal | none |
| RELA `R_X86_64_RELATIVE` with addend `0x1aa2a0` | none |
| other rip-displacement coincidences | `0x147258` (the call's own disp32), `0xfadd48` (last entry of the size switch table, coincidence), `0x1aa2db` (interior of an instruction inside the function itself); none is a `lea` |

The containing function is taken from the `.eh_frame_hdr` FDE table (55,790 entries), not from
Ghidra's procedure list: `0x147257` is inside FDE `0x147160 + 0x32e` (`FUN_00147160`,
`ResourceFactory::CreateResource`), and that FDE is the only one covering it. The Ghidra
`getReferencesTo(0x1aa2a0)` agrees (`CALLER from=0x147257 ... CALLERCOUNT 1`,
`local/validation/csdr-1aa2a0-callers/callers-ghidra.txt`).

How `FUN_00147160` itself is reached (observations, also from the census):
`call` at `0x146c53` (inside `FUN_00146b60`), a self-call at `0x1473a0` (retry after
`Failed to create resource`), and a function-pointer slot at `0x15feb00` (RELATIVE reloc, addend
`0x147160`) consumed by the resource-loader thread `FUN_00148600`. Ghidra's two DATA references at
`0x14879d` / `0x148cbe` are not references to `0x147160` in the bytes (`0x14879d` is a `vmovdqa` to a
stack slot; the raw census finds no `lea`/literal there), so they are stack-variable artefacts.
The producer side of the header word is covered below; `Set(hdr, tag, size)` at `0x1aa0a0` has zero
references (dead code), so the runtime only ever reads the header.

Also pinned by the same census: `OodleLZ_Decompress` (`0x89d3b0`) has one call, `0x1aa426`;
`FUN_00144880` (resource type create) has one call, `0x1472dc` (in `FUN_00147160`);
zlib `uncompress` (`0x1b9130`) has one call, `0x1aa3ee`.

### 2. Where the 3-bit tag comes from, and what the six cases do

The "tag" is the top three bits of one u32, the `bpe::Compression::CompressionHeader` word:

```
word = *(u32 *)(request + 0xf0)       // the 4th and last dword of the 12-byte member trailer
tag  = word >> 29                     // 0x1aa0d0 / 0x1aa250  GetTag
size = word & 0x1fffffff              // 0x1aa0e0 / 0x1aa240  GetSize   (decoded size)
```

Dispatch (`0x1aa333..0x1aa34f`): `shr eax,0x1d; cmp al,5; ja exit` then
`movsxd rax,[0xfadd4c + 4*tag]; add rax,0xfadd4c; jmp rax`. The six table entries decode to
`[0x1aa35c, 0x1aa3dc, 0x1aa351, 0x1aa351, 0x1aa351, 0x1aa3f5]`.

| tag | body | action |
|---|---|---|
| 0 | `0x1aa35c` | `memcpy(dst, compBuf, size)` via `0xf8f470` (after a log gate `0x1aa37f..0x1aa3c6`) |
| 1 | `0x1aa3dc` | `0x1b9130(dst, &destLen = rawLen, compBuf, compSize)` = zlib 1.2.3 `uncompress` (`inflateInit_` with `"1.2.3"`, struct size `0x58`, `inflate(.., Z_FINISH)`, `inflateEnd`) |
| 2, 3, 4 | `0x1aa351` | `r8d = 1` (fuzzSafe) then the Oodle block |
| 5 | `0x1aa3f5` | `r8d = 0` (fuzzSafe off) then the Oodle block |
| 6, 7 | none | `ja 0x1aa42f`: nothing is decoded; returns `size` anyway |

Oodle block (`0x1aa3fb..0x1aa42b`), one call at `0x1aa426`:
`OodleLZ_Decompress(compBuf = rsi-arg, compSize = rdx-arg, rawBuf = rcx-arg, rawLen = size,
fuzzSafe = r8d, checkCRC = 0, verbosity = 0, decBufBase = 0, decBufSize = 0, fpCallback = 0,
callbackUserData = 0, decoderMemory = [0x2ee6720], decoderMemorySize = [0x2ee6748], threadPhase = 3)`.
The 14-parameter shape (6 registers plus 8 pushes: `3, [0x2ee6748], [0x2ee6720], 0, 0, 0, 0, 0`)
matches the Oodle 2 signature. The function takes a lock (`0x326f0` on `0x2ee6728`), adds the decoded
size to a counter at `0x2ee6750`, unlocks (`0x32710`) and returns `size`.

Helper `FUN_001aa260` (decoded size for allocation): tags 0..4 return `size`; tag 5 tail-jumps to
`0x89dad0(size, 0)` (an Oodle decode-buffer-size helper; its body was not examined); 6/7 return 0.
`FUN_001aa210` (`IsCompressed`): `word > 0x1fffffff`, i.e. `tag != 0`.

### 3. Is `compBuf` an archive-member buffer for a `.csdr` member?

Yes for the resources actually loaded through this path, with the caveat in "Unresolved". The chain:

1. **Descriptor** (`FUN_00b0d3e0`, `0xb0d3e0`): builds the resource-type descriptor for `Shader` /
   suffix `_ps4`, sets `desc[0x28] = 0x1343534452` (the 8-byte tag `52 44 53 43 13 00 00 00`,
   "RDSC" + 0x13), `or byte [rbp-0x60],8` i.e. `desc[0x70] |= 8` ("stored compressed"), loader callback
   `FUN_00b0cc50`, and registers it under `esi = 0x63736472` ('csdr') through `0x1444e0`.
   The descriptor ctor `0x144640` clears bit 3 and zeroes `desc[0x28]` first (`and r14d,0xfffffff7`,
   `mov qword [rbx+0x28],0`), so these two stores are the only source.
2. **Request build** (`FUN_0014aee0`): if `desc[0x70] & 8` then `request[0xe0] = 0xc` (trailer length 12).
3. **Read** (`FUN_0014b980`): the file object at `request+0x18` is read into `request[0x90]`
   (size `request[0x78]`) through vtable `+0x80`; then `memcpy(request+0xe8, buf + size - 12, 12)`
   (`0x14bc5a`) and the first 8 bytes are compared with `desc[0x28]`; a mismatch sets `request[0x9c]` to 3
   (low dword) or 4 (high dword). On success `request[0x80] = request[0x78] - 12`. So the trailer's
   last u32 lands exactly at `request+0xf0`, and the data buffer keeps the member's first bytes with the
   trailer excluded from the size.
4. **Consumer** (`FUN_00147160`): if `desc[0x70] & 8` and `IsCompressed(request+0xf0)` then
   `alloc(DecodedSize(hdr), 1, desc[0x54])` and
   `Decompress(hdr = request+0xf0, compBuf = [request+0x90] (via 0x14c000), compSize = [request+0x80], dst, rawLen)`,
   otherwise the raw buffer is passed on (`0x14c020`). Consequences: tag 0 members never reach
   `0x1aa2a0` from this sole caller (the memcpy case is unreachable here), and tags 3/4 reach the Oodle
   call with `compBuf` = the member bytes minus the 12-byte trailer.
5. **Data** (`ARTIFACT_VERIFIED`, counts only, `--archives` mode of the checker, 34,392 `.csdr` members of
   `pak0`, `pak1`, `pakN`, `cachedFiles`): all 34,392 end in the 8-byte `RDSC` tag; header-word tag
   counts are `{0: 15072, 3: 16328, 4: 2992}` (tags 1, 2, 5, 6, 7 never occur);
   for tag 0 the size field equals the payload length (member length - 12) in all 15,072; for tags 3/4 the
   size field exceeds the payload in all 19,320 (so it is the decoded size). The first two member bytes
   are `0100`/`0200` (8,178 / 6,894) for tag 0, `8c0a` (16,328) for tag 3 and `8c06` (2,992) for tag 4.

So the answer to "does the Oodle call expand `.csdr` members" is yes for 19,320 of 34,392 members
(56 percent); the other 15,072 are stored and never enter `FUN_001aa2a0`.

## Observations versus inference

Observations (bytes or counts, reproducible by the checker): everything above in tables, the descriptor
stores, the trailer memcpy, the `request+0xf0` consumer, the tag/size split, the six table entries, the
Oodle argument layout, the single-caller census, the FDE extents, and the 34,392-member trailer census.

Inference (not directly executed): that `8c0a` / `8c06` are Oodle block headers (Mermaid / Kraken, by the
Oodle block-header convention: byte 0 low nibble `0xc`, byte 1 the decoder id); that 0x89d3b0 is
`OodleLZ_Decompress` (14-parameter shape and the Oodle 2 LZ strings; no symbol); that
`FUN_00b0cc50` consumes the decoded buffer (it is a separate task, see the sibling checkpoints).

## Unresolved

* The file object behind `request+0x18` (vtable `+0x80` read) was not traced into the PSARC reader; that
  the buffer is a PSARC member is established by the data census (every PSARC `.csdr` member carries the
  trailer) and the path (`materials/!cookedshaders/...`), not by following the VFS vtable.
* No Oodle or zlib decode of any member was run (`NOT RUN`); the checker is byte/graph only. The
  `8c0a` / `8c06` decoder identity is inference.
* Relative jump-table entries that could target `0x1aa2a0` are not enumerated generally; only the two
  tables inside `0x1aa260` / `0x1aa2a0` are decoded. No such table was found by the raw census (no
  literal, no lea, no reloc), and a tail-jump through a table would need a `lea` of the table base.
* `0x89dad0` (tag 5 decode-buffer size), the initialisers of the decoder-memory cells `0x2ee6720` /
  `0x2ee6748` (`FUN_001aa0f0`, `FUN_001aa160`) and tag 1's use of `rawLen` (`r13d`) were read but not
  characterised beyond what is listed.
* Tags 1, 2, 5, 6, 7 do not occur in the `.csdr` corpus; their behaviour is from code only.

## Reproduction

```sh
python3 tools/lift/check_csdr_oodle_dispatch.py                 # exit 0: 167 pin mutants + 7 detector
                                                                #  probes + 2 injection probes
python3 tools/lift/check_csdr_oodle_dispatch.py --archives --no-mutate   # exit 0: archive census
python3 tools/lift/check_csdr_oodle_dispatch.py --tamper elf    # exit 1 (expected)
python3 tools/lift/check_csdr_oodle_dispatch.py --print-census  # raw census, no assertions
```

Evidence (private, under `local/`): `local/validation/csdr-1aa2a0-callers/` (`callers-ghidra.txt`,
`census.txt`, `checker-full.txt`, `checker-archives.txt`, `checker-tamper.txt`, `dossier-request-*.txt`);
the sibling dossiers in `local/validation/csdr-decompressor-re/` (including `tail-census.json`, an
independent count of the same trailers by another worker, equal to this checker's counts).
