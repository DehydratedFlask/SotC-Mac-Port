#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 570 (RVA 0x16d80, 740 B, index 570).

Batch 21, slot 570: three bespoke preamble tables (four flag dwords, four zero cells, the xchg cell,
the self-referential sentinel block, a zeroed 0x18-byte block and the 0x18-byte triple whose third
qword is a back pointer to the bool variable), each table followed by its own __cxa_atexit record
(0x11070 / 0x11080 / 0x11090, identity tags), then the bool variable `mem_Verify` (functor tag
0x15949f0 = the bool family 0xc2a0 / 0xc2c0 / 0xc2e0 / 0xc2f0, class 0x1594218, dtor 0x20b0) and the
debug-menu toggle `Memory` / `Verify Memory Integrity` (0x26ea0, flags 0x30, dtor 0x272d0).

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes
an original RVA. Vtables, functor tags and destructor RVAs are IDENTITY TAGS, recorded and never run.
The M6 driver runs a fail-closed mutation self-test; --tamper elf|native are explicit rejection probes.
The body has exactly one conditional branch (0x1704d, the __stack_chk_guard compare).
"""

from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import l4_slot_lib as lib  # noqa: E402

SPEC = {
    "label": "570",
    "slot": 570,
    "base": 0x16d80,
    "size": 740,
    "body_sha256": "506dcf2a0047d9d6f34b85c76d34eb755332b6878afcdfd51d748b2055c18255",
    "pad": "90909090",
    "callees": [
        "concmd_base_ctor", "concmd_register", "bool_dtor", "release_f590", "menu_item_dtor",
        "menu_toggle_ctor", "bool_fn_copy", "bool_fn_clone", "bool_fn_move", "bool_fn_destroy",
        "bool_slot3", "bool_slot6",
    ],
    "bool_tags": [0x15949F0],
    "float_tags": [],
    "strings": [
        (0xF9489C, b"mem_Verify"),
        (0xF948A7, b"Memory"),
        (0xF948AE, b"Verify Memory Integrity"),
        (0xF94B3F, b""),
    ],
    "lengths": [],
    "pins": [
        # frame
        ("rip", 0x16D91, 7, 3, 0x15FE518),   # __stack_chk_guard cell
        ("rip", 0x16D9C, 7, 3, 0x1604218),   # __dso_handle
        # identity record 0 and the flags / cells / xchg tables
        ("rip", 0x16DA3, 7, 3, 0x11070),
        ("rip", 0x16DAA, 7, 3, 0x2DD9FF0),
        ("rip", 0x16DBB, 7, 3, 0x2DD9F80),
        ("rip", 0x16DC8, 7, 3, 0x2DD9F84),
        ("rip", 0x16DD5, 7, 3, 0x2DD9F88),
        ("rip", 0x16DE2, 7, 3, 0x2DD9F8C),
        ("rip", 0x16DEF, 7, 3, 0x2DD9F90),
        ("rip", 0x16E01, 7, 3, 0x2DD9FA8),
        ("rip", 0x16E13, 7, 3, 0x2DD9FC0),
        ("rip", 0x16E25, 7, 3, 0x2DD9FD8),
        ("rip", 0x16E3D, 7, 3, 0x2DD9FF0),   # xchgq %rax, cell
        # the sentinel block and its self pointer
        ("rip", 0x16E44, 7, 3, 0x2DDA038),
        ("rip", 0x16E4B, 11, 3, 0x2DDA000),  # movq $0
        ("rip", 0x16E56, 11, 3, 0x2DDA028),  # movq $-1
        ("rip", 0x16E73, 7, 3, 0x2DDA020),   # movq %rax (the self pointer)
        # identity records 1 and 2, and the two zeroed blocks
        ("rip", 0x16E89, 7, 3, 0x11080),
        ("rip", 0x16E94, 7, 3, 0x2DFA440),
        ("rip", 0x16EB6, 7, 3, 0x11090),
        ("rip", 0x16EBD, 7, 3, 0x2DFA480),
        ("rip", 0x16EC4, 7, 3, 0x2DFA460),
        ("rip", 0x16ED3, 7, 3, 0x2DFA930),   # the back pointer (the bool variable)
        ("rip", 0x16EE7, 7, 3, 0x2DFA470),
        # the bool variable
        ("rip", 0x16EF7, 7, 3, 0xF9489C),    # name `mem_Verify`
        ("rip", 0x16F08, 7, 3, 0x1594208),   # the class vtable the body derives 0x1594218 from
        ("rip", 0x16F0F, 7, 3, 0x2DFA930),
        ("rip", 0x16F1A, 7, 3, 0xF94B3F),    # the shared empty description
        ("rip", 0x16F50, 7, 3, 0x15940A0),
        ("rip", 0x16F65, 7, 3, 0x15949F0),   # the caller argument-functor tag
        # the toggle
        ("rip", 0x16FD9, 7, 3, 0xF948A7),    # label `Memory`
        ("rip", 0x16FEE, 7, 3, 0xF948AE),    # text `Verify Memory Integrity`
        ("rip", 0x16FFF, 7, 3, 0x2DFA9F8),
        ("rip", 0x1703F, 7, 3, 0x15FE518),   # the closing guard load
        # direct calls, in body order
        ("call", 0x16E84, 0xF8F390), ("call", 0x16EB1, 0xF8F390), ("call", 0x16EEE, 0xF8F390),
        ("call", 0x16F03, 0xF2F0), ("call", 0x16F4B, 0x20AF0), ("call", 0x16FA0, 0xF590),
        ("call", 0x16FB6, 0x20D60), ("call", 0x16FBE, 0xF590), ("call", 0x16FD0, 0xF8F390),
        ("call", 0x16FE5, 0xF2F0), ("call", 0x16FFA, 0xF2F0), ("call", 0x17018, 0x26EA0),
        ("call", 0x17020, 0xF590), ("call", 0x17028, 0xF590), ("call", 0x1703A, 0xF8F390),
        # the single conditional branch: the canary compare, whose target is the __stack_chk_fail tail
        ("imm", 0x1704D, "750f"),   # short `jne 0x1705e`
    ],
    "native": {
        "header": "native/core/ctor_slot_570_16d80.hpp",
        "source": "native/core/ctor_slot_570_16d80.cpp",
        "test": "native/tests/test_ctor_slot_570.cpp",
        "cmake": "native/CMakeLists.txt",
        "consts": {
            "kSlot570Rva": 0x16D80,
            "kSlot570Size": 740,
            "kSlot570TableWord": 0x2DD5F10,
            "kSlot570FlagCell0": 0x2DD9F80,
            "kSlot570FlagCount": 4,
            "kSlot570ZeroCell0": 0x2DD9F90,
            "kSlot570ZeroCellStride": 0x18,
            "kSlot570ZeroCellCount": 4,
            "kSlot570XchgCell": 0x2DD9FF0,
            "kSlot570SentinelBlock": 0x2DDA000,
            "kSlot570SentinelSelf": 0x2DDA038,
            "kSlot570SentinelOnes": 0xFFFFFFFFFFFFFFFF,
            "kSlot570SentinelCapacity": 0x80,
            "kSlot570ZeroBlock2": 0x2DFA440,
            "kSlot570TripleBlock": 0x2DFA460,
            "kSlot570TripleSelf0": 0x2DFA480,
            "kSlot570TripleSelf1": 0x2DFA480,
            "kSlot570TripleBackPtr": 0x2DFA930,
            "kSlot570IdentityDtor0": 0x11070,
            "kSlot570IdentityDtor1": 0x11080,
            "kSlot570IdentityDtor2": 0x11090,
            "kSlot570BoolRva": 0x2DFA930,
            "kSlot570BoolFnTag": 0x15949F0,
            "kSlot570BoolClassVptr": 0x1594218,
            "kSlot570BoolDtor": 0x20B0,
            "kSlot570BoolNameLength": 0xA,
            "kSlot570BoolValue": 0,
            "kSlot570BoolDefault": 0,
            "kSlot570AtexitCount": 5,
            "kSlot570ToggleRva": 0x2DFA9F8,
            "kSlot570ToggleLabelLength": 6,
            "kSlot570ToggleTextLength": 0x17,
            "kSlot570ToggleFlags": 0x30,
            "kSlot570MenuDtor": 0x272D0,
        },
        "header_tokens": [
            "xchgq", "0x2dd9ff0", "0x2dda038", "0x2dfa480", "0x2dfa930",
            "0x15949f0", "0x1594218", "0x20b0", "0x272d0", "not_written", "gap08",
            "INFERENCE",
        ],
        "source_order": ["write_preamble", "init_bool_var_literal", "family_toggle_item_26ea0"],
        "test_tokens": [
            "kSlot570AtexitCount", "xchg_previous", "kSlot570IdentityDtor0",
            "kSlot570IdentityDtor1", "kSlot570IdentityDtor2", "kSlot570ToggleText",
        ],
        "cmake_tokens": ["ctor_slot_570_16d80.cpp", "test_ctor_slot_570.cpp", "ctor_slot_570"],
    },
}

SUMMARY = ("three bespoke preamble tables (4 flags, 4 zero cells, xchg, self-referential sentinel, "
           "zeroed 0x18 block, 0x18 triple with the back pointer) + `mem_Verify` bool + `Memory` toggle")

if __name__ == "__main__":
    sys.exit(lib.main(SPEC, SUMMARY))
