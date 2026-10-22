#!/usr/bin/env python3
"""ELF-tie Init 0xf08 singleton install OBS 0x531d84..0x531dcf.

Private: needs local/re/eboot.elf. Pins alloc size, cell, callees, order before base.
Does NOT claim bodies of 0x523bd0 / 0x525b40 / 0x17070 / 0x170b0.
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
INSTALL = 0x531D84
INSTALL_SIZE = 75
INSTALL_SHA256 = "f7dc34b5ca7726cb2c454d96bc77fab45c57d1a4f0d2dfc665ec215f30889ef2"
NATIVE = ROOT / "native/core/singleton_f08_531d84.hpp"
SLICE = ROOT / "native/core/appmain_init_ctor_98a90_slice.hpp"
BASE_SLICE = ROOT / "native/core/appmain_init_base_initialize_slice.hpp"


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

    require(
        hashlib.sha256(img.read(INSTALL, INSTALL_SIZE)).hexdigest() == INSTALL_SHA256,
        "install frag sha",
    )
    # mov edi, 0xf08
    require(img.read(INSTALL, 5) == bytes.fromhex("bf080f0000"), "mov edi,0xf08")
    require(call_target(0x531D98, img) == 0x17070, "alloc 0x17070")
    require(call_target(0x531DA3, img) == 0x523BD0, "ctor 0x523bd0")
    require(lea_abs(0x531DA8, img) == 0x2EFC9E0, "cell 0x2efc9e0")
    require(call_target(0x531DBF, img) == 0x525B40, "dtor 0x525b40")
    require(call_target(0x531DC7, img) == 0x170B0, "delete 0x170b0")
    require(call_target(0x531DDA, img) == 0x255130, "then base Initialize")
    # store [r14], r12 before base
    require(img.read(0x531DCC, 3) == bytes.fromhex("4d8926"), "store cell")

    nh = NATIVE.read_text()
    require("install_singleton_f08_531d84" in nh, "API")
    require("0x2efc9e0" in nh or "2efc9e0" in nh, "cell pin")
    require("NOT modelled" in nh, "omission")
    require("0xf08" in nh or "0xF08" in nh or "kSingletonF08ObjectBytes" in nh, "size")
    sh = SLICE.read_text()
    require("singleton" in sh.lower() or "0xf08" in sh, "ctor slice wires")
    bh = BASE_SLICE.read_text()
    require("singleton" in bh.lower() or "0xf08" in bh, "base slice wires")

    print(
        f"OK check_f531d84_singleton_f08: install={INSTALL_SHA256[:12]}…; "
        f"size=0xf08 cell=0x2efc9e0 before base; "
        f"ctor/dtor bodies NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
