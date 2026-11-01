#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue E lea rdx,[rsp+0x470] after E525."""

from __future__ import annotations

import hashlib
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
FRAG = 0x532A84
FRAG_SIZE = 8
FRAG_SHA256 = "461528923dc1ceb08ec82a9875c84ae4bdd94f89c5c136c6a3b1423b2658057d"
END = 0x532A8C
DISP = 0x470
NATIVE = ROOT / "native/core/init_531d60_post_base_lea_rsp470_532a84_u1.hpp"
CONT_E = ROOT / "native/core/init_531d60_post_base_continue_e.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_lea_5388e0_532a7d_u1.hpp"
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
        "kInit531d60PostBaseLea5388e0_532a7d_U1EndRva = 0x532a84" in PREV_NATIVE.read_text(),
        "lea_5388e0 end == lea_rsp470 start",
    )
    frag = img.read(FRAG, FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest() == FRAG_SHA256, "frag sha")
    require(frag[0:3] == bytes.fromhex("488d94"), "lea rdx,[rsp+disp32] opcode")
    disp = struct.unpack("<i", frag[4:8])[0]
    require(disp == DISP, "disp 0x470")
    require(img.read(END, 4)[0:3] == bytes.fromhex("488d35"), "post-lea lea rsi head")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_lea_rsp470_532a84_u1" in nh, "API")
    ce = CONT_E.read_text()
    require("kInit531d60PostBaseContinueERva = 0x532b0c" in ce, "shrunk continue_e start")
    require("merge_lea_rsp470_532a84_u1_ran" in ce, "lea_rsp merge hook")
    require("core/init_531d60_post_base_lea_rsp470_532a84_u1.cpp" in CMAKE.read_text(), "cmake")

    print(
        f"OK check_f531d60_post_base_lea_rsp470_532a84_u1: frag={FRAG_SHA256[:12]}…; "
        f"disp={DISP:#x}; end={END:#x}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
