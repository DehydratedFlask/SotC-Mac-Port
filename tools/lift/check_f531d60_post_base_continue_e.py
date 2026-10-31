#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue E — empty pinned span @ 0x532b0c (E547).

Private: needs local/re/eboot.elf.  The 350 B block is modelled via peel u1 modules;
the pinned continue_e region is 0 B after vcall08_532b09_u1 @ 0x532b09..0x532b0c.
"""

from __future__ import annotations

import hashlib
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
START = 0x532B0C
SIZE = 0
END = 0x532B0C
REGION_SHA256 = "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"
VCALL08_NATIVE = ROOT / "native/core/init_531d60_post_base_vcall08_532b09_u1.hpp"
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
LEA_RAX532AF5_NATIVE = ROOT / "native/core/init_531d60_post_base_lea_rax_2eff450_532af5_u1.hpp"
XOR_ESI532AFC_NATIVE = ROOT / "native/core/init_531d60_post_base_xor_esi_532afc_u1.hpp"
MOV_EDX532AFE_NATIVE = ROOT / "native/core/init_531d60_post_base_mov_edx_2_532afe_u1.hpp"
MOVRDI532B03_NATIVE = ROOT / "native/core/init_531d60_post_base_mov_rdi_rax_mem_532b03_u1.hpp"
MOVRAX532B06_NATIVE = ROOT / "native/core/init_531d60_post_base_mov_rax_rdi_mem_532b06_u1.hpp"

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

    require(START + SIZE == END, "size arithmetic")
    require(hashlib.sha256(img.read(START, SIZE)).hexdigest() == REGION_SHA256, "region sha")
    require(img.read(END, 5)[0:2] == bytes([0xBF, 0x98]), "post-span mov edi,0x98 head")

    nh = NATIVE_HPP.read_text()
    require("init_531d60_post_base_continue_e" in nh, "API")
    require("NOT modelled" in nh, "omission")
    require("kInit531d60PostBaseContinueERva = 0x532b0c" in nh, "start rva")
    require("kInit531d60PostBaseContinueESize = 0" in nh, "empty size")
    require("0 units in empty continue_e @ 0x532b0c" in nh, "unit count 0")
    require("merge_vcall08_532b09_u1_ran" in nh, "vcall08 merge hook")
    require("merge_mov_rax_rdi_mem_532b06_u1_ran" in nh, "mov rax merge hook")
    require("init_531d60_post_base_vcall08_532b09_u1" in VCALL08_NATIVE.read_text(), "vcall08 u1 API")

    ncp = NATIVE_CPP.read_text()
    require("init_531d60_post_base_vcall08_532b09_u1" in ncp, "vcall08 in cpp")
    require("expect_vcall_54b250" in ncp, "conditional 54b250 validation")
    require("ContinueEUnit::vcall_rax08" not in ncp, "no continue_e vcall unit pin")

    cm = CMAKE.read_text()
    require("init_531d60_post_base_vcall08_532b09_u1.cpp" in cm, "vcall08 in CMake")
    require("init_531d60_post_base_continue_e.cpp" in cm, "continue_e in CMake")

    tt = TEST.read_text()
    require("kInit531d60PostBaseContinueEUnitCount == 0" in tt, "test expects empty continue_e")
    require("merge_mov_edi_98_532b0c_u1_ran" in tt, "registry merge in test")

    require("kInit531d60PostBaseContinueDEndRva = 0x5329ae" in PREV_NATIVE.read_text(),
            "batch D end matches block entry")

    print(
        f"OK check_f531d60_post_base_continue_e: empty span=0x{START:x}+{SIZE}B; "
        f"vcall08 @0x532b09 (+u1); registry mov edi @0x532b0c; end=0x{END:x}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
