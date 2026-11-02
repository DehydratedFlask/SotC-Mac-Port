#!/usr/bin/env python3
"""Tie the slot-23 `0xe71990` name/hash table to its only consumers (private; needs the ELF).

Slot 23 publishes 14 `{name pointer, u32 hash}` records at BSS `0x3353f40`. This checker pins the
*consumer* side, not just the constructor:

  1. Bodies of the constructor, the two accessors, the hash function and the semantic resolver
     (`0xe70af0`) are compared by SHA-256 with the pinned image.
  2. The two accessors are proved to be the only readers: both decode to `cmp esi,0xc` / `ja` /
     `shl 4` against the same base `0x3353f40`, one loading `+0x00` (name) and one `+0x08` (hash).
  3. The three RIP-relative references to the table are the only ones, and the file-backed image
     contains no absolute 4- or 8-byte literal of the table base (no data pointer, no relocation
     addend), so a third reader cannot exist as data.
  4. Both accessors are virtual members of the context class installed by `0xe71610`: vtable
     `0x15f13f0` slots `+0x28`/`+0x30`/`+0x38`/`+0x40`/`+0xa8` are the name accessor, the hash
     accessor, the hash function, the resolver and the hash->index search.
  5. `0xe70af0` hashes the name through `+0x38` and searches through `+0xa8`, then compares the
     hash against the 13 preset slot hashes through `+0x30` -- that is what makes the stored hash
     column load-bearing for the string bytes.
  6. The single consumer `0xe5bda0` calls the name accessor at exactly `0xe5c3f5` (semantic
     registration) and `0xe5c441` (invalid-format diagnostic), and the loop that feeds it is
     bounded by `cmpq $0xd` (indices 0..12), so record 13 is never read.
  7. `0xe71990` has no direct call site; it is reached only as entry 23 of the 573-entry table at
     `0x2dd70e0`.
  8. Optional (private fixture): the 14 fixture strings are byte-identical to the pinned image at
     the 14 addresses the constructor stores, and `djb2` over each of records 0..12 equals the
     stored hash constant; record 13 hashes to 0 and is the never-read sentinel.

Skips cleanly (exit 0, "SKIP") when `local/re/eboot.elf` is absent. The fixture and the private
disassembly census are optional sub-checks: when either input is missing that section reports
SKIP and the rest still runs.
"""

from __future__ import annotations

import hashlib
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF = ROOT / "local/re/eboot.elf"
DIS = ROOT / "local/validation/registry-analysis-agent/all.dis"
FDE = ROOT / "local/validation/registry-analysis-agent/fde_ranges.json"
FIXTURE = ROOT / "local/fixtures/slot23_e71990_names.bin"

ELFA_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"

TABLE_BASE = 0x3353F40
VTABLE = 0x15F13F0
CTOR_TABLE_BASE = 0x2DD70E0
CTOR_ENTRY_23 = 0xE71990

# (rva, length, body sha256) -- the producer and its only readers with the helpers they use.
BODIES = (
    (0xE71990, 337, "3932fb85b759ae53dd1675142ffda4b449be725f241cfeefdb40a07a2beaf774"),
    (0xE70A60, 26, "823cd67691612f512d0cdf1a8efe3fe09ed9df8a7d7aa2d3f91eadd69c731c53"),
    (0xE70A80, 26, "c4fd689120aa2f5fa72788a1f849aec86b3551a180a5a17728125292b0c4e4fd"),
    (0xE70AA0, 67, "85516bb8f64e82a12498c2f395a6d3442101961154e95d5fdfa679b6e0e0459d"),
    (0xE70AF0, 1552, "5e41af6c63f72f6f2cd61656b739bc6e24f0d609acc0552fd6c52efedb4f4562"),
    (0xE71310, 70, "5809bf478d6dd8732c22fd8b6744d3f6f8e168bb42106c8b8aedf7475eb547e4"),
    (0xE713B0, 70, "e7f018d092a0c3f703327f934f87fab97542d0001605c6607c425ba5f6a99a19"),
    (0xE715D0, 53, "a278456dbe0685b33e5e9bc9c81ba0365e6bbe971c60947ea336c62f67b1fcd4"),
    (0xE71610, 74, "8d1a0ab2f015b5c0d799e918cbdbf956fdc76f23da082df74e440c47d2d307bf"),
)

# FDE extents from tools/lift/cfg_census.py `load_fde_ranges` (start -> length).
FDE_PINS = {
    0xE5BDA0: 0x1BAC,   # the single consumer body (Ghidra body 6,945 B inside this extent)
    0xE70A60: 26,
    0xE70A80: 26,
    0xE70AA0: 67,
    0xE70AF0: 1552,
    0xE71310: 70,
    0xE715D0: 53,
    0xE71610: 74,
    0xE71990: 337,
}

VSLOTS = ((0x28, 0xE70A60), (0x30, 0xE70A80), (0x38, 0xE70AA0), (0x40, 0xE70AF0), (0xA8, 0xE715D0))

# Call sites inside `0xe5bda0` (FDE 0xe5bda0..0xe5d94c).
CALLS = (
    (0xE5C3F5, 0xE70A60, "name for semantic slot k"),
    (0xE5C400, 0xE70AF0, "name -> semantic index"),
    (0xE5C40D, 0xE71310, "store format for that index"),
    (0xE5C441, 0xE70A60, "name in the invalid-format diagnostic"),
    (0xE5C54F, 0xE70AF0, "per-element name -> index"),
    (0xE5C5BD, 0xE70AF0, "VERTEX_ORIGINAL_INDEX registration"),
    (0xE5CD78, 0xE70AA0, "hash a stored name pointer"),
    (0xE5CE2A, 0xE70AA0, "hash a stored name pointer"),
)

# The 14 name addresses in record order (stored at 0x3353f40 + 0x10*k + 0x00).
NAME_RVAS = (
    0x131C990, 0x131C9A2, 0x131C9B2, 0x131C9C3, 0x131C9D5, 0x131C9E4, 0x131C9F7,
    0x131CA0A, 0x131CA1D, 0x131CA30, 0x131CA44, 0x131CA59, 0x131CA78, 0x131CA94,
)

# Native copy of the 14 hash immediates (native/core/leaf_e71990.hpp `kLeafE71990Hash`).
HASHES = (
    0xF1E58049, 0x257DA323, 0x47A2C413, 0xED7E6AE8, 0xB2D47DCD, 0x4941F51C, 0x4941F51D,
    0x4941F51E, 0x4941F51F, 0x10EF7257, 0x7CAE59C9, 0x330BBB24, 0x2B1488E7, 0x00000000,
)

BANNER = "check_slot23_consumers"


def fail(msg: str) -> "NoReturn":  # noqa: F821 - typing only
    raise SystemExit(f"FAIL {BANNER}: {msg}")


def require(cond: bool, msg: str) -> None:
    if not cond:
        fail(msg)


def u32(blob: bytes, off: int) -> int:
    return struct.unpack_from("<I", blob, off)[0]


def u64(blob: bytes, off: int) -> int:
    return struct.unpack_from("<Q", blob, off)[0]


def djb2_xor(data: bytes) -> int:
    """`sotc::name_hash`: h = 5381; h = (h*33) ^ sign_extend32(byte); 0 -> 1."""
    h = 5381
    for b in data:
        h = ((h * 33) ^ (b - 256 if b >= 128 else b)) & 0xFFFFFFFF
    return h or 1


def check_call(img: d.Image, va: int, target: int, label: str) -> None:
    raw = img.read(va, 5)
    require(raw[0] == 0xE8, f"0x{va:x} ({label}) is not a direct call")
    got = va + 5 + struct.unpack_from("<i", raw, 1)[0]
    require(got == target, f"0x{va:x} calls 0x{got:x}, expected 0x{target:x} ({label})")


def main() -> int:
    if not ELF.is_file():
        print(f"SKIP {BANNER}: local/re/eboot.elf is absent (private game image required)")
        return 0

    raw = ELF.read_bytes()
    digest = hashlib.sha256(raw).hexdigest()
    require(digest == ELFA_SHA256, f"ELF sha256 {digest} != pinned {ELFA_SHA256}")
    _, ph = d.program_headers(raw)
    img = d.Image(raw, ph)

    # 1. Producer and reader bodies.
    for rva, size, want in BODIES:
        got = hashlib.sha256(img.read(rva, size)).hexdigest()
        require(got == want, f"0x{rva:x} body sha256 {got} != pinned {want}")

    # 2. The two accessors read the same base with the same bounds and a 0x10 stride.
    name_acc = img.read(0xE70A60, 26)
    hash_acc = img.read(0xE70A80, 26)
    require(name_acc[:3] == b"\x31\xc0\x83" and name_acc[3:6] == b"\xfe\x0c\x77",
            "0xe70a60 lost the `xor eax,eax; cmp esi,0xc; ja` guard")
    require(name_acc[10:13] == b"\x48\x8d\x0d", "0xe70a60 no longer leas its table through rcx")
    require(name_acc[13:17] == b"\xcf\x34\x4e\x02", "0xe70a60 table displacement changed")
    require(name_acc[17:21] == b"\x48\xc1\xe0\x04", "0xe70a60 lost the 16-byte stride")
    require(name_acc[21:26] == b"\x48\x8b\x04\x08\xc3", "0xe70a60 no longer returns record +0x00")
    require(hash_acc[13:17] == b"\xaf\x34\x4e\x02", "0xe70a80 table displacement changed")
    require(hash_acc[21:26] == b"\x8b\x44\x08\x08\xc3", "0xe70a80 no longer returns record +0x08")

    lea = 0xE70A6A
    require(lea + 7 + struct.unpack_from("<i", name_acc, 13)[0] == TABLE_BASE,
            "0xe70a60 lea no longer computes 0x3353f40")
    lea = 0xE70A8A
    require(lea + 7 + struct.unpack_from("<i", hash_acc, 13)[0] == TABLE_BASE,
            "0xe70a80 lea no longer computes 0x3353f40")

    # 3. No data pointer / relocation addend to the table anywhere in the file-backed image.
    for width in (4, 8):
        pat = TABLE_BASE.to_bytes(width, "little")
        hits = raw.count(pat)
        require(hits == 0, f"{hits} absolute {width}-byte literal(s) of the table base in the image")

    # 4. The context class installs vtable 0x15f13f0; its slots are the reader trio.
    ctor = img.read(0xE71610, 74)
    require(ctor[6:9] == b"\x48\x8d\x05", "0xe71610 no longer leas its vtable")
    require(ctor[13:20] == b"\x48\x89\xfb\x48\x83\xc0\x10", "0xe71610 vtable store sequence changed")
    lea_target = 0xE71610 + 13 + struct.unpack_from("<i", ctor, 9)[0]
    require(lea_target + 0x10 == VTABLE, f"0xe71610 vtable base 0x{lea_target + 0x10:x} != 0x15f13f0")
    vt = img.read(VTABLE, 0xB0)
    for off, want in VSLOTS:
        got = u64(vt, off)
        require(got == want, f"vtable+0x{off:x} = 0x{got:x}, expected 0x{want:x}")

    # 5. The resolver hashes by name (+0x38), searches by hash (+0xa8), then compares the hash
    #    against the 13 preset slot hashes through +0x30. Those three indirect sites are pinned.
    resolve = img.read(0xE70AF0, 1552)
    require(resolve[0x3D:0x43] == b"\x4c\x89\xfe\xff\x50\x38",
            "0xe70af0 lost the +0x38 name hash call")
    require(resolve[0x50:0x56] == b"\xff\x90\xa8\x00\x00\x00",
            "0xe70af0 lost the +0xa8 hash search call")
    require(resolve.count(b"\xff\x50\x30") == 13,
            "0xe70af0 no longer compares against exactly 13 preset slot hashes")
    require(resolve[0x70:0x73] == b"\xff\x50\x30", "0xe70af0 first +0x30 compare moved")

    # 6. The single consumer's call sites and its 0..12 loop bound.
    for va, target, label in CALLS:
        check_call(img, va, target, label)
    require(img.read(0xE5C48E, 4) == b"\x48\x83\xfb\x0d", "0xe5bda0 loop no longer compares index to 13")
    require(img.read(0xE5C48A, 4) == b"\x49\x83\xc7\x28", "0xe5bda0 lost the 0x28 row stride")
    # The 13-way dispatch table is indexed by k (0..12) and gates on the format code in r13d.
    require(img.read(0xE5C350, 5) == b"\x45\x8b\x2f\x89\xd8", "0xe5bda0 format load changed")

    # 7. The constructor is table-only: no direct call site, entry 23 of the backward table.
    require(raw.count(CTOR_ENTRY_23.to_bytes(4, "little")) == 2,
            "0xe71990 literal count changed (expected the table entry and its relocation record)")
    entry = u64(img.read(CTOR_TABLE_BASE - 8 * 23, 8), 0)
    require(entry == CTOR_ENTRY_23, f"ctor table entry 23 = 0x{entry:x}, expected 0xe71990")
    require(u64(img.read(CTOR_TABLE_BASE - 8 * 573, 8), 0) == 0xFFFFFFFFFFFFFFFF,
            "ctor table sentinel below entry 572 is not -1")

    # 8. Optional: private disassembly census (proves there is no fourth table reference and no
    #    call site the address pins missed).
    if DIS.is_file():
        text = DIS.read_text(errors="replace")
        lines = [ln.strip() for ln in text.splitlines() if "3353f40" in ln]
        require(len(lines) == 3, f"disassembly has {len(lines)} table references, expected 3")
        require(lines[0].split(":")[0].strip() == "e70a6a", "first table reference is not 0xe70a6a")
        require(lines[1].split(":")[0].strip() == "e70a8a", "second table reference is not 0xe70a8a")
        require(lines[2].split(":")[0].strip() == "e719a5", "third table reference is not 0xe719a5")
        for target, expected in ((0xE70A60, 2), (0xE70AF0, 3), (0xE70AA0, 2), (0xE715D0, 2)):
            got = text.count(f"callq\t0x{target:x} ")
            require(got == expected, f"disassembly has {got} calls to 0x{target:x}, expected {expected}")
    else:
        print(f"  SKIP disassembly census: {DIS.relative_to(ROOT)} is absent")

    # Optional: FDE extents.
    if FDE.is_file():
        import json
        ranges = json.loads(FDE.read_text())
        for start, size in FDE_PINS.items():
            got = ranges.get(hex(start))
            require(got == size, f"FDE 0x{start:x} length {got} != pinned {size}")
    else:
        print(f"  SKIP FDE census: {FDE.relative_to(ROOT)} is absent")

    # Optional: private fixture -- the strings must reproduce the original bytes and hashes.
    if FIXTURE.is_file():
        blob = FIXTURE.read_bytes()
        require(blob[:4] == b"S23N", "fixture magic is not S23N")
        version, count = u32(blob, 4), u32(blob, 8)
        require(version == 1 and count == 14, f"fixture header v{version} count={count}, expected v1/14")
        off = 12
        for k in range(14):
            n = u32(blob, off)
            off += 4
            body = blob[off:off + n]
            off += n
            require(len(body) == n, f"fixture record {k} truncated")
            original = img.read(NAME_RVAS[k], n + 1)
            require(original[-1] == 0, f"0x{NAME_RVAS[k]:x} no longer ends the string at +0x{n}")
            require(body == original[:n], f"fixture record {k} differs from 0x{NAME_RVAS[k]:x}")
            if k < 13:
                require(djb2_xor(body) == HASHES[k],
                        f"djb2(record {k}) != 0x{HASHES[k]:08x}")
        require(HASHES[13] == 0 and off == len(blob),
                "record 13 is not the zero-hash sentinel or the fixture has trailing bytes")
    else:
        print(f"  SKIP fixture comparison: {FIXTURE.relative_to(ROOT)} is absent")

    print(f"OK {BANNER}: table 0x{TABLE_BASE:x} has exactly two accessors (0xe70a60 name, "
          f"0xe70a80 hash), one consumer 0xe5bda0, 13 read slots and an unread sentinel")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
