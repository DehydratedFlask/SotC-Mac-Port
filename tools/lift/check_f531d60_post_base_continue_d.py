#!/usr/bin/env python3
"""ELF-tie Init `0x531d60` continue batch D boundary after E519 a473b0 u1 peel.

Private: needs local/re/eboot.elf.
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
FULL_RVA = 0x53299D
FULL_SIZE = 17
FULL_REGION_SHA256 = "6f4e5fa8921e53532c8ad4319ba268bd181d3874e83e4f140a4873bfccf3cb2d"
BOUNDARY = 0x5329AE
CELL = 0x32F79A8
CALL = 0xA473B0
NATIVE = ROOT / "native/core/init_531d60_post_base_continue_d.hpp"
NATIVE_CPP = ROOT / "native/core/init_531d60_post_base_continue_d.cpp"
U1 = ROOT / "native/core/init_531d60_post_base_a473b0_u1.hpp"
SLICE = ROOT / "native/core/appmain_init_ctor_98a90_slice.hpp"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    body = img.read(FULL_RVA, FULL_SIZE)
    require(len(body) == FULL_SIZE and FULL_RVA + FULL_SIZE == BOUNDARY, "full span")
    require(hashlib.sha256(body).hexdigest() == FULL_REGION_SHA256, "full region sha256")
    require(body[12] == 0xE8, "call opcode in full span")
    require(FULL_RVA + 12 + 5 + struct.unpack("<i", body[13:17])[0] == CALL, "call target")

    require("kInit531d60PostBaseA473b0U1EndRva = 0x5329ae" in U1.read_text(), "u1 end")
    require(img.read(BOUNDARY, 7)[0:3] == bytes.fromhex("488d05"), "boundary is batch E entry")

    header = NATIVE.read_text()
    for token in (
        "kInit531d60PostBaseContinueDRva = 0x5329ae",
        "kInit531d60PostBaseContinueDSize = 0",
        "kInit531d60PostBaseContinueDEndRva = 0x5329ae",
        "kInit531d60PostBaseContinueDFullSpanRva = 0x53299d",
        "kInit531d60PostBaseContinueDFullSpanSize = 17",
        "NOT modelled",
    ):
        require(token in header, f"header token {token!r}")
    require("init_531d60_post_base_a473b0_u1" in NATIVE_CPP.read_text(), "u1 merge")
    require("post_base_continue_d" in SLICE.read_text(), "slice wiring")

    print(
        f"OK check_f531d60_post_base_continue_d: full=0x{FULL_RVA:x}+{FULL_SIZE}B in u1; "
        f"boundary=0x{BOUNDARY:x} units=0; call->0x{CALL:x}; "
        f"0x5329ae_block/je_outcome NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
