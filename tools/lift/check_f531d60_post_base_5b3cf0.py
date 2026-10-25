#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue call-site for 0x5b3cf0.

Private: needs local/re/eboot.elf. Pins 5 B fragment after 0x5b3d00.
Target 0x5b3cf0 is a one-byte ret (like E406/E450/E451 ret-nops). Does NOT
claim following AVX BSS copy or later Init continues.
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
FRAG = 0x531E12
FRAG_SIZE = 5
FRAG_SHA256 = "b6984f320353f9cf97e320b0631ad9f5b439cc31d28bc34a2b1ccfa8899fe87d"
END = 0x531E17
TARGET = 0x5B3CF0
NATIVE = ROOT / "native/core/init_531d60_post_base_5b3cf0.hpp"
SLICE = ROOT / "native/core/appmain_init_ctor_98a90_slice.hpp"


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

    require(call_target(0x531E0D, img) == 0x5B3D00, "prev 0x5b3d00")
    require(img.read(0x5B3D00, 1) == b"\xc3", "prev target ret")
    require(hashlib.sha256(img.read(FRAG, FRAG_SIZE)).hexdigest() == FRAG_SHA256, "sha")
    require(call_target(FRAG, img) == TARGET, "call 0x5b3cf0")
    require(img.read(TARGET, 1) == b"\xc3", "0x5b3cf0 is ret")
    # Next is lea, not a call
    nxt = img.read(END, 7)
    require(nxt[0:3] == bytes.fromhex("488d05"), "next lea (not claimed)")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_5b3cf0" in nh, "API")
    require("NOT modelled" in nh, "omission")
    require("post_base_5b3cf0" in SLICE.read_text() or "PostBase5b3cf0" in SLICE.read_text(), "wire")

    print(
        f"OK check_f531d60_post_base_5b3cf0: frag={FRAG_SHA256[:12]}…; "
        f"call {TARGET:#x} (ret-nop); end={END:#x}; "
        f"avx_bss_copy/7f7220/1290_body/5c64c0_body/56dcb0_body/499f80_body/140d90_body/"
        f"14c2f0/6a5370 NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
