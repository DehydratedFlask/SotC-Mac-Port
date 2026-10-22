#!/usr/bin/env python3
"""ELF-tie app vslot 0x533100 shadowskels attach after load-attempt.

Private: needs local/re/eboot.elf. Pins 278 B fragment, fail join, f08+0x10.
Does NOT claim success atexit cluster, helper bodies, or next path.
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
ATTACH = 0x5335E1
ATTACH_SIZE = 278
ATTACH_SHA256 = "572cff3db9cc1769161dded9b0acca4b8bda8cd5002a1ba0948fd31f473be57e"
END = 0x5336F7
NATIVE = ROOT / "native/core/vslot_40_533100_skels_attach.hpp"
BASE = ROOT / "native/core/base_initialize_255130.hpp"


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

    require(hashlib.sha256(img.read(ATTACH, ATTACH_SIZE)).hexdigest() == ATTACH_SHA256, "sha")
    require(img.read(0x5335E1, 2) == bytes.fromhex("80bd"), "flag cmp")
    je = img.read(0x5335E8, 2)
    require(je[0] == 0x74, "je")
    fail = 0x5335EA + struct.unpack("b", bytes([je[1]]))[0]
    require(fail == 0x53365B, f"fail join {fail:#x}")
    require(call_target(0x533661, img) == 0x14D880, "14d880")
    require(call_target(0x53368E, img) == 0x14CCB0, "14ccb0")
    require(call_target(0x533696, img) == 0x14CCC0, "14ccc0")
    require(call_target(0x5336F2, img) == 0xF590, "f590")
    # install stores via [r15]; r15 was f08+0x10 from load-attempt
    require(img.read(0x53368B, 3) == bytes.fromhex("4d892f"), "store [r15]")

    nh = NATIVE.read_text()
    require("vslot_40_533100_skels_attach" in nh, "API")
    require("0x10" in nh or "kVslot40SkelsAttachSlotOff" in nh, "slot")
    require("NOT modelled" in nh, "omission")
    require("skels_attach" in BASE.read_text() or "SkelsAttach" in BASE.read_text(), "wire")

    print(
        f"OK check_f533100_vslot40_skels_attach: attach={ATTACH_SHA256[:12]}…; "
        f"fail@0x53365b; install/14ccb0/14ccc0/f590; end={END:#x}; "
        f"success_atexit/next_path NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
