#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue call-site for 0x56dcb0.

Private: needs local/re/eboot.elf. Pins 5 B fragment after post-base 0x499f80.
Does NOT claim body of 0x56dcb0 (126 B) or later Init continues.
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
FRAG = 0x531DF4
FRAG_SIZE = 5
FRAG_SHA256 = "f05f5ca1438a59b2280791b6df9e40b6e8c8a45ee1a1cdcca2e7c678e53f3cd6"
END = 0x531DF9
TARGET = 0x56DCB0
BODY_SIZE = 126
BODY_SHA256 = "195d7ee2edf3d17815d43f5fea45b7233b8354fade52f9ac562463edf4e564ea"
NATIVE = ROOT / "native/core/init_531d60_post_base_56dcb0.hpp"
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

    require(call_target(0x531DEF, img) == 0x499F80, "prev 0x499f80")
    require(hashlib.sha256(img.read(FRAG, FRAG_SIZE)).hexdigest() == FRAG_SHA256, "sha")
    require(call_target(FRAG, img) == TARGET, "call 0x56dcb0")
    require(call_target(END, img) == 0x56DCA0, "next 0x56dca0 (E406 ret-nop)")

    body = img.read(TARGET, BODY_SIZE)
    require(hashlib.sha256(body).hexdigest() == BODY_SHA256, "body sha")
    require(body[-1:] == b"\xc3", "body ret")
    # stack-chk cell used inside body (not claimed)
    lea = img.read(TARGET + 0x0B, 7)
    require(lea[0:3] == bytes.fromhex("4c8b35"), "mov r14,[rip]")
    cell = TARGET + 0x0B + 7 + struct.unpack("<i", lea[3:7])[0]
    require(cell == 0x15FE518, "guard cell in body")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_56dcb0" in nh, "API")
    require("NOT modelled" in nh, "omission")
    require("post_base_56dcb0" in SLICE.read_text() or "PostBase56dcb0" in SLICE.read_text(), "wire")

    print(
        f"OK check_f531d60_post_base_56dcb0: frag={FRAG_SHA256[:12]}…; "
        f"call {TARGET:#x}; end={END:#x}; "
        f"56dcb0_body/5c64c0/499f80_body/140d90_body/14c2f0/6a5370 NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
