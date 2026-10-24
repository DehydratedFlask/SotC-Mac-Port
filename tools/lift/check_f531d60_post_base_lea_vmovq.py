#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue lea rbx / vmovq fragment.

Private: needs local/re/eboot.elf. Pins 15 B after E464 0x52eb50 call-site.
Does NOT claim stack prep or call 0x532ca0 / trampoline bodies.
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
PREV_END = 0x532036
FRAG = 0x532036
FRAG_SIZE = 15
FRAG_SHA256 = "97b753ed55e1a62a915522804975439ae6a02b8dd00f027e5ba7b06af21670e4"
END = 0x532045
RBX_CELL = 0x32F79A8
SRC = 0x15FFA38
NATIVE = ROOT / "native/core/init_531d60_post_base_lea_vmovq.hpp"
SLICE = ROOT / "native/core/appmain_init_ctor_98a90_slice.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_52eb50.hpp"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    require(PREV_END == FRAG, "prev end == frag start")
    require(
        "kInit531d60PostBase52eb50EndRva = 0x532036" in PREV_NATIVE.read_text(),
        "52eb50 end",
    )
    frag = img.read(FRAG, FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest() == FRAG_SHA256, "sha")
    require(frag[0:3] == bytes.fromhex("488d1d"), "lea rbx head")
    lea_rel = struct.unpack("<i", frag[3:7])[0]
    require(FRAG + 7 + lea_rel == RBX_CELL, "lea rbx → 0x32f79a8")
    require(frag[7:11] == bytes.fromhex("c5fa7e05"), "vmovq xmm0 head")
    vmovq_rel = struct.unpack("<i", frag[11:15])[0]
    require(FRAG + 15 + vmovq_rel == SRC, "vmovq → 0x15ffa38")
    # Constant is file-backed (two packed u64 RVAs); pin first qword only.
    src = img.read(SRC, 8)
    require(struct.unpack("<Q", src)[0] == 0x538530, "src qword0")
    # Next edge starts with lea rsi (stack prep) — not claimed.
    require(img.read(END, 4) == bytes.fromhex("488db424"), "next lea rsi")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_lea_vmovq" in nh, "API")
    require("NOT modelled" in nh, "omission")
    require("0x32f79a8" in nh and "0x15ffa38" in nh, "pins")
    st = SLICE.read_text()
    require("post_base_lea_vmovq" in st or "PostBaseLeaVmovq" in st, "wire")

    print(
        f"OK check_f531d60_post_base_lea_vmovq: frag={FRAG_SHA256[:12]}…; "
        f"lea rbx→{RBX_CELL:#x} + vmovq←{SRC:#x}; end={END:#x}; "
        f"stack_prep/532ca0_body/52eb50_body/26800_body/f590_body/"
        f"7f7220_body/1290_body/5c64c0_body/56dcb0_body/499f80_body/"
        f"140d90_body/14c2f0/6a5370 NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
