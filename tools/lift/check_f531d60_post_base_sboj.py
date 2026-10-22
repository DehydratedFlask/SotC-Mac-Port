#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue Shadow Boss Outro Jump call-site fragment.

Private: needs local/re/eboot.elf. Pins 79 B after E462 SBJ pair.
Claims dual call-sites only; does NOT claim bodies or call 0x52eb50.
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
PREV_END = 0x531FE2
FRAG = 0x531FE2
FRAG_SIZE = 79
FRAG_SHA256 = "f91d994018147eceea0d235f631d216d0a791f218ea6b705d5e06b4c8174c6ea"
END = 0x532031
CALL_26800 = 0x532024
CALL_F590 = 0x53202C
STRING = 0x1014D08
NATIVE = ROOT / "native/core/init_531d60_post_base_sboj.hpp"
SLICE = ROOT / "native/core/appmain_init_ctor_98a90_slice.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_sbj.hpp"


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
    require("531fe2" in PREV_NATIVE.read_text().lower(), "prev sbj end pin")

    frag = img.read(FRAG, FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest() == FRAG_SHA256, "sha")
    require(len(frag) == FRAG_SIZE, "size")
    require(frag[0:4] == bytes.fromhex("488d9c24"), "lea rbx head")
    require(frag[8:10] == bytes.fromhex("488d"), "lea rax")
    require(img.read(STRING, 22).startswith(b"Shadow Boss Outro Jump"), "string")
    require(call_target(CALL_26800, img) == 0x26800, "call 0x26800")
    require(frag[0x47:0x4A] == bytes.fromhex("4889df"), "mov rdi,rbx")
    require(call_target(CALL_F590, img) == 0xF590, "call 0xf590")
    require(img.read(0x26800, 1) == b"\x55", "0x26800 prologue (unclaimed)")
    require(img.read(0xF590, 1) == b"\x55", "0xf590 prologue (unclaimed)")
    # Next edge is call 0x52eb50 — not claimed this turn.
    require(call_target(END, img) == 0x52EB50, "next call 0x52eb50")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_sboj" in nh, "API")
    require("NOT modelled" in nh, "omission")
    require("0x26800" in nh and "0xf590" in nh, "targets")
    require("Shadow Boss Outro Jump" in nh, "string name")
    st = SLICE.read_text()
    require("post_base_sboj" in st or "PostBaseSboj" in st, "wire")

    print(
        f"OK check_f531d60_post_base_sboj: frag={FRAG_SHA256[:12]}…; "
        f"calls 0x26800@0x532024 + 0xf590@0x53202c; end={END:#x}; "
        f"26800_body/f590_body/52eb50/1290_body/5c64c0_body/"
        f"56dcb0_body/499f80_body/140d90_body/14c2f0/6a5370 NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
