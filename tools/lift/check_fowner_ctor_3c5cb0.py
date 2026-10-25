#!/usr/bin/env python3
"""ELF-tie the lifted row-18 owner constructor `0x3c5cb0` (`kOwnerMemberRows[18]`).

Private: needs `local/re/eboot.elf`. Read-only and offline: instructions are matched by raw bytes and
byte ranges are hashed; **no guest byte is executed**. No emulator, interpreter, JIT or guest runtime.

Layers:
  1. Body extent/hash: `0x3c5cb0..0x3c5ded` (FDE extent), `0x13d` B, `ret` at `0x3c5de6`, a noreturn
     `__stack_chk_fail` call, three `nop`s of padding, and the *next function* at `0x3c5df0`. This
     pins the correction to the E570 row table (it reported `0x227b` / 1528 insns / 31 calls: the
     walk fell through the noreturn call into the following functions).
  2. The ordered store census (26 sites: 14 `this`, 6 sentinel-node, 6 stack stores), each pinned by raw
     instruction bytes and its decoded displacement.
  3. The three allocations (`0x30`, `0x30`, `0x50`), the two `0xf180` slots, the `0x3b310` mutex
     address and the two registration calls with their callable frames and relocated slots.
  4. The row tuple `{0xb8, 0x3c5cb0, this+0x3b70, [0x2edf140], dtor 0x3c74b0 + 0x170b0}`.
  5. The native header/source/test must carry the constants that match the ELF-derived values.
  6. Mutation self-test (default on; `--no-mutate` to skip): every recorded pin is perturbed in an
     in-memory overlay (first and last byte) with the whole-body hash disabled, and every native
     token is removed in turn; the checker must fail each time.
"""

from __future__ import annotations

import hashlib
import re
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
ELF_PATH = ROOT / "local/re/eboot.elf"

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
BODY_START, BODY_END = 0x3C5CB0, 0x3C5DED
BODY_SHA256 = "2d30b3a53e7b438ab83f9f74888f8b0a1376c03b0d6b44a502afab74b7c562fe"

HEADER = ROOT / "native/core/owner_ctor_3c5cb0.hpp"
SOURCE = ROOT / "native/core/owner_ctor_3c5cb0.cpp"
TEST = ROOT / "native/tests/test_owner_ctor_3c5cb0.cpp"

# Store sites in instruction order (the two stack-canary stores are not census entries):
#   (addr, base, disp, width, disp_off, disp_size, raw bytes)
STORES = [
    (0x3C5CD5, "rbx", 0x00, 1, -1, 0, "c60300"),
    (0x3C5CD8, "rbx", 0x04, 4, 2, 1, "c7430400000000"),
    (0x3C5CED, "rbx", 0x30, 16, 4, 1, "c5f8114330"),
    (0x3C5D00, "rax", 0x00, 8, -1, 0, "488900"),
    (0x3C5D03, "rax", 0x08, 8, 3, 1, "48894008"),
    (0x3C5D07, "rbx", 0x30, 8, 3, 1, "48894330"),
    (0x3C5D0B, "rbx", 0x48, 16, 4, 1, "c5f8114348"),
    (0x3C5D1E, "rax", 0x00, 8, -1, 0, "488900"),
    (0x3C5D21, "rax", 0x08, 8, 3, 1, "48894008"),
    (0x3C5D25, "rbx", 0x48, 8, 3, 1, "48894348"),
    (0x3C5D29, "rbx", 0x58, 4, 2, 1, "c7435800000000"),
    (0x3C5D30, "rbx", 0x68, 16, 4, 1, "c5f8114368"),
    (0x3C5D41, "rax", 0x00, 8, -1, 0, "488900"),
    (0x3C5D44, "rax", 0x08, 8, 3, 1, "48894008"),
    (0x3C5D48, "rbx", 0x68, 8, 3, 1, "48894368"),
    (0x3C5D4C, "rbx", 0x78, 4, 2, 1, "c74378ffffffff"),
    (0x3C5D53, "rbx", 0x7C, 4, 2, 1, "c7437c00000000"),
    (0x3C5D5A, "rbx", 0x80, 1, 2, 4, "c6838000000000"),
    (0x3C5D61, "rbx", 0x81, 1, 2, 4, "c6838100000000"),
    (0x3C5D68, "rbx", 0x84, 8, 3, 4, "48c7838400000000000000"),
    (0x3C5D98, "rbp", -0x38, 16, 4, 1, "c5fa7f45c8"),
    (0x3C5D9D, "rbp", -0x40, 8, 3, 1, "48895dc0"),
    (0x3C5DA1, "rbp", -0x28, 8, 3, 1, "48c745d800000000"),
    (0x3C5DBD, "rbp", -0x58, 16, 4, 1, "c5fa7f45a8"),
    (0x3C5DC2, "rbp", -0x60, 8, 3, 1, "48895da0"),
    (0x3C5DC6, "rbp", -0x48, 8, 3, 1, "48c745b800000000"),
]

# (call site, target) in instruction order.
CALLS = [
    (0x3C5CDF, 0x0F180),
    (0x3C5CF2, 0x17070),
    (0x3C5D10, 0x17070),
    (0x3C5D35, 0x17070),
    (0x3C5D73, 0x0F180),
    (0x3C5D7F, 0x3B310),
    (0x3C5DA9, 0x116040),
    (0x3C5DCE, 0x116540),
    (0x3C5DE7, 0xF8F3A0),  # __stack_chk_fail, compiler boilerplate
]

# Offsets of the object that are never written (derived from the census + callees below).
HOLES = [(0x01, 0x04), (0x28, 0x30), (0x40, 0x48), (0x5C, 0x68), (0x82, 0x84), (0x8C, 0x90)]

PT_LOAD = 1
PT_SCE_RELRO = 0x61000010

OVERLAY: dict[int, int] = {}
PINNED: list[tuple[int, int]] = []
RECORD = False
SKIP_HASH = False


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def load_segments(elf: bytes) -> list[tuple[int, int, int]]:
    e_phoff = struct.unpack_from("<Q", elf, 0x20)[0]
    e_phentsize, e_phnum = struct.unpack_from("<HH", elf, 0x36)
    out = []
    for i in range(e_phnum):
        base = e_phoff + i * e_phentsize
        p_type = struct.unpack_from("<I", elf, base)[0]
        if p_type not in (PT_LOAD, PT_SCE_RELRO):
            continue
        p_offset, p_vaddr = struct.unpack_from("<QQ", elf, base + 8)
        p_filesz = struct.unpack_from("<Q", elf, base + 32)[0]
        out.append((p_vaddr, p_offset, p_filesz))
    return out


def read_rva(elf: bytes, segments, rva: int, size: int) -> bytes:
    for vaddr, offset, filesz in segments:
        if vaddr <= rva and rva + size <= vaddr + filesz:
            start = offset + (rva - vaddr)
            data = bytearray(elf[start:start + size])
            for i in range(size):
                if rva + i in OVERLAY:
                    data[i] = OVERLAY[rva + i]
            return bytes(data)
    raise SystemExit(f"FAIL rva {rva:#x} (+{size:#x}) is not file-backed")


def u8(elf, segments, rva):
    return read_rva(elf, segments, rva, 1)[0]


def u32(elf, segments, rva):
    return struct.unpack("<I", read_rva(elf, segments, rva, 4))[0]


def i32(elf, segments, rva):
    return struct.unpack("<i", read_rva(elf, segments, rva, 4))[0]


def u64(elf, segments, rva):
    return struct.unpack("<Q", read_rva(elf, segments, rva, 8))[0]


def rel32_target(elf, segments, site):
    return site + 5 + i32(elf, segments, site + 1)


def rip_target(elf, segments, site, disp_off, length):
    return site + length + i32(elf, segments, site + disp_off)


def pin(elf, segments, addr, pattern, what):
    size = len(pattern) // 2
    if RECORD:
        PINNED.append((addr, size))
    require(read_rva(elf, segments, addr, size) == bytes.fromhex(pattern), f"{what} @ {addr:#x}")


def check_body(elf, segments) -> None:
    body = read_rva(elf, segments, BODY_START, BODY_END - BODY_START)
    require(len(body) == 0x13D, f"body size {len(body):#x}")
    if not SKIP_HASH:
        require(hashlib.sha256(body).hexdigest() == BODY_SHA256, "body sha256")
    pin(elf, segments, BODY_START, "554889e54157415653", "prologue")
    pin(elf, segments, BODY_START + 9, "4883ec48", "sub rsp, 0x48")
    pin(elf, segments, 0x3C5DE6, "c3", "ret")
    pin(elf, segments, 0x3C5DE7, "e8b495bc00", "__stack_chk_fail tail")
    require(rel32_target(elf, segments, 0x3C5DE7) == 0xF8F3A0, "stack fail target")
    pin(elf, segments, 0x3C5DEC, "90909090", "FDE nop + 3 B padding")
    # The next function starts right after the padding: it is NOT part of this body. The E570 walk
    # ran into it (and into 0x3c7380) and reported `0x227b` / 1528 instructions.
    pin(elf, segments, 0x3C5DF0, "554889e5415741564155415453",
        "next function prologue")
    pin(elf, segments, 0x3C5DFD, "4881ecc80d0000", "next function: sub rsp, 0xdc8")
    pin(elf, segments, 0x3C7380, "554889e5", "callback B entry prologue")
    if not SKIP_HASH:
        require(hashlib.sha256(read_rva(elf, segments, BODY_START, 0x227B)).hexdigest() != BODY_SHA256,
                "0x227b must not be the body extent")


def check_stores(elf, segments) -> None:
    kinds = {"rbx": 0, "rax": 0, "rbp": 0}
    for _, base, *_ in STORES:
        kinds[base] += 1
    require(kinds == {"rbx": 14, "rax": 6, "rbp": 6}, f"store census {kinds}")
    require(len(STORES) == 26, f"store census size {len(STORES)}")

    previous = BODY_START
    for addr, _, _, _, _, _, raw in STORES:
        require(addr > previous, f"store sites out of order at {addr:#x}")
        previous = addr
        pin(elf, segments, addr, raw, f"store site {addr:#x}")

    for addr, base, disp, width, disp_off, disp_size, _ in STORES:
        if disp_off < 0:
            require(disp == 0, f"no-displacement store at {addr:#x} has disp {disp}")
            continue
        raw = read_rva(elf, segments, addr + disp_off, disp_size)
        found = int.from_bytes(raw, "little", signed=True)
        if RECORD:
            PINNED.append((addr + disp_off, disp_size))
        require(found == disp, f"{base} disp at {addr:#x}: {found:#x} != {disp:#x}")

    # Covered object bytes: the census + the two 0xf180 slots + the 0x3b310 handle.
    covered = [False] * 0xB8
    for _, base, disp, width, _, _, _ in STORES:
        if base == "rbx":
            require(0 <= disp and disp + width <= 0xB8, f"store past the object at {disp:#x}")
            for i in range(disp, disp + width):
                covered[i] = True
    for lo, hi in ((0x08, 0x28), (0x90, 0xB0), (0xB0, 0xB8)):
        for i in range(lo, hi):
            covered[i] = True
    holes, start = [], None
    for i, flag in enumerate(covered + [True]):
        if not flag and start is None:
            start = i
        if flag and start is not None:
            holes.append((start, i))
            start = None
    require(holes == HOLES, f"holes {[(hex(a), hex(b)) for a, b in holes]}")


def check_calls(elf, segments) -> None:
    for site, expect in CALLS:
        pin(elf, segments, site, "e8", f"call opcode at {site:#x}")
        target = rel32_target(elf, segments, site)
        require(target == expect, f"call {site:#x} -> {target:#x} (expected {expect:#x})")
    # The three allocation sizes, in order: the `mov edi, imm32` that feeds each 0x17070 call.
    pin(elf, segments, 0x3C5CE8, "bf30000000", "alloc #1: mov edi, 0x30")
    pin(elf, segments, 0x3C5CF7, "bf30000000", "alloc #2: mov edi, 0x30")
    pin(elf, segments, 0x3C5D15, "bf50000000", "alloc #3: mov edi, 0x50")
    sizes = [u32(elf, segments, a + 1) for a in (0x3C5CE8, 0x3C5CF7, 0x3C5D15)]
    require(sizes == [0x30, 0x30, 0x50], f"allocation sizes {sizes}")
    # Each node is self-linked through the returned rax: [rax] = rax, [rax+8] = rax (stores above);
    # the head pointers land at +0x30, +0x48, +0x68.
    # this / pool registers and the three argument computations.
    pin(elf, segments, 0x3C5CC4, "4889fb", "rbx = this")
    pin(elf, segments, 0x3C5CC7, "4989f6", "r14 = pool (rsi)")
    pin(elf, segments, 0x3C5CCA, "488d7b08", "rdi = this + 8 (slot 0)")
    pin(elf, segments, 0x3C5D3A, "488dbb90000000", "rdi = this + 0x90 (slot 1)")
    pin(elf, segments, 0x3C5D78, "488dbbb0000000", "rdi = this + 0xb0 (mutex)")
    pin(elf, segments, 0x3C5CE4, "c5f857c0", "xmm0 = 0")


def check_registrations(elf, segments) -> None:
    # 0x116040(pool, &{this, [0x15ff810], 0, 0}, 1)
    pin(elf, segments, 0x3C5D84, "c5fa7e05849a2301", "vmovq xmm0, [slot A]")
    pin(elf, segments, 0x3C5D8C, "488d75c0", "rsi = &callable A (rbp-0x40)")
    pin(elf, segments, 0x3C5D90, "ba01000000", "edx = 1")
    pin(elf, segments, 0x3C5D95, "4c89f7", "rdi = pool")
    slot_a = rip_target(elf, segments, 0x3C5D84, 4, 8)
    require(slot_a == 0x15FF810, f"slot A {slot_a:#x}")
    require(u64(elf, segments, slot_a) == 0x3C5DF0, "slot A does not hold 0x3c5df0")
    require(u32(elf, segments, 0x3C5D91) == 1, "0x116040 flag is not 1")
    # 0x116540(pool, &{this, [0x15ff590], 0, 0})
    pin(elf, segments, 0x3C5DAE, "c5fa7e05da972301", "vmovq xmm0, [slot B]")
    pin(elf, segments, 0x3C5DB6, "488d75a0", "rsi = &callable B (rbp-0x60)")
    pin(elf, segments, 0x3C5DBA, "4c89f7", "rdi = pool")
    slot_b = rip_target(elf, segments, 0x3C5DAE, 4, 8)
    require(slot_b == 0x15FF590, f"slot B {slot_b:#x}")
    require(u64(elf, segments, slot_b) == 0x3C7380, "slot B does not hold 0x3c7380")
    # Both callables sit between rbp-0x60 and the canary at rbp-0x20 (four words each).
    pin(elf, segments, 0x3C5CD1, "488945e0", "canary at rbp-0x20")
    pin(elf, segments, 0x3C5DD6, "483b45e0", "canary compare")
    require(0x40 - 0x20 == 0x20 and 0x60 - 0x40 == 0x20, "callable frame size")


def check_row_tuple(elf, segments) -> None:
    """`kOwnerMemberRows[18]`: {size 0xb8, ctor 0x3c5cb0, this+0x3b70, arg [0x2edf140], dtor+free}."""
    pin(elf, segments, 0x256F57, "bfb8000000", "row site: mov edi, 0xb8")
    require(rel32_target(elf, segments, 0x256F5C) == 0x17070, "row site -> allocator 0x17070")
    pin(elf, segments, 0x256F61, "498b7500", "row site: rsi = [r13]")
    pin(elf, segments, 0x256E54, "4c8d2de582c802", "r13 = &0x2edf140")
    require(rip_target(elf, segments, 0x256E54, 3, 7) == 0x2EDF140, "row arg cell 0x2edf140")
    require(rel32_target(elf, segments, 0x256F6B) == 0x3C5CB0, "row site -> ctor 0x3c5cb0")
    pin(elf, segments, 0x256F70, "498b9e703b0000", "old = [this + 0x3b70]")
    pin(elf, segments, 0x256F77, "4c39e3", "old == new ?")
    require(rel32_target(elf, segments, 0x256F84) == 0x3C74B0, "row teardown -> dtor 0x3c74b0")
    require(rel32_target(elf, segments, 0x256F8C) == 0x170B0, "row teardown -> free 0x170b0")
    pin(elf, segments, 0x256F91, "4d89a6703b0000", "store the new pointer into this+0x3b70")
    pin(elf, segments, 0x3C74B0, "554889e5", "dtor 0x3c74b0 prologue (contract only)")


def native_texts() -> dict[str, str]:
    return {"header": HEADER.read_text(), "source": SOURCE.read_text(), "test": TEST.read_text()}


HEADER_TOKENS = (
    "kOwnerCtor3C5CB0Rva = 0x3C5CB0",
    "kOwnerCtor3C5CB0Size = 0x13D",
    "kOwnerCtor3C5CB0ObjSize = 0xB8",
    "kOwnerCtor3C5CB0SlotOffset = 0x3B70",
    "kOwnerCtor3C5CB0ArgRva = 0x2EDF140",
    "kOwnerDtor3C74B0Rva = 0x3C74B0",
    "kOwnerRow18NodeASize = 0x30",
    "kOwnerRow18NodeBSize = 0x30",
    "kOwnerRow18NodeCSize = 0x50",
    "kOwnerRow18HeadAOffset = 0x30",
    "kOwnerRow18HeadBOffset = 0x48",
    "kOwnerRow18HeadCOffset = 0x68",
    "kOwnerRow18Dword58Offset = 0x58",
    "kOwnerRow18Dword78Offset = 0x78",
    "kOwnerRow18Dword7COffset = 0x7C",
    "kOwnerRow18Byte80Offset = 0x80",
    "kOwnerRow18Byte81Offset = 0x81",
    "kOwnerRow18Qword84Offset = 0x84",
    "kOwnerRow18Slot0Offset = 0x08",
    "kOwnerRow18Slot1Offset = 0x90",
    "kOwnerRow18MutexOffset = 0xB0",
    "kOwnerRow18Dword78Value = 0xFFFFFFFFu",
    "kOwnerRow18CallbackASlotRva = 0x15FF810",
    "kOwnerRow18CallbackARva = 0x3C5DF0",
    "kOwnerRow18CallbackBSlotRva = 0x15FF590",
    "kOwnerRow18CallbackBRva = 0x3C7380",
    "kOwnerRow18RegisterAFlag = 1",
    "0x227b",
    "0x13d / 72 insns / 9 call sites",
    "partially initialized",
    "+0x01..+0x03 hole",
)
SOURCE_TOKENS = (
    '"alloc_list_a"',
    '"alloc_list_b"',
    '"alloc_list_c"',
    '"mutex_init"',
    '"register_116040"',
    '"register_116540"',
    "init_slot_f180",
    "self_link",
    "kOwnerRow18Dword78Value",
    "kOwnerRow18CallbackARva",
    "kOwnerRow18CallbackBRva",
    "owner_row18_ctor_hook",
    "owner_row18_bind",
)
TEST_TOKENS = (
    "Prefix::kHead",
    "Prefix::kNodeA",
    "Prefix::kNodeB",
    "Prefix::kNodeC",
    "Prefix::kMutex",
    "Prefix::kFull",
    "guards_ok",
    "0x3C5DF0",
    "0x3C7380",
    "0xFFFFFFFFu",
    "owner_row18_ctor_hook",
    "init_owner_members",
    "std::is_same_v<decltype(OwnerMemberHooks::ctor)",
    "kOwnerMemberRows[17]",
)


def parse_header_constants(header: str) -> dict[str, int]:
    out = {}
    for name, value in re.findall(
            r"inline constexpr [A-Za-z0-9_:]+ (k\w+) = (0x[0-9A-Fa-f]+|\d+)u?;", header):
        out[name] = int(value, 0)
    return out


def check_native_sources(texts: dict[str, str], elf=None, segments=None) -> None:
    header, source, test = texts["header"], texts["source"], texts["test"]
    for token in HEADER_TOKENS:
        require(token in header, f"header token {token!r}")
    for token in SOURCE_TOKENS:
        require(token in source, f"source token {token!r}")
    for token in TEST_TOKENS:
        require(token in test, f"test token {token!r}")
    require("memset(o, 0" not in source, "source must not zero-fill the object")
    require("calloc" not in source and "new " not in source, "source must not allocate by itself")
    require(len(re.findall(r"\bput(?:_bytes|_pointer)?[<(]", source)) >= 15,
            "source no longer writes the layout store by store")

    if elf is None:
        return
    # ELF-derived values must equal the native constants.
    constants = parse_header_constants(header)
    derived = elf_derived(elf, segments)
    for name, value in derived.items():
        require(name in constants, f"native constant {name} missing")
        require(constants[name] == value, f"native {name} = {constants[name]:#x}, ELF says {value:#x}")


def elf_derived(elf, segments) -> dict[str, int]:
    return {
        "kOwnerRow18NodeASize": u32(elf, segments, 0x3C5CE9),
        "kOwnerRow18NodeBSize": u32(elf, segments, 0x3C5CF8),
        "kOwnerRow18NodeCSize": u32(elf, segments, 0x3C5D16),
        "kOwnerRow18HeadAOffset": STORES[5][2],
        "kOwnerRow18HeadBOffset": STORES[9][2],
        "kOwnerRow18Dword58Offset": STORES[10][2],
        "kOwnerRow18HeadCOffset": STORES[14][2],
        "kOwnerRow18Dword78Offset": STORES[15][2],
        "kOwnerRow18Dword7COffset": STORES[16][2],
        "kOwnerRow18Byte80Offset": STORES[17][2],
        "kOwnerRow18Byte81Offset": STORES[18][2],
        "kOwnerRow18Qword84Offset": STORES[19][2],
        "kOwnerRow18Slot0Offset": u8(elf, segments, 0x3C5CCD),
        "kOwnerRow18Slot1Offset": u32(elf, segments, 0x3C5D3D),
        "kOwnerRow18MutexOffset": u32(elf, segments, 0x3C5D7B),
        "kOwnerRow18Dword78Value": u32(elf, segments, 0x3C5D4F),
        "kOwnerRow18CallbackARva": u64(elf, segments, 0x15FF810),
        "kOwnerRow18CallbackBRva": u64(elf, segments, 0x15FF590),
        "kOwnerRow18RegisterAFlag": u32(elf, segments, 0x3C5D91),
        "kOwnerRow18CallbackASlotRva": rip_target(elf, segments, 0x3C5D84, 4, 8),
        "kOwnerRow18CallbackBSlotRva": rip_target(elf, segments, 0x3C5DAE, 4, 8),
        "kOwnerCtor3C5CB0Size": BODY_END - BODY_START,
        "kOwnerCtor3C5CB0SlotOffset": i32(elf, segments, 0x256F73),
        "kOwnerCtor3C5CB0ArgRva": rip_target(elf, segments, 0x256E54, 3, 7),
        "kOwnerCtor3C5CB0ObjSize": u32(elf, segments, 0x256F58),
        "kOwnerDtor3C74B0Rva": rel32_target(elf, segments, 0x256F84),
    }


ELF_TIED = {
    "kOwnerRow18NodeASize", "kOwnerRow18NodeBSize", "kOwnerRow18NodeCSize",
    "kOwnerRow18HeadAOffset", "kOwnerRow18HeadBOffset", "kOwnerRow18Dword58Offset",
    "kOwnerRow18HeadCOffset", "kOwnerRow18Dword78Offset", "kOwnerRow18Dword7COffset",
    "kOwnerRow18Byte80Offset", "kOwnerRow18Byte81Offset", "kOwnerRow18Qword84Offset",
    "kOwnerRow18Slot0Offset", "kOwnerRow18Slot1Offset", "kOwnerRow18MutexOffset",
    "kOwnerRow18Dword78Value", "kOwnerRow18CallbackARva", "kOwnerRow18CallbackBRva",
    "kOwnerRow18RegisterAFlag", "kOwnerRow18CallbackASlotRva", "kOwnerRow18CallbackBSlotRva",
    "kOwnerCtor3C5CB0Size", "kOwnerCtor3C5CB0SlotOffset", "kOwnerCtor3C5CB0ArgRva",
    "kOwnerCtor3C5CB0ObjSize", "kOwnerDtor3C74B0Rva",
}


def run_elf_checks(elf, segments, texts) -> None:
    check_body(elf, segments)
    check_stores(elf, segments)
    check_calls(elf, segments)
    check_registrations(elf, segments)
    check_row_tuple(elf, segments)
    check_native_sources(texts, elf, segments)


def expect_failure(label: str, fn) -> None:
    try:
        fn()
    except SystemExit as error:
        require(str(error).startswith("FAIL"), f"mutation {label}: unexpected exit {error!r}")
        return
    raise SystemExit(f"FAIL mutation survived: {label}")


def mutation_self_test(elf, segments, texts) -> tuple[int, int]:
    global OVERLAY, SKIP_HASH, RECORD
    # ELF pins: perturb the first and last byte of every recorded pin, hash disabled.
    SKIP_HASH = True
    pins = sorted(set(PINNED))
    survivors = 0
    ran = 0
    for addr, size in pins:
        for byte_addr in sorted({addr, addr + size - 1}):
            OVERLAY = {}  # read the pristine byte (an earlier overlay may cover the same address)
            OVERLAY = {byte_addr: u8(elf, segments, byte_addr) ^ 0x01}
            ran += 1
            try:
                run_elf_checks(elf, segments, texts)
            except SystemExit:
                continue
            survivors += 1
            print(f"mutation survived at {byte_addr:#x} (pin {addr:#x}+{size})", file=sys.stderr)
    OVERLAY = {}
    SKIP_HASH = False
    require(survivors == 0, f"{survivors} ELF mutations survived")

    # The whole-body hash alone catches a flip in an unpinned body byte.
    OVERLAY = {}
    OVERLAY = {BODY_START + 0x20: u8(elf, segments, BODY_START + 0x20) ^ 0x80}
    expect_failure("body hash", lambda: check_body(elf, segments))
    OVERLAY = {}

    # Native tokens: remove each required token in turn.
    native_ran = 0
    for key, tokens in (("header", HEADER_TOKENS), ("source", SOURCE_TOKENS), ("test", TEST_TOKENS)):
        for token in tokens:
            mutated = dict(texts)
            mutated[key] = texts[key].replace(token, "\0MUTATED\0")
            expect_failure(f"{key} token {token!r}", lambda m=mutated: check_native_sources(m, elf, segments))
            native_ran += 1
    # Native constants: change each ELF-tied constant's value in the header text.
    for name, value in parse_header_constants(texts["header"]).items():
        if name not in ELF_TIED:
            continue
        pattern = re.compile(rf"({name} = )(0x[0-9A-Fa-f]+|\d+)(u?;)")
        mutated = dict(texts)
        mutated["header"] = pattern.sub(lambda m: f"{m.group(1)}{value ^ 0x4:#x}{m.group(3)}",
                                        texts["header"], count=1)
        expect_failure(f"constant {name}", lambda m=mutated: check_native_sources(m, elf, segments))
        native_ran += 1
    return ran, native_ran


def main() -> int:
    global RECORD
    mutate = "--no-mutate" not in sys.argv[1:]
    require(ELF_PATH.is_file(), f"{ELF_PATH} is absent (private game image required)")
    elf = ELF_PATH.read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    segments = load_segments(elf)
    texts = native_texts()

    RECORD = True
    run_elf_checks(elf, segments, texts)
    RECORD = False

    mutations = ""
    if mutate:
        elf_runs, native_runs = mutation_self_test(elf, segments, texts)
        mutations = f"; mutation self-test {elf_runs} ELF + {native_runs} native mutants all rejected"

    print(
        f"OK check_fowner_ctor_3c5cb0: body 0x13d sha256 {BODY_SHA256[:12]}..., "
        f"{len(STORES)} store sites, 3 allocations (0x30,0x30,0x50), 9 calls / 6 targets, "
        f"callables {{this,0x3c5df0}} {{this,0x3c7380}} via slots 0x15ff810/0x15ff590, "
        f"row tuple 0xb8/0x256f57/this+0x3b70/[0x2edf140]; guest bytes NOT executed{mutations}"
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
