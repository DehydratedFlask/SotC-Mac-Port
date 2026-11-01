#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue E mov edx,2 @ 0x532afe after E543."""

from __future__ import annotations

import hashlib
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
FRAG = 0x532AFE
FRAG_SIZE = 5
FRAG_SHA256 = "252ddaa7ae0a8d071529fab21284ab34cb7bda4eb60c58c59dcfec5f79338f15"
END = 0x532B03
NATIVE = ROOT / "native/core/init_531d60_post_base_mov_edx_2_532afe_u1.hpp"
CONT_E = ROOT / "native/core/init_531d60_post_base_continue_e.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_xor_esi_532afc_u1.hpp"
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
        "kInit531d60PostBaseXorEsi_532afc_U1EndRva = 0x532afe" in PREV_NATIVE.read_text(),
        "xor_esi u1 end == mov edx start",
    )
    frag = img.read(FRAG, FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest() == FRAG_SHA256, "frag sha")
    require(frag == bytes.fromhex("ba02000000"), "mov edx,2")
    require(img.read(END, 3)[0:2] == bytes([0x48, 0x8B]), "post-mov mov rdi,[rax] head")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_mov_edx_2_532afe_u1" in nh, "API")
    ce = CONT_E.read_text()
    require("kInit531d60PostBaseContinueERva = 0x532b0c" in ce, "shrunk continue_e start")
    require("merge_mov_edx_2_532afe_u1_ran" in ce, "mov edx merge hook")
    require("core/init_531d60_post_base_mov_edx_2_532afe_u1.cpp" in CMAKE.read_text(), "cmake")

    print(
        f"OK check_f531d60_post_base_mov_edx_2_532afe_u1: frag={FRAG_SHA256[:12]}…; "
        f"mov edx,2@{FRAG:#x}; end={END:#x}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
