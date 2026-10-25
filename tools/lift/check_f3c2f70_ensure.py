#!/usr/bin/env python3
"""ELF-tie ensure 0x3c2f70 (called from ctor 0x3be050 with n=0x100).

Private: needs local/re/eboot.elf. Pins body, elem stride 0xa8, alloc, install
stores, overflow abort. Does NOT claim migrate callees 0x1da30 / 0x170c0 / 0xf590.
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
FN = 0x3C2F70
FN_SIZE = 313
FN_SHA256 = "7f9ed966b0bacfec226e5b5728efbf0f7dc271a7c2282e08d75fd196108112e1"
NATIVE = ROOT / "native/core/ensure_capacity_3c2f70.hpp"
CTOR = ROOT / "native/core/ctor_98a90_3be050.hpp"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def call_target(site: int, img: d.Image) -> int:
    b = img.read(site, 5)
    require(b[0] == 0xE8, f"missing call at {site:#x}")
    rel = struct.unpack("<i", b[1:5])[0]
    return site + 5 + rel


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    body = img.read(FN, FN_SIZE)
    require(hashlib.sha256(body).hexdigest() == FN_SHA256, "fn sha")
    require(body[-1] == 0xC3, "ends ret")

    # imul rdi, rbx, 0xa8 (imm32 form)
    require(img.read(0x3C2FA1, 7) == bytes.fromhex("4869fba8000000"), "imul rdi,rbx,0xa8")
    require(call_target(0x3C2FA8, img) == 0x17070, "alloc")
    require(call_target(0x3C2FDE, img) == 0x1DA30, "elem move")
    require(call_target(0x3C3058, img) == 0xF590, "elem dtor")
    require(call_target(0x3C3070, img) == 0x170B0, "delete_")

    # install stores
    require(img.read(0x3C308E, 4) == bytes.fromhex("49894618"), "mov [r14+0x18],rax")
    require(img.read(0x3C3092, 4) == bytes.fromhex("49895610"), "mov [r14+0x10],rdx")
    require(img.read(0x3C3096, 4) == bytes.fromhex("49894e08"), "mov [r14+8],rcx")

    # overflow → abort PLT + ud2
    require(img.read(0x3C30A9, 1) == bytes.fromhex("e8"), "overflow call")
    require(img.read(0x3C30AE, 2) == bytes.fromhex("0f0b"), "ud2")

    # ctor callsite
    require(call_target(0x3BE0F3, img) == FN, "ctor calls ensure")
    require(img.read(0x3BE0EE, 5) == bytes.fromhex("be00010000"), "mov esi,0x100")
    require(img.read(0x3BE0EA, 4) == bytes.fromhex("498d7e20"), "lea rdi,[r14+0x20]")

    nh = NATIVE.read_text()
    require("ensure_capacity_3c2f70" in nh, "API")
    require("kEnsure3c2f70ElemBytes" in nh, "elem size")
    require("NOT modelled" in nh or "not modelled" in nh.lower(), "omission")
    require("ensure_capacity_3c2f70" in CTOR.read_text(), "ctor wires")

    print(
        f"OK check_f3c2f70_ensure: body={FN_SHA256[:12]}… ({FN_SIZE} B); "
        f"0xa8 stride + empty-path install pinned; migrate callees NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
