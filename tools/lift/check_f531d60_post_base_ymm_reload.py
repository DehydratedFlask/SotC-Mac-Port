#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue ymm reload→0x160433c fragment.

Private: needs local/re/eboot.elf. Pins 53 B fragment after xmm BSS seed.
Copies 0x30 B 0x160430c→0x160433c. Does NOT claim CP10/stack or 0x26800/0xf590.
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
PREV_END = 0x531F3C
FRAG = 0x531F3C
FRAG_SIZE = 53
FRAG_SHA256 = "afdfaa6c0d67e1200840d30380360ec9c39c6206bc58832d29c7f7093adec90f"
END = 0x531F71
SRC430C = 0x160430C
DST433C = 0x160433C
CP10 = 0x1014CF2
HELPER_26800_SITE = 0x531F86
NATIVE = ROOT / "native/core/init_531d60_post_base_ymm_reload.hpp"
SLICE = ROOT / "native/core/appmain_init_ctor_98a90_slice.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_xmm_seed.hpp"


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


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    require(PREV_END == FRAG, "prev end == frag start")
    require("531f3c" in PREV_NATIVE.read_text().lower(), "prev xmm_seed end pin")

    frag = img.read(FRAG, FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest() == FRAG_SHA256, "sha")
    require(len(frag) == FRAG_SIZE, "size")
    require(frag[0:5] == bytes.fromhex("b904000000"), "mov ecx,4")
    require(frag[5:10] == bytes.fromhex("c5fe6f4e10"), "vmovdqu ymm1,[rsi+0x10]")
    require(frag[10:14] == bytes.fromhex("c5fc1006"), "vmovups ymm0,[rsi]")
    require(lea_target(FRAG + 14, img) == CP10, "lea CP10 (unclaimed)")
    # After stack prep: stores to [rdi]
    require(frag[44:49] == bytes.fromhex("c5fe7f4f10"), "store ymm1→[rdi+0x10]")
    require(frag[49:53] == bytes.fromhex("c5fc1107"), "store ymm0→[rdi]")

    # E458 primes rdi=0x160433c; rsi enters as 0x160430c from E458/E459.
    require(lea_target(0x531EF7, img) == DST433C, "E458 lea rdi→0x160433c")
    require(lea_target(0x531EDB, img) == SRC430C, "E458 lea rsi→0x160430c")

    require(img.read(END, 3) == bytes.fromhex("c5f857"), "next vxorps")
    require(call_target(HELPER_26800_SITE, img) == 0x26800, "later 0x26800 (unclaimed)")
    require(img.read(CP10, 4) == b"CP10", "CP10 string (unclaimed)")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_ymm_reload" in nh, "API")
    require("NOT modelled" in nh, "omission")
    require("0x26800" in nh or "CP10" in nh, "stop before helpers")
    st = SLICE.read_text()
    require("post_base_ymm_reload" in st or "PostBaseYmmReload" in st, "wire")

    print(
        f"OK check_f531d60_post_base_ymm_reload: frag={FRAG_SHA256[:12]}…; "
        f"copy 0x30 {SRC430C:#x}->{DST433C:#x}; end={END:#x}; "
        f"CP10_stack/26800/f590/1290_body/5c64c0_body/56dcb0_body/"
        f"499f80_body/140d90_body/14c2f0/6a5370 NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
