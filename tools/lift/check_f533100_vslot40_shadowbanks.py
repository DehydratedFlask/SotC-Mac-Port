#!/usr/bin/env python3
"""ELF-tie app vslot 0x533100 shadowbanks load-attempt after anims attach.

Private: needs local/re/eboot.elf. Pins 99 B fragment, notify, path, cells.
Does NOT claim success atexit, banks attach, or 0x14c2f0/0x95580 bodies.
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
BANKS = 0x533870
BANKS_SIZE = 99
BANKS_SHA256 = "b925ff8d94d5e93c28b6755c66adefac66534c996bfbb03765e0b8613093d3c2"
END = 0x5338D3
NATIVE = ROOT / "native/core/vslot_40_533100_shadowbanks.hpp"
BASE = ROOT / "native/core/base_initialize_255130.hpp"
BODY_14CCB0 = bytes.fromhex("f0ff4730c3")


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

    require(hashlib.sha256(img.read(BANKS, BANKS_SIZE)).hexdigest() == BANKS_SHA256, "sha")
    require(lea_abs(0x533870, img) == 0x2EFC9E0, "f08 cell")
    require(img.read(0x53387D, 4) == bytes.fromhex("4883c720"), "add rdi,0x20")
    require(call_target(0x533881, img) == 0x95580, "95580 notify")
    require(lea_abs(0x533886, img) == 0x2EE2CF8, "loader cell")
    require(
        cstr(img, lea_abs(0x533897, img)) == b"$/sound/shadow/****/shadowbanks.crmp", "path"
    )
    require(call_target(0x5338AF, img) == 0x14C2F0, "14c2f0")
    require(img.read(0x5338C1, 2) == bytes.fromhex("41ff"), "indirect call form")
    require(call_target(0x5338CE, img) == 0x14CCB0, "14ccb0")
    require(img.read(0x14CCB0, 5) == BODY_14CCB0, "14ccb0 body")
    require(img.read(0x5338D3, 2) == bytes.fromhex("80bd"), "flag cmp")

    nh = NATIVE.read_text()
    require("vslot_40_533100_shadowbanks" in nh, "API")
    require("shadowbanks.crmp" in nh, "path")
    require("95580" in nh, "notify")
    require("NOT modelled" in nh, "omission")
    require("shadowbanks" in BASE.read_text(), "wire")

    print(
        f"OK check_f533100_vslot40_shadowbanks: banks={BANKS_SHA256[:12]}…; "
        f"notify; path; cells; 14c2f0/vcall/14ccb0; end={END:#x}; "
        f"success_atexit/banks_attach/14c2f0/95580 bodies NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
