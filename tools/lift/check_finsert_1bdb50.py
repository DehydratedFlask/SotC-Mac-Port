#!/usr/bin/env python3
"""ELF-tie the 0x58-byte-node list inserter 0x1bdb50.

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
RVA = 0x1BDB50
SIZE = 0x130
FN_SHA256 = "add72de8165917d2e57dcdd17f64eaa9e5b39f447484d2c605dc98d4d78c1313"
END = 0x1BDC80
ALLOC = 0x17090
RELEASE = 0xF590
CANARY = 0x15FE518
STACK_FAIL = 0xF8F3A0
NATIVE = ROOT / "native/core/insert_1bdb50.hpp"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    body = img.read(RVA, SIZE)
    require(hashlib.sha256(body).hexdigest() == FN_SHA256, "fn 0x1bdb50 sha256")
    require(body.rfind(b"\xc3") == 0x1BDC77 - RVA, "ret at 0x1bdc77")
    require(body[-3:] == b"\x90\x90\x90", "nop padding ends the body at 0x1bdc80")

    # movl $0x58,%edi  (the node size handed to 0x17090)
    require(body[0x1BDBFC - RVA:0x1BDC01 - RVA] == b"\xbf\x58\x00\x00\x00", "size 0x58")
    # movl %r14d,0x50(%rax)  (the dword flag)
    require(body[0x1BDC2C - RVA:0x1BDC30 - RVA] == b"\x44\x89\x70\x50", "flag store at +0x50")
    # incq 0x10(%rbx)  (count)
    require(body[0x1BDC4D - RVA:0x1BDC51 - RVA] == b"\x48\xff\x43\x10", "count increment")

    for site, target, label in (
        (0x1BDC0B, ALLOC, "alloc 0x17090"),
        (0x1BDC51, RELEASE, "release temp_b"),
        (0x1BDC5B, RELEASE, "release temp_a"),
        (0x1BDC78, STACK_FAIL, "stack check failure"),
    ):
        op = img.read(site, 5)
        require(op[0] == 0xE8, f"call opcode at 0x{site:x}")
        require(site + 5 + struct.unpack("<i", op[1:5])[0] == target, f"{label} at 0x{site:x}")

    # movq 0x14409ab(%rip),%r12 at 0x1bdb66 must resolve to the stack-canary slot.
    canary_load = img.read(0x1BDB66, 7)
    require(canary_load[:3] == b"\x4c\x8b\x25", "canary load shape")
    require(0x1BDB66 + 7 + struct.unpack("<i", canary_load[3:7])[0] == CANARY, "canary slot")

    # The two callables are copied to +0x10 and +0x30 as full 0x20 B vectors.
    require(body[0x1BDC1E - RVA:0x1BDC23 - RVA] == b"\xc5\xfc\x11\x40\x10", "copy A to +0x10")
    require(body[0x1BDC27 - RVA:0x1BDC2C - RVA] == b"\xc5\xfc\x11\x48\x30", "copy B to +0x30")

    header = NATIVE.read_text().lower()
    for token in (
        "kinsert1bdb50rva = 0x1bdb50",
        "kinsert1bdb50size = 0x130",
        "kinsert1bdb50endrva = 0x1bdc80",
        "kinsert1bdb50allocrva = 0x17090",
        "kinsert1bdb50releaserva = 0xf590",
        "not modelled",
    ):
        require(token in header, f"header token {token!r}")

    print(
        f"OK check_finsert_1bdb50: 0x1bdb50+{SIZE}B sha={FN_SHA256[:12]}…; 0x58 node, payload +0x10/+0x30, "
        "flag +0x50, count ++0x10, alloc 0x17090, two 0xf590 temp releases, canary tail; "
        "allocator/release bodies and the flag's meaning NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
