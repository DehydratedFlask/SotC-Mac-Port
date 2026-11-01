#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue E third vcall +0x18 @ 0x532af2 after E540."""

from __future__ import annotations

import hashlib
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
PREP = 0x532AEF
PREP_SIZE = 3
PREP_SHA256 = "0bc5a45e8b7b5bfc6d88912c19ddf334fbbdb17c2a1df7b79adf0dda065730ab"
FRAG = 0x532AF2
FRAG_SIZE = 3
FRAG_SHA256 = "22558207d1d705fcbda49d89be5b34ebe09d409878b0bf88e6819b7724e29e96"
END = 0x532AF5
RESOLVED = 0x5388C0
NATIVE = ROOT / "native/core/init_531d60_post_base_vcall18_532af2_u1.hpp"
CONT_E = ROOT / "native/core/init_531d60_post_base_continue_e.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_mov_rdi_rbx_532aef_u1.hpp"
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
        "kInit531d60PostBaseMovRdiRbx_532aef_U1EndRva = 0x532af2" in PREV_NATIVE.read_text(),
        "mov_rdi_rbx end == vcall start",
    )
    require(PREP + PREP_SIZE == FRAG, "prep size")
    prep = img.read(PREP, PREP_SIZE)
    require(hashlib.sha256(prep).hexdigest() == PREP_SHA256, "prep sha")
    require(prep[0:3] == bytes.fromhex("4889df"), "mov rdi,rbx")
    frag = img.read(FRAG, FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest() == FRAG_SHA256, "frag sha")
    require(frag == bytes.fromhex("ff5018"), "call [rax+0x18]")
    require(img.read(END, 7)[0:2] == bytes.fromhex("488d"), "post-call lea head")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_vcall18_532af2_u1" in nh, "API")
    require("NOT modelled" in nh, "omission")
    require(f"0x{RESOLVED:x}" in nh.lower() or "5388c0" in nh, "resolved ret pin")
    ce = CONT_E.read_text()
    require("kInit531d60PostBaseContinueERva = 0x532b0c" in ce, "shrunk continue_e start")
    require("merge_vcall18_532af2_u1_ran" in ce, "vcall532af2 merge hook")
    require("core/init_531d60_post_base_vcall18_532af2_u1.cpp" in CMAKE.read_text(), "cmake")

    print(
        f"OK check_f531d60_post_base_vcall18_532af2_u1: frag={FRAG_SHA256[:12]}…; "
        f"call [rax+0x18]@{FRAG:#x}→0x{RESOLVED:x}(ret elided); "
        f"prep={PREP:#x}+{PREP_SIZE}B demoted; end={END:#x}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
