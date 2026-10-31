#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue grouped run 0x53277a..0x53277f (5 B / 2 units)."""

from __future__ import annotations

import hashlib
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
PREV_END = 0x53277A
START = 0x53277A
SIZE = 5
END = 0x53277F
REGION_SHA256 = "1d144054591cfb4bcb510defedc711f26ac083368a5fa080af23c794263e9e86"
BRANCH_RVA = 0x53277D
BRANCH_TARGET = 0x5327BD

UNITS = [
    (0x53277A, 3, "6f4286cdf1bc8e242708e3bcf98649612b6c89649997a1fa0a5af953103bcea1",
     "test rax,rax", "pin", None),
    (0x53277D, 2, "17dc64d49bbde7c8a2294fe27e6b08751cd4171d0d72b282e2698ad53402dc46",
     "je 0x5327bd", "je", BRANCH_TARGET),
]

NATIVE = ROOT / "native/core/init_531d60_post_base_continue_a.hpp"
NATIVE_CPP = ROOT / "native/core/init_531d60_post_base_continue_a.cpp"
SLICE = ROOT / "native/core/appmain_init_ctor_98a90_slice.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_1fc80_u1.hpp"
TEST = ROOT / "native/tests/test_appmain_init_ctor_98a90_slice.cpp"
CMAKE = ROOT / "native/CMakeLists.txt"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    require(len(UNITS) == 2, "unit count")
    require(PREV_END == START, "prev end == start")
    require(
        "kInit531d60PostBase1fc80U1EndRva = 0x53277a" in PREV_NATIVE.read_text(),
        "1fc80_u1 end",
    )
    require(START + SIZE == END, "size arithmetic")

    cursor = START
    for rva, size, sha, mnemonic, kind, target in UNITS:
        require(rva == cursor, f"contiguity at 0x{rva:x} (expected 0x{cursor:x})")
        raw = img.read(rva, size)
        require(hashlib.sha256(raw).hexdigest() == sha, f"sha mismatch at 0x{rva:x}")
        if kind == "je":
            require(raw[0] == 0x74, f"je opcode at 0x{rva:x}")
            require(rva + 2 + struct.unpack("<b", raw[1:2])[0] == target,
                    f"je target at 0x{rva:x}")
        cursor = rva + size
    require(cursor == END, "units cover the claimed span")

    require(hashlib.sha256(img.read(START, SIZE)).hexdigest() == REGION_SHA256,
            "region sha")
    require(img.read(BRANCH_TARGET, 4) == bytes.fromhex("488d05ac"), "branch target head")

    nh = NATIVE.read_text()
    require("kInit531d60PostBaseContinueARva = 0x53277a" in nh, "start rva")
    require("post_base_continue_a" in SLICE.read_text(), "slice wire")
    require("post_base_1fc80_u1" in SLICE.read_text(), "u1 wire")
    require("branch_outcome_modelled = false" in NATIVE_CPP.read_text(), "branch flag")
    require("pbca_units" in TEST.read_text(), "test wire")

    print(
        f"OK check_f531d60_post_base_continue_a: span=0x{START:x}+{SIZE}B "
        f"units={len(UNITS)} region={REGION_SHA256[:12]}…; "
        f"je@{BRANCH_RVA:#x}->{BRANCH_TARGET:#x}; end={END:#x}; "
        f"1fc80_body/0x53277f_block/0x5327bd_block NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
