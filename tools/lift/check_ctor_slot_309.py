#!/usr/bin/env python3
"""ELF tie for startup constructor index 309 (RVA 0xa505e0, 1892 B): the Save System variable group.

Offline and read-only. Bytes of the pinned private ELF (`local/re/eboot.elf`, sha256 d0723afb...5861d3)
are read through `slot_pin_util.Elf`; nothing is loaded, relocated or executed. Skips cleanly (exit 0)
when the private inputs are absent.

What is pinned here (everything is a byte read of the pinned image):

  * the constructor-table word: entry 309 of the backwards table is the u64 at
    0x2dd70e0 - 309*8 = 0x2dd6738 and must hold 0xa505e0 (the only reference to the body in the image);
  * the body extent and digest: 1892 bytes at [0xa505e0, 0xa50d44), then 0x90 nop padding;
  * the FDE length: .eh_frame_hdr (PT_GNU_EH_FRAME at 0x150c08c, datarel sdata4 table) is searched for
    an entry whose initial_location is 0xa505e0; the FDE at 0x14927b8 then declares
    initial_location 0xa505e0 and address_range 1892. That is the authoritative body extent, so the
    checker does not have to trust a disassembler for it;
  * all 40 direct `callq` sites and their targets, all 73 rip-relative operands, and the literal
    instruction bytes listed in BYTES_PINS (prologue, the three `callq *0x18(%rax)` = ff 50 18, the
    canary load/store/compare, the single branch, the epilogue and the __stack_chk_fail call);
  * the branch target: the `jne rel8` at 0xa50d2d is decoded and must land on 0xa50d3e, where the body
    calls 0xf8f3a0 (__stack_chk_fail);
  * the 12 `__cxa_atexit` rows of the native table: their call sites must be exactly the 12 `f8f390`
    call sites in body order, every object address must appear as a rip operand inside its own step
    window, and every destructor constant must appear as a rip operand at or before its step;
  * the three console-variable classes: vtable address points 0x15d1ba8 / 0x15d1c60 / 0x15d1cf8 are
    reached from the pinned `lea` operands of 0xa4f4d0 / 0xa4fec0 / 0xa50280, their 0x26ea0 gate slots
    (address point + 0x18 / + 0x30) are the RELATIVE relocation targets 0xa4d490 / 0xa4d760,
    0xa4e190 / 0xa4e310, 0xa4e830 / 0xa4e9b0, and each of those bodies is `b8 01 00 00 00 c3` (1);
  * the three argument-block functor tags 0x15d1b78 / 0x15d1c30 / 0x15d1cc8: the target of
    `[tag + 0x18]` is 0xa4f4c0 / 0xa4feb0 / 0xa50270 and each is a bare `c3` (an inert destroy);
  * the strings of the group and their pinned lengths;
  * the native side: the header constants, the ordered model steps in the source, and the fixture /
    CMake tokens.

A mutation self-test flips the first and last byte of every recorded pin and requires every mutant to
be rejected (`--no-mutate` skips it). `--tamper elf|native` runs the check on a deliberately tampered
input and therefore exits 1: a checker that cannot fail proves nothing.
"""

from __future__ import annotations

import re
import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from slot_pin_util import Elf, ROOT, const_value, digest, require  # noqa: E402

RVA = 0xA505E0
SIZE = 1892
SHA = "a559b2f9a26e6ef95c647bad80f2695a6c662da6276224c908e10b6dfd1b11a5"
TABLE_TOP = 0x2DD70E0
TABLE_INDEX = 309
TABLE_WORD = TABLE_TOP - 8 * TABLE_INDEX          # 0x2dd6738
FDE = 0x14927B8
EH_FRAME_HDR = 0x150C08C

NATIVE = {
    "header": "native/core/ctor_slot_309_a505e0.hpp",
    "source": "native/core/ctor_slot_309_a505e0.cpp",
    "test": "native/tests/test_ctor_slot_309.cpp",
    "cmake": "native/CMakeLists.txt",
}

CONSTANTS = {
    "kSlot309Rva": 0xA505E0,
    "kSlot309BodySize": 1892,
    "kSlot309EndRva": 0xA50D44,
    "kSlot309InstructionCount": 354,
    "kSlot309DirectCallSites": 40,
    "kSlot309FunctorSiteCount": 3,
    "kSlot309TableWordRva": 0x2DD6738,
    "kSlot309TableTopRva": 0x2DD70E0,
    "kSlot309TableIndex": 309,
    "kSlot309CanaryCellRva": 0x15FE518,
    "kSlot309CanaryFrameOffset": 0xC0,
    "kSlot309StackChkFailRva": 0xF8F3A0,
    "kSlot309BranchRva": 0xA50D2D,
    "kSlot309BranchTargetRva": 0xA50D3E,
    "kSlot309StackChkFailCallRva": 0xA50D3E,
    "kSlot309FrameBytes": 0xE0,
    "kSlot309AtexitRva": 0xF8F390,
    "kSlot309DsoRva": 0x1604218,
    "kSlot309ReleaseRva": 0xF590,
    "kSlot309EmptyViewCtorRva": 0xF180,
    "kSlot309ConCmdBaseCtorRva": 0x20AF0,
    "kSlot309ConCmdRegisterRva": 0x20D60,
    "kSlot309MenuToggleCtorRva": 0x26EA0,
    "kSlot309MenuRootDtorRva": 0x27280,
    "kSlot309MenuItemDtorRva": 0x272D0,
    "kSlot309VarCtor1Rva": 0xA4F4D0,
    "kSlot309VarCtor2Rva": 0xA4FEC0,
    "kSlot309VarCtor3Rva": 0xA50280,
    "kSlot309CellRva": 0x32F79B8,
    "kSlot309ClearedRva": 0x32F79C8,
    "kSlot309Var1Rva": 0x32F79E8,
    "kSlot309NameObjectRva": 0x32F7AC0,
    "kSlot309BoolVarRva": 0x32F7AF0,
    "kSlot309EmptyViewRva": 0x32F7BC0,
    "kSlot309Toggle1Rva": 0x32F7BE0,
    "kSlot309Toggle2Rva": 0x32F7C88,
    "kSlot309Var2Rva": 0x32F7D30,
    "kSlot309Toggle3Rva": 0x32F7E08,
    "kSlot309Var3Rva": 0x32F7EB0,
    "kSlot309Toggle4Rva": 0x32F7F88,
    "kSlot309VarBytes": 0xD8,
    "kSlot309BoolVarStride": 0xD0,
    "kSlot309MenuItemStride": 0xA8,
    "kSlot309CellDtorRva": 0xA46620,
    "kSlot309ClearedDtorRva": 0xA47CB0,
    "kSlot309NameObjectDtorRva": 0x2C84F0,
    "kSlot309NameObjectVptr": 0x159E1B8,
    "kSlot309EmptyViewDtorRva": 0xF7B0,
    "kSlot309Var1Vptr": 0x15D1BA8,
    "kSlot309Var2Vptr": 0x15D1C60,
    "kSlot309Var3Vptr": 0x15D1CF8,
    "kSlot309Var1Slot3Rva": 0xA4D490,
    "kSlot309Var1Slot6Rva": 0xA4D760,
    "kSlot309Var2Slot3Rva": 0xA4E190,
    "kSlot309Var2Slot6Rva": 0xA4E310,
    "kSlot309Var3Slot3Rva": 0xA4E830,
    "kSlot309Var3Slot6Rva": 0xA4E9B0,
    "kSlot309VarSlot3Value": 1,
    "kSlot309VarSlot6Value": 1,
    "kSlot309Var1FunctorTag": 0x15D1B78,
    "kSlot309Var2FunctorTag": 0x15D1C30,
    "kSlot309Var3FunctorTag": 0x15D1CC8,
    "kSlot309Var1FunctorSlot3Rva": 0xA4F4C0,
    "kSlot309Var2FunctorSlot3Rva": 0xA4FEB0,
    "kSlot309Var3FunctorSlot3Rva": 0xA50270,
    "kSlot309FunctorSlotOffset": 0x18,
    "kSlot309BoolVarFnTag": 0x15D1C10,
    "kSlot309BoolVarFnSlot3Rva": 0xC2F0,
    "kSlot309VarFlags": 1,
    "kSlot309ItemFlags": 0x30,
    "kSlot309BoolValue": 0,
    "kSlot309BoolDefault": 0,
}

# The body order of the native model (each token must appear exactly once, in this order).
SOURCE_ORDER = [
    "record(kSlot309CellDtorRva",
    "o.cleared.zero = {};",
    "build_var(o.var1",
    "kSlot309NameObjectVptr",
    "init_bool_var_static",
    "init_slot_f180",
    "o.toggle1, kSlot309Toggle1Text",
    "o.toggle2, kSlot309Toggle2Text",
    "build_var(o.var2",
    "o.toggle3, kSlot309Toggle3Text",
    "build_var(o.var3",
    "o.toggle4, kSlot309Toggle4Text",
    "out.canary_checked = true;",
]

TEST_TOKENS = [
    "section_no_var_hook",
    "section_var_hook_fails",
    "section_functor_refused",
    "section_concmd_registration_fails",
    "section_parent_contract",
    "section_canary",
    "kSlot309AtexitRows.size() == 12",
    "r.atexit_records == 12 && r.functors_destroyed == 3",
]

CMAKE_TOKENS = [
    "core/ctor_slot_309_a505e0.cpp",
    "add_test(NAME ctor_slot_309 COMMAND test_ctor_slot_309)",
]

# The 12 atexit registrations, in body order: (site, dtor, object).
ATEXIT = [
    (0xA5062D, 0xA46620, 0x32F79B8),
    (0xA5065D, 0xA47CB0, 0x32F79C8),
    (0xA50754, 0xA47D20, 0x32F79E8),
    (0xA507D6, 0x2C84F0, 0x32F7AC0),
    (0xA508E2, 0x20B0, 0x32F7AF0),
    (0xA50903, 0xF7B0, 0x32F7BC0),
    (0xA50998, 0x272D0, 0x32F7BE0),
    (0xA50A15, 0x272D0, 0x32F7C88),
    (0xA50B13, 0xA48240, 0x32F7D30),
    (0xA50B97, 0x272D0, 0x32F7E08),
    (0xA50C92, 0xA4AB50, 0x32F7EB0),
    (0xA50D16, 0x272D0, 0x32F7F88),
]

# Strings of the group: (address, text).
STRINGS = [
    (0x12C33A0, b"force_save_debug_condition"),
    (0x12C33BB, b"Debug the PS4 Save System"),
    (0x12C33D5, b"sSaveManagerShowInfoLevel"),
    (0x12C33EF, b"saveSmallMemory"),
    (0x12C33FF, b"Save System"),
    (0x12C340B, b"Force Save Debug Condition"),
    (0x12C3426, b"Fake Small Memory Available (10MB)"),
    (0x12C34D1, b"saveLoadStallState"),
    (0x12C34E4, b"Stall before executing the specified save-load state"),
    (0x12C3519, b"Load Stall State"),
    (0x12C3598, b"saveStoreStallState"),
    (0x12C35AC, b"Stall before executing the specified save-store state"),
    (0x12C35E2, b"Save Stall State"),
    (0x12C3FC0, b""),
]

# vtable address point -> (slot 3 body, slot 6 body) and every one of them returns 1.
VAR_CLASSES = [
    (0x15D1BA8, 0xA4D490, 0xA4D760),
    (0x15D1C60, 0xA4E190, 0xA4E310),
    (0x15D1CF8, 0xA4E830, 0xA4E9B0),
]

# functor tag -> the body at [tag + 0x18]: an inert destroy.
FUNCTOR_TAGS = [
    (0x15D1B78, 0xA4F4C0),
    (0x15D1C30, 0xA4FEB0),
    (0x15D1CC8, 0xA50270),
]

# Every direct `callq` of the body: (site, target). 40 sites over 10 distinct callees.
CALLS = [
    (0xa5062d, 0xf8f390),
    (0xa5065d, 0xf8f390),
    (0xa50720, 0xa4f4d0),
    (0xa5072c, 0xf590),
    (0xa50742, 0xf590),
    (0xa50754, 0xf8f390),
    (0xa507c4, 0xf590),
    (0xa507d6, 0xf8f390),
    (0xa50846, 0x20af0),
    (0xa508a8, 0xf590),
    (0xa508c5, 0x20d60),
    (0xa508cd, 0xf590),
    (0xa508e2, 0xf8f390),
    (0xa508f1, 0xf180),
    (0xa50903, 0xf8f390),
    (0xa5096c, 0x26ea0),
    (0xa50974, 0xf590),
    (0xa5097c, 0xf590),
    (0xa50998, 0xf8f390),
    (0xa509f7, 0x26ea0),
    (0xa509ff, 0xf590),
    (0xa50a07, 0xf590),
    (0xa50a15, 0xf8f390),
    (0xa50ad8, 0xa4fec0),
    (0xa50ae4, 0xf590),
    (0xa50afa, 0xf590),
    (0xa50b13, 0xf8f390),
    (0xa50b75, 0x26ea0),
    (0xa50b7d, 0xf590),
    (0xa50b85, 0xf590),
    (0xa50b97, 0xf8f390),
    (0xa50c57, 0xa50280),
    (0xa50c63, 0xf590),
    (0xa50c79, 0xf590),
    (0xa50c92, 0xf8f390),
    (0xa50cf4, 0x26ea0),
    (0xa50cfc, 0xf590),
    (0xa50d04, 0xf590),
    (0xa50d16, 0xf8f390),
    (0xa50d3e, 0xf8f3a0),
]

# Every rip-relative operand: (site, instruction length, disp offset, target). 73 operands.
RIPS = [
    (0xa505f8, 7, 3, 0x15fe518),  # movq 0xbadf19(%rip), %rax    # 0xbadf38 <.text+0xbadf38>
    (0xa505ff, 7, 3, 0x1604218),  # leaq 0xbb3c12(%rip), %rdx    # 0xbb3c38 <.text+0xbb3c38>
    (0xa50606, 7, 3, 0xa46620),  # leaq -0x9fed(%rip), %rdi     # 0xffffffffffff6040 <.text+0xffffffffffff6040>
    (0xa5060d, 7, 3, 0x32f79b8),  # leaq 0x28a73a4(%rip), %rsi   # 0x28a73d8 <.text+0x28a73d8>
    (0xa50622, 11, 3, 0x32f79b8),  # movq $0x0, 0x28a738b(%rip) # 0x28a73d8 <.text+0x28a73d8>
    (0xa50632, 7, 3, 0xa47cb0),  # leaq -0x8989(%rip), %rdi     # 0xffffffffffff76d0 <.text+0xffffffffffff76d0>
    (0xa5063d, 7, 3, 0x32f79c8),  # leaq 0x28a7384(%rip), %rsi   # 0x28a73e8 <.text+0x28a73e8>
    (0xa5064a, 8, 4, 0x32f79c8),  # vmovups %xmm0, 0x28a7376(%rip)  # 0x28a73e8 <.text+0x28a73e8>
    (0xa50652, 11, 3, 0x32f79d8),  # movq $0x0, 0x28a737b(%rip) # 0x28a73f8 <.text+0x28a73f8>
    (0xa50662, 7, 3, 0x12c33a0),  # leaq 0x872d37(%rip), %rax    # 0x872dc0 <.text+0x872dc0>
    (0xa50669, 7, 3, 0x12c33bb),  # leaq 0x872d4b(%rip), %rcx    # 0x872ddb <.text+0x872ddb>
    (0xa50670, 7, 3, 0x32f79e8),  # leaq 0x28a7371(%rip), %rdi   # 0x28a7408 <.text+0x28a7408>
    (0xa50684, 7, 3, 0x15d1b78),  # leaq 0xb814ed(%rip), %rdx    # 0xb81598 <.text+0xb81598>
    (0xa50747, 7, 3, 0xa47d20),  # leaq -0x8a2e(%rip), %rdi     # 0xffffffffffff7740 <.text+0xffffffffffff7740>
    (0xa50759, 7, 3, 0x159e1a8),  # leaq 0xb4da48(%rip), %rcx    # 0xb4dbc8 <.text+0xb4dbc8>
    (0xa50760, 7, 3, 0x12c33d5),  # leaq 0x872c6e(%rip), %rax    # 0x872df5 <.text+0x872df5>
    (0xa50770, 7, 3, 0x32f7ac0),  # leaq 0x28a7349(%rip), %rbx   # 0x28a74e0 <.text+0x28a74e0>
    (0xa5079e, 8, 4, 0x32f7ac8),  # vmovdqu %xmm0, 0x28a7322(%rip)  # 0x28a74e8 <.text+0x28a74e8>
    (0xa507a6, 8, 4, 0x32f7ad8),  # vmovups %xmm2, 0x28a732a(%rip)  # 0x28a74f8 <.text+0x28a74f8>
    (0xa507ae, 11, 3, 0x32f7ae8),  # movq $0x0, 0x28a732f(%rip) # 0x28a7508 <.text+0x28a7508>
    (0xa507bd, 7, 3, 0x32f7ac0),  # movq %rcx, 0x28a72fc(%rip)   # 0x28a74e0 <.text+0x28a74e0>
    (0xa507c9, 7, 3, 0x2c84f0),  # leaq -0x7882e0(%rip), %rdi   # 0xffffffffff877f10 <.text+0xffffffffff877f10>
    (0xa507db, 7, 3, 0x1594208),  # leaq 0xb43a26(%rip), %rbx    # 0xb43c28 <.text+0xb43c28>
    (0xa507e2, 7, 3, 0x12c33ef),  # leaq 0x872c06(%rip), %rax    # 0x872e0f <.text+0x872e0f>
    (0xa507e9, 7, 3, 0x32f7af0),  # leaq 0x28a7300(%rip), %r13   # 0x28a7510 <.text+0x28a7510>
    (0xa50808, 7, 3, 0x12c3fc0),  # leaq 0x8737b1(%rip), %rax    # 0x8739e0 <.text+0x8739e0>
    (0xa5083f, 7, 3, 0x32f7af0),  # movq %rbx, 0x28a72aa(%rip)   # 0x28a7510 <.text+0x28a7510>
    (0xa5084b, 7, 3, 0x15940a0),  # leaq 0xb4384e(%rip), %rax    # 0xb43ac0 <.text+0xb43ac0>
    (0xa50855, 7, 3, 0x32f7bb0),  # leaq 0x28a7354(%rip), %rdx   # 0x28a75d0 <.text+0x28a75d0>
    (0xa50860, 7, 3, 0x15d1c10),  # leaq 0xb813a9(%rip), %rcx    # 0xb81630 <.text+0xb81630>
    (0xa5086b, 7, 3, 0x32f7af0),  # movq %rax, 0x28a727e(%rip)   # 0x28a7510 <.text+0x28a7510>
    (0xa50872, 7, 3, 0x32f7b50),  # movq %rdx, 0x28a72d7(%rip)   # 0x28a7570 <.text+0x28a7570>
    (0xa50879, 9, 2, 0x32f7b58),  # 00 movl $0x0, 0x28a72d6(%rip)   # 0x28a7579 <.text+0x28a7579>
    (0xa50883, 8, 4, 0x32f7b60),  # vmovups %ymm0, 0x28a72d5(%rip)  # 0x28a7580 <.text+0x28a7580>
    (0xa5088b, 11, 3, 0x32f7b90),  # movq $0x0, 0x28a72fa(%rip) # 0x28a75b0 <.text+0x28a75b0>
    (0xa50896, 7, 3, 0x32f7ba0),  # movq %rcx, 0x28a7303(%rip)   # 0x28a75c0 <.text+0x28a75c0>
    (0xa5089d, 11, 3, 0x32f7ba8),  # movq $0x0, 0x28a7300(%rip) # 0x28a75c8 <.text+0x28a75c8>
    (0xa508b0, 7, 3, 0x32f7af0),  # movq %rbx, 0x28a7239(%rip)   # 0x28a7510 <.text+0x28a7510>
    (0xa508b7, 7, 2, 0x32f7bb0),  # movb $0x0, 0x28a72f2(%rip)   # 0x28a75d0 <.text+0x28a75d0>
    (0xa508be, 7, 2, 0x32f7b58),  # movb $0x0, 0x28a7293(%rip)   # 0x28a7578 <.text+0x28a7578>
    (0xa508d2, 7, 3, 0x20b0),  # leaq -0xa4e829(%rip), %rdi   # 0xffffffffff5b1ad0 <.text+0xffffffffff5b1ad0>
    (0xa508e7, 7, 3, 0x32f7bc0),  # leaq 0x28a72d2(%rip), %r14   # 0x28a75e0 <.text+0x28a75e0>
    (0xa508f6, 7, 3, 0xf7b0),  # leaq -0xa4114d(%rip), %rdi   # 0xffffffffff5bf1d0 <.text+0xffffffffff5bf1d0>
    (0xa50908, 7, 3, 0x32f7be0),  # leaq 0x28a72d1(%rip), %r14   # 0x28a7600 <.text+0x28a7600>
    (0xa5090f, 7, 3, 0x12c33ff),  # leaq 0x872ae9(%rip), %rax    # 0x872e1f <.text+0x872e1f>
    (0xa5092a, 7, 3, 0x32f79e8),  # leaq 0x28a70b7(%rip), %rcx   # 0x28a7408 <.text+0x28a7408>
    (0xa50939, 7, 3, 0x12c340b),  # leaq 0x872acb(%rip), %rax    # 0x872e2b <.text+0x872e2b>
    (0xa50981, 7, 3, 0x272d0),  # leaq -0xa296b8(%rip), %r13   # 0xffffffffff5d6cf0 <.text+0xffffffffff5d6cf0>
    (0xa50988, 7, 3, 0x1604218),  # leaq 0xbb3889(%rip), %rdx    # 0xbb3c38 <.text+0xbb3c38>
    (0xa509a2, 7, 3, 0x32f7c88),  # leaq 0x28a72df(%rip), %r12   # 0x28a76a8 <.text+0x28a76a8>
    (0xa509b3, 7, 3, 0x12c3426),  # leaq 0x872a6c(%rip), %rax    # 0x872e46 <.text+0x872e46>
    (0xa509c4, 7, 3, 0x32f7af0),  # leaq 0x28a7125(%rip), %rcx   # 0x28a7510 <.text+0x28a7510>
    (0xa50a1a, 7, 3, 0x12c34d1),  # leaq 0x872ab0(%rip), %rax    # 0x872ef1 <.text+0x872ef1>
    (0xa50a21, 7, 3, 0x32f7d30),  # leaq 0x28a7308(%rip), %r13   # 0x28a7750 <.text+0x28a7750>
    (0xa50a35, 7, 3, 0x15d1c30),  # leaq 0xb811f4(%rip), %rcx    # 0xb81650 <.text+0xb81650>
    (0xa50a82, 7, 3, 0x12c34e4),  # leaq 0x872a5b(%rip), %rax    # 0x872f04 <.text+0x872f04>
    (0xa50aff, 7, 3, 0x1604218),  # leaq 0xbb3712(%rip), %r12    # 0xbb3c38 <.text+0xbb3c38>
    (0xa50b06, 7, 3, 0xa48240),  # leaq -0x88cd(%rip), %rdi     # 0xffffffffffff7c60 <.text+0xffffffffffff7c60>
    (0xa50b18, 7, 3, 0x32f7e08),  # leaq 0x28a72e9(%rip), %r15   # 0x28a7828 <.text+0x28a7828>
    (0xa50b1f, 7, 3, 0x12c33ff),  # leaq 0x8728d9(%rip), %rax    # 0x872e1f <.text+0x872e1f>
    (0xa50b42, 7, 3, 0x12c3519),  # leaq 0x8729d0(%rip), %rax    # 0x872f39 <.text+0x872f39>
    (0xa50b8a, 7, 3, 0x272d0),  # leaq -0xa298c1(%rip), %rdi   # 0xffffffffff5d6cf0 <.text+0xffffffffff5d6cf0>
    (0xa50b9c, 7, 3, 0x12c3598),  # leaq 0x8729f5(%rip), %rax    # 0x872fb8 <.text+0x872fb8>
    (0xa50ba3, 7, 3, 0x12c35ac),  # leaq 0x872a02(%rip), %rcx    # 0x872fcc <.text+0x872fcc>
    (0xa50baa, 7, 3, 0x32f7eb0),  # leaq 0x28a72ff(%rip), %r13   # 0x28a78d0 <.text+0x28a78d0>
    (0xa50bbe, 7, 3, 0x15d1cc8),  # leaq 0xb81103(%rip), %rdx    # 0xb816e8 <.text+0xb816e8>
    (0xa50c7e, 7, 3, 0x1604218),  # leaq 0xbb3593(%rip), %r12    # 0xbb3c38 <.text+0xbb3c38>
    (0xa50c85, 7, 3, 0xa4ab50),  # leaq -0x613c(%rip), %rdi     # 0xffffffffffffa570 <.text+0xffffffffffffa570>
    (0xa50c97, 7, 3, 0x32f7f88),  # leaq 0x28a72ea(%rip), %r14   # 0x28a79a8 <.text+0x28a79a8>
    (0xa50c9e, 7, 3, 0x12c33ff),  # leaq 0x87275a(%rip), %rax    # 0x872e1f <.text+0x872e1f>
    (0xa50cc1, 7, 3, 0x12c35e2),  # leaq 0x87291a(%rip), %rax    # 0x873002 <.text+0x873002>
    (0xa50d09, 7, 3, 0x272d0),  # leaq -0xa29a40(%rip), %rdi   # 0xffffffffff5d6cf0 <.text+0xffffffffff5d6cf0>
    (0xa50d1b, 7, 3, 0x15fe518),  # movq 0xbad7f6(%rip), %rax    # 0xbadf38 <.text+0xbadf38>
]

# Literal instruction bytes that no rip/call pin covers.
BYTES_PINS = [
    (0xA505ED, "4883e4e0", "and rsp,-0x20"),
    (0xA505F1, "4881ece0000000", "sub rsp,0xe0"),
    (0xA5061A, "48898424c0000000", "canary -> 0xc0(%rsp)"),
    (0xA50697, "c744243c00000000", "var1 value cell 0"),
    (0xA5069F, "c744243800000000", "var1 min 0"),
    (0xA506A7, "c744243405000000", "var1 max 5"),
    (0xA506C1, "41b901000000", "var1 flags r9d = 1"),
    (0xA5073C, "ff5018", "callq *0x18(%rax) #1"),
    (0xA50924, "41b830000000", "toggle flags r8d = 0x30"),
    (0xA50A48, "c744243c00000000", "var2 value cell 0"),
    (0xA50A50, "c744243800000000", "var2 min 0"),
    (0xA50A58, "c744243412000000", "var2 max 0x12"),
    (0xA50A72, "41b901000000", "var2 flags r9d = 1"),
    (0xA50AF4, "ff5018", "callq *0x18(%rax) #2"),
    (0xA50B34, "41b830000000", "toggle flags r8d = 0x30"),
    (0xA50BD1, "c744243c00000000", "var3 value cell 0"),
    (0xA50BD9, "c744243800000000", "var3 min 0"),
    (0xA50BE1, "c744243414000000", "var3 max 0x14"),
    (0xA50BF6, "41b901000000", "var3 flags r9d = 1"),
    (0xA50C73, "ff5018", "callq *0x18(%rax) #3"),
    (0xA50CB3, "41b830000000", "toggle flags r8d = 0x30"),
    (0xA50D25, "483b8424c0000000", "cmpq 0xc0(%rsp), %rax"),
    (0xA50D2F, "488d65d8", "leaq -0x28(%rbp), %rsp"),
    (0xA50D33, "5b415c415d415e415f5dc3", "pops + retq"),
    (0xA50D3E, "e85de6530090", "callq __stack_chk_fail + the body's last byte (nop)"),
]

# The three `movq $0, <rip>` stores: their imm32 tail must be zero.
ZERO_PINS = [
    (0xA50622 + 7, "cell (0x32f79b8)"),
    (0xA50652 + 7, "cleared + 0x10 (0x32f79d8)"),
    (0xA507AE + 7, "name object +0x28"),
]


def fde_range(e: Elf, initial: int) -> int:
    """address_range of the FDE whose initial_location is `initial` (PT_GNU_EH_FRAME datarel table)."""
    header = e.raw(EH_FRAME_HDR, 12)
    require(header[0] == 1 and header[1] == 0x1b and header[2] == 0x03 and header[3] == 0x3b,
            "eh_frame_hdr version/encodings")
    count = struct.unpack("<I", header[8:12])[0]
    table = e.img.read(EH_FRAME_HDR + 12, count * 8)
    for i in range(count):
        init, fde = struct.unpack_from("<ii", table, 8 * i)
        if init + EH_FRAME_HDR != initial:
            continue
        fde += EH_FRAME_HDR
        record = e.read(fde, 16)                       # pinned: the FDE record
        require(struct.unpack("<I", record[0:4])[0] == 36, "FDE record length")
        location = fde + 8 + struct.unpack("<i", record[8:12])[0]
        require(location == initial, f"FDE initial_location 0x{location:x}")
        return struct.unpack("<I", record[12:16])[0]
    raise SystemExit(f"FAIL no FDE for 0x{initial:x} in eh_frame_hdr")


def check_elf(e: Elf) -> None:
    # --- the startup-table row: the only reference to the body in the image ---------------------
    require(TABLE_TOP - 8 * TABLE_INDEX == TABLE_WORD == 0x2DD6738, "table row arithmetic")
    require(e.u64(TABLE_WORD) == RVA, "constructor-table entry 309 -> 0xa505e0")

    # --- body extent, digest, padding -----------------------------------------------------------
    body = e.read(RVA, SIZE)
    if not e.skip_hash:
        require(digest(body) == SHA, "body sha256")
    require(body[0] == 0x55 and body[1:4] == b"\x48\x89\xe5", "push rbp; mov rsp,rbp")
    require(e.read(RVA + SIZE, 4) == b"\x90\x90\x90\x90", "nop padding after the body")

    # --- the FDE is the authoritative extent ----------------------------------------------------
    require(fde_range(e, RVA) == SIZE, "FDE address_range == 1892")

    # --- every direct call, every rip operand, the literal bytes --------------------------------
    require(len(CALLS) == 40 and len({site for site, _ in CALLS}) == 40, "40 distinct call sites")
    for site, target in CALLS:
        require(e.call(site) == target, f"call at 0x{site:x} -> 0x{target:x}")
    require(len(RIPS) == 73 and len({site for site, _, _, _ in RIPS}) == 73, "73 distinct rip sites")
    for site, length, disp_off, target in RIPS:
        require(e.rip(site, length, disp_off) == target, f"rip operand at 0x{site:x} -> 0x{target:x}")
    for site, want, what in BYTES_PINS:
        e.bytes_at(site, want, what)
    for addr, what in ZERO_PINS:
        require(e.read(addr, 4) == b"\0\0\0\0", f"zero immediate at 0x{addr:x} ({what})")

    # --- the single branch, decoded -----------------------------------------------------------------
    branch = e.read(0xA50D2D, 2)
    require(branch[0] == 0x75, "the body's only branch is jne rel8 at 0xa50d2d")
    require(0xA50D2D + 2 + struct.unpack("<b", branch[1:2])[0] == 0xA50D3E, "jne target 0xa50d3e")
    require(e.call(0xA50D3E) == 0xF8F3A0, "the branch target calls __stack_chk_fail")
    require(e.rip(0xA505F8, 7, 3) == 0x15FE518, "canary read from the 0x15fe518 GOT cell")

    # --- the 12 __cxa_atexit registrations, in body order -----------------------------------------
    atexit_sites = [site for site, target in CALLS if target == 0xF8F390]
    require(atexit_sites == [site for site, _, _ in ATEXIT], "the 12 atexit sites in body order")
    rip_at = {site: target for site, _, _, target in RIPS}
    known_dtors = {dtor for _, dtor, _ in ATEXIT}
    first_object = {}
    for site, target in sorted(rip_at.items()):
        first_object.setdefault(target, site)
    previous = -1
    for site, dtor, obj in ATEXIT:
        where = first_object.get(obj)
        require(where is not None, f"object 0x{obj:x} is loaded by a rip operand")
        require(previous < where <= site, f"object 0x{obj:x} is loaded inside its own step window")
        previous = site
        dtor_sites = [s for s, t in rip_at.items() if t in known_dtors and s <= site]
        require(dtor_sites, f"step at 0x{site:x} loads a destructor constant before the call")
        require(rip_at[max(dtor_sites)] == dtor, f"step at 0x{site:x} registers dtor 0x{dtor:x}")

    # --- the three console-variable classes and their 0x26ea0 gate slots ---------------------------
    for vptr, slot3, slot6 in VAR_CLASSES:
        require(e.reloc_target(vptr + 0x18) == slot3, f"vptr 0x{vptr:x} slot 3 -> 0x{slot3:x}")
        require(e.reloc_target(vptr + 0x30) == slot6, f"vptr 0x{vptr:x} slot 6 -> 0x{slot6:x}")
        for body_rva in (slot3, slot6):
            e.bytes_at(body_rva, "b801000000c3", f"gate slot 0x{body_rva:x} returns 1")

    # --- the three argument-block functor tags destroy through a bare ret ---------------------------
    for tag, slot3 in FUNCTOR_TAGS:
        require(e.reloc_target(tag + 0x18) == slot3, f"functor tag 0x{tag:x} slot 3 -> 0x{slot3:x}")
        e.bytes_at(slot3, "c3", f"functor slot 3 0x{slot3:x} is a bare ret")

    # --- strings ----------------------------------------------------------------------------------
    for addr, text in STRINGS:
        e.cstring(addr, text)


def check_native(texts: dict[str, str]) -> None:
    header = texts["header"]
    for name, value in CONSTANTS.items():
        require(const_value(header, name) == value, f"native constant {name}")
    positions = []
    for token in SOURCE_ORDER:
        require(token in texts["source"], f"source token {token!r}")
        positions.append(texts["source"].index(token))
    require(positions == sorted(positions) and len(set(positions)) == len(positions),
            "native model steps are in body order")
    for token in TEST_TOKENS:
        require(token in texts["test"], f"test token {token!r}")
    for token in CMAKE_TOKENS:
        require(token in texts["cmake"], f"cmake token {token!r}")


def read_native() -> dict[str, str]:
    return {key: (ROOT / path).read_text() for key, path in NATIVE.items()}


def run(e: Elf, texts: dict[str, str]) -> None:
    check_elf(e)
    check_native(texts)


def expect_failure(label: str, fn) -> None:
    try:
        fn()
    except SystemExit as error:
        require(str(error).startswith("FAIL"), f"mutant {label}: unexpected exit {error!r}")
        return
    raise SystemExit(f"FAIL mutant survived: {label}")


def mutation_self_test(e: Elf, texts: dict[str, str]) -> tuple[int, int]:
    e.recording = True
    check_elf(e)
    e.recording = False
    pins = sorted(e.pins)
    e.skip_hash = True
    ran = survivors = 0
    for addr, size in pins:
        for byte_addr in sorted({addr, addr + size - 1}):
            e.overlay = {}
            pristine = e.raw(byte_addr, 1)[0]
            e.overlay = {byte_addr: pristine ^ 0x01}
            ran += 1
            try:
                check_elf(e)
            except SystemExit:
                continue
            survivors += 1
            print(f"mutation survived at 0x{byte_addr:x} (pin 0x{addr:x}+{size})", file=sys.stderr)
    e.overlay = {}
    e.skip_hash = False
    require(survivors == 0, f"{survivors} ELF mutations survived")

    native_ran = 0
    for key, tokens in (("header", list(CONSTANTS)), ("source", SOURCE_ORDER),
                        ("test", TEST_TOKENS), ("cmake", CMAKE_TOKENS)):
        for token in tokens:
            if token not in texts[key]:
                continue
            mutated = dict(texts)
            mutated[key] = texts[key].replace(token, "\0MUTATED\0")
            expect_failure(f"{key} token {token!r}", lambda m=mutated: check_native(m))
            native_ran += 1
    for name, value in CONSTANTS.items():
        pattern = re.compile(rf"({re.escape(name)}\s*=\s*)(0x[0-9A-Fa-f]+|\d+)(u?\s*;)")
        mutated = dict(texts)
        mutated["header"] = pattern.sub(lambda m: f"{m.group(1)}{value ^ 0x4:#x}{m.group(3)}", texts["header"], count=1)
        require(mutated["header"] != texts["header"], f"constant {name} pattern did not match")
        expect_failure(f"constant {name}", lambda m=mutated: check_native(m))
        native_ran += 1
    return ran, native_ran


def main() -> int:
    arguments = sys.argv[1:]
    if not (ROOT / "local/re/eboot.elf").is_file():
        print("SKIP check_ctor_slot_309: local/re/eboot.elf not present")
        return 0
    e = Elf()
    texts = read_native()
    if "--tamper" in arguments:
        mode = arguments[arguments.index("--tamper") + 1] if len(arguments) > arguments.index("--tamper") + 1 else "elf"
        if mode == "elf":
            e.overlay = {RVA + 0x10: e.raw(RVA + 0x10, 1)[0] ^ 0x40}
        else:
            texts = dict(texts)
            texts["header"] = texts["header"].replace("kSlot309Rva", "kTampered", 1)
    e.recording = True
    run(e, texts)
    e.recording = False
    note = ""
    if "--no-mutate" not in arguments and "--tamper" not in arguments:
        elf_runs, native_runs = mutation_self_test(e, texts)
        note = f"; mutation self-test {elf_runs} ELF + {native_runs} native mutants all rejected"
    print(f"OK check_ctor_slot_309: slot 309 (0xa505e0, {SIZE} B, sha {SHA[:12]}..., FDE range "
          f"{SIZE}) is row 309 of the constructor table at 0x{TABLE_WORD:x}; 40 direct calls, 73 rip "
          f"operands, 1 branch to 0xa50d3e (__stack_chk_fail), 3 `callq *0x18(%rax)` inert functor "
          f"destroys, 12 __cxa_atexit rows, 3 variable classes with gate slots = 1; guest bytes NOT "
          f"executed{note}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
