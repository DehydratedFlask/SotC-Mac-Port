#!/usr/bin/env python3
"""ELF-tie app vslot 0x534130 (Base Initialize +0x30 bool gate).

Private: needs local/re/eboot.elf. Pins body, strings, BSS cells, callees.
Does NOT claim bodies of 0x255f60 / 0x1fc80 / 0x1e5e0 or vslot 0x533100.
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
SLOT = 0x534130
SLOT_SIZE = 294
SLOT_SHA256 = "e88336995d4452cf010dfea25321df84d697e9ea5be056670c56ab5bc830e60f"
VT30 = 0x15C8D08
NATIVE = ROOT / "native/core/vslot_30_534130.hpp"
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


def u64(img: d.Image, addr: int) -> int:
    return struct.unpack("<Q", img.read(addr, 8))[0]


def cstr(img: d.Image, addr: int) -> bytes:
    out = bytearray()
    i = 0
    while True:
        b = img.read(addr + i, 1)[0]
        if b == 0:
            return bytes(out)
        out.append(b)
        i += 1
        if i > 64:
            raise SystemExit(f"FAIL unterminated at {addr:#x}")


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    require(hashlib.sha256(img.read(SLOT, SLOT_SIZE)).hexdigest() == SLOT_SHA256, "slot sha")
    require(img.read(SLOT + SLOT_SIZE - 1, 1) == b"\xc3", "ret")
    require(u64(img, VT30) == SLOT, "vtable +0x30")

    require(call_target(0x53413A, img) == 0x255F60, "precheck 0x255f60")
    require(call_target(0x534155, img) == 0x1FC80, "lookup boss")
    require(call_target(0x534169, img) == 0x1FC80, "lookup jump")
    require(call_target(0x53417E, img) == 0x1E5E0, "parse")
    require(call_target(0x5341BD, img) == 0x1FC80, "lookup outro")
    require(call_target(0x534213, img) == 0x1FC80, "lookup stage")

    require(cstr(img, lea_abs(0x53414B, img)) == b"boss", "boss")
    require(cstr(img, lea_abs(0x53415F, img)) == b"shadow_jumpToBoss", "jump")
    require(cstr(img, lea_abs(0x5341B3, img)) == b"boss_outro", "outro")
    require(cstr(img, lea_abs(0x534209, img)) == b"stage", "stage")

    require(lea_abs(0x534183, img) == 0x325E8B4, "cell boss val")
    require(lea_abs(0x53418C, img) == 0x325E8B8, "cell boss mode")
    require(lea_abs(0x5341A3, img) == 0x2DCDF14, "cell boss flag")
    require(lea_abs(0x534230, img) == 0x2DD9578, "cell stage")

    require(img.read(0x534218, 3) == bytes.fromhex("41b601"), "mov r14b,1 before stage test")
    require(img.read(0x255148, 3) == bytes.fromhex("ff5030"), "base vcall +0x30")

    nh = NATIVE.read_text()
    require("vslot_30_534130" in nh, "API")
    require("NOT modelled" in nh or "not modelled" in nh.lower(), "omission")
    require("0x533100" in nh, "cites sibling")
    bh = BASE.read_text()
    require("use_lifted_vslot_30" in bh, "base wires")
    require("vslot_30_534130" in bh or "Vslot30_534130Hooks" in bh, "base includes")

    print(
        f"OK check_f534130_vslot30: slot={SLOT_SHA256[:12]}…; "
        f"names boss/jump/outro/stage; cells pinned; "
        f"0x255f60/0x1fc80/0x1e5e0/0x533100 NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
