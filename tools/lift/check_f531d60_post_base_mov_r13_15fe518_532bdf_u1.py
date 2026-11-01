#!/usr/bin/env python3
"""mov r13,[0x15fe518] @ 0x532bdf after E613 mov rdi peel."""

from __future__ import annotations

import hashlib
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
PREV_END = 0X532BDF
FRAG = 0X532BDF
FRAG_SIZE = 7
FRAG_BYTES = bytes.fromhex("4c8b2d32b90c01")
FRAG_SHA256 = "aae6f2e795dc0d63c26e2be36a21be2f6fedfe2e5bd401c6e0595a7803ce4ef5"
END = 0X532BE6
NEXT_HEAD = bytes.fromhex("4885ff7405e8")
NATIVE = ROOT / "native/core/init_531d60_post_base_mov_r13_15fe518_532bdf_u1.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_mov_rdi_mem_r12_532bdb_u1.hpp"
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
        "kInit531d60PostBaseMovRdiMemR12_532bdb_U1EndRva = 0x532bdf" in PREV_NATIVE.read_text(),
        "previous unit ends at frag start",
    )
    require(PREV_END == FRAG, "contiguous")
    frag = img.read(FRAG, FRAG_SIZE)
    require(hashlib.sha256(frag).hexdigest() == FRAG_SHA256, "frag sha")
    require(frag == FRAG_BYTES, "mov r13,[0x15fe518] stack-chk guard cell")
    disp = struct.unpack("<i", frag[3:7])[0]
    require(FRAG + 7 + disp == 0x15fe518, "rip target 0x15fe518")
    require(NEXT_HEAD[0:3] == bytes.fromhex("4885ff"), "next instr head @ 0x532be6")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_mov_r13_15fe518_532bdf_u1" in nh, "API")
    require("G1 PARTIAL" in nh, "omit marker")
    require("merge_mov_r13_15fe518_532bdf_u1_ran" in BODY.read_text(), "body merge")
    require("merge_mov_r13_15fe518_532bdf_u1_ran" in REG.read_text(), "reg merge")
    require("kRegistryInit531d60FragmentPinnedRva = 0x" in REG.read_text(), "pin")
    require("init_531d60_post_base_mov_r13_15fe518_532bdf_u1.cpp" in CMAKE.read_text(), "cmake")

    print(
        f"OK check_f531d60_post_base_mov_r13_15fe518_532bdf_u1: frag={FRAG_SHA256[:12]}…; end={END:#x}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
