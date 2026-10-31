#!/usr/bin/env python3
"""ELF-tie Init merge fall-through after continue C (0x53299d..0x5329ae, 17 B / 4 units).

Private: needs local/re/eboot.elf. Same OBS span as batch D (E535); this checker ties the
entry to continue C's END and the post-end lea head from check_f531d60_post_base_continue_c.py.
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
CONTINUE_C_END = 0x53299D
RVA = 0x53299D
SIZE = 17
END = 0x5329AE
REGION_SHA256 = "6f4e5fa8921e53532c8ad4319ba268bd181d3874e83e4f140a4873bfccf3cb2d"
CELL = 0x32F79A8
CALL = 0xA473B0
POST_END_LEA = bytes.fromhex("488d050450dc02")

NATIVE = ROOT / "native/core/init_531d60_post_base_continue_c_after_merge.hpp"
NATIVE_CPP = ROOT / "native/core/init_531d60_post_base_continue_c_after_merge.cpp"
CONTINUE_C = ROOT / "native/core/init_531d60_post_base_continue_c.hpp"
TEST = ROOT / "native/tests/test_init_continue_c_after_merge.cpp"
CMAKE = ROOT / "native/CMakeLists.txt"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    require(RVA == CONTINUE_C_END, "fall-through entry equals continue C end")
    body = img.read(RVA, SIZE)
    require(len(body) == SIZE and RVA + SIZE == END, "span arithmetic")
    require(hashlib.sha256(body).hexdigest() == REGION_SHA256, "region sha256")
    require(img.read(CONTINUE_C_END, 7) == POST_END_LEA, "post-end lea head")

    require(body[0:3] == bytes.fromhex("488d05"), "lea rax,[rip+..]")
    require(RVA + 7 + struct.unpack("<i", body[3:7])[0] == CELL, "lea target 0x32f79a8")
    require(body[7:9] == bytes.fromhex("31f6"), "xor esi,esi")
    require(body[9:12] == bytes.fromhex("488b38"), "mov rdi,[rax]")
    require(body[12] == 0xE8, "call opcode")
    require(RVA + 12 + 5 + struct.unpack("<i", body[13:17])[0] == CALL, "call target 0xa473b0")

    nh = NATIVE.read_text()
    for token in (
        "init_531d60_post_base_continue_c_after_merge",
        "kInit531d60PostBaseContinueCAfterMergeRva = 0x53299d",
        "kInit531d60PostBaseContinueCAfterMergeSize = 17",
        "kInit531d60PostBaseContinueCAfterMergeEndRva = 0x5329ae",
        "kInit531d60PostBaseContinueCAfterMergeCellRva = 0x32f79a8",
        "kInit531d60PostBaseContinueCAfterMergeCallRva = 0xa473b0",
        "run_init_continue_c_after_merge",
        "NOT modelled",
    ):
        require(token in nh, f"header token {token!r}")
    require("kInit531d60PostBaseContinueCEndRva = 0x53299d" in CONTINUE_C.read_text(),
            "continue C end rva")

    cpp = NATIVE_CPP.read_text()
    require("run_init_continue_c_after_merge" in cpp, "cpp runner")
    require("init_531d60_post_base_continue_d" in cpp, "delegates to batch D")

    ct = CMAKE.read_text()
    require("core/init_531d60_post_base_continue_c_after_merge.cpp" in ct, "cmake core")
    require("test_init_continue_c_after_merge" in ct, "cmake test")

    tt = TEST.read_text()
    require("run_init_continue_c_after_merge" in tt, "test runner")
    require("0x53299d" in tt, "test entry pin")

    print(
        f"OK check_f531d60_post_base_continue_c_after_merge: span=0x{RVA:x}+{SIZE}B "
        f"region={REGION_SHA256[:12]}…; lea->0x{CELL:x}; call->0x{CALL:x}; "
        f"continue_c_end=0x{CONTINUE_C_END:x}; post_end_lea pinned; "
        f"continue_e/0xa473b0_body/cell_value NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
