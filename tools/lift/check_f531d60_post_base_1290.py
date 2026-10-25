#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue call-site for 0x1290.

Private: needs local/re/eboot.elf. Pins 5 B fragment after 0x5c64c0.
Does NOT claim body of 0x1290 or later Init continues.
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
FRAG = 0x531E03
FRAG_SIZE = 5
FRAG_SHA256 = "b1ec75e6b85686dfed68488ce47a925678be8d408c0fc7ef4db120cb97e599b1"
END = 0x531E08
TARGET = 0x1290
PROLOGUE_SIZE = 0x70  # through first nested call; body NOT modelled
PROLOGUE_SHA256 = "80ca337a2942c038f329e9bc6c8851e78ae200d2cba6106dfe96486f2d09b944"
LOADER_CELL = 0x2EE68C8
NATIVE = ROOT / "native/core/init_531d60_post_base_1290.hpp"
SLICE = ROOT / "native/core/appmain_init_ctor_98a90_slice.hpp"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def call_target(site: int, img: d.Image) -> int:
    b = img.read(site, 5)
    require(b[0] == 0xE8, f"missing call at {site:#x}")
    rel = struct.unpack("<i", b[1:5])[0]
    return site + 5 + rel


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    require(call_target(0x531DFE, img) == 0x5C64C0, "prev 0x5c64c0")
    require(hashlib.sha256(img.read(FRAG, FRAG_SIZE)).hexdigest() == FRAG_SHA256, "sha")
    require(call_target(FRAG, img) == TARGET, "call 0x1290")
    require(call_target(END, img) == 0x56DB70, "next 0x56db70 (not claimed)")

    pro = img.read(TARGET, PROLOGUE_SIZE)
    require(hashlib.sha256(pro).hexdigest() == PROLOGUE_SHA256, "prologue sha")
    lea = img.read(TARGET + 0x15, 7)
    require(lea[0:3] == bytes.fromhex("488d0d"), "lea loader cell")
    cell = TARGET + 0x15 + 7 + struct.unpack("<i", lea[3:7])[0]
    require(cell == LOADER_CELL, "loader cell")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_1290" in nh, "API")
    require("NOT modelled" in nh, "omission")
    require("post_base_1290" in SLICE.read_text() or "PostBase1290" in SLICE.read_text(), "wire")

    print(
        f"OK check_f531d60_post_base_1290: frag={FRAG_SHA256[:12]}…; "
        f"call {TARGET:#x}; end={END:#x}; "
        f"1290_body/56db70/5c64c0_body/56dcb0_body/499f80_body/140d90_body/14c2f0/6a5370 NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
