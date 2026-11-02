#!/usr/bin/env python3
"""ELF tie for startup constructor index 329 (RVA 0x829e90, 4082 B FDE): the Shadow Path /
path-finding debug page, and the block's own two ranged-variable constructors.

Offline and read-only. Bytes of the pinned private ELF (`local/re/eboot.elf`, sha256 d0723afb...5861d3)
are read through `slot_pin_util.Elf`; nothing is loaded, relocated or executed. Skips cleanly (exit 0)
when the private inputs are absent.

What is pinned here (everything is a byte read of the pinned image):

  * the constructor-table word: entry 329 of the backwards table is the u64 at
    0x2dd70e0 - 329*8 = 0x2dd6698 and must hold 0x829e90. The task brief quoted 0x2dd65b8, which is
    entry **357** (0x78c760); the checker pins the arithmetic, both registered neighbours (328 =
    0x82e040 at 0x2dd66a0, 330 = 0x810a50 at 0x2dd6690) and the fact that 0x2dd70e0 - 0x2dd65b8 =
    357*8, so that reading cannot come back;
  * the body extent and digest: the FDE at .eh_frame_hdr declares 4082 B at [0x829e90, 0x82ae82); the
    4081 B *reachable* span ends on the `callq __stack_chk_fail`, the FDE's last byte 0x82ae81 being
    the trailing nop, and both digests are pinned;
  * all 97 direct `callq` sites and their targets (11 distinct callees), all 124 rip-relative operands
    (11 stores over 9 cells, 69 distinct targets), and the literal instruction bytes in BYTES_PINS
    (prologue, both bound-cell immediates, the 1000.0f/1.0f immediates, the 0x30 item flags, the
    separator's zeroed second argument, the branch, the epilogue and __stack_chk_fail);
  * the single branch: the `jne rel8` at 0x82ae6b must land on 0x82ae7c (__stack_chk_fail);
  * the 22 `__cxa_atexit` rows of the native table, in body order, each with its destructor constant
    loaded before the call and its object loaded inside its own step window;
  * the three indirect sites: every one is `callq *0x18(%rax)` and dispatches through the site's own
    argument tag (0x15ceac8 / 0x15ceae8 / 0x15ceb80), whose slot 3 is a bare `ret`
    (0x829ac0 / 0x829b20 / 0xc4f0);
  * the four class tables this body installs or dispatches through (bool 0x1594218, float 0x15942b0,
    class P 0x15cea60, class C 0x15ceb18) with their destructor, non-deleting destructor, 0x20d60 and
    gate slots, the four argument-functor tables, and the eight gate bodies as bytes;
  * the block's own ranged-variable constructors: leaf A 0x8298d0 (352 B), wrapper B 0x829b30
    (436 B), leaf B 0x829cf0 (352 B), their two non-deleting destructors, the two class destructors
    and the eight 9..20 B functor slot bodies -- each pinned by digest, FDE length, direct calls and
    indirect-site count;
  * the twelve `0x2dce...`-free object strides: ten variables 0xc8/0xd8 apart and twelve items 0xa8
    apart from 0x3288d98 to 0x3289d78, matching the native aggregate's offsets;
  * the strings of the block (names, captions, the "Shadow"/"Shadow Path" group, the one non-empty
    description, "MenuSeparator" and the pathmanager.cpp file/assert text that names the subsystem);
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

RVA = 0x829E90
SIZE = 4082                                    # FDE address_range [0x829e90, 0x82ae82)
REACHABLE = 4081                               # through the callq at 0x82ae7c
SHA = "a35ded4ad1f7c74718b2a9e5d1367c49148663ec59b9a1cef5f65d3bc878b9ba"
SHA_REACHABLE = "36e03879d3cc28c36cf3d7d585e564c3ebe61f0d34e28670fe6def73308892bf"
TABLE_TOP = 0x2DD70E0
TABLE_INDEX = 329
TABLE_WORD = TABLE_TOP - 8 * TABLE_INDEX       # 0x2dd6698
TABLE_WORD_QUOTED_IN_BRIEF = 0x2DD65B8         # entry 357, not 329
EH_FRAME_HDR = 0x150C08C

NATIVE = {
    "header": "native/core/ctor_slot_329_829e90.hpp",
    "source": "native/core/ctor_slot_329_829e90.cpp",
    "test": "native/tests/test_ctor_slot_329.cpp",
    "cmake": "native/CMakeLists.txt",
}

CONSTANTS = {
    "kSlot329Rva": 0x829E90,
    "kSlot329BodySize": 4082,
    "kSlot329EndRva": 0x82AE82,
    "kSlot329ReachableBytes": 4081,
    "kSlot329InstructionCount": 774,
    "kSlot329DirectCallSites": 97,
    "kSlot329DirectCallees": 11,
    "kSlot329IndirectCallSites": 3,
    "kSlot329RipOperands": 124,
    "kSlot329RipTargets": 69,
    "kSlot329StoreSites": 11,
    "kSlot329StoreTargets": 9,
    "kSlot329BranchCount": 1,
    "kSlot329BasicBlocks": 2,
    "kSlot329TableTopRva": 0x2DD70E0,
    "kSlot329TableWordRva": 0x2DD6698,
    "kSlot329TableIndex": 329,
    "kSlot329NextUnrecoveredIndex": 335,
    "kSlot329NextUnrecoveredRva": 0x7EDF10,
    "kSlot329CanaryCellRva": 0x15FE518,
    "kSlot329CanaryFrameOffset": 0xC0,
    "kSlot329StackChkFailRva": 0xF8F3A0,
    "kSlot329BranchRva": 0x82AE6B,
    "kSlot329BranchTargetRva": 0x82AE7C,
    "kSlot329EpilogueRva": 0x82AE6D,
    "kSlot329FrameBytes": 0xE0,
    "kSlot329AtexitRva": 0xF8F390,
    "kSlot329DsoRva": 0x1604218,
    "kSlot329ReleaseRva": 0xF590,
    "kSlot329BaseCtorRva": 0x20AF0,
    "kSlot329RegisterRva": 0x20D60,
    "kSlot329ToggleCtorRva": 0x26EA0,
    "kSlot329GroupCtorRva": 0x26C10,
    "kSlot329SeparatorCtorRva": 0x26B60,
    "kSlot329FloatCtorRva": 0xC300,
    "kSlot329FloatCtorSize": 0x160,
    "kSlot329RangedLeafARva": 0x8298D0,
    "kSlot329RangedLeafASize": 352,
    "kSlot329RangedWrapperRva": 0x829B30,
    "kSlot329RangedWrapperSize": 436,
    "kSlot329RangedLeafBRva": 0x829CF0,
    "kSlot329RangedLeafBSize": 352,
    "kSlot329BoolVarDtorRva": 0x20B0,
    "kSlot329FloatVarDtorRva": 0x20F0,
    "kSlot329MenuItemDtorRva": 0x272D0,
    "kSlot329PathDataDtorRva": 0x81BE00,
    "kSlot329PathDataNonDeletingDtorRva": 0x829A30,
    "kSlot329CharaDtorRva": 0x81BE40,
    "kSlot329CharaNonDeletingDtorRva": 0x829E50,
    "kSlot329BoolMidVptr": 0x15940B0,
    "kSlot329BoolFinalVptr": 0x1594218,
    "kSlot329BoolArgFnVtbl": 0x15CEA30,
    "kSlot329NodeFnCopyRva": 0xC2A0,
    "kSlot329NodeFnCloneRva": 0xC2C0,
    "kSlot329NodeFnMoveRva": 0xC2E0,
    "kSlot329NodeFnDestroyRva": 0xC2F0,
    "kSlot329FloatMidVptr": 0x1594128,
    "kSlot329FloatFinalVptr": 0x15942B0,
    "kSlot329FloatClassFnVtbl": 0x1594318,
    "kSlot329FloatArgFnVtbl": 0x15CEB80,
    "kSlot329FloatFnCopyRva": 0xC4A0,
    "kSlot329FloatFnCloneRva": 0xC4C0,
    "kSlot329FloatFnMoveRva": 0xC4E0,
    "kSlot329FloatFnDestroyRva": 0xC4F0,
    "kSlot329PathDataMidVptr": 0x15CE950,
    "kSlot329PathDataFinalVptr": 0x15CEA60,
    "kSlot329PathDataArgFnVtbl": 0x15CEAC8,
    "kSlot329PathDataFnCopyRva": 0x829A70,
    "kSlot329PathDataFnCloneRva": 0x829A90,
    "kSlot329PathDataFnMoveRva": 0x829AB0,
    "kSlot329PathDataFnDestroyRva": 0x829AC0,
    "kSlot329CharaMidVptr": 0x15CE9C8,
    "kSlot329CharaFinalVptr": 0x15CEB18,
    "kSlot329CharaArgFnVtbl": 0x15CEAE8,
    "kSlot329CharaFnCopyRva": 0x829AD0,
    "kSlot329CharaFnCloneRva": 0x829AF0,
    "kSlot329CharaFnMoveRva": 0x829B10,
    "kSlot329CharaFnDestroyRva": 0x829B20,
    "kSlot329BoolGate3Rva": 0xB890,
    "kSlot329BoolGate6Rva": 0xB930,
    "kSlot329FloatGate3Rva": 0xBBA0,
    "kSlot329FloatGate6Rva": 0xBBF0,
    "kSlot329PathDataGate3Rva": 0x8254C0,
    "kSlot329PathDataGate6Rva": 0x825510,
    "kSlot329CharaGate3Rva": 0x825F40,
    "kSlot329CharaGate6Rva": 0x8261E0,
    "kSlot329BoolGate3": 1,
    "kSlot329BoolGate6": 0,
    "kSlot329FloatGate3": 1,
    "kSlot329FloatGate6": 3,
    "kSlot329PathDataGate3": 1,
    "kSlot329PathDataGate6": 1,
    "kSlot329CharaGate3": 1,
    "kSlot329CharaGate6": 1,
    "kSlot329VarFlags": 1,
    "kSlot329ItemFlags": 0x30,
    "kSlot329PathDataMin": 0,
    "kSlot329PathDataMax": 2,
    "kSlot329PathDataValue": 2,
    "kSlot329CharaMin": 0,
    "kSlot329CharaMax": 3,
    "kSlot329CharaValue": 1,
    "kSlot329TranslationMinBits": 0x00000000,
    "kSlot329TranslationMaxBits": 0x447A0000,
    "kSlot329TranslationValueBits": 0x3F800000,
    "kSlot329EmptyTextRva": 0x128DC9A,
    "kSlot329VarCount": 10,
    "kSlot329ItemCount": 12,
    "kSlot329ToggleCount": 10,
    "kSlot329AtexitRowCount": 22,
    "kSlot329FunctorSiteCount": 3,
    "kSlot329PathDataDispatches": 6,
    "kSlot329CharaDispatches": 12,
    "kSlot329TranslationDispatches": 6,
    "kSlot329FunctorDispatches": 24,
    "kSlot329RangedClassCount": 2,
    "kSlot329BlockRva": 0x3288D98,
    "kSlot329BlockBytes": 0xFE0,
}

# The body order of the native model (each token must appear once, in this order).
SOURCE_ORDER = [
    "if (!build_bool(0)) return out;",
    "if (!build_bool(1)) return out;",
    "if (!build_range(2)) return out;",
    "if (!build_bool(3)) return out;",
    "if (!build_bool(4)) return out;",
    "if (!build_range(5)) return out;",
    "if (!build_range(6)) return out;",
    "if (!build_bool(7)) return out;",
    "if (!build_bool(8)) return out;",
    "if (!build_bool(9)) return out;",
    "for (std::uint32_t index = 0; index < kSlot329ItemCount; ++index) {",
    "out.canary_checked = true;",
]

# The store order inside the shared ranged-variable net state (the leaf anchors).
LEAF_ORDER = [
    "init_concmd_base_20af0(var.base, name, description, flags);",
    "var.base.vptr = cls.mid_vptr;",
    "var.value_ref = value_ref;",
    "var.min_value[0] = 0;",
    "var.min_engaged = 1;",
    "var.max_value[0] = 0;",
    "var.max_engaged = 1;",
    "std::memset(var.args, 0, sizeof var.args);",
    "var.fn_vptr = cls.fn_vtbl;",
    "install_ranged_fn_from_args(var, args, cls.fn_vtbl);",
]

# The wrapper's tail, reproduced by the model's `finish` lambda + the site's restore.
FINISH_ORDER = [
    "var.base.vptr = spec.final_vptr;",
    "std::memcpy(&var.value, &spec.value, 4);",
    "var.default_value = spec.default_value;",
    "out.vars[index].registration = register_concmd_20d60(var.base, concmd);",
]

TEST_TOKENS = [
    "section_constants",
    "section_functor_and_target",
    "section_range_shapes",
    "section_native_build",
    "section_no_allocator",
    "section_menu_registration_blocked",
    "section_parent_contract",
    "section_canary",
    "kSlot329AtexitRows.size() == kSlot329AtexitRowCount",
    "r.atexit_records == 22 && r.functors_destroyed == 3 && r.functor_dispatches == 24",
    "blocked == Slot329Stage::item_ctor && r.blocked_index == 6",
    "kSlot329TableTopRva - 0x2dd65b8 == 357 * 8",
]

CMAKE_TOKENS = [
    "core/ctor_slot_329_829e90.cpp",
    "add_test(NAME ctor_slot_329 COMMAND test_ctor_slot_329)",
]

# The 22 atexit registrations, in body order: (site, dtor, object).
ATEXIT = [
    (0x829FC7, 0x20B0, 0x3288D98),
    (0x82A0A6, 0x20B0, 0x3288E60),
    (0x82A1C1, 0x81BE00, 0x3288F28),
    (0x82A2AC, 0x20B0, 0x3289000),
    (0x82A38E, 0x20B0, 0x32890C8),
    (0x82A483, 0x81BE40, 0x3289190),
    (0x82A59D, 0x20F0, 0x3289268),
    (0x82A684, 0x20B0, 0x3289340),
    (0x82A766, 0x20B0, 0x3289408),
    (0x82A849, 0x20B0, 0x32894D0),
    (0x82A8C9, 0x272D0, 0x3289598),
    (0x82A94D, 0x272D0, 0x3289640),
    (0x82A9D5, 0x272D0, 0x32896E8),
    (0x82AA5C, 0x272D0, 0x3289790),
    (0x82AAE0, 0x272D0, 0x3289838),
    (0x82AB67, 0x272D0, 0x32898E0),
    (0x82ABBA, 0x272D0, 0x3289988),
    (0x82AC41, 0x272D0, 0x3289A30),
    (0x82ACC5, 0x272D0, 0x3289AD8),
    (0x82AD49, 0x272D0, 0x3289B80),
    (0x82ADD0, 0x272D0, 0x3289C28),
    (0x82AE54, 0x272D0, 0x3289CD0),
]

# Every direct `callq` of the body: (site, target). 97 sites over 11 distinct callees.
CALLS = [
    (0x829F25, 0x20AF0), (0x829F8C, 0xF590), (0x829FA9, 0x20D60), (0x829FB1, 0xF590),
    (0x829FC7, 0xF8F390), (0x82A028, 0x20AF0), (0x82A072, 0xF590), (0x82A088, 0x20D60),
    (0x82A090, 0xF590), (0x82A0A6, 0xF8F390), (0x82A165, 0x8298D0), (0x82A171, 0xF590),
    (0x82A1A0, 0x20D60), (0x82A1A8, 0xF590), (0x82A1C1, 0xF8F390), (0x82A222, 0x20AF0),
    (0x82A273, 0xF590), (0x82A28B, 0x20D60), (0x82A293, 0xF590), (0x82A2AC, 0xF8F390),
    (0x82A30D, 0x20AF0), (0x82A35A, 0xF590), (0x82A370, 0x20D60), (0x82A378, 0xF590),
    (0x82A38E, 0xF8F390), (0x82A446, 0x829B30), (0x82A452, 0xF590), (0x82A46D, 0xF590),
    (0x82A483, 0xF8F390), (0x82A542, 0xC300), (0x82A54E, 0xF590), (0x82A57F, 0x20D60),
    (0x82A587, 0xF590), (0x82A59D, 0xF8F390), (0x82A5FE, 0x20AF0), (0x82A64D, 0xF590),
    (0x82A663, 0x20D60), (0x82A66B, 0xF590), (0x82A684, 0xF8F390), (0x82A6E5, 0x20AF0),
    (0x82A72F, 0xF590), (0x82A745, 0x20D60), (0x82A74D, 0xF590), (0x82A766, 0xF8F390),
    (0x82A7C7, 0x20AF0), (0x82A816, 0xF590), (0x82A82C, 0x20D60), (0x82A834, 0xF590),
    (0x82A849, 0xF8F390), (0x82A8A7, 0x26C10), (0x82A8AF, 0xF590), (0x82A8B7, 0xF590),
    (0x82A8C9, 0xF8F390), (0x82A928, 0x26EA0), (0x82A930, 0xF590), (0x82A938, 0xF590),
    (0x82A94D, 0xF8F390), (0x82A9B3, 0x26EA0), (0x82A9BB, 0xF590), (0x82A9C3, 0xF590),
    (0x82A9D5, 0xF8F390), (0x82AA34, 0x26EA0), (0x82AA3C, 0xF590), (0x82AA44, 0xF590),
    (0x82AA5C, 0xF8F390), (0x82AAC2, 0x26EA0), (0x82AACA, 0xF590), (0x82AAD2, 0xF590),
    (0x82AAE0, 0xF8F390), (0x82AB46, 0x26EA0), (0x82AB4E, 0xF590), (0x82AB56, 0xF590),
    (0x82AB67, 0xF8F390), (0x82ABA4, 0x26B60), (0x82ABAC, 0xF590), (0x82ABBA, 0xF8F390),
    (0x82AC19, 0x26EA0), (0x82AC21, 0xF590), (0x82AC29, 0xF590), (0x82AC41, 0xF8F390),
    (0x82ACA7, 0x26EA0), (0x82ACAF, 0xF590), (0x82ACB7, 0xF590), (0x82ACC5, 0xF8F390),
    (0x82AD2B, 0x26EA0), (0x82AD33, 0xF590), (0x82AD3B, 0xF590), (0x82AD49, 0xF8F390),
    (0x82ADAF, 0x26EA0), (0x82ADB7, 0xF590), (0x82ADBF, 0xF590), (0x82ADD0, 0xF8F390),
    (0x82AE36, 0x26EA0), (0x82AE3E, 0xF590), (0x82AE46, 0xF590), (0x82AE54, 0xF8F390),
    (0x82AE7C, 0xF8F3A0),
]

# Every direct callee: (target, site count, FDE length). The two 0xf8f3xx targets are PLT stubs with
# no FDE of their own, so their length is None.
CALLEES = [
    (0xF590, 43, 73),      # shared release of a borrowed stack view
    (0xF8F390, 22, None),  # __cxa_atexit PLT
    (0x26EA0, 10, 237),    # toggle item ctor
    (0x20D60, 9, 26),      # register-once guard
    (0x20AF0, 7, 90),      # ConCmdBase ctor
    (0x8298D0, 1, 352),    # local ranged leaf A (class P)
    (0x829B30, 1, 436),    # local ranged wrapper B (class C)
    (0xC300, 1, 352),      # float variable ctor
    (0x26C10, 1, 194),     # group item ctor
    (0x26B60, 1, 166),     # separator item ctor
    (0xF8F3A0, 1, None),   # __stack_chk_fail PLT (the branch target only)
]

# Every rip-relative operand: (site, instruction length, displacement offset, target). 124 operands.
RIPS = [
    (0x829EA8, 7, 3, 0x15FE518), (0x829EAF, 7, 3, 0x1594208), (0x829EB6, 7, 3, 0x128D690), (0x829EBD, 7, 3, 0x128DC9A),
    (0x829ECE, 7, 3, 0x3288D98), (0x829F15, 7, 3, 0x3288D98), (0x829F2A, 7, 3, 0x15940A0), (0x829F34, 7, 3, 0x3288E58),
    (0x829F3F, 7, 3, 0x15CEA30), (0x829F4A, 7, 3, 0x3288D98), (0x829F51, 7, 3, 0x3288DF8), (0x829F58, 10, 2, 0x3288E01),
    (0x829F62, 8, 4, 0x3288E08), (0x829F6A, 11, 3, 0x3288E38), (0x829F75, 7, 3, 0x3288E48), (0x829F81, 11, 3, 0x3288E50),
    (0x829F94, 7, 3, 0x3288D98), (0x829F9B, 7, 2, 0x3288E58), (0x829FA2, 7, 2, 0x3288E00), (0x829FB6, 7, 3, 0x20B0),
    (0x829FBD, 7, 3, 0x1604218), (0x829FCC, 7, 3, 0x3288E60), (0x829FD3, 7, 3, 0x128D6A2), (0x829FF2, 7, 3, 0x128DC9A),
    (0x82A03B, 7, 3, 0x15CEA30), (0x82A095, 7, 3, 0x20B0), (0x82A09C, 7, 3, 0x1604218), (0x82A0AB, 7, 3, 0x15CEA50),
    (0x82A0B2, 7, 3, 0x3288F28), (0x82A0B9, 7, 3, 0x128D6BD), (0x82A0FE, 7, 3, 0x15CEAC8), (0x82A125, 7, 3, 0x128DC9A),
    (0x82A1AD, 7, 3, 0x1604218), (0x82A1B4, 7, 3, 0x81BE00), (0x82A1C6, 7, 3, 0x3289000), (0x82A1CD, 7, 3, 0x128D6E0),
    (0x82A1EC, 7, 3, 0x128DC9A), (0x82A23A, 7, 3, 0x15CEA30), (0x82A298, 7, 3, 0x20B0), (0x82A29F, 7, 3, 0x1604218),
    (0x82A2B1, 7, 3, 0x32890C8), (0x82A2B8, 7, 3, 0x128D706), (0x82A2D7, 7, 3, 0x128DC9A), (0x82A320, 7, 3, 0x15CEA30),
    (0x82A37D, 7, 3, 0x20B0), (0x82A384, 7, 3, 0x1604218), (0x82A393, 7, 3, 0x3289190), (0x82A39A, 7, 3, 0x128D71F),
    (0x82A3F1, 7, 3, 0x15CEAE8), (0x82A418, 7, 3, 0x128D73D), (0x82A472, 7, 3, 0x81BE40), (0x82A479, 7, 3, 0x1604218),
    (0x82A488, 7, 3, 0x15942A0), (0x82A48F, 7, 3, 0x3289268), (0x82A496, 7, 3, 0x128D784), (0x82A4DB, 7, 3, 0x15CEB80),
    (0x82A502, 7, 3, 0x128DC9A), (0x82A58C, 7, 3, 0x20F0), (0x82A593, 7, 3, 0x1604218), (0x82A5A2, 7, 3, 0x3289340),
    (0x82A5A9, 7, 3, 0x128D7A5), (0x82A5C8, 7, 3, 0x128DC9A), (0x82A616, 7, 3, 0x15CEA30), (0x82A670, 7, 3, 0x20B0),
    (0x82A677, 7, 3, 0x1604218), (0x82A689, 7, 3, 0x3289408), (0x82A690, 7, 3, 0x128D7C5), (0x82A6AF, 7, 3, 0x128DC9A),
    (0x82A6F8, 7, 3, 0x15CEA30), (0x82A752, 7, 3, 0x20B0), (0x82A759, 7, 3, 0x1604218), (0x82A76B, 7, 3, 0x32894D0),
    (0x82A772, 7, 3, 0x128D7EB), (0x82A791, 7, 3, 0x128DC9A), (0x82A7DF, 7, 3, 0x15CEA30), (0x82A839, 7, 3, 0x1604218),
    (0x82A84E, 7, 3, 0x3289598), (0x82A85F, 7, 3, 0x128D810), (0x82A866, 7, 3, 0x128D817), (0x82A8BC, 7, 3, 0x272D0),
    (0x82A8CE, 7, 3, 0x3289640), (0x82A8D5, 7, 3, 0x3288E60), (0x82A8E6, 7, 3, 0x128D823), (0x82A93D, 7, 3, 0x272D0),
    (0x82A952, 7, 3, 0x32896E8), (0x82A959, 7, 3, 0x3288F28), (0x82A96A, 7, 3, 0x128D817), (0x82A971, 7, 3, 0x128D835),
    (0x82A9C8, 7, 3, 0x1604218), (0x82A9DA, 7, 3, 0x3289790), (0x82A9E1, 7, 3, 0x3289000), (0x82A9F2, 7, 3, 0x128D84C),
    (0x82AA49, 7, 3, 0x1604218), (0x82AA61, 7, 3, 0x3289838), (0x82AA68, 7, 3, 0x3289190), (0x82AA6F, 7, 3, 0x128D817),
    (0x82AA8F, 7, 3, 0x128D86B), (0x82AAE5, 7, 3, 0x32898E0), (0x82AAEC, 7, 3, 0x128D817), (0x82AB07, 7, 3, 0x3288D98),
    (0x82AB13, 7, 3, 0x128D881), (0x82AB6C, 7, 3, 0x3289988), (0x82AB78, 7, 3, 0x128D817), (0x82ABBF, 7, 3, 0x3289A30),
    (0x82ABC6, 7, 3, 0x32890C8), (0x82ABD7, 7, 3, 0x128D895), (0x82AC2E, 7, 3, 0x1604218), (0x82AC46, 7, 3, 0x3289AD8),
    (0x82AC4D, 7, 3, 0x3289268), (0x82AC54, 7, 3, 0x128D817), (0x82AC74, 7, 3, 0x128D8A4), (0x82ACCA, 7, 3, 0x3289B80),
    (0x82ACD1, 7, 3, 0x3289340), (0x82ACD8, 7, 3, 0x128D817), (0x82ACF8, 7, 3, 0x128D8B8), (0x82AD4E, 7, 3, 0x3289C28),
    (0x82AD55, 7, 3, 0x3289408), (0x82AD5C, 7, 3, 0x128D817), (0x82AD7C, 7, 3, 0x128D8C7), (0x82ADD5, 7, 3, 0x3289CD0),
    (0x82ADDC, 7, 3, 0x32894D0), (0x82ADE3, 7, 3, 0x128D817), (0x82AE03, 7, 3, 0x128D8E9), (0x82AE59, 7, 3, 0x15FE518),
]

# Every rip-relative STORE of the body: (site, target). 11 sites over 9 distinct cells. Only the FIRST
# bool variable is addressed rip-relative; every later object is reached through a register base.
STORES = [
    (0x829F15, 0x3288D98), (0x829F4A, 0x3288D98), (0x829F51, 0x3288DF8),
    (0x829F58, 0x3288E01), (0x829F62, 0x3288E08), (0x829F6A, 0x3288E38),
    (0x829F75, 0x3288E48), (0x829F81, 0x3288E50), (0x829F94, 0x3288D98),
    (0x829F9B, 0x3288E58), (0x829FA2, 0x3288E00),
]

# Literal instruction bytes that no rip/call pin covers. `disp32` operands are checked by RIPS, so the
# raw bytes here are the register/imm forms only.
BYTES_PINS = [
    (0x829E90, "554889e5", "push rbp; mov rsp,rbp"),
    (0x829E94, "415741564155415453", "push r15/r14/r13/r12/rbx"),
    (0x829E9D, "4883e4e0", "and rsp,-0x20"),
    (0x829EA1, "4881ece0000000", "sub rsp,0xe0"),
    (0x829EE8, "48898424c0000000", "canary -> 0xc0(%rsp)"),
    (0x829ED9, "b901000000", "the bool/range variable flags ecx = 1"),
    (0x82A0D9, "c744241c00000000", "path data: the min cell = 0"),
    (0x82A0E1, "c744241802000000", "path data: the max cell = 2"),
    (0x82A0F3, "41b901000000", "path data: flags r9d = 1"),
    (0x82A18F, "c783d000000002000000", "path data: value 2 into +0xd0"),
    (0x82A199, "c7436802000000", "path data: default 2 into +0x68"),
    (0x82A3BA, "c744241c01000000", "chara type: the value cell = 1"),
    (0x82A3C2, "c744241800000000", "chara type: the min cell = 0"),
    (0x82A3CA, "c744241403000000", "chara type: the max cell = 3"),
    (0x82A3E1, "41b901000000", "chara type: flags r9d = 1"),
    (0x82A4B6, "c744241c00000000", "translation: the min cell = 0.0f"),
    (0x82A4BE, "c744241800007a44", "translation: the max cell = 1000.0f"),
    (0x82A4D0, "41b901000000", "translation: flags r9d = 1"),
    (0x82A56C, "41c787d00000000000803f", "translation: value 1.0f into +0xd0"),
    (0x82A577, "41c747680000803f", "translation: default 1.0f into +0x68"),
    (0x82A871, "b930000000", "group: flags ecx = 0x30"),
    (0x82A8F1, "41b830000000", "first toggle: flags r8d = 0x30"),
    (0x82AB83, "31d2", "separator: xorl %edx,%edx (the unused second argument)"),
    (0x82AB85, "b930000000", "separator: flags ecx = 0x30"),
    (0x82AE6B, "750f", "the guard branch: jne rel8 -> 0x82ae7c"),
    (0x82AE6D, "488d65d8", "lea -0x28(%rbp),%rsp"),
    (0x82AE71, "5b415c415d415e415f5dc3", "pops + retq"),
    (0x82AE7C, "e81f457600", "callq __stack_chk_fail"),
    (0x82AE81, "90", "the body's last byte (nop)"),
]

# The three `callq *0x18(%rax)` sites and the argument tag each dispatches through.
INDIRECT = [
    (0x82A186, 0x15CEAC8, 0x829AC0),
    (0x82A467, 0x15CEAE8, 0x829B20),
    (0x82A563, 0x15CEB80, 0xC4F0),
]

# The four class tables this body installs or dispatches through, and their pinned slots. Address
# point = base + 0x10 = the object's vptr; slot 0 (destructor) sits at the point, slot 1 at point+8,
# slot 2 (0x20d60) at point+0x10, slot 3 at point+0x18 and slot 6 at point+0x30.
CLASSES = [
    # (base, point, dtor, non-deleting dtor, register body, gate 3 site, gate 3 body, gate 6 site,
    #  gate 6 body, gate 3 value, gate 6 value)
    (0x1594208, 0x1594218, 0x20B0, None, 0x20D60, 0x1594230, 0xB890, 0x1594248, 0xB930, 1, 0),
    (0x15942A0, 0x15942B0, 0x20F0, None, 0x20D60, 0x15942C8, 0xBBA0, 0x15942E0, 0xBBF0, 1, 3),
    (0x15CEA50, 0x15CEA60, 0x81BE00, 0x829A30, 0x20D60, 0x15CEA78, 0x8254C0, 0x15CEA90, 0x825510, 1, 1),
    (0x15CEB08, 0x15CEB18, 0x81BE40, 0x829E50, 0x20D60, 0x15CEB30, 0x825F40, 0x15CEB48, 0x8261E0, 1, 1),
]

# The four argument-functor tables. 0x15cea30 and 0x15ceae8 are four slots each; 0x15ceac8 and
# 0x15ceb80 carry the other family's four bodies in their second half.
ARG_TABLES = [
    (0x15CEA30, (0xC2A0, 0xC2C0, 0xC2E0, 0xC2F0)),
    (0x15CEAC8, (0x829A70, 0x829A90, 0x829AB0, 0x829AC0, 0x829AD0, 0x829AF0, 0x829B10, 0x829B20)),
    (0x15CEAE8, (0x829AD0, 0x829AF0, 0x829B10, 0x829B20)),
    (0x15CEB80, (0xC4A0, 0xC4C0, 0xC4E0, 0xC4F0, 0xC2A0, 0xC2C0, 0xC2E0, 0xC2F0)),
]

# The gate bodies and the functor destroy slots as bytes.
GATE_BYTES = {
    0x8254C0: "b801000000c3",
    0x825510: "b801000000c3",
    0x825F40: "b801000000c3",
    0x8261E0: "b801000000c3",
    0xB890: "b801000000c3",
    0xB930: "31c0c3",
    0xBBA0: "b801000000c3",
    0xBBF0: "b803000000c3",
    0xC4F0: "c3",
    0xC2F0: "c3",
    0x829AC0: "c3",
    0x829B20: "c3",
}

# The block's own ranged-variable constructors: (rva, FDE length, sha256).
SUB_BODIES = [
    (0x8298D0, 352, "73b9e5d74880b4db1210873a652e9a7f469b03a191a5961ea7f9c4a2fcd35398"),
    (0x829B30, 436, "c305a5991e3bf15d6cb3ed6a7909089e763d2b0aeacac1b6d8bd40cdd5297279"),
    (0x829CF0, 352, "dbabd757bd1602ab2d55ce7780837d078b41dacb43e739474d5fb845ad012563"),
    (0x829A30, 62, "a26d6cd79574c6c8fe23ed7d5903e81bdae1a11ef751c6118e0d8dc169a3d6cd"),
    (0x829E50, 62, "cdcf5cb356359e7ecb3e606be2bdbedfdf84670b9315dad2cb6bcecf356deaea"),
    (0x81BE00, 54, "60c429c3f278de8cf166bbb47364b7872fd4277910656b54f4f81c9f0d0194dc"),
    (0x81BE40, 54, "fd37d6daa4cbd1db7f2c50c1e062221eb38e499fd729c6dcdffa26484fd5cf72"),
    (0x829A70, 20, "cbc1c5f62b09159d4636f43602a1db1d9bf29fbca2b73f47e4ffda4118ca4c66"),
    (0x829A90, 20, "24eb1799606d34bb69fe5bf7b9dfe4a87fe56724f57e54abd7f3f429827d13b9"),
    (0x829AB0, 9, "547772c99434936d33353159cfc12801b74a62a235674217d91aa61cc05467bd"),
    (0x829AC0, 1, "ae3f4619b0413d70d3004b9131c3752153074e45725be13b9a148978895e359e"),
    (0x829AD0, 20, "a10c86d59b79057fca22efa69d8c5982377af5aeeaf848c5c8e142a1b5f942b1"),
    (0x829AF0, 20, "4a142c5be786c1f079a96e6c72f481b9a4154785b4034cd8d9464e2617d42f45"),
    (0x829B10, 9, "547772c99434936d33353159cfc12801b74a62a235674217d91aa61cc05467bd"),
    (0x829B20, 1, "ae3f4619b0413d70d3004b9131c3752153074e45725be13b9a148978895e359e"),
]

# Every direct call inside the three constructor bodies: (site, target).
SUB_CALLS = [
    (0x829906, 0x20AF0), (0x829A2A, 0xF8F3A0),
    (0x829C87, 0x829CF0), (0x829CB7, 0x20D60), (0x829CDE, 0xF8F3A0),
    (0x829D26, 0x20AF0), (0x829E4A, 0xF8F3A0),
]

# The indirect `callq *reg` sites inside each constructor body: (site, the instruction's bytes).
# Three-body count: 5 in leaf A, 6 in wrapper B, 5 in leaf B.
SUB_INDIRECT = [
    (0x8299B6, "ff5008"), (0x8299C7, "ff10"), (0x8299DD, "ff10"), (0x8299E9, "ff10"),
    (0x829A08, "ff5018"),
    (0x829C01, "ff5108"), (0x829C12, "ff10"), (0x829C28, "ff10"), (0x829C34, "ff10"),
    (0x829C53, "ff5018"), (0x829C9A, "ff5018"),
    (0x829DD6, "ff5008"), (0x829DE7, "ff10"), (0x829DFD, "ff10"), (0x829E09, "ff10"),
    (0x829E28, "ff5018"),
]

# The ten variables and twelve items of the block, as original object addresses (body order).
OBJECTS = [
    0x3288D98, 0x3288E60, 0x3288F28, 0x3289000, 0x32890C8,
    0x3289190, 0x3289268, 0x3289340, 0x3289408, 0x32894D0,
    0x3289598, 0x3289640, 0x32896E8, 0x3289790, 0x3289838, 0x32898E0,
    0x3289988, 0x3289A30, 0x3289AD8, 0x3289B80, 0x3289C28, 0x3289CD0,
]
OBJECT_STRIDES = [0xC8, 0xC8, 0xD8, 0xC8, 0xC8, 0xD8, 0xD8, 0xC8, 0xC8, 0xC8] + [0xA8] * 11
BLOCK_END = 0x3289D78

# Strings of the block: (address, text).
STRINGS = [
    (0x128D690, b"enablePathFinding"),
    (0x128D6A2, b"shadow_pathDebugDrawEnable"),
    (0x128D6BD, b"shadow_pathDebugDrawPathDataEnable"),
    (0x128D6E0, b"shadow_pathDebugDrawAttainRangeEnable"),
    (0x128D706, b"shadow_pathEnableEditing"),
    (0x128D71F, b"shadow_pathDebugDrawCharaType"),
    (0x128D784, b"shadow_pathDebugTranslationSpeed"),
    (0x128D7A5, b"shadow_pathDebugDisplayAllNames"),
    (0x128D7C5, b"shadow_enableSpeculativeAsyncRaycasts"),
    (0x128D7EB, b"shadow_debugSpeculativeAsyncRaycasts"),
    (0x128D810, b"Shadow"),
    (0x128D817, b"Shadow Path"),
    (0x128D823, b"Debug Draw Enable"),
    (0x128D835, b"Debug Draw Data Enable"),
    (0x128D84C, b"Debug Draw Attain Range Enable"),
    (0x128D86B, b"Debug Draw Chara Type"),
    (0x128D881, b"Enable Path Finding"),
    (0x128D895, b"Enable Editing"),
    (0x128D8A4, b"Editing Trans Speed"),
    (0x128D8B8, b"Show All Names"),
    (0x128D8C7, b"Speculative Async Raycasts Enable"),
    (0x128D8E9, b"Speculative Async Raycasts Debug"),
    (0x128D73D, b"Which attract mode to play. Default will play the system selected one."),
    (0x128DA07, b"Off"),
    (0x128DA0B, b"EPathDataDrawType::Off"),
    (0x128DA22, b"Proximity"),
    (0x128DA2C, b"EPathDataDrawType::Proximity"),
    (0x128DA49, b"All"),
    (0x128DA4D, b"EPathDataDrawType::All"),
    (0x128DA64, b"EPathDataDrawType"),
    (0x128DA79, b"Boss"),
    (0x128DA7E, b"EPathObjectCharaType::Boss"),
    (0x128DA99, b"Horse"),
    (0x128DA9F, b"EPathObjectCharaType::Horse"),
    (0x128DABB, b"SmallAnimal"),
    (0x128DAE9, b"SmallAnimalDemo"),
    (0x128DB1F, b"MAX"),
    (0x128DB3D, b"EPathObjectCharaType"),
    (0x128DBC1, b"d:\\dev\\cp10\\Main\\Code\\Game\\CP10\\Shadow\\sources\\gamecore\\path\\src\\pathmanager.cpp"),
    (0x128DC12, b"void bp_doDrawPathPoint(PathDataAddr *, int)"),
    (0xF96530, b"MenuSeparator"),
]
# The empty description view points at the zero byte that heads the enum string table.
EMPTY_BYTES = 0x128DC9A


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


def check_elf(e: Elf) -> None:
    # --- the startup-table row ---------------------------------------------------------------
    require(TABLE_TOP - 8 * TABLE_INDEX == TABLE_WORD == 0x2DD6698, "table row arithmetic")
    require(e.u64(TABLE_WORD) == RVA, "constructor-table entry 329 -> 0x829e90")
    # The table is indexed backwards, so entry 328 sits at a HIGHER address and entry 330 lower.
    require(TABLE_WORD + 8 == 0x2DD66A0 and e.u64(TABLE_WORD + 8) == 0x82E040,
            "entry 328 is the already-registered 0x82e040")
    require(TABLE_WORD - 8 == 0x2DD6690 and e.u64(TABLE_WORD - 8) == 0x810A50,
            "entry 330 is the already-registered 0x810a50")
    # The address quoted in the task brief is entry 357, not 329.
    require(TABLE_TOP - TABLE_WORD_QUOTED_IN_BRIEF == 357 * 8, "0x2dd65b8 is entry 357")
    require(e.u64(TABLE_WORD_QUOTED_IN_BRIEF) == 0x78C760, "entry 357 is 0x78c760")

    # --- body extent, digest, padding ---------------------------------------------------------
    body = e.read(RVA, SIZE)
    if not e.skip_hash:
        require(digest(body) == SHA, "body sha256 (4082 B FDE)")
        require(digest(body[:REACHABLE]) == SHA_REACHABLE, "reachable body sha256 (4081 B)")
    require(body[0] == 0x55 and body[1:4] == b"\x48\x89\xe5", "push rbp; mov rsp,rbp")
    require(body[-1] == 0x90, "the FDE's last byte is the trailing nop")
    require(fde_range(e, RVA) == SIZE, "FDE address_range == 4082")

    # --- every direct call, every rip operand, the literal bytes -------------------------------
    require(len(CALLS) == 97 and len({site for site, _ in CALLS}) == 97, "97 distinct call sites")
    for site, target in CALLS:
        require(e.call(site) == target, f"call at 0x{site:x} -> 0x{target:x}")
    require(len({target for _, target in CALLS}) == 11, "11 distinct direct callees")
    require(len(CALLEES) == 11, "the callee histogram covers every target")
    histogram = {}
    for site, target in CALLS:
        histogram[target] = histogram.get(target, 0) + 1
    for target, sites, fde_size in CALLEES:
        require(histogram.get(target) == sites, f"callee 0x{target:x} has {sites} call sites")
        if fde_size is not None:
            require(fde_range(e, target) == fde_size, f"callee 0x{target:x} FDE == {fde_size}")
    require(sum(sites for _, sites, _ in CALLEES) == len(CALLS), "the histogram sums to 97")
    require(len(RIPS) == 124 and len({site for site, _, _, _ in RIPS}) == 124, "124 distinct rip sites")
    for site, length, disp_off, target in RIPS:
        require(e.rip(site, length, disp_off) == target, f"rip operand at 0x{site:x} -> 0x{target:x}")
    require(len({target for _, _, _, target in RIPS}) == 69, "69 distinct rip targets")
    # The 11 rip-relative stores land on 9 distinct cells. Every site is in RIPS as well.
    require(len(STORES) == 11 and len({site for site, _ in STORES}) == 11, "11 distinct store sites")
    require(len({target for _, target in STORES}) == 9, "9 distinct store targets")
    rip_map = {site: target for site, _, _, target in RIPS}
    for site, target in STORES:
        require(rip_map.get(site) == target, f"store at 0x{site:x} -> 0x{target:x}")
    for site, want, what in BYTES_PINS:
        e.bytes_at(site, want, what)

    # --- the one branch, decoded --------------------------------------------------------------
    branch = e.read(0x82AE6B, 2)
    require(branch[0] == 0x75, "the guard branch is jne rel8 at 0x82ae6b")
    require(0x82AE6B + 2 + struct.unpack("<b", branch[1:2])[0] == 0x82AE7C, "jne target 0x82ae7c")
    require(e.call(0x82AE7C) == 0xF8F3A0, "the branch target calls __stack_chk_fail")

    # --- the three indirect sites -------------------------------------------------------------
    require(len(INDIRECT) == 3, "3 indirect sites")
    for site, tag, slot3 in INDIRECT:
        require(e.read(site, 3) == b"\xff\x50\x18", f"0x{site:x} is `callq *0x18(%rax)`")
        require(e.reloc_target(tag + 0x18) == slot3, f"tag 0x{tag:x} slot 3 -> 0x{slot3:x}")
        e.bytes_at(slot3, "c3", f"functor destroy 0x{slot3:x} is a bare ret")

    # --- the class tables and their gate answers ----------------------------------------------
    for (base, point, dtor, non_deleting, register_body, g3_site, g3_rva, g6_site, g6_rva, g3,
         g6) in CLASSES:
        require(point == base + 0x10, "the address point is table + 0x10")
        require(e.reloc_target(point) == dtor, f"vptr 0x{point:x} slot 0 -> 0x{dtor:x}")
        if non_deleting is not None:
            require(e.reloc_target(point + 8) == non_deleting,
                    f"vptr 0x{point:x} slot 1 -> 0x{non_deleting:x}")
        require(e.reloc_target(base + 0x20) == register_body,
                f"table 0x{base:x} + 0x20 -> 0x{register_body:x} (0x20d60)")
        require(e.reloc_target(g3_site) == g3_rva, f"slot 3 of 0x{point:x} -> 0x{g3_rva:x}")
        require(e.reloc_target(g6_site) == g6_rva, f"slot 6 of 0x{point:x} -> 0x{g6_rva:x}")
        e.bytes_at(g3_rva, GATE_BYTES[g3_rva], f"gate 3 body 0x{g3_rva:x}")
        e.bytes_at(g6_rva, GATE_BYTES[g6_rva], f"gate 6 body 0x{g6_rva:x}")
        require(g3_rva in GATE_BYTES and GATE_BYTES[g3_rva][:2] == "b8",
                "gate 3 returns an immediate")
        body3 = GATE_BYTES[g3_rva]
        value3 = struct.unpack("<I", bytes.fromhex(body3[2:10]))[0]
        require(g3 == value3, f"gate 3 value 0x{g3_rva:x}")
        body6 = GATE_BYTES[g6_rva]
        value6 = 0 if body6.startswith("31c0") else struct.unpack("<I", bytes.fromhex(body6[2:10]))[0]
        require(g6 == value6, f"gate 6 value 0x{g6_rva:x}")

    # --- the four argument-functor tables ------------------------------------------------------
    for table, slots in ARG_TABLES:
        for slot, wanted in enumerate(slots):
            require(e.reloc_target(table + 8 * slot) == wanted,
                    f"argument table 0x{table:x} slot {slot} -> 0x{wanted:x}")
    # Both destroy families are one `ret`, so every row-local and in-leaf dispatch is inert.
    for addr in (0xC4F0, 0xC2F0, 0x829AC0, 0x829B20):
        e.bytes_at(addr, "c3", f"destroy slot 0x{addr:x} is a bare ret")
    # The copy slots install their own family's class tag.
    require(e.rip(0xC2A4, 7, 3) == 0x1594280, "0xc2a0 copies the bool class tag 0x1594280")
    require(e.rip(0xC4A4, 7, 3) == 0x1594318, "0xc4a0 copies the float class tag 0x1594318")
    require(e.rip(0x829A74, 7, 3) == 0x15CEAC8, "0x829a70 copies the class-P functor tag")
    require(e.rip(0x829AD4, 7, 3) == 0x15CEAE8, "0x829ad0 copies the class-C functor tag")

    # --- the block's own ranged-variable constructors -------------------------------------------
    # The sub-body digests stay active during the mutation run: they are the only pin over the parts
    # of those bodies no site-level read covers.
    for rva, size, sha in SUB_BODIES:
        require(fde_range(e, rva) == size, f"sub-body 0x{rva:x} FDE == {size}")
        require(digest(e.read(rva, size)) == sha, f"sub-body 0x{rva:x} sha256")
    for site, target in SUB_CALLS:
        require(e.call(site) == target, f"sub-body call at 0x{site:x} -> 0x{target:x}")
    for site, want in SUB_INDIRECT:
        e.bytes_at(site, want, f"sub-body indirect site 0x{site:x}")

    # --- the 22 __cxa_atexit registrations, in body order ---------------------------------------
    atexit_sites = [site for site, target in CALLS if target == 0xF8F390]
    require(atexit_sites == [site for site, _, _ in ATEXIT], "the 22 atexit sites in body order")
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

    # --- the object block ------------------------------------------------------------------------
    require(len(OBJECTS) == 22, "ten variables + twelve items")
    for i, stride in enumerate(OBJECT_STRIDES):
        require(OBJECTS[i + 1] - OBJECTS[i] == stride,
                f"object {i} at 0x{OBJECTS[i]:x} is 0x{stride:x} from its neighbour")
    require(OBJECTS[-1] + 0xA8 == BLOCK_END, "the block ends at 0x3289d78")
    require(OBJECTS[0] == 0x3288D98, "the block starts at 0x3288d98")
    # The first ten objects are the variables, the last twelve the items.
    for i in range(10):
        require(ATEXIT[i][2] == OBJECTS[i], f"variable {i} at 0x{OBJECTS[i]:x}")
    for i in range(12):
        require(ATEXIT[10 + i][2] == OBJECTS[10 + i], f"item {i} at 0x{OBJECTS[10 + i]:x}")

    # --- strings ---------------------------------------------------------------------------------
    for addr, text in STRINGS:
        e.cstring(addr, text)
    e.bytes_at(EMPTY_BYTES, "00", "the empty description byte")


def check_native(texts: dict[str, str]) -> None:
    header = texts["header"]
    for name, value in CONSTANTS.items():
        require(const_value(header, name) == value, f"native constant {name}")
    for tokens, key in ((SOURCE_ORDER, "source"), (LEAF_ORDER, "source"), (FINISH_ORDER, "source")):
        positions = []
        for token in tokens:
            require(token in texts[key], f"source token {token!r}")
            positions.append(texts[key].index(token))
        require(positions == sorted(positions) and len(set(positions)) == len(positions),
                f"native steps are in body order ({tokens[0]!r} ...)")
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
                        ("source", LEAF_ORDER), ("source", FINISH_ORDER), ("test", TEST_TOKENS),
                        ("cmake", CMAKE_TOKENS)):
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
        print("SKIP check_ctor_slot_329: local/re/eboot.elf not present")
        return 0
    e = Elf()
    texts = read_native()
    if "--tamper" in arguments:
        mode = arguments[arguments.index("--tamper") + 1] if len(arguments) > arguments.index("--tamper") + 1 else "elf"
        if mode == "elf":
            e.overlay = {RVA + 0x10: e.raw(RVA + 0x10, 1)[0] ^ 0x40}
        else:
            texts = dict(texts)
            texts["header"] = texts["header"].replace("kSlot329Rva", "kTampered", 1)
    e.recording = True
    run(e, texts)
    e.recording = False
    note = ""
    if "--no-mutate" not in arguments and "--tamper" not in arguments:
        elf_runs, native_runs = mutation_self_test(e, texts)
        note = f"; mutation self-test {elf_runs} ELF + {native_runs} native mutants all rejected"
    print(f"OK check_ctor_slot_329: slot 329 (0x829e90, {SIZE} B FDE, {REACHABLE} B reachable, sha "
          f"{SHA[:12]}...) is row 329 of the constructor table at 0x{TABLE_WORD:x}; 97 direct calls over "
          f"11 callees, 124 rip operands, 1 branch (0x82ae6b -> 0x82ae7c __stack_chk_fail), 3 "
          f"`callq *0x18(%rax)` inert functor destroys, 22 __cxa_atexit rows, the local ranged-variable "
          f"leaf/wrapper constructors 0x8298d0 / 0x829b30 + 0x829cf0, and the four gate classes; guest "
          f"bytes NOT executed{note}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
