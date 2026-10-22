#!/usr/bin/env python3
"""ELF-tie heavy ctor 0x3be050 header + 0x80 buffer-grow remainder.

Private: needs local/re/eboot.elf. Pins ctor span, header, and grow sequence
0x3be0f8..0x3be1cb. Does NOT claim bodies of 0x3be930 / 0x323d0 / 0x3c2f70 /
0xf590 or list-walk from 0x3be1ce.
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
CTOR = 0x3BE050
CTOR_SIZE = 627
CTOR_SHA256 = "f186db1c213f27794ad55b543ca805d0eb60e164ed975dee84de41f42b51e354"
HEADER_SIZE = 102
HEADER_SHA256 = "74b2e5e4d57326262078f5ae77855f70c6f89eb814faec3f6f172e9b156c747c"
GROW = 0x3BE0F8
GROW_END = 0x3BE1CB  # last store of vec pointers
GROW_SHA256 = "93e3cb8028761f5d0a60d7dae07b4921185b70e4aca47ccd049c3d3b1a9bdb98"
# Note: GROW_SHA256 is remainder 0x3be0f8..0x3be2c2 (459 B); also pin grow span length.
NATIVE = ROOT / "native/core/ctor_98a90_3be050.hpp"
SLICE = ROOT / "native/core/appmain_init_ctor_98a90_slice.hpp"


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

    body = img.read(CTOR, CTOR_SIZE)
    require(hashlib.sha256(body).hexdigest() == CTOR_SHA256, "ctor sha")
    require(body[-1] == 0xC3, "ctor ends ret")
    require(img.read(0x3BE2D0, 2) == bytes.fromhex("5548"), "next fn at 0x3be2d0")

    hdr = img.read(CTOR, HEADER_SIZE)
    require(hashlib.sha256(hdr).hexdigest() == HEADER_SHA256, "header sha")

    rem = img.read(GROW, CTOR + CTOR_SIZE - GROW)
    require(hashlib.sha256(rem).hexdigest() == GROW_SHA256, "remainder sha")
    require(GROW_END - GROW + 1 == 212, "grow span size")

    require(img.read(0x3BE068, 5) == bytes.fromhex("bf48000000"), "mov edi, 0x48")
    require(call_target(0x3BE073, img) == 0x17070, "alloc nested")
    require(call_target(0x3BE0B6, img) == 0x3BE930, "call 0x3be930")
    require(call_target(0x3BE0C2, img) == 0x323D0, "call 0x323d0")
    require(call_target(0x3BE0F3, img) == 0x3C2F70, "call 0x3c2f70 ensure")

    # Buffer grow
    require(img.read(0x3BE112, 5) == bytes.fromhex("bf80000000"), "mov edi, 0x80")
    require(call_target(0x3BE117, img) == 0x17070, "alloc 0x80")
    require(call_target(0x3BE193, img) == 0xF590, "elem dtor 0xf590")
    require(call_target(0x3BE1A8, img) == 0x170B0, "delete_ old buf")
    # stores begin/end/cap (OBS: 48 89 41 58 / 4c 89 79 50 / 4c 89 71 48)
    require(img.read(0x3BE1BF, 4) == bytes.fromhex("48894158"), "mov [rcx+0x58], rax")
    require(img.read(0x3BE1C3, 4) == bytes.fromhex("4c897950"), "mov [rcx+0x50], r15")
    require(img.read(0x3BE1C7, 4) == bytes.fromhex("4c897148"), "mov [rcx+0x48], r14")

    # List walk follows (not claimed)
    require(img.read(0x3BE1CE, 7)[0:3] == bytes.fromhex("488d0d"), "lea list global")
    require(call_target(0x3BE29B, img) == 0x3C0840, "list insert 0x3c0840")

    nh = NATIVE.read_text()
    require("init_98a90_3be050" in nh, "API")
    require("kCtor98a90BufBytes" in nh, "buf const")
    require("list_walk_from_3be1ce" in nh, "list walk hook")
    require("NOT modelled" in nh or "not modelled" in nh.lower(), "omission")
    require("run_appmain_init_ctor_98a90_slice" in SLICE.read_text(), "slice API")

    print(
        f"OK check_f3be050_ctor: body={CTOR_SHA256[:12]}…; header={HEADER_SHA256[:12]}…; "
        f"grow 0x80→+0x48/+0x50/+0x58 pinned; list_walk/0x3be930/0x323d0/0x3c2f70/0xf590 "
        f"NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
