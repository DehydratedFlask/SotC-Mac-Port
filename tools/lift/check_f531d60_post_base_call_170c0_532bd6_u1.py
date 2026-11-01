#!/usr/bin/env python3
"""call 0x170c0 @ 0x532bd6 after E603 je peel."""

from __future__ import annotations

import hashlib
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
PREV_END = 0X532BD6
FRAG = 0X532BD6
FRAG_SIZE = 5
FRAG_BYTES = bytes.fromhex("e8e544aeff")
FRAG_SHA256 = "97a733387f6b7f963680ddacc338ac8debc69de87c4b17c3e2f496f9fc8e8345"
END = 0X532BDB
NEXT_HEAD = bytes.fromhex("498b3c244c8b")
NATIVE = ROOT / "native/core/init_531d60_post_base_call_170c0_532bd6_u1.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_je_532bdb_532bd4_u1.hpp"
BODY = ROOT / "native/core/init_531d60_post_base_registry_body.hpp"
REG = ROOT / "native/core/registry_init_531d60.hpp"
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
        "kInit531d60PostBaseJe532bdb_532bd4_U1EndRva = 0x532bd6" in PREV_NATIVE.read_text(),
        "previous unit ends at frag start",
    )
    require(PREV_END == FRAG, "contiguous")
    frag = img.read(FRAG, FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest() == FRAG_SHA256, "frag sha")
    require(frag == FRAG_BYTES, "call 0x170c0 (tag-2 free)")
    rel = struct.unpack("<i", frag[1:5])[0]
    require(FRAG + 5 + rel == 0x170c0, "rel32 target 0x170c0")
    require(NEXT_HEAD[0:3] == bytes.fromhex("498b3c"), "next instr head @ 0x532bdb")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_call_170c0_532bd6_u1" in nh, "API")
    require("G1 PARTIAL" in nh, "omit marker")
    require("merge_call_170c0_532bd6_u1_ran" in BODY.read_text(), "body merge")
    require("merge_call_170c0_532bd6_u1_ran" in REG.read_text(), "reg merge")
    require("kRegistryInit531d60FragmentPinnedRva = 0x" in REG.read_text(), "pin")
    require("init_531d60_post_base_call_170c0_532bd6_u1.cpp" in CMAKE.read_text(), "cmake")

    print(
        f"OK check_f531d60_post_base_call_170c0_532bd6_u1: frag={FRAG_SHA256[:12]}…; end={END:#x}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
