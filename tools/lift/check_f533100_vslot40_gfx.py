#!/usr/bin/env python3
"""ELF-tie app vslot 0x533100 post-join gfx-cell prelude (0x2ee2d00).

Private: needs local/re/eboot.elf. Pins 36 B fragment, callees, 0x147e10 body.
Does NOT claim 0x6a5370 / 0x6abcc0 bodies or shadowmodels.crmp path.
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
GFX = 0x533411
GFX_SIZE = 36
GFX_SHA256 = "6cc664e29f1c16e6f5b3069e2e74e2c056dcc3288882e0ebfd71e769d386c696"
END = 0x533435
NATIVE = ROOT / "native/core/vslot_40_533100_gfx_2ee2d00.hpp"
BASE = ROOT / "native/core/base_initialize_255130.hpp"
BODY_147E10 = bytes.fromhex("897740c3")  # mov [rdi+0x40], esi; ret


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

    require(hashlib.sha256(img.read(GFX, GFX_SIZE)).hexdigest() == GFX_SHA256, "gfx sha")
    require(lea_abs(0x533411, img) == 0x2EE2D00, "cell 0x2ee2d00")
    require(call_target(0x53341B, img) == 0x6A5370, "6a5370")
    require(call_target(0x533423, img) == 0x6ABCC0, "6abcc0")
    require(img.read(0x53342B, 5) == bytes.fromhex("be2a2a2a2a"), "imm 0x2a2a2a2a")
    require(call_target(0x533430, img) == 0x147E10, "147e10")
    require(img.read(0x147E10, 4) == BODY_147E10, "147e10 body")
    # next lea starts shadowmodels path
    require(lea_abs(0x53344A, img) == 0x1014E65, "shadowmodels path after end")
    from ps4_dynlink import Image  # noqa: F401

    s = bytearray()
    i = 0
    while True:
        b = img.read(0x1014E65 + i, 1)[0]
        if b == 0:
            break
        s.append(b)
        i += 1
        if i > 80:
            raise SystemExit("FAIL unterminated path")
    require(bytes(s) == b"$/Models/****/shadowmodels.crmp", "shadowmodels name")

    nh = NATIVE.read_text()
    require("vslot_40_533100_gfx_2ee2d00" in nh, "API")
    require("store_u32_at_40_147e10" in nh, "147e10 lift")
    require("NOT modelled" in nh, "omission")
    require("vslot_40_gfx" in BASE.read_text() or "gfx_2ee2d00" in BASE.read_text(), "wire")

    print(
        f"OK check_f533100_vslot40_gfx: gfx={GFX_SHA256[:12]}…; "
        f"cell=0x2ee2d00; 6a5370/6abcc0/147e10; marker; end={END:#x}; "
        f"6a5370/6abcc0/shadowmodels NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
