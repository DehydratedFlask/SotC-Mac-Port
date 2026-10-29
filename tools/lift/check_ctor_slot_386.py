#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 386 (RVA 0x6b12f0, 296 bytes), lane M1.

Console command `shadow_printCurrentlyLoadedSheetData` (flags 2, handler tag 0x6aee10, atexit 0x4b0).

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an
original RVA. Pins: constructor-table word, whole-body sha256 and extent, every rip-relative operand, call
target, immediate store and indirect call of the body (generated once from the disassembly, re-verified here
from bytes), the referenced strings, the functor vtables and the hash of every shared callee. A mutation
self-test flips the first/last byte of every pin and perturbs every native token; each mutant must be rejected.
Uses the declarative driver tools/lift/l4_slot_lib.py unchanged.
"""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import l4_slot_lib as lib  # noqa: E402

SPEC = {
    "slot": 386,
    "base": 0x6b12f0,
    "size": 296,
    "body_sha256": "5b5edb4436fc795c80f992957c37a904ab28d4c8c321b8b331ce8d303314cfbb",
    "pad": "9090909090909090",
    "callees": ['concmd_base_ctor', 'concmd_register', 'cmd_dtor', 'release_f590'],
    "bool_tags": [],
    "float_tags": [],
    "strings": [
        (0x127344e, b'shadow_printCurrentlyLoadedSheetData'),
        (0x1273473, b''),
    ],
    "lengths": [

    ],
    "pins": [
        ("rip", 0x6b1304, 7, 3, 0x15fe518),
        ("rip", 0x6b1310, 7, 3, 0x127344e),
        ("call", 0x6b1324, 0xf2f0),
        ("rip", 0x6b1329, 7, 3, 0x3017de0),
        ("rip", 0x6b1330, 8, 4, 0x15fe5e0),
        ("rip", 0x6b1338, 7, 3, 0x1273473),
        ("imm", 0x6b1348, "b902000000"),
        ("imm", 0x6b135f, "48c744244800000000"),
        ("rip", 0x6b136c, 7, 3, 0x6aee10),
        ("call", 0x6b1381, 0x20af0),
        ("rip", 0x6b1386, 7, 3, 0x1594dc0),
        ("imm", 0x6b13c6, "48c7839000000000000000"),
        ("imm", 0x6b13d1, "ff5020"),
        ("call", 0x6b13d9, 0xf590),
        ("call", 0x6b13e3, 0xf590),
        ("rip", 0x6b13e8, 7, 3, 0x4b0),
        ("rip", 0x6b13ef, 7, 3, 0x1604218),
        ("call", 0x6b13f9, 0xf8f390),
        ("call", 0x6b1413, 0xf8f3a0),
    ],
    "native": {'header': 'native/core/ctor_slot_386_6b12f0.hpp', 'source': 'native/core/ctor_slot_386_6b12f0.cpp', 'test': 'native/tests/test_ctor_slot_386.cpp', 'cmake': 'native/lanes/m1.cmake', 'consts': {'kSlot386Rva': 7017200, 'kSlot386Size': 296}, 'header_tokens': ['kSlot386HandlerTag = 0x6aee10', 'kSlot386CommandDescriptionLength = 0x0', 'kSlot386ObjectRva = 0x3017de0'], 'source_order': ['init_fconcmd_object(objects.command, spec, concmd)'], 'test_tokens': ['kSlot386Rva == 0x6b12f0', 'section_alone()', 'section_failures()'], 'cmake_tokens': ['core/ctor_slot_386_6b12f0.cpp', 'add_executable(test_ctor_slot_386 tests/test_ctor_slot_386.cpp)', 'add_test(NAME ctor_slot_386 COMMAND test_ctor_slot_386)']},
}


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, 'Console command `shadow_printCurrentlyLoadedSheetData` (flags 2, handler tag 0x6aee10, atexit 0x4b0).'))
