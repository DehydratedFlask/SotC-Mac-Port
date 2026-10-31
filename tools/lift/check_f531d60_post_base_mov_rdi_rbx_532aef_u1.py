#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue E mov rdi,rbx after E539."""

from __future__ import annotations

import hashlib
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
FRAG = 0x532AEF
FRAG_SIZE = 3
FRAG_SHA256 = "0bc5a45e8b7b5bfc6d88912c19ddf334fbbdb17c2a1df7b79adf0dda065730ab"
END = 0x532AF2
NATIVE = ROOT / "native/core/init_531d60_post_base_mov_rdi_rbx_532aef_u1.hpp"
CONT_E = ROOT / "native/core/init_531d60_post_base_continue_e.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_mov_rsp4b0_rax_532ae7_u1.hpp"
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
        "kInit531d60PostBaseMovRsp4b0_Rax_532ae7_U1EndRva = 0x532aef" in PREV_NATIVE.read_text(),
        "mov4b0_rax end == mov_rdi_rbx start",
    )
    frag = img.read(FRAG, FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest() == FRAG_SHA256, "frag sha")
    require(frag[0:3] == bytes.fromhex("4889df"), "mov rdi,rbx head")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_mov_rdi_rbx_532aef_u1" in nh, "API")
    ce = CONT_E.read_text()
    require("kInit531d60PostBaseContinueERva = 0x532af2" in ce, "shrunk continue_e start")
    require("merge_mov_rdi_rbx_532aef_u1_ran" in ce, "mov_rdi_rbx merge hook")
    require(
        "core/init_531d60_post_base_mov_rdi_rbx_532aef_u1.cpp" in CMAKE.read_text(),
        "cmake",
    )

    require(img.read(END, 3)[0:3] == bytes.fromhex("ff5018"), "post-mov vcall18 head")

    print(
        f"OK check_f531d60_post_base_mov_rdi_rbx_532aef_u1: frag={FRAG_SHA256[:12]}…; "
        f"end={END:#x}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
