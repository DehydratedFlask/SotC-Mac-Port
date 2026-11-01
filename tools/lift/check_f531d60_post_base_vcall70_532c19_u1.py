#!/usr/bin/env python3
"""call [rax+0x70] @ 0x532c19 after E614 mov rax."""

from __future__ import annotations

import hashlib
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
PREV_END = 0X532C19
FRAG = 0X532C19
FRAG_SIZE = 3
FRAG_BYTES = bytes.fromhex("ff5070")
FRAG_SHA256 = "0d81f6f0c789bb2d181997a46a6c9d8771ff74f200c6af35795cf2e6687ff1e8"
END = 0X532C1C
NATIVE = ROOT / "native/core/init_531d60_post_base_vcall70_532c19_u1.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_mov_rax_rdi_mem_532c16_u1.hpp"
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
        "kInit531d60PostBaseMovRaxRdiMem_532c16_U1EndRva = 0x532c19" in PREV_NATIVE.read_text(),
        "previous unit ends at frag start",
    )
    require(PREV_END == FRAG, "contiguous")
    frag = img.read(FRAG, FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest() == FRAG_SHA256, "frag sha")
    require(frag == FRAG_BYTES, "call [rax+0x70]")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_vcall70_532c19_u1" in nh, "API")
    require("G1 PARTIAL" in nh, "omit marker")
    require("merge_vcall70_532c19_u1_ran" in BODY.read_text(), "body merge")
    require("merge_vcall70_532c19_u1_ran" in REG.read_text(), "reg merge")
    require("kRegistryInit531d60FragmentPinnedRva = 0x" in REG.read_text(), "pin")
    require("init_531d60_post_base_vcall70_532c19_u1.cpp" in CMAKE.read_text(), "cmake")

    print(
        f"OK check_f531d60_post_base_vcall70_532c19_u1: frag={FRAG_SHA256[:12]}…; end={END:#x}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
