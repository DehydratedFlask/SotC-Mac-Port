#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue E second vcall +0x18 @ 0x532a72 after E522."""

from __future__ import annotations

import hashlib
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
PREP = 0x532A67
PREP_SIZE = 11
PREP_SHA256 = "b63792725390f937f14d983727ff481b586a1d6c8efcb5a0bc22f1f5a645d7d0"
FRAG = 0x532A72
FRAG_SIZE = 3
FRAG_SHA256 = "22558207d1d705fcbda49d89be5b34ebe09d409878b0bf88e6819b7724e29e96"
END = 0x532A75
STACK_OFF = 0x500
RESOLVED = 0x538200
NATIVE = ROOT / "native/core/init_531d60_post_base_vcall18_532a72_u1.hpp"
CONT_E = ROOT / "native/core/init_531d60_post_base_continue_e.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_a54920_532a62_u1.hpp"
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
        "kInit531d60PostBaseA54920_532a62_U1EndRva = 0x532a67" in PREV_NATIVE.read_text(),
        "a54920_532a62 end == prep start",
    )
    require(PREP + PREP_SIZE == FRAG, "prep size")
    prep = img.read(PREP, PREP_SIZE)
    require(hashlib.sha256(prep).hexdigest() == PREP_SHA256, "prep sha")
    require(prep[0:4] == bytes.fromhex("488b8424"), "prep mov rax,[rsp+…]")
    require(struct.unpack("<I", prep[4:8])[0] == STACK_OFF, "rsp+0x500")
    require(prep[8:11] == bytes.fromhex("4c89f7"), "mov rdi,r14")
    frag = img.read(FRAG, FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest() == FRAG_SHA256, "frag sha")
    require(frag == bytes.fromhex("ff5018"), "call [rax+0x18]")
    require(img.read(END, 4)[0:4] == bytes.fromhex("c5fa7e05"), "post-call vmovq head")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_vcall18_532a72_u1" in nh, "API")
    require("NOT modelled" in nh, "omission")
    require(f"0x{RESOLVED:x}" in nh.lower() or "538200" in nh, "resolved ret pin")
    vmovq = (ROOT / "native/core/init_531d60_post_base_vmovq_532a75_u1.hpp").read_text()
    require("kInit531d60PostBaseVmovq532a75_U1Rva = 0x532a75" in vmovq, "vmovq u1 follows vcall72")
    ce = CONT_E.read_text()
    require("merge_vcall18_532a72_u1_ran" in ce, "vcall72 merge hook")
    require("core/init_531d60_post_base_vcall18_532a72_u1.cpp" in CMAKE.read_text(), "cmake")

    print(
        f"OK check_f531d60_post_base_vcall18_532a72_u1: frag={FRAG_SHA256[:12]}…; "
        f"call [rax+0x18]@{FRAG:#x}→0x{RESOLVED:x}(ret elided); "
        f"prep={PREP:#x}+{PREP_SIZE}B demoted; end={END:#x}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
