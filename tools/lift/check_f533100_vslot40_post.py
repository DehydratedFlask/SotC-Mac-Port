#!/usr/bin/env python3
"""ELF-tie app vslot 0x533100 post-header fragment + lookup 0x1fc80.

Private: needs local/re/eboot.elf. Pins fragment, show_areaInfo, 0x1fc80.
Does NOT claim 0x525080 / 0x2569d0 / miss/hit bodies.
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
POST = 0x5331CD
POST_SIZE = 58
POST_SHA256 = "2332bbd7a0e4805e7b7674a5e414216aadaef3561d80ad0dad6863fe189feff4"
LOOKUP = 0x1FC80
LOOKUP_SIZE = 143
LOOKUP_SHA256 = "0e213a3d4118fdbc4ecce2cfe2b45ede126330585500311c06aa2f746090cff3"
NATIVE_POST = ROOT / "native/core/vslot_40_533100_post.hpp"
NATIVE_LOOKUP = ROOT / "native/core/lookup_1fc80.hpp"
BASE = ROOT / "native/core/base_initialize_255130.hpp"


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
    require(b[0] in (0x48, 0x4C) and b[1] == 0x8D, f"missing lea at {site:#x}")
    rel = struct.unpack("<i", b[3:7])[0]
    return site + 7 + rel


def cstr(img: d.Image, addr: int) -> bytes:
    out = bytearray()
    i = 0
    while True:
        b = img.read(addr + i, 1)[0]
        if b == 0:
            return bytes(out)
        out.append(b)
        i += 1
        if i > 80:
            raise SystemExit(f"FAIL unterminated at {addr:#x}")


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    require(
        hashlib.sha256(img.read(POST, POST_SIZE)).hexdigest() == POST_SHA256, "post sha"
    )
    require(lea_abs(0x5331CD, img) == 0x2EFC9E0, "f08 cell")
    require(call_target(0x5331D7, img) == 0x525080, "525080")
    require(call_target(0x5331E9, img) == 0x2569D0, "2569d0")
    require(cstr(img, lea_abs(0x5331F2, img)) == b"show_areaInfo", "name")
    require(call_target(0x5331F9, img) == LOOKUP, "1fc80")
    require(img.read(0x5331FE, 3) == bytes.fromhex("4885c0"), "test rax,rax")
    require(img.read(0x533201, 2) == bytes.fromhex("0f85"), "jne near hit")

    require(
        hashlib.sha256(img.read(LOOKUP, LOOKUP_SIZE)).hexdigest() == LOOKUP_SHA256,
        "1fc80 sha",
    )
    require(img.read(LOOKUP + LOOKUP_SIZE - 1, 1) == b"\xc3", "1fc80 ret")
    require(call_target(0x1FCDE, img) == 0xF8F500, "comparator PLT")
    require(img.read(0xF8F500, 2) == bytes.fromhex("ff25"), "0xf8f500 is PLT")
    require(img.read(0x1FCEA, 7) == bytes.fromhex("4881c398000000"), "add rbx,0x98")

    ph = NATIVE_POST.read_text()
    require("vslot_40_533100_post" in ph, "post API")
    require("show_areaInfo" in ph, "name pin")
    require("NOT modelled" in ph, "omission")
    lh = NATIVE_LOOKUP.read_text()
    require("lookup_1fc80" in lh, "lookup API")
    bh = BASE.read_text()
    require("vslot_40_post" in bh or "vslot_40_533100_post" in bh, "base wires")

    print(
        f"OK check_f533100_vslot40_post: post={POST_SHA256[:12]}…; "
        f"1fc80={LOOKUP_SHA256[:12]}…; show_areaInfo; "
        f"525080/2569d0/miss/hit NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
