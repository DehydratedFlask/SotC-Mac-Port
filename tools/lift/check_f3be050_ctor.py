#!/usr/bin/env python3
"""ELF-tie heavy ctor 0x3be050 header + 0x80 grow + null-head list cells.

Private: needs local/re/eboot.elf. Pins ctor span, header, grow, and list-head
lea/store/je at 0x3be1ce. Does NOT claim non-null list body (0xf8f500 PLT /
0x3c0840) or bodies of 0x3be930 / 0x323d0 / 0x3c2f70 / 0xf590.
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
GROW_SHA256 = "93e3cb8028761f5d0a60d7dae07b4921185b70e4aca47ccd049c3d3b1a9bdb98"
LIST = 0x3BE1CE
LIST_HEAD_END = 0x3BE1E5  # je target site
SELF_CELL = 0x2EF63F8
HEAD_CELL = 0x2EF6400
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

    body = img.read(CTOR, CTOR_SIZE)
    require(hashlib.sha256(body).hexdigest() == CTOR_SHA256, "ctor sha")
    require(body[-1] == 0xC3, "ctor ends ret")
    require(img.read(0x3BE2D0, 2) == bytes.fromhex("5548"), "next fn at 0x3be2d0")

    hdr = img.read(CTOR, HEADER_SIZE)
    require(hashlib.sha256(hdr).hexdigest() == HEADER_SHA256, "header sha")
    rem = img.read(GROW, CTOR + CTOR_SIZE - GROW)
    require(hashlib.sha256(rem).hexdigest() == GROW_SHA256, "remainder sha")

    require(img.read(0x3BE068, 5) == bytes.fromhex("bf48000000"), "mov edi, 0x48")
    require(call_target(0x3BE073, img) == 0x17070, "alloc nested")
    require(call_target(0x3BE0B6, img) == 0x3BE930, "call 0x3be930")
    require(call_target(0x3BE0C2, img) == 0x323D0, "call 0x323d0")
    require(call_target(0x3BE0F3, img) == 0x3C2F70, "call 0x3c2f70 ensure")

    require(img.read(0x3BE112, 5) == bytes.fromhex("bf80000000"), "mov edi, 0x80")
    require(call_target(0x3BE117, img) == 0x17070, "alloc 0x80")
    require(img.read(0x3BE1BF, 4) == bytes.fromhex("48894158"), "mov [rcx+0x58], rax")
    require(img.read(0x3BE1C3, 4) == bytes.fromhex("4c897950"), "mov [rcx+0x50], r15")
    require(img.read(0x3BE1C7, 4) == bytes.fromhex("4c897148"), "mov [rcx+0x48], r14")

    # List-walk head (null-head measured path)
    require(lea_abs(LIST, img) == HEAD_CELL, "lea head cell 0x2ef6400")
    require(lea_abs(0x3BE1D5, img) == SELF_CELL, "lea self cell 0x2ef63f8")
    require(img.read(0x3BE1DC, 3) == bytes.fromhex("4c8b21"), "mov r12,[rcx]")
    require(img.read(0x3BE1DF, 3) == bytes.fromhex("4c8930"), "mov [rax],r14")
    require(img.read(0x3BE1E2, 3) == bytes.fromhex("4d85e4"), "test r12,r12")
    require(img.read(0x3BE1E5, 6) == bytes.fromhex("0f84c9000000"), "je 0x3be2b4")
    require(LIST_HEAD_END - LIST + 1 == 24, "list head size")
    # Non-null body callees (not claimed as lifted)
    require(img.read(0xF8F500, 2) == bytes.fromhex("ff25"), "0xf8f500 is PLT")
    require(call_target(0x3BE29B, img) == 0x3C0840, "list insert 0x3c0840")

    nh = NATIVE.read_text()
    require("init_98a90_3be050" in nh, "API")
    require("kCtor98a90SelfCellRva" in nh, "self cell")
    require("kCtor98a90ListHeadCellRva" in nh, "head cell")
    require("list_walk_nonnull_body" in nh, "nonnull hook")
    require("store_tsc_323d0" in nh, "tsc leaf wired")
    require("NOT modelled" in nh or "not modelled" in nh.lower(), "omission")
    require("run_appmain_init_ctor_98a90_slice" in SLICE.read_text(), "slice API")

    print(
        f"OK check_f3be050_ctor: body={CTOR_SHA256[:12]}…; header+grow+list cells pinned; "
        f"0x323d0 lifted via store_tsc_323d0; "
        f"nonnull body/0xf8f500/0x3c0840/0x3be930/0x3c2f70/0xf590 NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
