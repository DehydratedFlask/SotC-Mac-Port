#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue call-site for 0xf90310.

Private: needs local/re/eboot.elf. Pins 5 B fragment after 0x56fad0.
Target 0xf90310 is splash-hide PLT (jmp [GOT]). Does NOT claim PLT/Sony body
or following ymm Init continues / helper bodies.
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
PREV = 0x531E41
FRAG = 0x531E46
FRAG_SIZE = 5
FRAG_SHA256 = "09049fbb1ca27f53b2693c5ca05f871d6a16eea9c73c2bcfb26b66b4190f9afd"
END = 0x531E4B
TARGET = 0xF90310
NATIVE = ROOT / "native/core/init_531d60_post_base_f90310.hpp"
SLICE = ROOT / "native/core/appmain_init_ctor_98a90_slice.hpp"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def call_target(site: int, img: d.Image) -> int:
    b = img.read(site, 5)
    require(b[0] == 0xE8, f"missing call at {site:#x}")
    rel = struct.unpack("<i", b[1:5])[0]
    return site + 5 + rel


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    require(call_target(PREV, img) == 0x56FAD0, "prev 0x56fad0")
    require(img.read(0x56FAD0, 1) == b"\xc3", "prev target ret")
    require(hashlib.sha256(img.read(FRAG, FRAG_SIZE)).hexdigest() == FRAG_SHA256, "sha")
    require(call_target(FRAG, img) == TARGET, "call 0xf90310")
    # PLT: ff 25 disp32
    plt = img.read(TARGET, 6)
    require(plt[0:2] == bytes.fromhex("ff25"), "0xf90310 is PLT jmp [GOT]")
    # Next is vmovups ymm, not a call
    nxt = img.read(END, 4)
    require(nxt[0:3] == bytes.fromhex("c5fc10"), "next vmovups ymm (not claimed)")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_f90310" in nh, "API")
    require("NOT modelled" in nh, "omission")
    st = SLICE.read_text()
    require("post_base_f90310" in st or "PostBaseF90310" in st, "wire")

    print(
        f"OK check_f531d60_post_base_f90310: frag={FRAG_SHA256[:12]}…; "
        f"call {TARGET:#x} (splash-hide PLT; body NOT claimed); end={END:#x}; "
        f"f90310_plt_body/ymm_continue/7f7220_body/1290_body/5c64c0_body/56dcb0_body/"
        f"499f80_body/140d90_body/14c2f0/6a5370 NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
