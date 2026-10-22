#!/usr/bin/env python3
"""ELF-tie prologue 0x255180 tail fragment (0x98a90 / app+0x3b58 / app+0x3b40).

Private: needs local/re/eboot.elf. Pins tail sequence 0x255264..0x2552cd.
Does NOT claim bodies of 0x3be050 / 0x3be390 / 0x1ab550 or debug gates after 0x2552cd.
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
PROLOGUE = 0x255180
PROLOGUE_SIZE = 448
PROLOGUE_SHA256 = "9bcd73ea53f39436edb42927a3e69bb6b816adf07c25c8c95e30ced1ab2cde85"
TAIL = 0x255264
TAIL_END = 0x2552CD  # last store self+0x3b40
TAIL_SHA256 = "31eeb31eade18060fc564da97fdc8bd35d6d6cde82b41329d590a6354526bca0"
# Note: TAIL_SHA256 above is for 0x255264..0x255340 (220 B full remainder). Pin install span too.
NATIVE = ROOT / "native/core/prologue_255180.hpp"
SLICE = ROOT / "native/core/appmain_init_prologue_tail_slice.hpp"


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

    body = img.read(PROLOGUE, PROLOGUE_SIZE)
    require(hashlib.sha256(body).hexdigest() == PROLOGUE_SHA256, "prologue sha")

    rem = img.read(TAIL, PROLOGUE + PROLOGUE_SIZE - TAIL)
    require(hashlib.sha256(rem).hexdigest() == TAIL_SHA256, "tail remainder sha")

    require(img.read(0x255264, 5) == bytes.fromhex("bf908a0900"), "mov edi, 0x98a90")
    require(call_target(0x255269, img) == 0x17070, "alloc 0x98a90")
    require(call_target(0x255274, img) == 0x3BE050, "ctor 0x3be050")
    # mov rbx, [r14+0x3b58]
    m = img.read(0x255279, 7)
    require(m[0:3] == bytes.fromhex("498b9e"), "mov rbx,[r14+disp]")
    require(struct.unpack("<i", m[3:7])[0] == 0x3B58, "disp 0x3b58")
    require(call_target(0x25528D, img) == 0x3BE390, "dtor 0x3be390")
    require(call_target(0x255295, img) == 0x170B0, "delete_")
    st = img.read(0x25529A, 7)
    require(st[0:3] == bytes.fromhex("4d89be"), "mov [r14+disp],r15")
    require(struct.unpack("<i", st[3:7])[0] == 0x3B58, "store 0x3b58")

    require(img.read(0x2552A1, 5) == bytes.fromhex("bf40000000"), "mov edi, 0x40")
    require(call_target(0x2552A6, img) == 0x17070, "alloc 0x40")
    require(call_target(0x2552B1, img) == 0x1AB550, "ctor 0x1ab550")
    m2 = img.read(0x2552B6, 7)
    require(m2[0:3] == bytes.fromhex("498bbe"), "mov rdi,[r14+disp]")
    require(struct.unpack("<i", m2[3:7])[0] == 0x3B40, "disp 0x3b40")
    # call [rax+0x08] at 0x2552ca
    require(img.read(0x2552CA, 3) == bytes.fromhex("ff5008"), "vslot+0x08")
    st2 = img.read(0x2552CD, 7)
    require(st2[0:3] == bytes.fromhex("49899e"), "mov [r14+disp],rbx")
    require(struct.unpack("<i", st2[3:7])[0] == 0x3B40, "store 0x3b40")

    # Debug gate follows — not claimed
    require(call_target(0x255309, img) == 0xBA40, "debug 0xba40 after install")

    hdr = NATIVE.read_text()
    require("run_prologue_255180_tail" in hdr, "tail API")
    require("0x2552cd" in hdr.lower() or "NOT modelled" in hdr, "omission")
    require("kPrologue98a90Bytes" in hdr, "size const")
    require("run_appmain_init_prologue_tail_slice" in SLICE.read_text(), "slice API")

    print(
        f"OK check_f255180_prologue_tail: rem={TAIL_SHA256[:12]}…; "
        f"0x98a90→0x3be050→self+0x3b58; 0x40→0x1ab550→self+0x3b40; "
        f"gates/0x3be050 body NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
