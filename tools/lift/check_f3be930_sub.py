#!/usr/bin/env python3
"""ELF-tie sub-init 0x3be930 + leaves 0x1d9c0 / 0xf180.

Private: needs local/re/eboot.elf. Pins orchestrator loops, block stride 0x98,
slot lea 0x2dd9f78. Does NOT claim ensure migrate / non-null list / 0x1ab550.
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
SUB = 0x3BE930
SUB_SIZE = 826
SUB_SHA256 = "e3cfc0d4baeef0553b8a93ce89b02c6536da32dc298fb98cce5584d847da00ba"
BLOCK = 0x1D9C0
BLOCK_SIZE = 73
BLOCK_SHA256 = "d10dc9b8cb9a4b41ed932f46a2880424408fb5077658347f48ba1f9a637c24fd"
SLOT = 0xF180
SLOT_SIZE = 28
SLOT_SHA256 = "bd33279f6055c44aae01e0f9cdcb392acdadfcf4749dc6fe6e6113f5940f714c"
VT = 0x2DD9F78
NATIVE = ROOT / "native/core/init_sub_3be930.hpp"
CTOR = ROOT / "native/core/ctor_98a90_3be050.hpp"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def call_target(site: int, img: d.Image) -> int:
    b = img.read(site, 5)
    require(b[0] == 0xE8, f"missing call at {site:#x}")
    rel = struct.unpack("<i", b[1:5])[0]
    return site + 5 + rel


def lea_abs(site: int, img: d.Image) -> int:
    b = img.read(site, 7)
    require(b[0] == 0x48 and b[1] == 0x8D, f"missing lea at {site:#x}")
    rel = struct.unpack("<i", b[3:7])[0]
    return site + 7 + rel


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    require(hashlib.sha256(img.read(SUB, SUB_SIZE)).hexdigest() == SUB_SHA256, "sub sha")
    require(img.read(SUB + SUB_SIZE - 1, 1) == b"\xc3", "sub ret")
    require(hashlib.sha256(img.read(BLOCK, BLOCK_SIZE)).hexdigest() == BLOCK_SHA256, "block sha")
    require(hashlib.sha256(img.read(SLOT, SLOT_SIZE)).hexdigest() == SLOT_SHA256, "slot sha")

    require(img.read(0x3BE941, 4) == bytes.fromhex("41c60600"), "mov byte [r14],0")
    require(call_target(0x3BE94D, img) == BLOCK, "first 0x1d9c0")
    require(img.read(0x3BE952, 5) == bytes.fromhex("bba8000000"), "mov ebx,0xa8")
    require(img.read(0x3BE969, 7) == bytes.fromhex("4881c398000000"), "add rbx,0x98")
    require(img.read(0x3BE970, 7) == bytes.fromhex("4881fba8980000"), "cmp rbx,0x98a8")
    require(call_target(0x3BE964, img) == BLOCK, "loop 0x1d9c0")
    require(img.read(0x3BEC49, 11) == bytes.fromhex("41c786108a090000000000"), "tail dword 0")
    require(img.read(0x3BEC54, 11) == bytes.fromhex("49c786188a090000000000"), "tail qword 0")

    require(call_target(0x1D9CD, img) == SLOT, "block calls f180")
    require(lea_abs(SLOT, img) == VT, "f180 lea 0x2dd9f78")
    require(call_target(0x3BE0B6, img) == SUB, "ctor calls 0x3be930")

    nh = NATIVE.read_text()
    require("init_sub_3be930" in nh, "API")
    require("init_block_1d9c0" in nh, "block API")
    require("init_slot_f180" in nh, "slot API")
    require("init_sub_3be930" in CTOR.read_text(), "ctor wires")

    print(
        f"OK check_f3be930_sub: sub={SUB_SHA256[:12]}…; block={BLOCK_SHA256[:12]}…; "
        f"slot={SLOT_SHA256[:12]}…; vt={VT:#x}; loops/stride pinned"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
