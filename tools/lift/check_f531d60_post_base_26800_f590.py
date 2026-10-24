#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue 0x26800/0xf590 call-site fragment.

Private: needs local/re/eboot.elf. Pins 34 B after ymm reload.
Claims dual call-sites only; does NOT claim 0x26800/0xf590 bodies or later pairs.
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
PREV_END = 0x531F71
FRAG = 0x531F71
FRAG_SIZE = 34
FRAG_SHA256 = "67fb1119c9592a17786d296ac118bbd1f5642ee7399532594a81c53f8b1b7893"
END = 0x531F93
CALL_26800 = 0x531F86
CALL_F590 = 0x531F8E
NATIVE = ROOT / "native/core/init_531d60_post_base_26800_f590.hpp"
SLICE = ROOT / "native/core/appmain_init_ctor_98a90_slice.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_ymm_reload.hpp"


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

    require(PREV_END == FRAG, "prev end == frag start")
    require("531f71" in PREV_NATIVE.read_text().lower(), "prev ymm_reload end pin")

    frag = img.read(FRAG, FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest() == FRAG_SHA256, "sha")
    require(len(frag) == FRAG_SIZE, "size")
    # Head prep (unclaimed semantics): vxorps xmm0,xmm0
    require(frag[0:3] == bytes.fromhex("c5f857"), "vxorps head")
    require(call_target(CALL_26800, img) == 0x26800, "call 0x26800")
    # mov rdi,rbx sits between the two calls (+0x1a)
    require(frag[0x1A:0x1D] == bytes.fromhex("4889df"), "mov rdi,rbx")
    require(call_target(CALL_F590, img) == 0xF590, "call 0xf590")
    # Bodies are push rbp — tourism, not this turn.
    require(img.read(0x26800, 1) == b"\x55", "0x26800 prologue (unclaimed)")
    require(img.read(0xF590, 1) == b"\x55", "0xf590 prologue (unclaimed)")
    # Next edge starts another string pair.
    require(img.read(END, 3) == bytes.fromhex("488d9c"), "next lea rbx stack")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_26800_f590" in nh, "API")
    require("NOT modelled" in nh, "omission")
    require("0x26800" in nh and "0xf590" in nh, "targets")
    st = SLICE.read_text()
    require("post_base_26800_f590" in st or "PostBase26800F590" in st, "wire")

    print(
        f"OK check_f531d60_post_base_26800_f590: frag={FRAG_SHA256[:12]}…; "
        f"calls 0x26800@0x531f86 + 0xf590@0x531f8e; end={END:#x}; "
        f"26800_body/f590_body/later_string_pairs/1290_body/5c64c0_body/"
        f"56dcb0_body/499f80_body/140d90_body/14c2f0/6a5370 NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
