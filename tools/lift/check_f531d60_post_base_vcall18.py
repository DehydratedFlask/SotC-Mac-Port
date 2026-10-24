#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue virtual-call +0x18 fragment.

Private: needs local/re/eboot.elf. Pins 14 B after E466 0x532ca0 call-site.
Does NOT claim callee body or later 0x532ca0 units.
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
PREV_END = 0x5320B1
FRAG = 0x5320B1
FRAG_SIZE = 14
FRAG_SHA256 = "637a53d374d8b1f442035146a06bedf348723c76e3ab9d50c9d812e8799a6a78"
END = 0x5320BF
VCALL = 0x5320BC
NATIVE = ROOT / "native/core/init_531d60_post_base_vcall18.hpp"
SLICE = ROOT / "native/core/appmain_init_ctor_98a90_slice.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_532ca0.hpp"


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
        "kInit531d60PostBase532ca0EndRva = 0x5320b1" in PREV_NATIVE.read_text(),
        "532ca0 end",
    )
    frag = img.read(FRAG, FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest() == FRAG_SHA256, "sha")
    require(frag[0:4] == bytes.fromhex("488b8424"), "mov rax,[rsp+…]")
    require(struct.unpack("<I", frag[4:8])[0] == 0xC10, "rsp+0xc10")
    require(frag[8:11] == bytes.fromhex("4c89f7"), "mov rdi,r14")
    require(frag[11:14] == bytes.fromhex("ff5018"), "call [rax+0x18]")
    require(VCALL == FRAG + 11, "vcall site")
    # Next unit starts with mov rdi,[rbx]
    require(img.read(END, 3) == bytes.fromhex("488b3b"), "next mov rdi,[rbx]")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_vcall18" in nh, "API")
    require("NOT modelled" in nh, "omission")
    require("0x18" in nh and "vcall" in nh.lower(), "slot")
    st = SLICE.read_text()
    require("post_base_vcall18" in st or "PostBaseVcall18" in st, "wire")

    print(
        f"OK check_f531d60_post_base_vcall18: frag={FRAG_SHA256[:12]}…; "
        f"call [rax+0x18]@{VCALL:#x} (callee NOT claimed); "
        f"end={END:#x}; "
        f"532ca0_body/stack_prep/52eb50_body/26800_body/f590_body/"
        f"7f7220_body/1290_body/5c64c0_body/56dcb0_body/499f80_body/"
        f"140d90_body/14c2f0/6a5370 NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
