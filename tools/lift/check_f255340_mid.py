#!/usr/bin/env python3
"""ELF-tie Base Initialize mid 0x255340 (debug-menu registration).

Private: needs local/re/eboot.elf. Pins body, strings, attach offsets/cells, callees.
Does NOT claim bodies of 0x26ea0 / 0x26b60 / 0x272d0 / 0xf590 / vslot+0x50 / stack-chk.
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
MID = 0x255340
MID_SIZE = 712
MID_SHA256 = "abc3f6c0adf8f1a73f9b0ac29e96bbc7c4a2089841436a79a4785d2b1c04bad2"
NATIVE = ROOT / "native/core/mid_255340.hpp"
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

    require(hashlib.sha256(img.read(MID, MID_SIZE)).hexdigest() == MID_SHA256, "mid sha")
    require(img.read(MID + MID_SIZE - 1, 1) == b"\xc3", "ret")

    require(call_target(0x255152, img) == MID, "base calls mid")
    require(img.read(0x255369, 3) == bytes.fromhex("ff5050"), "vslot+0x50")
    require(call_target(0x2553DA, img) == 0x26EA0, "26ea0 #1")
    require(call_target(0x2553E2, img) == 0xF590, "f590")
    require(call_target(0x255456, img) == 0x26EA0, "26ea0 #2")
    require(call_target(0x2554AB, img) == 0x26B60, "26b60")
    require(call_target(0x255522, img) == 0x26EA0, "26ea0 #3")
    require(call_target(0x2555A1, img) == 0x26EA0, "26ea0 #4")
    require(call_target(0x2555B9, img) == 0x272D0, "272d0")

    require(cstr(img, lea_abs(0x25537A, img)) == b"Debug Game", "Debug Game")
    require(cstr(img, lea_abs(0x255381, img)) == b"Toggle Debug Camera", "Toggle")
    require(cstr(img, lea_abs(0x25540B, img)) == b"Mirror Game World", "Mirror")
    require(cstr(img, lea_abs(0x255479, img)) == b"Debug", "Debug")
    require(cstr(img, lea_abs(0x2554D4, img)) == b"Fixed TimeStep", "Fixed")
    require(
        cstr(img, lea_abs(0x25555A, img)) == b"DeltaTime Correction Tolorance",
        "DeltaTol (OBS spelling)",
    )

    # self+0x2688 / self+0x2868
    require(img.read(0x25538C, 7) == bytes.fromhex("498d8d88260000"), "lea rcx,[r13+0x2688]")
    require(img.read(0x2554B8, 7) == bytes.fromhex("498d8d68280000"), "lea rcx,[r13+0x2868]")
    require(lea_abs(0x2553EF, img) == 0x2DFD1D0, "cell 0x2dfd1d0")
    require(lea_abs(0x255537, img) == 0x2EECDE8, "cell 0x2eecde8")
    require(img.read(0x2555F4, 1) == b"\x75", "jne stack-chk")

    nh = NATIVE.read_text()
    require("mid_255340" in nh, "API")
    require("NOT modelled" in nh or "not modelled" in nh.lower(), "omission")
    bh = BASE.read_text()
    require("use_lifted_mid_255340" in bh, "base wires")
    require("Mid255340Hooks" in bh or "mid_255340" in bh, "base includes")

    print(
        f"OK check_f255340_mid: mid={MID_SHA256[:12]}…; "
        f"4x26ea0+26b60+names/cells pinned; "
        f"helper bodies/stack-chk NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
