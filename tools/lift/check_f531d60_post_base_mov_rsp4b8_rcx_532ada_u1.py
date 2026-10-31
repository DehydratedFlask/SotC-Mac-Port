#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue E mov [rsp+0x4B8],rcx after E536."""

from __future__ import annotations

import hashlib
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
FRAG = 0x532ADA
FRAG_SIZE = 8
FRAG_SHA256 = "4cb548c13bf8f1664150ce0f13ed638f3714e8becf4cb4761ea5dfa0b7a08ba3"
END = 0x532AE2
DISP = 0x4B8
NATIVE = ROOT / "native/core/init_531d60_post_base_mov_rsp4b8_rcx_532ada_u1.hpp"
CONT_E = ROOT / "native/core/init_531d60_post_base_continue_e.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_lea_rsi_2efda50_532ad3_u1.hpp"
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
        "kInit531d60PostBaseLeaRsi2efda50_532ad3_U1EndRva = 0x532ada" in PREV_NATIVE.read_text(),
        "lea u1 end == mov u1 start",
    )
    frag = img.read(FRAG, FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest() == FRAG_SHA256, "frag sha")
    require(frag[0:4] == bytes.fromhex("48898c24"), "mov [rsp+disp],rcx head")
    disp = struct.unpack("<I", frag[4:8])[0]
    require(disp == DISP, "mov disp 0x4b8")
    require(img.read(END, 1)[0:1] == bytes.fromhex("e8"), "post-mov call 0xa54920 head")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_mov_rsp4b8_rcx_532ada_u1" in nh, "API")
    ce = CONT_E.read_text()
    require("kInit531d60PostBaseContinueERva = 0x532ae2" in ce, "shrunk continue_e start")
    require("merge_mov_rsp4b8_rcx_532ada_u1_ran" in ce, "mov4b8 merge hook")
    require(
        "core/init_531d60_post_base_mov_rsp4b8_rcx_532ada_u1.cpp" in CMAKE.read_text(),
        "cmake",
    )

    print(
        f"OK check_f531d60_post_base_mov_rsp4b8_rcx_532ada_u1: frag={FRAG_SHA256[:12]}…; "
        f"disp={DISP:#x}; end={END:#x}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
