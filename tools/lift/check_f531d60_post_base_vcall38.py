#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue virtual-call +0x38 fragment.

Private: needs local/re/eboot.elf. Pins 10 B after E485 cell-install 0x2eff448.
Does NOT claim callee body or following lea r13.
"""

from __future__ import annotations

import hashlib
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
PREV_END = 0x5323DB
FRAG = 0x5323DB
FRAG_SIZE = 10
FRAG_SHA256 = "a7107e2de0f3c55ad5caad382e266ae30ae61d5807a86aae2ed6cb9694e869c1"
END = 0x5323E5
VCALL = 0x5323E2
NATIVE = ROOT / "native/core/init_531d60_post_base_vcall38.hpp"
SLICE = ROOT / "native/core/appmain_init_ctor_98a90_slice.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_cell_2eff448.hpp"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    require(PREV_END == FRAG, "prev end == frag start")
    require(
        "kInit531d60PostBaseCell2eff448EndRva = 0x5323db" in PREV_NATIVE.read_text(),
        "cell 2eff448 end",
    )
    frag = img.read(FRAG, FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest() == FRAG_SHA256, "sha")
    require(frag[0:4] == bytes.fromhex("498b0424"), "mov rax,[r12]")
    require(frag[4:7] == bytes.fromhex("4c89e7"), "mov rdi,r12")
    require(frag[7:10] == bytes.fromhex("ff5038"), "call [rax+0x38]")
    require(VCALL == FRAG + 7, "vcall site")
    # Next unit starts with lea r13
    require(img.read(END, 3) == bytes.fromhex("4c8d2d"), "next lea r13")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_vcall38" in nh, "API")
    require("NOT modelled" in nh, "omission")
    require("0x38" in nh and "vcall" in nh.lower(), "slot")
    st = SLICE.read_text()
    require("post_base_vcall38" in st or "PostBaseVcall38" in st, "wire")

    print(
        f"OK check_f531d60_post_base_vcall38: frag={FRAG_SHA256[:12]}…; "
        f"call [rax+0x38]@{VCALL:#x} (callee NOT claimed); "
        f"end={END:#x}; "
        f"vcall38_callee/53fd10_body/17070_body/544fe0_body/stack_prep/"
        f"52eb50_body/26800_body/f590_body/"
        f"7f7220_body/1290_body/5c64c0_body/56dcb0_body/499f80_body/"
        f"140d90_body/14c2f0/6a5370 NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
