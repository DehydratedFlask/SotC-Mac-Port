#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue second 0xa2ed00 call-site fragment.

Private: needs local/re/eboot.elf. Pins 5 B after E491 second post-a2ed00 0xf590.
Does NOT claim 0xa2ed00 body or intervening 97 B stack prep.
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
PREV_END = 0x532472
PREP = 0x532472
PREP_SIZE = 97
PREP_SHA256 = "850099cd8c3e776bfc99047a6fd1086560e65137678f8bb69ff322582918d64d"
FRAG = 0x5324D3
FRAG_SIZE = 5
FRAG_SHA256 = "d3e8dbc62fab573c58ac27f7aecd040b79c1c95529b4451da2fa592e4b7c8f60"
END = 0x5324D8
TARGET = 0xA2ED00
NATIVE = ROOT / "native/core/init_531d60_post_base_a2ed00_u2.hpp"
SLICE = ROOT / "native/core/appmain_init_ctor_98a90_slice.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_f590_a2ed00_u2.hpp"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    require(PREV_END == PREP, "prev end == prep start")
    require(PREP + PREP_SIZE == FRAG, "prep size")
    require(
        "kInit531d60PostBaseF590A2ed00U2EndRva = 0x532472" in PREV_NATIVE.read_text(),
        "f590_a2ed00_u2 end",
    )
    prep = img.read(PREP, PREP_SIZE)
    require(hashlib.sha256(prep).hexdigest() == PREP_SHA256, "prep sha")
    require(prep[:4] == bytes.fromhex("498b7d00"), "prep mov rdi,[r13]")
    require(prep[4:7] == bytes.fromhex("4c8dbc"), "prep lea r15")
    frag = img.read(FRAG, FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest() == FRAG_SHA256, "sha")
    require(frag[0] == 0xE8, "call opcode")
    rel = struct.unpack("<i", frag[1:5])[0]
    require(FRAG + 5 + rel == TARGET, "call target")
    require(img.read(END, 3) == bytes.fromhex("4889df"), "next mov rdi,rbx")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_a2ed00_u2" in nh, "API")
    require("NOT modelled" in nh, "omission")
    require("0xa2ed00" in nh, "target")
    st = SLICE.read_text()
    require("post_base_a2ed00_u2" in st or "PostBaseA2ed00U2" in st, "wire")

    print(
        f"OK check_f531d60_post_base_a2ed00_u2: frag={FRAG_SHA256[:12]}…; "
        f"call 0xa2ed00@{FRAG:#x} (body NOT claimed); "
        f"prep={PREP:#x}+{PREP_SIZE}B stack prep demoted; "
        f"end={END:#x}; "
        f"a2ed00_body/stack_prep/a2caf0_body/f590_body/vcall38_callee/"
        f"53fd10_body/17070_body/544fe0_body/vslot70_callee/532ed0_body/"
        f"vcall18_callee/532ca0_body/52eb50_body/26800_body/"
        f"7f7220_body/1290_body/5c64c0_body/56dcb0_body/499f80_body/"
        f"140d90_body/14c2f0/6a5370 NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
