#!/usr/bin/env python3
"""lea rsi,[r14+0x3ec0] @ 0x532c0f after E612 mov rdi,[rax]."""

from __future__ import annotations

import hashlib
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
PREV_END = 0X532C0F
FRAG = 0X532C0F
FRAG_SIZE = 7
FRAG_BYTES = bytes.fromhex("498db6c03e0000")
FRAG_SHA256 = "355a18b607c5c3910c7fccc1c9abe92426e12f03abedac9d6a0aa1362661a4ed"
END = 0X532C16
NATIVE = ROOT / "native/core/init_531d60_post_base_lea_rsi_r14_3ec0_532c0f_u1.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_mov_rdi_rax_mem_532c0c_u1.hpp"
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
        "kInit531d60PostBaseMovRdiRaxMem_532c0c_U1EndRva = 0x532c0f" in PREV_NATIVE.read_text(),
        "previous unit ends at frag start",
    )
    require(PREV_END == FRAG, "contiguous")
    frag = img.read(FRAG, FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest() == FRAG_SHA256, "frag sha")
    require(frag == FRAG_BYTES, "lea rsi,[r14+0x3ec0]")
    require(struct.unpack("<I", frag[3:7])[0] == 0x3ec0, "disp 0x3ec0")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_lea_rsi_r14_3ec0_532c0f_u1" in nh, "API")
    require("G1 PARTIAL" in nh, "omit marker")
    require("merge_lea_rsi_r14_3ec0_532c0f_u1_ran" in BODY.read_text(), "body merge")
    require("merge_lea_rsi_r14_3ec0_532c0f_u1_ran" in REG.read_text(), "reg merge")
    require("kRegistryInit531d60FragmentPinnedRva = 0x" in REG.read_text(), "pin")
    require("init_531d60_post_base_lea_rsi_r14_3ec0_532c0f_u1.cpp" in CMAKE.read_text(), "cmake")

    print(
        f"OK check_f531d60_post_base_lea_rsi_r14_3ec0_532c0f_u1: frag={FRAG_SHA256[:12]}…; end={END:#x}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
