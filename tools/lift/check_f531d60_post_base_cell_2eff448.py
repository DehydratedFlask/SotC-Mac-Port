#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue cell-install at 0x2eff448.

Private: needs local/re/eboot.elf. Pins 29 B after E484 first 0x53fd10 call-site.
Does NOT claim vcall+0x70 callee body or following vcall [rax+0x38].
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
PREV_END = 0x5323BE
FRAG = 0x5323BE
FRAG_SIZE = 29
FRAG_SHA256 = "cd7c96f399e2add49a4f464e3529df616aa9cfdcd616c55f9d6fd9118b2bbdd7"
END = 0x5323DB
CELL = 0x2EFF448
NATIVE = ROOT / "native/core/init_531d60_post_base_cell_2eff448.hpp"
SLICE = ROOT / "native/core/appmain_init_ctor_98a90_slice.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_53fd10.hpp"


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
        "kInit531d60PostBase53fd10EndRva = 0x5323be" in PREV_NATIVE.read_text(),
        "53fd10 end",
    )
    frag = img.read(FRAG, FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest() == FRAG_SHA256, "sha")
    require(frag[:3] == bytes.fromhex("4c8d35"), "lea r14")
    disp = struct.unpack("<i", frag[3:7])[0]
    require(FRAG + 7 + disp == CELL, "lea cell 0x2eff448")
    require(frag[7:10] == bytes.fromhex("498b3e"), "mov rdi,[r14]")
    require(frag[10:13] == bytes.fromhex("4839df"), "cmp rdi,rbx")
    require(frag[13] == 0x74 and frag[14] == 0x0E, "jz skip")
    require(frag[15:18] == bytes.fromhex("4885ff"), "test rdi,rdi")
    require(frag[18] == 0x74 and frag[19] == 0x06, "jz store")
    require(frag[20:26] == bytes.fromhex("488b07ff5070"), "mov rax,[rdi]; call [rax+0x70]")
    require(frag[26:29] == bytes.fromhex("49891e"), "mov [r14],rbx")
    require(img.read(END, 4) == bytes.fromhex("498b0424"), "next mov rax,[r12]")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_cell_2eff448" in nh, "API")
    require("NOT modelled" in nh, "omission")
    require("0x2eff448" in nh, "cell")
    st = SLICE.read_text()
    require("post_base_cell_2eff448" in st or "PostBaseCell2eff448" in st, "wire")

    print(
        f"OK check_f531d60_post_base_cell_2eff448: frag={FRAG_SHA256[:12]}…; "
        f"cell=0x{CELL:x}@{FRAG:#x}+{FRAG_SIZE}B; "
        f"end={END:#x}; "
        f"vslot70_callee/53fd10_body/17070_body/544fe0_body/532ed0_body/"
        f"vcall18_callee/532ca0_body/52eb50_body/26800_body/f590_body/"
        f"7f7220_body/1290_body/5c64c0_body/56dcb0_body/499f80_body/"
        f"140d90_body/14c2f0/6a5370 NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
