#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue first post-a2ed00-u7 0xf590 call-site.

Private: needs local/re/eboot.elf. Pins 5 B after E507 seventh 0xa2ed00
call-site. Does NOT claim 0xf590 body or intervening mov rdi,rbx prep.
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
PREV_END = 0x532718
PREP = 0x532718
PREP_SIZE = 3
FRAG = 0x53271B
FRAG_SIZE = 5
FRAG_SHA256 = "014b54cd1f9830def273603ddd2acfe673c0907ff41c18e3baf126c149b2d2eb"
END = 0x532720
TARGET = 0xF590
NATIVE = ROOT / "native/core/init_531d60_post_base_f590_a2ed00_u13.hpp"
SLICE = ROOT / "native/core/appmain_init_ctor_98a90_slice.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_a2ed00_u7.hpp"


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
        "kInit531d60PostBaseA2ed00U7EndRva = 0x532718" in PREV_NATIVE.read_text(),
        "a2ed00_u7 end",
    )
    prep = img.read(PREP, PREP_SIZE)
    require(prep == bytes.fromhex("4889df"), "mov rdi,rbx")
    frag = img.read(FRAG, FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest() == FRAG_SHA256, "sha")
    require(frag[0] == 0xE8, "call opcode")
    rel = struct.unpack("<i", frag[1:5])[0]
    require(FRAG + 5 + rel == TARGET, "call target")
    require(img.read(END, 3) == bytes.fromhex("4c89ff"), "next mov rdi,r15")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_f590_a2ed00_u13" in nh, "API")
    require("NOT modelled" in nh, "omission")
    require("0xf590" in nh, "target")
    st = SLICE.read_text()
    require("post_base_f590_a2ed00_u13" in st or "PostBaseF590A2ed00U13" in st, "wire")

    print(
        f"OK check_f531d60_post_base_f590_a2ed00_u13: frag={FRAG_SHA256[:12]}…; "
        f"call 0xf590@{FRAG:#x} (body NOT claimed); "
        f"prep={PREP:#x}+{PREP_SIZE}B mov rdi,rbx demoted; "
        f"end={END:#x}; "
        f"f590_body/mov_rdi_prep/a2ed00_body/a2caf0_body/stack_prep/"
        f"vcall38_callee/53fd10_body/17070_body/544fe0_body/"
        f"52eb50_body/26800_body/7f7220_body/1290_body/5c64c0_body/"
        f"56dcb0_body/499f80_body/140d90_body/14c2f0/6a5370 NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
