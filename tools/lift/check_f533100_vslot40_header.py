#!/usr/bin/env python3
"""ELF-tie app vslot 0x533100 bounded header (install only).

Private: needs local/re/eboot.elf. Pins header install, size, cell, callees.
Does NOT claim full 3956 B body or 0x493230 / inlined destroy bodies.
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
VSLOT = 0x533100
HEADER_SIZE = 205
HEADER_SHA256 = "a612bbcdc08c38834715c563af609f4d95c81df02487af2f82dcbfdf0f5f664d"
FULL_SIZE = 3956
FULL_SHA256 = "4f89702543d996543b04713fcce6e2161a7c56bed306a15cb78446778be202e8"
NATIVE = ROOT / "native/core/vslot_40_533100_header.hpp"
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


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    require(
        hashlib.sha256(img.read(VSLOT, FULL_SIZE)).hexdigest() == FULL_SHA256, "full sha"
    )
    require(img.read(VSLOT + FULL_SIZE - 1, 1) == b"\xc3", "full ret")
    require(
        hashlib.sha256(img.read(VSLOT, HEADER_SIZE)).hexdigest() == HEADER_SHA256,
        "header sha",
    )
    require(img.read(0x533122, 5) == bytes.fromhex("bfc0010000"), "mov edi,0x1c0")
    require(call_target(0x533131, img) == 0x17070, "alloc")
    require(call_target(0x53313C, img) == 0x493230, "ctor")
    require(lea_abs(0x533141, img) == 0x2EF8C08, "cell 0x2ef8c08")
    require(call_target(0x533193, img) == 0x170B0, "delete in destroy loop")
    require(call_target(0x5331B5, img) == 0x3B440, "3b440 #1")
    require(call_target(0x5331BD, img) == 0x3B440, "3b440 #2")
    require(call_target(0x5331C5, img) == 0x170B0, "delete obj")
    require(img.read(0x5331CA, 3) == bytes.fromhex("4d8926"), "store cell")
    # post-header continues (NOT claimed)
    require(lea_abs(0x5331CD, img) == 0x2EFC9E0, "post-header touches f08 cell")
    require(struct.unpack("<Q", img.read(0x15C8D18, 8))[0] == VSLOT, "vtable slot")
    require(img.read(0x255160, 3) == bytes.fromhex("ff5040"), "base vcall +0x40")

    nh = NATIVE.read_text()
    require("vslot_40_533100_header" in nh, "API")
    require("NOT modelled" in nh, "omission")
    require("0x2ef8c08" in nh or "2ef8c08" in nh, "cell")
    bh = BASE.read_text()
    require("use_lifted_vslot_40_header" in bh, "base wires")
    require("Vslot40_533100HeaderHooks" in bh or "vslot_40_detail" in bh, "hooks")

    print(
        f"OK check_f533100_vslot40_header: header={HEADER_SHA256[:12]}…; "
        f"full={FULL_SHA256[:12]}… ({FULL_SIZE} B NOT claimed past header); "
        f"cell=0x2ef8c08 size=0x1c0"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
