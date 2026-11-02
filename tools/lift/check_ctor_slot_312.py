#!/usr/bin/env python3
"""ELF tie for startup constructor index 312 (RVA 0xa46130, 1250 B FDE): the entitlement/title block.

Offline and read-only. Bytes of the pinned private ELF (`local/re/eboot.elf`, sha256 d0723afb...5861d3)
are read through `slot_pin_util.Elf`; nothing is loaded, relocated or executed. Skips cleanly (exit 0)
when the private inputs are absent.

What is pinned here (everything is a byte read of the pinned image):

  * the constructor-table word: entry 312 of the backwards table is the u64 at
    0x2dd70e0 - 312*8 = 0x2dd6720 and must hold 0xa46130;
  * the body extent and digest: the FDE at .eh_frame_hdr declares 1250 B at [0xa46130, 0xa46612); the
    1,249 B *reachable* span ends at the trailing nop 0xa46611, and both digests are pinned;
  * all 30 direct `callq` sites and their targets, all 49 rip-relative operands (8 of them stores),
    and the literal instruction bytes listed in BYTES_PINS (prologue, the two `callq *0x18(%rax)`,
    the two value cells, the flags immediates, the record's four immediates, the canary compare, the
    single branch, the epilogue and the __stack_chk_fail call);
  * the branch target: the `jne rel8` at 0xa465f8 must land on 0xa4660c (__stack_chk_fail);
  * the 8 `__cxa_atexit` rows of the native table, in body order, each with its object loaded inside
    its own step window and its destructor constant loaded before the call;
  * the two int-variable classes: wrapper FDE 442 B / leaf FDE 323 B and their digests, every
    rip-relative operand and every call of the four bodies, the final/mid/seed vptr `lea` targets,
    the `[obj] = final vptr`, `[obj+0xd0]`, `[obj+0x68]`, `[obj+0x60]`, `[obj+0xc0]` stores and the
    four bound bytes of the leaf;
  * the five functor tables: 0x15d18a0 / 0x1595660 / 0x15944a0 slots 0..3 are the lifted bodies
    0xc720 / 0xc740 / 0xc760 / 0xc770, 0x15d18c0 slots 0..3 are their 0xa45d6x/8x/a0/b0 twins, and
    every destroy slot is one `c3`; the 0x26ea0 gate slots {1, 2} / {1, 1} are `b8 01 00 00 00 c3`
    / `b8 02 00 00 00 c3`;
  * the two resolved GOT values the argument blocks carry (0x52dab0 at 0x15ffba8, 0xa45dc0 at
    0x15ffef8) and the two callback tags (0xa44a40 / 0xa44a60);
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

RVA = 0xA46130
SIZE = 1250                                    # FDE address_range [0xa46130, 0xa46612)
REACHABLE = 1249                               # last reachable instruction ends at 0xa46611
SHA = "33e64f61e94046fabe78af180a26487bca42deb14db07dc89a1ca5ef3060e923"
SHA_REACHABLE = "327a08fc102adb871698e7d2bd36c62e6ec16749bcd228ac93411c21b4aa1ad3"
TABLE_TOP = 0x2DD70E0
TABLE_INDEX = 312
TABLE_WORD = TABLE_TOP - 8 * TABLE_INDEX       # 0x2dd6720
EH_FRAME_HDR = 0x150C08C

NATIVE = {
    "header": "native/core/ctor_slot_312_a46130.hpp",
    "source": "native/core/ctor_slot_312_a46130.cpp",
    "test": "native/tests/test_ctor_slot_312.cpp",
    "cmake": "native/CMakeLists.txt",
}

CONSTANTS = {
    "kSlot312Rva": 0xA46130,
    "kSlot312BodySize": 1250,
    "kSlot312EndRva": 0xA46612,
    "kSlot312ReachableBytes": 1249,
    "kSlot312InstructionCount": 238,
    "kSlot312DirectCallSites": 30,
    "kSlot312IndirectCallSites": 2,
    "kSlot312RipOperands": 49,
    "kSlot312StoreSites": 8,
    "kSlot312TableTopRva": 0x2DD70E0,
    "kSlot312TableWordRva": 0x2DD6720,
    "kSlot312TableIndex": 312,
    "kSlot312NextUnrecoveredIndex": 314,
    "kSlot312NextUnrecoveredRva": 0xA41810,
    "kSlot312CanaryCellRva": 0x15FE518,
    "kSlot312CanaryFrameOffset": 0x30,
    "kSlot312StackChkFailRva": 0xF8F3A0,
    "kSlot312BranchRva": 0xA465F8,
    "kSlot312BranchTargetRva": 0xA4660C,
    "kSlot312FrameBytes": 0x88,
    "kSlot312AtexitRva": 0xF8F390,
    "kSlot312DsoRva": 0x1604218,
    "kSlot312ReleaseRva": 0xF590,
    "kSlot312ConCmdRegisterRva": 0x20D60,
    "kSlot312MenuGroupCtorRva": 0x26C10,
    "kSlot312MenuToggleCtorRva": 0x26EA0,
    "kSlot312MenuRegisterRva": 0x268B0,
    "kSlot312MenuItemDtorRva": 0x272D0,
    "kSlot312EmptyViewCtorRva": 0xF180,
    "kSlot312StringVarCtorRva": 0x257A0,
    "kSlot312TitleSkuRva": 0x32F6F58,
    "kSlot312AgeRestrictionRva": 0x32F70A0,
    "kSlot312RegionRva": 0x32F7178,
    "kSlot312GroupRva": 0x32F7250,
    "kSlot312ToggleSkuRva": 0x32F72F8,
    "kSlot312ToggleAgeRva": 0x32F73A0,
    "kSlot312ToggleRegionRva": 0x32F7448,
    "kSlot312RecordRva": 0x32F74F0,
    "kSlot312StringVarDtorRva": 0x23C90,
    "kSlot312RecordDtorRva": 0xA44AC0,
    "kSlot312RecordSize": 0x80,
    "kSlot312IntVarClassCount": 2,
    "kSlot312VarFunctorDispatches": 11,
    "kSlot312RowLocalFunctorSites": 2,
    "kSlot312CtorFlags": 1,
    "kSlot312WrapperSize": 0x1BA,
    "kSlot312LeafSize": 0x143,
    "kSlot312GroupParentRva": 0x12C2EB3,
    "kSlot312MenuLabelRva": 0x12C2EC5,
    "kSlot312SkuTextRva": 0x12C2ECF,
    "kSlot312AgeTextRva": 0x12C2ED8,
    "kSlot312RegionTextRva": 0x12C2EE8,
    "kSlot312VarDescRva": 0x12C2FA6,
    "kSlot312ItemFlags": 0x30,
    "kSlot312RecordNameRva": 0x12C2E8B,
    "kSlot312RecordWord20": 0x0,
    "kSlot312RecordBlockRva": 0x12C2E60,
    "kSlot312RecordWord34": 0x0,
    "kSlot312RecordWord38": 0xFFFFFFFF,
    "kSlot312RecordWord3c": 0x72,
}

# The body order of the native model (each token must appear once, in this order).
SOURCE_ORDER = [
    "out.string_var = init_string_var_257a0(o.title_sku",
    "concmd.atexit_log.record(kSlot312StringVarDtorRva",
    "o.age_restriction, kSlot312IntVars[0]",
    "o.region, kSlot312IntVars[1]",
    "init_menu_group_26c10(o.group",
    "init_debug_menu_item_26ea0(o.toggle_sku",
    "init_debug_menu_item_26ea0(o.toggle_age",
    "init_debug_menu_item_26ea0(o.toggle_region",
    "o.record.word3c = kSlot312RecordWord3c;",
    "init_slot_f180(&o.record.empty40);",
    "init_slot_f180(&o.record.empty60);",
    "out.canary_checked = true;",
]

TEST_TOKENS = [
    "section_constants",
    "section_functor_and_target",
    "section_native_build",
    "section_no_var_hook",
    "section_var_hook_fails",
    "section_foreign_spec_refused",
    "section_menu_registration_blocked",
    "section_parent_contract",
    "section_canary",
    "kSlot312AtexitRows.size() == 8",
    "r.atexit_records == 8 && r.functors_destroyed == 2 && r.var_functor_calls == 22",
]

CMAKE_TOKENS = [
    "core/ctor_slot_312_a46130.cpp",
    "add_test(NAME ctor_slot_312 COMMAND test_ctor_slot_312)",
]

# The 8 atexit registrations, in body order: (site, dtor, object).
ATEXIT = [
    (0xA461D3, 0x23C90, 0x32F6F58),
    (0xA46287, 0x18A60, 0x32F70A0),
    (0xA46338, 0xA44A80, 0x32F7178),
    (0xA463C6, 0x272D0, 0x32F7250),
    (0xA46447, 0x272D0, 0x32F72F8),
    (0xA464CF, 0x272D0, 0x32F73A0),
    (0xA46557, 0x272D0, 0x32F7448),
    (0xA465E5, 0xA44AC0, 0x32F74F0),
]

# Every direct `callq` of the body: (site, target). 30 sites over 9 distinct callees.
CALLS = [
    (0xA461AA, 0x257A0),
    (0xA461B2, 0xF590),
    (0xA461BA, 0xF590),
    (0xA461D3, 0xF8F390),
    (0xA46260, 0xA45BA0),
    (0xA46272, 0xF590),
    (0xA46287, 0xF8F390),
    (0xA46314, 0xA45DE0),
    (0xA46326, 0xF590),
    (0xA46338, 0xF8F390),
    (0xA4639A, 0x26C10),
    (0xA463A2, 0xF590),
    (0xA463AA, 0xF590),
    (0xA463C6, 0xF8F390),
    (0xA46429, 0x26EA0),
    (0xA46431, 0xF590),
    (0xA46439, 0xF590),
    (0xA46447, 0xF8F390),
    (0xA464B1, 0x26EA0),
    (0xA464B9, 0xF590),
    (0xA464C1, 0xF590),
    (0xA464CF, 0xF8F390),
    (0xA46539, 0x26EA0),
    (0xA46541, 0xF590),
    (0xA46549, 0xF590),
    (0xA46557, 0xF8F390),
    (0xA465C7, 0xF180),
    (0xA465D3, 0xF180),
    (0xA465E5, 0xF8F390),
    (0xA4660C, 0xF8F3A0),
]

# Every rip-relative operand: (site, instruction length, disp offset, target). 49 operands.
RIPS = [
    (0xA46144, 7, 3, 0x15FE518),   # movq [0x15fe518], %rax  (__stack_chk_guard)
    (0xA4614F, 7, 3, 0x12C2E80),   # "titleskuid"
    (0xA4615D, 7, 3, 0x32F6F58),   # the string variable
    (0xA46168, 7, 3, 0x12C2E8B),   # "DEFAULT"
    (0xA461BF, 7, 3, 0x23C90),     # its destructor
    (0xA461C6, 7, 3, 0x1604218),   # __dso_handle
    (0xA461D8, 8, 4, 0x15FFBA8),   # q[1] of the class-A argument block (resolved 0x52dab0)
    (0xA461E0, 7, 3, 0x12C2E93),   # "titleAgeRestriction"
    (0xA461EE, 7, 3, 0x15D18A0),   # the class-A argument functor tag
    (0xA461F9, 7, 3, 0x32F70A0),   # the class-A variable
    (0xA46220, 7, 3, 0xA44A40),    # q[3]: the value-changed callback tag
    (0xA46277, 7, 3, 0x18A60),     # class-A destructor
    (0xA4628C, 8, 4, 0x15FFEF8),   # q[1] of the class-B argument block (resolved 0xa45dc0)
    (0xA46294, 7, 3, 0x12C2EA7),   # "titleRegion"
    (0xA462A2, 7, 3, 0x15D18C0),   # the class-B argument functor tag
    (0xA462AD, 7, 3, 0x32F7178),   # the class-B variable
    (0xA462D4, 7, 3, 0xA44A60),    # q[3] of class B
    (0xA4632B, 7, 3, 0xA44A80),    # class-B destructor
    (0xA4633D, 7, 3, 0x32F7250),   # the menu group item
    (0xA4634F, 7, 3, 0x12C2EB3),   # parent "Platform Services"
    (0xA46356, 7, 3, 0x12C2EC5),   # caption "TitleInfo"
    (0xA463AF, 7, 3, 0x272D0),     # menu item destructor
    (0xA463B6, 7, 3, 0x1604218),
    (0xA463CF, 7, 3, 0x32F72F8),   # toggle "Sku Name"
    (0xA463E1, 7, 3, 0x12C2ECF),
    (0xA463F2, 7, 3, 0x32F6F58),   # its target: the string variable
    (0xA4644C, 7, 3, 0x12C2ED8),
    (0xA46453, 7, 3, 0x32F73A0),   # toggle "Age Restriction"
    (0xA46465, 7, 3, 0x12C2EC5),
    (0xA46486, 7, 3, 0x32F70A0),   # its target: the class-A variable
    (0xA464D4, 7, 3, 0x12C2EE8),
    (0xA464DB, 7, 3, 0x32F7448),   # toggle "Region"
    (0xA464ED, 7, 3, 0x12C2EC5),
    (0xA4650E, 7, 3, 0x32F7178),   # its target: the class-B variable
    (0xA4655C, 8, 4, 0x12C2E60),   # the record's 16 B block {1, 7, -1, -1}
    (0xA46564, 7, 3, 0x12C2E8B),   # the record's "DEFAULT" view data
    (0xA4656B, 7, 3, 0x32F7530),   # the record's first empty view
    (0xA46576, 7, 3, 0x32F74F0),   # the record
    (0xA4657D, 7, 3, 0x32F74F0),   # store: +0x00
    (0xA46584, 11, 3, 0x32F74F8),  # store: +0x08 = 7
    (0xA4658F, 8, 4, 0x32F7500),   # store: +0x10 = 0 (xmm)
    (0xA46597, 10, 2, 0x32F7510),  # store: +0x20 = 0
    (0xA465A1, 8, 4, 0x32F7514),   # store: +0x24 (xmm)
    (0xA465A9, 10, 2, 0x32F7524),  # store: +0x34 = 0
    (0xA465B3, 10, 2, 0x32F7528),  # store: +0x38 = -1
    (0xA465BD, 10, 2, 0x32F752C),  # store: +0x3c = 0x72
    (0xA465CC, 7, 3, 0x32F7550),   # the record's second empty view
    (0xA465D8, 7, 3, 0xA44AC0),    # the record's destructor
    (0xA465EA, 7, 3, 0x15FE518),   # the canary re-read before the compare
]

# Literal instruction bytes that no rip/call pin covers.
BYTES_PINS = [
    (0xA46130, "554889e5", "push rbp; mov rsp,rbp"),
    (0xA46134, "415741564155415453", "push r15/r14/r13/r12/rbx"),
    (0xA4613D, "4881ec88000000", "sub rsp,0x88"),
    (0xA46178, "488945d0", "canary -> -0x30(%rbp)"),
    (0xA461F5, "4c897580", "q[0] of the class-A block = &block"),
    (0xA4620B, "c7855cffffffffffffff", "class-A value cell = -1"),
    (0xA46249, "48c745b000000000", "class-A aux cell = 0"),
    (0xA46251, "48894dc0", "class-A functor tag -> block+0x40"),
    (0xA46258, "48c745c800000000", "class-A fn aux = 0"),
    (0xA4626C, "ff5018", "callq *0x18(%rax) #1 (class A, slot 3 = bare ret)"),
    (0xA462BF, "c7855cffffff00000000", "class-B value cell = 0"),
    (0xA46320, "ff5018", "callq *0x18(%rax) #2 (class B, slot 3 = bare ret)"),
    (0xA46361, "b930000000", "group flags ecx = 0x30"),
    (0xA463EC, "41b830000000", "toggle flags r8d = 0x30 (Sku Name)"),
    (0xA46470, "41b830000000", "toggle flags r8d = 0x30 (Age Restriction)"),
    (0xA464F8, "41b830000000", "toggle flags r8d = 0x30 (Region)"),
    (0xA46584, "48c705690f8b0207000000", "record +0x08 = 7"),
    (0xA46597, "c7056f0f8b0200000000", "record +0x20 = 0"),
    (0xA465B3, "c7056b0f8b02ffffffff", "record +0x38 = 0xffffffff"),
    (0xA465BD, "c705650f8b0272000000", "record +0x3c = 0x72"),
    (0xA465F4, "483b45d0", "cmpq -0x30(%rbp), %rax"),
    (0xA465F8, "7512", "the body's only branch: jne rel8"),
    (0xA465FA, "4881c488000000", "add rsp,0x88"),
    (0xA46601, "5b415c415d415e415f5dc3", "pops + retq"),
    (0xA4660C, "e88f8d5400", "callq __stack_chk_fail"),
    (0xA46611, "90", "the body's last byte (nop)"),
]

# The two `callq *0x18(%rax)` sites and the tag each dispatches through.
INDIRECT = [
    (0xA4626C, 0x15D18A0, 0xC770),
    (0xA46320, 0x15D18C0, 0xA45DB0),
]

# class (wrapper, leaf, final vptr base, mid vptr base, seed tag, arg tag, final tag, dtor, slot3, slot6)
CLASSES = [
    (0xA45BA0, 0x3A980, 0x1594BF8, 0x1594190, 0x1595660, 0x15D18A0, 0x15944A0, 0x18A60, (0xBF70, 1), (0xBFC0, 2)),
    (0xA45DE0, 0xA45FA0, 0x15D18E0, 0x15D1808, 0x15D18C0, 0x15D18C0, 0x15D18C0, 0xA44A80,
     (0xA44F20, 1), (0xA451F0, 1)),
]

WRAPPER_SIZE = 442
WRAPPER_SHA = {
    0xA45BA0: "1fa595dfcbed6b17090a108a481dc8f46e36c33fb19037ed1899c7a4912f08bc",
    0xA45DE0: "7772b59edd7ce01374713002636ac19a4c86d7431aa36826d6ffe19b5c8af332",
}
LEAF_SIZE = 323
LEAF_SHA = {
    0x3A980: "738126cdc1dc7826c51bb4545b5cc76b8e70b0dd7e2f560aded0ce1732df5eef",
    0xA45FA0: "b7b8359cc90555f980f48e0b55b75c3f14215b2548917edbc40c0508945dba2c",
}

# The four lifted functor bodies of both classes: address -> (FDE length, head bytes).
# copy/move start `mov rax,[rsi+0x10]`, clone starts `lea rax,[rip+...]`, destroy is one `c3`.
FUNCTOR_BODIES = {
    0xC720: (20, "488b4610"), 0xC740: (20, "488d05"), 0xC760: (9, "488b4610"), 0xC770: (1, "c3"),
    0xA45D60: (20, "488b4610"), 0xA45D80: (20, "488d05"), 0xA45DA0: (9, "488b4610"),
    0xA45DB0: (1, "c3"),
}
FN_TABLE_SLOTS = [
    (0x15D18A0, (0xC720, 0xC740, 0xC760, 0xC770)),
    (0x1595660, (0xC720, 0xC740, 0xC760, 0xC770)),
    (0x15944A0, (0xC720, 0xC740, 0xC760, 0xC770)),
    (0x15D18C0, (0xA45D60, 0xA45D80, 0xA45DA0, 0xA45DB0)),
]

# Wrapper + leaf rip operands and calls (all four bodies), pinned separately from the slot body.
WRAPPER_RIPS = {
    0xA45BA0: [(0xA45BB4, 7, 3, 0x15FE518), (0xA45BBB, 7, 3, 0x1594BF8), (0xA45BD3, 7, 3, 0x15D18A0),
               (0xA45CC2, 7, 3, 0x12C2FA6), (0xA45D32, 7, 3, 0x15FE518)],
    0xA45DE0: [(0xA45DF4, 7, 3, 0x15FE518), (0xA45DFB, 7, 3, 0x15D18E0), (0xA45E13, 7, 3, 0x15D18C0),
               (0xA45F02, 7, 3, 0x12C2FA6), (0xA45F72, 7, 3, 0x15FE518)],
}
WRAPPER_CALLS = {
    0xA45BA0: [(0xA45CF9, 0x3A980), (0xA45D01, 0xF590), (0xA45D2D, 0x20D60), (0xA45D54, 0xF8F3A0)],
    0xA45DE0: [(0xA45F39, 0xA45FA0), (0xA45F41, 0xF590), (0xA45F6D, 0x20D60), (0xA45F94, 0xF8F3A0)],
}
LEAF_RIPS = {
    0x3A980: [(0x3A991, 7, 3, 0x15FE518), (0x3A9B1, 7, 3, 0x1594190), (0x3A9BC, 7, 3, 0x1595660)],
    0xA45FA0: [(0xA45FB1, 7, 3, 0x15FE518), (0xA45FD1, 7, 3, 0x15D1808), (0xA45FDC, 7, 3, 0x15D18C0)],
}
LEAF_CALLS = {
    0x3A980: [(0x3A9AC, 0x20AF0), (0x3AABD, 0xF8F3A0)],
    0xA45FA0: [(0xA45FCC, 0x20AF0), (0xA460DD, 0xF8F3A0)],
}
# Wrapper / leaf literal stores (offset-identical between the two classes).
WRAPPER_BYTES = [
    (0xA45BEC, "4c8933", "wrapperA [obj] = final vptr (before the leaf)"),
    (0xA45D13, "4c8933", "wrapperA [obj] = final vptr (after the leaf)"),
    (0xA45D22, "8983d0000000", "wrapperA [obj+0xd0] = *value cell"),
    (0xA45D2A, "894368", "wrapperA [obj+0x68] = *value cell (second load)"),
    (0xA45D40, "7512", "wrapperA canary branch"),
    (0xA45E2C, "4c8933", "wrapperB [obj] = final vptr (before the leaf)"),
    (0xA45F53, "4c8933", "wrapperB [obj] = final vptr (after the leaf)"),
    (0xA45F62, "8983d0000000", "wrapperB [obj+0xd0] = *value cell"),
    (0xA45F6A, "894368", "wrapperB [obj+0x68] = *value cell (second load)"),
    (0xA45F80, "7512", "wrapperB canary branch"),
    (0x3A9C7, "488903", "leafA [obj] = mid vptr"),
    (0x3A9CA, "4c897b60", "leafA [obj+0x60] = value cell (this+0xd0)"),
    (0x3A9CE, "c6436c00", "leafA byte [obj+0x6c] = 0"),
    (0x3A9D2, "c6437000", "leafA byte [obj+0x70] = 0 (min disengaged)"),
    (0x3A9D6, "c6437400", "leafA byte [obj+0x74] = 0"),
    (0x3A9DA, "c6437800", "leafA byte [obj+0x78] = 0 (max disengaged)"),
    (0x3A9E6, "48898bc0000000", "leafA [obj+0xc0] = seed functor tag"),
    (0x3A9ED, "48c783b000000000000000", "leafA [obj+0xb0] = 0"),
    (0x3A9F8, "48c783c800000000000000", "leafA [obj+0xc8] = 0"),
    (0x3AA26, "4885c0", "leafA test q[3]"),
    (0x3AA29, "740a", "leafA je (no self pointer)"),
    (0x3AAA1, "ff5018", "leafA functor destroy"),
    (0x3AAA8, "483b45d0", "leafA canary compare"),
    (0x3AAAC, "750f", "leafA canary branch"),
    (0xA45FE7, "488903", "leafB [obj] = mid vptr"),
    (0xA45FEA, "4c897b60", "leafB [obj+0x60] = value cell (this+0xd0)"),
    (0xA45FEE, "c6436c00", "leafB byte [obj+0x6c] = 0"),
    (0xA45FF2, "c6437000", "leafB byte [obj+0x70] = 0"),
    (0xA45FF6, "c6437400", "leafB byte [obj+0x74] = 0"),
    (0xA45FFA, "c6437800", "leafB byte [obj+0x78] = 0"),
    (0xA46006, "48898bc0000000", "leafB [obj+0xc0] = seed functor tag"),
    (0xA4600D, "48c783b000000000000000", "leafB [obj+0xb0] = 0"),
    (0xA46018, "48c783c800000000000000", "leafB [obj+0xc8] = 0"),
    (0xA46046, "4885c0", "leafB test q[3]"),
    (0xA46049, "740a", "leafB je"),
    (0xA460C1, "ff5018", "leafB functor destroy"),
    (0xA460C8, "483b45d0", "leafB canary compare"),
    (0xA460CC, "750f", "leafB canary branch"),
]

# Strings of the block: (address, text).
STRINGS = [
    (0x12C2E80, b"titleskuid"),
    (0x12C2E8B, b"DEFAULT"),
    (0x12C2E93, b"titleAgeRestriction"),
    (0x12C2EA7, b"titleRegion"),
    (0x12C2EB3, b"Platform Services"),
    (0x12C2EC5, b"TitleInfo"),
    (0x12C2ECF, b"Sku Name"),
    (0x12C2ED8, b"Age Restriction"),
    (0x12C2EE8, b"Region"),
    (0x12C2FA6, b""),
]

# The two resolved GOT values the argument blocks carry, as RELATIVE relocations.
GOT_VALUES = [(0x15FFBA8, 0x52DAB0), (0x15FFEF8, 0xA45DC0)]


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
        record = e.raw(fde, 16)                        # the FDE record (its range is the pin)
        require(struct.unpack("<I", record[0:4])[0] >= 16, "FDE record length")
        location = fde + 8 + struct.unpack("<i", record[8:12])[0]
        require(location == initial, f"FDE initial_location 0x{location:x}")
        return struct.unpack("<I", record[12:16])[0]
    raise SystemExit(f"FAIL no FDE for 0x{initial:x} in eh_frame_hdr")


def check_elf(e: Elf) -> None:
    # --- the startup-table row ---------------------------------------------------------------
    require(TABLE_TOP - 8 * TABLE_INDEX == TABLE_WORD == 0x2DD6720, "table row arithmetic")
    require(e.u64(TABLE_WORD) == RVA, "constructor-table entry 312 -> 0xa46130")

    # --- body extent, digest, padding ---------------------------------------------------------
    body = e.read(RVA, SIZE)
    if not e.skip_hash:
        require(digest(body) == SHA, "body sha256 (1250 B FDE)")
        require(digest(body[:REACHABLE]) == SHA_REACHABLE, "reachable body sha256 (1249 B)")
    require(body[0] == 0x55 and body[1:4] == b"\x48\x89\xe5", "push rbp; mov rsp,rbp")
    require(body[-1] == 0x90, "the body's last byte is the trailing nop")
    require(e.read(RVA + SIZE, 4) == b"\x90\x90\x90\x90", "nop padding after the body")
    require(fde_range(e, RVA) == SIZE, "FDE address_range == 1250")

    # --- every direct call, every rip operand, the literal bytes -------------------------------
    require(len(CALLS) == 30 and len({site for site, _ in CALLS}) == 30, "30 distinct call sites")
    for site, target in CALLS:
        require(e.call(site) == target, f"call at 0x{site:x} -> 0x{target:x}")
    require(len(RIPS) == 49 and len({site for site, _, _, _ in RIPS}) == 49, "49 distinct rip sites")
    for site, length, disp_off, target in RIPS:
        require(e.rip(site, length, disp_off) == target, f"rip operand at 0x{site:x} -> 0x{target:x}")
    for site, want, what in BYTES_PINS:
        e.bytes_at(site, want, what)

    # --- the single branch, decoded ------------------------------------------------------------
    branch = e.read(0xA465F8, 2)
    require(branch[0] == 0x75, "the body's only branch is jne rel8 at 0xa465f8")
    require(0xA465F8 + 2 + struct.unpack("<b", branch[1:2])[0] == 0xA4660C, "jne target 0xa4660c")
    require(e.call(0xA4660C) == 0xF8F3A0, "the branch target calls __stack_chk_fail")

    # --- the two row-local indirect sites ------------------------------------------------------
    for site, tag, slot3 in INDIRECT:
        require(e.read(site, 3) == b"\xff\x50\x18", f"0x{site:x} is `callq *0x18(%rax)`")
        require(e.reloc_target(tag + 0x18) == slot3, f"tag 0x{tag:x} slot 3 -> 0x{slot3:x}")
        e.bytes_at(slot3, "c3", f"functor destroy 0x{slot3:x} is a bare ret")

    # --- the 8 __cxa_atexit registrations, in body order ---------------------------------------
    atexit_sites = [site for site, target in CALLS if target == 0xF8F390]
    require(atexit_sites == [site for site, _, _ in ATEXIT], "the 8 atexit sites in body order")
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

    # --- the two int-variable classes -----------------------------------------------------------
    for wrapper, leaf, final_base, mid_base, seed, arg_tag, final_tag, dtor, gate3, gate6 in CLASSES:
        require(fde_range(e, wrapper) == WRAPPER_SIZE, f"wrapper 0x{wrapper:x} FDE == 442")
        require(fde_range(e, leaf) == LEAF_SIZE, f"leaf 0x{leaf:x} FDE == 323")
        # The four bodies are pinned by digest, kept active during the mutation run: their individual
        # instruction pins only sample them (as slot 309 does for its shared callees).
        require(digest(e.read(wrapper, WRAPPER_SIZE)) == WRAPPER_SHA[wrapper],
                f"wrapper 0x{wrapper:x} sha256")
        require(digest(e.read(leaf, LEAF_SIZE)) == LEAF_SHA[leaf], f"leaf 0x{leaf:x} sha256")
        for site, length, disp_off, target in WRAPPER_RIPS[wrapper]:
            require(e.rip(site, length, disp_off) == target, f"wrapper rip 0x{site:x} -> 0x{target:x}")
        for site, target in WRAPPER_CALLS[wrapper]:
            require(e.call(site) == target, f"wrapper call 0x{site:x} -> 0x{target:x}")
        for site, length, disp_off, target in LEAF_RIPS[leaf]:
            require(e.rip(site, length, disp_off) == target, f"leaf rip 0x{site:x} -> 0x{target:x}")
        for site, target in LEAF_CALLS[leaf]:
            require(e.call(site) == target, f"leaf call 0x{site:x} -> 0x{target:x}")
        # The final/mid vptr `lea` targets are address points, i.e. .data.rel.ro table + 0x10.
        require(e.rip(0xA45BBB if wrapper == 0xA45BA0 else 0xA45DFB, 7, 3) == final_base,
                f"wrapper 0x{wrapper:x} lea final table")
        require(e.reloc_target(final_base + 0x10) == dtor, f"final vptr 0x{final_base + 0x10:x} slot 0")
        require(e.reloc_target(final_base + 0x10 + 0x18) == gate3[0],
                f"vptr 0x{final_base + 0x10:x} slot 3 -> 0x{gate3[0]:x}")
        require(e.reloc_target(final_base + 0x10 + 0x30) == gate6[0],
                f"vptr 0x{final_base + 0x10:x} slot 6 -> 0x{gate6[0]:x}")
        e.bytes_at(gate3[0], f"b8{gate3[1]:02x}000000c3", f"gate slot 0x{gate3[0]:x}")
        e.bytes_at(gate6[0], f"b8{gate6[1]:02x}000000c3", f"gate slot 0x{gate6[0]:x}")
        require(e.reloc_target(mid_base + 0x20) == 0x20D60,
                f"mid vptr 0x{mid_base + 0x10:x} slot 2 is the 0x20d60 registration")
        tables = {table for table, _ in FN_TABLE_SLOTS}
        require(arg_tag in tables and seed in tables and final_tag in tables,
                f"class 0x{wrapper:x} functor tables are pinned")
    for site, want, what in WRAPPER_BYTES:
        e.bytes_at(site, want, what)

    # --- the five functor tables and their four lifted bodies -----------------------------------
    for table, (copy, clone, move, destroy) in FN_TABLE_SLOTS:
        for slot, body in enumerate((copy, clone, move, destroy)):
            require(e.reloc_target(table + 8 * slot) == body,
                    f"functor table 0x{table:x} slot {slot} -> 0x{body:x}")
    for addr, (size, head) in FUNCTOR_BODIES.items():
        require(fde_range(e, addr) == size, f"functor body 0x{addr:x} FDE == {size}")
        e.bytes_at(addr, head, f"functor body 0x{addr:x} head")

    # --- the resolved GOT values of the two argument blocks -------------------------------------
    for slot, value in GOT_VALUES:
        require(e.reloc_target(slot) == value, f"GOT 0x{slot:x} -> 0x{value:x}")

    # --- strings ---------------------------------------------------------------------------------
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
        mutated["header"] = pattern.sub(lambda m: f"{m.group(1)}{value ^ 0x4:#x}{m.group(3)}",
                                        texts["header"], count=1)
        require(mutated["header"] != texts["header"], f"constant {name} pattern did not match")
        expect_failure(f"constant {name}", lambda m=mutated: check_native(m))
        native_ran += 1
    return ran, native_ran


def main() -> int:
    arguments = sys.argv[1:]
    if not (ROOT / "local/re/eboot.elf").is_file():
        print("SKIP check_ctor_slot_312: local/re/eboot.elf not present")
        return 0
    e = Elf()
    texts = read_native()
    if "--tamper" in arguments:
        mode = arguments[arguments.index("--tamper") + 1] if len(arguments) > arguments.index("--tamper") + 1 else "elf"
        if mode == "elf":
            e.overlay = {RVA + 0x10: e.raw(RVA + 0x10, 1)[0] ^ 0x40}
        else:
            texts = dict(texts)
            texts["header"] = texts["header"].replace("kSlot312Rva", "kTampered", 1)
    e.recording = True
    run(e, texts)
    e.recording = False
    note = ""
    if "--no-mutate" not in arguments and "--tamper" not in arguments:
        elf_runs, native_runs = mutation_self_test(e, texts)
        note = f"; mutation self-test {elf_runs} ELF + {native_runs} native mutants all rejected"
    print(f"OK check_ctor_slot_312: slot 312 (0xa46130, {SIZE} B FDE, {REACHABLE} B reachable, sha "
          f"{SHA[:12]}...) is row 312 of the constructor table at 0x{TABLE_WORD:x}; 30 direct calls, 49 "
          f"rip operands, 1 branch to 0xa4660c (__stack_chk_fail), 2 `callq *0x18(%rax)` inert functor "
          f"destroys, 8 __cxa_atexit rows, 2 int-variable classes (wrapper 442 B / leaf 323 B) with "
          f"pinned functor tables and gate slots {1}/{2} and {1}/{1}; guest bytes NOT executed{note}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
