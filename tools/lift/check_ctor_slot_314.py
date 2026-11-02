#!/usr/bin/env python3
"""ELF tie for startup constructor index 314 (RVA 0xa41810, 4328 B FDE): the physics debug block.

Offline and read-only. Bytes of the pinned private ELF (`local/re/eboot.elf`, sha256 d0723afb...5861d3)
are read through `slot_pin_util.Elf`; nothing is loaded, relocated or executed. Skips cleanly (exit 0)
when the private inputs are absent.

What is pinned here (everything is a byte read of the pinned image):

  * the constructor-table word: entry 314 of the backwards table is the u64 at
    0x2dd70e0 - 314*8 = 0x2dd6710 and must hold 0xa41810;
  * the body extent and digest: the FDE declares 4328 B at [0xa41810, 0xa428f8); the two digests are
    pinned, the FDE ends exactly where the last reached instruction ends, and the padding after it is
    nop;
  * all 84 direct `callq` sites and their targets;
  * all 196 rip-relative operands (63 of them stores) and the branch/canary sequence;
  * the 8 `callq *0x18(%rax)` sites: their argument-functor tag and the bare-`ret` slot 3 body;
  * the 22 `__cxa_atexit` rows in body order, each with its destructor constant loaded before the call
    and its object loaded inside its own step window;
  * the object map: the page, the ten variables, the ten toggles and the zero cell, by their pinned
    original addresses and strides (the aggregate must land exactly on slot 313's first object);
  * the page constructor 0x475c0 (FDE 87 B, its vptr/store sequence) and the callback bool constructor
    0xa41570 (FDE 669 B: the ten functor dispatches, the two 0x20af0/0x20d60 calls, the final vptr and
    functor-tag stores, its own canary branch to __stack_chk_fail);
  * the four target gate bodies (0xb890 -> 1, 0xb930 -> 0, 0xbba0 -> 1, 0xbbf0 -> 3) and both
    argument-functor tables (0x15d1750 -> 0xc2a0/0xc2c0/0xc2e0/0xc2f0, 0x15d1770 ->
    0xc4a0/0xc4c0/0xc4e0/0xc4f0), all four 3-byte copy bodies and both bare-ret destroy slots;
  * the strings of the block and their pinned lengths;
  * the native side: the header constants, the ordered model steps in the source, and the
    fixture / CMake tokens.

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

RVA = 0xA41810
SIZE = 4329                                    # FDE address_range [0xa41810, 0xa428f9)
REACHABLE = 4328                               # the last reached instruction ends at 0xa428f8
SHA = "0368255a27de7bc548783937baf5a648b2b14433cddf3c218ee82d3d4cd1f218"
SHA_REACHABLE = "3e051b27f677728ea6b2d6dfe2bf6be74873270bb358c2eb8e1871d0300f63e0"
TABLE_TOP = 0x2DD70E0
TABLE_INDEX = 314
TABLE_WORD = TABLE_TOP - 8 * TABLE_INDEX       # 0x2dd6710
EH_FRAME_HDR = 0x150C08C

PAGE_CTOR = 0x475C0
PAGE_CTOR_SIZE = 87
BOOL_CB_CTOR = 0xA41570
BOOL_CB_CTOR_SIZE = 669

NATIVE = {
    "header": "native/core/ctor_slot_314_a41810.hpp",
    "source": "native/core/ctor_slot_314_a41810.cpp",
    "test": "native/tests/test_ctor_slot_314.cpp",
    "cmake": "native/CMakeLists.txt",
}

CONSTANTS = {
    "kSlot314Rva": 0xA41810,
    "kSlot314BodySize": 4329,
    "kSlot314EndRva": 0xA428F9,
    "kSlot314ReachableBytes": 4328,
    "kSlot314InstructionCount": 779,
    "kSlot314DirectCallSites": 84,
    "kSlot314IndirectCallSites": 8,
    "kSlot314RipOperands": 196,
    "kSlot314RipTargets": 119,
    "kSlot314StoreSites": 63,
    "kSlot314StoreTargets": 56,
    "kSlot314TableTopRva": 0x2DD70E0,
    "kSlot314TableWordRva": 0x2DD6710,
    "kSlot314TableIndex": 314,
    "kSlot314NextUnrecoveredIndex": 335,
    "kSlot314NextUnrecoveredRva": 0x7EDF10,
    "kSlot314CanaryCellRva": 0x15FE518,
    "kSlot314CanaryFrameOffset": 0xC0,
    "kSlot314StackChkFailRva": 0xF8F3A0,
    "kSlot314BranchRva": 0xA428E2,
    "kSlot314BranchTargetRva": 0xA428F3,
    "kSlot314FrameBytes": 0xE0,
    "kSlot314AtexitRva": 0xF8F390,
    "kSlot314DsoRva": 0x1604218,
    "kSlot314ReleaseRva": 0xF590,
    "kSlot314ConCmdBaseCtorRva": 0x20AF0,
    "kSlot314ConCmdRegisterRva": 0x20D60,
    "kSlot314MenuToggleCtorRva": 0x26EA0,
    "kSlot314MenuRegisterRva": 0x268B0,
    "kSlot314FloatVarCtorRva": 0xC300,
    "kSlot314BoolCallbackCtorRva": 0xA41570,
    "kSlot314PageCtorRva": 0x475C0,
    "kSlot314PageCtorSize": 87,
    "kSlot314BoolCallbackCtorSize": 669,
    "kSlot314PageCtorVptr": 0x1595900,
    "kSlot314PageVptr": 0x1594CA0,
    "kSlot314BoolMidVptr": 0x15940B0,
    "kSlot314BoolFinalVptr": 0x1594218,
    "kSlot314FloatMidVptr": 0x1594128,
    "kSlot314FloatFinalVptr": 0x15942B0,
    "kSlot314FloatFnVtbl": 0x1594318,
    "kSlot314BoolCallbackFnVtbl": 0x1594280,
    "kSlot314BoolArgFnVtbl": 0x15D1750,
    "kSlot314FloatArgFnVtbl": 0x15D1770,
    "kSlot314BoolArgDestroyRva": 0xC2F0,
    "kSlot314FloatArgDestroyRva": 0xC4F0,
    "kSlot314BoolGate3": 1,
    "kSlot314BoolGate6": 0,
    "kSlot314FloatGate3": 1,
    "kSlot314FloatGate6": 3,
    "kSlot314PageDtorRva": 0x1CE70,
    "kSlot314BoolVarDtorRva": 0x20B0,
    "kSlot314FloatVarDtorRva": 0x20F0,
    "kSlot314CellDtorRva": 0xA3D9E0,
    "kSlot314PageRva": 0x32F5AF0,
    "kSlot314PageSize": 0x2E8,
    "kSlot314CellRva": 0x32F6538,
    "kSlot314CellSize": 0xB0,
    "kSlot314LastObjectRva": 0x32F6C70,
    "kSlot314NextCtorFirstObjectRva": 0x32F6D18,
    "kSlot314PageNameRva": 0x12C2760,
    "kSlot314MenuParentRva": 0x12C2789,
    "kSlot314EmptyTextRva": 0x12C2C83,
    "kSlot314ItemFlags": 0x30,
    "kSlot314BoolVarFlags": 1,
    "kSlot314BoolCallbackFlags": 3,
    "kSlot314VarCount": 10,
    "kSlot314ItemCount": 10,
    "kSlot314AtexitRowCount": 22,
    "kSlot314FunctorSiteCount": 8,
    "kSlot314NodeCount": 12,
    "kSlot314BoolCallbackDispatches": 11,
    "kSlot314FunctorDispatches": 63,
}

# The native model's body order (each token must appear once, in this order).
SOURCE_ORDER = [
    "out.page = hooks.page_ctor(o.page, l2::literal_view(kSlot314PageName));",
    "init_slot314_page_stores(o.page);",
    "concmd.atexit_log.record(kSlot314PageDtorRva, &o.page)",
    "o.page.tail2e0 = 1;",
    "init_bool_var_static(var,",
    "if (!init_slot314_float_var_c300(var, spec))",
    "const Slot314FunctorSite& site = kSlot314FunctorSites[index - 2];",
    "out.var_results[index] = hooks.bool_callback_ctor(var, spec, concmd);",
    "const Slot314FunctorSite& site = kSlot314FunctorSites[3 + (index - 5)];",
    "if (!init_debug_menu_item_26ea0(item,",
    "register_debug_menu_item_268b0(item, menu, concmd.atexit_log)",
    "o.cell.word = 0;",
    "out.canary_checked = true;",
]

TEST_TOKENS = [
    "section_constants",
    "section_functor_and_target",
    "section_page",
    "section_native_build",
    "section_no_page_hook",
    "section_no_callback_hook",
    "section_foreign_spec_refused",
    "section_menu_registration_blocked",
    "section_parent_contract",
    "section_canary",
    "section_recording_hooks",
    "kSlot314AtexitRows.size() == 22",
    "r.atexit_records == 22 && r.functors_destroyed == 8",
    "r.var_results[5 + i].functor_dispatches == 11",
]

CMAKE_TOKENS = [
    "core/ctor_slot_314_a41810.cpp",
    "add_test(NAME ctor_slot_314 COMMAND test_ctor_slot_314)",
]

# Every direct `callq` of the body: (site, target). 84 sites over 9 distinct callees.
CALLS = [
    (0xA4184D, 0x475C0),
    (0xA41A1A, 0xF8F390),
    (0xA41A94, 0x20AF0),
    (0xA41AF6, 0xF590),
    (0xA41B13, 0x20D60),
    (0xA41B1B, 0xF590),
    (0xA41B31, 0xF8F390),
    (0xA41B97, 0x26EA0),
    (0xA41B9F, 0xF590),
    (0xA41BA7, 0xF590),
    (0xA41BBD, 0xF8F390),
    (0xA41C22, 0x20AF0),
    (0xA41C79, 0xF590),
    (0xA41C96, 0x20D60),
    (0xA41C9E, 0xF590),
    (0xA41CB7, 0xF8F390),
    (0xA41D19, 0x26EA0),
    (0xA41D21, 0xF590),
    (0xA41D29, 0xF590),
    (0xA41D3B, 0xF8F390),
    (0xA41DFA, 0xC300),
    (0xA41E06, 0xF590),
    (0xA41E3C, 0x20D60),
    (0xA41E44, 0xF590),
    (0xA41E60, 0xF8F390),
    (0xA41EC6, 0x26EA0),
    (0xA41ECE, 0xF590),
    (0xA41ED6, 0xF590),
    (0xA41EE8, 0xF8F390),
    (0xA41FA8, 0xC300),
    (0xA41FB4, 0xF590),
    (0xA41FE5, 0x20D60),
    (0xA41FED, 0xF590),
    (0xA42006, 0xF8F390),
    (0xA4206C, 0x26EA0),
    (0xA42074, 0xF590),
    (0xA4207C, 0xF590),
    (0xA4208E, 0xF8F390),
    (0xA42146, 0xC300),
    (0xA42152, 0xF590),
    (0xA42188, 0x20D60),
    (0xA42190, 0xF590),
    (0xA421A5, 0xF8F390),
    (0xA42207, 0x26EA0),
    (0xA4220F, 0xF590),
    (0xA42217, 0xF590),
    (0xA42229, 0xF8F390),
    (0xA42246, 0xF8F390),
    (0xA4231D, 0xA41570),
    (0xA42333, 0xF590),
    (0xA42348, 0xF8F390),
    (0xA423AA, 0x26EA0),
    (0xA423B2, 0xF590),
    (0xA423BA, 0xF590),
    (0xA423CF, 0xF8F390),
    (0xA42462, 0xA41570),
    (0xA42478, 0xF590),
    (0xA4248A, 0xF8F390),
    (0xA424EC, 0x26EA0),
    (0xA424F4, 0xF590),
    (0xA424FC, 0xF590),
    (0xA4250E, 0xF8F390),
    (0xA425A1, 0xA41570),
    (0xA425B7, 0xF590),
    (0xA425C9, 0xF8F390),
    (0xA4262B, 0x26EA0),
    (0xA42633, 0xF590),
    (0xA4263B, 0xF590),
    (0xA4264D, 0xF8F390),
    (0xA426E0, 0xA41570),
    (0xA426F6, 0xF590),
    (0xA42708, 0xF8F390),
    (0xA4276A, 0x26EA0),
    (0xA42772, 0xF590),
    (0xA4277A, 0xF590),
    (0xA4278C, 0xF8F390),
    (0xA4281F, 0xA41570),
    (0xA42835, 0xF590),
    (0xA42847, 0xF8F390),
    (0xA428A9, 0x26EA0),
    (0xA428B1, 0xF590),
    (0xA428B9, 0xF590),
    (0xA428CB, 0xF8F390),
    (0xA428F3, 0xF8F3A0),
]

# Every rip-relative operand of the body: (site, displacement offset, target, encoding). 196 operands.
# The full encoding is pinned so that a mutation anywhere in the instruction is rejected; the
# displacement offset is not always `length-4` (`mov [rip+d], imm32` carries the immediate after it).
RIPS = [
    (0xA41828, 3, 0x15FE518, "488b05e9ccbb00"),
    (0xA4182F, 3, 0x32F5AF0, "488d1dba428b02"),
    (0xA41836, 3, 0x12C2760, "488d35230f8800"),
    (0xA41852, 3, 0x1594C90, "488d053734b500"),
    (0xA4185D, 3, 0x32F5BA0, "488d153c438b02"),
    (0xA41868, 3, 0x32F5CF0, "488d0d81448b02"),
    (0xA4186F, 3, 0x1CE70, "488d3dfab55dff"),
    (0xA4187D, 3, 0x32F5AF0, "4889056c428b02"),
    (0xA41884, 3, 0x32F5B70, "48c705e1428b0200000000"),
    (0xA4188F, 2, 0x32F5B78, "c705df428b0200000000"),
    (0xA41899, 4, 0x32F5B80, "c5fc1105df428b02"),
    (0xA418A6, 3, 0x32F5BD0, "488d0523438b02"),
    (0xA418AD, 3, 0x1604218, "488d156429bc00"),
    (0xA418B9, 4, 0x32F5BA0, "c5fa7f05df428b02"),
    (0xA418C6, 3, 0x32F5C00, "488d0533438b02"),
    (0xA418CD, 4, 0x32F5BC0, "c5f8110deb428b02"),
    (0xA418DA, 4, 0x32F5BD0, "c5fa7f05ee428b02"),
    (0xA418E7, 3, 0x32F5C30, "488d0542438b02"),
    (0xA418EE, 4, 0x32F5BF0, "c5f8110dfa428b02"),
    (0xA418FB, 4, 0x32F5C00, "c5fa7f05fd428b02"),
    (0xA41908, 3, 0x32F5C60, "488d0551438b02"),
    (0xA4190F, 4, 0x32F5C20, "c5f8110d09438b02"),
    (0xA4191C, 4, 0x32F5C30, "c5fa7f050c438b02"),
    (0xA41929, 3, 0x32F5C90, "488d0560438b02"),
    (0xA41930, 4, 0x32F5C50, "c5f8110d18438b02"),
    (0xA4193D, 4, 0x32F5C60, "c5fa7f051b438b02"),
    (0xA4194A, 4, 0x32F5C80, "c5f8110d2e438b02"),
    (0xA41952, 3, 0x32F5CC0, "488d0567438b02"),
    (0xA4195E, 4, 0x32F5C90, "c5fa7f052a438b02"),
    (0xA4196B, 4, 0x32F5CB0, "c5f8110d3d438b02"),
    (0xA41973, 3, 0x32F5CC8, "4889054e438b02"),
    (0xA4197A, 3, 0x32F5CC0, "4889053f438b02"),
    (0xA41981, 3, 0x32F5D20, "488d0598438b02"),
    (0xA41988, 4, 0x32F5CE0, "c5f8110d50438b02"),
    (0xA41995, 4, 0x32F5CF0, "c5fa7f0553438b02"),
    (0xA419A2, 3, 0x32F5D50, "488d05a7438b02"),
    (0xA419A9, 4, 0x32F5D10, "c5f8110d5f438b02"),
    (0xA419B6, 4, 0x32F5D20, "c5fa7f0562438b02"),
    (0xA419C3, 3, 0x32F5D80, "488d05b6438b02"),
    (0xA419CA, 4, 0x32F5D40, "c5f8110d6e438b02"),
    (0xA419D7, 4, 0x32F5D50, "c5fa7f0571438b02"),
    (0xA419E4, 3, 0x32F5DB0, "488d05c5438b02"),
    (0xA419EB, 4, 0x32F5D70, "c5f8110d7d438b02"),
    (0xA419F8, 4, 0x32F5D80, "c5fa7f0580438b02"),
    (0xA41A05, 4, 0x32F5DA0, "c5f8110d93438b02"),
    (0xA41A12, 4, 0x32F5DB0, "c5fa7f0596438b02"),
    (0xA41A1F, 3, 0x1594208, "488d1de227b500"),
    (0xA41A26, 3, 0x12C2774, "488d05470d8800"),
    (0xA41A2D, 3, 0x32F5DD8, "4c8d2da4438b02"),
    (0xA41A47, 2, 0x32F5DD0, "c7057f438b0201000000"),
    (0xA41A56, 3, 0x12C2C83, "488d0526128800"),
    (0xA41A8D, 3, 0x32F5DD8, "48891d44438b02"),
    (0xA41A99, 3, 0x15940A0, "4c8d3d0026b500"),
    (0xA41AA3, 3, 0x32F5E98, "488d05ee438b02"),
    (0xA41AAE, 3, 0x15D1750, "488d0d9bfcb800"),
    (0xA41AB9, 3, 0x32F5DD8, "4c893d18438b02"),
    (0xA41AC0, 3, 0x32F5E38, "48890571438b02"),
    (0xA41AC7, 2, 0x32F5E41, "c70570438b0200000000"),
    (0xA41AD1, 4, 0x32F5E48, "c5fc11056f438b02"),
    (0xA41AD9, 3, 0x32F5E78, "48c70594438b0200000000"),
    (0xA41AE4, 3, 0x32F5E88, "48890d9d438b02"),
    (0xA41AEB, 3, 0x32F5E90, "48c7059a438b0200000000"),
    (0xA41AFE, 3, 0x32F5DD8, "48891dd3428b02"),
    (0xA41B05, 2, 0x32F5E98, "c6058c438b0201"),
    (0xA41B0C, 2, 0x32F5E40, "c6052d438b0201"),
    (0xA41B20, 3, 0x20B0, "488d3d89055cff"),
    (0xA41B27, 3, 0x1604218, "488d15ea26bc00"),
    (0xA41B36, 3, 0x12C2791, "488d0d540c8800"),
    (0xA41B3D, 3, 0x32F5EA0, "4c8d355c438b02"),
    (0xA41B4E, 3, 0x12C2789, "488d05340c8800"),
    (0xA41B6D, 3, 0x32F5DD8, "488d0d64428b02"),
    (0xA41BAC, 3, 0x272D0, "488d3d1d575eff"),
    (0xA41BB3, 3, 0x1604218, "488d155e26bc00"),
    (0xA41BC2, 3, 0x32F5F48, "4c8d357f438b02"),
    (0xA41BC9, 3, 0x12C2C83, "488d15b3108800"),
    (0xA41BDA, 3, 0x12C27A7, "488d05c60b8800"),
    (0xA41BEA, 3, 0x32F5F48, "48891d57438b02"),
    (0xA41C2A, 3, 0x32F6008, "488d05d7438b02"),
    (0xA41C35, 3, 0x15D1750, "488d0d14fbb800"),
    (0xA41C3C, 3, 0x32F5F48, "4c893d05438b02"),
    (0xA41C43, 3, 0x32F5FA8, "4889055e438b02"),
    (0xA41C4A, 2, 0x32F5FB1, "c7055d438b0200000000"),
    (0xA41C54, 4, 0x32F5FB8, "c5fc11055c438b02"),
    (0xA41C5C, 3, 0x32F5FE8, "48c70581438b0200000000"),
    (0xA41C67, 3, 0x32F5FF8, "48890d8a438b02"),
    (0xA41C6E, 3, 0x32F6000, "48c70587438b0200000000"),
    (0xA41C81, 3, 0x32F5F48, "48891dc0428b02"),
    (0xA41C88, 2, 0x32F6008, "c60579438b0200"),
    (0xA41C8F, 2, 0x32F5FB0, "c6051a438b0200"),
    (0xA41CA3, 3, 0x1604218, "488d1d6e25bc00"),
    (0xA41CAA, 3, 0x20B0, "488d3dff035cff"),
    (0xA41CBC, 3, 0x12C27C4, "488d0d010b8800"),
    (0xA41CC3, 3, 0x32F6010, "4c8d2d46438b02"),
    (0xA41CD4, 3, 0x12C2789, "488d05ae0a8800"),
    (0xA41D2E, 3, 0x272D0, "488d3d9b555eff"),
    (0xA41D40, 3, 0x15942A0, "4c8d2d5925b500"),
    (0xA41D47, 3, 0x12C27DC, "488d058e0a8800"),
    (0xA41D52, 3, 0x32F60B8, "488d1d5f438b02"),
    (0xA41D7E, 3, 0x32F6188, "488d1503448b02"),
    (0xA41D9A, 3, 0x15D1770, "488d05cff9b800"),
    (0xA41DC4, 3, 0x12C2C83, "488d05b80e8800"),
    (0xA41DEF, 3, 0x32F60B8, "4c892dc2428b02"),
    (0xA41E21, 3, 0x32F60B8, "4c892d90428b02"),
    (0xA41E28, 2, 0x32F6188, "c70556438b020000803f"),
    (0xA41E32, 2, 0x32F6120, "c705e4428b020000803f"),
    (0xA41E4C, 3, 0x1604218, "488d1dc523bc00"),
    (0xA41E53, 3, 0x20F0, "488d3d96025cff"),
    (0xA41E65, 3, 0x12C27FB, "488d0d8f098800"),
    (0xA41E6C, 3, 0x32F6190, "4c8d251d438b02"),
    (0xA41E7D, 3, 0x12C2789, "488d0505098800"),
    (0xA41E9C, 3, 0x32F60B8, "488d0d15428b02"),
    (0xA41EDB, 3, 0x272D0, "488d3dee535eff"),
    (0xA41EED, 3, 0x12C2811, "488d051d098800"),
    (0xA41EF4, 3, 0x32F6238, "488d1d3d438b02"),
    (0xA41F08, 3, 0x15D1770, "488d0d61f8b800"),
    (0xA41F2B, 3, 0x32F6238, "4c892d06438b02"),
    (0xA41F3A, 3, 0x32F6308, "488d15c7438b02"),
    (0xA41F51, 3, 0x12C2C83, "488d052b0d8800"),
    (0xA41FCA, 3, 0x32F6238, "4c892d67428b02"),
    (0xA41FD1, 2, 0x32F6308, "c7052d438b02000080bf"),
    (0xA41FDB, 2, 0x32F62A0, "c705bb428b02000080bf"),
    (0xA41FF2, 3, 0x1604218, "4c8d3d1f22bc00"),
    (0xA41FF9, 3, 0x20F0, "488d3df0005cff"),
    (0xA4200B, 3, 0x12C2828, "488d0d16088800"),
    (0xA42012, 3, 0x32F6310, "4c8d35f7428b02"),
    (0xA42023, 3, 0x12C2789, "488d055f078800"),
    (0xA42042, 3, 0x32F6238, "488d0def418b02"),
    (0xA42081, 3, 0x272D0, "488d3d48525eff"),
    (0xA42093, 3, 0x12C2838, "488d059e078800"),
    (0xA4209A, 3, 0x32F63B8, "4c8d3517438b02"),
    (0xA420AE, 3, 0x15D1770, "488d0dbbf6b800"),
    (0xA420D1, 3, 0x32F63B8, "4c892de0428b02"),
    (0xA420D8, 3, 0x32F6488, "488d15a9438b02"),
    (0xA420EF, 3, 0x12C2C83, "488d058d0b8800"),
    (0xA4216D, 3, 0x32F63B8, "4c892d44428b02"),
    (0xA42174, 2, 0x32F6488, "c7050a438b020000803f"),
    (0xA4217E, 2, 0x32F6420, "c70598428b020000803f"),
    (0xA42195, 3, 0x20F0, "488d3d54ff5bff"),
    (0xA421AA, 3, 0x12C2850, "488d0d9f068800"),
    (0xA421B1, 3, 0x32F6490, "4c8d25d8428b02"),
    (0xA421C2, 3, 0x12C2789, "488d05c0058800"),
    (0xA4221C, 3, 0x272D0, "488d3dad505eff"),
    (0xA4222E, 3, 0x32F6538, "488d3503438b02"),
    (0xA42235, 3, 0xA3D9E0, "488d3da4b7ffff"),
    (0xA4224B, 3, 0x15D1400, "488d35aef1b800"),
    (0xA42252, 3, 0x32F6560, "488d0d07438b02"),
    (0xA42259, 3, 0x15D1430, "488d15d0f1b800"),
    (0xA42260, 3, 0x32F6540, "488d05d9428b02"),
    (0xA42267, 4, 0x15FEC90, "c5fa7e0d21cabb00"),
    (0xA42273, 3, 0x32F65E8, "4c8d356e438b02"),
    (0xA422AA, 3, 0xA40160, "488d05afdeffff"),
    (0xA422BF, 3, 0x32F6568, "488d0da2428b02"),
    (0xA422C6, 3, 0x12C2862, "488d3595058800"),
    (0xA422F8, 3, 0x15D1750, "488d1551f4b800"),
    (0xA42338, 3, 0x20B0, "488d0571fd5bff"),
    (0xA4234D, 3, 0x12C2881, "488d0d2d058800"),
    (0xA42354, 3, 0x32F66B0, "4c8d3d55438b02"),
    (0xA42365, 3, 0x12C2789, "488d051d048800"),
    (0xA423BF, 3, 0x272D0, "488d050a4f5eff"),
    (0xA423D9, 3, 0x12C289D, "488d05bd048800"),
    (0xA423E0, 3, 0x32F6758, "4c8d3571438b02"),
    (0xA423EC, 3, 0x15D1750, "488d155df3b800"),
    (0xA42416, 3, 0xA40230, "488d0513deffff"),
    (0xA4247D, 3, 0x20B0, "488d3d2cfc5bff"),
    (0xA4248F, 3, 0x12C28B3, "488d0d1d048800"),
    (0xA42496, 3, 0x32F6820, "4c8d3d83438b02"),
    (0xA424A7, 3, 0x12C2789, "488d05db028800"),
    (0xA42501, 3, 0x272D0, "488d3dc84d5eff"),
    (0xA42518, 3, 0x12C28C5, "488d05a6038800"),
    (0xA4251F, 3, 0x32F68C8, "4c8d35a2438b02"),
    (0xA4252B, 3, 0x15D1750, "488d151ef2b800"),
    (0xA42555, 3, 0xA40300, "488d05a4ddffff"),
    (0xA425BC, 3, 0x20B0, "488d3dedfa5bff"),
    (0xA425CE, 3, 0x12C28DE, "488d0d09038800"),
    (0xA425D5, 3, 0x32F6990, "4c8d3db4438b02"),
    (0xA425E6, 3, 0x12C2789, "488d059c018800"),
    (0xA42640, 3, 0x272D0, "488d3d894c5eff"),
    (0xA42657, 3, 0x12C28F3, "488d0595028800"),
    (0xA4265E, 3, 0x32F6A38, "4c8d35d3438b02"),
    (0xA4266A, 3, 0x15D1750, "488d15dff0b800"),
    (0xA42694, 3, 0xA403C0, "488d0525ddffff"),
    (0xA426FB, 3, 0x20B0, "488d3daef95bff"),
    (0xA4270D, 3, 0x12C290E, "488d0dfa018800"),
    (0xA42714, 3, 0x32F6B00, "4c8d3de5438b02"),
    (0xA42725, 3, 0x12C2789, "488d055d008800"),
    (0xA4277F, 3, 0x272D0, "488d3d4a4b5eff"),
    (0xA42796, 3, 0x12C2926, "488d0589018800"),
    (0xA4279D, 3, 0x32F6BA8, "4c8d3504448b02"),
    (0xA427A9, 3, 0x15D1750, "488d15a0efb800"),
    (0xA427D3, 3, 0xA40480, "488d05a6dcffff"),
    (0xA4283A, 3, 0x20B0, "488d3d6ff85bff"),
    (0xA4284C, 3, 0x12C2947, "488d0df4008800"),
    (0xA42853, 3, 0x32F6C70, "4c8d3d16448b02"),
    (0xA42864, 3, 0x12C2789, "488d051eff8700"),
    (0xA428BE, 3, 0x272D0, "488d3d0b4a5eff"),
    (0xA428D0, 3, 0x15FE518, "488b0541bcbb00"),
]

# The 63 rip-relative STORE sites of the body (a subset of RIPS; 56 distinct target addresses).
STORE_SITES = [
    0xA4187D, 0xA41884, 0xA4188F, 0xA41899, 0xA418B9, 0xA418CD,
    0xA418DA, 0xA418EE, 0xA418FB, 0xA4190F, 0xA4191C, 0xA41930,
    0xA4193D, 0xA4194A, 0xA4195E, 0xA4196B, 0xA41973, 0xA4197A,
    0xA41988, 0xA41995, 0xA419A9, 0xA419B6, 0xA419CA, 0xA419D7,
    0xA419EB, 0xA419F8, 0xA41A05, 0xA41A12, 0xA41A47, 0xA41A8D,
    0xA41AB9, 0xA41AC0, 0xA41AC7, 0xA41AD1, 0xA41AD9, 0xA41AE4,
    0xA41AEB, 0xA41AFE, 0xA41B05, 0xA41B0C, 0xA41BEA, 0xA41C3C,
    0xA41C43, 0xA41C4A, 0xA41C54, 0xA41C5C, 0xA41C67, 0xA41C6E,
    0xA41C81, 0xA41C88, 0xA41C8F, 0xA41DEF, 0xA41E21, 0xA41E28,
    0xA41E32, 0xA41F2B, 0xA41FCA, 0xA41FD1, 0xA41FDB, 0xA420D1,
    0xA4216D, 0xA42174, 0xA4217E,
]

# The 8 row-local `callq *0x18(%rax)` sites: (site, argument-functor tag, slot-3 body).
INDIRECT = [
    (0xA41E1B, 0x15D1770, 0xC4F0),
    (0xA41FC4, 0x15D1770, 0xC4F0),
    (0xA42167, 0x15D1770, 0xC4F0),
    (0xA4232D, 0x15D1750, 0xC2F0),
    (0xA42472, 0x15D1750, 0xC2F0),
    (0xA425B1, 0x15D1750, 0xC2F0),
    (0xA426F0, 0x15D1750, 0xC2F0),
    (0xA4282F, 0x15D1750, 0xC2F0),
]

# The 22 atexit registrations, in body order: (site, dtor, object).
ATEXIT = [
    (0xA41A1A, 0x1CE70, 0x32F5AF0),
    (0xA41B31, 0x20B0, 0x32F5DD8),
    (0xA41BBD, 0x272D0, 0x32F5EA0),
    (0xA41CB7, 0x20B0, 0x32F5F48),
    (0xA41D3B, 0x272D0, 0x32F6010),
    (0xA41E60, 0x20F0, 0x32F60B8),
    (0xA41EE8, 0x272D0, 0x32F6190),
    (0xA42006, 0x20F0, 0x32F6238),
    (0xA4208E, 0x272D0, 0x32F6310),
    (0xA421A5, 0x20F0, 0x32F63B8),
    (0xA42229, 0x272D0, 0x32F6490),
    (0xA42246, 0xA3D9E0, 0x32F6538),
    (0xA42348, 0x20B0, 0x32F65E8),
    (0xA423CF, 0x272D0, 0x32F66B0),
    (0xA4248A, 0x20B0, 0x32F6758),
    (0xA4250E, 0x272D0, 0x32F6820),
    (0xA425C9, 0x20B0, 0x32F68C8),
    (0xA4264D, 0x272D0, 0x32F6990),
    (0xA42708, 0x20B0, 0x32F6A38),
    (0xA4278C, 0x272D0, 0x32F6B00),
    (0xA42847, 0x20B0, 0x32F6BA8),
    (0xA428CB, 0x272D0, 0x32F6C70),
]

# The original object map: (kind, object RVA, stride to the next object).
OBJECTS = [
    ("page", 0x32F5AF0, 0x2E8),
    ("var0", 0x32F5DD8, 0xC8), ("item0", 0x32F5EA0, 0xA8),
    ("var1", 0x32F5F48, 0xC8), ("item1", 0x32F6010, 0xA8),
    ("var2", 0x32F60B8, 0xD8), ("item2", 0x32F6190, 0xA8),
    ("var3", 0x32F6238, 0xD8), ("item3", 0x32F6310, 0xA8),
    ("var4", 0x32F63B8, 0xD8), ("item4", 0x32F6490, 0xA8),
    ("cell", 0x32F6538, 0xB0),
    ("var5", 0x32F65E8, 0xC8), ("item5", 0x32F66B0, 0xA8),
    ("var6", 0x32F6758, 0xC8), ("item6", 0x32F6820, 0xA8),
    ("var7", 0x32F68C8, 0xC8), ("item7", 0x32F6990, 0xA8),
    ("var8", 0x32F6A38, 0xC8), ("item8", 0x32F6B00, 0xA8),
    ("var9", 0x32F6BA8, 0xC8), ("item9", 0x32F6C70, 0xA8),
]

# The ten console variables: (name RVA, name length, object RVA).
VARS = [
    (0x12C2774, 0x14, 0x32F5DD8),
    (0x12C27A7, 0x1C, 0x32F5F48),
    (0x12C27DC, 0x1E, 0x32F60B8),
    (0x12C2811, 0x16, 0x32F6238),
    (0x12C2838, 0x17, 0x32F63B8),
    (0x12C2862, 0x1E, 0x32F65E8),
    (0x12C289D, 0x15, 0x32F6758),
    (0x12C28C5, 0x18, 0x32F68C8),
    (0x12C28F3, 0x1A, 0x32F6A38),
    (0x12C2926, 0x20, 0x32F6BA8),
]

# The ten toggles: (caption RVA, caption length, object RVA).
ITEMS = [
    (0x12C2791, 0x15, 0x32F5EA0),
    (0x12C27C4, 0x17, 0x32F6010),
    (0x12C27FB, 0x15, 0x32F6190),
    (0x12C2828, 0x0F, 0x32F6310),
    (0x12C2850, 0x11, 0x32F6490),
    (0x12C2881, 0x1B, 0x32F66B0),
    (0x12C28B3, 0x11, 0x32F6820),
    (0x12C28DE, 0x14, 0x32F6990),
    (0x12C290E, 0x17, 0x32F6B00),
    (0x12C2947, 0x1E, 0x32F6C70),
]

# The two argument-functor tables and their four lifted bodies (address -> FDE length, head bytes).
FN_TABLES = [
    (0x15D1750, (0xC2A0, 0xC2C0, 0xC2E0, 0xC2F0)),
    (0x15D1770, (0xC4A0, 0xC4C0, 0xC4E0, 0xC4F0)),
]
FUNCTOR_BODIES = {
    0xC2A0: (20, "488b4610"), 0xC2C0: (20, "488d05"), 0xC2E0: (9, "488b4610"), 0xC2F0: (1, "c3"),
    0xC4A0: (20, "488b4610"), 0xC4C0: (20, "488d05"), 0xC4E0: (9, "488b4610"), 0xC4F0: (1, "c3"),
}
# The 0x26ea0 gate answers: (slot RVA, expected bytes).
GATES = [
    (0xB890, "b801000000c3"),   # slot 3 of the bool class -> 1
    (0xB930, "31c0c3"),         # slot 6 of the bool class -> 0  (xor eax,eax; ret)
    (0xBBA0, "b801000000c3"),   # slot 3 of the float class -> 1
    (0xBBF0, "b803000000c3"),   # slot 6 of the float class -> 3
]

# 0xa41570: the eleven functor dispatches, the two calls, the vptr stores and the canary tail.
BOOL_CB_DISPATCH = [
    (0xA4162F, 1), (0xA41640, 0), (0xA41656, 0), (0xA41662, 0), (0xA41681, 3),
    (0xA41754, 1), (0xA41765, 0), (0xA4177B, 0), (0xA41787, 0), (0xA417A6, 3), (0xA417C3, 3),
]
BOOL_CB_BYTES = [
    (0xA415C2, "4c893b", "0xa41570 [this] = the final vptr before the base constructor"),
    (0xA416D7, "488903", "0xa41570 [this] = the mid vptr"),
    (0xA416E1, "48894360", "0xa41570 [obj+0x60] = &this->value"),
    (0xA416EC, "48898bb0000000", "0xa41570 [obj+0xb0] = the argument functor tag"),
    (0xA416F3, "48c783a000000000000000", "0xa41570 [obj+0xa0] = 0"),
    (0xA416FE, "48c783b800000000000000", "0xa41570 [obj+0xb8] = 0"),
    (0xA41711, "c5fc114370", "0xa41570 ymm [obj+0x70] = the argument block"),
    (0xA4172B, "488900", "0xa41570 [obj+0x70] = obj+0x70 (self pointer)"),
    (0xA417C6, "4c893b", "0xa41570 [this] = the final vptr"),
    (0xA417D5, "8883c0000000", "0xa41570 byte [obj+0xc0] = *value cell"),
    (0xA417DD, "884368", "0xa41570 byte [obj+0x68] = *value cell (second load)"),
    (0xA417F3, "7512", "0xa41570 canary branch to __stack_chk_fail"),
]

# The page constructor 0x475c0: its vptr source, field stores and size.
PAGE_CTOR_BYTES = [
    (0x475C0, "488d0529e35401", "lea rax,[0x15958f0]"),
    (0x475CB, "4883c010", "add rax,0x10"),
    (0x475CF, "488907", "[rdi] = the vptr"),
    (0x475D6, "c6470800", "byte [rdi+8] = 0"),
    (0x475DA, "c6470901", "byte [rdi+9] = 1"),
    (0x475DE, "48897710", "[rdi+0x10] = the name"),
    (0x475E2, "895718", "dword [rdi+0x18] = the flags word"),
    (0x475E5, "c5fc1147 1c".replace(" ", ""), "ymm [rdi+0x1c] = 0"),
    (0x475EA, "c7473c00000000", "dword [rdi+0x3c] = 0"),
    (0x475F1, "48894740", "[rdi+0x40] = rdi+0x38"),
    (0x475F5, "48894748", "[rdi+0x48] = rdi+0x38"),
    (0x475F9, "48c74750000000 00".replace(" ", ""), "[rdi+0x50] = 0"),
    (0x47601, "c7475800000000", "dword [rdi+0x58] = 0"),
    (0x4760A, "48874760", "xchg [rdi+0x60], rax (rax = 0)"),
    (0x4760E, "48c7477000000000", "[rdi+0x70] = 0"),
]

# Literal instruction bytes of the body that no call/rip pin covers.
BYTES_PINS = [
    (0xA41810, "554889e5", "push rbp; mov rsp,rbp"),
    (0xA41814, "415741564155415453", "push r15/r14/r13/r12/rbx"),
    (0xA4181D, "4883e4e0", "and rsp,-0x20"),
    (0xA41821, "4881ece0000000", "sub rsp,0xe0"),
    (0xA41845, "48898424c0000000", "the guard lands at [rsp+0xc0]"),
    (0xA41A47, "c7057f438b0201000000", "dword [page+0x2e0] = 1"),
    (0xA41AC7, "c70570438b0200000000", "dword [obj0+0x69] = 0"),
    (0xA41B05, "c6058c438b0201", "byte [obj0+0xc0] = 1"),
    (0xA41B0C, "c6052d438b0201", "byte [obj0+0x68] = 1"),
    (0xA41C88, "c60579438b0200", "byte [obj1+0xc0] = 0"),
    (0xA41C8F, "c6051a438b0200", "byte [obj1+0x68] = 0"),
    (0xA41D6E, "c744241c00000000", "float var 2 min cell = 0"),
    (0xA41D76, "c74424180000803f", "float var 2 max cell = 1.0f"),
    (0xA41F1B, "c744241c000080bf", "float var 3 min cell = -1.0f"),
    (0xA41F23, "c74424180000803f", "float var 3 max cell = 1.0f"),
    (0xA420C1, "c744241c00000000", "float var 4 min cell = 0"),
    (0xA420C9, "c74424180000c842", "float var 4 max cell = 100.0f"),
    (0xA41E28, "c70556438b020000803f", "float var 2 value = 1.0f"),
    (0xA41E32, "c705e4428b020000803f", "float var 2 default = 1.0f"),
    (0xA41FD1, "c7052d438b02000080bf", "float var 3 value = -1.0f"),
    (0xA41FDB, "c705bb428b02000080bf", "float var 3 default = -1.0f"),
    (0xA42174, "c7050a438b020000803f", "float var 4 value = 1.0f"),
    (0xA4217E, "c70598428b020000803f", "float var 4 default = 1.0f"),
    (0xA4197A, "4889053f438b02", "node 6 [next] = &node"),
    (0xA41973, "4889054e438b02", "node 6 [prev] = &node"),
    (0xA4223F, "48c70600000000", "the zero cell [cell] = 0"),
    (0xA422FF, "b903000000", "the callback bool flags = 3"),
    (0xA428DA, "483b8424c0000000", "cmp the frame canary"),
    (0xA428E2, "750f", "the body's only branch: jne rel8"),
    (0xA428E4, "488d65d8", "lea -0x28(%rbp),%rsp"),
    (0xA428E8, "5b415c415d415e415f5dc3", "pops + retq"),
    (0xA428F3, "e8a8ca5400", "callq __stack_chk_fail"),
    (0xA428F8, "90", "the FDE's trailing nop"),
]

# Strings of the block: (address, text, pinned length).
STRINGS = [
    (0x12C2760, b"PhysXToRenderThread", 0x13),
    (0x12C2774, b"physics_EnableUpdate", 0x14),
    (0x12C2789, b"Physics", 7),
    (0x12C2791, b"Enable Physics Update", 0x15),
    (0x12C27A7, b"physics_LockSimulationAt30Hz", 0x1C),
    (0x12C27C4, b"Lock Simulation at 30Hz", 0x17),
    (0x12C27DC, b"physics_clothMaxDistanceScalar", 0x1E),
    (0x12C27FB, b"Cloth Max Dist Scalar", 0x15),
    (0x12C2811, b"physics_clothDampening", 0x16),
    (0x12C2828, b"Cloth Dampening", 0x0F),
    (0x12C2838, b"physics_clothWindScalar", 0x17),
    (0x12C2850, b"Cloth Wind Scalar", 0x11),
    (0x12C2862, b"physX_connectToPhysicsDebugger", 0x1E),
    (0x12C2881, b"Connect To Physics Debugger", 0x1B),
    (0x12C289D, b"physX_enableDebugDraw", 0x15),
    (0x12C28B3, b"Enable Debug Draw", 0x11),
    (0x12C28C5, b"physX_drawClothCollision", 0x18),
    (0x12C28DE, b"Draw Cloth Collision", 0x14),
    (0x12C28F3, b"physX_drawClothMaxDistance", 0x1A),
    (0x12C290E, b"Draw Cloth Max Distance", 0x17),
    (0x12C2926, b"physX_drawClothMaxDistanceInward", 0x20),
    (0x12C2947, b"Draw Cloth Max Distance Inward", 0x1E),
    (0x12C2C83, b"", 0),
]


def fde_range(e: Elf, initial: int) -> int:
    """address_range of the FDE whose initial_location is `initial` (PT_GNU_EH_FRAME datarel table)."""
    header = e.raw(EH_FRAME_HDR, 12)
    require(header[0] == 1 and header[1] == 0x1B and header[2] == 0x03 and header[3] == 0x3B,
            "eh_frame_hdr version/encodings")
    count = struct.unpack("<I", header[8:12])[0]
    table = e.img.read(EH_FRAME_HDR + 12, count * 8)
    for i in range(count):
        init, fde = struct.unpack_from("<ii", table, 8 * i)
        if init + EH_FRAME_HDR != initial:
            continue
        fde += EH_FRAME_HDR
        record = e.raw(fde, 16)
        require(struct.unpack("<I", record[0:4])[0] >= 16, "FDE record length")
        location = fde + 8 + struct.unpack("<i", record[8:12])[0]
        require(location == initial, f"FDE initial_location 0x{location:x}")
        return struct.unpack("<I", record[12:16])[0]
    raise SystemExit(f"FAIL no FDE for 0x{initial:x} in eh_frame_hdr")


def branch(e: Elf, site: int, size: int, target: int, opcode: int) -> None:
    """Decodes a rel8 branch whose target must be `target`."""
    raw = e.read(site, size)
    require(raw[0] == opcode, f"branch opcode at 0x{site:x}")
    require(site + size + struct.unpack("<b", raw[-1:])[0] == target, f"branch target from 0x{site:x}")


def check_elf(e: Elf) -> None:
    # --- the startup-table row ---------------------------------------------------------------
    require(TABLE_TOP - 8 * TABLE_INDEX == TABLE_WORD == 0x2DD6710, "table row arithmetic")
    require(e.u64(TABLE_WORD) == RVA, "constructor-table entry 314 -> 0xa41810")

    # --- body extent, digest, padding ---------------------------------------------------------
    body = e.read(RVA, SIZE)
    if not e.skip_hash:
        require(digest(body) == SHA, "body sha256 (4329 B FDE)")
        require(digest(body[:REACHABLE]) == SHA_REACHABLE, "reachable body sha256 (4328 B)")
    require(body[0] == 0x55 and body[1:4] == b"\x48\x89\xe5", "push rbp; mov rsp,rbp")
    require(body[-1] == 0x90, "the FDE's last byte is the trailing nop")
    require(e.read(RVA + SIZE, 4) == b"\x90\x90\x90\x90", "nop padding after the body")
    require(fde_range(e, RVA) == SIZE, "FDE address_range == 4329")

    # --- every direct call, every rip operand, the literal bytes -------------------------------
    require(len(CALLS) == 84 and len({site for site, _ in CALLS}) == 84, "84 distinct call sites")
    require(len({target for _, target in CALLS}) == 9, "9 distinct callees")
    for site, target in CALLS:
        require(e.call(site) == target, f"call at 0x{site:x} -> 0x{target:x}")
    require(len(RIPS) == 196 and len({row[0] for row in RIPS}) == 196, "196 distinct rip sites")
    require(len({row[2] for row in RIPS}) == 119, "119 distinct rip targets")
    require(len(STORE_SITES) == 63 and len(set(STORE_SITES)) == 63, "63 distinct store sites")
    rip_targets = {row[0]: row[2] for row in RIPS}
    require(all(site in rip_targets for site in STORE_SITES), "every store site is a pinned rip operand")
    require(len({rip_targets[site] for site in STORE_SITES}) == 56, "56 distinct store targets")
    for site, off, target, encoding in RIPS:
        raw = bytes.fromhex(encoding)
        require(e.read(site, len(raw)) == raw, f"rip operand encoding at 0x{site:x}")
        require(site + len(raw) + struct.unpack("<i", raw[off:off + 4])[0] == target,
                f"rip operand at 0x{site:x} -> 0x{target:x}")
    for site, want, what in BYTES_PINS:
        e.bytes_at(site, want, what or f"bytes at 0x{site:x}")

    # --- the single branch and the canary ------------------------------------------------------
    require(e.rip(0xA41828, 7, 3) == 0x15FE518, "the guard is read through the GOT cell 0x15fe518")
    require(0x15FE518 in e.relocs, "0x15fe518 is a relocation slot (the guard pointer)")
    branch(e, 0xA428E2, 2, 0xA428F3, 0x75)
    require(e.call(0xA428F3) == 0xF8F3A0, "the branch target calls __stack_chk_fail")

    # --- the eight row-local indirect sites ----------------------------------------------------
    for site, tag, slot3 in INDIRECT:
        require(e.read(site, 3) == b"\xff\x50\x18", f"0x{site:x} is `callq *0x18(%rax)`")
        require(e.reloc_target(tag + 0x18) == slot3, f"tag 0x{tag:x} slot 3 -> 0x{slot3:x}")
        e.bytes_at(slot3, "c3", f"functor destroy 0x{slot3:x} is a bare ret")

    # --- the two argument-functor tables and their four bodies ----------------------------------
    for table, slots in FN_TABLES:
        for index, body_rva in enumerate(slots):
            require(e.reloc_target(table + 8 * index) == body_rva,
                    f"functor table 0x{table:x} slot {index} -> 0x{body_rva:x}")
    for addr, (size, head) in FUNCTOR_BODIES.items():
        require(fde_range(e, addr) == size, f"functor body 0x{addr:x} FDE == {size}")
        e.bytes_at(addr, head, f"functor body 0x{addr:x} head")

    # --- the gate bodies -----------------------------------------------------------------------
    for addr, want in GATES:
        e.bytes_at(addr, want, f"gate slot 0x{addr:x}")

    # --- the 22 __cxa_atexit registrations, in body order ---------------------------------------
    atexit_sites = [site for site, target in CALLS if target == 0xF8F390]
    require(atexit_sites == [site for site, _, _ in ATEXIT], "the 22 atexit sites in body order")
    rip_by_site = {site: target for site, _, target, _ in RIPS}
    first_load = {}
    for site in sorted(rip_by_site):
        first_load.setdefault(rip_by_site[site], site)
    known_dtors = {dtor for _, dtor, _ in ATEXIT}
    previous = -1
    for site, dtor, obj in ATEXIT:
        where = first_load.get(obj)
        require(where is not None, f"object 0x{obj:x} is loaded by a rip operand")
        require(previous < where <= site, f"object 0x{obj:x} is loaded inside its own step window")
        previous = site
        dtor_sites = [s for s, t in rip_by_site.items() if t in known_dtors and s <= site]
        require(dtor_sites, f"step at 0x{site:x} loads a destructor constant before the call")
        require(rip_by_site[max(dtor_sites)] == dtor, f"step at 0x{site:x} registers dtor 0x{dtor:x}")

    # --- the object map ------------------------------------------------------------------------
    for index, (_, addr, stride) in enumerate(OBJECTS[:-1]):
        require(addr + stride == OBJECTS[index + 1][1],
                f"object stride after 0x{addr:x} is 0x{stride:x}")
    require(OBJECTS[0][1] == 0x32F5AF0 and OBJECTS[-1][1] + OBJECTS[-1][2] == 0x32F6D18,
            "the object block runs from the page to slot 313's first object")
    require([v[2] for v in VARS] == [o[1] for o in OBJECTS if o[0].startswith("var")],
            "the variable objects are the pinned ones")
    require([v[2] for v in ITEMS] == [o[1] for o in OBJECTS if o[0].startswith("item")],
            "the toggle objects are the pinned ones")

    # --- strings -------------------------------------------------------------------------------
    for addr, text, length in STRINGS:
        require(e.read(addr, length + 1) == text + b"\0", f"string {text!r} at {addr:#x}")

    # --- the page constructor 0x475c0 ----------------------------------------------------------
    require(fde_range(e, PAGE_CTOR) == PAGE_CTOR_SIZE, "page ctor FDE == 87")
    require(e.rip(0x475C0, 7, 3) == 0x15958F0, "page ctor vtable base")
    require(e.rip(0xA41852, 7, 3) == 0x1594C90, "the body's page vtable base")
    for site, want, what in PAGE_CTOR_BYTES:
        e.bytes_at(site, want, what)

    # --- the callback bool constructor 0xa41570 ------------------------------------------------
    require(fde_range(e, BOOL_CB_CTOR) == BOOL_CB_CTOR_SIZE, "0xa41570 FDE == 669")
    slot_bytes = {0: (2, b"\xff\x10"), 1: (3, b"\xff\x50\x08"), 3: (3, b"\xff\x50\x18")}
    for site, slot in BOOL_CB_DISPATCH:
        size, want = slot_bytes[slot]
        require(e.read(site, size) == want, f"0xa41570 slot {slot} at 0x{site:x}")
    for site, want, what in BOOL_CB_BYTES:
        e.bytes_at(site, want, what)
    require(e.call(0xA416C0) == 0x20AF0, "0xa41570 calls the base constructor")
    require(e.call(0xA417E0) == 0x20D60, "0xa41570 registers the variable")
    require(e.call(0xA417B0) == 0xF590, "0xa41570 releases the description")
    branch(e, 0xA417F3, 2, 0xA41807, 0x75)
    require(e.call(0xA41807) == 0xF8F3A0, "0xa41570's canary target calls __stack_chk_fail")
    require(e.rip(0xA4158B, 7, 3) == 0x1594208, "0xa41570's final vptr source")
    require(e.rip(0xA416C5, 7, 3) == 0x15940A0, "0xa41570's mid vptr source")
    require(e.reloc_target(0x15FEC90) == 0x274710, "the callback argument q[1] value")


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
        mutated["header"] = pattern.sub(lambda m: f"{m.group(1)}{value ^ 0x4:#x}{m.group(3)}",
                                        texts["header"], count=1)
        require(mutated["header"] != texts["header"], f"constant {name} pattern did not match")
        expect_failure(f"constant {name}", lambda m=mutated: check_native(m))
        native_ran += 1
    return ran, native_ran


def main() -> int:
    arguments = sys.argv[1:]
    if not (ROOT / "local/re/eboot.elf").is_file():
        print("SKIP check_ctor_slot_314: local/re/eboot.elf not present")
        return 0
    e = Elf()
    texts = read_native()
    if "--tamper" in arguments:
        mode = arguments[arguments.index("--tamper") + 1] if len(arguments) > arguments.index("--tamper") + 1 else "elf"
        if mode == "elf":
            e.overlay = {RVA + 0x10: e.raw(RVA + 0x10, 1)[0] ^ 0x40}
        else:
            texts = dict(texts)
            texts["header"] = texts["header"].replace("kSlot314Rva", "kTampered", 1)
    e.recording = True
    run(e, texts)
    e.recording = False
    note = ""
    if "--no-mutate" not in arguments and "--tamper" not in arguments:
        elf_runs, native_runs = mutation_self_test(e, texts)
        note = f"; mutation self-test {elf_runs} ELF + {native_runs} native mutants all rejected"
    print(f"OK check_ctor_slot_314: slot 314 (0xa41810, {SIZE} B FDE, sha {SHA[:12]}...) is row 314 of "
          f"the constructor table at 0x{TABLE_WORD:x}; 84 direct calls over 9 callees, 196 rip operands, "
          f"1 branch to 0xa428f3 (__stack_chk_fail), 8 `callq *0x18(%rax)` inert functor destroys over "
          f"tags 0x15d1770/0x15d1750, 22 __cxa_atexit rows, 1 page + 10 variables + 10 toggles + 1 cell, "
          f"page ctor 0x475c0 (87 B) and callback bool ctor 0xa41570 (669 B) both pinned; guest bytes NOT "
          f"executed{note}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
