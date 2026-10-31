#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue first 0xa543b0 call-site after E511 a2e5d0_u1."""

from __future__ import annotations

import hashlib
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
PREV_END = 0x532740
PREP = 0x532740
PREP_SIZE = 32
PREP_SHA256 = "93bbc7873a7b20e776597eedf306fbb55a85d0823f6a7e129bb86348333ad9e7"
FRAG = 0x532760
FRAG_SIZE = 5
FRAG_SHA256 = "c832b5949ba0d918a3c0b5bf633f4090be81bd841a11ca207574abf45e6b7b49"
END = 0x532765
TARGET = 0xA543B0
NATIVE = ROOT / "native/core/init_531d60_post_base_a543b0_u1.hpp"
SLICE = ROOT / "native/core/appmain_init_ctor_98a90_slice.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_a2e5d0_u1.hpp"
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
        "kInit531d60PostBaseA2e5d0U1EndRva = 0x532740" in PREV_NATIVE.read_text(),
        "a2e5d0_u1 end",
    )
    prep = img.read(PREP, PREP_SIZE)
    require(hashlib.sha256(prep).hexdigest() == PREP_SHA256, "prep sha")
    frag = img.read(FRAG, FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest() == FRAG_SHA256, "sha")
    require(frag[0] == 0xE8, "call opcode")
    rel = struct.unpack("<i", frag[1:5])[0]
    require(FRAG + 5 + rel == TARGET, "call target")
    require(img.read(END, 3) == bytes.fromhex("488b44"), "next mov head")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_a543b0_u1" in nh, "API")
    require("NOT modelled" in nh, "omission")
    st = SLICE.read_text()
    require("post_base_a543b0_u1" in st or "PostBaseA543b0U1" in st, "wire")
    require("core/init_531d60_post_base_a543b0_u1.cpp" in CMAKE.read_text(), "cmake")

    print(
        f"OK check_f531d60_post_base_a543b0_u1: frag={FRAG_SHA256[:12]}…; "
        f"call 0xa543b0@{FRAG:#x}; prep={PREP:#x}+{PREP_SIZE}B demoted; end={END:#x}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
