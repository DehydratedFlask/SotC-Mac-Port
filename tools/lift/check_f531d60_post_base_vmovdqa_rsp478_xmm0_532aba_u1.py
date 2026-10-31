#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue E vmovdqa [rsp+0x478],xmm0 after E532."""

from __future__ import annotations

import hashlib
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
FRAG = 0x532ABA
FRAG_SIZE = 9
FRAG_SHA256 = "35346c53edb6caf2e4dce695c58808ed3e279f889d534ab9ba6f697e34fe9bc1"
END = 0x532AC3
DISP = 0x478
NATIVE = ROOT / "native/core/init_531d60_post_base_vmovdqa_rsp478_xmm0_532aba_u1.hpp"
CONT_E = ROOT / "native/core/init_531d60_post_base_continue_e.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_mov_rsp470_rdx_532ab2_u1.hpp"
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
        "kInit531d60PostBaseMovRsp470Rdx_532ab2_U1EndRva = 0x532aba" in PREV_NATIVE.read_text(),
        "mov u1 end == vmovdqa u1 start",
    )
    frag = img.read(FRAG, FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest() == FRAG_SHA256, "frag sha")
    require(frag[0:4] == bytes.fromhex("c5fa7f84"), "vmovdqa opcode head")
    disp = struct.unpack("<I", frag[5:9])[0]
    require(disp == DISP, "vmovdqa disp 0x478")
    require(img.read(END, 4)[0:2] == bytes.fromhex("4889"), "post-vmovdqa mov [rsp+disp] head")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_vmovdqa_rsp478_xmm0_532aba_u1" in nh, "API")
    ce = CONT_E.read_text()
    require("kInit531d60PostBaseContinueERva = 0x532ac3" in ce, "shrunk continue_e start")
    require("merge_vmovdqa_rsp478_xmm0_532aba_u1_ran" in ce, "vmovdqa merge hook")
    require("core/init_531d60_post_base_vmovdqa_rsp478_xmm0_532aba_u1.cpp" in CMAKE.read_text(), "cmake")

    print(
        f"OK check_f531d60_post_base_vmovdqa_rsp478_xmm0_532aba_u1: frag={FRAG_SHA256[:12]}…; "
        f"disp={DISP:#x}; end={END:#x}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
