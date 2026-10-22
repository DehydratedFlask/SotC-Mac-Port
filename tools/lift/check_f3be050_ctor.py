#!/usr/bin/env python3
"""ELF-tie heavy ctor 0x3be050 header fragment for 0x98a90 object.

Private: needs local/re/eboot.elf. Pins full ctor span to first ret and the
header through call 0x3be930. Does NOT claim bodies of 0x3be930 / 0x323d0 /
0x3c2f70 or remainder from 0x3be0f8.
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
CTOR = 0x3BE050
CTOR_SIZE = 627  # through ret at 0x3be2c2
CTOR_SHA256 = "f186db1c213f27794ad55b543ca805d0eb60e164ed975dee84de41f42b51e354"
HEADER_SIZE = 102  # to call 0x3be930
HEADER_SHA256 = "74b2e5e4d57326262078f5ae77855f70c6f89eb814faec3f6f172e9b156c747c"
NATIVE = ROOT / "native/core/ctor_98a90_3be050.hpp"
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

    body = img.read(CTOR, CTOR_SIZE)
    require(hashlib.sha256(body).hexdigest() == CTOR_SHA256, "ctor sha")
    require(body[-1] == 0xC3, "ctor ends ret")
    # Next function prologue immediately after nops
    require(img.read(0x3BE2D0, 2) == bytes.fromhex("5548"), "next fn at 0x3be2d0")

    hdr = img.read(CTOR, HEADER_SIZE)
    require(hashlib.sha256(hdr).hexdigest() == HEADER_SHA256, "header sha")

    require(img.read(0x3BE068, 5) == bytes.fromhex("bf48000000"), "mov edi, 0x48")
    require(call_target(0x3BE073, img) == 0x17070, "alloc nested")
    require(img.read(0x3BE078, 4) == bytes.fromhex("498d7e68"), "lea rdi,[r14+0x68]")
    require(call_target(0x3BE0B6, img) == 0x3BE930, "call 0x3be930")
    require(img.read(0x3BE0BB, 7) == bytes.fromhex("498dbe888a0900"), "lea +0x98a88")
    require(call_target(0x3BE0C2, img) == 0x323D0, "call 0x323d0")
    require(call_target(0x3BE0F3, img) == 0x3C2F70, "call 0x3c2f70 ensure")
    require(img.read(0x3BE112, 5) == bytes.fromhex("bf80000000"), "remainder mov edi,0x80")

    nh = NATIVE.read_text()
    require("init_98a90_3be050" in nh, "API")
    require("NOT modelled" in nh or "not modelled" in nh.lower(), "omission")
    require("0x3be0f8" in nh or "remainder" in nh.lower(), "remainder omission")
    require("run_appmain_init_ctor_98a90_slice" in SLICE.read_text(), "slice API")

    print(
        f"OK check_f3be050_ctor: body={CTOR_SHA256[:12]}… ({CTOR_SIZE} B); "
        f"header={HEADER_SHA256[:12]}…; nested 0x48+hooks pinned; "
        f"0x3be930/0x323d0/0x3c2f70/remainder NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
