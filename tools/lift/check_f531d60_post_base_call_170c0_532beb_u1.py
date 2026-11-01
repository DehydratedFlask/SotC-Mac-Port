#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 registry call 0x170c0 @ 0x532beb after E604 je peel."""

from __future__ import annotations

import hashlib
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
PREV_END = 0x532BEB
FRAG = 0x532BEB
FRAG_SIZE = 5
FRAG_BYTES = bytes.fromhex("e8d044aeff")
FRAG_SHA256 = "a4830c6c94910c61fa171c7aa1e9beca321fb35d44fa9cbc9d7346842e828bc2"
END = 0x532BF0
CALL_TARGET = 0x170C0
NATIVE = ROOT / "native/core/init_531d60_post_base_call_170c0_532beb_u1.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_je_532bf0_532be6_u1.hpp"
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
        "kInit531d60PostBaseJe532bf0_532be6_U1EndRva = 0x532beb" in PREV_NATIVE.read_text(),
        "je end == call start",
    )
    require(PREV_END == FRAG, "contiguous")
    frag = img.read(FRAG, FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest() == FRAG_SHA256, "frag sha")
    require(frag == FRAG_BYTES, "call rel32")
    require(frag[0] == 0xE8, "call opcode")
    rel = struct.unpack("<i", frag[1:5])[0]
    require(FRAG + 5 + rel == CALL_TARGET, "target 0x170c0")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_call_170c0_532beb_u1" in nh, "API")
    require("NOT modelled" in nh, "omit tail")
    require("merge_call_170c0_532beb_u1_ran" in BODY.read_text(), "body merge")
    require("merge_call_170c0_532beb_u1_ran" in REG.read_text(), "reg merge")
    require("kRegistryInit531d60FragmentPinnedRva = 0x532bf3" in REG.read_text(), "pin")
    require("init_531d60_post_base_call_170c0_532beb_u1.cpp" in CMAKE.read_text(), "cmake")

    print(
        f"OK check_f531d60_post_base_call_170c0_532beb_u1: frag={FRAG_SHA256[:12]}…; "
        f"call 0x170c0@{FRAG:#x}; end={END:#x}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
