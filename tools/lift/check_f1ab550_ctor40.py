#!/usr/bin/env python3
"""ELF-tie prologue-tail 0x40 ctor 0x1ab550 + base 0x1b9250.

Private: needs local/re/eboot.elf. Pins body through jmp, base init fields,
vtables, cell 0x2ee6778, PLT stubs. Does NOT claim PLT bodies.
"""

from __future__ import annotations

import hashlib
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
CTOR = 0x1AB550
CTOR_SIZE = 49
CTOR_SHA256 = "a81f2e5d5b48443397ce62c2564e8088f43e2c277a7cc4268fc6577d816d0a4c"
BODY_SIZE = 44
BODY_SHA256 = "92367169b00d82043e185b2eefd82541a3d1b58b4cdc835f96bc14a0a72cf613"
BASE = 0x1B9250
BASE_SIZE = 69
BASE_SHA256 = "c414e6b0bf75e7c2d0949712b041599f9c17a33c8d51eee73b3f12aedeb43443"
DERIVED_VT = 0x1598AE0
BASE_VT = 0x1598C30
CELL = 0x2EE6778
NATIVE = ROOT / "native/core/ctor_40_1ab550.hpp"
PROL = ROOT / "native/core/prologue_255180.hpp"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def call_target(site: int, img: d.Image) -> int:
    b = img.read(site, 5)
    require(b[0] == 0xE8, f"missing call at {site:#x}")
    rel = struct.unpack("<i", b[1:5])[0]
    return site + 5 + rel


def lea_abs(site: int, img: d.Image) -> int:
    b = img.read(site, 7)
    require(b[0] == 0x48 and b[1] == 0x8D, f"missing lea at {site:#x}")
    rel = struct.unpack("<i", b[3:7])[0]
    return site + 7 + rel


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    require(hashlib.sha256(img.read(CTOR, CTOR_SIZE)).hexdigest() == CTOR_SHA256, "ctor sha")
    require(hashlib.sha256(img.read(CTOR, BODY_SIZE)).hexdigest() == BODY_SHA256, "body sha")
    require(hashlib.sha256(img.read(BASE, BASE_SIZE)).hexdigest() == BASE_SHA256, "base sha")
    require(img.read(BASE + BASE_SIZE - 1, 1) == b"\xc3", "base ret")

    require(call_target(0x1AB559, img) == BASE, "call base 0x1b9250")
    require(lea_abs(0x1AB55E, img) + 0x10 == DERIVED_VT, "derived vt")
    require(img.read(0x1AB565, 4) == bytes.fromhex("4883c010"), "add rax,0x10")
    require(call_target(0x1AB56C, img) == 0xF901A0, "call PLT f901a0")
    require(img.read(0xF901A0, 2) == bytes.fromhex("ff25"), "f901a0 PLT")
    require(img.read(0x1AB577, 5)[0] == 0xE9, "jmp f901c0")
    jmp_rel = struct.unpack("<i", img.read(0x1AB577, 5)[1:5])[0]
    require(0x1AB577 + 5 + jmp_rel == 0xF901C0, "jmp target f901c0")
    require(img.read(0xF901C0, 2) == bytes.fromhex("ff25"), "f901c0 PLT")

    require(lea_abs(0x1B9250, img) + 0x10 == BASE_VT, "base vt")
    require(lea_abs(0x1B9257, img) == CELL, "cell 0x2ee6778")
    require(img.read(0x1B9269, 4) == bytes.fromhex("c6470800"), "mov byte [rdi+8],0")
    require(img.read(0x1B9271, 4) == bytes.fromhex("c6470a01"), "mov byte [rdi+0xa],1")
    require(img.read(0x1B9291, 3) == bytes.fromhex("488939"), "mov [rcx],rdi")

    require(call_target(0x2552B1, img) == CTOR, "prologue calls 0x1ab550")
    require(img.read(0x2552A1, 5) == bytes.fromhex("bf40000000"), "mov edi,0x40")

    nh = NATIVE.read_text()
    require("init_ctor_40_1ab550" in nh, "API")
    require("init_base_1b9250" in nh, "base API")
    require("NOT modelled" in nh or "not modelled" in nh.lower(), "omission")
    require("init_ctor_40_1ab550" in PROL.read_text() or "ctor_40_1ab550" in PROL.read_text(),
            "prologue wires")

    print(
        f"OK check_f1ab550_ctor40: ctor={CTOR_SHA256[:12]}… body={BODY_SHA256[:12]}…; "
        f"base={BASE_SHA256[:12]}…; vt {DERIVED_VT:#x}/{BASE_VT:#x}; cell {CELL:#x}; "
        f"PLT bodies NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
