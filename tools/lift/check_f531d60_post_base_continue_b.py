#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue B fall-through run 0x53277f..0x5327b1 (50 B / 14 units).

Private: needs local/re/eboot.elf. Pins every one of the 14 consecutive units
after E509 (continue_a je 0x5327bd fall-through), the two direct-call rel32
targets, the je rel32 target, and the byte span sha256 of each unit.
Does NOT claim any callee body, the je outcome, the two successors, or the
incoming rax/r13 provenance.
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
PREV_END = 0x53277f
START = 0x53277f
SIZE = 50
END = 0x5327B1
REGION_SHA256 = "409d74e274de0f6c73c799f325a084a2c6ac742896802657c734b6972c1cabd4"
BRANCH_RVA = 0x5327AB
BRANCH_TARGET = 0x5328E0

CALL_1E4C0 = 0x1E4C0
CALL_4C4950 = 0x4C4950
R13_CELL = 0x32F84F8

# (rva, size, sha256, mnemonic, kind, target)
#   kind: "call" -> rel32 direct call target must equal target
#         "je6"  -> near je (0f 84 <rel32>) target must equal target
#         "pin"  -> opcode/shape only
UNITS = [
    (0x53277F, 2, "2e2f89076640f42236c01bdc8663b50e0001b061170a88fd2ae0a99e0c7b1675",
     "xor esi,esi", "pin", None),
    (0x532781, 2, "7647777c227d53512d0ca616b868156458792b05694ebae60f82d4396cc5e7e7",
     "xor edx,edx", "pin", None),
    (0x532783, 5, "08bc125513338de3457d890ff8a3e73333341ff267b2380a5615ed160af1ca1d",
     "mov ecx,1", "pin", None),
    (0x532788, 3, "970aaedde198dbead329e33a0ccda3704efd78b57b8b6210655ab17dc5ae9d8e",
     "mov rdi,rax", "pin", None),
    (0x53278B, 3, "558681fe421c74c7b3a27974f59cddcc2d03b814d5f57165c1baabd2f884c044",
     "xor r14d,r14d", "pin", None),
    (0x53278E, 5, "1e31031c1fe275a9bcc1afd8cd4a32d35721bdacd1026bd3c3936829d6e4c02c",
     "call 0x1e4c0", "call", CALL_1E4C0),
    (0x532793, 2, "59c3ff3b5508dff4125aa5d1f346b64fba779a271c5d8a02cd964dd9edb49f4b",
     "mov ebx,eax", "pin", None),
    (0x532795, 3, "f51ac5e8eea4a3ee42a5139cfd45d8bf9b207585d7e09f610e6fc500ba6c6474",
     "mov r12b,bl", "pin", None),
    (0x532798, 4, "a65f79ddce849d64d3df1cb19add7b3bfe108b7ecfd51d7bed24fb3f7205ddf5",
     "xor r12b,1", "pin", None),
    (0x53279C, 4, "fd874704edf4d841e2364cd339e3e8baeb9d1f364bcd3be1c29eb6ed23ffda57",
     "movzx edi,r12b", "pin", None),
    (0x5327A0, 5, "20455445a7b8a1a7ea1cc62eaef3025fcdccc415006e0bcd2c699cb69ce10ea3",
     "call 0x4c4950", "call", CALL_4C4950),
    (0x5327A5, 4, "4c1648caac4a2a956dff05c0ed8149ebf8310fb0108beceee7f5b3e6d7f28aaf",
     "mov rdi,[r13+0]", "pin", None),
    (0x5327A9, 2, "7087cf5f9f4f30301fd1b1737cfc01976911f0b4063dca2561bf9152acbfbac4",
     "test bl,bl", "pin", None),
    (0x5327AB, 6, "3f4512e9a101a4842b7b77ed8c4a7ed8382df90c3892e8925f412b032778e8e6",
     "je 0x5328e0", "je6", BRANCH_TARGET),
]

NATIVE = ROOT / "native/core/init_531d60_post_base_continue_b.hpp"
NATIVE_CPP = ROOT / "native/core/init_531d60_post_base_continue_b.cpp"
SLICE = ROOT / "native/core/appmain_init_ctor_98a90_slice.hpp"
SLICE_CPP = ROOT / "native/core/appmain_init_ctor_98a90_slice.cpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_continue_a.hpp"
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

    require(len(UNITS) == 14, "unit count")
    require(PREV_END == START, "prev end == start")
    require(
        "kInit531d60PostBaseContinueAEndRva = 0x53277f" in PREV_NATIVE.read_text(),
        "continue_a end",
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
        elif kind == "je6":
            require(raw[0:2] == bytes([0x0F, 0x84]), f"je6 opcode at 0x{rva:x}")
            rel = struct.unpack("<i", raw[2:6])[0]
            require(rva + 6 + rel == target, f"je6 target at 0x{rva:x}")
        cursor = rva + size
    require(cursor == END, "units cover the claimed span")

    require(hashlib.sha256(img.read(START, SIZE)).hexdigest() == REGION_SHA256,
            "region sha")
    # The unit immediately after the claimed span is the untaken-path entry of
    # the pinned je6; it is NOT modelled.
    require(img.read(END, 5) == bytes.fromhex("e94e010000"), "post-end jmp head")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_continue_b" in nh, "API")
    require("NOT modelled" in nh, "omission")
    require("kInit531d60PostBaseContinueBRva = 0x53277f" in nh, "start rva")
    require("kInit531d60PostBaseContinueBEndRva = 0x5327b1" in nh, "end rva")
    require("kInit531d60PostBaseContinueBBranchTargetRva = 0x5328e0" in nh, "branch target")
    require("kInit531d60PostBaseContinueBUnitCount" in nh, "unit count")
    require("0x1e4c0" in nh, "call 1e4c0 pin")
    require("0x4c4950" in nh, "call 4c4950 pin")
    require("0x32f84f8" in nh, "r13 cell pin")
    for mnemonic in ("xor esi,esi", "je 0x5328e0", "mov rdi,[r13+0]"):
        require(mnemonic in nh, f"unit listing {mnemonic}")
    cpp = NATIVE_CPP.read_text()
    # E524/E528: the je outcome is modelled only when the 0x1e4c0 hook returns a proven
    # boolean; the previous hard-coded `= false` is superseded.
    require("branch_outcome_modelled = bl_is_boolean" in cpp, "branch outcome flag")
    require("branch_taken = bl_is_boolean && (out.bl == 0)" in cpp, "branch taken rule")
    require("bl == 0 => taken" in nh, "boolean contract documented")
    require("init_531d60_post_base_continue_b" in cpp, "cpp definition")
    st = SLICE.read_text()
    require("post_base_continue_b" in st or "PostBaseContinueB" in st, "slice wire")
    require("post_base_continue_b" in SLICE_CPP.read_text(), "slice run")
    require("core/init_531d60_post_base_continue_b.cpp" in CMAKE.read_text(), "cmake")
    tt = TEST.read_text()
    for needle in ("pbcb", "hook_cont_b_1e4c0", "pbcb_units"):
        require(needle in tt, f"test wire {needle}")

    print(
        f"OK check_f531d60_post_base_continue_b: span=0x{START:x}+{SIZE}B "
        f"units={len(UNITS)} region={REGION_SHA256[:12]}…; "
        f"calls=0x1e4c0/0x4c4950 pinned; "
        f"je6@{BRANCH_RVA:#x}->{BRANCH_TARGET:#x} target pinned (outcome NOT modelled); "
        f"end={END:#x}; "
        f"1e4c0_body/4c4950_body/je_outcome/0x5327b1_block/0x5328e0_block/"
        f"incoming_rax/r13_provenance NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
