#!/usr/bin/env python3
"""ELF-tie the bug-report Frame root (E588).

Private: needs local/re/eboot.elf. Pins 0xa70c30, 0xa70d10, the call/tail targets and the
"Frame"/"frameNum" literals.
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
GET_SHA = "33074135a3486645057378fb6041f670fe5d9318eb24781fbeec6b0a417f4e24"
ADOPT_SHA = "b1ed5e7e72a5ac845f06663de16d8f45ce895f4ad8d6d71873ab682eac25f09c"
HEADER = ROOT / "native/core/frame_root.hpp"


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

    require(hashlib.sha256(img.read(0xA70C30, 220)).hexdigest() == GET_SHA, "0xa70c30 sha256")
    require(hashlib.sha256(img.read(0xA70D10, 32)).hexdigest() == ADOPT_SHA, "0xa70d10 sha256")
    require(call_target(img, 0xA70D19) == 0xA70C30, "adopt calls the Frame getter")
    raw = img.read(0xA70D2A, 5)
    require(raw[0] == 0xE9, "adopt tail jump")
    require(0xA70D2A + 5 + struct.unpack("<i", raw[1:5])[0] == 0xAB0D40, "tail -> 0xab0d40")
    require(img.read(0xF94A28, 6) == b"Frame\x00", '"Frame" literal')
    require(img.read(0xFB3E5D, 9) == b"frameNum\x00", '"frameNum" literal')

    header = HEADER.read_text()
    for token in (
        "kFrameRootGetRva = 0xA70C30",
        "kFrameRootGetSize = 220",
        "kFrameRootAdoptRva = 0xA70D10",
        "kFrameRootAdoptSize = 32",
        "kFrameRootAppendRva = 0xAB0D40",
        "G1 PARTIAL",
    ):
        require(token in header, f"header token {token!r}")

    print(
        "OK check_fframe_root: 0xa70c30/0xa70d10 hashes, adopt->getter->0xab0d40, "
        '"Frame"/"frameNum" literals; live frame counter and ctrl object NOT lifted'
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
