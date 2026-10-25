#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue xmm BSS seed fragment.

Private: needs local/re/eboot.elf. Pins 53 B fragment after ymm inter-cell.
Seeds 0x16042d8 and overwrites head of 0x160430c from 0x1014be0.
Does NOT claim ymm reload→0x160433c, CP10/stack, or 0x26800/0xf590 bodies.
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
PREV_END = 0x531F07
FRAG = 0x531F07
FRAG_SIZE = 53
FRAG_SHA256 = "97400c254a60fac99ea29c935d99a69b10d73d90ac00f88964acbe3566564eec"
END = 0x531F3C
SRC = 0x1014BE0
DST42D8 = 0x16042D8
DST430C = 0x160430C
HELPER_26800_SITE = 0x531F86
NATIVE = ROOT / "native/core/init_531d60_post_base_xmm_seed.hpp"
SLICE = ROOT / "native/core/appmain_init_ctor_98a90_slice.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_ymm_intercell.hpp"


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
    require(
        b[0:3]
        in (
            bytes.fromhex("488d05"),
            bytes.fromhex("488d0d"),
            bytes.fromhex("488d35"),
            bytes.fromhex("488d3d"),
        ),
        f"missing lea at {site:#x}",
    )
    disp = struct.unpack("<i", b[3:7])[0]
    return site + 7 + disp


def rip_vmovups_xmm_target(site: int, img: d.Image) -> int:
    b = img.read(site, 8)
    require(b[0:4] == bytes.fromhex("c5f81005"), f"missing vmovups xmm0,[rip] at {site:#x}")
    disp = struct.unpack("<i", b[4:8])[0]
    return site + 8 + disp


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    require(PREV_END == FRAG, "prev end == frag start")
    require("531f07" in PREV_NATIVE.read_text().lower(), "prev intercell end pin")

    frag = img.read(FRAG, FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest() == FRAG_SHA256, "sha")
    require(len(frag) == FRAG_SIZE, "size")
    require(rip_vmovups_xmm_target(FRAG, img) == SRC, "src 0x1014be0")
    require(frag[8:14] == bytes.fromhex("c7010000803f"), "dword [rcx]=1.0")
    require(frag[14:24] == bytes.fromhex("48b9000000000000803f"), "mov rcx,imm64")
    require(frag[24:30] == bytes.fromhex("c706000080bf"), "dword [rsi]=-1.0")
    require(frag[30:37] == bytes.fromhex("c7460400000000"), "dword [rsi+4]=0")
    require(frag[37:44] == bytes.fromhex("c7460800000000"), "dword [rsi+8]=0")
    require(frag[44:49] == bytes.fromhex("c5f811460c"), "store xmm→[rsi+0xc]")
    require(frag[49:53] == bytes.fromhex("48894e1c"), "qword [rsi+0x1c]=rcx")

    src = img.read(SRC, 16)
    require(src == bytes.fromhex("000000000000803f0000000000000000"), "src (0,1,0,0)")

    # E458 primes rcx/rsi for this fragment.
    require(lea_target(0x531EF0, img) == DST42D8, "E458 lea rcx→0x16042d8")
    require(lea_target(0x531EDB, img) == DST430C, "E458 lea rsi→0x160430c")

    # Next: mov ecx,4 then ymm reload (not claimed); helpers later.
    require(img.read(END, 5) == bytes.fromhex("b904000000"), "next mov ecx,4")
    require(call_target(HELPER_26800_SITE, img) == 0x26800, "later 0x26800 (unclaimed)")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_xmm_seed" in nh, "API")
    require("NOT modelled" in nh, "omission")
    require("0x26800" in nh or "0xf590" in nh or "CP10" in nh, "stop before helpers")
    st = SLICE.read_text()
    require("post_base_xmm_seed" in st or "PostBaseXmmSeed" in st, "wire")

    print(
        f"OK check_f531d60_post_base_xmm_seed: frag={FRAG_SHA256[:12]}…; "
        f"seed {SRC:#x}->{DST42D8:#x}/{DST430C:#x}; end={END:#x}; "
        f"ymm_reload/CP10/26800/f590/1290_body/5c64c0_body/56dcb0_body/"
        f"499f80_body/140d90_body/14c2f0/6a5370 NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
