#!/usr/bin/env python3
"""ELF-tie the two owner vtable slot bodies `0xb111e0` (+0x48) and `0xb11210` (+0x58).

Private: needs local/re/eboot.elf. Pins the sha256 of both bodies plus `0x256970`, `0xa46dc0` and
`0xac53e0`, the rip-relative globals, the call targets, the stored constants, and the vtable slot
wiring that makes `+0x48`/`+0x58` slot offsets rather than object fields.
"""

from __future__ import annotations

import hashlib
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"

FUNCTIONS = [
    ("0xb111e0 slot +0x48", 0xB111E0, 0x19,
     "945e386bb60085b5efbb2346579110e30585ec111a4bbd62dc83d3d2b96beeb7"),
    ("0x256970 singleton ensure", 0x256970, 0x51,
     "2bb1eb8da983befc57fab8b2f7d2681f998051be4eddbb278928fa8b19523083"),
    ("0xa46dc0 singleton activate", 0xA46DC0, 0x65,
     "ed066f35d8865132edfcf152ac7c87f23b299755184921eabfeed001e1eba592"),
    ("0xb11210 slot +0x58", 0xB11210, 0x69,
     "50c2226b14670a3fdec8b86b0f41a5963a0a476e8810926bd6c30dd91e453a9c"),
    ("0xac53e0 tree0 insert", 0xAC53E0, 0xB2,
     "9c950f6fd35b8982f50d2f355374a9869c5282ac597ae2a94b7598c1400c65ad"),
]

HEADER48 = ROOT / "native/core/owner_ctor_b111e0_slot48.hpp"
SOURCE48 = ROOT / "native/core/owner_ctor_b111e0_slot48.cpp"
HEADER58 = ROOT / "native/core/owner_ctor_b11210_slot58.hpp"
SOURCE58 = ROOT / "native/core/owner_ctor_b11210_slot58.cpp"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


PT_LOAD = 1
PT_SCE_RELRO = 0x61000010


def load_segments(elf: bytes) -> list[tuple[int, int, int]]:
    """(vaddr, file_offset, filesz) for PT_LOAD, then for the file-backed SCE RELRO window.

    The RELRO segment holds both owner vtables; it is not PT_LOAD, so a PT_LOAD-only map would
    report the vtable reads as missing.
    """
    e_phoff = struct.unpack_from("<Q", elf, 0x20)[0]
    e_phentsize, e_phnum = struct.unpack_from("<HH", elf, 0x36)
    loads, relro = [], []
    for i in range(e_phnum):
        base = e_phoff + i * e_phentsize
        p_type = struct.unpack_from("<I", elf, base)[0]
        if p_type not in (PT_LOAD, PT_SCE_RELRO):
            continue
        p_offset, p_vaddr = struct.unpack_from("<QQ", elf, base + 8)
        p_filesz = struct.unpack_from("<Q", elf, base + 32)[0]
        (loads if p_type == PT_LOAD else relro).append((p_vaddr, p_offset, p_filesz))
    return loads + relro


def read_rva(elf: bytes, segments: list[tuple[int, int, int]], rva: int, size: int) -> bytes:
    for vaddr, offset, filesz in segments:
        if vaddr <= rva and rva + size <= vaddr + filesz:
            start = offset + (rva - vaddr)
            return elf[start:start + size]
    raise SystemExit(f"FAIL rva {rva:#x} (+{size:#x}) is not file-backed by any PT_LOAD")


def qword(elf: bytes, segments, rva: int) -> int:
    return struct.unpack("<Q", read_rva(elf, segments, rva, 8))[0]


def rel32_target(elf: bytes, segments, site: int) -> int:
    """Target of an E8/E9 rel32 at `site`."""
    rel = struct.unpack("<i", read_rva(elf, segments, site + 1, 4))[0]
    return site + 5 + rel


def rip_target(elf: bytes, segments, site: int, length: int) -> int:
    """Target of a rip-relative disp32 at `site + 3` (lea/mov, 7 bytes)."""
    disp = struct.unpack("<i", read_rva(elf, segments, site + 3, 4))[0]
    return site + length + disp


def pin(elf: bytes, segments, addr: int, pattern: str, what: str) -> None:
    """Assert the exact bytes at `addr`; the length comes from the pattern."""
    require(read_rva(elf, segments, addr, len(pattern) // 2) == bytes.fromhex(pattern), what)


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    segments = load_segments(elf)

    for name, rva, size, sha in FUNCTIONS:
        body = read_rva(elf, segments, rva, size)
        require(hashlib.sha256(body).hexdigest() == sha, f"{name} sha256")
        # The byte after each body is padding, never a second entry point.
        require(read_rva(elf, segments, rva + size, 1) == b"\x90", f"{name} not padded")

    # 0xb111e0: call 0x256970, load the cell, tail-jump 0xa46dc0.
    require(rel32_target(elf, segments, 0xB111E4) == 0x256970, "0xb111e0 -> 0x256970 call")
    require(rip_target(elf, segments, 0xB111E9, 7) == 0x32F79A8, "0xb111e0 cell lea")
    pin(elf, segments, 0xB111F0, "488b38", "cell load [rax]")
    require(rel32_target(elf, segments, 0xB111F4) == 0xA46DC0, "0xb111e0 tail jmp")

    # 0x256970: new(0x210), ctor 0xa46c70, replace 0x32e79a8, store at 0x2569b3.
    pin(elf, segments, 0x25697A, "bf10020000", "size 0x210")
    require(rel32_target(elf, segments, 0x25697F) == 0x17070, "0x256970 -> new")
    require(rel32_target(elf, segments, 0x25698A) == 0xA46C70, "0x256970 -> ctor")
    require(rip_target(elf, segments, 0x25698F, 7) == 0x32F79A8, "0x256970 cell lea")
    require(rel32_target(elf, segments, 0x2569A6) == 0xA46D10, "0x256970 -> dtor")
    require(rel32_target(elf, segments, 0x2569AE) == 0x170B0, "0x256970 -> free")
    pin(elf, segments, 0x2569B3, "4d8937", "cell store")

    # 0xa46dc0: priority 0x2bc, sceUserServiceInitialize, the NID j7DlalBzHh8 call, +0x178 = 1.
    pin(elf, segments, 0xA46DE0, "c745e0bc020000", "priority 0x2bc")
    require(rel32_target(elf, segments, 0xA46DE7) == 0xF90E00, "0xa46dc0 -> user service")
    pin(elf, segments, 0xA46DEC, "bf01000000", "share arg0 = 1")
    pin(elf, segments, 0xA46DF1, "be00000200", "share arg1 = 0x20000")
    pin(elf, segments, 0xA46DF6, "babc020000", "share arg2 = 0x2bc")
    pin(elf, segments, 0xA46DFB, "b93f000000", "share arg3 = 0x3f")
    require(rel32_target(elf, segments, 0xA46E00) == 0xF91360, "0xa46dc0 -> share call")
    pin(elf, segments, 0xA46E05, "c6837801000001", "flag +0x178 = 1")

    # 0xb11210: canary, gate read, key 4, the record qword, tag 2, call 0xac53e0.
    require(rip_target(elf, segments, 0xB11219, 7) == 0x15FE518, "stack canary global")
    require(rip_target(elf, segments, 0xB11220, 7) == 0x2DFCE70, "retail gate lea")
    pin(elf, segments, 0xB1122E, "488b4160", "gate field +0x60")
    pin(elf, segments, 0xB11232, "803800", "gate byte test")
    pin(elf, segments, 0xB11235, "752d", "gate skip branch")
    require(rip_target(elf, segments, 0xB11237, 7) == 0x3305568, "bag global lea")
    pin(elf, segments, 0xB11242, "be04000000", "key = 4")
    pin(elf, segments, 0xB1124A, "48b8420000000000803f", "record qword {i32 0x42, f32 1.0f}")
    pin(elf, segments, 0xB11258, "c745e802000000", "tag = 2")
    require(rel32_target(elf, segments, 0xB1125F) == 0xAC53E0, "0xb11210 -> tree0 insert")

    # 0xac53e0 reads the tree-0 handle at bag+0x18 and the sentinel at bag+0x20.
    pin(elf, segments, 0xAC5417, "498b4e20", "sentinel +0x20")
    pin(elf, segments, 0xAC541E, "498d7618", "handle +0x18")
    pin(elf, segments, 0xAC53F7, "bf30000000", "node size 0x30")

    # Vtable wiring: slots +0x48 and +0x58 of both owner vtables, and 0x2569d0 at +0x40 of the
    # second one. Both tables are preceded by a zero offset-to-top and a null RTTI word.
    for vptr, name in ((0x15C8CD8, "class A"), (0x15D5B80, "class B")):
        require(qword(elf, segments, vptr - 16) == 0, f"{name} RTTI slot")
        require(qword(elf, segments, vptr - 8) == 0, f"{name} offset-to-top")
        require(qword(elf, segments, vptr + 0x48) == 0xB111E0, f"{name} slot +0x48")
        require(qword(elf, segments, vptr + 0x58) == 0xB11210, f"{name} slot +0x58")
    require(qword(elf, segments, 0x15D5B80 + 0x40) == 0x2569D0, "class B slot +0x40 = 0x2569d0")

    header48 = HEADER48.read_text()
    for token in (
        "NOT a constructor",
        "slot `+0x48`",
        "kOwnerSlot48Size = 0x19",
        "kOwnerSingletonObjectSize = 0x210",
        "kOwnerSingletonFlagOffset = 0x178",
        "kOwnerUserServicePriority = 0x2BC",
        "j7DlalBzHh8",
        "G1 PARTIAL",
    ):
        require(token in header48, f"header48 token {token!r}")
    source48 = SOURCE48.read_text()
    for token in ("previous == object", "cell.object = object", "kOwnerSingletonFlagOffset] = 1"):
        require(token in source48, f"source48 token {token!r}")

    header58 = HEADER58.read_text()
    for token in (
        "NOT a constructor",
        "slot `+0x58`",
        "kOwnerSlot58Size = 0x69",
        "kOwnerSlot58Key = 0x4",
        "kOwnerSlot58ValueInt = 0x42",
        "kOwnerBagTree0HandleOffset = 0x18",
        "kOwnerBagTree0SentinelOffset = 0x20",
        "G1 PARTIAL",
    ):
        require(token in header58, f"header58 token {token!r}")
    source58 = SOURCE58.read_text()
    for token in ("hooks.retail_build()", "kOwnerSlot58Key", "kOwnerSlot58Tag", "out.ok"):
        require(token in source58, f"source58 token {token!r}")

    print(
        "OK check_fowner_ctor_b111e0_b11210: 5 bodies hashed, cell 0x32f79a8, gate 0x2dfce70, "
        "bag 0x3305568, key 4 / i 0x42 / f 1.0f / tag 2, slots +0x48/+0x58 in both owner vtables, "
        "0x2569d0 at +0x40 of 0x15d5b80; guest RVAs NOT executed"
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
