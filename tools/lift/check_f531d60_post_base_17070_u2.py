#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue second 0x17070 call-site fragment.

Private: needs local/re/eboot.elf. Pins 5 B after E482 cell-install 0x2eff450.
Does NOT claim 0x17070 body or intervening mov edi,0x490 arg prep.
"""

from __future__ import annotations

import hashlib
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
PREV_END = 0x5323A9
FRAG = 0x5323AE
FRAG_SIZE = 5
FRAG_SHA256 = "46dd52ba06976b4c218a70292a0289e28f56345b457e63f6f94d4d674d534e40"
END = 0x5323B3
TARGET = 0x17070
ARG_PREP = 0x5323A9
ARG_PREP_SIZE = 5
ARG_EDI = 0x490
NATIVE = ROOT / "native/core/init_531d60_post_base_17070_u2.hpp"
SLICE = ROOT / "native/core/appmain_init_ctor_98a90_slice.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_cell_2eff450.hpp"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    require(PREV_END == ARG_PREP, "prev end == arg prep start")
    require(ARG_PREP + ARG_PREP_SIZE == FRAG, "arg prep size")
    require(
        "kInit531d60PostBaseCell2eff450EndRva = 0x5323a9" in PREV_NATIVE.read_text(),
        "cell_2eff450 end",
    )
    arg = img.read(ARG_PREP, ARG_PREP_SIZE)
    require(arg[0] == 0xBF, "mov edi opcode")
    require(struct.unpack("<I", arg[1:5])[0] == ARG_EDI, "edi=0x490")
    frag = img.read(FRAG, FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest() == FRAG_SHA256, "sha")
    require(frag[0] == 0xE8, "call opcode")
    rel = struct.unpack("<i", frag[1:5])[0]
    require(FRAG + 5 + rel == TARGET, "call target")
    require(img.read(END, 3) == bytes.fromhex("4889c3"), "next mov rbx,rax")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_17070_u2" in nh, "API")
    require("NOT modelled" in nh, "omission")
    require("0x17070" in nh, "target")
    st = SLICE.read_text()
    require("post_base_17070_u2" in st or "PostBase17070U2" in st, "wire")

    print(
        f"OK check_f531d60_post_base_17070_u2: frag={FRAG_SHA256[:12]}…; "
        f"call 0x17070@{FRAG:#x} (body NOT claimed); "
        f"arg_prep={ARG_PREP:#x}+{ARG_PREP_SIZE}B mov edi,{ARG_EDI:#x} demoted; "
        f"end={END:#x}; "
        f"17070_body/arg_prep/544fe0_body/vslot70_callee/532ed0_body/"
        f"vcall18_callee/532ca0_body/52eb50_body/26800_body/f590_body/"
        f"7f7220_body/1290_body/5c64c0_body/56dcb0_body/499f80_body/"
        f"140d90_body/14c2f0/6a5370 NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
