#!/usr/bin/env python3
"""ELF-tie Init `0x531d60` continue batch D (0x53299d..0x5329ae, 17 B, 4 units).

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
RVA = 0x53299D
SIZE = 17
REGION_SHA256 = "6f4e5fa8921e53532c8ad4319ba268bd181d3874e83e4f140a4873bfccf3cb2d"
END = 0x5329AE
CELL = 0x32F79A8
CALL = 0xA473B0
NATIVE = ROOT / "native/core/init_531d60_post_base_continue_d.hpp"
SLICE = ROOT / "native/core/appmain_init_ctor_98a90_slice.hpp"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    body = img.read(RVA, SIZE)
    require(len(body) == SIZE and RVA + SIZE == END, "span arithmetic")
    require(hashlib.sha256(body).hexdigest() == REGION_SHA256, "region sha256")
    # lea rax,[rip+disp] ; xor esi,esi ; mov rdi,[rax] ; call rel32
    require(body[0:3] == bytes.fromhex("488d05"), "lea rax,[rip+..]")
    require(RVA + 7 + struct.unpack("<i", body[3:7])[0] == CELL, "lea target 0x32f79a8")
    require(body[7:9] == bytes.fromhex("31f6"), "xor esi,esi")
    require(body[9:12] == bytes.fromhex("488b38"), "mov rdi,[rax]")
    require(body[12] == 0xE8, "call opcode")
    require(RVA + 12 + 5 + struct.unpack("<i", body[13:17])[0] == CALL, "call target 0xa473b0")

    # The block is the fall-through of batch C's je; the branch target starts at END.
    c_end = 0x53299D
    require("kInit531d60PostBaseContinueCEndRva = 0x53299d" in
            (ROOT / "native/core/init_531d60_post_base_continue_c.hpp").read_text(),
            "batch C end matches this entry")
    require(img.read(END, 7)[0:3] == bytes.fromhex("488d05"), "END is the branch-target block")

    header = NATIVE.read_text()
    for token in (
        "kInit531d60PostBaseContinueDRva = 0x53299d",
        "kInit531d60PostBaseContinueDSize = 17",
        "kInit531d60PostBaseContinueDEndRva = 0x5329ae",
        "kInit531d60PostBaseContinueDCellRva = 0x32f79a8",
        "kInit531d60PostBaseContinueDCallRva = 0xa473b0",
        "NOT modelled",
    ):
        require(token in header, f"header token {token!r}")
    require("post_base_continue_d" in SLICE.read_text(), "slice wiring")

    print(
        f"OK check_f531d60_post_base_continue_d: span=0x{RVA:x}+{SIZE}B units=4; "
        f"lea->0x{CELL:x}; call->0x{CALL:x}; end=0x{END:x}; "
        f"a473b0_body/0x5329ae_block/cell_value/je_outcome NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
