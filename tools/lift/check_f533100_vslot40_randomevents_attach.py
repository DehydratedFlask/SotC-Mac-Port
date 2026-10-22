#!/usr/bin/env python3
"""ELF-tie app vslot 0x533100 shadowrandomevents attach after load-attempt.

Private: needs local/re/eboot.elf. Pins 278 B fragment, fail join, f08+0x60.
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
ATTACH = 0x533BC5
ATTACH_SIZE = 278
ATTACH_SHA256 = "f0a8fbd32676f61f60b5d5c1e5bfc77dedf175f469d55a8111cd2a74bd261f9b"
END = 0x533CDB
NATIVE = ROOT / "native/core/vslot_40_533100_randomevents_attach.hpp"
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
    require(img.read(0x533BC5, 2) == bytes.fromhex("80bd"), "flag cmp")
    je = img.read(0x533BCC, 2)
    require(je[0] == 0x74, "je")
    fail = 0x533BCE + struct.unpack("b", bytes([je[1]]))[0]
    require(fail == 0x533C3F, f"fail join {fail:#x}")
    require(call_target(0x533C45, img) == 0x14D880, "14d880")
    require(call_target(0x533C72, img) == 0x14CCB0, "14ccb0")
    require(call_target(0x533C7A, img) == 0x14CCC0, "14ccc0")
    require(call_target(0x533CD6, img) == 0xF590, "f590")
    require(img.read(0x533C6F, 3) == bytes.fromhex("4d892f"), "store [r15]")
    require(img.read(0x533C54, 4) == bytes.fromhex("41807e68"), "flag [r14+0x68]")

    nh = NATIVE.read_text()
    require("vslot_40_533100_randomevents_attach" in nh, "API")
    require("kVslot40RandomeventsAttachSlotOff" in nh, "slot")
    require("NOT modelled" in nh, "omission")
    require(
        "randomevents_attach" in BASE.read_text() or "RandomeventsAttach" in BASE.read_text(),
        "wire",
    )

    print(
        f"OK check_f533100_vslot40_randomevents_attach: attach={ATTACH_SHA256[:12]}…; "
        f"fail@0x533c3f; install/14ccb0/14ccc0/f590; end={END:#x}; "
        f"success_atexit/next_path NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
