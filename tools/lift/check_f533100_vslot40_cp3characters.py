#!/usr/bin/env python3
"""ELF-tie app vslot 0x533100 cp3Characters load-attempt after bossmusic attach.

Private: needs local/re/eboot.elf. Pins 88 B fragment, notify, path, cells.
Does NOT claim attach/install, 14ccb0, success atexit, or helper bodies.
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
LOAD = 0x533E54
LOAD_SIZE = 88
LOAD_SHA256 = "5be45d9358696b09fb498556ac927adb5cf03a5f8da28e47d02165c5f07e37d2"
END = 0x533EAC
NATIVE = ROOT / "native/core/vslot_40_533100_cp3characters.hpp"
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

    require(hashlib.sha256(img.read(LOAD, LOAD_SIZE)).hexdigest() == LOAD_SHA256, "sha")
    require(lea_abs(0x533E54, img) == 0x2EFC9E0, "f08 cell")
    require(img.read(0x533E5E, 4) == bytes.fromhex("4883c770"), "add rdi,0x70")
    require(call_target(0x533E62, img) == 0x95580, "95580 notify")
    require(lea_abs(0x533E67, img) == 0x2EE2CF8, "loader cell")
    require(
        cstr(img, lea_abs(0x533E7C, img)) == b"$/characters/shadow/****/cp3Characters.calt",
        "path",
    )
    require(call_target(0x533E97, img) == 0x14C2F0, "14c2f0")
    require(img.read(0x533EA9, 2) == bytes.fromhex("41ff"), "indirect call form")
    require(img.read(0x533EAC, 4) == bytes.fromhex("498b9db8"), "attach prev load")

    nh = NATIVE.read_text()
    require("vslot_40_533100_cp3characters" in nh, "API")
    require("cp3Characters.calt" in nh, "path")
    require("95580" in nh, "notify")
    require("NOT modelled" in nh, "omission")
    require("cp3characters" in BASE.read_text() or "Cp3characters" in BASE.read_text(),
            "wire")

    print(
        f"OK check_f533100_vslot40_cp3characters: calt={LOAD_SHA256[:12]}…; "
        f"notify; path; cells; 14c2f0/vcall; end={END:#x}; "
        f"attach/14ccb0/success_atexit/14c2f0/95580 bodies NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
