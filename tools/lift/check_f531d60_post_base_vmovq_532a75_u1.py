#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue E vmovq @ 0x532a75 after E523."""

from __future__ import annotations

import hashlib
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
FRAG = 0x532A75
FRAG_SIZE = 8
FRAG_SHA256 = "f970e4d743bedbd28f18045f6cd2d6c61d1367f844519a5fe55646970144c670"
END = 0x532A7D
SRC = 0x15FF528
NATIVE = ROOT / "native/core/init_531d60_post_base_vmovq_532a75_u1.hpp"
CONT_E = ROOT / "native/core/init_531d60_post_base_continue_e.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_vcall18_532a72_u1.hpp"
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
        "kInit531d60PostBaseVcall18_532a72_U1EndRva = 0x532a75" in PREV_NATIVE.read_text(),
        "vcall72 end == vmovq start",
    )
    frag = img.read(FRAG, FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest() == FRAG_SHA256, "frag sha")
    require(frag[0:4] == bytes.fromhex("c5fa7e05"), "vmovq opcode")
    rel = struct.unpack("<i", frag[4:8])[0]
    require(FRAG + 8 + rel == SRC, "vmovq target 0x15ff528")
    require(img.read(END, 4)[0:2] == bytes.fromhex("488d"), "post-call lea head")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_vmovq_532a75_u1" in nh, "API")
    ce = CONT_E.read_text()
    require("kInit531d60PostBaseContinueERva = 0x532a7d" in ce, "shrunk continue_e start")
    require("merge_vmovq_532a75_u1_ran" in ce, "vmovq merge hook")
    require("core/init_531d60_post_base_vmovq_532a75_u1.cpp" in CMAKE.read_text(), "cmake")

    print(
        f"OK check_f531d60_post_base_vmovq_532a75_u1: frag={FRAG_SHA256[:12]}…; "
        f"vmovq→{SRC:#x}; end={END:#x}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
