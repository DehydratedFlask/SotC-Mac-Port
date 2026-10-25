#!/usr/bin/env python3
"""ELF-tie the list-B inserter 0x1bdc80.

Private: needs local/re/eboot.elf. Pins the 417 B body, 0x50 allocation, container+0x20 sentinel,
count at +0x30, node stores and the four 0xf590 calls.
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
RVA = 0x1BDC80
SIZE = 0x1A1
FN_SHA256 = "414863a38e255a73f57a9216cb95b75ccaf574e8e7a03681c4196542724f250a"
END = 0x1BDE21
ALLOC = 0x17090
RELEASE = 0xF590
NATIVE = ROOT / "native/core/insert_1bdc80.hpp"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def call_target(img: "d.Image", addr: int) -> int:
    raw = img.read(addr, 5)
    require(raw[0] == 0xE8, f"call opcode at {addr:#x}")
    return addr + 5 + struct.unpack("<i", raw[1:5])[0]


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    body = img.read(RVA, SIZE)
    require(hashlib.sha256(body).hexdigest() == FN_SHA256, "0x1bdc80 sha256")
    require(img.read(END - 1, 1) == b"\xc3", "ret at 0x1bde20")
    require(body[0:4] == bytes.fromhex("554889e5"), "push rbp; mov rbp,rsp")

    # mov edi,0x50 ; call 0x17090
    require(body[0x110:0x115] == bytes.fromhex("bf50000000"), "alloc size 0x50")
    require(call_target(img, 0x1BDD9F) == ALLOC, "alloc call -> 0x17090")
    # lea rbx,[r12+0x20] ; count inc qword [r12+0x30]
    require(body[0x11A:0x11E] == bytes.fromhex("498d5c24"), "list B sentinel +0x20")
    require(b"\x49\xff\x44\x24\x30" in body, "count increment [container+0x30]")
    # node stores at +0x10 / +0x30
    require(b"\xc5\xfc\x11\x40\x10" in body, "node+0x10 store")
    require(b"\xc5\xfc\x11\x48\x30" in body, "node+0x30 store")
    for site in (0x1BDDE4, 0x1BDDEE, 0x1BDDF8, 0x1BDE02):
        require(call_target(img, site) == RELEASE, f"release at {site:#x} -> 0xf590")

    header = NATIVE.read_text().lower()
    for token in (
        "kinsert1bdc80rva = 0x1bdc80",
        "kinsert1bdc80size = 0x1a1",
        "kinsert1bdc80endrva = 0x1bde21",
        "kinsert1bdc80nodesize = 0x50",
        "kinsert1bdc80listoffset = 0x20",
        "kinsert1bdc80countoffset = 0x30",
        "not modelled",
    ):
        require(token in header, f"header token {token!r}")

    print(
        f"OK check_finsert_1bdc80: 0x1bdc80+{SIZE}B sha={FN_SHA256[:12]}…; 0x50 node; "
        "list B +0x20 / count +0x30; four 0xf590 hand-offs; reader/clear NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
