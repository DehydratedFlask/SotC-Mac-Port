#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue first 0xa2ea00 call-site after E509 u14.

Private: needs local/re/eboot.elf. Pins 5 B after E509 u14 end. Does NOT claim
0xa2ea00 body or intervening mov/xor/mov prep.
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
PREV_END = 0x532728
PREP = 0x532728
PREP_SIZE = 9
FRAG = 0x532731
FRAG_SIZE = 5
FRAG_SHA256 = "a6fd5d490e9c60bc5b39745543428bc4697cababb739e9d96c956d1d841055e6"
END = 0x532736
TARGET = 0xA2EA00
NATIVE = ROOT / "native/core/init_531d60_post_base_a2ea00_u1.hpp"
SLICE = ROOT / "native/core/appmain_init_ctor_98a90_slice.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_f590_a2ed00_u14.hpp"
CMAKE = ROOT / "native/CMakeLists.txt"


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
        "kInit531d60PostBaseF590A2ed00U14EndRva = 0x532728" in PREV_NATIVE.read_text(),
        "f590_a2ed00_u14 end",
    )
    prep = img.read(PREP, PREP_SIZE)
    require(
        hashlib.sha256(prep).hexdigest()
        == "2182cb59eed434d467341bc83963eaa5378c424b1ab06eb8de25832c92104333",
        "prep sha",
    )
    frag = img.read(FRAG, FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest() == FRAG_SHA256, "sha")
    require(frag[0] == 0xE8, "call opcode")
    rel = struct.unpack("<i", frag[1:5])[0]
    require(FRAG + 5 + rel == TARGET, "call target")
    require(img.read(END, 3) == bytes.fromhex("4889df"), "next mov rdi,rbx")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_a2ea00_u1" in nh, "API")
    require("NOT modelled" in nh, "omission")
    require("0xa2ea00" in nh, "target")
    st = SLICE.read_text()
    require("post_base_a2ea00_u1" in st or "PostBaseA2ea00U1" in st, "wire")
    require("core/init_531d60_post_base_a2ea00_u1.cpp" in CMAKE.read_text(), "cmake")

    print(
        f"OK check_f531d60_post_base_a2ea00_u1: frag={FRAG_SHA256[:12]}…; "
        f"call 0xa2ea00@{FRAG:#x} (body NOT claimed); "
        f"prep={PREP:#x}+{PREP_SIZE}B demoted; "
        f"end={END:#x}; "
        f"a2ea00_body/prep_r13/xor/mov_rdi/f590_body/a2ed00_body NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
