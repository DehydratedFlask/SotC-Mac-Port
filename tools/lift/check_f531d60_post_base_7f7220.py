#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue call-site for 0x7f7220.

Private: needs local/re/eboot.elf. Pins 5 B fragment after AVX BSS copy.
Does NOT claim staffroll helper body 0x7f7220 (~108 B) or later Init continues.
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
PREV_END = 0x531E3C  # AVX BSS end / this call-site
FRAG = 0x531E3C
FRAG_SIZE = 5
FRAG_SHA256 = "fe4ff86e2987347a09b1b5c2ace53eb3cc1b9ffdb205d18863f472dca7e67bb8"
END = 0x531E41
TARGET = 0x7F7220
NEXT = 0x56FAD0
NATIVE = ROOT / "native/core/init_531d60_post_base_7f7220.hpp"
SLICE = ROOT / "native/core/appmain_init_ctor_98a90_slice.hpp"
AVX = ROOT / "native/core/init_531d60_post_base_avx_bss.hpp"


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

    # Prev always-on is AVX BSS ending at this call
    require("kInit531d60PostBaseAvxBssEndRva = 0x531e3c" in AVX.read_text(), "avx end")
    require(hashlib.sha256(img.read(FRAG, FRAG_SIZE)).hexdigest() == FRAG_SHA256, "sha")
    require(call_target(FRAG, img) == TARGET, "call 0x7f7220")
    require(img.read(TARGET, 1) == b"\x55", "0x7f7220 push rbp (body unclaimed)")
    # Body tourism size pin: first ret at +107 → 108 B
    body = img.read(TARGET, 120)
    first_ret = body.index(b"\xc3")
    require(first_ret == 107, f"unexpected first ret +{first_ret}")
    require(call_target(END, img) == NEXT, "next 0x56fad0 (not claimed)")
    require(img.read(NEXT, 1) == b"\xc3", "0x56fad0 is ret")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_7f7220" in nh, "API")
    require("NOT modelled" in nh, "omission")
    require("staffroll" in nh.lower() or "0x7f7220 body" in nh or "body NOT" in nh, "body omit")
    st = SLICE.read_text()
    require("post_base_7f7220" in st or "PostBase7f7220" in st, "wire")

    print(
        f"OK check_f531d60_post_base_7f7220: frag={FRAG_SHA256[:12]}…; "
        f"call {TARGET:#x} (staffroll helper body NOT claimed, {first_ret + 1} B); "
        f"end={END:#x}; "
        f"7f7220_body/56fad0/1290_body/5c64c0_body/56dcb0_body/499f80_body/140d90_body/"
        f"14c2f0/6a5370 NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
