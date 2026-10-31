#!/usr/bin/env python3
"""ELF-tie Init 0x531d60 continue E tail block 0x532a0c..0x532b0c (256 B / 40 units).

Private: needs local/re/eboot.elf.  Pins every one of the 53 consecutive
instructions, the three call 0xa54920 rel32 targets, the three vcall+0x18
no-op sites with resolution pins (0x538200/0x5388c0 = ret), the one vcall+0x8
site with resolved target 0x54b250, the ten RIP-relative lea targets, the
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
START = 0x532A0C
SIZE = 256
END = 0x532B0C
REGION_SHA256 = "f1e5b5d3bd29fd2944b5e8e84411d0f04d275464f4464ad27b2686d6cec6047a"
U1_NATIVE = ROOT / "native/core/init_531d60_post_base_a54920_u1.hpp"

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
UNITS = [
    (0x532A0C, 8, "effa8e69926c8ef23f321ca74246f1225f29e9eab1de06d451e3a58bfccc56eb",
     "mov rax,[rsp+0x560]", "pin", None),
    (0x532A14, 3, "34eb50fdb7685f0af0775bc2ce9660fd5573393e1dac2a8c90d2ff1cb877d999",
     "mov rdi,r14", "pin", None),
    (0x532A17, 3, "22558207d1d705fcbda49d89be5b34ebe09d409878b0bf88e6819b7724e29e96",
     "call [rax+0x18]", "vcall18", None),
    (0x532A1A, 4, "5cbe8a11db9e94898aff3c8ff441d398f6eb9db110410761e1561b388d73a64b",
     "mov rdi,[r13]", "pin", None),
    (0x532A1E, 7, "1784ca2e16bd3ff8f129610b98b787c9f6b5a6b80f78258767327bde8adb2183",
     "lea rsi,[0x2efd9f8]", "lea_rip", LEA_2EFD9F8),
    (0x532A25, 8, "5c5e0f33e8a8e1c712e62fa8dff92b03c69b879d112e9741219f449941a1661f",
     "lea rdx,[rsp+0x4C0]", "pin", None),
    (0x532A2D, 4, "8b214ae88f0f588c81fdf4ffe8a118e62d66db17278a67c43100ae6aa2c44103",
     "vxorps ymm0,ymm0,ymm0", "pin", None),
    (0x532A31, 12, "077a730439ed84af6284e3dcc92f8b47bcc0e4dedebd02f51e32221e362eedc0",
     "mov qword [rsp+0x4F0],0", "pin", None),
    (0x532A3D, 8, "4d08b8b002b44067e68a40b337ef208cb3c5569567493bc83ced4029b2a85ecc",
     "lea r14,[rsp+0x4E0]", "pin", None),
    (0x532A45, 9, "14a25a1179edc25439298d65be39a343c440742774698f5ffdd9e7ac53ef12c0",
     "vmovaps [rsp+0x4C0],ymm0", "pin", None),
    (0x532A4E, 8, "600efcec713b1f42e17c8bf233b80e7bf6d35910650d7b42aa583dd1e6acd6ad",
     "mov [rsp+0x500],rbx", "pin", None),
    (0x532A56, 12, "44a5b926c89fa02e1d8c037036ccff511b9e5e259f0c91864bfd3008bd464de1",
     "mov qword [rsp+0x508],0", "pin", None),
    (0x532A62, 5, "05ace5dabb92d36d05fbf80283594b9c2ec091148c8fdf72cee43cec24a84a8d",
     "call 0xa54920", "call", CALL_A54920),
    (0x532A67, 8, "47e317e78bed8720e4a7954469d5a8bdba196684cdf37665661e1a1a431228a4",
     "mov rax,[rsp+0x500]", "pin", None),
    (0x532A6F, 3, "34eb50fdb7685f0af0775bc2ce9660fd5573393e1dac2a8c90d2ff1cb877d999",
     "mov rdi,r14", "pin", None),
    (0x532A72, 3, "22558207d1d705fcbda49d89be5b34ebe09d409878b0bf88e6819b7724e29e96",
     "call [rax+0x18]", "vcall18", None),
    (0x532A75, 8, "f970e4d743bedbd28f18045f6cd2d6c61d1367f844519a5fe55646970144c670",
     "vmovq xmm0,[0x15ff528]", "vmovq", VMOVQ_15FF528),
    (0x532A7D, 7, "8ed7d0f04cc53d86379785218e047d03c6415d6038a62558fe1d66d3ef9cc12b",
     "lea rax,[0x5388e0]", "lea_rip", LEA_5388E0),
    (0x532A84, 8, "461528923dc1ceb08ec82a9875c84ae4bdd94f89c5c136c6a3b1423b2658057d",
     "lea rdx,[rsp+0x470]", "pin", None),
    (0x532A8C, 7, "289697dd92f3475205bb3a4361d796f472d13d9ec5db442dad1d3ff59d970565",
     "lea rsi,[0x15c8e18]", "lea_rip", LEA_15C8E18),
    (0x532A93, 12, "f8a5ac42b01ee8b7274f6b01819cbb3ca6fae4e97f184f93d8482bfd92235a0f",
     "mov qword [rsp+0x4A0],0", "pin", None),
    (0x532A9F, 4, "5cbe8a11db9e94898aff3c8ff441d398f6eb9db110410761e1561b388d73a64b",
     "mov rdi,[r13]", "pin", None),
    (0x532AA3, 7, "0a4403826a6f1006e0fecae4fb334f077ba4318887ac325f479ad43795ec14e9",
     "lea rcx,[0x538870]", "lea_rip", LEA_538870),
    (0x532AAA, 8, "220adb28ef1e5ca9c923a5e40ce57d1917a64dd6c7f61e878ada6dc93dc6cbce",
     "lea rbx,[rsp+0x490]", "pin", None),
    (0x532AB2, 8, "cac7360526afa13797d46b776f807c4469acdac55b8c803039671171836c9d85",
     "mov [rsp+0x470],rdx", "pin", None),
    (0x532ABA, 9, "35346c53edb6caf2e4dce695c58808ed3e279f889d534ab9ba6f697e34fe9bc1",
     "vmovdqa [rsp+0x478],xmm0", "pin", None),
    (0x532AC3, 8, "5d8fd0a894ccbe37fed4e3633ffaef6b99d2a25d3cedf931f9d74a9bffa5f188",
     "mov [rsp+0x488],rax", "pin", None),
    (0x532ACB, 8, "d3742f2c57cd908ee5ecbda7fd0789a57c74c6a8de0db64521094caccd50b3a3",
     "mov [rsp+0x4B0],rsi", "pin", None),
    (0x532AD3, 7, "789228dafb710348d35a950b268d0872ae9833bc09f169b1f842bb4e8dfe352c",
     "lea rsi,[0x2efda50]", "lea_rip", LEA_2EFDA50),
    (0x532ADA, 8, "4cb548c13bf8f1664150ce0f13ed638f3714e8becf4cb4761ea5dfa0b7a08ba3",
     "mov [rsp+0x4B8],rcx", "pin", None),
    (0x532AE2, 5, "afb494bc73544c849f9ab6a180568c5f4233d1bb099ccb4fd395f3dedc9abca0",
     "call 0xa54920", "call", CALL_A54920),
    (0x532AE7, 8, "c6805245a1f0fa4a03404838e7a1d318ab410d133ca959599ff7320c7f1029bf",
     "mov rax,[rsp+0x4B0]", "pin", None),
    (0x532AEF, 3, "0bc5a45e8b7b5bfc6d88912c19ddf334fbbdb17c2a1df7b79adf0dda065730ab",
     "mov rdi,rbx", "pin", None),
    (0x532AF2, 3, "22558207d1d705fcbda49d89be5b34ebe09d409878b0bf88e6819b7724e29e96",
     "call [rax+0x18]", "vcall18", None),
    (0x532AF5, 7, "4cee64d34e337abd4b52b8fb14bbe891d22e32a51ffcebc92595df69dab2c773",
     "lea rax,[0x2eff450]", "lea_rip", LEA_2EFF450),
    (0x532AFC, 2, "2e2f89076640f42236c01bdc8663b50e0001b061170a88fd2ae0a99e0c7b1675",
     "xor esi,esi", "pin", None),
    (0x532AFE, 5, "252ddaa7ae0a8d071529fab21284ab34cb7bda4eb60c58c59dcfec5f79338f15",
     "mov edx,2", "pin", None),
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

    require(len(UNITS) == 40, "unit count")
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
    require("kInit531d60PostBaseContinueERva = 0x532a0c" in nh, "start rva")
    require("merge_a54920_u1_ran" in nh, "u1 merge hook")
    require("init_531d60_post_base_a54920_u1" in U1_NATIVE.read_text(), "a54920 u1 API")
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
    require("call_a54920_count == 3" in ncp, "3 a54920 calls total")
    require("vcall_noop_count == 3" in ncp, "3 noop vcalls counted")
    require("vcall_54b250_count == 1" in ncp, "1 54b250 vcall")
    require("stamp_count == 1" in ncp, "1 boss-flag stamp")
    require("lea_rip_count == 9" in ncp, "9 lea rip pins total")
    require("vmovq_count == 1" in ncp, "1 vmovq pin")

    # Slice wiring.
    shpp = SLICE_HPP.read_text()
    require("init_531d60_post_base_continue_e" in shpp, "continue_e in slice hpp")
    require("Init531d60PostBaseContinueEResult" in shpp, "continue_e result in slice hpp")
    scpp = SLICE_CPP.read_text()
    require("init_531d60_post_base_continue_e" in scpp, "continue_e in slice cpp")

    # CMake.
    cm = CMAKE.read_text()
    require("init_531d60_post_base_continue_e.cpp" in cm, "continue_e in CMake")

    # Test text.
    tt = TEST.read_text()
    require("init_531d60_post_base_continue_e" in tt, "continue_e in test")

    # Previous batch end check (batch D ends at 0x5329ae).
    require("kInit531d60PostBaseContinueDEndRva = 0x5329ae" in
            PREV_NATIVE.read_text(), "batch D end matches block entry")

    print(
        f"OK check_f531d60_post_base_continue_e: span=0x{START:x}+{SIZE}B units=40; "
        f"calls 0x{CALL_A54920:x}×2 pinned (+1 u1); vcall+0x18 no-ops×3→0x{NOOP_VCALL1_RESOLVED:x}/"
        f"0x{NOOP_VCALL2_RESOLVED:x}(ret); vcall+0x8→0x{RESOLVED_VCALL_54B250:x}; "
        f"stamp 0x{LEA_2DCDF14:x}; end=0x{END:x}; "
        f"a54920_body/54b250_body/descriptor_semantics/r13_r12b_provenance NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
