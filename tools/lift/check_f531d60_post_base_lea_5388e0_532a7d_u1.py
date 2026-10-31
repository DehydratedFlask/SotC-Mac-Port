#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue E lea rax @ 0x5388e0 after E524."""

from __future__ import annotations

import hashlib
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
FRAG = 0x532A7D
FRAG_SIZE = 7
FRAG_SHA256 = "8ed7d0f04cc53d86379785218e047d03c6415d6038a62558fe1d66d3ef9cc12b"
END = 0x532A84
TARGET = 0x5388E0
NATIVE = ROOT / "native/core/init_531d60_post_base_lea_5388e0_532a7d_u1.hpp"
CONT_E = ROOT / "native/core/init_531d60_post_base_continue_e.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_vmovq_532a75_u1.hpp"
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
        "kInit531d60PostBaseVmovq532a75_U1EndRva = 0x532a7d" in PREV_NATIVE.read_text(),
        "vmovq end == lea start",
    )
    frag = img.read(FRAG, FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest() == FRAG_SHA256, "frag sha")
    require(frag[0:2] == bytes.fromhex("488d"), "lea rax opcode")
    modrm = frag[2]
    require((modrm & 0xC7) == 0x05, "lea rip-rel shape")
    rel = struct.unpack("<i", frag[3:7])[0]
    require(FRAG + 7 + rel == TARGET, "lea target 0x5388e0")
    require(img.read(END, 4)[0:2] == bytes.fromhex("488d"), "post-lea lea rdx head")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_lea_5388e0_532a7d_u1" in nh, "API")
    ce = CONT_E.read_text()
    require("kInit531d60PostBaseContinueERva = 0x532a93" in ce, "shrunk continue_e start")
    require("merge_lea_5388e0_532a7d_u1_ran" in ce, "lea merge hook")
    require("core/init_531d60_post_base_lea_5388e0_532a7d_u1.cpp" in CMAKE.read_text(), "cmake")

    print(
        f"OK check_f531d60_post_base_lea_5388e0_532a7d_u1: frag={FRAG_SHA256[:12]}…; "
        f"lea→{TARGET:#x}; end={END:#x}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
