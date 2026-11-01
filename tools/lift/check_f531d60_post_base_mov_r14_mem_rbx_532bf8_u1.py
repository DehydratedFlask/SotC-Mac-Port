#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 registry mov [rbx],r14 @ 0x532bf8 after E607 call peel."""

from __future__ import annotations

import hashlib
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
PREV_END = 0x532BF8
FRAG = 0x532BF8
FRAG_SIZE = 3
FRAG_BYTES = bytes.fromhex("4c8933")
FRAG_SHA256 = "7241225d5359852087b3106b7249779b8871e6d8576b01678db3873f1ea38309"
END = 0x532BFB
NATIVE = ROOT / "native/core/init_531d60_post_base_mov_r14_mem_rbx_532bf8_u1.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_call_170b0_532bf3_u1.hpp"
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
        "kInit531d60PostBaseCall170b0_532bf3_U1EndRva = 0x532bf8" in PREV_NATIVE.read_text(),
        "call end == mov start",
    )
    require(PREV_END == FRAG, "contiguous")
    frag = img.read(FRAG, FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest() == FRAG_SHA256, "frag sha")
    require(frag == FRAG_BYTES, "mov [rbx],r14")
    require(img.read(END, 2) == bytes.fromhex("488d"), "next lea rax @ 0x532bfb")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_mov_r14_mem_rbx_532bf8_u1" in nh, "API")
    require("NOT modelled" in nh, "omit tail")
    require("merge_mov_r14_mem_rbx_532bf8_u1_ran" in BODY.read_text(), "body merge")
    require("merge_mov_r14_mem_rbx_532bf8_u1_ran" in REG.read_text(), "reg merge")
    require("kRegistryInit531d60FragmentPinnedRva = 0x532c0c" in REG.read_text(), "pin")
    require("init_531d60_post_base_mov_r14_mem_rbx_532bf8_u1.cpp" in CMAKE.read_text(), "cmake")

    print(
        f"OK check_f531d60_post_base_mov_r14_mem_rbx_532bf8_u1: frag={FRAG_SHA256[:12]}…; "
        f"mov [rbx],r14@{FRAG:#x}; end={END:#x}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
