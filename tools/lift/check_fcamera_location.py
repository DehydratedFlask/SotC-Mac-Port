#!/usr/bin/env python3
"""ELF-tie the pure camera math: 0x50ba0 Euler conversion and the six-double format site.

Private: needs local/re/eboot.elf. Pins the body hash, the two atan2 imports, the RAD2DEG and
gimbal constants, and the formatter's string/call sequence.
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
RVA = 0x50BA0
SIZE = 261
FN_SHA256 = "28f72958d08acb477bb157a705051e26f11ed306ac777b7e19ec9ffddc6289de"
END = 0x50CA4
ATAN2F = 0xF8FD30
ATAN2 = 0xF8FD50
NATIVE = ROOT / "native/core/camera_location_25a4d0.hpp"


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
    require(hashlib.sha256(body).hexdigest() == FN_SHA256, "0x50ba0 sha256")
    require(img.read(END, 1) == b"\xc3", "ret at 0x50ca4")

    for site, target in ((0x50BF2, ATAN2F), (0x50C12, ATAN2), (0x50C35, ATAN2F),
                         (0x50C53, ATAN2F), (0x50C73, ATAN2)):
        require(call_target(img, site) == target, f"atan2 call at {site:#x} -> {target:#x}")

    require(img.read(0xF99BF0, 8) == bytes.fromhex("e02e6542e02e6542"), "RAD2DEG pair")
    require(img.read(0xF99C4C, 4) == bytes.fromhex("00000036"), "16*FLT_EPSILON constant")
    require(img.read(0xF99C50, 4) == bytes.fromhex("e02e6542"), "RAD2DEG scalar")

    # Formatter site: "setCameraPos ..." literal and the 0x10a90 call.
    require(img.read(0x25A836, 7) == bytes.fromhex("488d35f997d600"), "format string lea")
    require(img.read(0xFC4036, 43) == b"setCameraPos %.2f %.2f %.2f %.2f %.2f %.2f\x00",
            "camera format string")
    require(call_target(img, 0x25A875) == 0x10A90, "formatter call -> 0x10a90")
    require(img.read(0x25A810, 7) == bytes.fromhex("488d050f98d600"), "Camera Location lea")

    header = NATIVE.read_text().lower()
    for token in (
        "kcameraeulerrva = 0x50ba0",
        "kcameraeulersize = 261",
        "kcameraeulerendrva = 0x50ca4",
        "kcameraradtodeg = 57.2957764f",
        "kcameragimbalepsilon = 1.90734863e-06f",
        "not modelled",
    ):
        require(token in header, f"header token {token!r}")

    print(
        f"OK check_fcamera_location: 0x50ba0+{SIZE}B sha={FN_SHA256[:12]}…; atan2f/atan2 calls; "
        "RAD2DEG 57.2957764; 16*FLT_EPSILON; formatter site + literal; live pose chain NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
