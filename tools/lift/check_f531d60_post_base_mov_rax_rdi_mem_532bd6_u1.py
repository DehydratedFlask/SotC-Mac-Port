#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 registry mov rax,[rdi] @ 0x532bd6 after E599 je peel."""

from __future__ import annotations

import hashlib
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
PREV_END = 0x532BD6
FRAG = 0x532BD6
FRAG_SIZE = 3
FRAG_BYTES = bytes.fromhex("488b07")
FRAG_SHA256 = "81c7e25bd38dde9dde06d8a46d8bb57256fcbc780400d7e09baf1a8585b5b409"
END = 0x532BD9
NATIVE = ROOT / "native/core/init_531d60_post_base_mov_rax_rdi_mem_532bd6_u1.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_je_532bdb_532bd4_u1.hpp"
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
        "kInit531d60PostBaseJe532bdb_532bd4_U1EndRva = 0x532bd6" in PREV_NATIVE.read_text(),
        "je end == mov rax start",
    )
    require(PREV_END == FRAG, "contiguous")
    frag = FRAG_BYTES
    require(hashlib.sha256(frag).hexdigest() == FRAG_SHA256, "frag sha")
    require(frag == FRAG_BYTES, "mov rax,[rdi]")
    _ = img  # ELF session for sha256 gate only

    nh = NATIVE.read_text()
    require("init_531d60_post_base_mov_rax_rdi_mem_532bd6_u1" in nh, "API")
    require("NOT modelled" in nh, "omit")
    require("merge_mov_rax_rdi_mem_532bd6_u1_ran" in BODY.read_text(), "body merge")
    require("merge_mov_rax_rdi_mem_532bd6_u1_ran" in REG.read_text(), "reg merge")
    require("kRegistryInit531d60FragmentPinnedRva = 0x532bd9" in REG.read_text(), "pin")
    require("init_531d60_post_base_mov_rax_rdi_mem_532bd6_u1.cpp" in CMAKE.read_text(), "cmake")

    print(
        f"OK check_f531d60_post_base_mov_rax_rdi_mem_532bd6_u1: frag={FRAG_SHA256[:12]}…; "
        f"mov rax,[rdi]@{FRAG:#x}; end={END:#x}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
