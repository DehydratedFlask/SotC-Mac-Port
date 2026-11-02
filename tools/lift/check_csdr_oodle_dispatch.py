#!/usr/bin/env python3
"""ELF tie for the `.csdr` -> bpe::Compression::Decompress (0x1aa2a0) -> OodleLZ_Decompress path.

Offline and read-only. Bytes of the pinned private ELF (`local/re/eboot.elf`, sha256 d0723afb...5861d3)
are read through `slot_pin_util.Elf`; nothing is loaded, relocated or executed. Skips cleanly (exit 0)
when the private ELF is absent; a present-but-different ELF is a FAIL.

Question answered (static analysis only):

  1. Who references FUN_001aa2a0?  A whole-image census over the three image-mapping segments
     (PT_LOAD phdr 0, RELRO phdr 1, PT_LOAD phdr 2) for: E8 call rel32, E9 jmp rel32, 0F 8x jcc rel32,
     `lea reg,[rip+disp]` (REX optional), any other rip-relative disp coincidence (classified, never
     silently dropped), 64-bit literals, 32-bit literals and RELA R_X86_64_RELATIVE addends. The
     containing function of every hit comes from the .eh_frame_hdr FDE table (55,790 entries), not from a
     disassembler's procedure list, so a "no procedure" hole cannot hide a caller.
     Result: exactly one reference, the `call` at 0x147257 inside the FDE [0x147160, +0x32e).
  2. Where does the 3-bit tag come from and what do the six cases do?  `tag = *(u32*)(request+0xf0) >> 29`,
     `size = *(u32*)(request+0xf0) & 0x1fffffff`. The relative switch table at 0xfadd4c is decoded from
     the bytes: tag 0 -> memcpy (0xf8f470), tag 1 -> zlib 1.2.3 uncompress (0x1b9130), tags 2/3/4 ->
     OodleLZ_Decompress(fuzzSafe=1), tag 5 -> OodleLZ_Decompress(fuzzSafe=0), tags 6/7 -> no decode.
  3. Is `compBuf` a `.csdr` archive-member buffer?  The `csdr` resource-type descriptor (0xb0d3e0) sets the
     descriptor +0x70 bit 3 ("stored compressed") and the descriptor +0x28 trailer tag 0x1343534452. The
     resource loader (0x14aee0 / 0x14b980) then reads the file, copies the last 12 bytes to request+0xe8
     (so the trailer's last u32 lands at request+0xf0 = the header word), and shrinks request+0x80 to
     file_size - 12. The consumer 0x147160 passes (hdr = request+0xf0, compBuf = [request+0x90],
     compSize = [request+0x80]) to 0x1aa2a0 only if descriptor+0x70 bit 3 is set AND the header word is
     > 0x1fffffff (tag != 0); that gate makes the memcpy case unreachable from the sole caller.

Mutation probes (`--no-mutate` skips them): every recorded ELF pin has its first and last byte flipped and
the check must fail; the whole-image census is re-run once with synthetic references injected into padding
and must report exactly them; and a small synthetic image proves each detector class fires. `--tamper elf`
runs on a deliberately altered input and therefore exits 1: a checker that cannot fail proves nothing.

Optional (`--archives`): aggregate-only trailer census of every `.csdr` member of the private PSARCs under
local/extracted/CUSA08809 (counts only; no member bytes are stored or printed).
"""

from __future__ import annotations

import bisect
import re
import struct
import sys
from array import array
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from slot_pin_util import Elf, ROOT, digest, require  # noqa: E402

EH_FRAME_HDR = 0x150C08C

# --- addresses -------------------------------------------------------------------------------------
DECOMPRESS = 0x1AA2A0
DECOMPRESS_SIZE = 0x1CA
DECOMPRESS_SHA = None            # filled by --print (pinned below once derived)
SWITCH_TABLE = 0xFADD4C
SIZE_TABLE = 0xFADD34
CALLER = 0x147160
CALLER_RANGE = 0x32E
CALL_SITE = 0x147257
OODLE = 0x89D3B0
OODLE_CALL_SITE = 0x1AA426
ZLIB_UNCOMPRESS = 0x1B9130
MEMCPY = 0xF8F470
LOCK = 0x326F0
UNLOCK = 0x32710
STACK_CHK_FAIL = 0xF8F3A0
TYPE_CREATE = 0x144880
TYPE_CREATE_SITE = 0x1472DC
DESC_CTOR = 0x144640
DESC_REGISTER = 0x1444E0
CSDR_REGISTRATION = 0xB0D3E0
CSDR_LOADER_CB = 0xB0CC50
DECODE_BUFFER_SIZE = 0x89DAD0
OODLE_STACK_ARG_DECODER_MEM = 0x2EE6720
OODLE_STACK_ARG_DECODER_MEM_SIZE = 0x2EE6748
STATS_COUNTER = 0x2EE6750
LOCK_OBJECT = 0x2EE6728
SWITCH_TARGETS = [0x1AA35C, 0x1AA3DC, 0x1AA351, 0x1AA351, 0x1AA351, 0x1AA3F5]
SIZE_TARGETS = [0x1AA27E, 0x1AA27E, 0x1AA27E, 0x1AA27E, 0x1AA27E, 0x1AA288]

# --- census expectations ---------------------------------------------------------------------------
# {target: (name, e8, e9, jcc, lea, q64, d32, reloc)} -- every hit class except the classified "loose"
# rip-displacement coincidences, which are reported and (for the primary target) pinned below.
CENSUS = {
    DECOMPRESS: ("FUN_001aa2a0 bpe::Compression::Decompress", [0x147257], [], [], [], [], [], []),
    OODLE: ("OodleLZ_Decompress", [0x1AA426], [], [], [], [], [], []),
    TYPE_CREATE: ("FUN_00144880 resource type create", [0x1472DC], [], [], [], [], [], []),
    CALLER: ("FUN_00147160 CreateResource", [0x146C53, 0x1473A0], [], [], [], [0x15FEB00], [0x15FEB00],
             [0x15FEB00]),
    ZLIB_UNCOMPRESS: ("FUN_001b9130 zlib uncompress", [0x1AA3EE], [], [], [], [], [], []),
}
# Loose rip-displacement coincidences for the primary target: a 4-byte window whose rip-relative
# reading lands on the target but whose instruction context is not a `lea` (and is not the call's own
# displacement). They are bytes inside other instructions / a relative switch table, not references.
LOOSE_PRIMARY = {0xFADD48: 0, 0x1AA2DB: 2}

# --- byte pins (address, hex, what) ---------------------------------------------------------------
PINS = [
    (0x1AA2A0, "554889e54157415641554154534883ec38", "FUN_001aa2a0 prologue (first byte of the FDE extent)"),
    (0x1AA469, "90", "first padding byte, last byte of the FDE extent [0x1aa2a0, 0x1aa46a)"),
    # FUN_001aa2a0: argument save, mask, lock, tag extraction, bounds check, jump through the table
    (0x1AA2B8, "48894da04d89c54989d44989f74989fe", "arg save rcx->[rbp-0x60] r8->r13 rdx->r12 rsi->r15 rdi->r14"),
    (0x1AA31E, "418b1e", "ebx = [r14] (header word)"),
    (0x1AA328, "81e3ffffff1f", "ebx &= 0x1fffffff (size)"),
    (0x1AA333, "418b06c1e81d3c050f87ee000000", "eax = [r14] >> 29; cmp al,5; ja exit"),
    (0x1AA341, "488d0d043ae000486304814801c8ffe0", "lea table; movsxd; add; jmp rax"),
    # case bodies
    (0x1AA351, "41b801000000e99f000000", "tags 2/3/4: r8d = 1 (fuzzSafe) ; jmp Oodle block"),
    (0x1AA3CB, "488b7da04c89fe4889dae89650de00eb53",
     "tag 0: memcpy(dst=[rbp-0x60], src=r15, n=rbx) ; jmp exit"),
    (0x1AA3DC, "488b7da0488d75ac4c89fa4489e144896dace83ded0000eb3a",
     "tag 1: uncompress(dst=[rbp-0x60], &destLen, src=r15, srcLen=r12d) ; jmp exit"),
    (0x1AA3F5, "41b800000000", "tag 5: r8d = 0 (fuzzSafe off)"),
    (0x1AA3FB, "488b55a041b9000000004c89ff4c89e64889d96a03ff3532c3d302ff3504c3d3026a006a006a006a006a00",
     "Oodle arg setup: rdx=dst r9d=0 rdi=compBuf rsi=compSize rcx=rawLen + 8 stack pushes"),
    (0x1AA42F, "488d3df2c2d302", "exit: lea lock object"),
    (0x1AA442, "48011d07c3d302", "add [stats], rbx"),
    (0x1AA452, "4889d8", "return rbx (decoded size)"),
    # size helpers (0x1aa260 reads the same header word)
    (0x1AA210, "813fffffff1f0f97c0c3", "IsCompressed: [rdi] > 0x1fffffff (tag != 0)"),
    (0x1AA260, "8b3f31c089f9c1e91d80f9057719488d05bf3ae00048630c88", "DecodedSize: tag extraction + table"),
    (0x1AA27E, "81e7ffffff1f4889f8c3", "DecodedSize tags 0..4: size & 0x1fffffff"),
    (0x1AA288, "81e7ffffff1f31f6e9", "DecodedSize tag 5: tail-jump (size, 0)"),
    # header-word helpers (Set has zero references: dead code in this image)
    (0x1AA0A0, "81e2ffffff1fc1e61d09d68937c3", "Set(hdr, tag, size): [hdr] = tag<<29 | size&0x1fffffff"),
    (0x1AA0B0, "c70700000000c3", "Clear(hdr): [hdr] = 0"),
    (0x1AA0D0, "8b07c1e81dc3", "GetTag: [rdi] >> 29"),
    (0x1AA0E0, "8b0725ffffff1fc3", "GetSize: [rdi] & 0x1fffffff"),
    (0x1AA250, "8b07c1e81dc3", "GetTag (second copy)"),
    (0x1AA240, "8b0725ffffff1fc3", "GetSize (second copy)"),
    # the sole caller FUN_00147160
    (0x1471F0, "4d8b670841f6442470084c89", "r12 = request->descriptor ; test [r12+0x70], 8"),
    (0x147207, "498dbff00000004889bd20feffff", "rdi = &request[0xf0] (header word)"),
    (0x14722A, "418b542454be01000000", "alloc(size, 1, [descriptor+0x54])"),
    (0x147241, "488bbd20feffff4c89f64c89ea4889c14d89e04989c6",
     "Decompress(hdr, compBuf=r14, compSize=r13, dst=rax, rawLen=r12)"),
    (0x1471E2, "4d8baf80000000", "r13 = request[0x80] (compSize)"),
    (0x14C000, "31c04883bf80000000007407488b8790000000c3", "FUN_0014c000: [+0x80] ? [+0x90] : 0 (compBuf)"),
    # request loader: the 12-byte trailer
    (0x14AF1A, "488973088b4670a8010f8582080000a808740b48c783e00000000c000000",
     "0x14aee0: request[8] = desc; if desc[0x70] & 8: request[0xe0] = 12 (trailer length)"),
    (0x14BC35, "4d8ba7e00000004d85e4746f4d8b6f78488b7c24184c89e24c89ee4c29e64903b790000000",
     "0x14b980: r12 = trailerLen; memcpy(request+0xe8, buf + size - trailerLen, trailerLen)"),
    (0x14BC63, "498bb7e8000000b201488b78284889f048c1e8204889f948c1e92039fe74174183bf9c00000000750b41c7879c00000003000000",
     "compare request[0xe8] (8 bytes) with desc[0x28]; mismatch -> error 3 / 4"),
    (0x14BCB0, "4d8b6f784d29e54d89af80000000", "request[0x80] = request[0x78] - trailerLen"),
    (0x14BBBA, "ff9080000000", "file object vtable +0x80 : read(buffer=[+0x90], size=[+0x78])"),
    # the csdr descriptor (FUN_00b0d3e0)
    (0xB0D420, "48b85244534313000000", "mov rax, 0x1343534452 (RDSC + 0x13): desc[0x28] tag"),
    (0xB0D438, "be72647363", "mov esi, 'csdr' (0x63736472)"),
    (0xB0D447, "804da008", "or byte [rbp-0x60], 8 : desc[0x70] |= 8 (stored compressed)"),
    (0xB0D440, "48898558ffffff", "mov [rbp-0xa8], rax : desc[0x28] = tag"),
    (0x1446DD, "4183e6f7", "ctor: flags &= ~8 before the caller ORs it back"),
    (0x144739, "44897370", "ctor: desc[0x70] = flags"),
    (0x1446C5, "48c7432800000000", "ctor: desc[0x28] = 0 (tag cleared; csdr sets it afterwards)"),
]

CALLS = [
    # (site, target, what)
    (0x147215, 0x1AA210, "IsCompressed"),
    (0x147225, 0x1AA260, "DecodedSize"),
    (0x14723C, 0x12820, "alloc"),
    (0x147257, DECOMPRESS, "Decompress"),
    (0x1472DC, TYPE_CREATE, "type create"),
    (0x1AA32E, LOCK, "lock"),
    (0x1AA3D5, MEMCPY, "memcpy (tag 0)"),
    (0x1AA3EE, ZLIB_UNCOMPRESS, "zlib uncompress (tag 1)"),
    (OODLE_CALL_SITE, OODLE, "OodleLZ_Decompress"),
    (0x1AA436, UNLOCK, "unlock"),
    (0x1AA464, STACK_CHK_FAIL, "__stack_chk_fail"),
    (0x14BC5A, MEMCPY, "trailer memcpy"),
    (0xB0D41B, DESC_CTOR, "descriptor ctor"),
    (0xB0D464, DESC_REGISTER, "descriptor register"),
]


# ---------------------------------------------------------------------------------------------------
def fde_table(e: Elf) -> list[tuple[int, int]]:
    header = e.raw(EH_FRAME_HDR, 12)
    require(header[0] == 1 and header[1] == 0x1B and header[2] == 0x03 and header[3] == 0x3B,
            "eh_frame_hdr version/encodings")
    count = struct.unpack("<I", header[8:12])[0]
    require(count == 55790, f"eh_frame_hdr entry count {count}")
    blob = e.img.read(EH_FRAME_HDR + 12, count * 8)
    for i in range(count * 8):
        a = EH_FRAME_HDR + 12 + i
        if a in e.overlay:
            blob = blob[:i] + bytes([e.overlay[a]]) + blob[i + 1:]
    rows = [(init + EH_FRAME_HDR, fde + EH_FRAME_HDR) for init, fde in struct.iter_unpack("<ii", blob)]
    require(all(rows[i][0] < rows[i + 1][0] for i in range(len(rows) - 1)), "FDE table not strictly sorted")
    return rows


def fde_pinned(e: Elf, rows: list[tuple[int, int]], initial: int) -> int:
    """Pins the table entry and FDE record of `initial` and returns its address_range."""
    idx = bisect.bisect_left([r[0] for r in rows], initial)
    require(idx < len(rows) and rows[idx][0] == initial, f"no FDE table entry for 0x{initial:x}")
    e.read(EH_FRAME_HDR + 12 + 8 * idx, 8)
    fde = rows[idx][1]
    record = e.read(fde, 16)
    require(struct.unpack("<I", record[0:4])[0] == 0x20, f"FDE record length at 0x{fde:x}")
    require(struct.unpack("<I", record[4:8])[0] != 0, f"FDE CIE pointer at 0x{fde:x}")
    location = fde + 8 + struct.unpack("<i", record[8:12])[0]
    require(location == initial, f"FDE initial_location 0x{location:x} != 0x{initial:x}")
    return struct.unpack("<I", record[12:16])[0]


def containing(e: Elf, rows: list[tuple[int, int]], va: int) -> tuple[int, int]:
    idx = bisect.bisect_right([r[0] for r in rows], va) - 1
    require(idx >= 0, f"no FDE at or below 0x{va:x}")
    init, fde = rows[idx]
    rng = struct.unpack("<I", e.raw(fde + 12, 4))[0]
    require(init <= va < init + rng, f"0x{va:x} is in no FDE (nearest 0x{init:x}+0x{rng:x})")
    return init, rng


# ---------------------------------------------------------------------------------------------------
def segments(e: Elf) -> list[tuple[int, bytes]]:
    out = []
    for p in e.img.loads:
        blob = bytearray(e.img.read(p["vaddr"], p["filesz"]))
        for addr, val in e.overlay.items():
            if p["vaddr"] <= addr < p["vaddr"] + p["filesz"]:
                blob[addr - p["vaddr"]] = val
        out.append((p["vaddr"], bytes(blob)))
    return out


def census(segs: list[tuple[int, bytes]], relocs, targets: list[int]) -> dict[int, dict[str, list]]:
    """Whole-image reference census. Every class is reported; nothing is filtered by a disassembler."""
    res = {t: {"e8": [], "e9": [], "jcc": [], "lea": [], "loose": [], "q64": [], "d32": [], "reloc": []}
           for t in targets}
    want = set(targets)
    rip_keys: dict[int, list[tuple[int, int]]] = {}
    for t in targets:
        for k in (0, 1, 2, 4):
            rip_keys.setdefault(t - 4 - k, []).append((t, k))
    for base, blob in segs:
        n = len(blob)
        for m in re.finditer(rb"[\xe8\xe9]", blob):
            i = m.start()
            if i + 5 > n:
                continue
            tgt = base + i + 5 + struct.unpack_from("<i", blob, i + 1)[0]
            if tgt in want:
                res[tgt]["e8" if blob[i] == 0xE8 else "e9"].append(base + i)
        for m in re.finditer(rb"\x0f[\x80-\x8f]", blob):
            i = m.start()
            if i + 6 > n:
                continue
            tgt = base + i + 6 + struct.unpack_from("<i", blob, i + 2)[0]
            if tgt in want:
                res[tgt]["jcc"].append(base + i)
        for a in range(4):
            sub = blob[a:n - ((n - a) % 4)]
            arr = array("i")
            arr.frombytes(sub)
            pos = base + a
            for v in arr:
                key = v + pos
                if key in rip_keys:
                    for t, k in rip_keys[key]:
                        i = pos - base
                        is_lea = k == 0 and i >= 2 and blob[i - 1] & 0xC7 == 0x05 and blob[i - 2] == 0x8D
                        res[t]["lea" if is_lea else "loose"].append((pos, k))
                if v in want:
                    res[v]["d32"].append(pos)
                pos += 4
        for t in targets:
            for m in re.finditer(re.escape(struct.pack("<Q", t)), blob):
                res[t]["q64"].append(base + m.start())
    for r in relocs.values():
        if r["addend"] in want and r["type"] == 8:
            res[r["addend"]]["reloc"].append(r["target"])
    for t in targets:
        for key in res[t]:
            res[t][key] = sorted(res[t][key])
    return res


def own_displacements(hit_sites: list[int]) -> set[int]:
    return {s + 1 for s in hit_sites}


# ---------------------------------------------------------------------------------------------------
def check_elf(e: Elf, rows: list[tuple[int, int]]) -> dict:
    facts: dict = {}
    # 1. extents from the FDE table
    rng = fde_pinned(e, rows, DECOMPRESS)
    require(rng == DECOMPRESS_SIZE, f"FDE range of 0x1aa2a0 is 0x{rng:x}")
    crng = fde_pinned(e, rows, CALLER)
    require(crng == CALLER_RANGE, f"FDE range of 0x147160 is 0x{crng:x}")
    require(containing(e, rows, CALL_SITE) == (CALLER, CALLER_RANGE), "call site is not inside FDE 0x147160")
    require(containing(e, rows, OODLE_CALL_SITE) == (DECOMPRESS, DECOMPRESS_SIZE),
            "Oodle call site is not inside FDE 0x1aa2a0")
    require(containing(e, rows, TYPE_CREATE_SITE) == (CALLER, CALLER_RANGE), "type-create site not inside 0x147160")

    # 2. literal bytes
    for site, hexstr, what in PINS:
        e.bytes_at(site, hexstr, what)

    # 3. direct call targets
    for site, target, what in CALLS:
        require(e.call(site) == target, f"{what}: call at 0x{site:x} -> 0x{e.call(site):x}, want 0x{target:x}")

    # 4. relative switch tables, decoded from the bytes
    require(e.lea(0x1AA341) == SWITCH_TABLE, "dispatch lea does not point at the switch table")
    raw = e.read(SWITCH_TABLE, 24)
    targets = [SWITCH_TABLE + struct.unpack_from("<i", raw, 4 * i)[0] for i in range(6)]
    require(targets == SWITCH_TARGETS, f"switch table targets {[hex(t) for t in targets]}")
    # the size helper's own table (lea at 0x1aa26e, 7 bytes, rip = 0x1aa275)
    require(e.rip(0x1AA26E, 7, 3) == SIZE_TABLE, "size helper table address")
    raw = e.read(SIZE_TABLE, 24)
    stargets = [SIZE_TABLE + struct.unpack_from("<i", raw, 4 * i)[0] for i in range(6)]
    require(stargets == SIZE_TARGETS, f"size table targets {[hex(t) for t in stargets]}")

    # 5. tag -> codec, derived by following the bytes of each case
    # jnbe at 0x1aa33b (0f 87 rel32) -> 0x1aa42f
    require(0x1AA341 + struct.unpack("<i", e.read(0x1AA33D, 4))[0] == 0x1AA42F, "bounds-check exit target")
    tag_map = {}
    for tag, tgt in enumerate(targets):
        if tgt == 0x1AA35C:
            # memcpy case: the body (log gate) reaches the memcpy call at 0x1aa3d5
            tag_map[tag] = "memcpy"
        elif tgt == 0x1AA3DC:
            tag_map[tag] = "zlib_uncompress"
        elif tgt == 0x1AA351:
            require(e.read(tgt, 6) == bytes.fromhex("41b801000000"), "fuzzSafe=1 setter")
            require(e.jmp(tgt + 6) == 0x1AA3FB, "fuzzSafe=1 case jumps to the Oodle block")
            tag_map[tag] = "oodle_fuzzsafe1"
        elif tgt == 0x1AA3F5:
            require(e.read(tgt, 6) == bytes.fromhex("41b800000000"), "fuzzSafe=0 setter")
            tag_map[tag] = "oodle_fuzzsafe0"        # falls through into 0x1aa3fb
    for tag in (6, 7):
        tag_map[tag] = "no_decode"
    require(tag_map == {0: "memcpy", 1: "zlib_uncompress", 2: "oodle_fuzzsafe1", 3: "oodle_fuzzsafe1",
                        4: "oodle_fuzzsafe1", 5: "oodle_fuzzsafe0", 6: "no_decode", 7: "no_decode"},
            f"tag map {tag_map}")
    # the two stack arguments of the Oodle call are the shared decoder-memory cells
    require(e.rip(0x1AA410, 6, 2) == OODLE_STACK_ARG_DECODER_MEM_SIZE, "stack arg 13 cell")
    require(e.rip(0x1AA416, 6, 2) == OODLE_STACK_ARG_DECODER_MEM, "stack arg 12 cell")
    require(e.lea(0x1AA42F) == LOCK_OBJECT and e.lea(0x1AA321) == LOCK_OBJECT, "lock object")
    require(e.rip(0x1AA442, 7, 3) == STATS_COUNTER, "bytes-decoded counter")
    # jmp rel32 at 0x1aa290 -> decode-buffer-size helper
    require(e.jmp(0x1AA290) == DECODE_BUFFER_SIZE, "size helper tail jump target")
    # tag-1 argument block: the dest length lives at [rbp-0x54] (set from r13d, the caller's rawLen)
    e.bytes_at(0x1AA3E0, "488d75ac4c89fa4489e144896dac", "zlib arg block")

    # 6. csdr descriptor wiring
    loader_cb = e.rip(0xB0D431, 7, 3)
    require(loader_cb == CSDR_LOADER_CB, f"csdr loader callback 0x{loader_cb:x}")
    require(e.call(0xB0D41B) == DESC_CTOR, "descriptor ctor call")
    # type name string "Shader" (0xb0d408 lea rsi) and platform suffix cell "_ps4" (0xb0d42a lea rcx)
    e.cstring(e.lea(0xB0D408), b"Shader")
    e.cstring(e.lea(0xB0D42A), b"_ps4")

    facts["tag_map"] = tag_map
    facts["switch_targets"] = targets
    return facts


def check_body_hash(e: Elf) -> str:
    body = e.raw(DECOMPRESS, DECOMPRESS_SIZE)
    if not e.skip_hash:
        require(digest(body) == BODY_SHA, f"0x1aa2a0 body sha256 {digest(body)}")
    return digest(body)


BODY_SHA = "fe721ccebe16e9c3923ecad8bb1c6e8fe7e2780b5a7229de2b175097f0583734"


def check_census(e: Elf, results: dict[int, dict[str, list]], strict_primary: bool = True) -> None:
    for target, (name, e8, e9, jcc, lea, q64, d32, reloc) in CENSUS.items():
        r = results[target]
        got = (r["e8"], r["e9"], r["jcc"], [p for p, _ in r["lea"]], r["q64"], r["d32"], r["reloc"])
        want = (e8, e9, jcc, lea, q64, d32, reloc)
        require(got == want, f"census of 0x{target:x} ({name}): got {got}, want {want}")
        # every loose rip-displacement coincidence is either the call's own displacement or pinned
        own = own_displacements(r["e8"] + r["e9"])
        loose = {p: k for p, k in r["loose"] if p not in own}
        if target == DECOMPRESS and strict_primary:
            require(loose == LOOSE_PRIMARY, f"loose rip coincidences of 0x1aa2a0: {loose}")
    # containing function of the single caller: the FDE table, not a procedure list
    # (done in check_elf; asserted again here for the census hit itself)


def run(e: Elf, rows) -> None:
    check_elf(e, rows)
    check_body_hash(e)


# ---------------------------------------------------------------------------------------------------
def expect_failure(label: str, fn) -> None:
    try:
        fn()
    except (SystemExit, ValueError, struct.error, IndexError, KeyError) as error:
        if isinstance(error, SystemExit):
            require(str(error).startswith("FAIL"), f"mutant {label}: unexpected exit {error!r}")
        return
    raise SystemExit(f"FAIL mutant survived: {label}")


def mutation_self_test(e: Elf) -> int:
    e.recording = True
    run(e, fde_table(e))
    e.recording = False
    pins = sorted(e.pins)
    e.skip_hash = True
    ran = survivors = 0
    for addr, size in pins:
        for byte_addr in sorted({addr, addr + size - 1}):
            e.overlay = {}
            pristine = e.raw(byte_addr, 1)[0]
            e.overlay = {byte_addr: pristine ^ 0x01}
            ran += 1
            try:
                run(e, fde_table(e))
            except (SystemExit, ValueError, struct.error, IndexError, KeyError):
                continue
            survivors += 1
            print(f"mutation survived at 0x{byte_addr:x} (pin 0x{addr:x}+{size})", file=sys.stderr)
    e.overlay = {}
    e.skip_hash = False
    require(survivors == 0, f"{survivors} ELF mutations survived")
    return ran


def detector_probes() -> int:
    """Synthetic image: each census detector class must fire on exactly the injected reference."""
    T = 0x1000
    blob = bytearray(b"\x90" * 0x400)
    base = 0x800
    def put(off: int, data: bytes) -> None:
        blob[off:off + len(data)] = data
    put(0x10, b"\xe8" + struct.pack("<i", T - (base + 0x10 + 5)))                       # call
    put(0x30, b"\xe9" + struct.pack("<i", T - (base + 0x30 + 5)))                       # jmp
    put(0x50, b"\x0f\x84" + struct.pack("<i", T - (base + 0x50 + 6)))                   # jz
    put(0x70, b"\x48\x8d\x05" + struct.pack("<i", T - (base + 0x70 + 7)))               # lea rax,[rip+d]
    put(0x90, b"\x4c\x8d\x0d" + struct.pack("<i", T - (base + 0x90 + 7)))               # lea r9,[rip+d]
    put(0xB0, struct.pack("<Q", T))                                                     # 64-bit literal
    put(0xD0, b"\x83\x3d" + struct.pack("<i", T - (base + 0xD0 + 7)) + b"\x01")         # cmp [rip+d],1 (k=1)
    r = census([(base, bytes(blob))], {}, [T])[T]
    require(r["e8"] == [base + 0x10] and r["e9"] == [base + 0x30] and r["jcc"] == [base + 0x50],
            "probe: rel32 call/jmp/jcc detectors")
    require([p for p, _ in r["lea"]] == [base + 0x73, base + 0x93], f"probe: lea detector {r['lea']}")
    require(r["q64"] == [base + 0xB0], "probe: 64-bit detector")
    # loose = the rel32 displacements of the call/jmp/jcc themselves (k=0) + the `cmp [rip+d], imm8` (k=1)
    require([p for p, _ in r["loose"]] == [base + 0x11, base + 0x31, base + 0x52, base + 0xD2],
            f"probe: loose classification {r['loose']}")
    r = census([(base, b"\x90" * 0x400)], {}, [T])[T]
    require(all(not v for v in r.values()), "probe: clean image must report nothing")
    return 7


def injection_probe(e: Elf, relocs) -> int:
    """Whole-image re-run with references injected into padding: the census must report exactly them."""
    base_a = 0xBA0                                    # a 16-byte 0x90 run in the first code segment
    require(e.raw(base_a, 16) == b"\x90" * 16, "injection site is nop padding")
    overlay = {}
    for i, b in enumerate(b"\xe8" + struct.pack("<i", DECOMPRESS - (base_a + 5))):
        overlay[base_a + i] = b
    base_b = base_a + 8
    for i, b in enumerate(b"\x48\x8d\x05" + struct.pack("<i", DECOMPRESS - (base_b + 7))):
        overlay[base_b + i] = b
    e.overlay = overlay
    try:
        segs = segments(e)
        res = census(segs, relocs, [DECOMPRESS, OODLE, TYPE_CREATE, CALLER, ZLIB_UNCOMPRESS])
    finally:
        e.overlay = {}
    got = res[DECOMPRESS]
    require(got["e8"] == [base_a, CALL_SITE] and [p for p, _ in got["lea"]] == [base_b + 3],
            f"injection probe: census did not report exactly the injected refs: {got['e8']} {got['lea']}")
    try:
        check_census(e, res)
    except SystemExit:
        return 2
    raise SystemExit("FAIL injection probe: injected references were accepted by the census check")


# ---------------------------------------------------------------------------------------------------
EXPECT_ARCHIVE = {
    "members": 34392, "tag_ok": 34392,
    "types": {0: 15072, 3: 16328, 4: 2992},
    "type0_size_equals_payload": 15072,
    "typed_size_gt_payload": 19320,
    "prefix": {"0100": 8178, "0200": 6894, "8c06": 2992, "8c0a": 16328},
}


def archive_census() -> dict:
    sys.path.insert(0, str(ROOT / "scripts"))
    from psarc import Psarc  # noqa: E402
    base = ROOT / "local/extracted/CUSA08809"
    got = {"members": 0, "tag_ok": 0, "types": {}, "type0_size_equals_payload": 0,
           "typed_size_gt_payload": 0, "prefix": {}}
    for name in ("pak0.psarc", "pak1.psarc", "pakN.psarc", "pak2.psarc", "pak3.psarc", "cachedFiles.psarc"):
        path = base / name
        if not path.is_file():
            continue
        ps = Psarc(path)
        for member, idx in ps.members():
            if not member.endswith(".csdr"):
                continue
            data = ps.read(idx)
            got["members"] += 1
            if len(data) < 16:
                continue
            tag = int.from_bytes(data[-12:-4], "little")
            word = int.from_bytes(data[-4:], "little")
            typ, size = word >> 29, word & 0x1FFFFFFF
            payload = len(data) - 12
            got["tag_ok"] += tag == 0x1343534452
            got["types"][typ] = got["types"].get(typ, 0) + 1
            pre = data[:2].hex()
            got["prefix"][pre] = got["prefix"].get(pre, 0) + 1
            if typ == 0:
                got["type0_size_equals_payload"] += size == payload
            else:
                got["typed_size_gt_payload"] += size > payload
    return got


# ---------------------------------------------------------------------------------------------------
def main() -> int:
    arguments = sys.argv[1:]
    if not (ROOT / "local/re/eboot.elf").is_file():
        print("SKIP check_csdr_oodle_dispatch: local/re/eboot.elf not present")
        return 0
    e = Elf()
    rows = fde_table(e)
    if "--tamper" in arguments:
        e.overlay = {CALL_SITE + 1: e.raw(CALL_SITE + 1, 1)[0] ^ 0x40}
    if "--print-census" in arguments:
        res = census(segments(e), e.relocs, [DECOMPRESS, OODLE, TYPE_CREATE, CALLER, ZLIB_UNCOMPRESS])
        for t, r in res.items():
            print(hex(t), {k: ([hex(x) if isinstance(x, int) else (hex(x[0]), x[1]) for x in v]) for k, v in r.items()})
        print("body sha", digest(e.raw(DECOMPRESS, DECOMPRESS_SIZE)))
        return 0
    e.recording = True
    run(e, rows)
    e.recording = False
    results = census(segments(e), e.relocs, list(CENSUS))
    check_census(e, results)
    # containing function (FDE, not procedure list) of every single hit
    for t in CENSUS:
        for site in results[t]["e8"] + results[t]["e9"] + [p for p, _ in results[t]["lea"]]:
            containing(e, rows, site)
    note = ""
    if "--no-mutate" not in arguments and "--tamper" not in arguments:
        ran = mutation_self_test(e)
        probes = detector_probes()
        inj = injection_probe(e, e.relocs)
        note = (f"; mutation self-test {ran} ELF mutants + {probes} detector probes + {inj} whole-image "
                f"injection probes all rejected/detected")
    if "--archives" in arguments:
        got = archive_census()
        require(got == EXPECT_ARCHIVE, f"archive trailer census {got} != {EXPECT_ARCHIVE}")
        note += "; archive trailer census 34392 members tag/size/type counts match (counts only)"
    print(f"OK check_csdr_oodle_dispatch: FUN_001aa2a0 (FDE 0x1aa2a0+0x1ca) has exactly 1 reference "
          f"(call 0x147257 in FDE 0x147160+0x32e); switch table 0xfadd4c = {[hex(t) for t in SWITCH_TARGETS]} "
          f"(tag 0 memcpy, 1 zlib, 2/3/4 Oodle fuzzSafe=1, 5 Oodle fuzzSafe=0, 6/7 none); Oodle 0x89d3b0 has "
          f"1 call (0x1aa426); csdr descriptor sets flag 8 + tag RDSC; guest bytes NOT executed{note}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
