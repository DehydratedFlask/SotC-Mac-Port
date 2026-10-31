#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue first 0x1fc80 call-site after E512 a543b0_u1."""

from __future__ import annotations

import hashlib
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
PREV_END = 0x532765
PREP = 0x532765
PREP_SIZE = 16
PREP_SHA256 = "114f2073a47890304e45fa7b7bc37c3331d7d9a260279e07eecbb929a411955f"
FRAG = 0x532775
FRAG_SIZE = 5
FRAG_SHA256 = "7f083533b1025791bcb29da0d369eed42d2b6f8321dfe83a2a165467e54528c3"
END = 0x53277A
TARGET = 0x1FC80
NATIVE = ROOT / "native/core/init_531d60_post_base_1fc80_u1.hpp"
SLICE = ROOT / "native/core/appmain_init_ctor_98a90_slice.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_a543b0_u1.hpp"
CMAKE = ROOT / "native/CMakeLists.txt"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    require(PREV_END == PREP, "prev end == prep start")
    require(PREP + PREP_SIZE == FRAG, "prep size")
    require(
        "kInit531d60PostBaseA543b0U1EndRva = 0x532765" in PREV_NATIVE.read_text(),
        "a543b0_u1 end",
    )
    prep = img.read(PREP, PREP_SIZE)
    require(hashlib.sha256(prep).hexdigest() == PREP_SHA256, "prep sha")
    frag = img.read(FRAG, FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest() == FRAG_SHA256, "sha")
    require(frag[0] == 0xE8, "call opcode")
    rel = struct.unpack("<i", frag[1:5])[0]
    require(FRAG + 5 + rel == TARGET, "call target")
    require(img.read(END, 2) == bytes.fromhex("4885"), "next test head")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_1fc80_u1" in nh, "API")
    require("NOT modelled" in nh, "omission")
    st = SLICE.read_text()
    require("post_base_1fc80_u1" in st or "PostBase1fc80U1" in st, "wire")
    require("core/init_531d60_post_base_1fc80_u1.cpp" in CMAKE.read_text(), "cmake")

    print(
        f"OK check_f531d60_post_base_1fc80_u1: frag={FRAG_SHA256[:12]}…; "
        f"call 0x1fc80@{FRAG:#x}; prep={PREP:#x}+{PREP_SIZE}B demoted; end={END:#x}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
