#!/usr/bin/env python3
"""ELF-tie the Init 0x531d60 registry fragment (alloc → fill → store 0x2efd160).

Private: needs local/re/eboot.elf. Pins Init body digest and the straight-line sequence
at 0x532b0c..0x532bf8:
  mov edi, 0x98; call 0x17070; call 0x54dc10; call 0x54dc20; call 0x52a220;
  lea rbx, 0x2efd160; …; movq r14, (rbx).
Does NOT claim the rest of Init or listener-slot dtor fidelity.
"""

from __future__ import annotations

import hashlib
import re
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
INIT = 0x531D60
INIT_SIZE = 3890
INIT_SHA256 = "cc36f59699fbc400ed76002c08567620d26fa41dc33bd689627f4e6ed65fceb4"
FRAG = 0x532B0C
NATIVE = ROOT / "native/core/registry_init_531d60.hpp"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def call_target(abs_call_site: int, img: d.Image) -> int:
    b = img.read(abs_call_site, 5)
    require(b[0] == 0xE8, f"missing call at {abs_call_site:#x}")
    rel = struct.unpack("<i", b[1:5])[0]
    return abs_call_site + 5 + rel


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    init_body = img.read(INIT, INIT_SIZE)
    require(len(init_body) == INIT_SIZE, "0x531d60 size")
    digest = hashlib.sha256(init_body).hexdigest()
    require(digest == INIT_SHA256, f"0x531d60 sha changed: {digest}")

    require(img.read(FRAG, 5) == bytes.fromhex("bf98000000"), "mov edi, 0x98")
    require(call_target(0x532B11, img) == 0x17070, "alloc 0x17070")
    require(call_target(0x532B19, img) == 0x54DC10, "table base 0x54dc10")
    require(call_target(0x532B21, img) == 0x54DC20, "count 0x54dc20")
    require(call_target(0x532B2E, img) == 0x52A220, "fill 0x52a220")

    lea = img.read(0x532B33, 7)
    require(lea[0:3] == bytes.fromhex("488d1d"), "lea rbx, 0x2efd160")
    disp = struct.unpack("<i", lea[3:7])[0]
    require(0x532B33 + 7 + disp == 0x2EFD160, "lea target")

    require(img.read(0x532BF8, 3) == bytes.fromhex("4c8933"), "store r14 → [rbx]")

    native = NATIVE.read_text()
    require("init_registry_fragment_531d60" in native, "API")
    require("0x531d60" in native and "0x2efd160" in native, "RVA comments")
    require(re.search(r"kRegistryInitObjectBytes\s*=\s*0x98", native), "alloc size")
    require("NOT modelled" in native or "not modelled" in native.lower(), "omission")
    require("registry_cell_2efd160" in native, "global cell")

    print(
        "OK check_f531d60_registry: fragment alloc→fill→store pinned; "
        f"0x531d60={digest[:12]}…"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
