#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue E lea rsi,[0x2efda50] after E535."""

from __future__ import annotations

import hashlib
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
FRAG = 0x532AD3
FRAG_SIZE = 7
FRAG_SHA256 = "789228dafb710348d35a950b268d0872ae9833bc09f169b1f842bb4e8dfe352c"
END = 0x532ADA
TARGET = 0x2EFDA50
NATIVE = ROOT / "native/core/init_531d60_post_base_lea_rsi_2efda50_532ad3_u1.hpp"
CONT_E = ROOT / "native/core/init_531d60_post_base_continue_e.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_mov_rsp4b0_rsi_532acb_u1.hpp"
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
        "kInit531d60PostBaseMovRsp4b0_Rsi_532acb_U1EndRva = 0x532ad3" in PREV_NATIVE.read_text(),
        "mov4b0 u1 end == lea start",
    )
    frag = img.read(FRAG, FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest() == FRAG_SHA256, "frag sha")
    require(frag[0:3] == bytes.fromhex("488d35"), "lea rsi,[rip+disp32] opcode")
    rel = struct.unpack("<i", frag[3:7])[0]
    require(FRAG + 7 + rel == TARGET, "lea target 0x2efda50")
    require(img.read(END, 4)[0:4] == bytes.fromhex("48898c24"), "post-lea mov [rsp+disp],rcx head")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_lea_rsi_2efda50_532ad3_u1" in nh, "API")
    ce = CONT_E.read_text()
    require("kInit531d60PostBaseContinueERva = 0x532ada" in ce, "shrunk continue_e start")
    require("merge_lea_rsi_2efda50_532ad3_u1_ran" in ce, "lea merge hook")
    require(
        "core/init_531d60_post_base_lea_rsi_2efda50_532ad3_u1.cpp" in CMAKE.read_text(),
        "cmake",
    )

    print(
        f"OK check_f531d60_post_base_lea_rsi_2efda50_532ad3_u1: frag={FRAG_SHA256[:12]}…; "
        f"lea→{TARGET:#x}; end={END:#x}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
