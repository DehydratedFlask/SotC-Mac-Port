#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue C merge run 0x53294b..0x53299d (82 B / 21 units).

Private: needs local/re/eboot.elf. Pins every one of the 21 consecutive units
after E517 first merge `0xa599e0` call-site (0x532946..0x53294b), the one
remaining direct-call rel32 target, the two indirect `call [rax]` vtable-slot-0 sites,
the one RIP-relative lea target, the four RIP-relative global loads, and the
je rel8 target. Does NOT claim any callee body, the je outcome, the two
successors, or the incoming r14b/r15 provenance.
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
START = 0x53294B
SIZE = 82
END = 0x53299D
REGION_SHA256 = "d8e2dd85a5edebf5af32861d46a3035f5adde2f5d4e9cb344804883052636645"
BRANCH_RVA = 0x53299B
BRANCH_TARGET = 0x5329AE

CALL_A431E0 = 0xA431E0
LEA_32F6D18 = 0x32F6D18
GLOBAL_2EFE318 = 0x2EFE318
GLOBAL_2EFE340 = 0x2EFE340

UNITS = [
    (0x53294B, 7, "72118d3b2feeb3b1f808b88e89fbce5f09184398e5441090d7c00a5f901dd7cd",
     "lea rax,[0x32f6d18]", "lea", LEA_32F6D18),
    (0x532952, 2, "2e2f89076640f42236c01bdc8663b50e0001b061170a88fd2ae0a99e0c7b1675",
     "xor esi,esi", "pin", None),
    (0x532954, 3, "4eef54c393ee98ea9816e4ce4154f9264971ddf328e3ce421b06e8841edef72f",
     "mov rdi,r15", "pin", None),
    (0x532957, 3, "65b0c3cc3d7863e2c087793da24eee81909ef4fb5f1a1080e7766dbce504e0eb",
     "mov rbx,[rax]", "pin", None),
    (0x53295A, 7, "7ce91e10406eaceab3f7679ede1c833630caf1138f84aedf9d0f4010afe4d9ad",
     "mov rax,[0x2efe318]", "mov_rip", GLOBAL_2EFE318),
    (0x532961, 2, "6e7c72ff1c0aef12b52b1cab58166f527b87f2b37842ad6e82a9c81524c33bff",
     "call [rax]", "vcall0", None),
    (0x532963, 7, "826fbada8674eb21855d9e9e0c8a7bdfe9f5709b6dc931da1c5a5628222dba0e",
     "mov rax,[0x2efe340]", "mov_rip", GLOBAL_2EFE340),
    (0x53296A, 2, "2e2f89076640f42236c01bdc8663b50e0001b061170a88fd2ae0a99e0c7b1675",
     "xor esi,esi", "pin", None),
    (0x53296C, 3, "0bc5a45e8b7b5bfc6d88912c19ddf334fbbdb17c2a1df7b79adf0dda065730ab",
     "mov rdi,rbx", "pin", None),
    (0x53296F, 4, "2e886acf7022f26436056b8080c5ce424d76ae1beaefbe576186b5a6c776e253",
     "mov rax,[rax+0x60]", "pin", None),
    (0x532973, 3, "a47fe7f6ec040a966cb24a4799930b575501c9fc0592cc5f2bb577ee74f7cdda",
     "cmp byte [rax],0", "pin", None),
    (0x532976, 4, "38903f682aeb132085de0a63fecaac4483a718348755ab6284fb7a6bd4d375d0",
     "sete sil", "pin", None),
    (0x53297A, 5, "0a827c78a98f188f5cc68da10edb40ff29a464d32bddcf5592ecf613b545091f",
     "call 0xa431e0", "call", CALL_A431E0),
    (0x53297F, 7, "0c878345cd57764adb4e76fe11145d41b81450b4be515e7c9340fd1345440792",
     "mov rax,[0x2efe318]", "mov_rip", GLOBAL_2EFE318),
    (0x532986, 2, "2e2f89076640f42236c01bdc8663b50e0001b061170a88fd2ae0a99e0c7b1675",
     "xor esi,esi", "pin", None),
    (0x532988, 3, "4eef54c393ee98ea9816e4ce4154f9264971ddf328e3ce421b06e8841edef72f",
     "mov rdi,r15", "pin", None),
    (0x53298B, 2, "6e7c72ff1c0aef12b52b1cab58166f527b87f2b37842ad6e82a9c81524c33bff",
     "call [rax]", "vcall0", None),
    (0x53298D, 7, "a33a44ce758eb4ef5e1183478d91095c0c1213a33c097d0a3bfc7ea1952ef53a",
     "mov rax,[0x2efe340]", "mov_rip", GLOBAL_2EFE340),
    (0x532994, 4, "2e886acf7022f26436056b8080c5ce424d76ae1beaefbe576186b5a6c776e253",
     "mov rax,[rax+0x60]", "pin", None),
    (0x532998, 3, "a47fe7f6ec040a966cb24a4799930b575501c9fc0592cc5f2bb577ee74f7cdda",
     "cmp byte [rax],0", "pin", None),
    (0x53299B, 2, "96130f87157d43e27bc054e16bf1575134eda74a9718cd0783013beaa07bced4",
     "je 0x5329ae", "je8", BRANCH_TARGET),
]

NATIVE = ROOT / "native/core/init_531d60_post_base_continue_c.hpp"
NATIVE_CPP = ROOT / "native/core/init_531d60_post_base_continue_c.cpp"
SLICE = ROOT / "native/core/appmain_init_ctor_98a90_slice.hpp"
SLICE_CPP = ROOT / "native/core/appmain_init_ctor_98a90_slice.cpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_a599e0_u1.hpp"
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

    require(len(UNITS) == 21, "unit count")
    require(
        "kInit531d60PostBaseA599e0U1EndRva = 0x53294b" in PREV_NATIVE.read_text(),
        "a599e0_u1 end",
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
        elif kind == "vcall0":
            require(raw[0:2] == bytes([0xFF, 0x10]), f"vcall0 opcode at 0x{rva:x}")
        elif kind == "lea":
            require(raw[0:2] in (bytes([0x48, 0x8D]), bytes([0x4C, 0x8D])),
                    f"lea opcode at 0x{rva:x}")
            modrm = raw[2]
            require((modrm & 0xC7) == 0x05, f"lea rip-rel shape at 0x{rva:x}")
            rel = struct.unpack("<i", raw[3:7])[0]
            require(rva + 7 + rel == target, f"lea target at 0x{rva:x}")
        elif kind == "mov_rip":
            require(raw[0:2] == bytes([0x48, 0x8B]), f"mov_rip opcode at 0x{rva:x}")
            modrm = raw[2]
            require((modrm & 0xC7) == 0x05, f"mov_rip rip-rel shape at 0x{rva:x}")
            rel = struct.unpack("<i", raw[3:7])[0]
            require(rva + 7 + rel == target, f"mov_rip target at 0x{rva:x}")
        elif kind == "je8":
            require(raw[0] == 0x74, f"je8 opcode at 0x{rva:x}")
            rel = struct.unpack("<b", raw[1:2])[0]
            require(rva + 2 + rel == target, f"je8 target at 0x{rva:x}")
        cursor = rva + size
    require(cursor == END, "units cover the claimed span")

    require(hashlib.sha256(img.read(START, SIZE)).hexdigest() == REGION_SHA256,
            "region sha")
    require(img.read(END, 7) == bytes.fromhex("488d050450dc02"), "post-end lea head")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_continue_c" in nh, "API")
    require("NOT modelled" in nh, "omission")
    require("kInit531d60PostBaseContinueCRva = 0x53294b" in nh, "start rva")
    require("kInit531d60PostBaseContinueCEndRva = 0x53299d" in nh, "end rva")
    require("kInit531d60PostBaseContinueCBranchTargetRva = 0x5329ae" in nh, "branch target")
    require("kInit531d60PostBaseContinueCUnitCount" in nh, "unit count")
    require("0xa431e0" in nh, "call a431e0 pin")
    require("0x32f6d18" in nh, "lea 32f6d18 pin")
    require("0x2efe318" in nh, "global 2efe318 pin")
    require("0x2efe340" in nh, "global 2efe340 pin")
    for mnemonic in ("call [rax]", "je 0x5329ae", "sete sil"):
        require(mnemonic in nh, f"unit listing {mnemonic}")
    cpp = NATIVE_CPP.read_text()
    require("branch_outcome_modelled = false" in cpp, "branch outcome flag")
    require("init_531d60_post_base_continue_c" in cpp, "cpp definition")
    st = SLICE.read_text()
    require("post_base_a599e0_u1" in st or "PostBaseA599e0U1" in st, "a599e0 u1 wire")
    require("post_base_continue_c" in st or "PostBaseContinueC" in st, "slice wire")
    require("post_base_continue_c" in SLICE_CPP.read_text(), "slice run")
    require("core/init_531d60_post_base_continue_c.cpp" in CMAKE.read_text(), "cmake")
    tt = TEST.read_text()
    for needle in ("pbcc", "hook_cont_c_a599e0", "post_base_a599e0_u1"):
        require(needle in tt, f"test wire {needle}")

    print(
        f"OK check_f531d60_post_base_continue_c: span=0x{START:x}+{SIZE}B "
        f"units={len(UNITS)} region={REGION_SHA256[:12]}…; "
        f"call=0xa431e0 pinned (0xa599e0 in u1); "
        f"vcalls=2 (slot0 @[0x2efe318]); "
        f"lea=0x32f6d18 pinned; "
        f"globals=0x2efe318x2/0x2efe340x2 pinned; "
        f"je8@{BRANCH_RVA:#x}->{BRANCH_TARGET:#x} target pinned "
        f"(default outcome unmodelled; known flag byte models taken iff zero); "
        f"end={END:#x}; "
        f"a52b70_body/a599e0_body/a431e0_body/0x53299d_block/0x5329ae_block/"
        f"incoming_r14b/r15_provenance NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
