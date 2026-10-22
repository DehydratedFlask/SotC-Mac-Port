#!/usr/bin/env python3
"""ELF-tie app vslot 0x533100 cp3Characters attach after load-attempt.

Private: needs local/re/eboot.elf. Pins 165 B fragment, self+0x3cb8, f590 end.
Does NOT claim success atexit notify, helper bodies, or next path.
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
ATTACH = 0x533EAC
ATTACH_SIZE = 165
ATTACH_SHA256 = "386566fe21c971f563714bf74f5c317e937e7099c0c0bf33344fdb08e68932a3"
END = 0x533F51
NATIVE = ROOT / "native/core/vslot_40_533100_cp3characters_attach.hpp"
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
    require(img.read(0x533EAC, 4) == bytes.fromhex("498b9db8"), "load prev [r13+0x3cb8]")
    require(call_target(0x533EB3, img) == 0x14D880, "14d880")
    require(img.read(0x533EC9, 4) == bytes.fromhex("4180bdc0"), "flag [r13+0x3cc0]")
    require(img.read(0x533EF1, 3) == bytes.fromhex("49893f"), "store [r15]")
    require(call_target(0x533EF4, img) == 0x14CCB0, "14ccb0")
    require(call_target(0x533EFC, img) == 0x14CCC0, "14ccc0")
    require(img.read(0x533F01, 2) == bytes.fromhex("80bd"), "success flag cmp")
    require(call_target(0x533F4C, img) == 0xF590, "f590")
    # self = rdi stashed at function entry
    require(img.read(0x53311B, 7) == bytes.fromhex("4889bd00fdffff"), "self→[rbp-0x300]")

    nh = NATIVE.read_text()
    require("vslot_40_533100_cp3characters_attach" in nh, "API")
    require("kVslot40Cp3charactersAttachSlotOff" in nh, "slot")
    require("NOT modelled" in nh, "omission")
    require(
        "cp3characters_attach" in BASE.read_text() or "Cp3charactersAttach" in BASE.read_text(),
        "wire",
    )

    print(
        f"OK check_f533100_vslot40_cp3characters_attach: attach={ATTACH_SHA256[:12]}…; "
        f"self+0x3cb8; install/14ccb0/14ccc0/f590; end={END:#x}; "
        f"success_atexit/next_path NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
