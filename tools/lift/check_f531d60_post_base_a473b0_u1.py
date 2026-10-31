#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 merge batch-D direct call-site after continue C fall-through."""

from __future__ import annotations

import hashlib
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
PREP = 0x53299D
PREP_SIZE = 12
PREP_SHA256 = "28e86e19fe87c8d0095df8433743c1c33507823f44b09355244becbab9ed76fb"
FRAG = 0x5329A9
FRAG_SIZE = 5
FRAG_SHA256 = "4066b90aefad2531c06181609539febe60d131c065c7f7fbbcebc8caa11c8b8b"
END = 0x5329AE
TARGET = 0xA473B0
CELL = 0x32F79A8
NATIVE = ROOT / "native/core/init_531d60_post_base_a473b0_u1.hpp"
SLICE = ROOT / "native/core/appmain_init_ctor_98a90_slice.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_continue_c.hpp"
CMAKE = ROOT / "native/CMakeLists.txt"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    require(
        "kInit531d60PostBaseContinueCEndRva = 0x53299d" in PREV_NATIVE.read_text(),
        "continue_c end",
    )
    require(PREP + PREP_SIZE == FRAG, "prep size")
    prep = img.read(PREP, PREP_SIZE)
    require(hashlib.sha256(prep).hexdigest() == PREP_SHA256, "prep sha")
    require(prep[0:3] == bytes.fromhex("488d05"), "prep lea")
    require(PREP + 7 + struct.unpack("<i", prep[3:7])[0] == CELL, "cell target")
    frag = img.read(FRAG, FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest() == FRAG_SHA256, "frag sha")
    require(frag[0] == 0xE8, "call opcode")
    rel = struct.unpack("<i", frag[1:5])[0]
    require(FRAG + 5 + rel == TARGET, "call target")
    require(img.read(END, 7)[0:3] == bytes.fromhex("488d05"), "batch E entry lea head")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_a473b0_u1" in nh, "API")
    st = SLICE.read_text()
    require("post_base_continue_d" in st, "slice wire")
    require("core/init_531d60_post_base_a473b0_u1.cpp" in CMAKE.read_text(), "cmake")

    print(
        f"OK check_f531d60_post_base_a473b0_u1: frag={FRAG_SHA256[:12]}…; "
        f"call 0xa473b0@{FRAG:#x}; prep={PREP:#x}+{PREP_SIZE}B demoted; "
        f"end={END:#x}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
