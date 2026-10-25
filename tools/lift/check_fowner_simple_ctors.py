#!/usr/bin/env python3
"""ELF-tie the three trivial 0x2569d0 constructors (E580).

Private: needs local/re/eboot.elf. Pins the 0xa1cc0/0xfafb0/0x26f820 bodies, their field stores and
the native object sizes.
"""

from __future__ import annotations

import hashlib
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
FUNCTIONS = [
    ("0xa1cc0 no-op", 0xA1CC0, 1, "ae3f4619b0413d70d3004b9131c3752153074e45725be13b9a148978895e359e"),
    ("0xfafb0", 0xFAFB0, 92, "93b32120b1765de72bfdc321bc567439b28c33475b4a75a7fa572cce47386d71"),
    ("0x26f820", 0x26F820, 57, "32109cc889c59a864d0b329a1ff425b8f30bea0a9a4a92bd734fe49a0a856595"),
]
HEADER = ROOT / "native/core/owner_simple_ctors.hpp"
SOURCE = ROOT / "native/core/owner_simple_ctors.cpp"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    for name, rva, size, sha in FUNCTIONS:
        require(hashlib.sha256(img.read(rva, size)).hexdigest() == sha, f"{name} sha256")

    # 0xfafb0: vptr store, 0x2000 fill, the three pointers, 0x80000 fill.
    for addr, pattern, what in (
        (0xFAFB7, "488d05dacc4901", "vptr lea"),
        (0xFAFC1, "31f6ba00200000", "first memset size 0x2000"),
        (0xFAFD8, "488dbb28200000", "lea +0x2028"),
        (0xFAFDF, "4c8db328200800", "lea +0x82028"),
        (0xFAFED, "4889bb08200000", "store +0x2008"),
        (0xFAFF4, "4c89b318200000", "store +0x2018"),
        (0xFB000, "4c89b310200000", "store +0x2010"),
    ):
        raw = bytes.fromhex(pattern)
        require(img.read(addr, len(raw)) == raw, what)

    # 0x26f820: base call, vptr store, +0x18 zero, sub-object tail call.
    for addr, pattern, what in (
        (0x26F826, "4889fbe8", "base call setup"),
        (0x26F82E, "488d0593d13201", "vptr lea"),
        (0x26F83C, "48c7431800000000", "zero +0x18"),
        (0x26F844, "4881c320400000", "add +0x4020"),
        (0x26F854, "e9b7badcff", "tail call"),
    ):
        raw = bytes.fromhex(pattern)
        require(img.read(addr, len(raw)) == raw, what)

    header = HEADER.read_text()
    for token in (
        "kOwnerA1cc0Size = 1",
        "kOwnerFafb0ObjectSize = 0x82028",
        "kOwner26f820ObjectSize = 0x4028",
        "kOwner26f820SubOffset = 0x4020",
        "host value",
        "G1 PARTIAL",
    ):
        require(token in header, f"header token {token!r}")

    source = SOURCE.read_text()
    for token in ("0x2008", "0x2010", "0x2018", "memset", "kOwner26f820SubOffset"):
        require(token in source, f"source token {token!r}")

    print(
        "OK check_fowner_simple_ctors: 0xa1cc0/0xfafb0/0x26f820 hashes, 0x2000/0x80000 fills, "
        "three pointer fields, base/sub ctor order; guest vptr NOT executed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
