#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue first 0x532ed0 call-site fragment.

Private: needs local/re/eboot.elf. Pins 5 B after E475 fifth virtual-call +0x18.
Does NOT claim 0x532ed0 body or intervening stack prep.
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
PREV_END = 0x532285
FRAG = 0x5322F8
FRAG_SIZE = 5
FRAG_SHA256 = "5549c8cbcd72111be69565dc70a5430229a90e4d5497acf776704ff10b5462e3"
END = 0x5322FD
TARGET = 0x532ED0
STACK_PREP = 0x532285
STACK_PREP_SIZE = 115
NATIVE = ROOT / "native/core/init_531d60_post_base_532ed0.hpp"
SLICE = ROOT / "native/core/appmain_init_ctor_98a90_slice.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_vcall18_u5.hpp"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    require(PREV_END == STACK_PREP, "prev end == stack prep start")
    require(STACK_PREP + STACK_PREP_SIZE == FRAG, "prep size")
    require(
        "kInit531d60PostBaseVcall18U5EndRva = 0x532285" in PREV_NATIVE.read_text(),
        "vcall18_u5 end",
    )
    frag = img.read(FRAG, FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest() == FRAG_SHA256, "sha")
    require(frag[0] == 0xE8, "call opcode")
    rel = struct.unpack("<i", frag[1:5])[0]
    require(FRAG + 5 + rel == TARGET, "call target")
    require(img.read(END, 4) == bytes.fromhex("488b8424"), "next mov rax,[rsp+…]")
    # body is push rbp helper — claimed NOT modelled
    require(img.read(TARGET, 2) == bytes.fromhex("5548"), "body push rbp (not claimed)")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_532ed0" in nh, "API")
    require("NOT modelled" in nh, "omission")
    require("0x532ed0" in nh, "target")
    st = SLICE.read_text()
    require("post_base_532ed0" in st or "PostBase532ed0" in st, "wire")

    print(
        f"OK check_f531d60_post_base_532ed0: frag={FRAG_SHA256[:12]}…; "
        f"call 0x532ed0@{FRAG:#x} (body NOT claimed); "
        f"stack_prep={STACK_PREP:#x}+{STACK_PREP_SIZE}B demoted; "
        f"end={END:#x}; "
        f"532ed0_body/stack_prep/vcall18_callee/532ca0_body/52eb50_body/"
        f"26800_body/f590_body/7f7220_body/1290_body/5c64c0_body/"
        f"56dcb0_body/499f80_body/140d90_body/14c2f0/6a5370 NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
