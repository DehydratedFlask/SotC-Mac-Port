#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue ymm inter-cell copy fragment.

Private: needs local/re/eboot.elf. Pins 57 B fragment after ymm BSS seed.
Copies 0x30 B 0x1604278→0x16042a8 and 0x16042dc→0x160430c.
Does NOT claim xmm seed @ 0x531f07+, calls 0x26800/0xf590, or helper bodies.
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
PREV_END = 0x531ECE  # E457 end / this start
FRAG = 0x531ECE
FRAG_SIZE = 57
FRAG_SHA256 = "7ea466637afb823ffeceb5418697fa55a5a0a00d41adf684cd6862c6c6e64eb0"
END = 0x531F07
SRC4278 = 0x1604278
SRC42DC = 0x16042DC
DST42A8 = 0x16042A8
DST430C = 0x160430C
HELPER_26800_SITE = 0x531F86
NATIVE = ROOT / "native/core/init_531d60_post_base_ymm_intercell.hpp"
SLICE = ROOT / "native/core/appmain_init_ctor_98a90_slice.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_ymm_bss.hpp"


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
        f"missing lea at {site:#x}: {b[:3].hex()}",
    )
    disp = struct.unpack("<i", b[3:7])[0]
    return site + 7 + disp


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    require(PREV_END == FRAG, "prev end == frag start")
    # E457 ends immediately before this fragment.
    require("0x531ece" in PREV_NATIVE.read_text().lower() or "531ece" in PREV_NATIVE.read_text(),
            "prev ymm_bss end pin")

    frag = img.read(FRAG, FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest() == FRAG_SHA256, "sha")
    require(len(frag) == FRAG_SIZE, "size")

    # Head: vmovups ymm2,[rsi+0x10]; ymm1,[rsi]; ymm0,[rdi]
    require(frag[0:5] == bytes.fromhex("c5fc105610"), "load ymm2 [rsi+0x10]")
    require(frag[5:9] == bytes.fromhex("c5fc100e"), "load ymm1 [rsi]")
    require(frag[9:13] == bytes.fromhex("c5fc1007"), "load ymm0 [rdi]")
    require(lea_target(FRAG + 13, img) == DST430C, "lea rsi 0x160430c")
    # Stores to [rcx+0x10] / [rcx] (rcx enters as 0x16042a8 from E457 lea)
    require(frag[20:25] == bytes.fromhex("c5fc115110"), "store ymm2→[rcx+0x10]")
    require(frag[25:29] == bytes.fromhex("c5fc1109"), "store ymm1→[rcx]")
    require(frag[29:34] == bytes.fromhex("c5fc104f10"), "load ymm1 [rdi+0x10]")
    require(lea_target(FRAG + 34, img) == 0x16042D8, "lea rcx 0x16042d8 (unclaimed)")
    require(lea_target(FRAG + 41, img) == 0x160433C, "lea rdi 0x160433c (unclaimed)")
    require(frag[48:53] == bytes.fromhex("c5fc114e10"), "store ymm1→[rsi+0x10]")
    require(frag[53:57] == bytes.fromhex("c5fc1106"), "store ymm0→[rsi]")

    # Next: xmm load from constant (not claimed); helpers later.
    require(img.read(END, 4) == bytes.fromhex("c5f81005"), "next xmm load")
    require(call_target(HELPER_26800_SITE, img) == 0x26800, "later 0x26800 (unclaimed)")

    # Confirm E457 lea that primes rcx=0x16042a8 sits inside prior fragment.
    require(lea_target(0x531EA1, img) == DST42A8, "E457 lea rcx→0x16042a8")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_ymm_intercell" in nh, "API")
    require("NOT modelled" in nh, "omission")
    require("0x26800" in nh or "0xf590" in nh, "stop before helpers")
    st = SLICE.read_text()
    require("post_base_ymm_intercell" in st or "PostBaseYmmIntercell" in st, "wire")

    print(
        f"OK check_f531d60_post_base_ymm_intercell: frag={FRAG_SHA256[:12]}…; "
        f"copy 0x30 {SRC4278:#x}->{DST42A8:#x} and {SRC42DC:#x}->{DST430C:#x}; "
        f"end={END:#x}; xmm_seed/26800/f590/1290_body/5c64c0_body/56dcb0_body/"
        f"499f80_body/140d90_body/14c2f0/6a5370 NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
