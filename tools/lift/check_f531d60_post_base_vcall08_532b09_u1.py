#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue E vcall +0x8 @ 0x532b09 after E546."""

from __future__ import annotations

import hashlib
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
FRAG = 0x532B09
FRAG_SIZE = 3
FRAG_SHA256 = "3bf4b917424b527784120e40e040cc1709ac317c24237f69f8dad8bee50a4fde"
END = 0x532B0C
RESOLVED = 0x54B250
NATIVE = ROOT / "native/core/init_531d60_post_base_vcall08_532b09_u1.hpp"
CONT_E = ROOT / "native/core/init_531d60_post_base_continue_e.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_mov_rax_rdi_mem_532b06_u1.hpp"
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
        "kInit531d60PostBaseMovRaxRdiMem_532b06_U1EndRva = 0x532b09" in PREV_NATIVE.read_text(),
        "mov_rax_rdi_mem end == vcall08 start",
    )
    frag = img.read(FRAG, FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest() == FRAG_SHA256, "frag sha")
    require(frag == bytes.fromhex("ff5008"), "call [rax+0x8]")
    require(img.read(END, 5)[0:2] == bytes([0xBF, 0x98]), "post-call mov edi,0x98 head")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_vcall08_532b09_u1" in nh, "API")
    require("NOT modelled" in nh, "omission")
    require(f"0x{RESOLVED:x}" in nh.lower() or "54b250" in nh, "resolved target pin")
    ce = CONT_E.read_text()
    require("kInit531d60PostBaseContinueERva = 0x532b0c" in ce, "empty continue_e start")
    require("merge_vcall08_532b09_u1_ran" in ce, "vcall08 merge hook")
    require("core/init_531d60_post_base_vcall08_532b09_u1.cpp" in CMAKE.read_text(), "cmake")

    print(
        f"OK check_f531d60_post_base_vcall08_532b09_u1: frag={FRAG_SHA256[:12]}…; "
        f"call [rax+0x8]@{FRAG:#x}→0x{RESOLVED:x}; end={END:#x}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
