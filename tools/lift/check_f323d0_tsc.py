#!/usr/bin/env python3
"""ELF-tie TSC leaf 0x323d0 (called from ctor 0x3be050 at obj+0x98a88).

Private: needs local/re/eboot.elf. Pins body, cells 0x2ea4700/0x2ea4708, rdtsc,
and PLT call 0xf8f860. Does NOT claim PLT body.
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
LEAF = 0x323D0
LEAF_SIZE = 67
LEAF_SHA256 = "32da90bc4ce93a445148b51b3131ce7ca4ef6a5c88116c4c80cf0b87a437f70c"
GUARD = 0x2EA4700
GLOBAL = 0x2EA4708
NATIVE = ROOT / "native/core/store_tsc_323d0.hpp"
CTOR = ROOT / "native/core/ctor_98a90_3be050.hpp"


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


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    body = img.read(LEAF, LEAF_SIZE)
    require(hashlib.sha256(body).hexdigest() == LEAF_SHA256, "leaf sha")
    require(body[-1] == 0xC3, "ends ret")

    require(lea_abs(0x323D7, img) == GUARD, "lea guard 0x2ea4700")
    require(lea_abs(0x323F1, img) == GLOBAL, "lea global 0x2ea4708")
    require(img.read(0x323E1, 4) == bytes.fromhex("49833e00"), "cmp [r14],0")
    require(img.read(0x323E5, 2) == bytes.fromhex("751b"), "jne skip_init")
    require(call_target(0x323E7, img) == 0xF8F860, "call 0xf8f860")
    require(img.read(0xF8F860, 2) == bytes.fromhex("ff25"), "0xf8f860 is PLT")
    require(img.read(0x323EF, 2) == bytes.fromhex("0f31"), "rdtsc (init path)")
    require(img.read(0x32402, 2) == bytes.fromhex("0f31"), "rdtsc (always)")
    require(img.read(0x3240B, 3) == bytes.fromhex("488913"), "mov [rbx],rdx")

    # Callsite from ctor
    require(call_target(0x3BE0C2, img) == LEAF, "ctor calls 0x323d0")
    require(img.read(0x3BE0BB, 7) == bytes.fromhex("498dbe888a0900"), "lea +0x98a88")

    nh = NATIVE.read_text()
    require("store_tsc_323d0" in nh, "API")
    require("host_rdtsc64" in nh, "host rdtsc")
    require("NOT modelled" in nh or "not modelled" in nh.lower() or "NOT claimed" in nh, "omission")
    require("store_tsc_323d0" in CTOR.read_text(), "ctor wires leaf")

    print(
        f"OK check_f323d0_tsc: body={LEAF_SHA256[:12]}… ({LEAF_SIZE} B); "
        f"cells {GUARD:#x}/{GLOBAL:#x}; rdtsc+PLT pin; PLT body NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
