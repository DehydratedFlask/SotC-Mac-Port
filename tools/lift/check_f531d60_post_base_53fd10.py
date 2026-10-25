#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue first 0x53fd10 call-site fragment.

Private: needs local/re/eboot.elf. Pins 5 B after E483 second 0x17070 call-site.
Does NOT claim 0x53fd10 body or intervening mov rbx,rax / mov rdi,rbx prep.
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
PREV_END = 0x5323B3
PREP = 0x5323B3
PREP_SIZE = 6
FRAG = 0x5323B9
FRAG_SIZE = 5
FRAG_SHA256 = "944b93e9b570fe0bdc8482562c0e753a7de31faadc5e21fcb21c79b6b2cb5b6d"
END = 0x5323BE
TARGET = 0x53FD10
NATIVE = ROOT / "native/core/init_531d60_post_base_53fd10.hpp"
SLICE = ROOT / "native/core/appmain_init_ctor_98a90_slice.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_17070_u2.hpp"


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
        "kInit531d60PostBase17070U2EndRva = 0x5323b3" in PREV_NATIVE.read_text(),
        "17070_u2 end",
    )
    prep = img.read(PREP, PREP_SIZE)
    require(prep == bytes.fromhex("4889c34889df"), "mov rbx,rax; mov rdi,rbx")
    frag = img.read(FRAG, FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest() == FRAG_SHA256, "sha")
    require(frag[0] == 0xE8, "call opcode")
    rel = struct.unpack("<i", frag[1:5])[0]
    require(FRAG + 5 + rel == TARGET, "call target")
    require(img.read(END, 7)[:3] == bytes.fromhex("4c8d35"), "next lea r14")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_53fd10" in nh, "API")
    require("NOT modelled" in nh, "omission")
    require("0x53fd10" in nh, "target")
    st = SLICE.read_text()
    require("post_base_53fd10" in st or "PostBase53fd10" in st, "wire")

    print(
        f"OK check_f531d60_post_base_53fd10: frag={FRAG_SHA256[:12]}…; "
        f"call 0x53fd10@{FRAG:#x} (body NOT claimed); "
        f"prep={PREP:#x}+{PREP_SIZE}B mov rbx,rax;mov rdi,rbx demoted; "
        f"end={END:#x}; "
        f"53fd10_body/mov_rbx_prep/17070_body/544fe0_body/vslot70_callee/"
        f"532ed0_body/vcall18_callee/532ca0_body/52eb50_body/26800_body/"
        f"f590_body/7f7220_body/1290_body/5c64c0_body/56dcb0_body/"
        f"499f80_body/140d90_body/14c2f0/6a5370 NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
