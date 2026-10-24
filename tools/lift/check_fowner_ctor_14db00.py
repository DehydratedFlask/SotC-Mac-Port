#!/usr/bin/env python3
"""ELF-tie the lifted row-3 owner constructor `0x14db00` (the ResourcePool ctor, `kOwnerMemberRows[3]`).

Private: needs `local/re/eboot.elf`. Read-only and offline: instructions are matched by raw bytes and
byte ranges are hashed; **no guest byte is executed**. No emulator, interpreter, JIT or guest runtime.

Layers:
  1. Body extent/hash: `0x14db00..0x14dc83` (FDE extent), `0x183` B, `ret` at `0x14dc7c`, a noreturn
     `__stack_chk_fail` call, 13 `nop`s of padding, and the *next function* at `0x14dc90` (which is
     also the relocated `resource_dump` handler). This pins the correction to the E570 row table
     (`0x426` / 276 insns / 20 calls / 4 dispatches: the walk fell through the noreturn call).
  2. The ordered store census (26 sites), each pinned by raw instruction bytes and its decoded
     displacement, and the exact set of object bytes it covers (the holes).
  3. Calls: nine direct sites in order, the two indirect sites resolved to their fixed targets
     (`[0x1594dc0 + 0x20]` -> `0x20d60`; `[[this] + 0x50]` with `[this] = 0x1598628` -> `0x14e910`),
     allocation sizes, the `resource_dump` literals and the `0x20af0` argument registers.
  4. The four lifted callees (`0xf2f0`, `0x20af0`, `0x1ab270`, `0x14e910`) and the `0x20d60` guard:
     whole-function hashes plus their store sites.
  5. The row tuple `{0x138, 0x14db00, 0x2ee2cf8, [0x2ee2d00], virtual deleting dtor}`.
  6. The native header/source/test must carry the constants and they must equal the ELF values.
  7. Mutation self-test (default on; `--no-mutate` to skip): every recorded pin is perturbed in an
     in-memory overlay (first and last byte) with the whole-body hash disabled, every native token
     is removed in turn and every ELF-tied constant is changed; the checker must fail each time.
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
BODY_START, BODY_END = 0x14DB00, 0x14DC83
BODY_SHA256 = "22e020bf87b5fd68100b7957ed762e95e6605ba8a8a0ab10a9705febe836b20a"

HEADER = ROOT / "native/core/owner_ctor_14db00.hpp"
SOURCE = ROOT / "native/core/owner_ctor_14db00.cpp"
TEST = ROOT / "native/tests/test_owner_ctor_14db00.cpp"

# Whole-function hashes of the lifted callees: (start, size, sha256).
CALLEES = {
    "string_view_f2f0": (0x0F2F0, 0x28, "0173e3a3f11a8aa020b87731d2eef5026621d6f1fa0967581b0dab3df646973b"),
    "command_ctor_20af0": (0x20AF0, 0x5A, "e9dbf86d6bb1496a9b34cbf1968ba5cc6909e06d1e5a902cb936704efedf0b45"),
    "register_guard_20d60": (0x20D60, 0x1A, "f4463b3b78853849ab4eec851d22d17ecde2d207efd57918e0186412bd84b403"),
    "helper_ctor_1ab270": (0x1AB270, 0x20, "ab1156613b025812201f4df903dd50f759992e99360c9c64521d1c2487403b5e"),
    "set_helper_14e910": (0x14E910, 0x8B, "34bce37b9d614487c9e23e088077ce04218d93a24a6985a7165c0e3daf859d10"),
}

# Store sites of 0x14db00 in instruction order (the canary store is not a census entry):
#   (addr, base, disp, width, disp_off, disp_size, raw bytes)
STORES = [
    (0x14DB3B, "rbx", 0x00, 8, -1, 0, "488913"),
    (0x14DB3E, "rbx", 0x08, 8, 3, 1, "48897308"),
    (0x14DB42, "rbx", 0x18, 8, 3, 1, "48894b18"),
    (0x14DB46, "rbx", 0x20, 8, 3, 1, "48894b20"),
    (0x14DB4A, "rbx", 0x28, 8, 3, 1, "48c7432800000000"),
    (0x14DB52, "rbx", 0x30, 16, 4, 1, "c5f8114330"),
    (0x14DB57, "rbx", 0x30, 1, 2, 1, "c6433000"),
    (0x14DB5B, "rbx", 0x48, 16, 4, 1, "c5f8114348"),
    (0x14DB60, "rbx", 0x60, 16, 4, 1, "c5f8114360"),
    (0x14DB6E, "rax", 0x00, 8, -1, 0, "488900"),
    (0x14DB71, "rax", 0x08, 8, 3, 1, "48894008"),
    (0x14DB75, "rax", 0x10, 8, 3, 1, "48894010"),
    (0x14DB79, "rax", 0x18, 1, 2, 1, "c6401801"),
    (0x14DB7D, "rax", 0x19, 1, 2, 1, "c6401901"),
    (0x14DB81, "rbx", 0x60, 8, 3, 1, "48894360"),
    (0x14DB9A, "rbx", 0x78, 8, 3, 1, "48c7437800000000"),
    (0x14DBCB, "rbp", -0x70, 8, 3, 1, "48894590"),
    (0x14DBCF, "rbp", -0x68, 8, 3, 1, "48c745983e000000"),
    (0x14DBD7, "rbp", -0x60, 16, 4, 1, "c5f81145a0"),
    (0x14DBF7, "rbx", 0x80, 8, 3, 4, "48898b80000000"),
    (0x14DBFE, "rbx", 0xE8, 16, 4, 4, "c5fa7f83e8000000"),
    (0x14DC0A, "rbx", 0xE0, 8, 3, 4, "48899be0000000"),
    (0x14DC11, "rbx", 0xF8, 32, 4, 4, "c5fc1183f8000000"),
    (0x14DC30, "rbx", 0x120, 1, 2, 4, "c6832001000000"),
    (0x14DC3C, "rbx", 0x128, 16, 4, 4, "c5f8118328010000"),
]

# Direct calls in instruction order.
CALLS = [
    (0x14DB65, 0x17070),
    (0x14DB85, 0x3B310),
    (0x14DBA9, 0xF2F0),
    (0x14DBDC, 0x20AF0),
    (0x14DC1F, 0xF590),
    (0x14DC27, 0xF590),
    (0x14DC4B, 0x17070),
    (0x14DC56, 0x1AB270),
    (0x14DC7D, 0xF8F3A0),  # __stack_chk_fail, compiler boilerplate
]

# Object bytes the body (plus the 0x3b310 handle and the 0x20af0 / 0x20d60 callees) never writes.
HOLES = [(0x10, 0x18), (0x40, 0x48), (0x58, 0x60), (0x88, 0x98), (0x99, 0x9C),
         (0x118, 0x120), (0x121, 0x128)]

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
    require(len(body) == 0x183, f"body size {len(body):#x}")
    if not SKIP_HASH:
        require(hashlib.sha256(body).hexdigest() == BODY_SHA256, "body sha256")
    pin(elf, segments, BODY_START, "554889e5415741564155415453", "prologue")
    pin(elf, segments, BODY_START + 0x0D, "4883ec48", "sub rsp, 0x48")
    pin(elf, segments, 0x14DC7C, "c3", "ret")
    pin(elf, segments, 0x14DC7D, "e81e17e400", "__stack_chk_fail tail")
    require(rel32_target(elf, segments, 0x14DC7D) == 0xF8F3A0, "stack fail target")
    pin(elf, segments, 0x14DC82, "90" * 14, "FDE nop + 13 B padding")
    # The next function starts right after the padding; it is NOT part of this body, and it is the
    # relocated `resource_dump` handler (slot 0x15feb58).
    pin(elf, segments, 0x14DC90, "554889e5415741564155415453",
        "next function prologue")
    pin(elf, segments, 0x14DC9D, "4881ec88000000", "next function: sub rsp, 0x88")
    if not SKIP_HASH:
        require(hashlib.sha256(read_rva(elf, segments, BODY_START, 0x426)).hexdigest() != BODY_SHA256,
                "0x426 must not be the body extent")


def check_stores(elf, segments) -> None:
    kinds = {"rbx": 0, "rax": 0, "rbp": 0}
    for _, base, *_ in STORES:
        kinds[base] += 1
    require(len(STORES) == 25, f"store census size {len(STORES)}")
    require(kinds == {"rbx": 17, "rax": 5, "rbp": 3}, f"store census {kinds}")

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

    # Covered object bytes: the census + the 0x3b310 handle (+0x70) + the 0x20af0 command base
    # (+0x80, 0x60 B minus its own holes) + the 0x20d60 registered byte.
    covered = [False] * 0x138
    for _, base, disp, width, _, _, _ in STORES:
        if base == "rbx":
            require(0 <= disp and disp + width <= 0x138, f"store past the object at {disp:#x}")
            for i in range(disp, disp + width):
                covered[i] = True
    for i in range(0x70, 0x78):
        covered[i] = True
    for lo, hi in ((0x98, 0x99), (0x9C, 0xA0), (0xA0, 0xE0)):  # 0x20af0 + 0x20d60 byte
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
    # +0x50 is zeroed by the 16 B block at +0x48 and written only by the (callee) 0x14e910:
    # no census store lands on it, which is what makes the old-control release dead.
    for addr, base, disp, width, _, _, _ in STORES:
        if base == "rbx" and addr > 0x14DB5B:
            require(not (disp <= 0x50 < disp + width), f"store {addr:#x} touches +0x50 after the zero")
    # `[rbx]` (the vptr) is written exactly once and read only by the devirtualized dispatch.
    require([a for a, b, d, w, *_ in STORES if b == "rbx" and d == 0] == [0x14DB3B],
            "the vptr is written more than once")


def check_calls(elf, segments) -> None:
    for site, expect in CALLS:
        pin(elf, segments, site, "e8", f"call opcode at {site:#x}")
        target = rel32_target(elf, segments, site)
        require(target == expect, f"call {site:#x} -> {target:#x} (expected {expect:#x})")
    # Allocation sizes (edi immediates that feed the two 0x17070 calls).
    pin(elf, segments, 0x14DB22, "bf60000000", "alloc #1: mov edi, 0x60")
    pin(elf, segments, 0x14DC37, "bf28000000", "alloc #2: mov edi, 0x28")
    require([u32(elf, segments, 0x14DB23), u32(elf, segments, 0x14DC38)] == [0x60, 0x28],
            "allocation sizes")
    pin(elf, segments, 0x14DB1F, "4889fb", "rbx = this")
    pin(elf, segments, 0x14DB3E, "48897308", "[this+8] = rsi (engine)")
    pin(elf, segments, 0x14DB6A, "488d7b70", "rdi = this + 0x70 (mutex)")
    pin(elf, segments, 0x14DB8A, "4c8d75b0", "r14 = &holder A (rbp-0x50)")
    pin(elf, segments, 0x14DB8E, "4989df", "r15 = this")
    pin(elf, segments, 0x14DBA5, "4983ef80", "r15 = this + 0x80 (command)")
    pin(elf, segments, 0x14DBAE, "4c8d6590", "r12 = &holder B (rbp-0x70)")

    # The vptr store: `lea rdx,[0x1598618]; add rdx,0x10; mov [rbx],rdx`.
    pin(elf, segments, 0x14DB18, "488d15f9aa4401", "lea rdx, [0x1598618]")
    pin(elf, segments, 0x14DB33, "4883c210", "add rdx, 0x10")
    pool_vtable = rip_target(elf, segments, 0x14DB18, 3, 7) + 0x10
    require(pool_vtable == 0x1598628, f"pool vptr {pool_vtable:#x}")

    # Indirect call 1: `lea rax,[0x1594dc0]` ... `call [rax + 0x20]` with rdi = this + 0x80.
    pin(elf, segments, 0x14DBE1, "488d05d8714401", "lea rax, [0x1594dc0]")
    pin(elf, segments, 0x14DBF0, "4c89ff", "rdi = command")
    pin(elf, segments, 0x14DBF3, "488d4810", "rcx = rax + 0x10 (vptr 0x1594dd0)")
    pin(elf, segments, 0x14DC19, "ff5020", "call [rax + 0x20]")
    base = rip_target(elf, segments, 0x14DBE1, 3, 7)
    require(base == 0x1594DC0, f"command vtable base {base:#x}")
    require(u64(elf, segments, base + 0x20) == 0x20D60, "vtable 0x1594dd0 entry 2 is not 0x20d60")

    # Indirect call 2: `r15 = [[this] + 0x50]`; `[this]` is the vptr stored at 0x14db3b.
    pin(elf, segments, 0x14DC44, "488b03", "rax = [this] (the constructor's own vptr)")
    pin(elf, segments, 0x14DC47, "4c8b7850", "r15 = [rax + 0x50]")
    pin(elf, segments, 0x14DC5B, "4889df", "rdi = this")
    pin(elf, segments, 0x14DC5E, "4c89f6", "rsi = helper")
    pin(elf, segments, 0x14DC61, "41ffd7", "call r15")
    require(u64(elf, segments, pool_vtable + 0x50) == 0x14E910, "pool vtable entry 10 is not 0x14e910")

    # Literals and the 0x20af0 / f2f0 argument shapes.
    pin(elf, segments, 0x14DB91, "488d15a8c6e500", "rdx = &\"resource_dump\"")
    pin(elf, segments, 0x14DB98, "31f6", "esi = 0")
    pin(elf, segments, 0x14DBA2, "4c89f7", "rdi = &holder A")
    name = rip_target(elf, segments, 0x14DB91, 3, 7)
    require(name == 0xFAA240, f"name literal {name:#x}")
    require(read_rva(elf, segments, name, 14) == b"resource_dump\x00", "name literal text")
    pin(elf, segments, 0x14DBB2, "488d0595c6e500", "rax = &description")
    description = rip_target(elf, segments, 0x14DBB2, 3, 7)
    text = b"Dump all resources with the specified FourCC. Params: <fourcc>"
    require(description == 0xFAA24E, f"description literal {description:#x}")
    require(read_rva(elf, segments, description, len(text) + 1) == text + b"\x00", "description text")
    require(len(text) == 0x3E and u32(elf, segments, 0x14DBCF + 4) == 0x3E, "description length")
    pin(elf, segments, 0x14DBBD, "b902000000", "ecx = 2 (command flags)")
    pin(elf, segments, 0x14DBC2, "4c89ff", "rdi = command")
    pin(elf, segments, 0x14DBC5, "4c89f6", "rsi = &holder A")
    pin(elf, segments, 0x14DBC8, "4c89e2", "rdx = &holder B")
    pin(elf, segments, 0x14DBB9, "c5f857c0", "xmm0 = 0 (holder B aux/ctl)")

    # Releases: holder B, then holder A.
    pin(elf, segments, 0x14DC1C, "4c89e7", "rdi = &holder B")
    pin(elf, segments, 0x14DC24, "4c89f7", "rdi = &holder A")

    # The relocated handler slot written to +0xe8.
    pin(elf, segments, 0x14DBE8, "c5fa7e05680f4b01", "vmovq xmm0, [0x15feb58]")
    slot = rip_target(elf, segments, 0x14DBE8, 4, 8)
    require(slot == 0x15FEB58, f"handler slot {slot:#x}")
    require(u64(elf, segments, slot) == 0x14DC90, "handler slot does not hold 0x14dc90")


def check_callees(elf, segments) -> None:
    for name, (start, size, digest) in CALLEES.items():
        body = read_rva(elf, segments, start, size)
        if not SKIP_HASH:
            require(hashlib.sha256(body).hexdigest() == digest, f"callee {name} sha256")
    # 0xf2f0: [rdi] = rdx; strlen(rdx) via PLT 0xf8f3b0; [rdi+8] = len; [rdi+0x10..0x20) = 0.
    pin(elf, segments, 0xF2F9, "4889d7", "rdi = cstr")
    pin(elf, segments, 0xF2FC, "488913", "[rbx] = rdx")
    require(rel32_target(elf, segments, 0xF2FF) == 0xF8F3B0, "f2f0 strlen PLT")
    pin(elf, segments, 0xF308, "48894308", "[rbx+8] = len")
    pin(elf, segments, 0xF30C, "c5f8114310", "[rbx+0x10..0x20) = 0")
    # 0x20af0: vptr 0x1594d80 + 0x10, byte +0x18 = 0, dword +0x1c = ecx, two 0x20 B copies + lock inc.
    pin(elf, segments, 0x20AF0, "488d05", "lea rax, [0x1594d80]")
    require(rip_target(elf, segments, 0x20AF0, 3, 7) + 0x10 == 0x1594D90, "command base vptr")
    pin(elf, segments, 0x20AFB, "488907", "[rdi] = vptr")
    pin(elf, segments, 0x20AFE, "c6471800", "byte [rdi+0x18] = 0")
    pin(elf, segments, 0x20B02, "894f1c", "dword [rdi+0x1c] = ecx")
    pin(elf, segments, 0x20B09, "c5f8114720", "name[0..16) -> +0x20")
    pin(elf, segments, 0x20B12, "48894730", "name.aux -> +0x30")
    pin(elf, segments, 0x20B1A, "48894738", "name.ctl -> +0x38")
    pin(elf, segments, 0x20B23, "f0", "lock prefix (name)")
    pin(elf, segments, 0x20B2B, "c5f8114740", "desc[0..16) -> +0x40")
    pin(elf, segments, 0x20B34, "48894750", "desc.aux -> +0x50")
    pin(elf, segments, 0x20B3C, "48894758", "desc.ctl -> +0x58")
    pin(elf, segments, 0x20B45, "f0", "lock prefix (description)")
    # 0x20d60: if (byte [rdi+0x18]) return; byte [rdi+0x18] = 1; rsi = *(0x2dfcbc8); jmp 0x20d80.
    pin(elf, segments, 0x20D60, "807f1800", "cmp byte [rdi+0x18], 0")
    pin(elf, segments, 0x20D6E, "c6471801", "byte [rdi+0x18] = 1")
    require(rip_target(elf, segments, 0x20D67, 3, 7) == 0x2DFCBC8, "console cell 0x2dfcbc8")
    # 0x1ab270: [rdi] = vptr 0x1598aa0 + 0x10, [rdi+0x10..0x20) = 0, [rdi+0x20] = 0.
    pin(elf, segments, 0x1AB270, "488d05", "lea rax, [0x1598aa0]")
    require(rip_target(elf, segments, 0x1AB270, 3, 7) + 0x10 == 0x1598AB0, "helper vptr")
    pin(elf, segments, 0x1AB27F, "488907", "[rdi] = vptr")
    pin(elf, segments, 0x1AB282, "c5f8114710", "[rdi+0x10..0x20) = 0")
    pin(elf, segments, 0x1AB287, "48c7472000000000", "[rdi+0x20] = 0")
    # 0x14e910: control block 0x20 B via 0x17090; strong = weak = 1; [this+0x48] = helper;
    # old = [this+0x50]; [this+0x50] = control.
    pin(elf, segments, 0x14E91A, "bf20000000", "mov edi, 0x20")
    require(rel32_target(elf, segments, 0x14E922) == 0x17090, "14e910 allocator is 0x17090")
    require(rip_target(elf, segments, 0x14E927, 3, 7) + 0x10 == 0x15986F0, "control vptr")
    pin(elf, segments, 0x14E92E, "c7400801000000", "[ctrl+8] = 1")
    pin(elf, segments, 0x14E935, "c7400c01000000", "[ctrl+0xc] = 1")
    pin(elf, segments, 0x14E940, "488908", "[ctrl] = vptr")
    pin(elf, segments, 0x14E943, "4c897010", "[ctrl+0x10] = helper")
    pin(elf, segments, 0x14E947, "4c897348", "[this+0x48] = helper")
    pin(elf, segments, 0x14E94B, "4c8b7350", "old = [this+0x50]")
    pin(elf, segments, 0x14E94F, "48894350", "[this+0x50] = control")
    pin(elf, segments, 0x14E953, "4d85f6", "test old, old")


def check_row_tuple(elf, segments) -> None:
    """`kOwnerMemberRows[3]`: {size 0x138, ctor 0x14db00, 0x2ee2cf8, arg [0x2ee2d00], virtual dtor}."""
    pin(elf, segments, 0x256B6B, "bf38010000", "row site: mov edi, 0x138")
    require(rel32_target(elf, segments, 0x256B70) == 0x17070, "row site -> allocator 0x17070")
    pin(elf, segments, 0x256B75, "498b3424", "row site: rsi = [r12]")
    pin(elf, segments, 0x256A25, "4c8d25d4c2c802", "r12 = &0x2ee2d00")
    require(rip_target(elf, segments, 0x256A25, 3, 7) == 0x2EE2D00, "row arg cell 0x2ee2d00")
    require(rel32_target(elf, segments, 0x256B7F) == 0x14DB00, "row site -> ctor 0x14db00")
    pin(elf, segments, 0x256B84, "4c8d3d6dc1c802", "r15 = &0x2ee2cf8")
    require(rip_target(elf, segments, 0x256B84, 3, 7) == 0x2EE2CF8, "row slot 0x2ee2cf8")
    pin(elf, segments, 0x256B8B, "498b3f", "old = [slot]")
    pin(elf, segments, 0x256B8E, "4839df", "old == new ?")
    pin(elf, segments, 0x256B98, "488b07", "rax = [old] (vptr)")
    pin(elf, segments, 0x256B9B, "ff5008", "call [vptr + 8] (virtual deleting dtor, no 0x170b0)")
    pin(elf, segments, 0x256B9E, "49891f", "store the new pointer into the slot")
    # Entry 1 of the pool vtable (offset 8) is the deleting wrapper 0x14e170 = 0x14de00 + 0x170b0.
    require(u64(elf, segments, 0x1598628 + 8) == 0x14E170, "pool vtable entry 1 is not 0x14e170")
    pin(elf, segments, 0x14E170, "554889e55350", "deleting wrapper prologue (contract only)")
    require(rel32_target(elf, segments, 0x14E179) == 0x14DE00, "wrapper -> complete dtor 0x14de00")
    pin(elf, segments, 0x14E187, "e9", "wrapper tail jmp")
    require(0x14E187 + 5 + i32(elf, segments, 0x14E188) == 0x170B0, "wrapper tail -> free 0x170b0")
    pin(elf, segments, 0x14DE00, "554889e5", "dtor 0x14de00 prologue (contract only)")


def native_texts() -> dict[str, str]:
    return {"header": HEADER.read_text(), "source": SOURCE.read_text(), "test": TEST.read_text()}


HEADER_TOKENS = (
    "kOwnerCtor14DB00Rva = 0x14DB00",
    "kOwnerCtor14DB00Size = 0x183",
    "kOwnerCtor14DB00ObjSize = 0x138",
    "kOwnerCtor14DB00SlotRva = 0x2EE2CF8",
    "kOwnerCtor14DB00ArgRva = 0x2EE2D00",
    "kOwnerDtor14DE00Rva = 0x14DE00",
    "kOwnerDtor14E170Rva = 0x14E170",
    "kOwnerRow3PoolVptrRva = 0x1598628",
    "kOwnerRow3CommandBaseVptrRva = 0x1594D90",
    "kOwnerRow3CommandVptrRva = 0x1594DD0",
    "kOwnerRow3ControlVptrRva = 0x15986F0",
    "kOwnerRow3HelperVptrRva = 0x1598AB0",
    "kOwnerRow3CommandRegisterRva = 0x20D60",
    "kOwnerRow3SetHelperRva = 0x14E910",
    "kOwnerRow3AnchorSize = 0x60",
    "kOwnerRow3HelperSize = 0x28",
    "kOwnerRow3ControlSize = 0x20",
    "kOwnerRow3CommandNameRva = 0xFAA240",
    "kOwnerRow3CommandDescriptionRva = 0xFAA24E",
    "kOwnerRow3CommandNameLength = 13",
    "kOwnerRow3CommandDescriptionLength = 0x3E",
    "kOwnerRow3CommandFlags = 2",
    "kOwnerRow3HandlerSlotRva = 0x15FEB58",
    "kOwnerRow3HandlerRva = 0x14DC90",
    "kOwnerRow3EngineOffset = 0x08",
    "kOwnerRow3MapHeaderOffset = 0x18",
    "kOwnerRow3MapLeftOffset = 0x20",
    "kOwnerRow3MapRootOffset = 0x28",
    "kOwnerRow3Zero30Offset = 0x30",
    "kOwnerRow3HelperPtrOffset = 0x48",
    "kOwnerRow3HelperCtrlOffset = 0x50",
    "kOwnerRow3AnchorOffset = 0x60",
    "kOwnerRow3MutexOffset = 0x70",
    "kOwnerRow3Qword78Offset = 0x78",
    "kOwnerRow3CommandOffset = 0x80",
    "kOwnerRow3BackPointerOffset = 0xE0",
    "kOwnerRow3HandlerOffset = 0xE8",
    "kOwnerRow3Zero0F8Offset = 0xF8",
    "kOwnerRow3Byte120Offset = 0x120",
    "kOwnerRow3Zero128Offset = 0x128",
    "kOwnerCommandRegisteredOffset = 0x18",
    "kOwnerCommandFlagsOffset = 0x1C",
    "kOwnerCommandNameOffset = 0x20",
    "kOwnerCommandDescriptionOffset = 0x40",
    "kOwnerCommandBaseSize = 0x60",
    "0x426",
    "94 insns",
    "11 call sites",
    "0x14e170",
    "devirtualized",
    "dead on this path",
)
SOURCE_TOKENS = (
    '"vptr"',
    '"alloc_anchor"',
    '"mutex_init"',
    '"console_register"',
    '"alloc_helper"',
    '"alloc_control"',
    "owner_row3_string_view_f2f0",
    "owner_row3_command_ctor_20af0",
    "owner_row3_helper_ctor_1ab270",
    "release_shared_str_f590",
    "kOwnerRow3HandlerRva",
    "kOwnerRow3CommandFlags",
    "kOwnerRow3CommandDescriptionLength",
    "owner_row3_ctor_hook",
    "owner_row3_bind",
    "fetch_add",
)
TEST_TOKENS = (
    "Prefix::kBlocks",
    "Prefix::kAnchor",
    "Prefix::kMutex",
    "Prefix::kCommand",
    "Prefix::kTail",
    "Prefix::kFull",
    "guards_ok",
    "name_counter == 6 && desc_counter == 10",
    "kOwnerRow3CommandFlags",
    "owner_row3_ctor_hook",
    "init_owner_members",
    "std::is_same_v<decltype(OwnerMemberHooks::ctor)",
    "kOwnerMemberRows[2]",
    "\"vptr\"",
)


def parse_header_constants(header: str) -> dict[str, int]:
    out = {}
    for name, value in re.findall(
            r"inline constexpr [A-Za-z0-9_:]+ (k\w+) = (0x[0-9A-Fa-f]+|\d+)u?;", header):
        out[name] = int(value, 0)
    return out


def elf_derived(elf, segments) -> dict[str, int]:
    return {
        "kOwnerCtor14DB00Size": BODY_END - BODY_START,
        "kOwnerCtor14DB00ObjSize": u32(elf, segments, 0x256B6C),
        "kOwnerCtor14DB00SlotRva": rip_target(elf, segments, 0x256B84, 3, 7),
        "kOwnerCtor14DB00ArgRva": rip_target(elf, segments, 0x256A25, 3, 7),
        "kOwnerRow3PoolVptrRva": rip_target(elf, segments, 0x14DB18, 3, 7) + 0x10,
        "kOwnerRow3CommandBaseVptrRva": rip_target(elf, segments, 0x20AF0, 3, 7) + 0x10,
        "kOwnerRow3CommandVptrRva": rip_target(elf, segments, 0x14DBE1, 3, 7) + 0x10,
        "kOwnerRow3ControlVptrRva": rip_target(elf, segments, 0x14E927, 3, 7) + 0x10,
        "kOwnerRow3HelperVptrRva": rip_target(elf, segments, 0x1AB270, 3, 7) + 0x10,
        "kOwnerRow3CommandRegisterRva": u64(elf, segments, 0x1594DC0 + 0x20),
        "kOwnerRow3SetHelperRva": u64(elf, segments, 0x1598628 + 0x50),
        "kOwnerRow3AnchorSize": u32(elf, segments, 0x14DB23),
        "kOwnerRow3HelperSize": u32(elf, segments, 0x14DC38),
        "kOwnerRow3ControlSize": u32(elf, segments, 0x14E91B),
        "kOwnerRow3CommandNameRva": rip_target(elf, segments, 0x14DB91, 3, 7),
        "kOwnerRow3CommandDescriptionRva": rip_target(elf, segments, 0x14DBB2, 3, 7),
        "kOwnerRow3CommandDescriptionLength": u32(elf, segments, 0x14DBD3),
        "kOwnerRow3CommandFlags": u32(elf, segments, 0x14DBBE),
        "kOwnerRow3HandlerSlotRva": rip_target(elf, segments, 0x14DBE8, 4, 8),
        "kOwnerRow3HandlerRva": u64(elf, segments, rip_target(elf, segments, 0x14DBE8, 4, 8)),
        "kOwnerRow3EngineOffset": STORES[1][2],
        "kOwnerRow3MapHeaderOffset": STORES[2][2],
        "kOwnerRow3MapLeftOffset": STORES[3][2],
        "kOwnerRow3MapRootOffset": STORES[4][2],
        "kOwnerRow3Zero30Offset": STORES[5][2],
        "kOwnerRow3HelperPtrOffset": STORES[7][2],
        "kOwnerRow3AnchorOffset": STORES[14][2],
        "kOwnerRow3MutexOffset": u8(elf, segments, 0x14DB6D),
        "kOwnerRow3Qword78Offset": STORES[15][2],
        "kOwnerRow3CommandOffset": STORES[19][2],
        "kOwnerRow3HandlerOffset": STORES[20][2],
        "kOwnerRow3BackPointerOffset": STORES[21][2],
        "kOwnerRow3Zero0F8Offset": STORES[22][2],
        "kOwnerRow3Byte120Offset": STORES[23][2],
        "kOwnerRow3Zero128Offset": STORES[24][2],
        "kOwnerRow3HelperCtrlOffset": u8(elf, segments, 0x14E952),
        "kOwnerCommandRegisteredOffset": u8(elf, segments, 0x20B00),
        "kOwnerCommandFlagsOffset": u8(elf, segments, 0x20B04),
        "kOwnerCommandNameOffset": u8(elf, segments, 0x20B0D),
        "kOwnerCommandDescriptionOffset": u8(elf, segments, 0x20B2F),
        "kOwnerDtor14E170Rva": u64(elf, segments, 0x1598628 + 8),
        "kOwnerDtor14DE00Rva": rel32_target(elf, segments, 0x14E179),
    }


ELF_TIED = {
    "kOwnerCtor14DB00Size", "kOwnerCtor14DB00ObjSize", "kOwnerCtor14DB00SlotRva",
    "kOwnerCtor14DB00ArgRva", "kOwnerRow3PoolVptrRva", "kOwnerRow3CommandBaseVptrRva",
    "kOwnerRow3CommandVptrRva", "kOwnerRow3ControlVptrRva", "kOwnerRow3HelperVptrRva",
    "kOwnerRow3CommandRegisterRva", "kOwnerRow3SetHelperRva", "kOwnerRow3AnchorSize",
    "kOwnerRow3HelperSize", "kOwnerRow3ControlSize", "kOwnerRow3CommandNameRva",
    "kOwnerRow3CommandDescriptionRva", "kOwnerRow3CommandDescriptionLength",
    "kOwnerRow3CommandFlags", "kOwnerRow3HandlerSlotRva", "kOwnerRow3HandlerRva",
    "kOwnerRow3EngineOffset", "kOwnerRow3MapHeaderOffset", "kOwnerRow3MapLeftOffset",
    "kOwnerRow3MapRootOffset", "kOwnerRow3Zero30Offset", "kOwnerRow3HelperPtrOffset",
    "kOwnerRow3AnchorOffset", "kOwnerRow3MutexOffset", "kOwnerRow3Qword78Offset",
    "kOwnerRow3CommandOffset", "kOwnerRow3HandlerOffset", "kOwnerRow3BackPointerOffset",
    "kOwnerRow3Zero0F8Offset", "kOwnerRow3Byte120Offset", "kOwnerRow3Zero128Offset",
    "kOwnerRow3HelperCtrlOffset", "kOwnerCommandRegisteredOffset", "kOwnerCommandFlagsOffset",
    "kOwnerCommandNameOffset", "kOwnerCommandDescriptionOffset", "kOwnerDtor14DE00Rva",
    "kOwnerDtor14E170Rva",
}


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
    require(len(re.findall(r"\bput(?:_bytes|_pointer)?[<(]", source)) >= 25,
            "source no longer writes the layout store by store")
    if elf is None:
        return
    constants = parse_header_constants(header)
    for name, value in elf_derived(elf, segments).items():
        require(name in constants, f"native constant {name} missing")
        require(constants[name] == value,
                f"native {name} = {constants[name]:#x}, ELF says {value:#x}")


def run_elf_checks(elf, segments, texts) -> None:
    check_body(elf, segments)
    check_stores(elf, segments)
    check_calls(elf, segments)
    check_callees(elf, segments)
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
    global OVERLAY, SKIP_HASH
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

    # The whole-body hash alone catches a flip in an unpinned body byte; the callee hashes catch a
    # flip of the first and last byte of each lifted callee (those bytes have no other pin).
    OVERLAY = {}
    OVERLAY = {BODY_START + 0x20: u8(elf, segments, BODY_START + 0x20) ^ 0x80}
    expect_failure("body hash", lambda: check_body(elf, segments))
    OVERLAY = {}
    for name, (start, size, _) in CALLEES.items():
        for byte_addr in (start, start + size - 1):
            OVERLAY = {}
            OVERLAY = {byte_addr: u8(elf, segments, byte_addr) ^ 0x80}
            expect_failure(f"callee {name} hash @ {byte_addr:#x}",
                           lambda: check_callees(elf, segments))
            ran += 1
    OVERLAY = {}

    native_ran = 0
    for key, tokens in (("header", HEADER_TOKENS), ("source", SOURCE_TOKENS), ("test", TEST_TOKENS)):
        for token in tokens:
            mutated = dict(texts)
            mutated[key] = texts[key].replace(token, "\0MUTATED\0")
            expect_failure(f"{key} token {token!r}", lambda m=mutated: check_native_sources(m, elf, segments))
            native_ran += 1
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
        f"OK check_fowner_ctor_14db00: body 0x183 sha256 {BODY_SHA256[:12]}..., "
        f"{len(STORES)} store sites, allocations 0x60/0x28 (+0x20 in 0x14e910), 9 direct + 2 resolved "
        f"indirect calls (-> 0x20d60, 0x14e910), 5 callee hashes, resource_dump literals, "
        f"row tuple 0x138/0x256b6b/0x2ee2cf8/[0x2ee2d00]; guest bytes NOT executed{mutations}"
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
