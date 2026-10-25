#!/usr/bin/env python3
"""ELF-tie the two app-info entry writers: 0x25a4d0 (product info) and 0xb10 (game entry).

Private: needs local/re/eboot.elf. Pins extents/hashes, the four inserter calls, the string
constants, the platform/debug/camera value producers and the camera-conditional branch.
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
PRODUCT_RVA = 0x25A4D0
PRODUCT_SIZE = 1014
PRODUCT_SHA = "12a2d0d7aeec89e9e73aa97d91eb72263610c2b27bbc633f9bc80faa411347fa"
GAME_RVA = 0xB10
GAME_SIZE = 139
GAME_SHA = "4220f09b8279ff3dd5ce4f4780300f954ff884bf181ef161f72859f12b5d8494"
INSERT = 0x1BDB50
NATIVE_H = ROOT / "native/core/app_info_install.hpp"
NATIVE_C = ROOT / "native/core/app_info_install.cpp"


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

    product = img.read(PRODUCT_RVA, PRODUCT_SIZE)
    require(hashlib.sha256(product).hexdigest() == PRODUCT_SHA, "0x25a4d0 sha256")
    require(img.read(0x25A8BF, 1) == b"\xc3", "ret at 0x25a8bf")
    require(img.read(0x25A8C0, 5)[0] == 0xE8, "cold stack-check call at 0x25a8c0")

    game = img.read(GAME_RVA, GAME_SIZE)
    require(hashlib.sha256(game).hexdigest() == GAME_SHA, "0xb10 sha256")
    require(img.read(0xB9A, 1) == b"\xc3", "ret at 0xb9a")

    # Four product inserts and the game insert all target 0x1bdb50.
    for site in (0x25A538, 0x25A604, 0x25A68A, 0x25A88F, 0xB72):
        require(call_target(img, site) == INSERT, f"insert call at {site:#x} -> 0x1bdb50")

    # Value producers and the camera-conditional handle read.
    for site, target in ((0x25A56A, 0x3F4E0), (0x25A597, 0x1360),
                         (0x25A647, 0x120C0), (0x25A875, 0x10A90)):
        require(call_target(img, site) == target, f"producer at {site:#x} -> {target:#x}")
    require(b"\x49\x8b\x87\x68\x26\x00\x00" in product, "owner+0x2668 camera handle read")

    # String constants.
    strings = {
        0xF9199E: b"Game\x00",
        0xF919A3: b"Shadow of the Colossus\x00",
        0xFC3FC8: b"Build Version\x00",
        0xFC3FD6: b"Platform Type\x00",
        0xFC4001: b"Is Debug Memory Available\x00",
        0xFC4026: b"Camera Location\x00",
        0xFC3FEA: b"ps4\x00",
        0xFC3FEE: b" - NEO\x00",
        0xFC3FF5: b" - STANDARD\x00",
        0xFC401B: b"True\x00",
        0xFC4020: b"False\x00",
        0xFC4036: b"setCameraPos %.2f %.2f %.2f %.2f %.2f %.2f\x00",
    }
    for rva, expected in strings.items():
        require(img.read(rva, len(expected)) == expected, f"string at {rva:#x}")

    header = NATIVE_H.read_text().lower()
    for token in (
        "kappinfoinstallerrva = 0x25a4d0",
        "kappinfoinstallersize = 1014",
        "kappinfogamewriterrva = 0xb10",
        "kappinfogamewritersize = 139",
        "kappinfoentryflag = 1",
        "not modelled",
    ):
        require(token in header, f"header token {token!r}")
    source = NATIVE_C.read_text().lower()
    for token in ("kappinfobuildversionname", "kappinfoplatformneo",
                  "kappinfoplatformstandard", "kappinfodebugmemoryname",
                  "kappinfocameraname", "kappinfogamename"):
        require(token in source, f"source token {token!r}")

    print(
        f"OK check_fapp_info_install: 0x25a4d0+{PRODUCT_SIZE}B sha={PRODUCT_SHA[:12]}…; "
        f"0xb10+{GAME_SIZE}B sha={GAME_SHA[:12]}…; 5 inserts -> 0x1bdb50; "
        "IsNeo/format/debug/format producers and all 12 strings match; native API NOT executed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
