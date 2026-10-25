#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue first 0xa2caf0 call-site fragment.

Private: needs local/re/eboot.elf. Pins 5 B after E487 lea r13 pin 0x32f4fe0.
Does NOT claim 0xa2caf0 body or intervening mov rdi,[r13] prep.
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
PREV_END = 0x5323EC
PREP = 0x5323EC
PREP_SIZE = 4
FRAG = 0x5323F0
FRAG_SIZE = 5
FRAG_SHA256 = "8ca5682667323bb0e72506beb12bbdb662477476698ceac795fa5b829a11144f"
END = 0x5323F5
TARGET = 0xA2CAF0
NATIVE = ROOT / "native/core/init_531d60_post_base_a2caf0.hpp"
SLICE = ROOT / "native/core/appmain_init_ctor_98a90_slice.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_lea_32f4fe0.hpp"


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
        "kInit531d60PostBaseLea32f4fe0EndRva = 0x5323ec" in PREV_NATIVE.read_text(),
        "lea 32f4fe0 end",
    )
    prep = img.read(PREP, PREP_SIZE)
    require(prep == bytes.fromhex("498b7d00"), "mov rdi,[r13]")
    frag = img.read(FRAG, FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest() == FRAG_SHA256, "sha")
    require(frag[0] == 0xE8, "call opcode")
    rel = struct.unpack("<i", frag[1:5])[0]
    require(FRAG + 5 + rel == TARGET, "call target")
    require(img.read(END, 4) == bytes.fromhex("498b7d00"), "next mov rdi,[r13]")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_a2caf0" in nh, "API")
    require("NOT modelled" in nh, "omission")
    require("0xa2caf0" in nh, "target")
    st = SLICE.read_text()
    require("post_base_a2caf0" in st or "PostBaseA2caf0" in st, "wire")

    print(
        f"OK check_f531d60_post_base_a2caf0: frag={FRAG_SHA256[:12]}…; "
        f"call 0xa2caf0@{FRAG:#x} (body NOT claimed); "
        f"prep={PREP:#x}+{PREP_SIZE}B mov rdi,[r13] demoted; "
        f"end={END:#x}; "
        f"a2caf0_body/mov_rdi_prep/vcall38_callee/53fd10_body/17070_body/"
        f"544fe0_body/vslot70_callee/532ed0_body/vcall18_callee/"
        f"532ca0_body/52eb50_body/26800_body/f590_body/7f7220_body/"
        f"1290_body/5c64c0_body/56dcb0_body/499f80_body/140d90_body/"
        f"14c2f0/6a5370 NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
