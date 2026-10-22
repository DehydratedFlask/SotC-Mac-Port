#!/usr/bin/env python3
"""ELF-tie app vslot 0x533100 miss path (r_fps) after show_areaInfo miss.

Private: needs local/re/eboot.elf. Pins miss fragment, r_fps, callees, join.
Does NOT claim 0x21620 body / vcall bodies / join past 0x533305 / hit path.
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
MISS = 0x533207
MISS_SIZE = 254
MISS_SHA256 = "1b38d9b295f223adf3125c3443261031839bf0c0f64a02effa96d4fd16040149"
JOIN = 0x533305
NATIVE = ROOT / "native/core/vslot_40_533100_miss.hpp"
BASE = ROOT / "native/core/base_initialize_255130.hpp"
POST = ROOT / "native/core/vslot_40_533100_post.hpp"


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

    require(hashlib.sha256(img.read(MISS, MISS_SIZE)).hexdigest() == MISS_SHA256, "miss sha")
    # post jne lands on hit=join
    jne = img.read(0x533201, 6)
    require(jne[:2] == bytes.fromhex("0f85"), "jne")
    hit = 0x533207 + struct.unpack("<i", jne[2:6])[0]
    require(hit == JOIN, "jne → join/hit")

    require(cstr(img, lea_abs(0x53320E, img)) == b"r_fps", "r_fps")
    require(call_target(0x53327D, img) == 0x21620, "21620")
    require(img.read(0x533290, 3) == bytes.fromhex("ff5018"), "vcall +0x18")
    require(img.read(0x5332A5, 3) == bytes.fromhex("ff5030"), "vcall +0x30")
    require(call_target(0x5332DA, img) == 0xBA40, "ba40")
    require(call_target(0x5332F4, img) == 0xF590, "f590 #1")
    require(call_target(0x533300, img) == 0xF590, "f590 #2")
    # ba40 cell lea
    require(lea_abs(0x5332D0, img) == 0x2EFE560, "ba40 cell 0x2efe560")

    nh = NATIVE.read_text()
    require("vslot_40_533100_miss" in nh, "API")
    require("r_fps" in nh, "name")
    require("NOT modelled" in nh, "omission")
    bh = BASE.read_text()
    require("vslot_40_miss" in bh or "vslot_40_533100_miss" in bh, "base wires")
    ph = POST.read_text()
    require("miss" in ph.lower(), "post mentions miss")

    print(
        f"OK check_f533100_vslot40_miss: miss={MISS_SHA256[:12]}…; "
        f"r_fps; 21620/ba40/f590 pinned; join={JOIN:#x}; "
        f"21620 body/hit/join+ NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
