#!/usr/bin/env python3
"""ELF-tie the 0x14c2f0 ResourceId constructor (E582).

Private: needs local/re/eboot.elf. Pins the body and FNV hashes, the movbe fourcc load, the three
key calls (0xf1a0, 0x147e20, 0x161f80) and the native patch/hash order.
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
BODY_SHA = "548a3850d7c9e5d34fe50f5d05bf1509fd433fb231614c13ee8e2abf4f3bcd6b"
FNV_SHA = "552c900d77771ee72cb1c8ed6f39bd510460ae7b614d92b654f1a85ca6031599"
HEADER = ROOT / "native/core/resource_id_14c2f0.hpp"
SOURCE = ROOT / "native/core/resource_id_14c2f0.cpp"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def call_target(img, addr: int) -> int:
    raw = img.read(addr, 5)
    require(raw[0] == 0xE8, f"call opcode at {addr:#x}")
    return addr + 5 + struct.unpack("<i", raw[1:5])[0]


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    require(hashlib.sha256(img.read(0x14C2F0, 166)).hexdigest() == BODY_SHA, "0x14c2f0 sha256")
    require(hashlib.sha256(img.read(0x161F80, 38)).hexdigest() == FNV_SHA, "0x161f80 sha256")
    require(call_target(img, 0x14C309) == 0xF1A0, "SharedStr ctor call")
    require(call_target(img, 0x14C361) == 0x147E20, "platform tag lookup call")
    require(call_target(img, 0x14C37E) == 0x161F80, "FNV call")
    require(img.read(0x14C34D, 7) == bytes.fromhex("440f38f06c03fc"), "movbe fourcc load")
    require(img.read(0x14C371, 5)[0] == 0xE8, "memcpy call")

    header = HEADER.read_text()
    for token in (
        "kResourceIdCtor14c2f0Size = 166",
        "kPlatformTagLookupRva = 0x147E20",
        "movbe",
        "NOT modelled",
    ):
        require(token in header, f"header token {token!r}")

    source = SOURCE.read_text()
    for token in ("strstr", "__builtin_bswap32", "fnv_span_161f80", "platform_tag"):
        require(token in source, f"source token {token!r}")

    print(
        "OK check_fresource_id_14c2f0: 0x14c2f0 166B sha=548a3850…, 0x161f80 FNV sha=552c900d…, "
        "movbe fourcc, SharedStr/tag/FNV calls; runtime tag table NOT lifted"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
