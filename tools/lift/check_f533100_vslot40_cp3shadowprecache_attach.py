#!/usr/bin/env python3
"""ELF-tie app vslot 0x533100 cp3shadowprecache attach after load-attempt.

Private: needs local/re/eboot.elf. Pins 165 B fragment, self+0x3cc8, f590 end.
Does NOT claim success atexit notify, helper bodies, or 0x140d90.
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
ATTACH = 0x533F97
ATTACH_SIZE = 165
ATTACH_SHA256 = "da6b240442d580e8d8d256a07d133ca66be7ed5c2ddb7d92bda5c8508c3307d3"
END = 0x53403C
NATIVE = ROOT / "native/core/vslot_40_533100_cp3shadowprecache_attach.hpp"
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
    require(img.read(0x533F97, 4) == bytes.fromhex("498b9dc8"), "load prev [r13+0x3cc8]")
    require(call_target(0x533F9E, img) == 0x14D880, "14d880")
    require(img.read(0x533FB4, 4) == bytes.fromhex("4180bdd0"), "flag [r13+0x3cd0]")
    require(img.read(0x533FDC, 3) == bytes.fromhex("49893f"), "store [r15]")
    require(call_target(0x533FDF, img) == 0x14CCB0, "14ccb0")
    require(call_target(0x533FE7, img) == 0x14CCC0, "14ccc0")
    require(img.read(0x533FEC, 2) == bytes.fromhex("80bd"), "success flag cmp")
    require(call_target(0x534037, img) == 0xF590, "f590")
    require(img.read(0x53403C, 5) == bytes.fromhex("be09000000"), "next mov esi,9")
    require(img.read(0x53311B, 7) == bytes.fromhex("4889bd00fdffff"), "self→[rbp-0x300]")

    nh = NATIVE.read_text()
    require("vslot_40_533100_cp3shadowprecache_attach" in nh, "API")
    require("kVslot40Cp3shadowprecacheAttachSlotOff" in nh, "slot")
    require("NOT modelled" in nh, "omission")
    require(
        "cp3shadowprecache_attach" in BASE.read_text()
        or "Cp3shadowprecacheAttach" in BASE.read_text(),
        "wire",
    )

    print(
        f"OK check_f533100_vslot40_cp3shadowprecache_attach: attach={ATTACH_SHA256[:12]}…; "
        f"self+0x3cc8; install/14ccb0/14ccc0/f590; end={END:#x}; "
        f"success_atexit/140d90 NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
