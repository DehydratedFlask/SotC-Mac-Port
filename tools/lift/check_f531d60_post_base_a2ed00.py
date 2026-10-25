#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue first 0xa2ed00 call-site fragment.

Private: needs local/re/eboot.elf. Pins 5 B after E488 first 0xa2caf0 call-site.
Does NOT claim 0xa2ed00 body or intervening 104 B stack prep.
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
PREV_END = 0x5323F5
PREP = 0x5323F5
PREP_SIZE = 104
PREP_SHA256 = "b242c16d7a4f219f9a27588f31436b7b33ec5de5670a4234f941fd3deb744edb"
FRAG = 0x53245D
FRAG_SIZE = 5
FRAG_SHA256 = "c4a13ed5a578d6fba8a4bde2757ca839837b7d0ceba1ad6d915562f36d4f1edc"
END = 0x532462
TARGET = 0xA2ED00
NATIVE = ROOT / "native/core/init_531d60_post_base_a2ed00.hpp"
SLICE = ROOT / "native/core/appmain_init_ctor_98a90_slice.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_a2caf0.hpp"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    require(PREV_END == PREP, "prev end == prep start")
    require(PREP + PREP_SIZE == FRAG, "prep size")
    require(
        "kInit531d60PostBaseA2caf0EndRva = 0x5323f5" in PREV_NATIVE.read_text(),
        "a2caf0 end",
    )
    prep = img.read(PREP, PREP_SIZE)
    require(hashlib.sha256(prep).hexdigest() == PREP_SHA256, "prep sha")
    require(prep[:4] == bytes.fromhex("498b7d00"), "prep mov rdi,[r13]")
    require(prep[4:7] == bytes.fromhex("4c8dbc"), "prep lea r15")
    frag = img.read(FRAG, FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest() == FRAG_SHA256, "sha")
    require(frag[0] == 0xE8, "call opcode")
    rel = struct.unpack("<i", frag[1:5])[0]
    require(FRAG + 5 + rel == TARGET, "call target")
    require(img.read(END, 3) == bytes.fromhex("4889df"), "next mov rdi,rbx")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_a2ed00" in nh, "API")
    require("NOT modelled" in nh, "omission")
    require("0xa2ed00" in nh, "target")
    st = SLICE.read_text()
    require("post_base_a2ed00" in st or "PostBaseA2ed00" in st, "wire")

    print(
        f"OK check_f531d60_post_base_a2ed00: frag={FRAG_SHA256[:12]}…; "
        f"call 0xa2ed00@{FRAG:#x} (body NOT claimed); "
        f"prep={PREP:#x}+{PREP_SIZE}B stack prep demoted; "
        f"end={END:#x}; "
        f"a2ed00_body/stack_prep/a2caf0_body/f590_body/vcall38_callee/"
        f"53fd10_body/17070_body/544fe0_body/vslot70_callee/532ed0_body/"
        f"vcall18_callee/532ca0_body/52eb50_body/26800_body/"
        f"7f7220_body/1290_body/5c64c0_body/56dcb0_body/499f80_body/"
        f"140d90_body/14c2f0/6a5370 NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
