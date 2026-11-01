#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue E mov [rsp+0x470],rdx after E531."""

from __future__ import annotations

import hashlib
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
FRAG = 0x532AB2
FRAG_SIZE = 8
FRAG_SHA256 = "cac7360526afa13797d46b776f807c4469acdac55b8c803039671171836c9d85"
END = 0x532ABA
DISP = 0x470
NATIVE = ROOT / "native/core/init_531d60_post_base_mov_rsp470_rdx_532ab2_u1.hpp"
CONT_E = ROOT / "native/core/init_531d60_post_base_continue_e.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_lea_rbx_rsp490_532aaa_u1.hpp"
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
        "kInit531d60PostBaseLeaRbxRsp490_532aaa_U1EndRva = 0x532ab2" in PREV_NATIVE.read_text(),
        "lea_rbx end == mov u1 start",
    )
    frag = img.read(FRAG, FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest() == FRAG_SHA256, "frag sha")
    require(frag[0:4] == bytes.fromhex("48899424"), "mov [rsp+disp],rdx opcode head")
    disp = struct.unpack("<I", frag[4:8])[0]
    require(disp == DISP, "mov disp 0x470")
    require(img.read(END, 4)[0:4] == bytes.fromhex("c5fa7f84"), "post-mov vmovdqa head")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_mov_rsp470_rdx_532ab2_u1" in nh, "API")
    ce = CONT_E.read_text()
    require("kInit531d60PostBaseContinueERva = 0x532b0c" in ce, "shrunk continue_e start")
    require("merge_mov_rsp470_rdx_532ab2_u1_ran" in ce, "mov merge hook")
    require("core/init_531d60_post_base_mov_rsp470_rdx_532ab2_u1.cpp" in CMAKE.read_text(), "cmake")

    print(
        f"OK check_f531d60_post_base_mov_rsp470_rdx_532ab2_u1: frag={FRAG_SHA256[:12]}…; "
        f"disp={DISP:#x}; end={END:#x}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
