#!/usr/bin/env python3
"""nopw padding @ 0x532b6a after E569 je peel."""

from __future__ import annotations

import hashlib
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
PREV_END = 0X532B6A
FRAG = 0X532B6A
FRAG_SIZE = 6
FRAG_BYTES = bytes.fromhex("660f1f440000")
FRAG_SHA256 = "830498425a3f3929e23603db13a6dd564f33297213a33122c0393cd105c624b3"
END = 0X532B70
NEXT_HEAD = bytes.fromhex("498b5d104d8b")
NATIVE = ROOT / "native/core/init_531d60_post_base_nopw_532b6a_u1.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_je_532bba_532b68_u1.hpp"
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
        "kInit531d60PostBaseJe532bba_532b68_U1EndRva = 0x532b6a" in PREV_NATIVE.read_text(),
        "previous unit ends at frag start",
    )
    require(PREV_END == FRAG, "contiguous")
    frag = img.read(FRAG, FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest() == FRAG_SHA256, "frag sha")
    require(frag == FRAG_BYTES, "nopw (%rax,%rax) padding")
    require(NEXT_HEAD[0:3] == bytes.fromhex("498b5d"), "next instr head @ 0x532b70")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_nopw_532b6a_u1" in nh, "API")
    require("G1 PARTIAL" in nh, "omit marker")
    require("merge_nopw_532b6a_u1_ran" in BODY.read_text(), "body merge")
    require("merge_nopw_532b6a_u1_ran" in REG.read_text(), "reg merge")
    require("kRegistryInit531d60FragmentPinnedRva = 0x" in REG.read_text(), "pin")
    require("init_531d60_post_base_nopw_532b6a_u1.cpp" in CMAKE.read_text(), "cmake")

    print(
        f"OK check_f531d60_post_base_nopw_532b6a_u1: frag={FRAG_SHA256[:12]}…; end={END:#x}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
