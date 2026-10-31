#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue grouped run 0x532765..0x53277f (26 B / 6 units).

Private: needs local/re/eboot.elf. Pins every one of the 6 consecutive units
after E512 (first 0xa543b0 call-site), the direct-call rel32 target 0x1fc80, the
lea RIP-relative target, the je rel8 target, and the byte span sha256 of each unit.
Does NOT claim any callee body, the je outcome, the two successors, or data layouts.
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
PREV_END = 0x532765
START = 0x532765
SIZE = 26
END = 0x53277F
REGION_SHA256 = "5c1c9d10c17102c25cd69ab0b42240aa8a04b1e6908ab04e44a41b5e0e1d9f46"
BRANCH_RVA = 0x53277D
BRANCH_TARGET = 0x5327BD

CALL_1FC80 = 0x1FC80
LEA_1014D69 = 0x1014D69

UNITS = [
    (0x532765, 5, "b09f6d31bd33b5ef67f21764c53d5626a54d828ed322ebf9db54c570da85d692",
     "mov rax,[rsp+0x20]", "pin", None),
    (0x53276A, 7, "9b67b616fba345bf41279bd35aa958cdd48905797de7b509bcc4db32491f515d",
     "lea rsi,[0x1014d69]", "lea", LEA_1014D69),
    (0x532771, 4, "1179442bc59381ce1b74c69d79731e8f6f95f04f842d5c03173a706f8e96a592",
     "lea rdi,[rax+8]", "pin", None),
    (0x532775, 5, "7f083533b1025791bcb29da0d369eed42d2b6f8321dfe83a2a165467e54528c3",
     "call 0x1fc80", "call", CALL_1FC80),
    (0x53277A, 3, "6f4286cdf1bc8e242708e3bcf98649612b6c89649997a1fa0a5af953103bcea1",
     "test rax,rax", "pin", None),
    (0x53277D, 2, "17dc64d49bbde7c8a2294fe27e6b08751cd4171d0d72b282e2698ad53402dc46",
     "je 0x5327bd", "je", BRANCH_TARGET),
]

NATIVE = ROOT / "native/core/init_531d60_post_base_continue_a.hpp"
NATIVE_CPP = ROOT / "native/core/init_531d60_post_base_continue_a.cpp"
SLICE = ROOT / "native/core/appmain_init_ctor_98a90_slice.hpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_a543b0_u1.hpp"
TEST = ROOT / "native/tests/test_appmain_init_ctor_98a90_slice.cpp"
CMAKE = ROOT / "native/CMakeLists.txt"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    require(len(UNITS) == 6, "unit count")
    require(PREV_END == START, "prev end == start")
    require(
        "kInit531d60PostBaseA543b0U1EndRva = 0x532765" in PREV_NATIVE.read_text(),
        "a543b0_u1 end",
    )
    require(START + SIZE == END, "size arithmetic")

    cursor = START
    for rva, size, sha, mnemonic, kind, target in UNITS:
        require(rva == cursor, f"contiguity at 0x{rva:x} (expected 0x{cursor:x})")
        raw = img.read(rva, size)
        require(len(raw) == size, f"short read at 0x{rva:x}")
        got = hashlib.sha256(raw).hexdigest()
        require(got == sha, f"sha mismatch at 0x{rva:x}: {got}")
        if kind == "call":
            require(raw[0] == 0xE8, f"call opcode at 0x{rva:x}")
            rel = struct.unpack("<i", raw[1:5])[0]
            require(rva + 5 + rel == target, f"call target at 0x{rva:x}")
        elif kind == "lea":
            require(raw[0] in (0x48, 0x4C) and raw[1] == 0x8D, f"lea opcode at 0x{rva:x}")
            require(raw[2] & 0xC7 == 0x05, f"lea rip-relative modrm at 0x{rva:x}")
            rel = struct.unpack("<i", raw[3:7])[0]
            require(rva + 7 + rel == target, f"lea target at 0x{rva:x}")
        elif kind == "je":
            require(raw[0] == 0x74, f"je opcode at 0x{rva:x}")
            require(rva + 2 + struct.unpack("<b", raw[1:2])[0] == target,
                    f"je target at 0x{rva:x}")
        cursor = rva + size
    require(cursor == END, "units cover the claimed span")

    require(hashlib.sha256(img.read(START, SIZE)).hexdigest() == REGION_SHA256,
            "region sha")
    require(img.read(BRANCH_TARGET, 4) == bytes.fromhex("488d05ac"), "branch target head")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_continue_a" in nh, "API")
    require("NOT modelled" in nh, "omission")
    require("kInit531d60PostBaseContinueARva = 0x532765" in nh, "start rva")
    require("kInit531d60PostBaseContinueAEndRva = 0x53277f" in nh, "end rva")
    require("kInit531d60PostBaseContinueABranchTargetRva = 0x5327bd" in nh, "branch target")
    st = SLICE.read_text()
    require("post_base_continue_a" in st or "PostBaseContinueA" in st, "slice wire")
    require("post_base_a543b0_u1" in st or "PostBaseA543b0U1" in st, "u1 wire")
    require("core/init_531d60_post_base_continue_a.cpp" in CMAKE.read_text(), "cmake")
    tt = TEST.read_text()
    for needle in ("pbca", "hook_cont_a_1fc80", "pbca_units"):
        require(needle in tt, f"test wire {needle}")

    print(
        f"OK check_f531d60_post_base_continue_a: span=0x{START:x}+{SIZE}B "
        f"units={len(UNITS)} region={REGION_SHA256[:12]}…; "
        f"call=0x1fc80 pinned; je@{BRANCH_RVA:#x}->{BRANCH_TARGET:#x}; end={END:#x}; "
        f"a543b0_body/0x53277f_block/0x5327bd_block NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
