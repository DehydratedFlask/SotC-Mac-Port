#!/usr/bin/env python3
"""ELF-tie base-Initialize prologue 0x255180 early fragment + leaf 0x335a0.

Private: needs local/re/eboot.elf. Pins 448 B prologue digest and the early
sequence through 0x35770; pins 0x335a0 body. Does NOT claim 0x264e50 / 0x335d0 /
0x3df40 bodies or prologue tail after 0x255264.
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
PROLOGUE = 0x255180
PROLOGUE_SIZE = 448
PROLOGUE_SHA256 = "9bcd73ea53f39436edb42927a3e69bb6b816adf07c25c8c95e30ced1ab2cde85"
LEAF_335A0 = 0x335A0
LEAF_335A0_SIZE = 34
LEAF_335A0_SHA256 = "c73b05ca7ff2dccfb327617b05db6ddbfae4f0aadfbec656bfea2a85b3e14b9a"
NATIVE = ROOT / "native/core/prologue_255180.hpp"
LEAF_H = ROOT / "native/core/init_flag_object_335a0.hpp"
SLICE = ROOT / "native/core/appmain_init_prologue_slice.hpp"


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

    body = img.read(PROLOGUE, PROLOGUE_SIZE)
    require(hashlib.sha256(body).hexdigest() == PROLOGUE_SHA256, "prologue sha")

    require(img.read(0x25519B, 5) == bytes.fromhex("bf300a0000"), "mov edi, 0xa30")
    require(call_target(0x2551A8, img) == 0x17070, "alloc a30")
    require(call_target(0x2551B3, img) == 0x264E50, "ctor 0x264e50")
    lea = img.read(0x2551B8, 7)
    require(lea[0:3] == bytes.fromhex("4c8d2d"), "lea r13 cell")
    disp = struct.unpack("<i", lea[3:7])[0]
    require(0x2551B8 + 7 + disp == 0x2DFD4B8, "cell 0x2dfd4b8")

    require(call_target(0x2551D7, img) == 0x11E70, "0x11e70")
    require(call_target(0x2551DC, img) == 0x3DF40, "0x3df40")
    require(img.read(0x2551E9, 5) == bytes.fromhex("bf40000000"), "mov edi, 0x40")
    require(call_target(0x255211, img) == 0x17070, "alloc 0x40")
    require(call_target(0x25521C, img) == LEAF_335A0, "0x335a0")
    lea2 = img.read(0x255221, 7)
    require(lea2[0:3] == bytes.fromhex("4c8d3d"), "lea r15 flag cell")
    disp2 = struct.unpack("<i", lea2[3:7])[0]
    require(0x255221 + 7 + disp2 == 0x2EA4718, "cell 0x2ea4718")
    require(call_target(0x255245, img) == 0x335D0, "0x335d0")
    require(call_target(0x255252, img) == 0x357F0, "0x357f0 sil=1 path")
    require(call_target(0x25525F, img) == 0x35770, "0x35770")

    # Tail edge present (deepened separately in check_f255180_prologue_tail.py)
    require(img.read(0x255264, 5) == bytes.fromhex("bf908a0900"), "mov edi, 0x98a90 tail")

    leaf = img.read(LEAF_335A0, LEAF_335A0_SIZE)
    require(hashlib.sha256(leaf).hexdigest() == LEAF_335A0_SHA256, "0x335a0 sha")
    require(leaf[-1] == 0xC3, "0x335a0 ret")

    hdr = NATIVE.read_text()
    require("run_prologue_255180_early" in hdr, "API")
    require("NOT modelled" in hdr or "not modelled" in hdr.lower(), "omission")
    require(LEAF_H.is_file() and "init_flag_object_335a0" in LEAF_H.read_text(), "335a0 API")
    require("run_appmain_init_prologue_slice" in SLICE.read_text(), "slice API")

    print(
        f"OK check_f255180_prologue: body={PROLOGUE_SHA256[:12]}…; early fragment through "
        f"0x35770 pinned; 0x335a0 lifted; heavy early callees NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
