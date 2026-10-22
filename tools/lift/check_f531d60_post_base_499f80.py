#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 post-base continue call-site for 0x499f80.

Private: needs local/re/eboot.elf. Pins 21 B fragment after Base Initialize
return. Does NOT claim body of 0x499f80 (192 B) or subsequent Init leaves.
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
FRAG = 0x531DDF
FRAG_SIZE = 21
FRAG_SHA256 = "df62ba535e269af5ce059e24463a597a27b4751bdf3607bc8679f4d24d3d0b88"
END = 0x531DF4
TARGET = 0x499F80
BODY_SIZE = 193  # through ret; identity pin only — body NOT modelled
BODY_SHA256 = "9ea23d248ce29318034c8f929f5cc435bbf7f92f57113ebf93b5b4c8e853f8ce"
NATIVE = ROOT / "native/core/init_531d60_post_base_499f80.hpp"
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

    require(call_target(0x531DDA, img) == 0x255130, "Init → base Initialize")
    require(hashlib.sha256(img.read(FRAG, FRAG_SIZE)).hexdigest() == FRAG_SHA256, "sha")
    require(img.read(FRAG, 2) == bytes.fromhex("84c0"), "test al,al")
    require(img.read(0x531DE1, 6) == bytes.fromhex("0f84cf090000"), "jz fail")
    require(img.read(0x531DE7, 8) == bytes.fromhex("498dbc24703c0000"), "lea rdi,[r12+0x3c70]")
    require(call_target(0x531DEF, img) == TARGET, "call 0x499f80")
    require(call_target(END, img) == 0x56DCB0, "next 0x56dcb0 (not claimed)")

    # Body identity pin only — NOT modelled by native lift.
    body = img.read(TARGET, BODY_SIZE)
    require(hashlib.sha256(body).hexdigest() == BODY_SHA256, "body sha")
    require(body[-1:] == b"\xc3", "body ret")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_499f80" in nh, "API")
    require("kInit531d60PostBase499f80ArgOff" in nh, "arg off")
    require("NOT modelled" in nh, "omission")
    require("post_base_499f80" in SLICE.read_text() or "PostBase499f80" in SLICE.read_text(), "wire")

    print(
        f"OK check_f531d60_post_base_499f80: frag={FRAG_SHA256[:12]}…; "
        f"arg=self+0x3c70; call {TARGET:#x}; end={END:#x}; "
        f"499f80_body/56dcb0/140d90_body/14c2f0/6a5370 NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
