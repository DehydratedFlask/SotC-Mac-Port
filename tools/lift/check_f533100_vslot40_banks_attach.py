#!/usr/bin/env python3
"""ELF-tie app vslot 0x533100 shadowbanks attach after load-attempt.

Private: needs local/re/eboot.elf. Pins 278 B fragment, fail join, f08+0x40.
Does NOT claim success atexit cluster, helper bodies, or shadowstreams.
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
ATTACH = 0x5338D3
ATTACH_SIZE = 278
ATTACH_SHA256 = "8d40d0d965218272b7e11120fa4634df31414bce7e66d27e78bc49b7604d2a29"
END = 0x5339E9
NATIVE = ROOT / "native/core/vslot_40_533100_banks_attach.hpp"
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
    require(img.read(0x5338D3, 2) == bytes.fromhex("80bd"), "flag cmp")
    je = img.read(0x5338DA, 2)
    require(je[0] == 0x74, "je")
    fail = 0x5338DC + struct.unpack("b", bytes([je[1]]))[0]
    require(fail == 0x53394D, f"fail join {fail:#x}")
    require(call_target(0x533953, img) == 0x14D880, "14d880")
    require(call_target(0x533980, img) == 0x14CCB0, "14ccb0")
    require(call_target(0x533988, img) == 0x14CCC0, "14ccc0")
    require(call_target(0x5339E4, img) == 0xF590, "f590")
    require(img.read(0x53397D, 3) == bytes.fromhex("4d892f"), "store [r15]")
    require(img.read(0x533962, 4) == bytes.fromhex("41807e48"), "flag [r14+0x48]")

    nh = NATIVE.read_text()
    require("vslot_40_533100_banks_attach" in nh, "API")
    require("kVslot40BanksAttachSlotOff" in nh, "slot")
    require("NOT modelled" in nh, "omission")
    require("banks_attach" in BASE.read_text() or "BanksAttach" in BASE.read_text(), "wire")

    print(
        f"OK check_f533100_vslot40_banks_attach: attach={ATTACH_SHA256[:12]}…; "
        f"fail@0x53394d; install/14ccb0/14ccc0/f590; end={END:#x}; "
        f"success_atexit/shadowstreams NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
