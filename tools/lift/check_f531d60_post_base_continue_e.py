#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue E tail block 0x532b03..0x532b0c (9 B / 3 units).

Private: needs local/re/eboot.elf.  Pins every one of the 52 consecutive
instructions, the three call 0xa54920 rel32 targets, the three vcall+0x18
no-op sites with resolution pins (0x538200/0x5388c0 = ret), the one vcall+0x8
site with resolved target 0x54b250, the nine RIP-relative lea targets, the
one vmovq xmm target, and the mov [rax],r12b stamp site at 0x532a04.

Does NOT claim any callee body, the incoming r13/r12b provenance, the
0xa54920 descriptor semantics, or the 0x54b250 body.
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
START = 0x532B03
SIZE = 9
END = 0x532B0C
REGION_SHA256 = "2215fd568b5c1abc91b9679fc8db300cc4dd910938de5a84d7cb3648baa8167e"
VMOVQ_NATIVE = ROOT / "native/core/init_531d60_post_base_vmovq_532a75_u1.hpp"
LEA_NATIVE = ROOT / "native/core/init_531d60_post_base_lea_5388e0_532a7d_u1.hpp"
LEA_RSP_NATIVE = ROOT / "native/core/init_531d60_post_base_lea_rsp470_532a84_u1.hpp"
LEA_RSI_NATIVE = ROOT / "native/core/init_531d60_post_base_lea_rsi_15c8e18_532a8c_u1.hpp"
MOVQ_NATIVE = ROOT / "native/core/init_531d60_post_base_movq_rsp4a0_0_532a93_u1.hpp"
MOVRDI_NATIVE = ROOT / "native/core/init_531d60_post_base_mov_rdi_r13_532a9f_u1.hpp"
LEA538870_NATIVE = ROOT / "native/core/init_531d60_post_base_lea_538870_532aa3_u1.hpp"
LEARBX_NATIVE = ROOT / "native/core/init_531d60_post_base_lea_rbx_rsp490_532aaa_u1.hpp"
MOVRSP_NATIVE = ROOT / "native/core/init_531d60_post_base_mov_rsp470_rdx_532ab2_u1.hpp"
VMOVDQA_NATIVE = ROOT / "native/core/init_531d60_post_base_vmovdqa_rsp478_xmm0_532aba_u1.hpp"
MOV488_NATIVE = ROOT / "native/core/init_531d60_post_base_mov_rsp488_rax_532ac3_u1.hpp"
MOV4B0_NATIVE = ROOT / "native/core/init_531d60_post_base_mov_rsp4b0_rsi_532acb_u1.hpp"
MOV4B0RAX_NATIVE = ROOT / "native/core/init_531d60_post_base_mov_rsp4b0_rax_532ae7_u1.hpp"
MOVRDIRBX_NATIVE = ROOT / "native/core/init_531d60_post_base_mov_rdi_rbx_532aef_u1.hpp"
LEA2EFDA50_NATIVE = ROOT / "native/core/init_531d60_post_base_lea_rsi_2efda50_532ad3_u1.hpp"
MOV4B8_NATIVE = ROOT / "native/core/init_531d60_post_base_mov_rsp4b8_rcx_532ada_u1.hpp"
U1_NATIVE = ROOT / "native/core/init_531d60_post_base_a54920_u1.hpp"
U2_NATIVE = ROOT / "native/core/init_531d60_post_base_a54920_532a62_u1.hpp"
U3_NATIVE = ROOT / "native/core/init_531d60_post_base_a54920_532ae2_u1.hpp"
VC18_NATIVE = ROOT / "native/core/init_531d60_post_base_vcall18_532a17.hpp"
VC72_NATIVE = ROOT / "native/core/init_531d60_post_base_vcall18_532a72_u1.hpp"
VCaf2_NATIVE = ROOT / "native/core/init_531d60_post_base_vcall18_532af2_u1.hpp"

CALL_A54920 = 0xA54920
NOOP_VCALL1_RESOLVED = 0x538200   # ret
NOOP_VCALL2_RESOLVED = 0x5388C0   # ret
RESOLVED_VCALL_54B250 = 0x54B250

LEA_2DCDF14 = 0x2DCDF14
LEA_2EFD9A0 = 0x2EFD9A0
LEA_2EFD9F8 = 0x2EFD9F8
LEA_2EFDA50 = 0x2EFDA50
LEA_15C8D98 = 0x15C8D98
LEA_15C8E18 = 0x15C8E18
LEA_5388E0 = 0x5388E0
LEA_538870 = 0x538870
LEA_2EFF450 = 0x2EFF450
VMOVQ_15FF528 = 0x15FF528

# (rva, size, sha256, mnemonic, kind, target)
#   kind: "call"     -> rel32 direct call (E8); target must equal target
#         "vcall18"  -> indirect call [rax+0x18] (FF 50 18); no target
#         "vcall08"  -> indirect call [rax+0x8] (FF 50 08); no target
#         "lea_rip"  -> RIP-relative lea; target must equal target
#         "vmovq"    -> vmovq xmm,[rip+disp32]; target must equal target
#         "pin"       -> opcode/shape only
LEA_RAX532AF5_NATIVE = ROOT / "native/core/init_531d60_post_base_lea_rax_2eff450_532af5_u1.hpp"
XOR_ESI532AFC_NATIVE = ROOT / "native/core/init_531d60_post_base_xor_esi_532afc_u1.hpp"
MOV_EDX532AFE_NATIVE = ROOT / "native/core/init_531d60_post_base_mov_edx_2_532afe_u1.hpp"

UNITS = [
    (0x532B03, 3, "6355e6dc47fc8485f47f6455189f2dd9151220eae0f98847b8b09b55df9c3751",
     "mov rdi,[rax]", "pin", None),
    (0x532B06, 3, "81c7e25bd38dde9dde06d8a46d8bb57256fcbc780400d7e09baf1a8585b5b409",
     "mov rax,[rdi]", "pin", None),
    (0x532B09, 3, "3bf4b917424b527784120e40e040cc1709ac317c24237f69f8dad8bee50a4fde",
     "call [rax+0x8]", "vcall08", None),
]

NATIVE_HPP = ROOT / "native/core/init_531d60_post_base_continue_e.hpp"
NATIVE_CPP = ROOT / "native/core/init_531d60_post_base_continue_e.cpp"
SLICE_HPP = ROOT / "native/core/appmain_init_ctor_98a90_slice.hpp"
SLICE_CPP = ROOT / "native/core/appmain_init_ctor_98a90_slice.cpp"
PREV_NATIVE = ROOT / "native/core/init_531d60_post_base_continue_d.hpp"
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

    require(len(UNITS) == 3, "unit count")
    require(START + SIZE == END, "size arithmetic")

    # Verify contiguous unit coverage.
    cursor = START
    for rva, size, sha, mnemonic, kind, target in UNITS:
        require(rva == cursor, f"contiguity at 0x{rva:x} (expected 0x{cursor:x})")
        raw = img.read(rva, size)
        require(len(raw) == size, f"short read at 0x{rva:x}")
        got = hashlib.sha256(raw).hexdigest()
        require(got == sha, f"sha mismatch at 0x{rva:x}: {got} != {sha}")
        if kind == "call":
            require(raw[0] == 0xE8, f"call opcode at 0x{rva:x}")
            rel = struct.unpack("<i", raw[1:5])[0]
            require(rva + 5 + rel == target, f"call target at 0x{rva:x}")
        elif kind == "vcall18":
            require(raw[0:3] == bytes([0xFF, 0x50, 0x18]),
                    f"vcall18 opcode at 0x{rva:x}")
        elif kind == "vcall08":
            require(raw[0:3] == bytes([0xFF, 0x50, 0x08]),
                    f"vcall08 opcode at 0x{rva:x}")
        elif kind == "lea_rip":
            require(raw[0:2] in (bytes([0x48, 0x8D]), bytes([0x4C, 0x8D])),
                    f"lea opcode at 0x{rva:x}")
            modrm = raw[2]
            require((modrm & 0xC7) == 0x05, f"lea rip-rel shape at 0x{rva:x}")
            rel = struct.unpack("<i", raw[3:7])[0]
            require(rva + 7 + rel == target, f"lea target at 0x{rva:x}")
        elif kind == "vmovq":
            require(raw[0:4] == bytes([0xC5, 0xFA, 0x7E, 0x05]),
                    f"vmovq opcode at 0x{rva:x}")
            rel = struct.unpack("<i", raw[4:8])[0]
            require(rva + 8 + rel == target, f"vmovq target at 0x{rva:x}")
        cursor = rva + size
    require(cursor == END, "units cover the claimed span")

    # Full region sha.
    require(hashlib.sha256(img.read(START, SIZE)).hexdigest() == REGION_SHA256,
            "region sha")

    # The byte immediately after the block is `mov edi,0x98` (registry fragment).
    require(img.read(END, 5)[0:2] == bytes([0xBF, 0x98]),
            "post-block mov edi,0x98 head")

    # Native header checks.
    nh = NATIVE_HPP.read_text()
    require("init_531d60_post_base_continue_e" in nh, "API")
    require("NOT modelled" in nh, "omission")
    require("kInit531d60PostBaseContinueERva = 0x532b03" in nh, "start rva")
    require("merge_a54920_532ae2_u1_ran" in nh, "a54920_532ae2 merge hook")
    require("merge_a54920_u1_ran" in nh, "u1 merge hook")
    require("merge_a54920_532a62_u1_ran" in nh, "a54920_532a62 merge hook")
    require("merge_vcall18_532a17_ran" in nh, "vcall18 merge hook")
    require("merge_vcall18_532a72_u1_ran" in nh, "vcall72 merge hook")
    require("merge_vcall18_532af2_u1_ran" in nh, "vcall532af2 merge hook")
    require("merge_vmovq_532a75_u1_ran" in nh, "vmovq merge hook")
    require("merge_lea_5388e0_532a7d_u1_ran" in nh, "lea merge hook")
    require("merge_lea_rsp470_532a84_u1_ran" in nh, "lea_rsp merge hook")
    require("merge_lea_rsi_15c8e18_532a8c_u1_ran" in nh, "lea_rsi merge hook")
    require("merge_movq_rsp4a0_0_532a93_u1_ran" in nh, "movq merge hook")
    require("merge_mov_rdi_r13_532a9f_u1_ran" in nh, "mov_rdi merge hook")
    require("merge_lea_538870_532aa3_u1_ran" in nh, "lea_538870 merge hook")
    require("merge_lea_rbx_rsp490_532aaa_u1_ran" in nh, "lea_rbx merge hook")
    require("merge_mov_rsp470_rdx_532ab2_u1_ran" in nh, "mov_rsp470 merge hook")
    require("merge_vmovdqa_rsp478_xmm0_532aba_u1_ran" in nh, "vmovdqa merge hook")
    require("merge_mov_rsp488_rax_532ac3_u1_ran" in nh, "mov488 merge hook")
    require("merge_mov_rsp4b0_rsi_532acb_u1_ran" in nh, "mov4b0 merge hook")
    require("merge_lea_rsi_2efda50_532ad3_u1_ran" in nh, "lea_rsi_2efda50 merge hook")
    require("merge_mov_rsp4b8_rcx_532ada_u1_ran" in nh, "mov4b8 merge hook")
    require("merge_mov_rsp4b0_rax_532ae7_u1_ran" in nh, "mov4b0_rax merge hook")
    require("merge_mov_rdi_rbx_532aef_u1_ran" in nh, "mov_rdi_rbx merge hook")
    require("merge_lea_rax_2eff450_532af5_u1_ran" in nh, "lea_rax_532af5 merge hook")
    require("merge_xor_esi_532afc_u1_ran" in nh, "xor_esi_532afc merge hook")
    require("merge_mov_edx_2_532afe_u1_ran" in nh, "mov_edx_532afe merge hook")
    require("init_531d60_post_base_mov_rsp470_rdx_532ab2_u1" in MOVRSP_NATIVE.read_text(), "mov_rsp470 u1 API")
    require("init_531d60_post_base_vmovdqa_rsp478_xmm0_532aba_u1" in VMOVDQA_NATIVE.read_text(), "vmovdqa u1 API")
    require("init_531d60_post_base_mov_rsp488_rax_532ac3_u1" in MOV488_NATIVE.read_text(), "mov488 u1 API")
    require("init_531d60_post_base_mov_rsp4b0_rsi_532acb_u1" in MOV4B0_NATIVE.read_text(), "mov4b0 u1 API")
    require("init_531d60_post_base_lea_rsi_2efda50_532ad3_u1" in LEA2EFDA50_NATIVE.read_text(), "lea_rsi_2efda50 u1 API")
    require("init_531d60_post_base_mov_rsp4b8_rcx_532ada_u1" in MOV4B8_NATIVE.read_text(), "mov4b8 u1 API")
    require("init_531d60_post_base_mov_rsp4b0_rax_532ae7_u1" in MOV4B0RAX_NATIVE.read_text(), "mov4b0_rax u1 API")
    require("init_531d60_post_base_mov_rdi_rbx_532aef_u1" in MOVRDIRBX_NATIVE.read_text(), "mov_rdi_rbx u1 API")
    require("init_531d60_post_base_lea_rax_2eff450_532af5_u1" in LEA_RAX532AF5_NATIVE.read_text(), "lea_rax_532af5 u1 API")
    require("init_531d60_post_base_xor_esi_532afc_u1" in XOR_ESI532AFC_NATIVE.read_text(), "xor_esi_532afc u1 API")
    require("init_531d60_post_base_mov_edx_2_532afe_u1" in MOV_EDX532AFE_NATIVE.read_text(), "mov_edx_532afe u1 API")
    require("init_531d60_post_base_vmovq_532a75_u1" in VMOVQ_NATIVE.read_text(), "vmovq u1 API")
    require("init_531d60_post_base_lea_5388e0_532a7d_u1" in LEA_NATIVE.read_text(), "lea u1 API")
    require("init_531d60_post_base_lea_rsp470_532a84_u1" in LEA_RSP_NATIVE.read_text(), "lea_rsp u1 API")
    require("init_531d60_post_base_lea_rsi_15c8e18_532a8c_u1" in LEA_RSI_NATIVE.read_text(), "lea_rsi u1 API")
    require("init_531d60_post_base_movq_rsp4a0_0_532a93_u1" in MOVQ_NATIVE.read_text(), "movq u1 API")
    require("init_531d60_post_base_mov_rdi_r13_532a9f_u1" in MOVRDI_NATIVE.read_text(), "mov_rdi u1 API")
    require("init_531d60_post_base_lea_538870_532aa3_u1" in LEA538870_NATIVE.read_text(), "lea_538870 u1 API")
    require("init_531d60_post_base_lea_rbx_rsp490_532aaa_u1" in LEARBX_NATIVE.read_text(), "lea_rbx u1 API")
    require("init_531d60_post_base_a54920_u1" in U1_NATIVE.read_text(), "a54920 u1 API")
    require("init_531d60_post_base_a54920_532a62_u1" in U2_NATIVE.read_text(), "a54920_532a62 u1 API")
    require("init_531d60_post_base_a54920_532ae2_u1" in U3_NATIVE.read_text(), "a54920_532ae2 u1 API")
    require("init_531d60_post_base_vcall18_532a17" in VC18_NATIVE.read_text(), "vcall18 API")
    require("init_531d60_post_base_vcall18_532a72_u1" in VC72_NATIVE.read_text(), "vcall72 API")
    require("kInit531d60PostBaseContinueEEndRva = 0x532b0c" in nh, "end rva")
    require("kInit531d60PostBaseContinueEUnitCount" in nh, "unit count")
    require("kInit531d60PostBaseContinueECtorRva = 0xa54920" in nh, "ctor rva pin")
    require("kInit531d60PostBaseContinueENoopVcall1TargetRva = 0x538200" in nh, "noop1 pin")
    require("kInit531d60PostBaseContinueENoopVcall2TargetRva = 0x5388c0" in nh, "noop2 pin")
    require("kInit531d60PostBaseContinueEResolvedVcallRva = 0x54b250" in nh, "resolved vcall pin")
    require("kInit531d60PostBaseContinueEBossFlagRva = 0x2dcdf14" in nh, "boss flag pin")
    require("kInit531d60PostBaseContinueEName1Rva = 0x2efd9a0" in nh, "name1 pin")
    require("kInit531d60PostBaseContinueEName2Rva = 0x2efd9f8" in nh, "name2 pin")
    require("kInit531d60PostBaseContinueEName3Rva = 0x2efda50" in nh, "name3 pin")
    require("kInit531d60PostBaseContinueEVtable1Rva = 0x15c8d98" in nh, "vtable1 pin")
    require("kInit531d60PostBaseContinueEVtable2Rva = 0x15c8e18" in nh, "vtable2 pin")
    require("kInit531d60PostBaseContinueEThunk1Rva = 0x5388e0" in nh, "thunk1 pin")
    require("kInit531d60PostBaseContinueEThunk2Rva = 0x538870" in nh, "thunk2 pin")
    require("kInit531d60PostBaseContinueECell2eff450Rva = 0x2eff450" in nh, "cell2eff450 pin")
    require("kInit531d60PostBaseContinueEVmovqSrcRva = 0x15ff528" in nh, "vmovq src pin")
    require("kInit531d60PostBaseContinueEVtable2eff450Rva = 0x15c9830" in nh, "vtable 2eff450 pin")
    for mnemonic in ("call 0xa54920", "call [rax+0x18]", "call [rax+0x8]",
                     "mov [rax],r12b", "mov rdi,[r13]", "vmovq xmm0,[0x15ff528]"):
        require(mnemonic in nh, f"unit listing {mnemonic}")

    # Native cpp checks.
    ncp = NATIVE_CPP.read_text()
    require("vcall_noop_count" in ncp, "noop vcall counted")
    require("20538200" not in ncp, "no bare no-op target hex in cpp")  # sanity
    require("merge_a54920_u1_ran" in ncp, "u1 merge in cpp")
    require("merge_a54920_532a62_u1_ran" in ncp, "a54920_532a62 merge in cpp")
    require("merge_a54920_532ae2_u1_ran" in ncp, "a54920_532ae2 merge in cpp")
    require("merge_vcall18_532a17_ran" in ncp, "vcall18 merge in cpp")
    require("merge_vcall18_532a72_u1_ran" in ncp, "vcall72 merge in cpp")
    require("merge_vmovq_532a75_u1_ran" in ncp, "vmovq merge in cpp")
    require("merge_lea_5388e0_532a7d_u1_ran" in ncp, "lea merge in cpp")
    require("merge_lea_rsp470_532a84_u1_ran" in ncp, "lea_rsp merge in cpp")
    require("merge_lea_rsi_15c8e18_532a8c_u1_ran" in ncp, "lea_rsi merge in cpp")
    require("merge_movq_rsp4a0_0_532a93_u1_ran" in ncp, "movq merge in cpp")
    require("merge_mov_rdi_r13_532a9f_u1_ran" in ncp, "mov_rdi merge in cpp")
    require("merge_lea_538870_532aa3_u1_ran" in ncp, "lea_538870 merge in cpp")
    require("merge_lea_rbx_rsp490_532aaa_u1_ran" in ncp, "lea_rbx merge in cpp")
    require("merge_mov_rsp470_rdx_532ab2_u1_ran" in ncp, "mov_rsp470 merge in cpp")
    require("merge_vmovdqa_rsp478_xmm0_532aba_u1_ran" in ncp, "vmovdqa merge in cpp")
    require("merge_mov_rsp488_rax_532ac3_u1_ran" in ncp, "mov488 merge in cpp")
    require("merge_mov_rsp4b0_rsi_532acb_u1_ran" in ncp, "mov4b0 merge in cpp")
    require("merge_lea_rsi_2efda50_532ad3_u1_ran" in ncp, "lea_rsi_2efda50 merge in cpp")
    require("merge_mov_rsp4b8_rcx_532ada_u1_ran" in ncp, "mov4b8 merge in cpp")
    require("merge_mov_rsp4b0_rax_532ae7_u1_ran" in ncp, "mov4b0_rax merge in cpp")
    require("merge_mov_rdi_rbx_532aef_u1_ran" in ncp, "mov_rdi_rbx merge in cpp")
    require("merge_lea_rax_2eff450_532af5_u1_ran" in ncp, "lea_rax merge in cpp")
    require("merge_xor_esi_532afc_u1_ran" in ncp, "xor_esi merge in cpp")
    require("expect_vmovq" in ncp, "conditional vmovq validation")
    require("expect_a54920" in ncp, "conditional a54920 validation")
    require("expect_vcall_noop" in ncp, "conditional vcall noop validation")
    require("vcall_54b250_count == 1" in ncp, "1 54b250 vcall")
    require("expect_stamp" in ncp, "conditional boss-flag stamp")
    require("expect_lea_rip" in ncp, "conditional lea rip validation")
    require("expect_vmovq" in ncp, "conditional vmovq pin")

    # Slice wiring.
    shpp = SLICE_HPP.read_text()
    require("init_531d60_post_base_continue_e" in shpp, "continue_e in slice hpp")
    require("Init531d60PostBaseContinueEResult" in shpp, "continue_e result in slice hpp")
    scpp = SLICE_CPP.read_text()
    require("init_531d60_post_base_continue_e" in scpp, "continue_e in slice cpp")

    # CMake.
    cm = CMAKE.read_text()
    require("init_531d60_post_base_continue_e.cpp" in cm, "continue_e in CMake")
    require("init_531d60_post_base_a54920_532a62_u1.cpp" in cm, "a54920_532a62 in CMake")
    require("init_531d60_post_base_a54920_532ae2_u1.cpp" in cm, "a54920_532ae2 in CMake")
    require("init_531d60_post_base_vcall18_532a17.cpp" in cm, "vcall18_532a17 in CMake")
    require("init_531d60_post_base_vcall18_532a72_u1.cpp" in cm, "vcall18_532a72 in CMake")
    require("init_531d60_post_base_vmovq_532a75_u1.cpp" in cm, "vmovq_532a75 in CMake")
    require("init_531d60_post_base_lea_5388e0_532a7d_u1.cpp" in cm, "lea_5388e0_532a7d in CMake")
    require("init_531d60_post_base_lea_rsp470_532a84_u1.cpp" in cm, "lea_rsp470_532a84 in CMake")
    require("init_531d60_post_base_lea_rsi_15c8e18_532a8c_u1.cpp" in cm, "lea_rsi_532a8c in CMake")
    require("init_531d60_post_base_movq_rsp4a0_0_532a93_u1.cpp" in cm, "movq_532a93 in CMake")
    require("init_531d60_post_base_mov_rdi_r13_532a9f_u1.cpp" in cm, "mov_rdi_532a9f in CMake")
    require("init_531d60_post_base_lea_538870_532aa3_u1.cpp" in cm, "lea_538870_532aa3 in CMake")
    require("init_531d60_post_base_lea_rbx_rsp490_532aaa_u1.cpp" in cm, "lea_rbx_532aaa in CMake")
    require("init_531d60_post_base_mov_rsp470_rdx_532ab2_u1.cpp" in cm, "mov_rsp470_532ab2 in CMake")
    require("init_531d60_post_base_vmovdqa_rsp478_xmm0_532aba_u1.cpp" in cm, "vmovdqa_532aba in CMake")
    require("init_531d60_post_base_mov_rsp488_rax_532ac3_u1.cpp" in cm, "mov488_532ac3 in CMake")
    require("init_531d60_post_base_mov_rsp4b0_rsi_532acb_u1.cpp" in cm, "mov4b0_532acb in CMake")
    require("init_531d60_post_base_lea_rsi_2efda50_532ad3_u1.cpp" in cm, "lea_rsi_2efda50 in CMake")
    require("init_531d60_post_base_mov_rsp4b8_rcx_532ada_u1.cpp" in cm, "mov4b8 in CMake")
    require("init_531d60_post_base_mov_rsp4b0_rax_532ae7_u1.cpp" in cm, "mov4b0_rax in CMake")
    require("init_531d60_post_base_mov_rdi_rbx_532aef_u1.cpp" in cm, "mov_rdi_rbx in CMake")
    require("init_531d60_post_base_lea_rax_2eff450_532af5_u1.cpp" in cm, "lea_rax_532af5 in CMake")
    require("init_531d60_post_base_xor_esi_532afc_u1.cpp" in cm, "xor_esi_532afc in CMake")

    # Test text.
    tt = TEST.read_text()
    require("init_531d60_post_base_continue_e" in tt, "continue_e in test")

    # Previous batch end check (batch D ends at 0x5329ae).
    require("kInit531d60PostBaseContinueDEndRva = 0x5329ae" in
            PREV_NATIVE.read_text(), "batch D end matches block entry")

    print(
        f"OK check_f531d60_post_base_continue_e: span=0x{START:x}+{SIZE}B units=5; "
        f"calls 0x{CALL_A54920:x}×0 pinned (+3 u1); vcall+0x18 no-ops×1 pinned (+2 vcall18); "
        f"no-ops→0x{NOOP_VCALL1_RESOLVED:x}/"
        f"0x{NOOP_VCALL2_RESOLVED:x}(ret); vcall+0x8→0x{RESOLVED_VCALL_54B250:x}; "
        f"stamp 0x{LEA_2DCDF14:x}; end=0x{END:x}; "
        f"a54920_body/54b250_body/descriptor_semantics/r13_r12b_provenance NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
