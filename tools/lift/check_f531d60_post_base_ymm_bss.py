#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue ymm BSS seed fragment.

Private: needs local/re/eboot.elf. Pins 131 B fragment after 0xf90310 call-site.
Seeds BSS from 0x1014a80 into 0x1604278 / 0x160426c / 0x16042dc / 0x1604268.
Does NOT claim inter-cell ymm @ 0x531ece+, calls 0x26800/0xf590, or helper bodies.
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
PREV = 0x531E46
FRAG = 0x531E4B
FRAG_SIZE = 131
FRAG_SHA256 = "293909e2feb298eaf5fdbfbad88a6c8a84f2f054eac156c2c41b4872a5a7ba56"
END = 0x531ECE
SRC = 0x1014A80
DST4278 = 0x1604278
DST426C = 0x160426C
DST42DC = 0x16042DC
DST4268 = 0x1604268
NEXT_INTERCELL = 0x531ECE  # vmovups ymm2, [rsi+0x10]
HELPER_26800_SITE = 0x531F86
NATIVE = ROOT / "native/core/init_531d60_post_base_ymm_bss.hpp"
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
    require(
        b[0:3] in (bytes.fromhex("488d05"), bytes.fromhex("488d0d"),
                   bytes.fromhex("488d35"), bytes.fromhex("488d3d")),
        f"missing lea at {site:#x}: {b[:3].hex()}",
    )
    disp = struct.unpack("<i", b[3:7])[0]
    return site + 7 + disp


def rip_vmovups_ymm_target(site: int, img: d.Image) -> int:
    b = img.read(site, 8)
    require(b[0:4] == bytes.fromhex("c5fc1005"), f"missing vmovups ymm0,[rip] at {site:#x}")
    disp = struct.unpack("<i", b[4:8])[0]
    return site + 8 + disp


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    require(call_target(PREV, img) == 0xF90310, "prev 0xf90310")
    frag = img.read(FRAG, FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest() == FRAG_SHA256, "sha")
    require(len(frag) == FRAG_SIZE, "size")
    require(rip_vmovups_ymm_target(FRAG, img) == SRC, "src 0x1014a80")
    require(lea_target(FRAG + 8, img) == DST4278, "lea rsi 0x1604278")
    require(lea_target(FRAG + 15, img) == DST426C, "lea rcx 0x160426c")
    require(lea_target(FRAG + 22, img) == DST42DC, "lea rdi 0x16042dc")
    require(lea_target(FRAG + 29, img) == DST4268, "lea rax 0x1604268")
    # After lea rbx / xor edx: vmovups [rsi], ymm0
    require(frag[46:50] == bytes.fromhex("c5fc1106"), "store ymm0→[rsi]")
    require(frag[50:58] == bytes.fromhex("48c746200000803f"), "qword [rsi+0x20]=1.0")
    require(frag[58:66] == bytes.fromhex("48c7462800000000"), "qword [rsi+0x28]=0")
    require(frag[66:72] == bytes.fromhex("c701000080bf"), "dword [rcx]=-1.0")
    # vmovups [rdi], ymm0 after lea rcx→0x16042a8 (next-frag prep; not claimed)
    require(frag[93:97] == bytes.fromhex("c5fc1107"), "store ymm0→[rdi]")
    require(frag[125:131] == bytes.fromhex("c7000000803f"), "dword [rax]=1.0")
    # Next edge is inter-cell ymm (not claimed); helpers later.
    require(img.read(END, 5) == bytes.fromhex("c5fc105610"), "next inter-cell ymm2 load")
    require(END == NEXT_INTERCELL, "end pin")
    require(call_target(HELPER_26800_SITE, img) == 0x26800, "later 0x26800 (unclaimed)")

    src = img.read(SRC, 32)
    require(src == bytes.fromhex("000080bf" + "00000000" * 3 + "0000803f" + "00000000" * 3),
            "src lane floats -1,0,0,0,1,0,0,0")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_ymm_bss" in nh, "API")
    require("NOT modelled" in nh, "omission")
    require("0x26800" in nh or "0xf590" in nh, "stop before helpers")
    st = SLICE.read_text()
    require("post_base_ymm_bss" in st or "PostBaseYmmBss" in st, "wire")

    print(
        f"OK check_f531d60_post_base_ymm_bss: frag={FRAG_SHA256[:12]}…; "
        f"seed 32B {SRC:#x}->{DST4278:#x}/{DST426C:#x}/{DST42DC:#x}/{DST4268:#x}; "
        f"end={END:#x}; intercell/26800/f590/1290_body/5c64c0_body/56dcb0_body/"
        f"499f80_body/140d90_body/14c2f0/6a5370 NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
