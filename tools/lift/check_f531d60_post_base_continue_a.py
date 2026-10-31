#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue grouped run 0x532720..0x53277f (95 B / 22 units).

Private: needs local/re/eboot.elf. Pins every one of the 22 consecutive units
after E508 (first post-a2ed00-u7 0xf590 call-site), the five direct-call rel32
targets, the four lea RIP-relative targets, the je rel8 target, and the byte
span sha256 of each unit. Does NOT claim any callee body, the je outcome, the
two successors, or the object/data layouts behind the lea targets.
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
PREV_END = 0x532720
START = 0x532720
SIZE = 95
END = 0x53277F
REGION_SHA256 = "9f4c676b6b368c43629b591db887ef2654bfade085b69fb91e493cb66ebcee40"
BRANCH_RVA = 0x53277D
BRANCH_TARGET = 0x5327BD

CALL_F590 = 0xF590
CALL_A2EA00 = 0xA2EA00
CALL_A2E5D0 = 0xA2E5D0
CALL_A543B0 = 0xA543B0
CALL_1FC80 = 0x1FC80
LEA_32F79A8 = 0x32F79A8
LEA_32F84F8 = 0x32F84F8
LEA_1014D69 = 0x1014D69

# (rva, size, sha256, mnemonic, kind, target)
#   kind: "call" -> rel32 direct call target must equal target
#         "lea"  -> RIP-relative displacement must land on target
#         "je"   -> rel8 branch target must equal target
#         "pin"  -> opcode/shape only
UNITS = [
    (0x532720, 3, "4eef54c393ee98ea9816e4ce4154f9264971ddf328e3ce421b06e8841edef72f",
     "mov rdi,r15", "pin", None),
    (0x532723, 5, "99e1c7bf68018ac5cc75142af7d243c4dca81ce54eae209bd8d8cd573ac6e143",
     "call 0xf590", "call", CALL_F590),
    (0x532728, 4, "657bd26319a05c1bdb127a1c2035563613bad51b5ac54606db6aa18c5cb1c98a",
     "mov rbx,[r13]", "pin", None),
    (0x53272C, 2, "2e2f89076640f42236c01bdc8663b50e0001b061170a88fd2ae0a99e0c7b1675",
     "xor esi,esi", "pin", None),
    (0x53272E, 3, "0bc5a45e8b7b5bfc6d88912c19ddf334fbbdb17c2a1df7b79adf0dda065730ab",
     "mov rdi,rbx", "pin", None),
    (0x532731, 5, "a6fd5d490e9c60bc5b39745543428bc4697cababb739e9d96c956d1d841055e6",
     "call 0xa2ea00", "call", CALL_A2EA00),
    (0x532736, 3, "0bc5a45e8b7b5bfc6d88912c19ddf334fbbdb17c2a1df7b79adf0dda065730ab",
     "mov rdi,rbx", "pin", None),
    (0x532739, 2, "750468e50b8500dfb8f128039880db2804b753a3ffcba500cbba0621b06a948c",
     "mov esi,eax", "pin", None),
    (0x53273B, 5, "156379f8d32eaa5ae11112c89e04710ce4e4b979a7ec9ff21b2c4290cf96f375",
     "call 0xa2e5d0", "call", CALL_A2E5D0),
    (0x532740, 7, "174a36616c9054ebd8f6c7b29356a44f8611c24ad265f9797603d98a3e5d55b1",
     "lea rax,[0x32f79a8]", "lea", LEA_32F79A8),
    (0x532747, 7, "f3df838c0139885befe0b7b6e3092862097292f752821e76c455349cf708ad5c",
     "lea r13,[0x32f84f8]", "lea", LEA_32F84F8),
    (0x53274E, 3, "d2dd46ebb1555ee3ece77ec5799d371e6449a036344614c1e33c310697c2180c",
     "mov rax,[rax]", "pin", None),
    (0x532751, 4, "5cbe8a11db9e94898aff3c8ff441d398f6eb9db110410761e1561b388d73a64b",
     "mov rdi,[r13]", "pin", None),
    (0x532755, 7, "703bb189fb59e65a5e36d01df6bf9fe24c53fb3bbf4b2bc469992e20a33d52e4",
     "mov rsi,[rax+0xf8]", "pin", None),
    (0x53275C, 4, "0aaf7c8dae8cac3c5920008deba1aee859618144bbe0a4e4fa6639b73eaf50ab",
     "add rsi,0x40", "pin", None),
    (0x532760, 5, "c832b5949ba0d918a3c0b5bf633f4090be81bd841a11ca207574abf45e6b7b49",
     "call 0xa543b0", "call", CALL_A543B0),
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
SLICE_CPP = ROOT / "native/core/appmain_init_ctor_98a90_slice.cpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_f590_a2ed00_u13.hpp"
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

    require(len(UNITS) == 22, "unit count")
    require(PREV_END == START, "prev end == start")
    require(
        "kInit531d60PostBaseF590A2ed00U13EndRva = 0x532720" in PREV_NATIVE.read_text(),
        "u13 end",
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
    # The unit immediately after the claimed span is the untaken-path entry of
    # the pinned branch target; it is NOT modelled.
    require(img.read(BRANCH_TARGET, 4) == bytes.fromhex("488d05ac"), "branch target head")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_continue_a" in nh, "API")
    require("NOT modelled" in nh, "omission")
    require("kInit531d60PostBaseContinueARva = 0x532720" in nh, "start rva")
    require("kInit531d60PostBaseContinueAEndRva = 0x53277f" in nh, "end rva")
    require("kInit531d60PostBaseContinueABranchTargetRva = 0x5327bd" in nh, "branch target")
    require("kInit531d60PostBaseContinueAUnitCount" in nh, "unit count")
    require("0x1014d69" in nh, "lea rva pin")
    require("0x32f84f8" in nh, "lea rva pin 2")
    require("0xa543b0" in nh, "call pin")
    for mnemonic in ("mov rdi,r15", "je 0x5327bd", "mov rsi,[rax+0xf8]"):
        require(mnemonic in nh, f"unit listing {mnemonic}")
    cpp = NATIVE_CPP.read_text()
    require("branch_outcome_modelled = false" in cpp, "branch outcome flag")
    require("init_531d60_post_base_continue_a" in cpp, "cpp definition")
    st = SLICE.read_text()
    require("post_base_continue_a" in st or "PostBaseContinueA" in st, "slice wire")
    require("post_base_continue_a" in SLICE_CPP.read_text(), "slice run")
    require("core/init_531d60_post_base_continue_a.cpp" in CMAKE.read_text(), "cmake")
    tt = TEST.read_text()
    for needle in ("pbca", "hook_cont_a_a543b0", "0x1274", "pbca_units"):
        require(needle in tt, f"test wire {needle}")

    print(
        f"OK check_f531d60_post_base_continue_a: span=0x{START:x}+{SIZE}B "
        f"units={len(UNITS)} region={REGION_SHA256[:12]}…; "
        f"calls=0xf590/0xa2ea00/0xa2e5d0/0xa543b0/0x1fc80 pinned; "
        f"lea=0x32f79a8/0x32f84f8/0x1014d69 pinned; "
        f"je@{BRANCH_RVA:#x}->{BRANCH_TARGET:#x} target pinned "
        f"(default outcome unmodelled; known 0x1fc80 rax models taken iff null); "
        f"end={END:#x}; "
        f"f590_body/a2ea00_body/a2e5d0_body/a543b0_body/"
        f"0x53277f_block/0x5327bd_block/incoming_r13/r15/"
        f"32f79a8_layout/32f84f8_layout/1014d69_data/stack_frame NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
