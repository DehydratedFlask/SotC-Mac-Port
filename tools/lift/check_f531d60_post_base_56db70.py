#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue call-site for 0x56db70.

Private: needs local/re/eboot.elf. Pins 5 B fragment after 0x1290.
Target 0x56db70 is a one-byte ret (like E406 0x56dca0). Does NOT claim
later Init continues.
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
FRAG = 0x531E08
FRAG_SIZE = 5
FRAG_SHA256 = "55982090acafd363882e33cd4df8f61c342f4e7288c764239c3ab7fb122570ee"
END = 0x531E0D
TARGET = 0x56DB70
NATIVE = ROOT / "native/core/init_531d60_post_base_56db70.hpp"
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

    require(call_target(0x531E03, img) == 0x1290, "prev 0x1290")
    require(hashlib.sha256(img.read(FRAG, FRAG_SIZE)).hexdigest() == FRAG_SHA256, "sha")
    require(call_target(FRAG, img) == TARGET, "call 0x56db70")
    require(img.read(TARGET, 1) == b"\xc3", "0x56db70 is ret")
    require(call_target(END, img) == 0x5B3D00, "next 0x5b3d00 (not claimed)")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_56db70" in nh, "API")
    require("NOT modelled" in nh, "omission")
    require("post_base_56db70" in SLICE.read_text() or "PostBase56db70" in SLICE.read_text(), "wire")

    print(
        f"OK check_f531d60_post_base_56db70: frag={FRAG_SHA256[:12]}…; "
        f"call {TARGET:#x} (ret-nop); end={END:#x}; "
        f"5b3d00/1290_body/5c64c0_body/56dcb0_body/499f80_body/140d90_body/14c2f0/6a5370 NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
