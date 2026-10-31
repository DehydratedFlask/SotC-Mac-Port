#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue E mov qword [rsp+0x4A0],0 after E527."""

from __future__ import annotations

import hashlib
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
FRAG = 0x532A93
FRAG_SIZE = 12
FRAG_SHA256 = "f8a5ac42b01ee8b7274f6b01819cbb3ca6fae4e97f184f93d8482bfd92235a0f"
END = 0x532A9F
NATIVE = ROOT / "native/core/init_531d60_post_base_movq_rsp4a0_0_532a93_u1.hpp"
CONT_E = ROOT / "native/core/init_531d60_post_base_continue_e.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_lea_rsi_15c8e18_532a8c_u1.hpp"
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
        "kInit531d60PostBaseLeaRsi15c8e18_532a8c_U1EndRva = 0x532a93" in PREV_NATIVE.read_text(),
        "lea_rsi end == movq start",
    )
    frag = img.read(FRAG, FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest() == FRAG_SHA256, "frag sha")
    require(frag[0:3] == bytes.fromhex("48c784"), "mov qword [rsp+disp32] head")
    require(frag[7:12] == bytes(5), "immediate zero qword")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_movq_rsp4a0_0_532a93_u1" in nh, "API")
    ce = CONT_E.read_text()
    require("kInit531d60PostBaseContinueERva = 0x532aaa" in ce, "shrunk continue_e start")
    require("merge_movq_rsp4a0_0_532a93_u1_ran" in ce, "movq merge hook")
    require("core/init_531d60_post_base_movq_rsp4a0_0_532a93_u1.cpp" in CMAKE.read_text(), "cmake")

    post = img.read(END, 4)
    require(post[0:3] == bytes.fromhex("498b7d"), "post-movq mov rdi,[r13] head")

    print(
        f"OK check_f531d60_post_base_movq_rsp4a0_0_532a93_u1: frag={FRAG_SHA256[:12]}…; "
        f"end={END:#x}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
