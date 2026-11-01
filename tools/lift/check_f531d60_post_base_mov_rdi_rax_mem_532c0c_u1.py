#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 registry mov rdi,[rax] @ 0x532c0c after E611 mov edx peel."""

from __future__ import annotations

import hashlib
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
PREV_END = 0x532C0C
FRAG = 0x532C0C
FRAG_SIZE = 3
FRAG_BYTES = bytes.fromhex("488b38")
FRAG_SHA256 = "6355e6dc47fc8485f47f6455189f2dd9151220eae0f98847b8b09b55df9c3751"
END = 0x532C0F
NATIVE = ROOT / "native/core/init_531d60_post_base_mov_rdi_rax_mem_532c0c_u1.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_mov_edx_63647372_532c07_u1.hpp"
BODY = ROOT / "native/core/init_531d60_post_base_registry_body.hpp"
REG = ROOT / "native/core/registry_init_531d60.hpp"
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
        "kInit531d60PostBaseMovEdx63647372_532c07_U1EndRva = 0x532c0c" in PREV_NATIVE.read_text(),
        "mov edx end == mov rdi,[rax] start",
    )
    require(PREV_END == FRAG, "contiguous")
    frag = img.read(FRAG, FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest() == FRAG_SHA256, "frag sha")
    require(frag == FRAG_BYTES, "mov rdi,[rax]")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_mov_rdi_rax_mem_532c0c_u1" in nh, "API")
    require("NOT modelled" in nh, "omit tail")
    require("merge_mov_rdi_rax_mem_532c0c_u1_ran" in BODY.read_text(), "body merge")
    require("merge_mov_rdi_rax_mem_532c0c_u1_ran" in REG.read_text(), "reg merge")
    require("kRegistryInit531d60FragmentPinnedRva = 0x532c0f" in REG.read_text(), "pin")
    require("init_531d60_post_base_mov_rdi_rax_mem_532c0c_u1.cpp" in CMAKE.read_text(), "cmake")

    print(
        f"OK check_f531d60_post_base_mov_rdi_rax_mem_532c0c_u1: frag={FRAG_SHA256[:12]}…; "
        f"end={END:#x}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
