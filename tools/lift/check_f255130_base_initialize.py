#!/usr/bin/env python3
"""ELF-tie base Initialize 0x255130 orchestrator + leaf 0x357f0 + app vslots 6/8.

Private: needs local/re/eboot.elf. Pins orchestrator body, call targets, vtable
words at 0x15c8d08/0x15c8d18, and 0x357f0 bytes. Does NOT claim bodies of
0x255180 / 0x255340 / 0x534130 / 0x533100.
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
BASE = 0x255130
BASE_SIZE = 79
BASE_SHA256 = "a8425cb0e7e7f6fc6e3102154ea6c5f25d72b3dfcef59586ff0e67313746df79"
LEAF = 0x357F0
LEAF_BYTES = bytes.fromhex("400fb6c6874720c3")
VT30 = 0x15C8D08
VT40 = 0x15C8D18
SLOT30 = 0x534130
SLOT40 = 0x533100
NATIVE = ROOT / "native/core/base_initialize_255130.hpp"
LEAF_H = ROOT / "native/core/xchg_store_u32_357f0.hpp"
SLICE = ROOT / "native/core/appmain_init_base_initialize_slice.hpp"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def call_target(site: int, img: d.Image) -> int:
    b = img.read(site, 5)
    require(b[0] == 0xE8, f"missing call at {site:#x}")
    rel = struct.unpack("<i", b[1:5])[0]
    return site + 5 + rel


def u64(img: d.Image, addr: int) -> int:
    return struct.unpack("<Q", img.read(addr, 8))[0]


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    body = img.read(BASE, BASE_SIZE)
    require(len(body) == BASE_SIZE, "size")
    digest = hashlib.sha256(body).hexdigest()
    require(digest == BASE_SHA256, f"sha {digest}")

    require(call_target(0x25513D, img) == 0x255180, "prologue 0x255180")
    require(img.read(0x255148, 3) == bytes.fromhex("ff5030"), "vcall [rax+0x30]")
    require(call_target(0x255152, img) == 0x255340, "mid 0x255340")
    require(img.read(0x255160, 3) == bytes.fromhex("ff5040"), "vcall [rax+0x40]")
    require(call_target(0x25516F, img) == LEAF, "leaf 0x357f0")

    lea = img.read(0x255163, 7)
    require(lea[0:3] == bytes.fromhex("488d05"), "lea flag cell")
    disp = struct.unpack("<i", lea[3:7])[0]
    require(0x255163 + 7 + disp == 0x2EA4718, "cell 0x2ea4718")

    require(img.read(LEAF, 8) == LEAF_BYTES, "0x357f0 bytes")
    require(u64(img, VT30) == SLOT30, "vtable +0x30 → 0x534130")
    require(u64(img, VT40) == SLOT40, "vtable +0x40 → 0x533100")

    # Init callsite
    require(call_target(0x531DDA, img) == BASE, "Init → base Initialize")

    hdr = NATIVE.read_text()
    require("base_initialize_255130" in hdr, "API")
    require("NOT modelled" in hdr or "not modelled" in hdr.lower(), "omission")
    require("0x255180" in hdr and "0x255340" in hdr, "callee cites")
    require("0x534130" in hdr and "0x533100" in hdr, "vslot cites")
    require(LEAF_H.is_file() and "xchg_store_u32_357f0" in LEAF_H.read_text(), "leaf API")
    require("run_appmain_init_base_initialize_slice" in SLICE.read_text(), "slice API")

    print(
        f"OK check_f255130_base_initialize: orchestrator={digest[:12]}…; "
        f"vslots {SLOT30:#x}/{SLOT40:#x}; leaf 0x357f0; bodies of 0x255180/0x255340 NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
