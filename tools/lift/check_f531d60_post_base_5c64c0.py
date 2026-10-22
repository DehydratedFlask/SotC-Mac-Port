#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue call-site for 0x5c64c0.

Private: needs local/re/eboot.elf. Pins 5 B fragment after 0x56dcb0/0x56dca0.
Does NOT claim body of 0x5c64c0 or later Init continues.
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
FRAG = 0x531DFE
FRAG_SIZE = 5
FRAG_SHA256 = "11e4ea9d33f244a619eaacfaa0633b319b9360b6257f97f71aeca57b9bf67b24"
END = 0x531E03
TARGET = 0x5C64C0
PROLOGUE_SIZE = 0x70  # through first nested call; body NOT modelled
PROLOGUE_SHA256 = "a4744b2a2dc8cfdc56611e67d88aae91f2f68a0e652fb509290e33794e92831a"
STORE_CELL = 0x2FDAA60
NATIVE = ROOT / "native/core/init_531d60_post_base_5c64c0.hpp"
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

    require(call_target(0x531DF4, img) == 0x56DCB0, "prev 0x56dcb0")
    require(call_target(0x531DF9, img) == 0x56DCA0, "intervening 0x56dca0 ret-nop")
    require(img.read(0x56DCA0, 1) == b"\xc3", "0x56dca0 is ret")
    require(hashlib.sha256(img.read(FRAG, FRAG_SIZE)).hexdigest() == FRAG_SHA256, "sha")
    require(call_target(FRAG, img) == TARGET, "call 0x5c64c0")
    require(call_target(END, img) == 0x1290, "next 0x1290 (not claimed)")

    pro = img.read(TARGET, PROLOGUE_SIZE)
    require(hashlib.sha256(pro).hexdigest() == PROLOGUE_SHA256, "prologue sha")
    lea = img.read(TARGET + 0x0C, 7)
    require(lea[0:3] == bytes.fromhex("488d05"), "lea store cell")
    cell = TARGET + 0x0C + 7 + struct.unpack("<i", lea[3:7])[0]
    require(cell == STORE_CELL, "store cell")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_5c64c0" in nh, "API")
    require("NOT modelled" in nh, "omission")
    require("post_base_5c64c0" in SLICE.read_text() or "PostBase5c64c0" in SLICE.read_text(), "wire")

    print(
        f"OK check_f531d60_post_base_5c64c0: frag={FRAG_SHA256[:12]}…; "
        f"call {TARGET:#x}; end={END:#x}; "
        f"5c64c0_body/1290/56dcb0_body/499f80_body/140d90_body/14c2f0/6a5370 NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
