#!/usr/bin/env python3
"""ELF-tie the ResourceSystem platform-tag lookup (E589).

Private: needs local/re/eboot.elf. Pins 0x147e20, the 0x147e10 default-key setter, the 0x144640
entry ctor, the 0x1497f0 node allocation, and the "_cmn"/"_ps4" literals.
"""

from __future__ import annotations

import hashlib
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
LOOKUP_SHA = "400cd00971c9020e005604664f6228bfd2025dce4782918e3b09217c193d8abe"
ENTRY_SHA = "6d68e88f376cb476f3be344b535be242fc095711e848268041b2ed1f7cd33a9b"
HEADER = ROOT / "native/core/platform_tag_147e20.hpp"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    require(hashlib.sha256(img.read(0x147E20, 208)).hexdigest() == LOOKUP_SHA, "0x147e20 sha256")
    require(hashlib.sha256(img.read(0x144640, 268)).hexdigest() == ENTRY_SHA, "0x144640 sha256")
    require(img.read(0x147E10, 4) == bytes.fromhex("897740c3"), "0x147e10 default-key setter")
    for addr, pattern, what in (
        (0x1498E5, "bfe0000000", "0x1497f0 allocates 0xe0"),
        (0x1498FF, "498d7e30", "0x1497f0 node value +0x30"),
        (0x149903, "41894e20", "0x1497f0 key +0x20"),
    ):
        raw = bytes.fromhex(pattern)
        require(img.read(addr, len(raw)) == raw, what)
    require(img.read(0xFA9445, 5) == b"_cmn\x00", '"_cmn" literal')
    require(img.read(0xFA95D8, 5) == b"_ps4\x00", '"_ps4" literal')

    header = HEADER.read_text()
    for token in (
        "kPlatformTagLookup147e20Size = 208",
        "kPlatformDefaultKeySetterRva = 0x147E10",
        "kPlatformTagRowCount = 25",
        "kPlatformTagCmn = 0x6E6D635F",
        "kPlatformTagPs4 = 0x3473705F",
        "kPlatformWildcardFourcc = 0x2A2A2A2A",
        "G1 PARTIAL",
    ):
        require(token in header, f"header token {token!r}")

    print(
        "OK check_fplatform_tag_147e20: lookup/entry hashes, 0x147e10 default-key setter, "
        "0x1497f0 0xe0 node/key/value offsets, _cmn/_ps4 literals, 25-row native table; "
        "RB nodes NOT lifted"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
