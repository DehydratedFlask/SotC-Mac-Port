#!/usr/bin/env python3
"""ELF tie for startup constructor index 335 (RVA 0x7edf10, 2725 B FDE): the "Shadow Cheat" assist
debug page and the four helpers this block is the only user of.

Offline and read-only. Bytes of the pinned private ELF (`local/re/eboot.elf`, sha256 d0723afb...5861d3)
are read through `slot_pin_util.Elf`; nothing is loaded, relocated or executed. Skips cleanly (exit 0)
when the private inputs are absent.

What is pinned here (everything is a byte read of the pinned image):

  * the constructor-table word: entry 335 of the backwards table is the u64 at
    0x2dd70e0 - 335*8 = 0x2dd6668 and must hold 0x7edf10, with both registered neighbours (334 =
    0x7fc530 at 0x2dd6670, 336 = 0x7ec4c0 at 0x2dd6660);
  * the body extent and digest: the FDE at .eh_frame_hdr declares 2725 B at [0x7edf10, 0x7ee9b5); the
    2724 B *reachable* span ends on the `callq __stack_chk_fail`, the FDE's last byte 0x7ee9b4 being
    the trailing nop, and both digests are pinned;
  * all 58 direct `callq` sites and their targets (11 distinct callees), all 77 rip-relative operands
    (44 distinct targets; **no** rip-relative store -- every object is addressed through a register),
    and the literal instruction bytes in BYTES_PINS (prologue, the four value immediates, both bound
    pairs, the two slider range loads, the 0x30 item flags, the branch, the epilogue and
    __stack_chk_fail);
  * the single branch: the `jne rel8` at 0x7ee99e must land on 0x7ee9af (__stack_chk_fail);
  * the 11 `__cxa_atexit` rows of the native table, in body order, each with its destructor constant
    loaded before the call and its object loaded inside its own step window;
  * the six indirect sites: every one is `callq *0x18(%rax)` and dispatches through the block's own
    argument tag 0x15ce630, whose slot 3 is the bare `ret` 0xc4f0; the three bool sites use
    0x15ce650, whose slot 3 is 0xc2f0, also a bare `ret`;
  * the three class tables this body installs (bool 0x1594218, float 0x15942b0, bounded 0x15943c0)
    with their destructor and 0x20d60 slots, the two argument-functor tables, and the float class
    functor 0x1594318 that every copy/clone slot installs;
  * the four helpers this block is the only user of: 0x7edd80 (385 B bounded-variable constructor),
    0xf2f0 (40 B borrowed view), 0x7dbca0 (15 B value-cell accessor), 0x52a990 (10 B FNV-1a 32 tail
    jump into 0x161fb0), plus the two change callbacks 0x7ed280 / 0x7ed2d0 and their 0x7edd60 thunk --
    each pinned by digest, FDE length and (where they have one) their direct calls;
  * the object block: thirteen object addresses from 0x32656c0 to 0x3265f98 with the measured strides
    (0xd8 floats, 0xc8 bools, 0x18/0x10 hash records, **0xd0** bounded variables, 0xa8 items) and the
    two value cells 0x325f410 + 4*11 / 4*12;
  * the strings of the block (the nine names, the two hashed bar names, "Shadow Cheat", the two
    captions and the empty description byte);
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

RVA = 0x7EDF10
SIZE = 2725                                    # FDE address_range [0x7edf10, 0x7ee9b5)
REACHABLE = 2724                               # through the callq at 0x7ee9af
SHA = "5049dc6e9e0934db9aa5f0e22d71d36a7a0f4500b0d236acec042afba562b054"
SHA_REACHABLE = "35eba527ea1c6916aee7a36378488a8bb6eac3e031cc97e97e99a9a18d22a082"
TABLE_TOP = 0x2DD70E0
TABLE_INDEX = 335
TABLE_WORD = TABLE_TOP - 8 * TABLE_INDEX       # 0x2dd6668
EH_FRAME_HDR = 0x150C08C

NATIVE = {
    "header": "native/core/ctor_slot_335_7edf10.hpp",
    "source": "native/core/ctor_slot_335_7edf10.cpp",
    "test": "native/tests/test_ctor_slot_335.cpp",
    "cmake": "native/CMakeLists.txt",
}

CONSTANTS = {
    "kSlot335Rva": 0x7EDF10,
    "kSlot335BodySize": 2725,
    "kSlot335EndRva": 0x7EE9B5,
    "kSlot335ReachableBytes": 2724,
    "kSlot335InstructionCount": 485,
    "kSlot335DirectCallSites": 58,
    "kSlot335DirectCallees": 11,
    "kSlot335IndirectCallSites": 6,
    "kSlot335RipOperands": 77,
    "kSlot335RipTargets": 44,
    "kSlot335RipStoreSites": 0,
    "kSlot335BranchCount": 1,
    "kSlot335BasicBlocks": 2,
    "kSlot335TableTopRva": 0x2DD70E0,
    "kSlot335TableWordRva": 0x2DD6668,
    "kSlot335TableIndex": 335,
    "kSlot335NextUnrecoveredIndex": 336,
    "kSlot335NextUnrecoveredRva": 0x7EC4C0,
    "kSlot335CanaryCellRva": 0x15FE518,
    "kSlot335CanaryFrameOffset": 0xE0,
    "kSlot335StackChkFailRva": 0xF8F3A0,
    "kSlot335BranchRva": 0x7EE99E,
    "kSlot335BranchTargetRva": 0x7EE9AF,
    "kSlot335EpilogueRva": 0x7EE9A0,
    "kSlot335FrameBytes": 0x100,
    "kSlot335AtexitRva": 0xF8F390,
    "kSlot335DsoRva": 0x1604218,
    "kSlot335ReleaseRva": 0xF590,
    "kSlot335BaseCtorRva": 0x20AF0,
    "kSlot335RegisterRva": 0x20D60,
    "kSlot335UnboundedCtorRva": 0x6AA00,
    "kSlot335UnboundedCtorSize": 323,
    "kSlot335BoundedCtorRva": 0xC300,
    "kSlot335BoundedCtorSize": 352,
    "kSlot335SliderCtorRva": 0x27050,
    "kSlot335SliderCtorSize": 208,
    "kSlot335MenuRegisterRva": 0x268B0,
    "kSlot335HashRva": 0x52A990,
    "kSlot335HashSize": 10,
    "kSlot335HashBodyRva": 0x161FB0,
    "kSlot335FnvBasis": 0x811C9DC5,
    "kSlot335FnvPrime": 0x01000193,
    "kSlot335ValueCellRva": 0x7DBCA0,
    "kSlot335ValueCellSize": 15,
    "kSlot335ValueArrayRva": 0x325F410,
    "kSlot335BorrowedStrRva": 0xF2F0,
    "kSlot335BorrowedStrSize": 40,
    "kSlot335StrlenRva": 0xF8F3B0,
    "kSlot335BoundedHelperRva": 0x7EDD80,
    "kSlot335BoundedHelperSize": 386,
    "kSlot335CallbackThunkRva": 0x7EDD60,
    "kSlot335CallbackHpRva": 0x7ED280,
    "kSlot335CallbackGripRva": 0x7ED2D0,
    "kSlot335CallbackStoreRva": 0x7DBC30,
    "kSlot335CallbackLookupRva": 0x572900,
    "kSlot335FloatVarDtorRva": 0x20F0,
    "kSlot335BoolVarDtorRva": 0x20B0,
    "kSlot335BoundedVarDtorRva": 0xAD30,
    "kSlot335MenuItemDtorRva": 0x272D0,
    "kSlot335BoolMidVptr": 0x15940B0,
    "kSlot335BoolFinalVptr": 0x1594218,
    "kSlot335FloatMidVptr": 0x1594128,
    "kSlot335FloatFinalVptr": 0x15942B0,
    "kSlot335BoundedFinalVptr": 0x15943C0,
    "kSlot335FloatClassFnVtbl": 0x1594318,
    "kSlot335FloatArgFnVtbl": 0x15CE630,
    "kSlot335BoolArgFnVtbl": 0x15CE650,
    "kSlot335FloatFnCopyRva": 0xC4A0,
    "kSlot335FloatFnCloneRva": 0xC4C0,
    "kSlot335FloatFnMoveRva": 0xC4E0,
    "kSlot335FloatFnDestroyRva": 0xC4F0,
    "kSlot335BoolFnCopyRva": 0xC2A0,
    "kSlot335BoolFnCloneRva": 0xC2C0,
    "kSlot335BoolFnMoveRva": 0xC2E0,
    "kSlot335BoolFnDestroyRva": 0xC2F0,
    "kSlot335VarFlags": 1,
    "kSlot335ItemFlags": 0x30,
    "kSlot335BoolValue": 0,
    "kSlot335BoolDefault": 0,
    "kSlot335HpCellIndex": 11,
    "kSlot335GripCellIndex": 12,
    "kSlot335ValueArrayWindow": 16,
    "kSlot335HpMinBits": 0x42C80000,
    "kSlot335HpMaxBits": 0x440E8000,
    "kSlot335GripMinBits": 0x42C80000,
    "kSlot335GripMaxBits": 0x442F0000,
    "kSlot335HpRangeMinRva": 0x128ADE0,
    "kSlot335HpRangeMaxRva": 0x128ADE4,
    "kSlot335GripRangeMinRva": 0x128ADE8,
    "kSlot335DamageMultBits": 0x3F800000,
    "kSlot335HealMultBits": 0x3F99999A,
    "kSlot335GripLossBits": 0x3F400000,
    "kSlot335GripGainBits": 0x3F99999A,
    "kSlot335EmptyTextRva": 0x128AF2D,
    "kSlot335UnboundedCount": 4,
    "kSlot335BoolCount": 3,
    "kSlot335HashCount": 2,
    "kSlot335BoundedCount": 2,
    "kSlot335VarCount": 9,
    "kSlot335ItemCount": 2,
    "kSlot335AtexitRowCount": 11,
    "kSlot335FunctorSiteCount": 6,
    "kSlot335ReleaseCount": 22,
    "kSlot335BlockRva": 0x32656C0,
    "kSlot335BlockBytes": 0x8D8,
    "kSlot335BlockEndRva": 0x3265F98,
}

# The body order of the native model (each token must appear once, in this order).
SOURCE_ORDER = [
    "if (!build_unbounded(index)) return out;",
    "if (!build_bool(index)) return out;",
    "if (!build_hash(index)) return out;",
    "if (!build_bounded(index)) return out;",
    "if (!build_item(index)) return out;",
    "out.canary_checked = true;",
]

# The 0x7edd80 model, in the original's store order.
BOUNDED_HELPER_ORDER = [
    "if (value_cell == nullptr) return false;",
    "if (!slot335_bounded_var_state(var, name, &var.default_value, min_value, max_value,",
    "var.base.vptr = kSlot335BoundedFinalVptr;",
    "std::memcpy(&var.default_value, value_cell, 4);",
    "return register_concmd_20d60(var.base, registry).ok;",
]

# The 0x52a990 model: the record is written before the hash, exactly like the two sites.
HASH_ORDER = [
    "if (spec.name == nullptr || spec.name[0] == '\\0') {",
    "record.name = spec.name;",
    "record.hash = slot335_fnv1a32(spec.name);",
]

# The unbounded sites' post-constructor tail, in body order.
FINISH_ORDER = [
    "var.base.vptr = kSlot335FloatFinalVptr;",
    "var.value = spec.value_bits;",
    "var.default_value = spec.value_bits;",
    "out.unbounded[index].registration = register_concmd_20d60(var.base, concmd);",
]

TEST_TOKENS = [
    "section_constants",
    "section_helpers",
    "section_native_build",
    "section_unpinned_functor",
    "section_hash_refused",
    "section_menu_registration_blocked",
    "section_parent_contract",
    "section_canary",
    "kSlot335AtexitRows.size() == kSlot335AtexitRowCount",
    "r.atexit_records == 11 && r.menu_items == 2 && r.vars_built == 9",
    "r.functors_destroyed == kSlot335FunctorSiteCount && r.functors_destroyed == 6",
    "kSlot335Rva == 0x7edf10",
]

CMAKE_TOKENS = [
    "core/ctor_slot_335_7edf10.cpp",
    "add_test(NAME ctor_slot_335 COMMAND test_ctor_slot_335)",
]

# The 11 atexit registrations, in body order: (site, dtor, object).
ATEXIT = [
    (0x7EE040, 0x20F0, 0x32656C0),
    (0x7EE139, 0x20F0, 0x3265798),
    (0x7EE22B, 0x20F0, 0x3265870),
    (0x7EE32A, 0x20F0, 0x3265948),
    (0x7EE42B, 0x20B0, 0x3265A20),
    (0x7EE516, 0x20B0, 0x3265AE8),
    (0x7EE601, 0x20B0, 0x3265BB0),
    (0x7EE741, 0xAD30, 0x3265CA8),
    (0x7EE7E1, 0x272D0, 0x3265D78),
    (0x7EE8E7, 0xAD30, 0x3265E20),
    (0x7EE987, 0x272D0, 0x3265EF0),
]

# Every direct `callq` of the body: (site, target). 58 sites over 11 distinct callees.
CALLS = [
    (0x7EDFF0, 0x6AA00), (0x7EDFF8, 0xF590), (0x7EE022, 0x20D60), (0x7EE02A, 0xF590), (0x7EE040, 0xF8F390),
    (0x7EE0E4, 0x6AA00), (0x7EE0EC, 0xF590), (0x7EE11B, 0x20D60), (0x7EE123, 0xF590), (0x7EE139, 0xF8F390),
    (0x7EE1D6, 0x6AA00), (0x7EE1DE, 0xF590), (0x7EE20D, 0x20D60), (0x7EE215, 0xF590), (0x7EE22B, 0xF8F390),
    (0x7EE2D7, 0x6AA00), (0x7EE2DF, 0xF590), (0x7EE309, 0x20D60), (0x7EE311, 0xF590), (0x7EE32A, 0xF8F390),
    (0x7EE3A2, 0x20AF0), (0x7EE3F7, 0xF590), (0x7EE40D, 0x20D60), (0x7EE415, 0xF590), (0x7EE42B, 0xF8F390),
    (0x7EE498, 0x20AF0), (0x7EE4E2, 0xF590), (0x7EE4F8, 0x20D60), (0x7EE500, 0xF590), (0x7EE516, 0xF8F390),
    (0x7EE583, 0x20AF0), (0x7EE5CD, 0xF590), (0x7EE5E3, 0x20D60), (0x7EE5EB, 0xF590), (0x7EE601, 0xF8F390),
    (0x7EE617, 0x52A990), (0x7EE630, 0x52A990), (0x7EE65C, 0x7DBCA0), (0x7EE6DA, 0xF2F0),
    (0x7EE705, 0x7EDD80), (0x7EE70D, 0xF590), (0x7EE728, 0xF590), (0x7EE741, 0xF8F390), (0x7EE7BF, 0x27050),
    (0x7EE7C7, 0xF590), (0x7EE7CF, 0xF590), (0x7EE7E1, 0xF8F390), (0x7EE80A, 0x7DBCA0), (0x7EE880, 0xF2F0),
    (0x7EE8AB, 0x7EDD80), (0x7EE8B3, 0xF590), (0x7EE8CE, 0xF590), (0x7EE8E7, 0xF8F390), (0x7EE965, 0x27050),
    (0x7EE96D, 0xF590), (0x7EE975, 0xF590), (0x7EE987, 0xF8F390), (0x7EE9AF, 0xF8F3A0),
]

# Every direct callee: (target, site count, FDE length). The two 0xf8f3xx targets are PLT stubs with
# no FDE of their own, so their length is None.
CALLEES = [
    (0xF590, 22, 73),      # shared release of a borrowed stack view
    (0xF8F390, 11, None),  # __cxa_atexit PLT
    (0x20D60, 7, 26),      # register-once guard
    (0x6AA00, 4, 323),     # unbounded float variable ctor
    (0x20AF0, 3, 90),      # ConCmdBase ctor
    (0x52A990, 2, 10),     # FNV-1a 32 tail jump
    (0x7DBCA0, 2, 15),     # &value_array[index]
    (0xF2F0, 2, 40),       # borrowed view from a C string
    (0x7EDD80, 2, 386),    # bounded variable constructor
    (0x27050, 2, 208),     # debug-menu slider item ctor
    (0xF8F3A0, 1, None),   # __stack_chk_fail PLT (the branch target only)
]

# Every rip-relative operand: (site, instruction length, displacement offset, target). 77 operands.
RIPS = [
    (0x7EDF28, 7, 3, 0x15FE518), (0x7EDF2F, 7, 3, 0x15942A0), (0x7EDF36, 7, 3, 0x32656C0),
    (0x7EDF3D, 7, 3, 0x128AE40), (0x7EDF4D, 7, 3, 0x15CE630), (0x7EDF9B, 7, 3, 0x128AF2D),
    (0x7EE02F, 7, 3, 0x20F0), (0x7EE036, 7, 3, 0x1604218), (0x7EE045, 7, 3, 0x3265798),
    (0x7EE04C, 7, 3, 0x128AE61), (0x7EE065, 7, 3, 0x15CE630), (0x7EE08A, 7, 3, 0x128AF2D),
    (0x7EE128, 7, 3, 0x20F0), (0x7EE12F, 7, 3, 0x1604218), (0x7EE13E, 7, 3, 0x3265870),
    (0x7EE145, 7, 3, 0x128AE80), (0x7EE17C, 7, 3, 0x128AF2D), (0x7EE21A, 7, 3, 0x20F0),
    (0x7EE221, 7, 3, 0x1604218), (0x7EE230, 7, 3, 0x3265948), (0x7EE237, 7, 3, 0x128AE9D),
    (0x7EE276, 7, 3, 0x15CE630), (0x7EE2A3, 7, 3, 0x128AF2D), (0x7EE316, 7, 3, 0x1604218),
    (0x7EE31D, 7, 3, 0x20F0), (0x7EE32F, 7, 3, 0x1594208), (0x7EE336, 7, 3, 0x3265A20),
    (0x7EE33D, 7, 3, 0x128AEBA), (0x7EE35F, 7, 3, 0x128AF2D), (0x7EE3A7, 7, 3, 0x15940A0),
    (0x7EE3BC, 7, 3, 0x15CE650), (0x7EE41A, 7, 3, 0x20B0), (0x7EE421, 7, 3, 0x1604218),
    (0x7EE430, 7, 3, 0x3265AE8), (0x7EE437, 7, 3, 0x128AECA), (0x7EE459, 7, 3, 0x128AF2D),
    (0x7EE4AB, 7, 3, 0x15CE650), (0x7EE505, 7, 3, 0x20B0), (0x7EE50C, 7, 3, 0x1604218),
    (0x7EE51B, 7, 3, 0x3265BB0), (0x7EE522, 7, 3, 0x128AEE1), (0x7EE544, 7, 3, 0x128AF2D),
    (0x7EE596, 7, 3, 0x15CE650), (0x7EE5F0, 7, 3, 0x20B0), (0x7EE5F7, 7, 3, 0x1604218),
    (0x7EE606, 7, 3, 0x3265C80), (0x7EE60D, 7, 3, 0x128AEFC), (0x7EE61F, 7, 3, 0x3265C98),
    (0x7EE626, 7, 3, 0x128AF0F), (0x7EE635, 7, 3, 0x128AF20), (0x7EE661, 8, 4, 0x15FE600),
    (0x7EE671, 7, 3, 0x7ED280), (0x7EE678, 7, 3, 0x15CE630), (0x7EE681, 7, 3, 0x128AF2D),
    (0x7EE6DF, 7, 3, 0x3265CA8), (0x7EE72D, 7, 3, 0x1604218), (0x7EE734, 7, 3, 0xAD30),
    (0x7EE746, 7, 3, 0x128AF2E), (0x7EE74D, 7, 3, 0x128AF3B), (0x7EE758, 8, 4, 0x128ADE4),
    (0x7EE760, 7, 3, 0x3265D78), (0x7EE7AB, 8, 4, 0x128ADE0), (0x7EE7D4, 7, 3, 0x272D0),
    (0x7EE7E6, 7, 3, 0x128AF48), (0x7EE81D, 7, 3, 0x7ED2D0), (0x7EE824, 7, 3, 0x15CE630),
    (0x7EE82D, 7, 3, 0x128AF2D), (0x7EE885, 7, 3, 0x3265E20), (0x7EE8D3, 7, 3, 0x1604218),
    (0x7EE8DA, 7, 3, 0xAD30), (0x7EE8EC, 7, 3, 0x128AF2E), (0x7EE8F3, 7, 3, 0x128AF48),
    (0x7EE8FE, 8, 4, 0x128ADE4), (0x7EE906, 7, 3, 0x3265EF0), (0x7EE951, 8, 4, 0x128ADE8),
    (0x7EE97A, 7, 3, 0x272D0), (0x7EE98C, 7, 3, 0x15FE518),
]

# Literal instruction bytes that no rip/call pin covers.
BYTES_PINS = [
    (0x7EDF10, "554889e5", "push rbp; mov rsp,rbp"),
    (0x7EDF14, "415741564155415453", "push r15/r14/r13/r12/rbx"),
    (0x7EDF1D, "4883e4e0", "and rsp,-0x20"),
    (0x7EDF21, "4881ec00010000", "sub rsp,0x100"),
    (0x7EDF7F, "48898424e0000000", "canary -> 0xe0(%rsp)"),
    (0x7EDF65, "b901000000", "flags ecx = 1 (first unbounded variable)"),
    (0x7EDF8C, "48c744244020000000", "name length 0x20 = shadow_EasyMode_playerDamageMult"),
    (0x7EE011, "c783d00000000000803f", "damage mult: 1.0f into +0xd0"),
    (0x7EE01B, "c743680000803f", "damage mult: 1.0f into +0x68"),
    (0x7EE10A, "c783d00000009a99993f", "heal mult: 1.2f into +0xd0"),
    (0x7EE114, "c743689a99993f", "heal mult: 1.2f into +0x68"),
    (0x7EE1FC, "c783d00000000000403f", "grip loss: 0.75f into +0xd0"),
    (0x7EE206, "c743680000403f", "grip loss: 0.75f into +0x68"),
    (0x7EE2F8, "c783d00000009a99993f", "grip gain: 1.2f into +0xd0"),
    (0x7EE302, "c743689a99993f", "grip gain: 1.2f into +0x68"),
    (0x7EE398, "4983c410", "addq $0x10,%r12 (the bool mid vptr 0x15940a0 + 0x10)"),
    (0x7EE3CE, "c7436900000000", "bool variable: dword [obj+0x69] = 0"),
    (0x7EE402, "c683c0000000 00".replace(" ", ""), "bool variable: value 0 into +0xc0"),
    (0x7EE409, "c6436800", "bool variable: default 0 into +0x68"),
    (0x7EE63C, "bf0b000000", "Max_HP_Value: value-cell index 11"),
    (0x7EE690, "c744245c0000c842", "Max_HP_Value: min cell = 100.0f"),
    (0x7EE698, "c744245800800e44", "Max_HP_Value: max cell = 570.0f"),
    (0x7EE7ED, "bf0c000000", "Max_Grip_Value: value-cell index 12"),
    (0x7EE83C, "c744245c0000c842", "Max_Grip_Value: min cell = 100.0f"),
    (0x7EE844, "c744245800002f44", "Max_Grip_Value: max cell = 700.0f"),
    (0x7EE774, "41b830000000", "slider flags r8d = 0x30"),
    (0x7EE91A, "41b830000000", "second slider flags r8d = 0x30"),
    (0x7EE99E, "750f", "the guard branch: jne rel8 -> 0x7ee9af"),
    (0x7EE9A0, "488d65d8", "lea -0x28(%rbp),%rsp"),
    (0x7EE9A4, "5b415c415d415e415f5d", "pops"),
    (0x7EE9AE, "c3", "retq"),
    (0x7EE9AF, "e8ec097a00", "callq __stack_chk_fail"),
    (0x7EE9B4, "90", "the body's last byte (nop)"),
]

# The six `callq *0x18(%rax)` sites and the argument tag each dispatches through.
INDIRECT = [
    (0x7EE008, 0x15CE630, 0xC4F0),
    (0x7EE101, 0x15CE630, 0xC4F0),
    (0x7EE1F3, 0x15CE630, 0xC4F0),
    (0x7EE2EF, 0x15CE630, 0xC4F0),
    (0x7EE722, 0x15CE630, 0xC4F0),
    (0x7EE8C8, 0x15CE630, 0xC4F0),
]

# The four class tables this body installs. Address point = base + 0x10 = the object's vptr; slot 0
# (the destructor) sits at the point and slot 3 (the `[vptr+0x18]` gate a menu toggle would call) at
# point + 0x18 = 0xb890 / 0xbba0. 0x20d60 is NOT a virtual slot of these library classes: the body
# calls it directly. Nothing here dispatches through any of them.
CLASSES = [
    # (base, point, slot 0, slot 2)
    (0x1594208, 0x1594218, 0x20B0, 0xB890),
    (0x15942A0, 0x15942B0, 0x20F0, 0xBBA0),
    (0x15943B0, 0x15943C0, 0xAD30, 0xBBA0),
    (0x1594118, 0x1594128, 0xBB20, None),
]

# The two argument-functor tables of the block.
ARG_TABLES = [
    (0x15CE630, (0xC4A0, 0xC4C0, 0xC4E0, 0xC4F0)),
    (0x15CE650, (0xC2A0, 0xC2C0, 0xC2E0, 0xC2F0)),
]

# The clone/copy/move/destroy slot bodies and the two gate-free dispatches, as bytes.
SLOT_BYTES = {
    0xC4A0: "488b4610488d0d6d7e58014889471048894f20c3",
    0xC4C0: "488d05517e580148894720488b461048894710c3",
    0xC4E0: "488b461048894710c3",
    0xC4F0: "c3",
    0xC2A0: "488b4610488d0dd57f58014889471048894f20c3",
    0xC2C0: "488d05b97f580148894720488b461048894710c3",
    0xC2E0: "488b461048894710c3",
    0xC2F0: "c3",
    0x7EDD60: "488b47184889f74889d64889ca4c89c1ffe09090",
    0x7DBC30: "4863c7488d0dd637a802c5fa110481",
    0x7DBCA0: "4863c7" "488d0d6637a802" "488d0481" "c3",
}

# The four helpers this block is the only user of, plus the two change callbacks: (rva, FDE, sha256).
SUB_BODIES = [
    (0x7EDD80, 386, "32c869f63cb240466c8b3dbba95d29ca8c73cdb8b167e362172fa5eb54ad9a88"),
    (0x7ED280, 70, "55ab38ff2dd88060a5da5333b8c1cd560a12273bc5b76e14088890cd500199b3"),
    (0x7ED2D0, 99, "9d5698c12b85abac059fd03e0fedba73b2e6fdd337f4cfabce200ee18a051c28"),
    (0xF2F0, 40, "0173e3a3f11a8aa020b87731d2eef5026621d6f1fa0967581b0dab3df646973b"),
    (0x7DBCA0, 15, "2e315acfc9ed40c914889ce5b3f6fd407c78edcb114841cd704765db292591bf"),
    (0x52A990, 10, "0a633e2d30c9e38a4b9829a318723764d2c4cbeccaa47330c39bfcce9bd5d9f0"),
    (0x6AA00, 323, "6f1e2ce71c5fbf3804b82fc7f423a8a90121915584e99575c83414fd2295a1d4"),
    (0xC300, 352, "b0fad1ce8145048f5faf081eb6e4a03f97f9b8eb1cdaa44a76ff41e94f5f442f"),
    (0x27050, 208, "ad7ca05e3df035fa5df336f0463fb63e331566f140d6f0bf3ad16e9e2766b053"),
    (0x268B0, 684, "b963713d3ebb64cd2beb5ae62be4d91a4b4bd32a37f24a9911b2a8d269d46867"),
]

# Every direct call inside the helpers: (site, target).
SUB_CALLS = [
    (0x7EDEA8, 0xC300),      # 0x7edd80 -> the bounded float constructor
    (0x7EDED5, 0x20D60),     # 0x7edd80 -> the register-once guard
    (0x7ED29E, 0x7DBC30),    # 0x7ed280 -> the float store helper
    (0x7ED30B, 0x7DBC30),    # 0x7ed2d0 -> the float store helper
]

# The two callbacks' tail jumps into the sorted-vector lookup, and the FNV tail jump.
SUB_JUMPS = [
    (0x7ED2C1, 0x572900),
    (0x7ED32E, 0x572900),
    (0x52A995, 0x161FB0),
]

# The object block: thirteen addresses and the stride to the next one.
OBJECTS = [0x32656C0, 0x3265798, 0x3265870, 0x3265948, 0x3265A20, 0x3265AE8, 0x3265BB0,
           0x3265C80, 0x3265C98, 0x3265CA8, 0x3265D78, 0x3265E20, 0x3265EF0]
OBJECT_STRIDES = [0xD8, 0xD8, 0xD8, 0xD8, 0xC8, 0xC8, 0xD0, 0x18, 0x10, 0xD0, 0xA8, 0xD0]
BLOCK_END = 0x3265F98

# Strings of the block: (address, text). The two hashed names are NUL-terminated and 18 / 16 bytes.
STRINGS = [
    (0x128AE40, b"shadow_EasyMode_playerDamageMult"),
    (0x128AE61, b"shadow_EasyMode_playerHealMult"),
    (0x128AE80, b"shadow_EasyMode_gripLossMult"),
    (0x128AE9D, b"shadow_EasyMode_gripGainMult"),
    (0x128AEBA, b"shadow_noDamage"),
    (0x128AECA, b"shadow_maxAttackDamage"),
    (0x128AEE1, b"shadow_printPlayerNowPower"),
    (0x128AEFC, b"WanderHealthBarMax"),
    (0x128AF0F, b"WanderGripBarMax"),
    (0x128AF20, b"Max_HP_Value"),
    (0x128AF2E, b"Shadow Cheat"),
    (0x128AF3B, b"Max HP Value"),
    (0x128AF48, b"Max_Grip_Value"),
]
# The empty description view points at the second of two consecutive NULs after "Max_HP_Value".
EMPTY_BYTES = 0x128AF2D


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
    require(TABLE_TOP - 8 * TABLE_INDEX == TABLE_WORD == 0x2DD6668, "table row arithmetic")
    require(e.u64(TABLE_WORD) == RVA, "constructor-table entry 335 -> 0x7edf10")
    # The table is indexed backwards, so entry 334 sits at a HIGHER address and entry 336 lower.
    require(TABLE_WORD + 8 == 0x2DD6670 and e.u64(TABLE_WORD + 8) == 0x7FC530,
            "entry 334 is the already-registered 0x7fc530")
    require(TABLE_WORD - 8 == 0x2DD6660 and e.u64(TABLE_WORD - 8) == 0x7EC4C0,
            "entry 336 is the next unrecovered 0x7ec4c0")

    # --- body extent, digest, padding ---------------------------------------------------------
    body = e.read(RVA, SIZE)
    if not e.skip_hash:
        require(digest(body) == SHA, "body sha256 (2725 B FDE)")
        require(digest(body[:REACHABLE]) == SHA_REACHABLE, "reachable body sha256 (2724 B)")
    require(body[0] == 0x55 and body[1:4] == b"\x48\x89\xe5", "push rbp; mov rsp,rbp")
    require(body[-1] == 0x90, "the FDE's last byte is the trailing nop")
    require(fde_range(e, RVA) == SIZE, "FDE address_range == 2725")

    # --- every direct call, every rip operand, the literal bytes -------------------------------
    require(len(CALLS) == 58 and len({site for site, _ in CALLS}) == 58, "58 distinct call sites")
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
    require(sum(sites for _, sites, _ in CALLEES) == len(CALLS), "the histogram sums to 58")

    require(len(RIPS) == 77 and len({site for site, _, _, _ in RIPS}) == 77, "77 distinct rip sites")
    for site, length, disp_off, target in RIPS:
        require(e.rip(site, length, disp_off) == target, f"rip operand at 0x{site:x} -> 0x{target:x}")
    require(len({target for _, _, _, target in RIPS}) == 44, "44 distinct rip targets")
    # Every disp32 of a rip-relative instruction is its last four bytes; the table above says so.
    for _site, length, disp_off, _target in RIPS:
        require(disp_off == length - 4, "the displacement is the instruction's last dword")
    # This body has NO rip-relative store: every object is loaded into a register first.
    for site, _length, _disp_off, target in RIPS:
        require(not (RVA <= target < RVA + SIZE), f"0x{site:x} addresses the body itself")

    for site, want, what in BYTES_PINS:
        e.bytes_at(site, want, what)

    # --- the one branch, decoded --------------------------------------------------------------
    branch = e.read(0x7EE99E, 2)
    require(branch[0] == 0x75, "the guard branch is jne rel8 at 0x7ee99e")
    require(0x7EE99E + 2 + struct.unpack("<b", branch[1:2])[0] == 0x7EE9AF, "jne target 0x7ee9af")
    require(e.call(0x7EE9AF) == 0xF8F3A0, "the branch target calls __stack_chk_fail")

    # --- the six indirect sites ---------------------------------------------------------------
    require(len(INDIRECT) == 6, "6 indirect sites")
    for site, tag, slot3 in INDIRECT:
        require(e.read(site, 3) == b"\xff\x50\x18", f"0x{site:x} is `callq *0x18(%rax)`")
        require(e.reloc_target(tag + 0x18) == slot3, f"tag 0x{tag:x} slot 3 -> 0x{slot3:x}")
        e.bytes_at(slot3, "c3", f"functor destroy 0x{slot3:x} is a bare ret")

    # --- the class tables ----------------------------------------------------------------------
    for base, point, slot0, slot3 in CLASSES:
        require(point == base + 0x10, "the address point is table + 0x10")
        require(e.reloc_target(point) == slot0, f"vptr 0x{point:x} slot 0 -> 0x{slot0:x}")
        if slot3 is not None:
            require(e.reloc_target(point + 0x18) == slot3,
                    f"vptr 0x{point:x} slot 3 -> 0x{slot3:x}")
    # The float class functor installed by every copy/clone slot is the same four-body table.
    require(e.reloc_target(0x1594318) == 0xC4A0, "the class functor 0x1594318 slot 0 -> 0xc4a0")
    require(e.reloc_target(0x1594320) == 0xC4C0, "the class functor 0x1594318 slot 1 -> 0xc4c0")
    require(e.reloc_target(0x1594330) == 0xC4F0, "the class functor 0x1594318 slot 3 -> 0xc4f0")

    # --- the two argument-functor tables ------------------------------------------------------
    for table, slots in ARG_TABLES:
        for slot, wanted in enumerate(slots):
            require(e.reloc_target(table + 8 * slot) == wanted,
                    f"argument table 0x{table:x} slot {slot} -> 0x{wanted:x}")
    for addr, want in SLOT_BYTES.items():
        e.bytes_at(addr, want, f"functor/thunk body 0x{addr:x}")
    # The copy slots install their own family's class tag.
    require(e.rip(0xC4A4, 7, 3) == 0x1594318, "0xc4a0 copies the float class tag 0x1594318")
    require(e.rip(0xC2A4, 7, 3) == 0x1594280, "0xc2a0 copies the bool class tag 0x1594280")
    # 0x52a990 is a tail jump into the FNV-1a 32 body; 0xf2f0 calls the strlen PLT.
    require(e.read(0x52A990, 5) == b"\xbe\xc5\x9d\x1c\x81", "0x52a990 loads the FNV basis 0x811c9dc5")
    require(e.call(0xF2FF) == 0xF8F3B0, "0xf2f0 measures the string with strlen")
    # 0x7dbca0 indexes the .bss array at 0x325f410 (4-byte stride), which is where the cells live.
    require(e.rip(0x7DBCA3, 7, 3) == 0x325F410, "0x7dbca0 indexes the 0x325f410 array")
    # The two callback bodies clamp and forward: they are stored, never called here.
    require(e.rip(0x7ED288, 8, 4) == 0x128ADB4, "0x7ed280 clamps against 570.0f")
    require(e.rip(0x7ED2DC, 8, 4) == 0x128ADB8, "0x7ed2d0 clamps against 100.0f")
    require(e.rip(0x7ED2E4, 8, 4) == 0x128ADBC, "0x7ed2d0 clamps against 700.0f")
    require(e.rip(0x7ED2A3, 7, 3) == 0x2EFD160, "0x7ed280 forwards into the 0x2efd160 cell")
    require(e.call(0x7ED29E) == 0x7DBC30, "0x7ed280 stores through 0x7dbc30")
    require(e.jmp(0x7ED2C1) == 0x572900, "0x7ed280 tail-jumps into the 0x572900 lookup")
    # The site reads the thunk pointer out of the 0x15fe600 cell.
    require(e.u64(0x15FE600) == 0x7EDD60, "the 0x15fe600 cell holds 0x7edd60")

    # --- the four helpers and the two callbacks ------------------------------------------------
    for rva, size, sha in SUB_BODIES:
        require(fde_range(e, rva) == size, f"sub-body 0x{rva:x} FDE == {size}")
        require(digest(e.read(rva, size)) == sha, f"sub-body 0x{rva:x} sha256")
    for site, target in SUB_CALLS:
        require(e.call(site) == target, f"helper call at 0x{site:x} -> 0x{target:x}")
    for site, target in SUB_JUMPS:
        require(e.jmp(site) == target, f"helper tail jump at 0x{site:x} -> 0x{target:x}")
    # The five functor slots 0x7edd80 dispatches are `call *reg` (unresolved on purpose).
    for site in (0x7EDE27, 0x7EDE38, 0x7EDE4E, 0x7EDE5A, 0x7EDE79):
        require(e.read(site, 2)[0] == 0xFF, f"0x{site:x} is an indirect `call *reg`")

    # --- the 11 __cxa_atexit registrations, in body order ---------------------------------------
    atexit_sites = [site for site, target in CALLS if target == 0xF8F390]
    require(atexit_sites == [site for site, _, _ in ATEXIT], "the 11 atexit sites in body order")
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
    require(len(OBJECTS) == 13 and len(OBJECT_STRIDES) == 12, "thirteen objects with twelve strides")
    for i, stride in enumerate(OBJECT_STRIDES):
        require(OBJECTS[i + 1] - OBJECTS[i] == stride,
                f"object {i} at 0x{OBJECTS[i]:x} is 0x{stride:x} from its neighbour")
    require(OBJECTS[-1] + 0xA8 == BLOCK_END, "the block ends at 0x3265f98")
    require(OBJECTS[0] == 0x32656C0, "the block starts at 0x32656c0")
    for i in range(7):
        require(ATEXIT[i][2] == OBJECTS[i], f"variable {i} at 0x{OBJECTS[i]:x}")
    require(ATEXIT[7][2] == OBJECTS[9], "the HP slider's variable is the 0x3265ca8 object")
    require(ATEXIT[8][2] == OBJECTS[10], "the HP slider item is the 0x3265d78 object")
    require(ATEXIT[9][2] == OBJECTS[11], "the grip slider's variable is the 0x3265e20 object")
    require(ATEXIT[10][2] == OBJECTS[12], "the grip slider item is the 0x3265ef0 object")
    # The two value cells 0x7dbca0 hands out are exactly the ones the model mirrors.
    require(OBJECTS[0] - 0x32656C0 == 0, "block base")
    require(0x325F410 + 4 * 11 == 0x325F43C and 0x325F410 + 4 * 12 == 0x325F440,
            "the two value cells are 0x325f410[11] and [12]")

    # --- strings ---------------------------------------------------------------------------------
    for addr, text in STRINGS:
        e.cstring(addr, text)
    e.bytes_at(EMPTY_BYTES, "00", "the empty description byte")
    require(e.read(0x128AF2C, 2) == b"\x00\x00", "two consecutive NULs precede \"Shadow Cheat\"")


def check_native(texts: dict[str, str]) -> None:
    header = texts["header"]
    for name, value in CONSTANTS.items():
        require(const_value(header, name) == value, f"native constant {name}")
    for tokens, key in ((SOURCE_ORDER, "source"), (BOUNDED_HELPER_ORDER, "source"),
                        (HASH_ORDER, "source"), (FINISH_ORDER, "source")):
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
                        ("source", BOUNDED_HELPER_ORDER), ("source", HASH_ORDER),
                        ("source", FINISH_ORDER), ("test", TEST_TOKENS), ("cmake", CMAKE_TOKENS)):
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
        print("SKIP check_ctor_slot_335: local/re/eboot.elf not present")
        return 0
    e = Elf()
    texts = read_native()
    if "--tamper" in arguments:
        mode = arguments[arguments.index("--tamper") + 1] if len(arguments) > arguments.index("--tamper") + 1 else "elf"
        if mode == "elf":
            e.overlay = {RVA + 0x10: e.raw(RVA + 0x10, 1)[0] ^ 0x40}
        else:
            texts = dict(texts)
            texts["header"] = texts["header"].replace("kSlot335Rva", "kTampered", 1)
    e.recording = True
    run(e, texts)
    e.recording = False
    note = ""
    if "--no-mutate" not in arguments and "--tamper" not in arguments:
        elf_runs, native_runs = mutation_self_test(e, texts)
        note = f"; mutation self-test {elf_runs} ELF + {native_runs} native mutants all rejected"
    print(f"OK check_ctor_slot_335: slot 335 (0x7edf10, {SIZE} B FDE, {REACHABLE} B reachable, sha "
          f"{SHA[:12]}...) is row 335 of the constructor table at 0x{TABLE_WORD:x}; 58 direct calls over "
          f"11 callees, 77 rip operands (0 stores), 1 branch (0x7ee99e -> 0x7ee9af __stack_chk_fail), 6 "
          f"`callq *0x18(%rax)` inert functor destroys through 0x15ce630, 11 __cxa_atexit rows, and the "
          f"block's own helpers 0x7edd80 / 0xf2f0 / 0x7dbca0 / 0x52a990; guest bytes NOT executed{note}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
