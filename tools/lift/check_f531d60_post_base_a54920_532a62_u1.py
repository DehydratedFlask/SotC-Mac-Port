#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue E second 0xa54920 call-site @ 0x532a62 after E521."""

from __future__ import annotations

import hashlib
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
PREP = 0x532A1A
PREP_SIZE = 72
PREP_SHA256 = "48f12468339429132b04aa7d077d788b73738ccde4c3653c03870f2895e43168"
FRAG = 0x532A62
FRAG_SIZE = 5
FRAG_SHA256 = "05ace5dabb92d36d05fbf80283594b9c2ec091148c8fdf72cee43cec24a84a8d"
END = 0x532A67
TARGET = 0xA54920
NATIVE = ROOT / "native/core/init_531d60_post_base_a54920_532a62_u1.hpp"
CONT_E = ROOT / "native/core/init_531d60_post_base_continue_e.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_vcall18_532a17.hpp"
CMAKE = ROOT / "native/CMakeLists.txt"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    require(
        "kInit531d60PostBaseVcall18532a17EndRva = 0x532a1a" in PREV_NATIVE.read_text(),
        "vcall18 end == prep start",
    )
    require(PREP + PREP_SIZE == FRAG, "prep size")
    prep = img.read(PREP, PREP_SIZE)
    require(hashlib.sha256(prep).hexdigest() == PREP_SHA256, "prep sha")
    require(prep[0:4] == bytes.fromhex("498b7d00"), "prep mov rdi,[r13] head")
    frag = img.read(FRAG, FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest() == FRAG_SHA256, "frag sha")
    require(frag[0] == 0xE8, "call opcode")
    rel = struct.unpack("<i", frag[1:5])[0]
    require(FRAG + 5 + rel == TARGET, "call target")
    require(img.read(END, 4)[0:4] == bytes.fromhex("488b8424"), "post-call mov rax head")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_a54920_532a62_u1" in nh, "API")
    ce = CONT_E.read_text()
    u72 = ROOT / "native/core/init_531d60_post_base_vcall18_532a72_u1.hpp"
    require("kInit531d60PostBaseVcall18_532a72_U1PrepRva = 0x532a67" in u72.read_text(),
            "vcall72 prep after u2")
    require("merge_a54920_532a62_u1_ran" in ce, "u1 merge hook")
    require("core/init_531d60_post_base_a54920_532a62_u1.cpp" in CMAKE.read_text(), "cmake")

    print(
        f"OK check_f531d60_post_base_a54920_532a62_u1: frag={FRAG_SHA256[:12]}…; "
        f"call 0xa54920@{FRAG:#x}; prep={PREP:#x}+{PREP_SIZE}B demoted; end={END:#x}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
