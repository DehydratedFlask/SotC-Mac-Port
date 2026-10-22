#!/usr/bin/env python3
"""ELF-tie app vslot 0x533100 shadowmodels attach after load-attempt.

Private: needs local/re/eboot.elf. Pins 259 B fragment, fail join, skels start.
Does NOT claim success atexit cluster, helper bodies, or shadowskels.
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
ATTACH = 0x53347F
ATTACH_SIZE = 259
ATTACH_SHA256 = "debc8b3281007b084b65534ff77a9e0726dae3b7114c9c3185512522036d152c"
END = 0x533582
NATIVE = ROOT / "native/core/vslot_40_533100_shadow_attach.hpp"
BASE = ROOT / "native/core/base_initialize_255130.hpp"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def call_target(site: int, img: d.Image) -> int:
    b = img.read(site, 5)
    require(b[0] == 0xE8, f"missing call at {site:#x}")
    rel = struct.unpack("<i", b[1:5])[0]
    return site + 5 + rel


def lea_abs(site: int, img: d.Image) -> int:
    b = img.read(site, 7)
    require(b[0] in (0x48, 0x4C) and b[1] == 0x8D, f"missing lea at {site:#x}")
    rel = struct.unpack("<i", b[3:7])[0]
    return site + 7 + rel


def cstr(img: d.Image, addr: int) -> bytes:
    out = bytearray()
    i = 0
    while True:
        b = img.read(addr + i, 1)[0]
        if b == 0:
            return bytes(out)
        out.append(b)
        i += 1
        if i > 80:
            raise SystemExit(f"FAIL unterminated at {addr:#x}")


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    require(hashlib.sha256(img.read(ATTACH, ATTACH_SIZE)).hexdigest() == ATTACH_SHA256, "sha")
    # fail branch target
    require(img.read(0x53347F, 2) == bytes.fromhex("807d"), "flag cmp")
    je = img.read(0x533483, 2)
    require(je[0] == 0x74, "je")
    fail = 0x533485 + je[1] if je[0] == 0x74 else 0
    # 74 xx is short je
    fail = 0x533485 + struct.unpack("b", bytes([je[1]]))[0]
    require(fail == 0x5334F6, f"fail join {fail:#x}")
    require(call_target(0x5334FC, img) == 0x14D880, "14d880")
    require(call_target(0x533528, img) == 0x14CCB0, "14ccb0")
    require(call_target(0x533530, img) == 0x14CCC0, "14ccc0")
    require(call_target(0x53357D, img) == 0xF590, "f590")
    # shadowskels path starts after end
    require(cstr(img, lea_abs(0x5335A5, img)) == b"$/Models/****/shadowskels.crmp", "skels")

    nh = NATIVE.read_text()
    require("vslot_40_533100_shadow_attach" in nh, "API")
    require("took_fail_path" in nh, "fail path")
    require("NOT modelled" in nh, "omission")
    require("shadow_attach" in BASE.read_text(), "wire")

    print(
        f"OK check_f533100_vslot40_shadow_attach: attach={ATTACH_SHA256[:12]}…; "
        f"fail@0x5334f6; install/14ccb0/14ccc0/f590; end={END:#x}; "
        f"success_atexit/shadowskels NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
