#!/usr/bin/env python3
"""ELF-tie the allocator wrappers 0x17070 / 0x170b0 / 0x170c0.

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
NATIVE = ROOT / "native/core/alloc_wrappers_17070.hpp"

CASES = [
    # rva, size, bytes hex, tail target
    (0x17070, 20, "be01000000ba08000000b901000000e99cb7ffff", 0x12820),
    (0x170B0, 10, "be01000000e936baffff", 0x12AF0),
    (0x170C0, 10, "be02000000e926baffff", 0x12AF0),
]


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    for rva, size, expect, target in CASES:
        body = img.read(rva, size)
        require(body.hex() == expect, f"bytes at 0x{rva:x}")
        require(body[-5] == 0xE9, f"tail jmp at 0x{rva:x}")
        rel = struct.unpack("<i", body[-4:])[0]
        require(rva + size + rel == target, f"tail target from 0x{rva:x}")

    header = NATIVE.read_text().lower()
    for token in (
        "kalloc17070rva = 0x17070",
        "kalloc17070size = 20",
        "kfree170b0rva = 0x170b0",
        "kfree170c0rva = 0x170c0",
        "kallocdispatchrva = 0x12820",
        "kfreedispatchrva = 0x12af0",
        "not modelled",
    ):
        require(token in header, f"header token {token!r}")

    print(
        "OK check_falloc_wrappers: 0x17070(20B)->0x12820 tag1/align8/flags1; "
        "0x170b0(10B)->0x12af0 tag1; 0x170c0(10B)->0x12af0 tag2; "
        "dispatch bodies/allocator bookkeeping NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
