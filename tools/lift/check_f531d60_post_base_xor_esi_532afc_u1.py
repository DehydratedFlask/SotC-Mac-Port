#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue E xor esi,esi @ 0x532afc after E542."""

from __future__ import annotations

import hashlib
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
FRAG = 0x532AFC
FRAG_SIZE = 2
FRAG_SHA256 = "2e2f89076640f42236c01bdc8663b50e0001b061170a88fd2ae0a99e0c7b1675"
END = 0x532AFE
NATIVE = ROOT / "native/core/init_531d60_post_base_xor_esi_532afc_u1.hpp"
CONT_E = ROOT / "native/core/init_531d60_post_base_continue_e.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_lea_rax_2eff450_532af5_u1.hpp"
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
        "kInit531d60PostBaseLeaRax2eff450_532af5_U1EndRva = 0x532afc" in PREV_NATIVE.read_text(),
        "lea_rax u1 end == xor start",
    )
    frag = img.read(FRAG, FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest() == FRAG_SHA256, "frag sha")
    require(frag == bytes.fromhex("31f6"), "xor esi,esi")
    require(img.read(END, 5)[0] == 0xBA, "post-xor mov edx head")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_xor_esi_532afc_u1" in nh, "API")
    ce = CONT_E.read_text()
    require("kInit531d60PostBaseContinueERva = 0x532afe" in ce, "shrunk continue_e start")
    require("merge_xor_esi_532afc_u1_ran" in ce, "xor merge hook")
    require("core/init_531d60_post_base_xor_esi_532afc_u1.cpp" in CMAKE.read_text(), "cmake")

    print(
        f"OK check_f531d60_post_base_xor_esi_532afc_u1: frag={FRAG_SHA256[:12]}…; "
        f"xor esi,esi@{FRAG:#x}; end={END:#x}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
