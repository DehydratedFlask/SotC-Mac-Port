#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue E third 0xa54920 call-site @ 0x532ae2 after E537."""

from __future__ import annotations

import hashlib
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
FRAG = 0x532AE2
FRAG_SIZE = 5
FRAG_SHA256 = "afb494bc73544c849f9ab6a180568c5f4233d1bb099ccb4fd395f3dedc9abca0"
END = 0x532AE7
TARGET = 0xA54920
NATIVE = ROOT / "native/core/init_531d60_post_base_a54920_532ae2_u1.hpp"
CONT_E = ROOT / "native/core/init_531d60_post_base_continue_e.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_mov_rsp4b8_rcx_532ada_u1.hpp"
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
        "kInit531d60PostBaseMovRsp4b8_Rcx_532ada_U1EndRva = 0x532ae2" in PREV_NATIVE.read_text(),
        "mov4b8 end == call-site start",
    )
    frag = img.read(FRAG, FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest() == FRAG_SHA256, "frag sha")
    require(frag[0] == 0xE8, "call opcode")
    rel = struct.unpack("<i", frag[1:5])[0]
    require(FRAG + 5 + rel == TARGET, "call target")
    require(img.read(END, 4)[0:4] == bytes.fromhex("488b8424"), "post-call mov rax head")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_a54920_532ae2_u1" in nh, "API")
    ce = CONT_E.read_text()
    require("kInit531d60PostBaseContinueERva = 0x532ae7" in ce, "shrunk continue_e start")
    require("merge_a54920_532ae2_u1_ran" in ce, "u1 merge hook")
    require("core/init_531d60_post_base_a54920_532ae2_u1.cpp" in CMAKE.read_text(), "cmake")

    print(
        f"OK check_f531d60_post_base_a54920_532ae2_u1: frag={FRAG_SHA256[:12]}…; "
        f"call 0xa54920@{FRAG:#x}; end={END:#x}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
