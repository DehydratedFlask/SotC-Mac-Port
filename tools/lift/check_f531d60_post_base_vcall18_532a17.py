#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue E first virtual-call +0x18 after a54920 u1."""

from __future__ import annotations

import hashlib
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
PREV_END = 0x532A0C
FRAG = 0x532A0C
FRAG_SIZE = 14
FRAG_SHA256 = "3813daded6de13841978a55a6d97a2a52b4052c53262f7916d6d986df368a89d"
END = 0x532A1A
VCALL = 0x532A17
STACK_OFF = 0x560
RESOLVED = 0x538200
NATIVE = ROOT / "native/core/init_531d60_post_base_vcall18_532a17.hpp"
CONT_E = ROOT / "native/core/init_531d60_post_base_continue_e.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_a54920_u1.hpp"
CMAKE = ROOT / "native/CMakeLists.txt"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    require(PREV_END == FRAG, "prev end == frag start")
    require(
        "kInit531d60PostBaseA54920U1EndRva = 0x532a0c" in PREV_NATIVE.read_text(),
        "a54920 u1 end",
    )
    frag = img.read(FRAG, FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest() == FRAG_SHA256, "sha")
    require(frag[0:4] == bytes.fromhex("488b8424"), "mov rax,[rsp+…]")
    require(struct.unpack("<I", frag[4:8])[0] == STACK_OFF, "rsp+0x560")
    require(frag[8:11] == bytes.fromhex("4c89f7"), "mov rdi,r14")
    require(frag[11:14] == bytes.fromhex("ff5018"), "call [rax+0x18]")
    require(VCALL == FRAG + 11, "vcall site")
    require(img.read(END, 4)[0:3] == bytes.fromhex("488b7d"), "next mov rdi,[r13]")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_vcall18_532a17" in nh, "API")
    require("NOT modelled" in nh, "omission")
    require(f"0x{RESOLVED:x}" in nh.lower() or "538200" in nh, "resolved ret pin")
    ce = CONT_E.read_text()
    u2 = ROOT / "native/core/init_531d60_post_base_a54920_532a62_u1.hpp"
    require("kInit531d60PostBaseA54920_532a62_U1PrepRva = 0x532a1a" in u2.read_text(),
            "a54920_532a62 prep after vcall18")
    require("merge_a54920_532a62_u1_ran" in ce, "continue_e E522 merge hook")
    require("merge_vcall18_532a17_ran" in ce, "vcall merge hook")
    require("core/init_531d60_post_base_vcall18_532a17.cpp" in CMAKE.read_text(), "cmake")

    print(
        f"OK check_f531d60_post_base_vcall18_532a17: frag={FRAG_SHA256[:12]}…; "
        f"call [rax+0x18]@{VCALL:#x}→0x{RESOLVED:x}(ret elided); "
        f"end={END:#x}; stack_prep/a54920_body/callee_body NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
