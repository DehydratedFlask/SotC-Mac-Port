#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 merge second direct call-site after E517 a599e0 u1."""

from __future__ import annotations

import hashlib
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
PREP = 0x53294B
PREP_SIZE = 47
PREP_SHA256 = "0dfb0b4c037553fbdae12cdf0db06c004345ff1a7d0caea4f7882b292ca81062"
FRAG = 0x53297A
FRAG_SIZE = 5
FRAG_SHA256 = "0a827c78a98f188f5cc68da10edb40ff29a464d32bddcf5592ecf613b545091f"
END = 0x53297F
TARGET = 0xA431E0
NATIVE = ROOT / "native/core/init_531d60_post_base_a431e0_u1.hpp"
SLICE = ROOT / "native/core/appmain_init_ctor_98a90_slice.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_a599e0_u1.hpp"
CMAKE = ROOT / "native/CMakeLists.txt"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    require(
        "kInit531d60PostBaseA599e0U1EndRva = 0x53294b" in PREV_NATIVE.read_text(),
        "a599e0_u1 end",
    )
    require(PREP + PREP_SIZE == FRAG, "prep size")
    prep = img.read(PREP, PREP_SIZE)
    require(hashlib.sha256(prep).hexdigest() == PREP_SHA256, "prep sha")
    frag = img.read(FRAG, FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest() == FRAG_SHA256, "sha")
    require(frag[0] == 0xE8, "call opcode")
    rel = struct.unpack("<i", frag[1:5])[0]
    require(FRAG + 5 + rel == TARGET, "call target")
    require(img.read(END, 7)[0:3] == bytes.fromhex("488b05"), "next mov rax head")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_a431e0_u1" in nh, "API")
    st = SLICE.read_text()
    require("post_base_a431e0_u1" in st or "PostBaseA431e0U1" in st, "wire")
    require("core/init_531d60_post_base_a431e0_u1.cpp" in CMAKE.read_text(), "cmake")

    print(
        f"OK check_f531d60_post_base_a431e0_u1: frag={FRAG_SHA256[:12]}…; "
        f"call 0xa431e0@{FRAG:#x}; prep={PREP:#x}+{PREP_SIZE}B demoted; "
        f"end={END:#x}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
