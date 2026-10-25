#!/usr/bin/env python3
"""ELF-tie the measured app-main Init→registry slice landmarks.

Private: needs local/re/eboot.elf. Pins:
  vtable 0x15c8cd8 base; +0x18 @ 0x15c8cf0 → Run 0x25a8d0;
  +0x20 @ 0x15c8cf8 → Init 0x531d60; callsite 0x25aa06 = call [rax+0x20].
Re-pins Init registry fragment digest via check_f531d60_registry landmarks.
Requires native header to document PR #5 heap merge gap (E399–E403 cite-only).
Does NOT claim full Init, pools/listeners, or G1 PASSED.
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
INIT = 0x531D60
INIT_SIZE = 3890
INIT_SHA256 = "cc36f59699fbc400ed76002c08567620d26fa41dc33bd689627f4e6ed65fceb4"
VT_BASE = 0x15C8CD8
VT_RUN = 0x15C8CF0
VT_INIT = 0x15C8CF8
RUN = 0x25A8D0
CALLSITE = 0x25AA06
NATIVE = ROOT / "native/core/appmain_init_registry_slice.hpp"
NATIVE_CPP = ROOT / "native/core/appmain_init_registry_slice.cpp"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def u64(img: d.Image, addr: int) -> int:
    return struct.unpack("<Q", img.read(addr, 8))[0]


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    require(u64(img, VT_RUN) == RUN, "vtable +0x18 → Run")
    require(u64(img, VT_INIT) == INIT, "vtable +0x20 → Init 0x531d60")
    require(img.read(CALLSITE, 3) == bytes.fromhex("ff5020"), "call [rax+0x20] @ 0x25aa06")

    init_body = img.read(INIT, INIT_SIZE)
    require(len(init_body) == INIT_SIZE, "Init size")
    digest = hashlib.sha256(init_body).hexdigest()
    require(digest == INIT_SHA256, f"Init sha changed: {digest}")

    # Fragment still present (alloc→fill→store) — slice consumes that API.
    require(img.read(0x532B0C, 5) == bytes.fromhex("bf98000000"), "mov edi, 0x98")

    hdr = NATIVE.read_text()
    require("run_appmain_init_registry_slice" in hdr, "slice API")
    require("0x15c8cf8" in hdr and "0x531d60" in hdr, "vtable/Init landmarks")
    require("merge gap" in hdr.lower() or "NOT an ancestor" in hdr, "merge-gap honesty")
    require("E399" in hdr and "E403" in hdr, "PR5 evidence cite")
    require("4d7e484" in hdr or "0510d35" in hdr, "PR5 tip cite")
    require("NOT modelled" in hdr or "not modelled" in hdr.lower(), "omission")
    require("recovered_constructors" in hdr, "not registered into CRT runner")
    require(NATIVE_CPP.is_file(), "slice cpp")
    require("init_registry_fragment_531d60" in NATIVE_CPP.read_text(), "wires Init fragment")

    # Tip must NOT contain PR #5 heap modules (merge gap observation).
    for name in (
        "heap_allocator.hpp",
        "bucket_allocator.hpp",
        "composite_allocator.hpp",
        "allocator_bootstrap.hpp",
    ):
        require(not (ROOT / "native/core" / name).is_file(), f"unexpected PR5 module on tip: {name}")

    print(
        f"OK check_fappmain_init_registry_slice: vtable slot4→{INIT:#x}; "
        f"callsite {CALLSITE:#x}; Init={digest[:12]}…; PR5 merge gap documented"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
