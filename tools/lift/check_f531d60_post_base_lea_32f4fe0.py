#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue lea r13 pin at 0x32f4fe0.

Private: needs local/re/eboot.elf. Pins 7 B after E486 vcall [rax+0x38].
Does NOT claim mov rdi,[r13] / call 0xa2caf0 body.
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
PREV_END = 0x5323E5
FRAG = 0x5323E5
FRAG_SIZE = 7
FRAG_SHA256 = "b0ec43085623c15841398d4f0246ce89cadb0cca78b998b9578af39c2db13eed"
END = 0x5323EC
CELL = 0x32F4FE0
NATIVE = ROOT / "native/core/init_531d60_post_base_lea_32f4fe0.hpp"
SLICE = ROOT / "native/core/appmain_init_ctor_98a90_slice.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_vcall38.hpp"


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
        "kInit531d60PostBaseVcall38EndRva = 0x5323e5" in PREV_NATIVE.read_text(),
        "vcall38 end",
    )
    frag = img.read(FRAG, FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest() == FRAG_SHA256, "sha")
    require(frag[:3] == bytes.fromhex("4c8d2d"), "lea r13")
    disp = struct.unpack("<i", frag[3:7])[0]
    require(FRAG + 7 + disp == CELL, "lea cell 0x32f4fe0")
    # Next: mov rdi,[r13]
    require(img.read(END, 4) == bytes.fromhex("498b7d00"), "next mov rdi,[r13]")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_lea_32f4fe0" in nh, "API")
    require("NOT modelled" in nh, "omission")
    require("0x32f4fe0" in nh, "cell")
    st = SLICE.read_text()
    require("post_base_lea_32f4fe0" in st or "PostBaseLea32f4fe0" in st, "wire")

    print(
        f"OK check_f531d60_post_base_lea_32f4fe0: frag={FRAG_SHA256[:12]}…; "
        f"lea r13,[0x32f4fe0]@{FRAG:#x}; end={END:#x}; "
        f"a2caf0_body/mov_rdi_load/vcall38_callee/53fd10_body/17070_body/"
        f"stack_prep/52eb50_body/26800_body/f590_body/"
        f"7f7220_body/1290_body/5c64c0_body/56dcb0_body/499f80_body/"
        f"140d90_body/14c2f0/6a5370 NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
