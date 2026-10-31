#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 merge first 0xa52b70 call-site after continue_b jmp to 0x532904."""

from __future__ import annotations

import hashlib
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
PREV_END = 0x5327B1
MERGE_JMP = 0x5327B1
MERGE_JMP_SHA256 = "fb89143aa6398bfc034e4b05d911ebfd8f0056e5c01669513eb3f7a603b56598"
PREP = 0x532904
PREP_SIZE = 7
PREP_SHA256 = "cfb21a9c3093d56024cab55d016fb752aae01eb71b761eaeb69622c4c0546a09"
FRAG = 0x53290B
FRAG_SIZE = 5
FRAG_SHA256 = "c6b65f41259ea6fa7075a4c35e88a4443b6ffbecfdc08a87eced18d5f70f936a"
END = 0x532910
TARGET = 0xA52B70
NATIVE = ROOT / "native/core/init_531d60_post_base_a52b70_u1.hpp"
SLICE = ROOT / "native/core/appmain_init_ctor_98a90_slice.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_4c4950_u1.hpp"
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
        "kInit531d60PostBase4c4950U1EndRva = 0x5327a5" in PREV_NATIVE.read_text(),
        "4c4950_u1 end",
    )
    jmp = img.read(MERGE_JMP, 5)
    require(hashlib.sha256(jmp).hexdigest() == MERGE_JMP_SHA256, "merge jmp sha")
    rel = struct.unpack("<i", jmp[1:5])[0]
    require(MERGE_JMP + 5 + rel == PREP, "fall-through jmp lands at merge prep")

    require(PREP + PREP_SIZE == FRAG, "prep size")
    prep = img.read(PREP, PREP_SIZE)
    require(hashlib.sha256(prep).hexdigest() == PREP_SHA256, "prep sha")
    frag = img.read(FRAG, FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest() == FRAG_SHA256, "sha")
    require(frag[0] == 0xE8, "call opcode")
    rel = struct.unpack("<i", frag[1:5])[0]
    require(FRAG + 5 + rel == TARGET, "call target")
    require(img.read(END, 7)[0:3] == bytes.fromhex("488d05"), "next lea rax head")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_a52b70_u1" in nh, "API")
    st = SLICE.read_text()
    require("post_base_a52b70_u1" in st or "PostBaseA52b70U1" in st, "wire")
    require("core/init_531d60_post_base_a52b70_u1.cpp" in CMAKE.read_text(), "cmake")

    print(
        f"OK check_f531d60_post_base_a52b70_u1: frag={FRAG_SHA256[:12]}…; "
        f"call 0xa52b70@{FRAG:#x}; prep={PREP:#x}+{PREP_SIZE}B demoted; "
        f"merge_jmp@{MERGE_JMP:#x}; end={END:#x}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
