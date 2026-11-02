#!/usr/bin/env python3
"""ELF tie for startup constructor index 326 (RVA 0x837f30, 7055 B FDE): the Colossus / Shadow
eye-colour debug page.

Offline and read-only. Bytes of the pinned private ELF (`local/re/eboot.elf`, sha256 d0723afb...5861d3)
are read through `slot_pin_util.Elf`; nothing is loaded, relocated or executed. Skips cleanly (exit 0)
when the private inputs are absent.

What is pinned here (everything is a byte read of the pinned image):

  * the constructor-table word: entry 326 of the backwards table is the u64 at
    0x2dd70e0 - 326*8 = 0x2dd66b0 and must hold 0x837f30. The task brief quoted 0x2dd65d0, which is
    entry **354** (0x79e180); the checker pins the arithmetic, both registered neighbours (325 =
    0x839f00 at 0x2dd66b8, 327 = 0x836330 at 0x2dd66a8) and the fact that 0x2dd70e0 - 0x2dd65d0 =
    354*8, so that reading cannot come back;
  * the body extent and digest: the FDE at .eh_frame_hdr declares 7055 B at [0x837f30, 0x839abf); the
    7054 B *reachable* span ends on the trailing nop at 0x839abe, and both digests are pinned;
  * all 159 direct `callq` sites and their targets (10 distinct callees), all 263 rip-relative
    operands (50 stores over 45 distinct addresses, 113 distinct targets), and the literal instruction
    bytes listed in BYTES_PINS (prologue, the two bound immediates, the 3.0f value/default stores, the
    0x30 item flags, the 0x26800 kind stores, both branches, the epilogue and __stack_chk_fail);
  * the single branch: the `jne rel8` at 0x839aa8 must land on 0x839ab9 (__stack_chk_fail);
  * the 36 `__cxa_atexit` rows of the native table, in body order, each with its object loaded inside
    its own step window and its destructor constant loaded before the call;
  * the thirteen indirect sites: every one is `callq *0x18(%rax)` and dispatches through the argument
    tag 0x15cec00, whose slot 3 is 0xc4f0, one `c3`;
  * the argument-functor tables 0x15cebe0 / 0x15cec00 (four slot bodies each, byte-identical to
    0x1594318's) and the class tables 0x1594218 / 0x15942b0 / 0x15943c0 with their destructor,
    0x20d60 and gate slots;
  * the two 0x26ea0 gate classes: 0x1594218 slot 3 = 0xb890 -> 1, slot 6 = 0xb930 -> 0; 0x15942b0
    slot 3 = 0xbba0 -> 1, slot 6 = 0xbbf0 -> 3 (so a toggle on a float variable narrows its range);
  * the 0x26800 kind-0 "MenuSeparator" row (173 B): digest, its two direct calls, its rip operand, and
    the kind 3 -> 0 pair of stores;
  * the twelve floats of the 0x2dce400 colour table the eye-colour variables point their value cells
    at, and the twelve original object addresses with their 0xd0 stride;
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

RVA = 0x837F30
SIZE = 7055                                    # FDE address_range [0x837f30, 0x839abf)
REACHABLE = 7054                               # the last reached byte is the trailing nop
SHA = "ac6a7fbd92619ee108317392a840e15895a28d9a4962f108f76260c1cef10261"
SHA_REACHABLE = "aef55b758612ca5e9e492338a759683d2a73c5257b08c4942513c6f0de170dfd"
TABLE_TOP = 0x2DD70E0
TABLE_INDEX = 326
TABLE_WORD = TABLE_TOP - 8 * TABLE_INDEX       # 0x2dd66b0
TABLE_WORD_QUOTED_IN_BRIEF = 0x2DD65D0         # entry 354, not 326
EH_FRAME_HDR = 0x150C08C

# The 0x26800 kind-0 label / "MenuSeparator" ctor.
LABEL_CTOR = 0x26800
LABEL_CTOR_SIZE = 173
LABEL_CTOR_SHA = "f5414aca23ef4f949b7ffdcc250406dd8ad8a952a1e440e1114289a90f2f5018"

NATIVE = {
    "header": "native/core/ctor_slot_326_837f30.hpp",
    "source": "native/core/ctor_slot_326_837f30.cpp",
    "test": "native/tests/test_ctor_slot_326.cpp",
    "cmake": "native/CMakeLists.txt",
}

CONSTANTS = {
    "kSlot326Rva": 0x837F30,
    "kSlot326BodySize": 7055,
    "kSlot326EndRva": 0x839ABF,
    "kSlot326ReachableBytes": 7054,
    "kSlot326InstructionCount": 1306,
    "kSlot326DirectCallSites": 159,
    "kSlot326DirectCallees": 10,
    "kSlot326IndirectCallSites": 13,
    "kSlot326RipOperands": 263,
    "kSlot326RipTargets": 113,
    "kSlot326StoreSites": 50,
    "kSlot326StoreTargets": 45,
    "kSlot326BranchCount": 1,
    "kSlot326BasicBlocks": 2,
    "kSlot326TableTopRva": 0x2DD70E0,
    "kSlot326TableWordRva": 0x2DD66B0,
    "kSlot326TableIndex": 326,
    "kSlot326NextUnrecoveredIndex": 335,
    "kSlot326NextUnrecoveredRva": 0x7EDF10,
    "kSlot326CanaryCellRva": 0x15FE518,
    "kSlot326CanaryFrameOffset": 0xC0,
    "kSlot326StackChkFailRva": 0xF8F3A0,
    "kSlot326BranchRva": 0x839AA8,
    "kSlot326BranchTargetRva": 0x839AB9,
    "kSlot326EpilogueRva": 0x839AAA,
    "kSlot326FrameBytes": 0xE0,
    "kSlot326AtexitRva": 0xF8F390,
    "kSlot326DsoRva": 0x1604218,
    "kSlot326ReleaseRva": 0xF590,
    "kSlot326ConCmdBaseCtorRva": 0x20AF0,
    "kSlot326ConCmdRegisterRva": 0x20D60,
    "kSlot326FloatVarCtorRva": 0xC300,
    "kSlot326FloatVarCtorSize": 0x160,
    "kSlot326MenuGroupCtorRva": 0x26C10,
    "kSlot326MenuToggleCtorRva": 0x26EA0,
    "kSlot326MenuSliderCtorRva": 0x27050,
    "kSlot326MenuRegisterRva": 0x268B0,
    "kSlot326LabelItemCtorRva": 0x26800,
    "kSlot326LabelItemCtorSize": 173,
    "kSlot326LabelItemCtorEndRva": 0x268AD,
    "kSlot326SeparatorTextRva": 0xF96530,
    "kSlot326EmptyViewCtorRva": 0xF180,
    "kSlot326SharedStrCtorRva": 0xF1A0,
    "kSlot326BoolVarDtorRva": 0x20B0,
    "kSlot326FloatVarDtorRva": 0x20F0,
    "kSlot326EyeVarDtorRva": 0xAD30,
    "kSlot326MenuItemDtorRva": 0x272D0,
    "kSlot326BoolMidVptr": 0x15940B0,
    "kSlot326BoolFinalVptr": 0x1594218,
    "kSlot326FloatMidVptr": 0x1594128,
    "kSlot326FloatFinalVptr": 0x15942B0,
    "kSlot326EyeFinalVptr": 0x15943C0,
    "kSlot326BoolArgFnVtbl": 0x15CEBE0,
    "kSlot326FloatArgFnVtbl": 0x15CEC00,
    "kSlot326FloatFnCopyRva": 0xC4A0,
    "kSlot326FloatFnCloneRva": 0xC4C0,
    "kSlot326FloatFnMoveRva": 0xC4E0,
    "kSlot326FloatFnDestroyRva": 0xC4F0,
    "kSlot326BoolGate3Rva": 0xB890,
    "kSlot326BoolGate6Rva": 0xB930,
    "kSlot326FloatGate3Rva": 0xBBA0,
    "kSlot326FloatGate6Rva": 0xBBF0,
    "kSlot326BoolGate3": 1,
    "kSlot326BoolGate6": 0,
    "kSlot326FloatGate3": 1,
    "kSlot326FloatGate6": 3,
    "kSlot326VarFlags": 1,
    "kSlot326ItemFlags": 0x30,
    "kSlot326LabelItemFlags": 0,
    "kSlot326ThresholdMinBits": 0x00000000,
    "kSlot326ThresholdMaxBits": 0x7F7FFFFF,
    "kSlot326EyeMinBits": 0x00000000,
    "kSlot326EyeMaxBits": 0x40000000,
    "kSlot326ThresholdValueBits": 0x40400000,
    "kSlot326SliderMinBits": 0x3C23D70A,
    "kSlot326SliderMaxBits": 0x3DCCCCCD,
    "kSlot326GroupKind": 1,
    "kSlot326ItemKind": 2,
    "kSlot326LabelKind": 0,
    "kSlot326LabelTransientKind": 3,
    "kSlot326EmptyTextRva": 0x128E348,
    "kSlot326BoolCount": 2,
    "kSlot326FloatCount": 13,
    "kSlot326EyeCount": 12,
    "kSlot326ItemCount": 21,
    "kSlot326AtexitRowCount": 36,
    "kSlot326FunctorSiteCount": 13,
    "kSlot326EyeValueTableRva": 0x2DCE400,
    "kSlot326BlockRva": 0x3291890,
    "kSlot326BlockBytes": 0x19F0,
}

# The body order of the native model (each token must appear once, in this order).
SOURCE_ORDER = [
    "eye_values = kSlot326EyeValueBits;",
    "if (!build_bool(0)) return out;",
    "if (!build_bool(1)) return out;",
    "if (!build_float(0)) return out;",
    "for (std::uint32_t index = 0; index < 4; ++index) {",
    "for (std::uint32_t index = 1; index < kSlot326FloatCount; ++index) {",
    "for (std::uint32_t index = 4; index < kSlot326ItemCount; ++index) {",
    "out.canary_checked = true;",
]

# The store order inside the shared 0xc300 net state (the same anchors the c300 body carries).
C300_ORDER = [
    "var.base.vptr = kRangedFloatMidVptr;",
    "var.value_ref = value_ref;",
    "var.min_value[0] = 0;",
    "var.min_engaged = 1;",
    "var.max_value[0] = 0;",
    "var.max_engaged = 1;",
    "std::memset(var.args, 0, sizeof var.args);",
    "var.fn_vptr = kRangedFloatFnVtbl;",
    "install_float_fn_from_args(var, args, kRangedFloatFnVtbl);",
]

LABEL_ORDER = [
    "if (!init_shared_str_f1a0(text, kSlot326SeparatorText, hooks)) return false;",
    "item.link.self = nullptr;",
    "item.label = label;",
    "concmd_acquire_view(label);",
    "item.kind = kSlot326LabelTransientKind;",
    "item.text = text;",
    "init_slot_f180(&item.empty);",
    "item.target = nullptr;",
    "item.range_min = kSlot326LabelRangeMin;",
    "item.range_max = kSlot326LabelRangeMax;",
    "item.flags = flags;",
    "item.kind = kSlot326LabelKind;",
]

TEST_TOKENS = [
    "section_constants",
    "section_functor_and_target",
    "section_float_shapes",
    "section_label_item",
    "section_native_build",
    "section_no_allocator",
    "section_menu_registration_blocked",
    "section_parent_contract",
    "section_canary",
    "kSlot326AtexitRows.size() == kSlot326AtexitRowCount",
    "r.atexit_records == 36 && r.functors_destroyed == 13",
    "blocked == Slot326Stage::item_ctor && r.blocked_index == 5",
    "sizeof(Slot326EyeVar) == 0xd0",
]

CMAKE_TOKENS = [
    "core/ctor_slot_326_837f30.cpp",
    "add_test(NAME ctor_slot_326 COMMAND test_ctor_slot_326)",
]

# The 36 atexit registrations, in body order: (site, dtor, object).
ATEXIT = [
    (0x838062, 0x20B0, 0x3291890),
    (0x83815C, 0x20B0, 0x3291958),
    (0x83827B, 0x20F0, 0x3291A20),
    (0x838302, 0x272D0, 0x3291AF8),
    (0x838386, 0x272D0, 0x3291BA0),
    (0x838410, 0x272D0, 0x3291C48),
    (0x838494, 0x272D0, 0x3291CF0),
    (0x8385A5, 0xAD30, 0x3291D98),
    (0x8386B2, 0xAD30, 0x3291E68),
    (0x8387BD, 0xAD30, 0x3291F38),
    (0x8388C7, 0xAD30, 0x3292008),
    (0x8389DC, 0xAD30, 0x32920D8),
    (0x838AF2, 0xAD30, 0x32921A8),
    (0x838C03, 0xAD30, 0x3292278),
    (0x838D16, 0xAD30, 0x3292348),
    (0x838E28, 0xAD30, 0x3292418),
    (0x838F42, 0xAD30, 0x32924E8),
    (0x839053, 0xAD30, 0x32925B8),
    (0x839162, 0xAD30, 0x3292688),
    (0x8391E5, 0x272D0, 0x3292758),
    (0x83922E, 0x272D0, 0x3292800),
    (0x8392A7, 0x272D0, 0x32928A8),
    (0x839338, 0x272D0, 0x3292950),
    (0x8393C9, 0x272D0, 0x32929F8),
    (0x83945D, 0x272D0, 0x3292AA0),
    (0x8394F1, 0x272D0, 0x3292B48),
    (0x839571, 0x272D0, 0x3292BF0),
    (0x839605, 0x272D0, 0x3292C98),
    (0x839699, 0x272D0, 0x3292D40),
    (0x83972D, 0x272D0, 0x3292DE8),
    (0x8397C1, 0x272D0, 0x3292E90),
    (0x839841, 0x272D0, 0x3292F38),
    (0x8398D5, 0x272D0, 0x3292FE0),
    (0x839969, 0x272D0, 0x3293088),
    (0x8399FD, 0x272D0, 0x3293130),
    (0x839A91, 0x272D0, 0x32931D8),
]

# Every direct `callq` of the body: (site, target). 159 sites over 10 distinct callees.
CALLS = [
    (0x837FC5, 0x20AF0), (0x838027, 0xF590), (0x838044, 0x20D60), (0x83804C, 0xF590),
    (0x838062, 0xF8F390), (0x8380C7, 0x20AF0), (0x83811E, 0xF590), (0x83813B, 0x20D60),
    (0x838143, 0xF590), (0x83815C, 0xF8F390), (0x83821F, 0xC300), (0x83822B, 0xF590),
    (0x838261, 0x20D60), (0x838269, 0xF590), (0x83827B, 0xF8F390), (0x8382D9, 0x26C10),
    (0x8382E1, 0xF590), (0x8382E9, 0xF590), (0x838302, 0xF8F390), (0x838361, 0x26EA0),
    (0x838369, 0xF590), (0x838371, 0xF590), (0x838386, 0xF8F390), (0x8383EC, 0x26EA0),
    (0x8383F4, 0xF590), (0x8383FC, 0xF590), (0x838410, 0xF8F390), (0x838476, 0x26EA0),
    (0x83847E, 0xF590), (0x838486, 0xF590), (0x838494, 0xF8F390), (0x838548, 0xC300),
    (0x838554, 0xF590), (0x838585, 0x20D60), (0x83858F, 0xF590), (0x8385A5, 0xF8F390),
    (0x838653, 0xC300), (0x83865F, 0xF590), (0x838694, 0x20D60), (0x83869C, 0xF590),
    (0x8386B2, 0xF8F390), (0x838765, 0xC300), (0x838771, 0xF590), (0x83879C, 0x20D60),
    (0x8387A4, 0xF590), (0x8387BD, 0xF8F390), (0x83886F, 0xC300), (0x83887B, 0xF590),
    (0x8388AD, 0x20D60), (0x8388B5, 0xF590), (0x8388C7, 0xF8F390), (0x83897C, 0xC300),
    (0x838988, 0xF590), (0x8389BA, 0x20D60), (0x8389C2, 0xF590), (0x8389DC, 0xF8F390),
    (0x838A96, 0xC300), (0x838AA2, 0xF590), (0x838AD4, 0x20D60), (0x838ADC, 0xF590),
    (0x838AF2, 0xF8F390), (0x838BA4, 0xC300), (0x838BB0, 0xF590), (0x838BE2, 0x20D60),
    (0x838BEA, 0xF590), (0x838C03, 0xF8F390), (0x838CC0, 0xC300), (0x838CCC, 0xF590),
    (0x838CF8, 0x20D60), (0x838D00, 0xF590), (0x838D16, 0xF8F390), (0x838DCF, 0xC300),
    (0x838DDB, 0xF590), (0x838E07, 0x20D60), (0x838E0F, 0xF590), (0x838E28, 0xF8F390),
    (0x838EE2, 0xC300), (0x838EEE, 0xF590), (0x838F21, 0x20D60), (0x838F29, 0xF590),
    (0x838F42, 0xF8F390), (0x838FF4, 0xC300), (0x839000, 0xF590), (0x839035, 0x20D60),
    (0x83903D, 0xF590), (0x839053, 0xF8F390), (0x839109, 0xC300), (0x839115, 0xF590),
    (0x839141, 0x20D60), (0x839149, 0xF590), (0x839162, 0xF8F390), (0x8391C0, 0x26C10),
    (0x8391C8, 0xF590), (0x8391D0, 0xF590), (0x8391E5, 0xF8F390), (0x839218, 0x26800),
    (0x839220, 0xF590), (0x83922E, 0xF8F390), (0x839285, 0x26C10), (0x83928D, 0xF590),
    (0x839295, 0xF590), (0x8392A7, 0xF8F390), (0x839316, 0x27050), (0x83931E, 0xF590),
    (0x839326, 0xF590), (0x839338, 0xF8F390), (0x8393A7, 0x27050), (0x8393AF, 0xF590),
    (0x8393B7, 0xF590), (0x8393C9, 0xF8F390), (0x839438, 0x27050), (0x839440, 0xF590),
    (0x839448, 0xF590), (0x83945D, 0xF8F390), (0x8394D3, 0x27050), (0x8394DB, 0xF590),
    (0x8394E3, 0xF590), (0x8394F1, 0xF8F390), (0x83954F, 0x26C10), (0x839557, 0xF590),
    (0x83955F, 0xF590), (0x839571, 0xF8F390), (0x8395E0, 0x27050), (0x8395E8, 0xF590),
    (0x8395F0, 0xF590), (0x839605, 0xF8F390), (0x83967B, 0x27050), (0x839683, 0xF590),
    (0x83968B, 0xF590), (0x839699, 0xF8F390), (0x83970F, 0x27050), (0x839717, 0xF590),
    (0x83971F, 0xF590), (0x83972D, 0xF8F390), (0x8397A3, 0x27050), (0x8397AB, 0xF590),
    (0x8397B3, 0xF590), (0x8397C1, 0xF8F390), (0x83981F, 0x26C10), (0x839827, 0xF590),
    (0x83982F, 0xF590), (0x839841, 0xF8F390), (0x8398B0, 0x27050), (0x8398B8, 0xF590),
    (0x8398C0, 0xF590), (0x8398D5, 0xF8F390), (0x83994B, 0x27050), (0x839953, 0xF590),
    (0x83995B, 0xF590), (0x839969, 0xF8F390), (0x8399DF, 0x27050), (0x8399E7, 0xF590),
    (0x8399EF, 0xF590), (0x8399FD, 0xF8F390), (0x839A73, 0x27050), (0x839A7B, 0xF590),
    (0x839A83, 0xF590), (0x839A91, 0xF8F390), (0x839AB9, 0xF8F3A0),
]

# Every rip-relative operand: (site, instruction length, displacement offset, target). 263 operands.
RIPS = [
    (0x837F48, 7, 3, 0x15FE518), (0x837F4F, 7, 3, 0x1594208), (0x837F56, 7, 3, 0x3291890),
    (0x837F5D, 7, 3, 0x128E170), (0x837F64, 7, 3, 0x128E348), (0x837FB5, 7, 3, 0x3291890),
    (0x837FCA, 7, 3, 0x15940A0), (0x837FD4, 7, 3, 0x3291950), (0x837FDF, 7, 3, 0x15CEBE0),
    (0x837FEA, 7, 3, 0x3291890), (0x837FF1, 7, 3, 0x32918F0), (0x837FF8, 10, 2, 0x32918F9),
    (0x838002, 8, 4, 0x3291900), (0x83800A, 11, 3, 0x3291930), (0x838015, 7, 3, 0x3291940),
    (0x83801C, 11, 3, 0x3291948), (0x83802F, 7, 3, 0x3291890), (0x838036, 7, 2, 0x3291950),
    (0x83803D, 7, 2, 0x32918F8), (0x838051, 7, 3, 0x20B0), (0x838058, 7, 3, 0x1604218),
    (0x838067, 7, 3, 0x128E348), (0x838078, 7, 3, 0x128E185), (0x83807F, 7, 3, 0x3291958),
    (0x83808F, 7, 3, 0x3291958), (0x8380CF, 7, 3, 0x3291A18), (0x8380DA, 7, 3, 0x15CEBE0),
    (0x8380E1, 7, 3, 0x3291958), (0x8380E8, 7, 3, 0x32919B8), (0x8380EF, 10, 2, 0x32919C1),
    (0x8380F9, 8, 4, 0x32919C8), (0x838101, 11, 3, 0x32919F8), (0x83810C, 7, 3, 0x3291A08),
    (0x838113, 11, 3, 0x3291A10), (0x838126, 7, 3, 0x3291958), (0x83812D, 7, 2, 0x3291A18),
    (0x838134, 7, 2, 0x32919C0), (0x838148, 7, 3, 0x1604218), (0x83814F, 7, 3, 0x20B0),
    (0x838161, 7, 3, 0x15942A0), (0x838168, 7, 3, 0x128E198), (0x838173, 7, 3, 0x3291A20),
    (0x83819F, 7, 3, 0x3291AF0), (0x8381BB, 7, 3, 0x15CEC00), (0x8381E5, 7, 3, 0x128E348),
    (0x838210, 7, 3, 0x3291A20), (0x838246, 7, 3, 0x3291A20), (0x83824D, 10, 2, 0x3291AF0),
    (0x838257, 10, 2, 0x3291A88), (0x83826E, 7, 3, 0x20F0), (0x838280, 7, 3, 0x3291AF8),
    (0x838291, 7, 3, 0x128E1B7), (0x838298, 7, 3, 0x128E1BC), (0x8382EE, 7, 3, 0x272D0),
    (0x8382F5, 7, 3, 0x1604218), (0x838307, 7, 3, 0x3291BA0), (0x838318, 7, 3, 0x128E1C5),
    (0x838329, 7, 3, 0x3291958), (0x838376, 7, 3, 0x1604218), (0x83838B, 7, 3, 0x128E1D6),
    (0x838392, 7, 3, 0x3291C48), (0x8383A3, 7, 3, 0x128E1BC), (0x8383C2, 7, 3, 0x3291890),
    (0x838415, 7, 3, 0x128E1E6), (0x83841C, 7, 3, 0x3291CF0), (0x83842D, 7, 3, 0x128E1BC),
    (0x83844C, 7, 3, 0x3291A20), (0x83849D, 7, 3, 0x15CEC00), (0x8384B0, 7, 3, 0x128E217),
    (0x8384B7, 7, 3, 0x3291D98), (0x8384FD, 7, 3, 0x2DCE400), (0x838509, 7, 3, 0x128E348),
    (0x838567, 7, 3, 0x15943B0), (0x838578, 7, 3, 0x3291D98), (0x83857F, 6, 2, 0x3291E00),
    (0x838594, 7, 3, 0xAD30), (0x83859B, 7, 3, 0x1604218), (0x8385AA, 7, 3, 0x128E229),
    (0x8385B1, 7, 3, 0x128E348), (0x8385C5, 7, 3, 0x15CEC00), (0x8385CC, 7, 3, 0x3291E68),
    (0x838677, 7, 3, 0x2DCE400), (0x838681, 7, 3, 0x3291E68), (0x83868E, 6, 2, 0x3291ED0),
    (0x8386A1, 7, 3, 0xAD30), (0x8386A8, 7, 3, 0x1604218), (0x8386B7, 7, 3, 0x128E23B),
    (0x8386BE, 7, 3, 0x3291F38), (0x8386D2, 7, 3, 0x15CEC00), (0x83870E, 7, 3, 0x128E348),
    (0x83878F, 7, 3, 0x3291F38), (0x838796, 6, 2, 0x3291FA0), (0x8387A9, 7, 3, 0x1604218),
    (0x8387B0, 7, 3, 0xAD30), (0x8387C2, 7, 3, 0x128E24D), (0x8387C9, 7, 3, 0x3292008),
    (0x8387DD, 7, 3, 0x15CEC00), (0x838819, 7, 3, 0x128E348), (0x838893, 7, 3, 0x2DCE400),
    (0x83889D, 7, 3, 0x3292008), (0x8388A7, 6, 2, 0x3292070), (0x8388BA, 7, 3, 0xAD30),
    (0x8388CC, 7, 3, 0x2DCE410), (0x8388D3, 7, 3, 0x128E25F), (0x838903, 7, 3, 0x32920D8),
    (0x838924, 7, 3, 0x15CEC00), (0x83894B, 7, 3, 0x128E348), (0x8389A3, 7, 3, 0x32920D8),
    (0x8389AA, 7, 3, 0x32920D8), (0x8389B4, 6, 2, 0x3292140), (0x8389C7, 7, 3, 0xAD30),
    (0x8389CE, 7, 3, 0x1604218), (0x8389D5, 7, 3, 0x32920D8), (0x8389E1, 7, 3, 0x128E26D),
    (0x8389EC, 7, 3, 0x32921A8), (0x838A00, 7, 3, 0x15CEC00), (0x838A40, 7, 3, 0x128E348),
    (0x838AB5, 7, 3, 0x2DCE410), (0x838ABF, 7, 3, 0x32921A8), (0x838ACE, 6, 2, 0x3292210),
    (0x838AE1, 7, 3, 0xAD30), (0x838AE8, 7, 3, 0x1604218), (0x838AF7, 7, 3, 0x128E27B),
    (0x838AFE, 7, 3, 0x3292278), (0x838B12, 7, 3, 0x15CEC00), (0x838B4E, 7, 3, 0x128E348),
    (0x838BC8, 7, 3, 0x2DCE410), (0x838BD2, 7, 3, 0x3292278), (0x838BDC, 6, 2, 0x32922E0),
    (0x838BEF, 7, 3, 0x1604218), (0x838BF6, 7, 3, 0xAD30), (0x838C08, 7, 3, 0x128E289),
    (0x838C12, 7, 3, 0x3292348), (0x838C26, 7, 3, 0x15CEC00), (0x838C66, 7, 3, 0x128E348),
    (0x838CEB, 7, 3, 0x3292348), (0x838CF2, 6, 2, 0x32923B0), (0x838D05, 7, 3, 0xAD30),
    (0x838D0C, 7, 3, 0x1604218), (0x838D1B, 7, 3, 0x2DCE420), (0x838D22, 7, 3, 0x128E297),
    (0x838D39, 7, 3, 0x3292418), (0x838D73, 7, 3, 0x15CEC00), (0x838D9D, 7, 3, 0x128E348),
    (0x838DFA, 7, 3, 0x3292418), (0x838E01, 6, 2, 0x3292480), (0x838E14, 7, 3, 0x1604218),
    (0x838E1B, 7, 3, 0xAD30), (0x838E2D, 7, 3, 0x128E2A4), (0x838E34, 7, 3, 0x32924E8),
    (0x838E48, 7, 3, 0x15CEC00), (0x838E8C, 7, 3, 0x128E348), (0x838F01, 7, 3, 0x2DCE420),
    (0x838F14, 7, 3, 0x32924E8), (0x838F1B, 6, 2, 0x3292550), (0x838F2E, 7, 3, 0xAD30),
    (0x838F35, 7, 3, 0x1604218), (0x838F47, 7, 3, 0x128E2B1), (0x838F52, 7, 3, 0x32925B8),
    (0x838F66, 7, 3, 0x15CEC00), (0x838F9E, 7, 3, 0x128E348), (0x839018, 7, 3, 0x2DCE420),
    (0x839025, 7, 3, 0x32925B8), (0x83902F, 6, 2, 0x3292620), (0x839042, 7, 3, 0xAD30),
    (0x839049, 7, 3, 0x1604218), (0x839058, 7, 3, 0x128E2BE), (0x83906C, 7, 3, 0x15CEC00),
    (0x83907A, 7, 3, 0x3292688), (0x8390B2, 7, 3, 0x128E348), (0x839134, 7, 3, 0x3292688),
    (0x83913B, 6, 2, 0x32926F0), (0x83914E, 7, 3, 0x1604218), (0x839155, 7, 3, 0xAD30),
    (0x839167, 7, 3, 0x3292758), (0x839178, 7, 3, 0x128E2CB), (0x83917F, 7, 3, 0x128E2D2),
    (0x8391D5, 7, 3, 0x272D0), (0x8391EA, 7, 3, 0x3292800), (0x839233, 7, 3, 0x32928A8),
    (0x839249, 7, 3, 0x128E2E3), (0x83929A, 7, 3, 0x272D0), (0x8392AC, 7, 3, 0x128E2FD),
    (0x8392C5, 8, 4, 0x128E138), (0x8392CD, 7, 3, 0x3292950), (0x8392E4, 7, 3, 0x3291D98),
    (0x839305, 8, 4, 0x128E134), (0x83932B, 7, 3, 0x272D0), (0x83933D, 7, 3, 0x128E2FF),
    (0x839356, 8, 4, 0x128E138), (0x83935E, 7, 3, 0x32929F8), (0x839375, 7, 3, 0x3291E68),
    (0x839396, 8, 4, 0x128E134), (0x8393BC, 7, 3, 0x272D0), (0x8393CE, 7, 3, 0x128E301),
    (0x8393E7, 8, 4, 0x128E138), (0x8393EF, 7, 3, 0x3292AA0), (0x839406, 7, 3, 0x3291F38),
    (0x839427, 8, 4, 0x128E134), (0x83944D, 7, 3, 0x272D0), (0x839462, 7, 3, 0x128E2E3),
    (0x839469, 7, 3, 0x128E303), (0x839474, 8, 4, 0x128E138), (0x83947C, 7, 3, 0x3292B48),
    (0x8394BB, 8, 4, 0x128E134), (0x8394CC, 7, 3, 0x3292008), (0x8394F6, 7, 3, 0x3292BF0),
    (0x839507, 7, 3, 0x128E2D2), (0x83950E, 7, 3, 0x128E305), (0x839564, 7, 3, 0x272D0),
    (0x839576, 7, 3, 0x128E2FD), (0x83958F, 8, 4, 0x128E138), (0x839597, 7, 3, 0x3292C98),
    (0x8395AE, 7, 3, 0x32920D8), (0x8395CF, 8, 4, 0x128E134), (0x8395F5, 7, 3, 0x272D0),
    (0x83960A, 7, 3, 0x128E305), (0x839611, 7, 3, 0x128E2FF), (0x83961C, 8, 4, 0x128E138),
    (0x839624, 7, 3, 0x3292D40), (0x839663, 8, 4, 0x128E134), (0x839674, 7, 3, 0x32921A8),
    (0x83969E, 7, 3, 0x128E305), (0x8396A5, 7, 3, 0x128E301), (0x8396B0, 8, 4, 0x128E138),
    (0x8396B8, 7, 3, 0x3292DE8), (0x8396F7, 8, 4, 0x128E134), (0x839708, 7, 3, 0x3292278),
    (0x839732, 7, 3, 0x128E305), (0x839739, 7, 3, 0x128E303), (0x839744, 8, 4, 0x128E138),
    (0x83974C, 7, 3, 0x3292E90), (0x83978B, 8, 4, 0x128E134), (0x83979C, 7, 3, 0x3292348),
    (0x8397C6, 7, 3, 0x3292F38), (0x8397D7, 7, 3, 0x128E2D2), (0x8397DE, 7, 3, 0x128E31B),
    (0x839834, 7, 3, 0x272D0), (0x839846, 7, 3, 0x128E2FD), (0x83985F, 8, 4, 0x128E138),
    (0x839867, 7, 3, 0x3292FE0), (0x83987E, 7, 3, 0x3292418), (0x83989F, 8, 4, 0x128E134),
    (0x8398C5, 7, 3, 0x272D0), (0x8398DA, 7, 3, 0x128E31B), (0x8398E1, 7, 3, 0x128E2FF),
    (0x8398EC, 8, 4, 0x128E138), (0x8398F4, 7, 3, 0x3293088), (0x839933, 8, 4, 0x128E134),
    (0x839944, 7, 3, 0x32924E8), (0x83996E, 7, 3, 0x128E31B), (0x839975, 7, 3, 0x128E301),
    (0x839980, 8, 4, 0x128E138), (0x839988, 7, 3, 0x3293130), (0x8399C7, 8, 4, 0x128E134),
    (0x8399D8, 7, 3, 0x32925B8), (0x839A02, 7, 3, 0x128E31B), (0x839A09, 7, 3, 0x128E303),
    (0x839A14, 8, 4, 0x128E138), (0x839A1C, 7, 3, 0x32931D8), (0x839A5B, 8, 4, 0x128E134),
    (0x839A6C, 7, 3, 0x3292688), (0x839A96, 7, 3, 0x15FE518),
]

# Every direct callee: (target, site count, FDE length). The two 0xf8f3xx targets are PLT stubs with no
# FDE of their own, so their length is None.
CALLEES = [
    (0xF590, 71, 73),      # shared release of a borrowed stack view
    (0xF8F390, 36, None),  # __cxa_atexit PLT
    (0x20D60, 15, 26),     # register-once guard
    (0xC300, 13, 352),     # float variable ctor
    (0x27050, 12, 208),    # slider item ctor
    (0x26C10, 5, 194),     # group item ctor
    (0x26EA0, 3, 237),     # toggle item ctor
    (0x20AF0, 2, 90),      # ConCmdBase ctor
    (0x26800, 1, 173),     # kind-0 label item ctor
    (0xF8F3A0, 1, None),   # __stack_chk_fail PLT (the branch target only)
]

# Every rip-relative STORE of the body: (site, target). 50 sites over 45 distinct cells.
STORES = [
    (0x837FB5, 0x3291890), (0x837FEA, 0x3291890), (0x837FF1, 0x32918F0),
    (0x837FF8, 0x32918F9), (0x838002, 0x3291900), (0x83800A, 0x3291930),
    (0x838015, 0x3291940), (0x83801C, 0x3291948), (0x83802F, 0x3291890),
    (0x838036, 0x3291950), (0x83803D, 0x32918F8), (0x83808F, 0x3291958),
    (0x8380E1, 0x3291958), (0x8380E8, 0x32919B8), (0x8380EF, 0x32919C1),
    (0x8380F9, 0x32919C8), (0x838101, 0x32919F8), (0x83810C, 0x3291A08),
    (0x838113, 0x3291A10), (0x838126, 0x3291958), (0x83812D, 0x3291A18),
    (0x838134, 0x32919C0), (0x838210, 0x3291A20), (0x838246, 0x3291A20),
    (0x83824D, 0x3291AF0), (0x838257, 0x3291A88), (0x838578, 0x3291D98),
    (0x83857F, 0x3291E00), (0x838681, 0x3291E68), (0x83868E, 0x3291ED0),
    (0x83878F, 0x3291F38), (0x838796, 0x3291FA0), (0x83889D, 0x3292008),
    (0x8388A7, 0x3292070), (0x8389AA, 0x32920D8), (0x8389B4, 0x3292140),
    (0x838ABF, 0x32921A8), (0x838ACE, 0x3292210), (0x838BD2, 0x3292278),
    (0x838BDC, 0x32922E0), (0x838CEB, 0x3292348), (0x838CF2, 0x32923B0),
    (0x838DFA, 0x3292418), (0x838E01, 0x3292480), (0x838F14, 0x32924E8),
    (0x838F1B, 0x3292550), (0x839025, 0x32925B8), (0x83902F, 0x3292620),
    (0x839134, 0x3292688), (0x83913B, 0x32926F0),
]

# Literal instruction bytes that no rip/call pin covers.
BYTES_PINS = [
    (0x837F30, "554889e5", "push rbp; mov rsp,rbp"),
    (0x837F34, "415741564155415453", "push r15/r14/r13/r12/rbx"),
    (0x837F3D, "4883e4e0", "and rsp,-0x20"),
    (0x837F41, "4881ece0000000", "sub rsp,0xe0"),
    (0x837F88, "48898424c0000000", "canary -> 0xc0(%rsp)"),
    (0x83818F, "c744241c00000000", "the threshold's min cell = 0.0f"),
    (0x838197, "c7442418ffff7f7f", "the threshold's max cell = FLT_MAX"),
    (0x8381B0, "41b901000000", "the threshold's flags r9d = 1"),
    (0x838217, "4154", "pushq %r12 = &description (first stack argument)"),
    (0x83821E, "50", "pushq %rax = &argument block (second stack argument)"),
    (0x838224, "4883c410", "add rsp,0x10 (pop the two stack arguments)"),
    (0x838240, "ff5018", "callq *0x18(%rax) #1 (the argument functor destroy)"),
    (0x838253, "00004040", "the threshold's 3.0f value immediate"),
    (0x83825D, "00004040", "the threshold's 3.0f default immediate"),
    (0x8382A3, "b930000000", "the page group's flags ecx = 0x30"),
    (0x838323, "41b830000000", "the first toggle's flags r8d = 0x30"),
    (0x8384C2, "c744241c00000000", "the eye colours' min cell = 0.0f"),
    (0x8384CA, "c744241800000040", "the eye colours' max cell = 2.0f"),
    (0x838564, "ff5018", "callq *0x18(%rax) #2"),
    (0x838674, "ff5018", "callq *0x18(%rax) #3"),
    (0x838786, "ff5018", "callq *0x18(%rax) #4"),
    (0x838890, "ff5018", "callq *0x18(%rax) #5"),
    (0x83899D, "ff5018", "callq *0x18(%rax) #6"),
    (0x838AB2, "ff5018", "callq *0x18(%rax) #7"),
    (0x838BC5, "ff5018", "callq *0x18(%rax) #8"),
    (0x838CDC, "ff5018", "callq *0x18(%rax) #9"),
    (0x838DF0, "ff5018", "callq *0x18(%rax) #10"),
    (0x838EFE, "ff5018", "callq *0x18(%rax) #11"),
    (0x839015, "ff5018", "callq *0x18(%rax) #12"),
    (0x83912A, "ff5018", "callq *0x18(%rax) #13"),
    (0x83918A, "b930000000", "the second group's flags ecx = 0x30"),
    (0x8391FA, "31d2", "xorl %edx,%edx (the 0x26800 row: unused second argument)"),
    (0x8391FC, "31c9", "xorl %ecx,%ecx (the 0x26800 row: flags 0)"),
    (0x8392C5, "c5fa100d", "vmovss xmm1 <- the slider's max (0.1f)"),
    (0x8392DE, "41b830000000", "the first slider's flags r8d = 0x30"),
    (0x839AA8, "750f", "the guard branch: jne rel8"),
    (0x839AAA, "488d65d8", "lea -0x28(%rbp),%rsp"),
    (0x839AAE, "5b415c415d415e415f5dc3", "pops + retq"),
    (0x839ABE, "90", "the body's last byte (nop)"),
]

# The thirteen `callq *0x18(%rax)` sites, all through the argument tag 0x15cec00 slot 3 = 0xc4f0.
INDIRECT = [
    (0x838240, 0x15CEC00, 0xC4F0),
    (0x838564, 0x15CEC00, 0xC4F0),
    (0x838674, 0x15CEC00, 0xC4F0),
    (0x838786, 0x15CEC00, 0xC4F0),
    (0x838890, 0x15CEC00, 0xC4F0),
    (0x83899D, 0x15CEC00, 0xC4F0),
    (0x838AB2, 0x15CEC00, 0xC4F0),
    (0x838BC5, 0x15CEC00, 0xC4F0),
    (0x838CDC, 0x15CEC00, 0xC4F0),
    (0x838DF0, 0x15CEC00, 0xC4F0),
    (0x838EFE, 0x15CEC00, 0xC4F0),
    (0x839015, 0x15CEC00, 0xC4F0),
    (0x83912A, 0x15CEC00, 0xC4F0),
]

# The three class tables this body installs or dispatches through, and their pinned slots.
CLASSES = [
    # (table base, address point, dtor slot 0, register slot 0x20, gate 3 site, gate 3 body, gate 6 site,
    #  gate 6 body, gate 3 value, gate 6 value)
    (0x1594208, 0x1594218, 0x20B0, 0x20D60, 0x1594230, 0xB890, 0x1594248, 0xB930, 1, 0),
    (0x15942A0, 0x15942B0, 0x20F0, 0x20D60, 0x15942C8, 0xBBA0, 0x15942E0, 0xBBF0, 1, 3),
    (0x15943B0, 0x15943C0, 0xAD30, 0x20D60, None, None, None, None, None, None),
]
# The two argument-functor tables, all eight slots each. The first four slots are the family the tag
# belongs to (bool: 0xc2a0..0xc2f0; float: 0xc4a0..0xc4f0); slots 4..7 repeat the other family's four.
ARG_TABLES = [
    (0x15CEBE0, (0xC2A0, 0xC2C0, 0xC2E0, 0xC2F0, 0xC4A0, 0xC4C0, 0xC4E0, 0xC4F0)),
    (0x15CEC00, (0xC4A0, 0xC4C0, 0xC4E0, 0xC4F0, 0xC2A0, 0xC2C0, 0xC2E0, 0xC2F0)),
]
# The gate bodies as bytes: `mov eax,imm32; ret` and `xor eax,eax; ret`.
GATE_BYTES = {
    0xB890: "b801000000c3",
    0xB930: "31c0c3",
    0xBBA0: "b801000000c3",
    0xBBF0: "b803000000c3",
    0xC4F0: "c3",
}

# The 0xc300 body's store script, so the module's reproduced net state is tied to the *bytes* and not to
# another model: (site, bytes, what). 0xc300 FDE is 352 B, sha pinned below.
C300_SHA = "b0fad1ce8145048f5faf081eb6e4a03f97f9b8eb1cdaa44a76ff41e94f5f442f"
C300_BYTES = [
    (0xC33B, "488d05d67d5801", "lea 0x1594118 (the mid vptr table base)"),
    (0xC34D, "4883c010", "add rax,0x10 -> 0x1594128"),
    (0xC351, "488903", "[this] = the mid vptr"),
    (0xC354, "4c896360", "[this+0x60] = the caller's value cell"),
    (0xC358, "c6436c00", "byte [this+0x6c] = 0 (then the dword)"),
    (0xC35C, "c6437001", "byte [this+0x70] = 1 (min engaged)"),
    (0xC360, "418b07", "eax = *min"),
    (0xC363, "89436c", "[this+0x6c] = the min dword"),
    (0xC366, "c6437400", "byte [this+0x74] = 0 (then the dword)"),
    (0xC36A, "c6437801", "byte [this+0x78] = 1 (max engaged)"),
    (0xC36E, "418b06", "eax = *max"),
    (0xC371, "894374", "[this+0x74] = the max dword"),
    (0xC374, "c5fc118380000000", "ymm [this+0x80] = 0 (the argument copy)"),
    (0xC37C, "48898bc0000000", "[this+0xc0] = 0x1594318 (the seed functor tag)"),
    (0xC383, "48c783b000000000000000", "[this+0xb0] = 0 (payload)"),
    (0xC38E, "48c783c800000000000000", "[this+0xc8] = 0 (aux)"),
    (0xC3C9, "488900", "[this+0x80] = this+0x80 (the q[3] self-pointer rule)"),
    (0xC429, "48898bc8", "[this+0xc8] = the caller's aux (the aux swap)"),
]

# The 0x26800 body: its direct calls, its rip operand and its literal bytes.
LABEL_CALLS = [(0x2685C, 0xF1A0), (0x26865, 0xF180), (0x268A8, 0x268B0)]
LABEL_RIPS = [(0x2684E, 7, 3, 0xF96530), (0x2686A, 7, 3, 0x2DFD4B8)]
LABEL_BYTES = [
    (0x26800, "554889e5", "push rbp; mov rsp,rbp"),
    (0x2680D, "48c70300000000", "[this] = 0 (link.self)"),
    (0x26814, "48895b08", "link.prev = this"),
    (0x26818, "48895b10", "link.next = this"),
    (0x2681C, "48c7431800000000", "link.owner = 0"),
    (0x26824, "c6432000", "byte [this+0x20] = 0 (registered)"),
    (0x26828, "c5f81006", "xmm0 <- the caller's label view"),
    (0x26855, "c7434803000000", "kind = 3 (the transient store)"),
    (0x26871, "48c7839000000000000000", "target = 0"),
    (0x2687C, "c783980000000000803f", "range min = 1.0f"),
    (0x26886, "c7839c00000000002041", "range max = 10.0f"),
    (0x26890, "4489b3a0000000", "flags = r14d"),
    (0x26897, "c7434800000000", "kind = 0 (the net store)"),
    (0x268A8, "e903000000", "jmp 0x268b0 (the registration tail)"),
]

# The twelve original eye-colour object addresses (0xd0 apart) and the value cells they point at.
EYE_OBJECTS = [
    0x3291D98, 0x3291E68, 0x3291F38, 0x3292008, 0x32920D8, 0x32921A8,
    0x3292278, 0x3292348, 0x3292418, 0x32924E8, 0x32925B8, 0x3292688,
]
EYE_VALUES = [
    0x2DCE400, 0x2DCE404, 0x2DCE408, 0x2DCE40C, 0x2DCE410, 0x2DCE414,
    0x2DCE418, 0x2DCE41C, 0x2DCE420, 0x2DCE424, 0x2DCE428, 0x2DCE42C,
]
# The 0x2dce400 table, as the twelve little-endian dwords the image holds.
EYE_VALUE_BITS = [
    0x3F800000, 0x3F800000, 0x3F800000, 0x00000000,
    0x3E99999A, 0x3F333333, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F000000, 0x3E99999A, 0x3F800000,
]

# Strings of the block: (address, text).
STRINGS = [
    (0x128E170, b"shadow_debugEyeState"),
    (0x128E185, b"forceColossusAngry"),
    (0x128E198, b"playerNotTouchingBossThreshold"),
    (0x128E1B7, b"CP10"),
    (0x128E1BC, b"Colossus"),
    (0x128E1C5, b"Force Eyes Angry"),
    (0x128E1D6, b"Debug Eye State"),
    (0x128E1E6, b"Player not touching colossus threshold (seconds)"),
    (0x128E217, b"eyeColorInactiveR"),
    (0x128E229, b"eyeColorInactiveG"),
    (0x128E23B, b"eyeColorInactiveB"),
    (0x128E24D, b"eyeColorInactiveA"),
    (0x128E25F, b"eyeColorBlueR"),
    (0x128E26D, b"eyeColorBlueG"),
    (0x128E27B, b"eyeColorBlueB"),
    (0x128E289, b"eyeColorBlueA"),
    (0x128E297, b"eyeColorRedR"),
    (0x128E2A4, b"eyeColorRedG"),
    (0x128E2B1, b"eyeColorRedB"),
    (0x128E2BE, b"eyeColorRedA"),
    (0x128E2CB, b"Shadow"),
    (0x128E2D2, b"Shadow Eye Color"),
    (0x128E2E3, b"Shadow Eye Color Inactive"),
    (0x128E305, b"Shadow Eye Color Blue"),
    (0x128E31B, b"Shadow Eye Color Red"),
    (0x128E2FD, b"R"),
    (0x128E2FF, b"G"),
    (0x128E301, b"B"),
    (0x128E303, b"A"),
    (0xF96530, b"MenuSeparator"),
]
# The four channel labels share one table: R\0G\0B\0A\0.
CHANNEL_TABLE = (0x128E2FD, b"R\x00G\x00B\x00A\x00")
# The empty description view points at the zero byte that heads the next string table.
EMPTY_BYTES = 0x128E348


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
    require(TABLE_TOP - 8 * TABLE_INDEX == TABLE_WORD == 0x2DD66B0, "table row arithmetic")
    require(e.u64(TABLE_WORD) == RVA, "constructor-table entry 326 -> 0x837f30")
    # The table is indexed backwards, so entry 325 sits at a HIGHER address and entry 327 lower.
    require(TABLE_WORD + 8 == 0x2DD66B8 and e.u64(TABLE_WORD + 8) == 0x839F00,
            "entry 325 is the already-registered 0x839f00")
    require(TABLE_WORD - 8 == 0x2DD66A8 and e.u64(TABLE_WORD - 8) == 0x836330,
            "entry 327 is the already-registered 0x836330")
    # The address quoted in the task brief is entry 354, not 326.
    require(TABLE_TOP - TABLE_WORD_QUOTED_IN_BRIEF == 354 * 8, "0x2dd65d0 is entry 354")
    require(e.u64(TABLE_WORD_QUOTED_IN_BRIEF) == 0x79E180, "entry 354 is 0x79e180")

    # --- body extent, digest, padding ---------------------------------------------------------
    body = e.read(RVA, SIZE)
    if not e.skip_hash:
        require(digest(body) == SHA, "body sha256 (7055 B FDE)")
        require(digest(body[:REACHABLE]) == SHA_REACHABLE, "reachable body sha256 (7054 B)")
    require(body[0] == 0x55 and body[1:4] == b"\x48\x89\xe5", "push rbp; mov rsp,rbp")
    require(body[-1] == 0x90, "the body's last byte is the trailing nop")
    require(fde_range(e, RVA) == SIZE, "FDE address_range == 7055")

    # --- every direct call, every rip operand, the literal bytes -------------------------------
    require(len(CALLS) == 159 and len({site for site, _ in CALLS}) == 159, "159 distinct call sites")
    for site, target in CALLS:
        require(e.call(site) == target, f"call at 0x{site:x} -> 0x{target:x}")
    require(len({target for _, target in CALLS}) == 10, "10 distinct direct callees")
    require(len(CALLEES) == 10, "the callee histogram covers every target")
    histogram = {}
    for site, target in CALLS:
        histogram[target] = histogram.get(target, 0) + 1
    for target, sites, fde_size in CALLEES:
        require(histogram.get(target) == sites, f"callee 0x{target:x} has {sites} call sites")
        if fde_size is not None:
            require(fde_range(e, target) == fde_size, f"callee 0x{target:x} FDE == {fde_size}")
    require(sum(sites for _, sites, _ in CALLEES) == len(CALLS), "the histogram sums to 159")
    require(len(RIPS) == 263 and len({site for site, _, _, _ in RIPS}) == 263, "263 distinct rip sites")
    for site, length, disp_off, target in RIPS:
        require(e.rip(site, length, disp_off) == target, f"rip operand at 0x{site:x} -> 0x{target:x}")
    require(len({target for _, _, _, target in RIPS}) == 113, "113 distinct rip targets")
    # The 50 rip-relative stores land on 45 distinct cells: the objects, the two value cells, the
    # default dwords, the final vptrs and the functor words. Every site is in RIPS as well.
    require(len(STORES) == 50 and len({site for site, _ in STORES}) == 50, "50 distinct store sites")
    require(len({target for _, target in STORES}) == 45, "45 distinct store targets")
    rip_map = {site: target for site, _, _, target in RIPS}
    for site, target in STORES:
        require(rip_map.get(site) == target, f"store at 0x{site:x} -> 0x{target:x}")
    for site, want, what in BYTES_PINS:
        e.bytes_at(site, want, what)

    # --- the one branch, decoded --------------------------------------------------------------
    branch = e.read(0x839AA8, 2)
    require(branch[0] == 0x75, "the guard branch is jne rel8 at 0x839aa8")
    require(0x839AA8 + 2 + struct.unpack("<b", branch[1:2])[0] == 0x839AB9, "jne target 0x839ab9")
    require(e.call(0x839AB9) == 0xF8F3A0, "the branch target calls __stack_chk_fail")

    # --- the thirteen indirect sites ----------------------------------------------------------
    require(len(INDIRECT) == 13, "13 indirect sites")
    for site, tag, slot3 in INDIRECT:
        require(e.read(site, 3) == b"\xff\x50\x18", f"0x{site:x} is `callq *0x18(%rax)`")
        require(e.reloc_target(tag + 0x18) == slot3, f"tag 0x{tag:x} slot 3 -> 0x{slot3:x}")
        e.bytes_at(slot3, "c3", f"functor destroy 0x{slot3:x} is a bare ret")

    # --- the class tables and their gate answers ----------------------------------------------
    for base, point, dtor, register_body, g3_site, g3_rva, g6_site, g6_rva, g3, g6 in CLASSES:
        require(point == base + 0x10, "the address point is table + 0x10")
        require(e.reloc_target(point) == dtor, f"vptr 0x{point:x} slot 0 -> 0x{dtor:x}")
        require(e.reloc_target(base + 0x20) == register_body,
                f"table 0x{base:x} + 0x20 -> 0x{register_body:x} (0x20d60)")
        if g3_site is None:
            continue
        require(e.reloc_target(g3_site) == g3_rva, f"slot 3 of 0x{point:x} -> 0x{g3_rva:x}")
        require(e.reloc_target(g6_site) == g6_rva, f"slot 6 of 0x{point:x} -> 0x{g6_rva:x}")
        e.bytes_at(g3_rva, GATE_BYTES[g3_rva], f"gate 3 body 0x{g3_rva:x}")
        e.bytes_at(g6_rva, GATE_BYTES[g6_rva], f"gate 6 body 0x{g6_rva:x}")
        require(g3_rva in GATE_BYTES and GATE_BYTES[g3_rva][:2] == "b8",
                "gate 3 returns an immediate")
    require(GATE_BYTES[0xB890] == "b801000000c3" and GATE_BYTES[0xB930] == "31c0c3",
            "the bool class gate bodies are pinned")
    require(GATE_BYTES[0xBBA0] == "b801000000c3" and GATE_BYTES[0xBBF0] == "b803000000c3",
            "the float class gate bodies are pinned")

    # --- the 0xc300 body's store script (what the module reproduces) ----------------------------
    require(fde_range(e, 0xC300) == 352, "the 0xc300 body is 352 B by its FDE")
    require(digest(e.read(0xC300, 352)) == C300_SHA, "the 0xc300 body sha256")
    require(e.rip(0xC33B, 7, 3) == 0x1594118, "0xc33b leads the mid vptr table 0x1594118")
    for site, want, what in C300_BYTES:
        e.bytes_at(site, want, what)

    # --- the two argument-functor tables -------------------------------------------------------
    for table, slots in ARG_TABLES:
        for slot, wanted in enumerate(slots):
            require(e.reloc_target(table + 8 * slot) == wanted,
                    f"argument table 0x{table:x} slot {slot} -> 0x{wanted:x}")
    # Both families' destroy slots are one `ret`, so every row-local dispatch is inert.
    e.bytes_at(0xC4F0, "c3", "the float destroy slot is a bare ret")
    e.bytes_at(0xC2F0, "c3", "the bool destroy slot is a bare ret")
    # The two copy slots install their own family's class tag.
    e.bytes_at(0xC2A4, "488d0dd5", "0xc2a0 lea (the bool class table 0x1594280)")
    require(e.rip(0xC2A4, 7, 3) == 0x1594280, "0xc2a0 copies the bool class tag 0x1594280")
    require(e.rip(0xC4A4, 7, 3) == 0x1594318, "0xc4a0 copies the float class tag 0x1594318")

    # --- the 36 __cxa_atexit registrations, in body order ---------------------------------------
    atexit_sites = [site for site, target in CALLS if target == 0xF8F390]
    require(atexit_sites == [site for site, _, _ in ATEXIT], "the 36 atexit sites in body order")
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
    # The eye-colour objects are 0xd0 apart, the two bools 0xc8, the threshold 0xd8, every item 0xa8.
    require(ATEXIT[0][2] + 0xc8 == ATEXIT[1][2], "the two bool variables are 0xc8 apart")
    require(ATEXIT[1][2] + 0xc8 == ATEXIT[2][2], "the threshold follows the bools")
    require(ATEXIT[2][2] + 0xd8 == ATEXIT[3][2], "the page group follows the threshold")
    for i in range(7, 18):
        require(ATEXIT[i + 1][2] - ATEXIT[i][2] == 0xd0, f"eye colour {i - 7} is 0xd0 apart")
    for i in range(3, 6):
        require(ATEXIT[i + 1][2] - ATEXIT[i][2] == 0xa8, f"item {i - 3} is 0xa8 apart")
    for i in range(19, 35):
        require(ATEXIT[i + 1][2] - ATEXIT[i][2] == 0xa8, f"item {i - 19 + 4} is 0xa8 apart")
    require(ATEXIT[-1][2] + 0xa8 == 0x3293280, "the block ends at 0x3293280")

    # --- the twelve eye-colour objects and their value cells ------------------------------------
    for i, addr in enumerate(EYE_OBJECTS):
        require(ATEXIT[7 + i][2] == addr, f"eye colour {i} sits at 0x{addr:x}")
        require(EYE_VALUES[i] == 0x2DCE400 + 4 * i, f"eye colour {i} value cell")
    for i, bits in enumerate(EYE_VALUE_BITS):
        require(struct.unpack("<I", e.read(EYE_VALUES[i], 4))[0] == bits,
                f"0x2dce400[{i}] holds 0x{bits:08x}")

    # --- the 0x26800 kind-0 row ----------------------------------------------------------------
    require(fde_range(e, LABEL_CTOR) == LABEL_CTOR_SIZE, "0x26800 FDE == 173")
    require(digest(e.read(LABEL_CTOR, LABEL_CTOR_SIZE)) == LABEL_CTOR_SHA, "0x26800 sha256")
    for site, target in LABEL_CALLS:
        if target == 0x268B0:
            continue
        require(e.call(site) == target, f"0x26800 call at 0x{site:x} -> 0x{target:x}")
    require(e.jmp(LABEL_CALLS[2][0]) == 0x268B0, "0x26800 tail-jumps 0x268b0")
    for site, length, disp_off, target in LABEL_RIPS:
        require(e.rip(site, length, disp_off) == target, f"0x26800 rip 0x{site:x} -> 0x{target:x}")
    for site, want, what in LABEL_BYTES:
        e.bytes_at(site, want, what)

    # --- strings ---------------------------------------------------------------------------------
    for addr, text in STRINGS:
        e.cstring(addr, text)
    e.bytes_at(CHANNEL_TABLE[0], CHANNEL_TABLE[1].hex(), "the R/G/B/A channel table")
    e.bytes_at(EMPTY_BYTES, "00", "the empty description byte")


def check_native(texts: dict[str, str]) -> None:
    header = texts["header"]
    for name, value in CONSTANTS.items():
        require(const_value(header, name) == value, f"native constant {name}")
    for tokens, key in ((SOURCE_ORDER, "source"), (C300_ORDER, "source"), (LABEL_ORDER, "source")):
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
                        ("source", C300_ORDER), ("source", LABEL_ORDER), ("test", TEST_TOKENS),
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
        print("SKIP check_ctor_slot_326: local/re/eboot.elf not present")
        return 0
    e = Elf()
    texts = read_native()
    if "--tamper" in arguments:
        mode = arguments[arguments.index("--tamper") + 1] if len(arguments) > arguments.index("--tamper") + 1 else "elf"
        if mode == "elf":
            e.overlay = {RVA + 0x10: e.raw(RVA + 0x10, 1)[0] ^ 0x40}
        else:
            texts = dict(texts)
            texts["header"] = texts["header"].replace("kSlot326Rva", "kTampered", 1)
    e.recording = True
    run(e, texts)
    e.recording = False
    note = ""
    if "--no-mutate" not in arguments and "--tamper" not in arguments:
        elf_runs, native_runs = mutation_self_test(e, texts)
        note = f"; mutation self-test {elf_runs} ELF + {native_runs} native mutants all rejected"
    print(f"OK check_ctor_slot_326: slot 326 (0x837f30, {SIZE} B FDE, {REACHABLE} B reachable, sha "
          f"{SHA[:12]}...) is row 326 of the constructor table at 0x{TABLE_WORD:x}; 159 direct calls over "
          f"10 callees, 263 rip operands, 1 branch (0x839aa8 -> 0x839ab9 __stack_chk_fail), 13 "
          f"`callq *0x18(%rax)` inert functor destroys, 36 __cxa_atexit rows, the 0x26800 kind-0 "
          f"'MenuSeparator' ctor, and the {{1,0}}/{{1,3}} gate classes; guest bytes NOT executed{note}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
