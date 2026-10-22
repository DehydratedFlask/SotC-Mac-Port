#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue call-site for 0x532ca0.

Private: needs local/re/eboot.elf. Pins 5 B call-site after E465 lea/vmovq.
Does NOT claim intervening stack prep (103 B) or 0x532ca0 helper body.
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
PREV_END = 0x532045  # lea/vmovq end; stack prep starts here (unclaimed)
FRAG = 0x5320AC
FRAG_SIZE = 5
FRAG_SHA256 = "48b9e51229a8c744aefa3163d0bbda6cc39f8e286155edb6279e72bd83afe248"
END = 0x5320B1
TARGET = 0x532CA0
STACK_PREP = 0x532045
STACK_PREP_SIZE = 103
NATIVE = ROOT / "native/core/init_531d60_post_base_532ca0.hpp"
SLICE = ROOT / "native/core/appmain_init_ctor_98a90_slice.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_lea_vmovq.hpp"


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

    require(
        "kInit531d60PostBaseLeaVmovqEndRva = 0x532045" in PREV_NATIVE.read_text(),
        "lea_vmovq end",
    )
    # Stack prep sits between prev end and this call-site — pin size, do not claim.
    require(STACK_PREP + STACK_PREP_SIZE == FRAG, "stack prep abuts call")
    require(img.read(STACK_PREP, 4) == bytes.fromhex("488db424"), "stack prep lea rsi")
    require(hashlib.sha256(img.read(FRAG, FRAG_SIZE)).hexdigest() == FRAG_SHA256, "sha")
    require(call_target(FRAG, img) == TARGET, "call 0x532ca0")
    require(img.read(TARGET, 1) == b"\x55", "0x532ca0 push rbp (body unclaimed)")
    # Next edge starts with movq from stack — not claimed.
    require(img.read(END, 4) == bytes.fromhex("488b8424"), "next movq")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_532ca0" in nh, "API")
    require("NOT modelled" in nh, "omission")
    require("stack prep" in nh.lower() or "0x532045" in nh, "stack prep omit")
    st = SLICE.read_text()
    require("post_base_532ca0" in st or "PostBase532ca0" in st, "wire")

    print(
        f"OK check_f531d60_post_base_532ca0: frag={FRAG_SHA256[:12]}…; "
        f"call {TARGET:#x} (body NOT claimed); "
        f"stack_prep={STACK_PREP:#x}+{STACK_PREP_SIZE} NOT claimed; "
        f"end={END:#x}; "
        f"532ca0_body/52eb50_body/26800_body/f590_body/7f7220_body/"
        f"1290_body/5c64c0_body/56dcb0_body/499f80_body/140d90_body/"
        f"14c2f0/6a5370 NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
