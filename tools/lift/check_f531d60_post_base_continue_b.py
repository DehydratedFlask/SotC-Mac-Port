#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue B fall-through run 0x5327a5..0x5327b1 (12 B / 3 units)."""

from __future__ import annotations

import hashlib
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
PREV_END = 0x5327A5
START = 0x5327A5
SIZE = 12
END = 0x5327B1
REGION_SHA256 = "81bd97f0bf0e4c9a724f01955c79d4d1bdaba7a5bddb8aa572baebf5ab9c93a1"
BRANCH_RVA = 0x5327AB
BRANCH_TARGET = 0x5328E0

UNITS = [
    (0x5327A5, 4, "4c1648caac4a2a956dff05c0ed8149ebf8310fb0108beceee7f5b3e6d7f28aaf",
     "mov rdi,[r13+0]", "pin", None),
    (0x5327A9, 2, "7087cf5f9f4f30301fd1b1737cfc01976911f0b4063dca2561bf9152acbfbac4",
     "test bl,bl", "pin", None),
    (0x5327AB, 6, "3f4512e9a101a4842b7b77ed8c4a7ed8382df90c3892e8925f412b032778e8e6",
     "je 0x5328e0", "je6", BRANCH_TARGET),
]

NATIVE = ROOT / "native/core/init_531d60_post_base_continue_b.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_4c4950_u1.hpp"
SLICE = ROOT / "native/core/appmain_init_ctor_98a90_slice.hpp"
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
        "kInit531d60PostBase4c4950U1EndRva = 0x5327a5" in PREV_NATIVE.read_text(),
        "4c4950_u1 end",
    )
    require(PREV_END == START, "prev end == start")
    require(START + SIZE == END, "size arithmetic")

    cursor = START
    for rva, size, sha, mnemonic, kind, target in UNITS:
        require(rva == cursor, f"contiguity at 0x{rva:x}")
        raw = img.read(rva, size)
        require(hashlib.sha256(raw).hexdigest() == sha, f"sha mismatch at 0x{rva:x}")
        if kind == "je6":
            require(raw[0:2] == bytes.fromhex("0f84"), f"je6 opcode at 0x{rva:x}")
            rel = struct.unpack("<i", raw[2:6])[0]
            require(rva + 6 + rel == target, f"je6 target at 0x{rva:x}")
        cursor = rva + size
    require(cursor == END, "units cover span")
    require(hashlib.sha256(img.read(START, SIZE)).hexdigest() == REGION_SHA256, "region sha")

    nh = NATIVE.read_text()
    require("kInit531d60PostBaseContinueBRva = 0x5327a5" in nh, "start rva")
    require("post_base_4c4950_u1" in SLICE.read_text(), "u1 wire")
    require("core/init_531d60_post_base_4c4950_u1.cpp" in CMAKE.read_text(), "cmake")

    print(
        f"OK check_f531d60_post_base_continue_b: span=0x{START:x}+{SIZE}B "
        f"units={len(UNITS)} region={REGION_SHA256[:12]}…; "
        f"je@{BRANCH_RVA:#x}->{BRANCH_TARGET:#x}; end={END:#x}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
