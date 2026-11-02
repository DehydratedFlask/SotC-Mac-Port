#!/usr/bin/env python3
"""Fail-closed checker for the on-disk `.csdr` member layout behind the `FUN_00b0cc50` loader callback.

What this pins (task `GPU-01-CSDR-B0CC50-EXPANSION`, checkpoint 2026-11-01-csdr-b0cc50-expansion.md):

* `FUN_00b0cc50` (624 B) is the `csdr` resource-type callback stored by the descriptor builder
  `FUN_00b0d3e0`. It is a candidate-path state machine; it never reads member bytes. The member bytes
  are consumed by the generic resource path and, for the compressed form, by the statically linked
  Oodle 2 LZ block decoder. So the *container layout* is established from three independent sources:
  the Oodle block/chunk/quantum parsers in the ELF, the RDSC trailer the descriptor registers, and
  the real members.
* A compressed member (19,320 of 34,392 corpus members) is, byte for byte:

      off  size  field
        0     1  0x8c   Oodle block header byte 0: low nibble 0xC, bits4-5 = 0 (header type 4),
                        bit6 = 0 (not memcpy), bit7 = 1 (decoder reset)
        1     1  0x06 (Kraken) | 0x0a (Mermaid): low 7 bits = decode type, bit7 = 0 (no checksum)
        2     3  chunk header, big endian: bits 18..23 = 0 (flags/special 0), bits 0..17 = compLen - 1
        5     3  quantum header, big endian: bit23 = 1 (compressed), bits19..22 = 1 (mode),
                        bits0..18 = compLen - 3
        8     8  the first 8 output bytes, stored raw (u32 LE in {1, 2}, same domain as raw members)
       16  ...   LZ entropy/command streams (NOT decoded here)
     5+compLen  12  RDSC trailer: 52 44 53 43 13 00 00 00 then u32 LE = tag(3 bits) << 29 | rawLen
      => size == compLen + 17 ;  rawLen > compLen ;  rawLen <= 0x20000 (single quantum)

  A raw member (15,072) is `payload ++ RDSC trailer` with tag 0 and low29 == size - 12.

Three layers, all fail-closed:

1. PUBLIC (always): a synthetic fixture builder + `validate_member` strict profile, with 70+ mutation
   probes; each field mutation must be rejected. Uses no game bytes.
2. PRIVATE ELF (`local/re/eboot.elf`, sha256 pinned): byte/rip/call pins for the descriptor builder,
   the Oodle block-header parser, the chunk-header (QH) parser, the decode-type selector, the newLZ
   quantum-header read, the compressor tables, whole-body hashes, and a mutation self-test that flips
   the first and last byte of every pin and requires every mutant to be rejected.
3. PRIVATE CORPUS (`local/extracted/CUSA08809/*.psarc`): every `.csdr` member is read in memory (never
   written to disk) and must satisfy the profile; counts are pinned.

`validate_member` encodes the CORPUS profile, which is deliberately narrower than what the ELF parser
accepts (the parser also handles V3 headers, memcpy blocks, checksums and multi-chunk streams); the
profile rejects those because no shipped member uses them. Tightening it never hides a regression.

Exit codes: 0 = OK (or SKIP for absent private inputs), 1 = FAIL. `--require-private` turns SKIP into
FAIL. `--no-corpus` skips layer 3. Run Python with `-I`.
"""

from __future__ import annotations

import hashlib
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools/lift"))
sys.path.insert(0, str(ROOT / "scripts"))

ELF_PATH = ROOT / "local/re/eboot.elf"
CORPUS_DIR = ROOT / "local/extracted/CUSA08809"
CORPUS_ARCHIVES = ("pak0.psarc", "pak1.psarc", "pakN.psarc", "cachedFiles.psarc")

RDSC = bytes.fromhex("5244534313000000")   # movabs rax,0x1343534452 at 0xb0d420, little endian
TRAILER = 12
MAX_QUANTUM_RAW = 0x20000
PROFILE_B0 = 0x8C
PROFILE_B1 = {0x06: ("Kraken", 8, 4), 0x0A: ("Mermaid", 9, 3)}   # byte1 -> (name, compressor id, tag)

# ---- corpus expectations (observed on the pinned corpus, re-derived by --corpus) -----------------
CORPUS_TOTAL = 34392
CORPUS_COMPRESSED = 19320
CORPUS_RAW = 15072
CORPUS_MERMAID = 16328
CORPUS_KRAKEN = 2992
CORPUS_MAX_RAWLEN = 33468
CORPUS_MAX_COMPLEN = 20380

# ---- ELF pins -------------------------------------------------------------------------------------
ELF_BODIES = (
    # name, rva, size, sha256, first4, last4
    ("csdr callback 0xb0cc50", 0xB0CC50, 624,
     "8f7682a8da64388edcf8223b0a25ccc083f2f1bb739ed104dea32300edd1acbc", "554889e5", "e0244800"),
    ("path builder 0xb0cb80", 0xB0CB80, 201,
     "4e0e30467b865d0ffee6d46ac2a389b1b559c422f0bc50fc4a29cd3167c40283", "554889e5", "57274800"),
    ("descriptor builder 0xb0d3e0", 0xB0D3E0, 177,
     "5d619090676f34ca8bcdbb4e20855f699c8475775770f9c2c06d3bf5dacb63b9", "554889e5", "0f1f4800"),
    ("Oodle block header parser 0x8ac710", 0x8AC710, 778,
     "ef24535a354c52e595b024dce5e7580c87462ff7f64151a995a3959626398de3", "554889e5", "acfeffff"),
    ("Oodle chunk header (QH) parser 0x8ace10", 0x8ACE10, 426,
     "ec2b489ee13bfc467d16a876b442e9cc537ea7f3558edd65df4bc08f3d46218c", "554889e5", "415f5dc3"),
    ("newLZ decoder 0x8c1e60", 0x8C1E60, 12474,
     "4f81120272295168904a996a1b3845b7bb8940fe401f1a2f9f98be485307892e", "554889e5", "5a9d004c"),
)

# (address, hex bytes, what)
BYTE_PINS = (
    # descriptor builder FUN_00b0d3e0: resource type 'csdr', RDSC trailer tag, callback pointer
    (0xB0D420, "48b85244534313000000", "mov rax,0x1343534452 (RDSC + 0x13 trailer tag)"),
    (0xB0D438, "be72647363", "mov esi,0x63736472 ('csdr' resource type)"),
    # Oodle block header parser FUN_008ac710
    (0x8AC719, "0fb60b", "movzx ecx,byte [rbx]  (block byte 0)"),
    (0x8AC71E, "83e00f", "and eax,0xf"),
    (0x8AC721, "83f80c", "cmp eax,0xc  (low nibble must be 0xC for the V4 header)"),
    (0x8AC726, "b804020000", "mov eax,0x204  (bextr start 4, len 2)"),
    (0x8AC72B, "c4e278f7d1", "bextr edx,ecx,eax  (bits 4-5 of byte 0)"),
    (0x8AC732, "83ca04", "or edx,4  (header type, 4 when bits 4-5 are 0)"),
    (0x8AC737, "83fa04", "cmp edx,4"),
    (0x8AC740, "b806010000", "mov eax,0x106  (bextr start 6, len 1)"),
    (0x8AC745, "c4e278f7c1", "bextr eax,ecx,eax  (bit 6 -> hdr[3], 'memcpy')"),
    (0x8AC74A, "c1e907", "shr ecx,7  (bit 7 -> hdr[4], 'reset')"),
    (0x8AC753, "0fb64301", "movzx eax,byte [rbx+1]  (block byte 1)"),
    (0x8AC757, "4883c302", "add rbx,2  (block header is 2 bytes)"),
    (0x8AC75D, "c1e907", "shr ecx,7  (byte1 bit 7 -> hdr[5], 'checksum')"),
    (0x8AC760, "83e07f", "and eax,0x7f  (byte1 low 7 bits = decode type)"),
    (0x8AC763, "894f14", "mov [rdi+0x14],ecx"),
    # decode-type selector and header-parser call in FUN_0089bcb0 / its tail
    (0x89BDED, "e81e090100", "call 0x8ac710"),
    (0x89C252, "b251", "mov dl,0x51  (bit set: types 6 and 10 -> QH parser)"),
    (0x89C265, "83f907", "cmp ecx,7"),
    (0x89C26B, "d2ea", "shr dl,cl"),
    (0x89C2A0, "e86b0b0100", "call 0x8ace10  (3-byte chunk header)"),
    (0x89C2A7, "e8e4080100", "call 0x8acb90  (other chunk header form)"),
    # chunk header (QH) parser FUN_008ace10
    (0x8ACE57, "410fb60c24", "movzx ecx,byte [r12]"),
    (0x8ACE5C, "410fb6742401", "movzx esi,byte [r12+1]"),
    (0x8ACE62, "410fb67c2402", "movzx edi,byte [r12+2]"),
    (0x8ACE6A, "c1e608", "shl esi,8"),
    (0x8ACE6D, "83e203", "and edx,3  (only bits 0-1 of byte 0 are length)"),
    (0x8ACE70, "c1e210", "shl edx,0x10"),
    (0x8ACE77, "81faffff0300", "cmp edx,0x3ffff  (special-value escape)"),
    (0x8ACE7F, "c0e902", "shr cl,2  (special selector = b0>>2)"),
    (0x8ACEEB, "ffc2", "inc edx  (compLen = field + 1)"),
    (0x8ACF08, "4585ed", "test r13d,r13d  (optional 3-byte CRC only when checksums on)"),
    # newLZ quantum header read in FUN_008c1e60
    (0x8C1EF0, "418b0424", "mov eax,[r12]"),
    (0x8C1EF6, "0fca", "bswap edx  (big-endian read)"),
    (0x8C1EF8, "85d2", "test edx,edx"),
    (0x8C1EFA, "0f8880000000", "js  (bit 31 = bit 23 of the 24-bit header = 'compressed')"),
    (0x8C1F83, "41c1eb08", "shr r11d,8"),
    (0x8C1F87, "4181e3ffff0700", "and r11d,0x7ffff  (19-bit size)"),
    (0x8C1FA0, "b81b040000", "mov eax,0x41b  (bextr start 27, len 4)"),
    (0x8C1FAA, "c46278f7f2", "bextr r14d,edx,eax  (header bits 19..22 = mode)"),
    (0x8C2055, "83bd14ffffff02", "cmp dword [rbp-0xec],2  (mode must be <= 1)"),
    (0x8C205C, "0f83242e0000", "jnc  -> error return"),
    (0x8C20AC, "4883bdf8feffff00", "cmp qword [rbp-0x108],0  (output offset 0 only)"),
    (0x8C20B6, "498b4c2403", "mov rcx,[r12+3]  (8 bytes right after the 3-byte quantum header)"),
    (0x8C20BB, "4983c40b", "add r12,0xb  (source advances 3 + 8)"),
    (0x8C20C2, "48890a", "mov [rdx],rcx  (stored raw as output bytes 0..7)"),
)

CALL_PINS = (
    (0x89BDED, 0x8AC710, "block header parse"),
    (0x89C2A0, 0x8ACE10, "chunk header (QH) parse"),
    (0x89C2A7, 0x8ACB90, "alternate chunk header parse"),
    (0x8ACE3E, 0x8C1780, "QH output zero-fill"),
    (0xB0D464, 0x1444E0, "register resource-type descriptor"),
)

LEA_PINS = (
    (0xB0D408, 0x12CFC22, "string 'Shader'"),
    (0xB0D42A, 0x12CFC29, "string '_ps4'"),
    (0xB0D431, 0xB0CC50, "callback pointer stored into the descriptor"),
    (0xB0D44F, 0x2ED1FB0, "resource-type table cell"),
)

STRING_PINS = (
    (0x12CFC22, b"Shader"),
    (0x12CFC1C, b".csdr"),
    (0x12CFC29, b"_ps4"),
    (0x1295036, b"Kraken"),
    (0x129503D, b"Mermaid"),
    (0x129A9B8, b"v:\\devel\\projects\\oodle2\\core\\newlz.cpp"),
    (0x129AC12, b"newLZ_decode_chunk_phase2"),
    (0x129AC2C, b"bad chunk_type"),
)

# decode-type -> compressor-id table at 0x1295090 (u32 LE x 13); 0xffffffff = invalid type
DECODE_TO_COMPRESSOR = (1, 2, 4, 5, 6, 7, 8, 0, 0xFFFFFFFF, 0xFFFFFFFF, 9, 10, 13)
DECODE_TABLE = 0x1295090
NAME_TABLE = 0x15CED00     # 8-byte string pointers indexed by compressor id


class Reject(ValueError):
    """The member is not in the corpus profile."""


def fail(message: str) -> None:
    print(f"FAIL check_csdr_b0cc50_container: {message}", file=sys.stderr)
    raise SystemExit(1)


# ---------------------------------------------------------------------------------------------------
# Layer 1: the container profile
# ---------------------------------------------------------------------------------------------------

def _need(cond: bool, message: str) -> None:
    if not cond:
        raise Reject(message)


def validate_member(data: bytes) -> dict:
    """Strict corpus profile. Returns the decoded fields, raises Reject otherwise."""
    _need(len(data) >= TRAILER + 1, "member shorter than the RDSC trailer")
    trailer = data[-TRAILER:]
    _need(trailer[:8] == RDSC, "RDSC trailer magic")
    word = struct.unpack("<I", trailer[8:])[0]
    tag, low29 = word >> 29, word & 0x1FFFFFFF

    if data[0] != PROFILE_B0:
        # raw form: payload ++ trailer; tag 0; low29 is the payload length; payload starts with a
        # little-endian dword in {1, 2}
        _need(len(data) >= 4 + TRAILER, "raw member too short")
        _need(tag == 0, "raw member tag must be 0")
        _need(low29 == len(data) - TRAILER, "raw member low29 != size - 12")
        _need(struct.unpack_from("<I", data, 0)[0] in (1, 2), "raw member first dword not in {1,2}")
        return {"form": "raw", "tag": tag, "size": len(data), "payload": low29}

    _need(len(data) >= 2 + 3 + 3 + 8 + TRAILER, "compressed member shorter than its fixed fields")
    b0, b1 = data[0], data[1]
    _need(b0 == PROFILE_B0, "block byte 0")
    _need(b1 in PROFILE_B1, "block byte 1 (decode type / checksum bit)")
    name, _cid, want_tag = PROFILE_B1[b1]

    chunk = int.from_bytes(data[2:5], "big")
    _need(chunk >> 18 == 0, "chunk header flag/special bits must be 0")
    _need(chunk != 0x3FFFF, "chunk header special value")
    comp_len = chunk + 1
    _need(5 + comp_len + TRAILER == len(data), "size != 5 + compLen + 12")

    quantum = int.from_bytes(data[5:8], "big")
    _need(quantum >> 23 == 1, "quantum compressed flag")
    _need((quantum >> 19) & 0xF == 1, "quantum mode must be 1")
    _need((quantum & 0x7FFFF) == comp_len - 3, "quantum size != compLen - 3")

    _need(tag == want_tag, "trailer tag does not match the compressor")
    _need(comp_len < low29 <= MAX_QUANTUM_RAW, "rawLen must exceed compLen and fit one quantum")
    first = struct.unpack_from("<I", data, 8)[0]
    _need(first in (1, 2), "raw prefix dword not in {1,2}")
    return {"form": "compressed", "codec": name, "tag": tag, "comp_len": comp_len,
            "raw_len": low29, "size": len(data), "prefix": data[8:16]}


def build_fixture(codec_byte: int = 0x0A, raw_len: int = 9000, comp_len: int = 2000) -> bytearray:
    """Synthetic member (no game bytes): correct container fields around a deterministic filler."""
    name, _cid, tag = PROFILE_B1[codec_byte]
    out = bytearray([PROFILE_B0, codec_byte])
    out += (comp_len - 1).to_bytes(3, "big")
    out += ((1 << 23) | (1 << 19) | (comp_len - 3)).to_bytes(3, "big")
    out += struct.pack("<II", 2, 1)                                   # 8 raw output bytes
    out += bytes((i * 131 + 7) & 0xFF for i in range(comp_len - 3 - 8))
    out += RDSC + struct.pack("<I", (tag << 29) | raw_len)
    assert len(out) == comp_len + 17
    return out


def build_raw_fixture(length: int = 400) -> bytearray:
    out = bytearray(struct.pack("<II", 1, 7)) + bytearray((i * 29) & 0xFF for i in range(length - 8))
    out += RDSC + struct.pack("<I", length)
    return out


def _set(buf: bytearray, off: int, value: int) -> bytearray:
    out = bytearray(buf)
    out[off] = value & 0xFF
    return out


def mutation_probes() -> list[tuple[str, bytes]]:
    probes: list[tuple[str, bytes]] = []
    good = build_fixture()

    for bit in range(8):                                   # block byte 0
        probes.append((f"b0 flip bit{bit}", bytes(_set(good, 0, good[0] ^ (1 << bit)))))
    for bit in range(8):                                   # block byte 1 (0x0a)
        probes.append((f"b1 flip bit{bit}", bytes(_set(good, 1, good[1] ^ (1 << bit)))))
    probes.append(("b1 -> 0x07 (decode type 7)", bytes(_set(good, 1, 0x07))))
    probes.append(("b1 -> 0x8c checksum+type 12", bytes(_set(good, 1, 0x8C))))

    chunk = int.from_bytes(good[2:5], "big")
    for delta in (-1, 1, 1000):
        mutated = bytearray(good)
        mutated[2:5] = (chunk + delta).to_bytes(3, "big")
        probes.append((f"chunk header {delta:+d}", bytes(mutated)))
    for bit in (18, 19, 20, 23):
        mutated = bytearray(good)
        mutated[2:5] = (chunk | (1 << bit)).to_bytes(3, "big")
        probes.append((f"chunk header bit{bit} set", bytes(mutated)))
    special = bytearray(good)
    special[2:5] = (0x3FFFF).to_bytes(3, "big")
    probes.append(("chunk header special 0x3ffff", bytes(special)))

    quantum = int.from_bytes(good[5:8], "big")
    for name, value in (("compressed flag cleared", quantum & ~(1 << 23)),
                        ("mode 0", (quantum & ~(0xF << 19))),
                        ("mode 2", (quantum & ~(0xF << 19)) | (2 << 19)),
                        ("mode 15", (quantum & ~(0xF << 19)) | (15 << 19)),
                        ("size +1", quantum + 1),
                        ("size -1", quantum - 1),
                        ("size 0", quantum & ~0x7FFFF)):
        mutated = bytearray(good)
        mutated[5:8] = value.to_bytes(3, "big")
        probes.append((f"quantum {name}", bytes(mutated)))

    for i in range(8):                                     # RDSC magic, every byte
        probes.append((f"trailer magic byte{i}", bytes(_set(good, len(good) - 12 + i,
                                                            good[len(good) - 12 + i] ^ 0x01))))
    word = struct.unpack("<I", good[-4:])[0]
    for name, value in (("tag 0", word & 0x1FFFFFFF), ("tag 4 on Mermaid", (4 << 29) | (word & 0x1FFFFFFF)),
                        ("tag 7", (7 << 29) | (word & 0x1FFFFFFF)),
                        ("rawLen == compLen", (3 << 29) | 2000),
                        ("rawLen < compLen", (3 << 29) | 1999),
                        ("rawLen 0x20001", (3 << 29) | 0x20001),
                        ("rawLen 0", (3 << 29) | 0)):
        mutated = bytearray(good)
        mutated[-4:] = struct.pack("<I", value)
        probes.append((f"trailer {name}", bytes(mutated)))
    kraken = build_fixture(0x06, 5000, 1500)
    probes.append(("Kraken with Mermaid tag", bytes(kraken[:-4] + struct.pack("<I", (3 << 29) | 5000))))

    probes.append(("truncated by 1", bytes(good[:-1])))
    probes.append(("extended by 1", bytes(good + b"\x00")))
    probes.append(("truncated to 10", bytes(good[:10])))
    probes.append(("empty", b""))
    probes.append(("trailer only", bytes(RDSC + struct.pack("<I", 12))))
    for off, value in ((8, 3), (8, 0), (8, 0x01000000)):
        mutated = bytearray(good)
        mutated[off:off + 4] = struct.pack("<I", value)
        probes.append((f"prefix dword {value:#x}", bytes(mutated)))
    probes.append(("memcpy block (0xcc)", bytes(_set(good, 0, 0xCC))))

    raw = build_raw_fixture()
    for i in range(8):
        probes.append((f"raw trailer magic byte{i}", bytes(_set(raw, len(raw) - 12 + i,
                                                                raw[len(raw) - 12 + i] ^ 0x01))))
    raw_word = struct.unpack("<I", raw[-4:])[0]
    for name, value in (("size +1", raw_word + 1), ("size -1", raw_word - 1), ("tag 3", (3 << 29) | raw_word)):
        mutated = bytearray(raw)
        mutated[-4:] = struct.pack("<I", value)
        probes.append((f"raw {name}", bytes(mutated)))
    probes.append(("raw first dword 3", bytes(struct.pack("<I", 3) + raw[4:])))
    probes.append(("raw first dword 0x8c", bytes(_set(raw, 0, 0x8C))))
    return probes


def check_public() -> int:
    for label, builder in (("Mermaid", lambda: build_fixture(0x0A, 9000, 2000)),
                           ("Kraken", lambda: build_fixture(0x06, 5000, 1500)),
                           ("tiny", lambda: build_fixture(0x0A, 40, 20)),
                           ("max quantum", lambda: build_fixture(0x06, MAX_QUANTUM_RAW, 4096))):
        info = validate_member(bytes(builder()))
        if info["form"] != "compressed":
            fail(f"fixture {label} not classified compressed")
    info = validate_member(bytes(build_fixture(0x0A, 9000, 2000)))
    if (info["comp_len"], info["raw_len"], info["tag"], info["codec"]) != (2000, 9000, 3, "Mermaid"):
        fail(f"fixture decode mismatch {info}")
    info = validate_member(bytes(build_fixture(0x06, 5000, 1500)))
    if (info["tag"], info["codec"]) != (4, "Kraken"):
        fail(f"Kraken fixture decode mismatch {info}")
    if validate_member(bytes(build_raw_fixture()))["form"] != "raw":
        fail("raw fixture not classified raw")

    probes = mutation_probes()
    survivors = []
    for name, data in probes:
        try:
            validate_member(data)
        except Reject:
            continue
        survivors.append(name)
    if survivors:
        fail(f"{len(survivors)} public mutation probes survived: {survivors}")
    return len(probes)


# ---------------------------------------------------------------------------------------------------
# Layer 2: ELF pins
# ---------------------------------------------------------------------------------------------------

def elf_pins(e) -> None:
    def need(cond: bool, message: str) -> None:
        if not cond:
            raise SystemExit(f"FAIL {message}")

    for name, rva, size, sha, first, last in ELF_BODIES:
        e.body_hash(name, rva, size, sha)
        if first:
            e.bytes_at(rva, first, f"{name} prologue")
        if last:
            e.bytes_at(rva + size - 4, last, f"{name} last 4 bytes")

    for addr, hexstr, what in BYTE_PINS:
        e.bytes_at(addr, hexstr, what)
    for site, target, what in CALL_PINS:
        need(e.call(site) == target, f"call at {site:#x} ({what}) -> {target:#x}")
    for site, target, what in LEA_PINS:
        need(e.lea(site) == target, f"lea at {site:#x} ({what}) -> {target:#x}")
    for addr, text in STRING_PINS:
        e.cstring(addr, text)

    table = e.read(DECODE_TABLE, 4 * len(DECODE_TO_COMPRESSOR))
    need(struct.unpack("<%dI" % len(DECODE_TO_COMPRESSOR), table) == DECODE_TO_COMPRESSOR,
         "decode-type -> compressor table 0x1295090")
    for cid, addr in ((8, 0x1295036), (9, 0x129503D)):
        need(e.u64(NAME_TABLE + 8 * cid) == addr, f"compressor name pointer {cid}")
    # tag 4 <-> Kraken (compressor 8, decode type 6), tag 3 <-> Mermaid (9, 10): both resolved via the table
    need(DECODE_TO_COMPRESSOR[0x06] == 8 and DECODE_TO_COMPRESSOR[0x0A] == 9,
         "profile bytes 0x06/0x0a do not resolve to Kraken/Mermaid")
    # selector bit set 0x51 >> (type - 6): types 6, 10 and 12 select the QH parser; 7/8/9/11 do not
    selected = [t for t in range(6, 13) if (0x51 >> (t - 6)) & 1]
    need(selected == [6, 10, 12], f"selector 0x51 selects {selected}, expected [6, 10, 12]")
    # descriptor trailer tag: 8 raw bytes of the movabs are exactly the corpus RDSC magic
    need(e.read(0xB0D422, 8) == RDSC, "descriptor immediate != corpus RDSC magic")


def check_elf() -> int:
    from slot_pin_util import Elf  # lazy: absent private inputs must be a clean SKIP

    e = Elf()
    e.recording = True
    elf_pins(e)
    e.recording = False

    e.skip_hash = True
    e.recording = True
    e.pins = set()
    elf_pins(e)
    e.recording = False
    mutants = survivors = 0
    for addr, size in sorted(e.pins):
        for byte_addr in sorted({addr, addr + size - 1}):
            mutants += 1
            e.overlay = {byte_addr: e.raw(byte_addr, 1)[0] ^ 0x01}
            try:
                elf_pins(e)
            except SystemExit:
                continue
            survivors += 1
            print(f"mutation survived at {byte_addr:#x} (pin {addr:#x}+{size})", file=sys.stderr)
    e.overlay = {}
    e.skip_hash = False
    if survivors:
        fail(f"{survivors} ELF mutations survived")
    elf_pins(e)                       # pristine image still passes after the mutation run
    return mutants


# ---------------------------------------------------------------------------------------------------
# Layer 3: the real corpus (read in memory only)
# ---------------------------------------------------------------------------------------------------

def check_corpus() -> dict:
    from psarc import Psarc

    stats = {"total": 0, "compressed": 0, "raw": 0, "mermaid": 0, "kraken": 0,
             "max_raw": 0, "max_comp": 0}
    for name in CORPUS_ARCHIVES:
        archive = Psarc(CORPUS_DIR / name)
        try:
            for path, index in archive.members():
                if not path.endswith(".csdr"):
                    continue
                data = archive.read(index)
                try:
                    info = validate_member(data)
                except Reject as exc:
                    fail(f"{name}:{path}: {exc}")
                stats["total"] += 1
                if info["form"] == "raw":
                    stats["raw"] += 1
                else:
                    stats["compressed"] += 1
                    stats["mermaid" if info["codec"] == "Mermaid" else "kraken"] += 1
                    stats["max_raw"] = max(stats["max_raw"], info["raw_len"])
                    stats["max_comp"] = max(stats["max_comp"], info["comp_len"])
        finally:
            archive.close()
    want = {"total": CORPUS_TOTAL, "compressed": CORPUS_COMPRESSED, "raw": CORPUS_RAW,
            "mermaid": CORPUS_MERMAID, "kraken": CORPUS_KRAKEN,
            "max_raw": CORPUS_MAX_RAWLEN, "max_comp": CORPUS_MAX_COMPLEN}
    if stats != want:
        fail(f"corpus statistics {stats} != pinned {want}")
    return stats


def main(argv: list[str]) -> int:
    require_private = "--require-private" in argv
    skip_corpus = "--no-corpus" in argv

    probes = check_public()
    parts = [f"public layer: fixtures accepted, {probes} mutation probes rejected"]

    if not ELF_PATH.is_file():
        if require_private:
            fail("local/re/eboot.elf absent and --require-private given")
        print("OK check_csdr_b0cc50_container: " + parts[0])
        print("SKIP check_csdr_b0cc50_container: private ELF not present (ELF pins and corpus not run)")
        return 0
    digest = hashlib.sha256(ELF_PATH.read_bytes()).hexdigest()
    if digest != "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3":
        fail("unexpected ELF SHA256")
    mutants = check_elf()
    parts.append(f"ELF layer: {len(BYTE_PINS)} byte pins, {len(CALL_PINS)} calls, {len(LEA_PINS)} leas, "
                 f"{len(STRING_PINS)} strings, {len(ELF_BODIES)} body hashes, tables, {mutants} mutants rejected")

    if skip_corpus:
        parts.append("corpus layer: NOT RUN (--no-corpus)")
    elif not (CORPUS_DIR / "pak0.psarc").is_file():
        if require_private:
            fail("corpus archives absent and --require-private given")
        parts.append("corpus layer: SKIP (archives absent)")
    else:
        s = check_corpus()
        parts.append(f"corpus layer: {s['total']} .csdr members = {s['compressed']} compressed "
                     f"({s['mermaid']} Mermaid + {s['kraken']} Kraken) + {s['raw']} raw, all in profile, "
                     f"max rawLen {s['max_raw']}, max compLen {s['max_comp']}")
    print("OK check_csdr_b0cc50_container: " + "; ".join(parts))
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
