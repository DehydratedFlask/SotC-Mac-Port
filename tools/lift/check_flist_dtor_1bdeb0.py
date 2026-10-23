#!/usr/bin/env python3
"""ELF-tie the list destructor 0x1bdeb0 (0x58-byte flag nodes).

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
RVA = 0x1BDEB0
SIZE = 120
FN_SHA256 = "7d64b9d335c76abf2ba37b4763dfea142bd7c4f5c5e67c01a2c01d682adaba35"
END = 0x1BDF28
RELEASE = 0xF590
FREE = 0x170C0
NATIVE = ROOT / "native/core/list_dtor_1bdeb0.hpp"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    body = img.read(RVA, SIZE)
    require(hashlib.sha256(body).hexdigest() == FN_SHA256, "fn 0x1bdeb0 sha256")
    require(body[-1:] == b"\xc3", "ret at 0x1bdf27")
    # cmp dword [r15+0x50],0 ; jne back
    require(bytes.fromhex("41837f5000") in body, "flag test dword [r15+0x50]")
    # dec dword [r14+0x10]
    require(bytes.fromhex("49ff4e10") in body, "count decrement [r14+0x10]")
    # 0xf590 twice
    for site in (0x1BDEFC, 0x1BDF05):
        op = img.read(site, 5)
        require(op[0] == 0xE8, f"call opcode at 0x{site:x}")
        require(site + 5 + struct.unpack("<i", op[1:5])[0] == RELEASE, f"release at 0x{site:x}")
    # 0x170c0 free
    op = img.read(0x1BDF12, 5)
    require(op[0] == 0xE8, "free call opcode")
    require(0x1BDF12 + 5 + struct.unpack("<i", op[1:5])[0] == FREE, "free 0x170c0")

    header = NATIVE.read_text().lower()
    for token in (
        "klistdtor1bdeb0rva = 0x1bdeb0",
        "klistdtor1bdeb0size = 120",
        "klistdtor1bdeb0endrva = 0x1bdf28",
        "klistdtor1bdeb0releaserva = 0xf590",
        "klistdtor1bdeb0freerva = 0x170c0",
        "not modelled",
    ):
        require(token in header, f"header token {token!r}")

    print(
        f"OK check_flist_dtor_1bdeb0: 0x1bdeb0+{SIZE}B sha={FN_SHA256[:12]}…; "
        "flag test +0x50, count -0x10, release 0xf590 x2, tag-2 free 0x170c0; "
        "release/free bodies and the 0x2ee68c8 singleton NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
