#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue AVX BSS copy fragment.

Private: needs local/re/eboot.elf. Pins 37 B fragment after 0x5b3cf0 call-site.
Copies 16 B from 0x1b1dd40 to 0x1b1dd00 and 0x1b1dd20. Does NOT claim call
0x7f7220 or later Init continues / helper bodies.
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
PREV = 0x531E12
FRAG = 0x531E17
FRAG_SIZE = 37
FRAG_SHA256 = "8d66f3254a81e85dc9ff744baad0edf8495f8837295b34a099de842bee1e59ca"
END = 0x531E3C
SRC = 0x1B1DD40
DST0 = 0x1B1DD00
DST1 = 0x1B1DD20
NEXT_CALL = 0x7F7220
NATIVE = ROOT / "native/core/init_531d60_post_base_avx_bss.hpp"
SLICE = ROOT / "native/core/appmain_init_ctor_98a90_slice.hpp"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def call_target(site: int, img: d.Image) -> int:
    b = img.read(site, 5)
    require(b[0] == 0xE8, f"missing call at {site:#x}")
    rel = struct.unpack("<i", b[1:5])[0]
    return site + 5 + rel


def lea_target(site: int, img: d.Image) -> int:
    b = img.read(site, 7)
    require(b[0:3] == bytes.fromhex("488d05") or b[0:3] == bytes.fromhex("488d0d"),
            f"missing lea at {site:#x}")
    disp = struct.unpack("<i", b[3:7])[0]
    return site + 7 + disp


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    require(call_target(PREV, img) == 0x5B3CF0, "prev 0x5b3cf0")
    require(img.read(0x5B3CF0, 1) == b"\xc3", "prev target ret")
    frag = img.read(FRAG, FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest() == FRAG_SHA256, "sha")
    require(lea_target(FRAG, img) == SRC, "lea src 0x1b1dd40")
    require(lea_target(FRAG + 7, img) == DST0, "lea dst0 0x1b1dd00")
    # vmovups xmm0,[rax]; vmovups xmm1,[rax]; vmovups [rcx],xmm0
    require(frag[14:26] == bytes.fromhex("c5f81000c5f81008c5f81101"), "vmovups lane")
    require(lea_target(FRAG + 26, img) == DST1, "lea dst1 0x1b1dd20")
    require(frag[33:37] == bytes.fromhex("c5f81109"), "store xmm1")
    require(call_target(END, img) == NEXT_CALL, "next call 0x7f7220 (not claimed)")
    # Staffroll helper body is ~108 B (first ret @ +107) — tourism, not this turn.
    require(img.read(NEXT_CALL, 1) == b"\x55", "0x7f7220 prologue (unclaimed)")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_avx_bss" in nh, "API")
    require("NOT modelled" in nh, "omission")
    require("0x7f7220" in nh, "stop before 7f7220")
    st = SLICE.read_text()
    require("post_base_avx_bss" in st or "PostBaseAvxBss" in st, "wire")

    print(
        f"OK check_f531d60_post_base_avx_bss: frag={FRAG_SHA256[:12]}…; "
        f"copy 16B {SRC:#x}->{DST0:#x}/{DST1:#x}; end={END:#x}; "
        f"7f7220_body/1290_body/5c64c0_body/56dcb0_body/499f80_body/140d90_body/"
        f"14c2f0/6a5370 NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
