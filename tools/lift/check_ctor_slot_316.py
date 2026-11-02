#!/usr/bin/env python3
"""ELF tie for startup constructor index 316 (RVA 0xa33280, 2561 B FDE): the Localization block.

Offline and read-only. Bytes of the pinned private ELF (`local/re/eboot.elf`, sha256 d0723afb...5861d3)
are read through `slot_pin_util.Elf`; nothing is loaded, relocated or executed. Skips cleanly (exit 0)
when the private inputs are absent.

What is pinned here (everything is a byte read of the pinned image):

  * the constructor-table word: entry 316 of the backwards table is the u64 at
    0x2dd70e0 - 316*8 = 0x2dd6700 and must hold 0xa33280;
  * the body extent and digest: the FDE at .eh_frame_hdr declares 2561 B at [0xa33280, 0xa33c81); the
    2560 B *reachable* span ends at the trailing nop 0xa33c80, and both digests are pinned;
  * all 54 direct `callq` sites and their targets, all 88 rip-relative operands (11 of them stores
    over 9 distinct addresses, 62 distinct targets), and the literal instruction bytes listed in
    BYTES_PINS (prologue, the five indirect call encodings, the bound/value immediates, the two
    branches, the epilogue and the __stack_chk_fail call);
  * both branches: the `jne rel8` at 0xa33c6a must land on 0xa33c7b (__stack_chk_fail) and the
    `je rel8` at 0xa338a7 on 0xa338b1 (the functor self-pointer skip);
  * the 14 `__cxa_atexit` rows of the native table, in body order, each with its object loaded inside
    its own step window and its destructor constant loaded before the call;
  * the five indirect sites: the four `callq *0x18(%rax)` dispatch through the argument tag 0x15d1188
    (slot 3 = 0xa32490, one `c3`), and `callq *0x20(%rax)` dispatches through the command class table
    0x1594dc0 slot 0x20 = 0x20d60 (the same slot as its vptr's +0x10);
  * the language-variable constructor pair 0xa324c0 (436 B) / 0xa32680 (352 B): digests, every
    rip-relative operand and every direct call of both bodies, the four lifted functor bodies
    (0xa32440 / 0xa32460 / 0xa32480 / 0xa32490), the 0xa324a0 invoker thunk, the mid/final vptr `lea`
    bases, and the object stores at +0x60/+0x68/+0x6c/+0x70/+0x74/+0x78/+0x80/+0x88/+0x98/+0xb0/
    +0xc0/+0xc8/+0xd0;
  * the three target classes and their 0x26ea0 gate answers: 0x15d11b8 {slot3 0xa31d90 -> 1,
    slot6 0xa31f10 -> 1}, 0x1594218 {0xb890 -> 1, 0xb930 -> 0}, 0x1594dd0 {0x22ae0 -> 0};
  * the two resolved GOT values the body carries (0xa324a0 at 0x15ffef0, 0x1990 at 0x15fe5e0);
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

RVA = 0xA33280
SIZE = 2561                                    # FDE address_range [0xa33280, 0xa33c81)
REACHABLE = 2560                               # last reachable instruction ends at 0xa33c80
SHA = "3f9fec64ac4e1c2e47678f68cb94281b1b441d33410b9dc92f1b786dcad821d6"
SHA_REACHABLE = "0ade15657b51ff758d29d61901ad38405241ab87ce9b455a4af6b7fa905dc84e"
TABLE_TOP = 0x2DD70E0
TABLE_INDEX = 316
TABLE_WORD = TABLE_TOP - 8 * TABLE_INDEX       # 0x2dd6700
EH_FRAME_HDR = 0x150C08C

# The language-variable constructor pair.
WRAPPER = 0xA324C0
WRAPPER_SIZE = 436
WRAPPER_SHA = "85af8ff01b088b210d1f65f7581e1282850f4f14d2b8f7c8e8e48a90e10d9978"
LEAF = 0xA32680
LEAF_SIZE = 352
LEAF_SHA = "292770cd70049a603c6b161491ae0f540368765ce9913267175c481b2b5d193e"

NATIVE = {
    "header": "native/core/ctor_slot_316_a33280.hpp",
    "source": "native/core/ctor_slot_316_a33280.cpp",
    "test": "native/tests/test_ctor_slot_316.cpp",
    "cmake": "native/CMakeLists.txt",
}

CONSTANTS = {
    "kSlot316Rva": 0xA33280,
    "kSlot316BodySize": 2561,
    "kSlot316EndRva": 0xA33C81,
    "kSlot316ReachableBytes": 2560,
    "kSlot316InstructionCount": 473,
    "kSlot316DirectCallSites": 54,
    "kSlot316IndirectCallSites": 5,
    "kSlot316RipOperands": 88,
    "kSlot316RipTargets": 62,
    "kSlot316StoreSites": 11,
    "kSlot316StoreTargets": 9,
    "kSlot316BranchCount": 2,
    "kSlot316TableTopRva": 0x2DD70E0,
    "kSlot316TableWordRva": 0x2DD6700,
    "kSlot316TableIndex": 316,
    "kSlot316NextUnrecoveredIndex": 326,
    "kSlot316NextUnrecoveredRva": 0x837F30,
    "kSlot316CanaryCellRva": 0x15FE518,
    "kSlot316CanaryFrameOffset": 0xC0,
    "kSlot316StackChkFailRva": 0xF8F3A0,
    "kSlot316BranchRva": 0xA33C6A,
    "kSlot316BranchTargetRva": 0xA33C7B,
    "kSlot316InnerBranchRva": 0xA338A7,
    "kSlot316InnerBranchTargetRva": 0xA338B1,
    "kSlot316FrameBytes": 0xE0,
    "kSlot316AtexitRva": 0xF8F390,
    "kSlot316DsoRva": 0x1604218,
    "kSlot316ReleaseRva": 0xF590,
    "kSlot316ConCmdBaseCtorRva": 0x20AF0,
    "kSlot316ConCmdRegisterRva": 0x20D60,
    "kSlot316MenuToggleCtorRva": 0x26EA0,
    "kSlot316MenuRegisterRva": 0x268B0,
    "kSlot316MenuItemDtorRva": 0x272D0,
    "kSlot316NameViewRva": 0xF2F0,
    "kSlot316BoolVarDtorRva": 0x20B0,
    "kSlot316CommandDtorRva": 0x4B0,
    "kSlot316CellDtorRva": 0xA2C760,
    "kSlot316LanguageVarDtorRva": 0xA2C850,
    "kSlot316UnlocalizedDtorRva": 0xF7B0,
    "kSlot316LanguageVarWrapperRva": 0xA324C0,
    "kSlot316LanguageVarWrapperSize": 436,
    "kSlot316LanguageVarWrapperInstructions": 99,
    "kSlot316LanguageVarLeafRva": 0xA32680,
    "kSlot316LanguageVarLeafSize": 352,
    "kSlot316LanguageVarLeafInstructions": 90,
    "kSlot316SharedStrCtorRva": 0xF1A0,
    "kSlot316LanguageVarMidVptr": 0x15D1120,
    "kSlot316LanguageVarFinalVptr": 0x15D11B8,
    "kSlot316LanguageFnVtbl": 0x15D1188,
    "kSlot316LanguageThunkRva": 0xA324A0,
    "kSlot316LanguageFnCopyRva": 0xA32440,
    "kSlot316LanguageFnCloneRva": 0xA32460,
    "kSlot316LanguageFnMoveRva": 0xA32480,
    "kSlot316LanguageFnDestroyRva": 0xA32490,
    "kSlot316LanguageGate3Rva": 0xA31D90,
    "kSlot316LanguageGate6Rva": 0xA31F10,
    "kSlot316LanguageGate3": 1,
    "kSlot316LanguageGate6": 1,
    "kSlot316BoolFinalVptr": 0x1594218,
    "kSlot316BoolFnTag": 0x15D1220,
    "kSlot316BoolGate3": 1,
    "kSlot316BoolGate6": 0,
    "kSlot316CommandVptr": 0x1594DD0,
    "kSlot316CommandGate3": 0,
    "kSlot316LanguageMin": 1,
    "kSlot316LanguageMax": 26,
    "kSlot316LanguageUnset": 0xFFFFFFFF,
    "kSlot316VarFlags": 1,
    "kSlot316CommandFlags": 2,
    "kSlot316ItemFlags": 0x30,
    "kSlot316LanguageDispatches": 11,
    "kSlot316LanguageRowLocalDispatches": 1,
    "kSlot316FunctorDispatches": 48,
    "kSlot316MenuParentRva": 0x12C191F,
    "kSlot316EmptyTextRva": 0x12C1BFB,
    "kSlot316UnlocalizedTextRva": 0x12C1BB2,
    "kSlot316CellRva": 0x32F4FE0,
    "kSlot316UnlocalizedRva": 0x32F58F8,
    "kSlot316ForeignGapRva": 0x32F58A0,
    "kSlot316ForeignGapSize": 0x58,
    "kSlot316AtexitRowCount": 14,
    "kSlot316FunctorSiteCount": 4,
    "kSlot316CommandRegisterSiteRva": 0xA338CB,
    "kSlot316CommandTableRva": 0x1594DC0,
    "kSlot316CommandRegisterSlot": 0x20,
    "kSlot316ItemCount": 6,
}

# The body order of the native model (each token must appear once, in this order).
SOURCE_ORDER = [
    "o.cell.word = 0;",
    "concmd.atexit_log.record(kSlot316CellDtorRva",
    "o.var_text",
    "o.var_subtitles",
    "o.var_speech",
    "o.var_system",
    "init_bool_var_static(o.var_english",
    "init_fconcmd_object(o.command",
    "init_debug_menu_item_26ea0(item",
    "register_debug_menu_item_268b0(item",
    "init_shared_str_f1a0(o.unlocalized",
    "out.canary_checked = true;",
]

TEST_TOKENS = [
    "section_constants",
    "section_functor_and_target",
    "section_language_var",
    "section_native_build",
    "section_no_language_hook",
    "section_hook_fails",
    "section_foreign_spec_refused",
    "section_no_allocator",
    "section_menu_registration_blocked",
    "section_parent_contract",
    "section_canary",
    "kSlot316AtexitRows.size() == 14",
    "r.atexit_records == 14 && r.functors_destroyed == 4",
]

CMAKE_TOKENS = [
    "core/ctor_slot_316_a33280.cpp",
    "add_test(NAME ctor_slot_316 COMMAND test_ctor_slot_316)",
]

# The 14 atexit registrations, in body order: (site, dtor, object).
ATEXIT = [
    (0xA332C6, 0xA2C760, 0x32F4FE0),
    (0xA333E4, 0xA2C850, 0x32F4FE8),
    (0xA334E7, 0xA2C850, 0x32F50C0),
    (0xA335F5, 0xA2C850, 0x32F5198),
    (0xA336F9, 0xA2C850, 0x32F5270),
    (0xA33805, 0x20B0, 0x32F5348),
    (0xA338F6, 0x4B0, 0x32F5410),
    (0xA33981, 0x272D0, 0x32F54B0),
    (0xA33A08, 0x272D0, 0x32F5558),
    (0xA33A8F, 0x272D0, 0x32F5600),
    (0xA33B1A, 0x272D0, 0x32F56A8),
    (0xA33BA4, 0x272D0, 0x32F5750),
    (0xA33C2B, 0x272D0, 0x32F57F8),
    (0xA33C53, 0xF7B0, 0x32F58F8),
]

# Every direct `callq` of the body: (site, target). 54 sites over 9 distinct callees.
CALLS = [
    (0xA332C6, 0xF8F390), (0xA333A4, 0xA324C0), (0xA333B0, 0xF590), (0xA333CB, 0xF590),
    (0xA333E4, 0xF8F390), (0xA334AE, 0xA324C0), (0xA334BA, 0xF590), (0xA334D5, 0xF590),
    (0xA334E7, 0xF8F390), (0xA335B2, 0xA324C0), (0xA335BE, 0xF590), (0xA335D9, 0xF590),
    (0xA335F5, 0xF8F390), (0xA336BD, 0xA324C0), (0xA336C9, 0xF590), (0xA336E4, 0xF590),
    (0xA336F9, 0xF8F390), (0xA3376C, 0x20AF0), (0xA337CE, 0xF590), (0xA337EB, 0x20D60),
    (0xA337F3, 0xF590), (0xA33805, 0xF8F390), (0xA3381B, 0xF2F0), (0xA3387A, 0x20AF0),
    (0xA338D3, 0xF590), (0xA338DD, 0xF590), (0xA338F6, 0xF8F390), (0xA3395F, 0x26EA0),
    (0xA33967, 0xF590), (0xA3396F, 0xF590), (0xA33981, 0xF8F390), (0xA339E3, 0x26EA0),
    (0xA339EB, 0xF590), (0xA339F3, 0xF590), (0xA33A08, 0xF8F390), (0xA33A71, 0x26EA0),
    (0xA33A79, 0xF590), (0xA33A81, 0xF590), (0xA33A8F, 0xF8F390), (0xA33AF8, 0x26EA0),
    (0xA33B00, 0xF590), (0xA33B08, 0xF590), (0xA33B1A, 0xF8F390), (0xA33B7C, 0x26EA0),
    (0xA33B84, 0xF590), (0xA33B8C, 0xF590), (0xA33BA4, 0xF8F390), (0xA33C0D, 0x26EA0),
    (0xA33C15, 0xF590), (0xA33C1D, 0xF590), (0xA33C2B, 0xF8F390), (0xA33C41, 0xF1A0),
    (0xA33C53, 0xF8F390), (0xA33C7B, 0xF8F3A0),
]

# Every rip-relative operand: (site, instruction length, disp offset, target). 88 operands.
RIPS = [
    (0xA33298, 7, 3, 0x15FE518),   # __stack_chk_guard GOT cell
    (0xA3329F, 7, 3, 0x32F4FE0),   # the pointer cell
    (0xA332A6, 7, 3, 0xA2C760),    # its destructor
    (0xA332AD, 7, 3, 0x1604218),   # __dso_handle
    (0xA332CB, 8, 4, 0x15FFEF0),   # resolved 0xa324a0 (the invoker thunk)
    (0xA332D3, 7, 3, 0x12C1820),   # "loc_text"
    (0xA332DF, 7, 3, 0x12C1829),   # its description
    (0xA332E6, 7, 3, 0x32F4FE8),   # the first language variable
    (0xA332FA, 7, 3, 0x15D1188),   # the argument functor tag
    (0xA3332E, 7, 3, 0xA2C790),    # its callback
    (0xA333D3, 7, 3, 0x1604218),
    (0xA333DA, 7, 3, 0xA2C850),    # the variable's destructor
    (0xA333EE, 7, 3, 0x12C1848),   # "loc_subtitles"
    (0xA333F8, 7, 3, 0x12C1856),
    (0xA333FF, 7, 3, 0x32F50C0),
    (0xA3340F, 7, 3, 0x15D1188),
    (0xA3343E, 7, 3, 0xA2C890),
    (0xA334DA, 7, 3, 0xA2C850),
    (0xA334F1, 7, 3, 0x12C187A),   # "loc_speech"
    (0xA334FB, 7, 3, 0x12C1885),
    (0xA33502, 7, 3, 0x32F5198),
    (0xA33512, 7, 3, 0x15D1188),
    (0xA33541, 7, 3, 0xA2C950),
    (0xA335DE, 7, 3, 0xA2C850),
    (0xA335E5, 7, 3, 0x1604218),
    (0xA335FF, 7, 3, 0x12C18A6),   # "loc_system"
    (0xA33606, 7, 3, 0x12C18B1),
    (0xA3360D, 7, 3, 0x32F5270),
    (0xA3361D, 7, 3, 0x15D1188),
    (0xA33651, 7, 3, 0xA2CA10),
    (0xA336E9, 7, 3, 0xA2C850),
    (0xA336FE, 7, 3, 0x1594208),   # the bool class table base (the body adds 0x10)
    (0xA33705, 7, 3, 0x12C18D2),   # "sLanguageUnlocalizedAsEnglish"
    (0xA3370C, 7, 3, 0x32F5348),   # the bool variable
    (0xA3372B, 7, 3, 0x12C1BFB),   # its empty description
    (0xA33765, 7, 3, 0x32F5348),   # store: [obj] = 0x1594218 before the base ctor
    (0xA33771, 7, 3, 0x15940A0),   # the mid vptr base
    (0xA3377B, 7, 3, 0x32F5408),   # obj+0xc0: the value cell
    (0xA33786, 7, 3, 0x15D1220),   # the functor tag
    (0xA33791, 7, 3, 0x32F5348),   # store: [obj] = 0x15940b0
    (0xA33798, 7, 3, 0x32F53A8),   # store: +0x60 = the value cell
    (0xA3379F, 10, 2, 0x32F53B1),  # store: dword +0x69 = 0
    (0xA337A9, 8, 4, 0x32F53B8),   # store: ymm +0x70 = 0
    (0xA337B1, 11, 3, 0x32F53E8),  # store: +0xa0 = 0
    (0xA337BC, 7, 3, 0x32F53F8),   # store: +0xb0 = 0x15d1220
    (0xA337C3, 11, 3, 0x32F5400),  # store: +0xb8 = 0
    (0xA337D6, 7, 3, 0x32F5348),   # store: [obj] = 0x1594218 (final)
    (0xA337DD, 7, 2, 0x32F5408),   # store: byte +0xc0 = 0 (the value)
    (0xA337E4, 7, 2, 0x32F53B0),   # store: byte +0x68 = 0 (the default)
    (0xA337F8, 7, 3, 0x20B0),      # the bool variable's destructor
    (0xA3380F, 7, 3, 0x12C18F0),   # "loc_reload"
    (0xA33820, 7, 3, 0x32F5410),   # the command
    (0xA33827, 8, 4, 0x15FE5E0),   # resolved 0x1990 (the command invoker thunk)
    (0xA3382F, 7, 3, 0x12C18FB),   # its description
    (0xA33865, 7, 3, 0xA2CAD0),    # its handler tag
    (0xA3387F, 7, 3, 0x1594DC0),   # the command class table (its register slot is +0x20)
    (0xA338E2, 7, 3, 0x1604218),
    (0xA338E9, 7, 3, 0x4B0),       # the command's destructor
    (0xA338FB, 7, 3, 0x32F54B0),   # item "Language Text"
    (0xA3390C, 7, 3, 0x12C191F),   # the parent "Localization"
    (0xA33913, 7, 3, 0x12C192C),
    (0xA3391E, 7, 3, 0x32F4FE8),   # its target: the first language variable
    (0xA33974, 7, 3, 0x272D0),     # the item destructor
    (0xA33986, 7, 3, 0x32F5558),   # item "Language Subtitles"
    (0xA33997, 7, 3, 0x12C193A),
    (0xA339A2, 7, 3, 0x32F50C0),
    (0xA339F8, 7, 3, 0x272D0),
    (0xA33A0D, 7, 3, 0x32F5600),   # item "Language Speech"
    (0xA33A14, 7, 3, 0x12C191F),
    (0xA33A29, 7, 3, 0x32F5198),
    (0xA33A3B, 7, 3, 0x12C194D),
    (0xA33A94, 7, 3, 0x32F56A8),   # item "Language System"
    (0xA33AA5, 7, 3, 0x12C191F),
    (0xA33AAC, 7, 3, 0x12C195D),
    (0xA33AB7, 7, 3, 0x32F5270),
    (0xA33B0D, 7, 3, 0x1604218),
    (0xA33B1F, 7, 3, 0x32F5750),   # item "Use English instead of Unlocalized"
    (0xA33B30, 7, 3, 0x12C196D),
    (0xA33B3B, 7, 3, 0x32F5348),   # its target: the bool variable
    (0xA33B91, 7, 3, 0x1604218),
    (0xA33BA9, 7, 3, 0x32F57F8),   # item "Reload"
    (0xA33BB0, 7, 3, 0x32F5410),   # its target: the command
    (0xA33BB7, 7, 3, 0x12C191F),
    (0xA33BD7, 7, 3, 0x12C1990),
    (0xA33C30, 7, 3, 0x32F58F8),   # the "!!!Unlocalized!!!" holder
    (0xA33C37, 7, 3, 0x12C1BB2),
    (0xA33C46, 7, 3, 0xF7B0),      # its destructor
    (0xA33C58, 7, 3, 0x15FE518),   # the canary re-read before the compare
]

# Literal instruction bytes that no rip/call pin covers.
BYTES_PINS = [
    (0xA33280, "554889e5", "push rbp; mov rsp,rbp"),
    (0xA33284, "415741564155415453", "push r15/r14/r13/r12/rbx"),
    (0xA3328D, "4883e4e0", "and rsp,-0x20"),
    (0xA33291, "4881ece0000000", "sub rsp,0xe0"),
    (0xA332B7, "48898424c0000000", "canary -> 0xc0(%rsp)"),
    (0xA332BF, "48c70600000000", "movq $0,(the pointer cell)"),
    (0xA33301, "c744241cffffffff", "the value cell = -1 (all four language variables)"),
    (0xA33309, "c744241801000000", "the min bound cell = 1"),
    (0xA33311, "c74424141a000000", "the max bound cell = 0x1a (26)"),
    (0xA33323, "41b901000000", "language-variable flags r9d = 1"),
    (0xA333A0, "4157", "push r15 = &functor block (first stack argument)"),
    (0xA333A2, "4156", "push r14 = &description view (second stack argument)"),
    (0xA333C5, "ff5018", "callq *0x18(%rax) #1 (slot 3 = bare ret)"),
    (0xA334CF, "ff5018", "callq *0x18(%rax) #2"),
    (0xA335D3, "ff5018", "callq *0x18(%rax) #3"),
    (0xA336DE, "ff5018", "callq *0x18(%rax) #4"),
    (0xA33721, "b901000000", "bool variable flags ecx = 1"),
    (0xA33761, "4883c310", "addq $0x10,%rbx: the bool variable's final vptr"),
    (0xA337DD, "c605241c8c0200", "bool variable +0xc0 = 0 (the value)"),
    (0xA3383F, "b902000000", "command flags ecx = 2"),
    (0xA3387F, "488d053a15b600", "lea 0x1594dc0 (the command class table)"),
    (0xA3388C, "488d4810", "lea 0x10(%rax),%rcx = the address point 0x1594dd0"),
    (0xA33890, "48890b", "mov %rcx,(%rbx) = [command] = 0x1594dd0"),
    (0xA338A7, "7408", "the functor self-pointer skip (never taken here)"),
    (0xA338A9, "488d4b60", "lea 0x60(%rbx),%rcx = &delegate"),
    (0xA338AD, "48894b60", "mov %rcx,0x60(%rbx) = the delegate self-pointer"),
    (0xA338C0, "48c7839000000000000000", "command +0x90 = 0 (the vector capacity)"),
    (0xA338CB, "ff5020", "callq *0x20(%rax) = 0x20d60 (slot 2 of 0x1594dc0)"),
    (0xA33925, "41b830000000", "toggle flags r8d = 0x30"),
    (0xA33C62, "483b8424c0000000", "cmpq 0xc0(%rsp),%rax"),
    (0xA33C6A, "750f", "the guard branch: jne rel8"),
    (0xA33C6C, "488d65d8", "lea -0x28(%rbp),%rsp"),
    (0xA33C70, "5b415c415d415e415f5dc3", "pops + retq"),
    (0xA33C7B, "e820b75500", "callq __stack_chk_fail"),
    (0xA33C80, "90", "the body's last byte (nop)"),
]

# The four row-local `callq *0x18(%rax)` sites and the tag each dispatches through.
INDIRECT = [
    (0xA333C5, 0x15D1188, 0xA32490),
    (0xA334CF, 0x15D1188, 0xA32490),
    (0xA335D3, 0x15D1188, 0xA32490),
    (0xA336DE, 0x15D1188, 0xA32490),
]
# One `callq *0x20(%rax)` with %rax = the command class table (not its vptr).
COMMAND_REGISTER = (0xA338CB, 0x1594DC0, 0x20, 0x20D60)

# The functor class of the four language variables: table -> its four lifted slot bodies.
LANGUAGE_FN_SLOTS = (0xA32440, 0xA32460, 0xA32480, 0xA32490)
INVOKER_THUNK = 0xA324A0
# address -> (FDE length, head bytes) for the five tiny bodies.
TINY_BODIES = {
    0xA32440: (20, "488b4610"),   # copy:  dst+0x10 = src+0x10 ; dst+0x20 = 0x15d1188
    0xA32460: (20, "488d05"),     # clone: dst+0x20 = 0x15d1188 ; dst+0x10 = src+0x10
    0xA32480: (9, "488b4610"),    # move:  dst+0x10 = src+0x10
    0xA32490: (1, "c3"),          # destroy: one `ret`
    0xA324A0: (18, "488b4718"),   # thunk: rax = [rdi+0x18]; shift 3 args; jmp rax
}

# The wrapper and leaf: every rip operand and every direct call of each body.
WRAPPER_RIPS = [
    (0xA324DB, 7, 3, 0x15FE518),
    (0xA324E2, 7, 3, 0x15D11A8),   # the final vptr table base (the body adds 0x10)
    (0xA3251A, 7, 3, 0x15D1188),
    (0xA3264C, 7, 3, 0x15FE518),
]
WRAPPER_CALLS = [(0xA32617, 0xA32680), (0xA32647, 0x20D60), (0xA3266E, 0xF8F3A0)]
LEAF_RIPS = [
    (0xA32691, 7, 3, 0x15FE518),
    (0xA326BB, 7, 3, 0x15D1110),   # the mid vptr table base (the body adds 0x10)
    (0xA326C6, 7, 3, 0x15D1188),
    (0xA327BB, 7, 3, 0x15FE518),
]
LEAF_CALLS = [(0xA326B6, 0x20AF0), (0xA327DA, 0xF8F3A0)]

# Object stores of the leaf/wrapper, pinned as literal bytes.
OBJECT_BYTES = [
    (0xA324E2, "4c8d35bfecb900", "wrapper: lea 0x15d11a8 (the final vptr table)"),
    (0xA32516, "4983c610", "wrapper: add r14,0x10 -> 0x15d11b8"),
    (0xA32525, "4c8933", "wrapper: [obj] = final vptr (before the leaf)"),
    (0xA3262D, "4c8933", "wrapper: [obj] = final vptr (after the leaf)"),
    (0xA3263A, "8b01", "wrapper: eax = *value cell"),
    (0xA3263C, "8983d0000000", "wrapper: [obj+0xd0] = the value"),
    (0xA32642, "8b01", "wrapper: eax = *value cell (second read)"),
    (0xA32644, "894368", "wrapper: [obj+0x68] = the default"),
    (0xA326D1, "488903", "leaf: [obj] = the mid vptr"),
    (0xA326D4, "4c896360", "leaf: [obj+0x60] = the caller's value cell"),
    (0xA326D8, "c6436c00", "leaf: byte [obj+0x6c] = 0"),
    (0xA326DC, "c6437001", "leaf: byte [obj+0x70] = 1 (min engaged)"),
    (0xA326E0, "418b07", "leaf: eax = *min cell"),
    (0xA326E3, "89436c", "leaf: [obj+0x6c] = the min"),
    (0xA326E6, "c6437400", "leaf: byte [obj+0x74] = 0"),
    (0xA326EA, "c6437801", "leaf: byte [obj+0x78] = 1 (max engaged)"),
    (0xA326EE, "418b06", "leaf: eax = *max cell"),
    (0xA326F1, "894374", "leaf: [obj+0x74] = the max"),
    (0xA326F4, "c5fc118380000000", "leaf: ymm [obj+0x80] = 0"),
    (0xA326FC, "48898bc0000000", "leaf: [obj+0xc0] = 0x15d1188"),
    (0xA32703, "48c783b000000000000000", "leaf: [obj+0xb0] = 0"),
    (0xA3270E, "48c783c800000000000000", "leaf: [obj+0xc8] = 0"),
    (0xA32719, "c4c178104508", "leaf: xmm0 = the argument block +0x08"),
    (0xA32732, "498b4518", "leaf: q[3] of the argument block"),
    (0xA3273D, "4885c0", "leaf: test q[3]"),
    (0xA32740, "740a", "leaf: je (no self pointer)"),
    (0xA327B4, "488b4598", "leaf: the local functor tag"),
    (0xA327B8, "ff5018", "leaf: destroy the local functor"),
]

# The three target classes and their 0x26ea0 gate answers (the gate bodies are pinned as bytes:
# `mov eax,imm32; ret` for the two 6-byte returns and `xor eax,eax; ret` for the 3-byte zero).
CLASSES = [
    # (vptr, dtor slot 0, gate3 site, gate3 rva, gate3 bytes, gate6 site, gate6 rva, gate6 bytes)
    (0x15D11B8, 0xA2C850, 0x15D11D0, 0xA31D90, "b801000000c3", 0x15D11E8, 0xA31F10, "b801000000c3"),
    (0x1594218, 0x20B0, 0x1594230, 0xB890, "b801000000c3", 0x1594248, 0xB930, "31c0c3"),
    (0x1594DD0, 0x4B0, 0x1594DE8, 0x22AE0, "31c0c3", None, None, None),
]
# Mid-construction tags that must NOT answer the gate.
TRANSIENT = [(0x15D1120, 0x15D1110), (0x15940B0, 0x15940A0)]

# Strings of the block: (address, text).
STRINGS = [
    (0x12C1820, b"loc_text"),
    (0x12C1829, b"Change language for Text usage"),
    (0x12C1848, b"loc_subtitles"),
    (0x12C1856, b"Change language for Subtitles usage"),
    (0x12C187A, b"loc_speech"),
    (0x12C1885, b"Change language for Speech usage"),
    (0x12C18A6, b"loc_system"),
    (0x12C18B1, b"Change language for System usage"),
    (0x12C18D2, b"sLanguageUnlocalizedAsEnglish"),
    (0x12C18F0, b"loc_reload"),
    (0x12C18FB, b"Reloads localization data from disk"),
    (0x12C191F, b"Localization"),
    (0x12C192C, b"Language Text"),
    (0x12C193A, b"Language Subtitles"),
    (0x12C194D, b"Language Speech"),
    (0x12C195D, b"Language System"),
    (0x12C196D, b"Use English instead of Unlocalized"),
    (0x12C1990, b"Reload"),
    (0x12C1BB2, b"!!!Unlocalized!!!"),
    (0x12C1BFB, b""),
]

# The two resolved GOT values the body carries, as RELATIVE relocations.
GOT_VALUES = [(0x15FFEF0, 0xA324A0), (0x15FE5E0, 0x1990)]


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
    require(TABLE_TOP - 8 * TABLE_INDEX == TABLE_WORD == 0x2DD6700, "table row arithmetic")
    require(e.u64(TABLE_WORD) == RVA, "constructor-table entry 316 -> 0xa33280")
    # The next two indices are NOT the cliff: 317 / 318 are registered startup slots already.
    require(e.u64(TABLE_WORD - 8) == 0xA2C390, "entry 317 is the already-registered 0xa2c390")
    require(e.u64(TABLE_WORD - 16) == 0xA20E80, "entry 318 is the already-registered 0xa20e80")

    # --- body extent, digest, padding ---------------------------------------------------------
    body = e.read(RVA, SIZE)
    if not e.skip_hash:
        require(digest(body) == SHA, "body sha256 (2561 B FDE)")
        require(digest(body[:REACHABLE]) == SHA_REACHABLE, "reachable body sha256 (2560 B)")
    require(body[0] == 0x55 and body[1:4] == b"\x48\x89\xe5", "push rbp; mov rsp,rbp")
    require(body[-1] == 0x90, "the body's last byte is the trailing nop")
    require(e.read(RVA + SIZE, 4) == b"\x90\x90\x90\x90", "nop padding after the body")
    require(fde_range(e, RVA) == SIZE, "FDE address_range == 2561")

    # --- every direct call, every rip operand, the literal bytes -------------------------------
    require(len(CALLS) == 54 and len({site for site, _ in CALLS}) == 54, "54 distinct call sites")
    for site, target in CALLS:
        require(e.call(site) == target, f"call at 0x{site:x} -> 0x{target:x}")
    require(len(RIPS) == 88 and len({site for site, _, _, _ in RIPS}) == 88, "88 distinct rip sites")
    for site, length, disp_off, target in RIPS:
        require(e.rip(site, length, disp_off) == target, f"rip operand at 0x{site:x} -> 0x{target:x}")
    require(len({target for _, _, _, target in RIPS}) == 62, "62 distinct rip targets")
    for site, want, what in BYTES_PINS:
        e.bytes_at(site, want, what)

    # --- the two branches, decoded -------------------------------------------------------------
    inner = e.read(0xA338A7, 2)
    require(inner[0] == 0x74, "the functor self-pointer skip is je rel8 at 0xa338a7")
    require(0xA338A7 + 2 + struct.unpack("<b", inner[1:2])[0] == 0xA338B1, "je target 0xa338b1")
    branch = e.read(0xA33C6A, 2)
    require(branch[0] == 0x75, "the guard branch is jne rel8 at 0xa33c6a")
    require(0xA33C6A + 2 + struct.unpack("<b", branch[1:2])[0] == 0xA33C7B, "jne target 0xa33c7b")
    require(e.call(0xA33C7B) == 0xF8F3A0, "the branch target calls __stack_chk_fail")

    # --- the four row-local indirect sites and the one command-register site -------------------
    for site, tag, slot3 in INDIRECT:
        require(e.read(site, 3) == b"\xff\x50\x18", f"0x{site:x} is `callq *0x18(%rax)`")
        require(e.reloc_target(tag + 0x18) == slot3, f"tag 0x{tag:x} slot 3 -> 0x{slot3:x}")
        e.bytes_at(slot3, "c3", f"functor destroy 0x{slot3:x} is a bare ret")
    site, table, off, target = COMMAND_REGISTER
    require(e.read(site, 3) == b"\xff\x50\x20", f"0x{site:x} is `callq *0x20(%rax)`")
    require(e.reloc_target(table + off) == target, f"0x{table:x}+0x{off:x} -> 0x{target:x}")

    # --- the 14 __cxa_atexit registrations, in body order ---------------------------------------
    atexit_sites = [site for site, target in CALLS if target == 0xF8F390]
    require(atexit_sites == [site for site, _, _ in ATEXIT], "the 14 atexit sites in body order")
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

    # --- the language-variable constructor pair -------------------------------------------------
    require(fde_range(e, WRAPPER) == WRAPPER_SIZE, "wrapper 0xa324c0 FDE == 436")
    require(fde_range(e, LEAF) == LEAF_SIZE, "leaf 0xa32680 FDE == 352")
    require(digest(e.read(WRAPPER, WRAPPER_SIZE)) == WRAPPER_SHA, "wrapper 0xa324c0 sha256")
    require(digest(e.read(LEAF, LEAF_SIZE)) == LEAF_SHA, "leaf 0xa32680 sha256")
    for site, length, disp_off, target in WRAPPER_RIPS:
        require(e.rip(site, length, disp_off) == target, f"wrapper rip 0x{site:x} -> 0x{target:x}")
    for site, target in WRAPPER_CALLS:
        require(e.call(site) == target, f"wrapper call 0x{site:x} -> 0x{target:x}")
    for site, length, disp_off, target in LEAF_RIPS:
        require(e.rip(site, length, disp_off) == target, f"leaf rip 0x{site:x} -> 0x{target:x}")
    for site, target in LEAF_CALLS:
        require(e.call(site) == target, f"leaf call 0x{site:x} -> 0x{target:x}")
    for site, want, what in OBJECT_BYTES:
        e.bytes_at(site, want, what)
    # The two vptr tables: address point = table + 0x10 and slot 0 is the class destructor.
    require(e.reloc_target(0x15D11A8 + 0x10) == 0xA2C850, "final vptr 0x15d11b8 slot 0 is the dtor")
    require(e.reloc_target(0x15D1110 + 0x10) == 0xA31D10, "mid vptr 0x15d1120 slot 0 is 0xa31d10")

    # --- the functor class and its four lifted bodies --------------------------------------------
    for slot, body in enumerate(LANGUAGE_FN_SLOTS):
        require(e.reloc_target(0x15D1188 + 8 * slot) == body,
                f"functor table 0x15d1188 slot {slot} -> 0x{body:x}")
    for addr, (size, head) in TINY_BODIES.items():
        require(fde_range(e, addr) == size, f"tiny body 0x{addr:x} FDE == {size}")
        e.bytes_at(addr, head, f"tiny body 0x{addr:x} head")

    # --- the three target classes and their gate answers ------------------------------------------
    for vptr, dtor, g3_site, g3_rva, g3_bytes, g6_site, g6_rva, g6_bytes in CLASSES:
        require(e.reloc_target(vptr) == dtor, f"vptr 0x{vptr:x} slot 0 -> 0x{dtor:x}")
        require(e.reloc_target(g3_site) == g3_rva, f"slot 3 of 0x{vptr:x} -> 0x{g3_rva:x}")
        e.bytes_at(g3_rva, g3_bytes, f"gate 3 body 0x{g3_rva:x}")
        if g6_site is not None:
            require(e.reloc_target(g6_site) == g6_rva, f"slot 6 of 0x{vptr:x} -> 0x{g6_rva:x}")
            e.bytes_at(g6_rva, g6_bytes, f"gate 6 body 0x{g6_rva:x}")
        else:
            # slot 6 of the command class is never read by 0x26ea0 (slot 3 != 1 short-circuits).
            require(vptr + 0x30 == 0x1594E00, "the unread slot-6 address")

    # --- the resolved GOT values the body carries ------------------------------------------------
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
        print("SKIP check_ctor_slot_316: local/re/eboot.elf not present")
        return 0
    e = Elf()
    texts = read_native()
    if "--tamper" in arguments:
        mode = arguments[arguments.index("--tamper") + 1] if len(arguments) > arguments.index("--tamper") + 1 else "elf"
        if mode == "elf":
            e.overlay = {RVA + 0x10: e.raw(RVA + 0x10, 1)[0] ^ 0x40}
        else:
            texts = dict(texts)
            texts["header"] = texts["header"].replace("kSlot316Rva", "kTampered", 1)
    e.recording = True
    run(e, texts)
    e.recording = False
    note = ""
    if "--no-mutate" not in arguments and "--tamper" not in arguments:
        elf_runs, native_runs = mutation_self_test(e, texts)
        note = f"; mutation self-test {elf_runs} ELF + {native_runs} native mutants all rejected"
    print(f"OK check_ctor_slot_316: slot 316 (0xa33280, {SIZE} B FDE, {REACHABLE} B reachable, sha "
          f"{SHA[:12]}...) is row 316 of the constructor table at 0x{TABLE_WORD:x}; 54 direct calls, 88 "
          f"rip operands, 2 branches (a338a7 skip / a33c6a guard -> 0xa33c7b __stack_chk_fail), 4 "
          f"`callq *0x18(%rax)` inert functor destroys + 1 `callq *0x20(%rax)` register (0x20d60), 14 "
          f"__cxa_atexit rows, the 436 B/352 B language-variable ctor pair with its four lifted functor "
          f"bodies, and the three gate classes {{1,1}}/{{1,0}}/{{0,-}}; guest bytes NOT executed{note}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
