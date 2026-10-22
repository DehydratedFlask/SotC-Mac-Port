#!/usr/bin/env python3
"""ELF-tie app vslot 0x533100 shadowmodels load-attempt after gfx prelude.

Private: needs local/re/eboot.elf. Pins 74 B fragment, path, cells, 0x14ccb0.
Does NOT claim 0x14c2f0 body, vcall body, attach, or shadowskels.
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
SHADOW = 0x533435
SHADOW_SIZE = 74
SHADOW_SHA256 = "98ef60e8b1aa7506e56010afff1822b06c16ba39a483b750823c430ee133f47a"
END = 0x53347f
NATIVE = ROOT / "native/core/vslot_40_533100_shadowmodels.hpp"
BASE = ROOT / "native/core/base_initialize_255130.hpp"
BODY_14CCB0 = bytes.fromhex("f0ff4730c3")  # lock inc [rdi+0x30]; ret


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

    require(hashlib.sha256(img.read(SHADOW, SHADOW_SIZE)).hexdigest() == SHADOW_SHA256, "sha")
    require(lea_abs(0x533435, img) == 0x2EE2CF8, "loader cell")
    require(lea_abs(0x53343C, img) == 0x2EFC9E0, "f08 cell")
    require(cstr(img, lea_abs(0x53344A, img)) == b"$/Models/****/shadowmodels.crmp", "path")
    require(call_target(0x533461, img) == 0x14C2F0, "14c2f0")
    require(img.read(0x533470, 2) == bytes.fromhex("41ff"), "indirect call form")  # call r14
    require(call_target(0x53347A, img) == 0x14CCB0, "14ccb0")
    require(img.read(0x14CCB0, 5) == BODY_14CCB0, "14ccb0 body")
    # next insn is cmp success flag
    require(img.read(0x53347F, 2) in (bytes.fromhex("807d"), bytes.fromhex("80bd")), "flag cmp")

    nh = NATIVE.read_text()
    require("vslot_40_533100_shadowmodels" in nh, "API")
    require("shadowmodels.crmp" in nh, "path")
    require("refcount_inc_30_14ccb0" in nh, "14ccb0 lift")
    require("NOT modelled" in nh, "omission")
    require("vslot_40_shadow" in BASE.read_text() or "shadowmodels" in BASE.read_text(), "wire")

    print(
        f"OK check_f533100_vslot40_shadow: shadow={SHADOW_SHA256[:12]}…; "
        f"path; cells; 14c2f0/vcall/14ccb0; end={END:#x}; "
        f"14c2f0/vcall/attach/shadowskels NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
