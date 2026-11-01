#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 registry lea rax @ 0x532bfb after E608 mov peel."""

from __future__ import annotations

import hashlib
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
PREV_END = 0x532BFB
FRAG = 0x532BFB
FRAG_SIZE = 7
FRAG_BYTES = bytes.fromhex("488d05f6009b02")
FRAG_SHA256 = "e3c7b3ff7c67bc2dbe7f8acc1b9e244947b103e692e3824baac8a17bf07f669b"
END = 0x532C02
TARGET = 0x2EE2CF8
NATIVE = ROOT / "native/core/init_531d60_post_base_lea_rax_2ee2cf8_532bfb_u1.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_mov_r14_mem_rbx_532bf8_u1.hpp"
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
        "kInit531d60PostBaseMovR14MemRbx_532bf8_U1EndRva = 0x532bfb" in PREV_NATIVE.read_text(),
        "mov end == lea start",
    )
    require(PREV_END == FRAG, "contiguous")
    frag = img.read(FRAG, FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest() == FRAG_SHA256, "frag sha")
    require(frag == FRAG_BYTES, "lea rax,[0x2ee2cf8]")
    rel = struct.unpack("<i", frag[3:7])[0]
    require(FRAG + 7 + rel == TARGET, "lea target 0x2ee2cf8")
    require(img.read(END, 5) == bytes.fromhex("4c8b742420"), "next mov r14,[rsp+0x20]")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_lea_rax_2ee2cf8_532bfb_u1" in nh, "API")
    require("NOT modelled" in nh, "omit tail")
    require("merge_lea_rax_2ee2cf8_532bfb_u1_ran" in BODY.read_text(), "body merge")
    require("merge_lea_rax_2ee2cf8_532bfb_u1_ran" in REG.read_text(), "reg merge")
    require("kRegistryInit531d60FragmentPinnedRva = 0x532c07" in REG.read_text(), "pin")
    require("init_531d60_post_base_lea_rax_2ee2cf8_532bfb_u1.cpp" in CMAKE.read_text(), "cmake")

    print(
        f"OK check_f531d60_post_base_lea_rax_2ee2cf8_532bfb_u1: frag={FRAG_SHA256[:12]}…; "
        f"lea→{TARGET:#x}; end={END:#x}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
