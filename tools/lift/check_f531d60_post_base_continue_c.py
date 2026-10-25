#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue C merge run 0x532904..0x53299d (153 B / 38 units).

Private: needs local/re/eboot.elf. Pins every one of the 38 consecutive units
after E510 (continue_b je fall-through `jmp 0x532904` merge entry), the three
direct-call rel32 targets, the three indirect `call [rax]` vtable-slot-0 sites,
the three RIP-relative lea targets, the six RIP-relative global loads, and the
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
START = 0x532904
SIZE = 153
END = 0x53299D
REGION_SHA256 = "2390d483b2e1bc47fd6657dae2bda4290b4f0d5f3bf986c7c27f795164560aa1"
BRANCH_RVA = 0x53299B
BRANCH_TARGET = 0x5329AE

CALL_A52B70 = 0xA52B70
CALL_A599E0 = 0xA599E0
CALL_A431E0 = 0xA431E0
LEA_32F8B80 = 0x32F8B80
LEA_2EFE318 = 0x2EFE318
LEA_32F6D18 = 0x32F6D18
GLOBAL_2EFE318 = 0x2EFE318
GLOBAL_2EFE340 = 0x2EFE340

# (rva, size, sha256, mnemonic, kind, target)
#   kind: "call"     -> rel32 direct call (E8) target must equal target
#         "vcall0"   -> indirect call [rax] (ff 10), no target
#         "lea"      -> RIP-relative lea target must equal target
#         "mov_rip"  -> RIP-relative mov rax,[rip+disp32] target must equal target
#         "je8"      -> near je (74 <rel8>) target must equal target
#         "pin"      -> opcode/shape only
UNITS = [
    (0x532904, 4, "8aa9c86504d89f38804640a4602461ce4f77bb337096a5b98b02f57a7255aee5",
     "movzx esi,r14b", "pin", None),
    (0x532908, 3, "4eef54c393ee98ea9816e4ce4154f9264971ddf328e3ce421b06e8841edef72f",
     "mov rdi,r15", "pin", None),
    (0x53290B, 5, "c6b65f41259ea6fa7075a4c35e88a4443b6ffbecfdc08a87eced18d5f70f936a",
     "call 0xa52b70", "call", CALL_A52B70),
    (0x532910, 7, "5284fd9663f3d2c67d611f295637f9a500e501a6ac3814620dcd8ff0e9a53603",
     "lea rax,[0x32f8b80]", "lea", LEA_32F8B80),
    (0x532917, 7, "8a7024341a077f54572870d13e05bcd1149e0427f7f2ee8f657cdbe18884eb1f",
     "lea r15,[0x2efe318]", "lea", LEA_2EFE318),
    (0x53291E, 2, "2e2f89076640f42236c01bdc8663b50e0001b061170a88fd2ae0a99e0c7b1675",
     "xor esi,esi", "pin", None),
    (0x532920, 3, "4eef54c393ee98ea9816e4ce4154f9264971ddf328e3ce421b06e8841edef72f",
     "mov rdi,r15", "pin", None),
    (0x532923, 3, "65b0c3cc3d7863e2c087793da24eee81909ef4fb5f1a1080e7766dbce504e0eb",
     "mov rbx,[rax]", "pin", None),
    (0x532926, 7, "5f081fe6702348e928bce7cffdbee3230777101a0ef7b5792fef1adf4ebe8ee8",
     "mov rax,[0x2efe318]", "mov_rip", GLOBAL_2EFE318),
    (0x53292D, 2, "6e7c72ff1c0aef12b52b1cab58166f527b87f2b37842ad6e82a9c81524c33bff",
     "call [rax]", "vcall0", None),
    (0x53292F, 7, "476c829140652b3719bff2aed12f6e33902a9fc6d294a3479c3641b6d77ac283",
     "mov rax,[0x2efe340]", "mov_rip", GLOBAL_2EFE340),
    (0x532936, 2, "2e2f89076640f42236c01bdc8663b50e0001b061170a88fd2ae0a99e0c7b1675",
     "xor esi,esi", "pin", None),
    (0x532938, 3, "0bc5a45e8b7b5bfc6d88912c19ddf334fbbdb17c2a1df7b79adf0dda065730ab",
     "mov rdi,rbx", "pin", None),
    (0x53293B, 4, "2e886acf7022f26436056b8080c5ce424d76ae1beaefbe576186b5a6c776e253",
     "mov rax,[rax+0x60]", "pin", None),
    (0x53293F, 3, "a47fe7f6ec040a966cb24a4799930b575501c9fc0592cc5f2bb577ee74f7cdda",
     "cmp byte [rax],0", "pin", None),
    (0x532942, 4, "38903f682aeb132085de0a63fecaac4483a718348755ab6284fb7a6bd4d375d0",
     "sete sil", "pin", None),
    (0x532946, 5, "df35ed5325b33af8a179faf21d991ef84d5139f9f6d285a3816c54d9de24be44",
     "call 0xa599e0", "call", CALL_A599E0),
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
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_continue_b.hpp"
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

    require(len(UNITS) == 38, "unit count")
    require(
        "kInit531d60PostBaseContinueBEndRva = 0x5327b1" in PREV_NATIVE.read_text(),
        "continue_b end",
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
    # The unit immediately after the claimed span is the untaken-path entry of
    # the pinned je8; it is NOT modelled (head = lea rax,[0x32f79a8]).
    require(img.read(END, 7) == bytes.fromhex("488d050450dc02"), "post-end lea head")

    nh = NATIVE.read_text()
    require("init_531d60_post_base_continue_c" in nh, "API")
    require("NOT modelled" in nh, "omission")
    require("kInit531d60PostBaseContinueCRva = 0x532904" in nh, "start rva")
    require("kInit531d60PostBaseContinueCEndRva = 0x53299d" in nh, "end rva")
    require("kInit531d60PostBaseContinueCBranchTargetRva = 0x5329ae" in nh, "branch target")
    require("kInit531d60PostBaseContinueCUnitCount" in nh, "unit count")
    require("0xa52b70" in nh, "call a52b70 pin")
    require("0xa599e0" in nh, "call a599e0 pin")
    require("0xa431e0" in nh, "call a431e0 pin")
    require("0x32f8b80" in nh, "lea 32f8b80 pin")
    require("0x2efe318" in nh, "lea/global 2efe318 pin")
    require("0x32f6d18" in nh, "lea 32f6d18 pin")
    require("0x2efe340" in nh, "global 2efe340 pin")
    for mnemonic in ("movzx esi,r14b", "call [rax]", "je 0x5329ae", "sete sil"):
        require(mnemonic in nh, f"unit listing {mnemonic}")
    cpp = NATIVE_CPP.read_text()
    require("branch_outcome_modelled = false" in cpp, "branch outcome flag")
    require("init_531d60_post_base_continue_c" in cpp, "cpp definition")
    st = SLICE.read_text()
    require("post_base_continue_c" in st or "PostBaseContinueC" in st, "slice wire")
    require("post_base_continue_c" in SLICE_CPP.read_text(), "slice run")
    require("core/init_531d60_post_base_continue_c.cpp" in CMAKE.read_text(), "cmake")
    tt = TEST.read_text()
    for needle in ("pbcc", "hook_cont_c_a52b70", "pbcc_units"):
        require(needle in tt, f"test wire {needle}")

    print(
        f"OK check_f531d60_post_base_continue_c: span=0x{START:x}+{SIZE}B "
        f"units={len(UNITS)} region={REGION_SHA256[:12]}…; "
        f"calls=0xa52b70/0xa599e0/0xa431e0 pinned; "
        f"vcalls=3 (slot0 @[0x2efe318]); "
        f"lea=0x32f8b80/0x2efe318/0x32f6d18 pinned; "
        f"globals=0x2efe318x3/0x2efe340x3 pinned; "
        f"je8@{BRANCH_RVA:#x}->{BRANCH_TARGET:#x} target pinned (outcome NOT modelled); "
        f"end={END:#x}; "
        f"callee_bodies/je_outcome/0x53299d_block/0x5329ae_block/"
        f"incoming_r14b/r15_provenance NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
