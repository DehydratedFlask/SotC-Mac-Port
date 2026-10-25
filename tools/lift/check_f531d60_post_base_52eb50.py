#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue call-site for 0x52eb50.

Private: needs local/re/eboot.elf. Pins 5 B fragment after E463 Outro Jump.
Does NOT claim trampoline body 0x52eb50 (jmp → 0xa44da0) or later Init continues.
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
PREV_END = 0x532031  # SBOJ end / this call-site
FRAG = 0x532031
FRAG_SIZE = 5
FRAG_SHA256 = "7de9082ade080a6443530c9e20eb0124258e43352f6eb61765ef9cd703ebb063"
END = 0x532036
TARGET = 0x52EB50
NATIVE = ROOT / "native/core/init_531d60_post_base_52eb50.hpp"
SLICE = ROOT / "native/core/appmain_init_ctor_98a90_slice.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_sboj.hpp"


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

    require(PREV_END == FRAG, "prev end == frag start")
    require("kInit531d60PostBaseSbojEndRva = 0x532031" in PREV_NATIVE.read_text(), "sboj end")
    require(hashlib.sha256(img.read(FRAG, FRAG_SIZE)).hexdigest() == FRAG_SHA256, "sha")
    require(call_target(FRAG, img) == TARGET, "call 0x52eb50")
    # Body tourism pin: trampoline starts with lea rax (not push rbp); first jmp
    # at +0x16 → 0xa44da0. Do NOT claim body.
    body = img.read(TARGET, 32)
    require(body[0:3] == bytes.fromhex("488d05"), "0x52eb50 lea rax head (unclaimed)")
    require(body[0x16] == 0xE9, "0x52eb50 jmp trampoline (unclaimed)")
    jmp_rel = struct.unpack("<i", body[0x17:0x1B])[0]
    require(TARGET + 0x1B + jmp_rel == 0xA44DA0, "jmp target 0xa44da0 (unclaimed)")
    # Next edge starts with lea rbx (not claimed this turn).
    require(img.read(END, 3) == bytes.fromhex("488d1d"), "next lea rbx")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_52eb50" in nh, "API")
    require("NOT modelled" in nh, "omission")
    require("trampoline" in nh.lower() or "0x52eb50 body" in nh or "body NOT" in nh, "body omit")
    st = SLICE.read_text()
    require("post_base_52eb50" in st or "PostBase52eb50" in st, "wire")

    print(
        f"OK check_f531d60_post_base_52eb50: frag={FRAG_SHA256[:12]}…; "
        f"call {TARGET:#x} (trampoline body NOT claimed); "
        f"end={END:#x}; "
        f"52eb50_body/26800_body/f590_body/7f7220_body/"
        f"1290_body/5c64c0_body/56dcb0_body/499f80_body/140d90_body/"
        f"14c2f0/6a5370 NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
