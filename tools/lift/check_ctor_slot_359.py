#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 359 (RVA 0x785f60, 296 bytes), lane M1.

Console command `giveItem` (flags 2, handler tag 0x7836d0, atexit 0x4b0).

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
    "slot": 359,
    "base": 0x785f60,
    "size": 296,
    "body_sha256": "769cf4cf86539db28d383f388910ab895a5b633d8b08deaed7081666e7964965",
    "pad": "9090909090909090",
    "callees": ['concmd_base_ctor', 'concmd_register', 'cmd_dtor', 'release_f590'],
    "bool_tags": [],
    "float_tags": [],
    "strings": [
        (0x12833bb, b'giveItem'),
        (0x12833c4, b'Give the player the indicated item. Params: <item_index> <from_hard_list>'),
    ],
    "lengths": [
        ('Give the player the indicated item. Params: <item_index> <from_hard_list>', 0x785fc9, "48c744243849000000"),
    ],
    "pins": [
        ("rip", 0x785f74, 7, 3, 0x15fe518),
        ("rip", 0x785f80, 7, 3, 0x12833bb),
        ("call", 0x785f94, 0xf2f0),
        ("rip", 0x785f99, 7, 3, 0x3255c30),
        ("rip", 0x785fa0, 8, 4, 0x15fe5e0),
        ("rip", 0x785fa8, 7, 3, 0x12833c4),
        ("imm", 0x785fb8, "b902000000"),
        ("imm", 0x785fc9, "48c744243849000000"),
        ("rip", 0x785fdc, 7, 3, 0x7836d0),
        ("call", 0x785ff1, 0x20af0),
        ("rip", 0x785ff6, 7, 3, 0x1594dc0),
        ("imm", 0x786036, "48c7839000000000000000"),
        ("imm", 0x786041, "ff5020"),
        ("call", 0x786049, 0xf590),
        ("call", 0x786053, 0xf590),
        ("rip", 0x786058, 7, 3, 0x4b0),
        ("rip", 0x78605f, 7, 3, 0x1604218),
        ("call", 0x786069, 0xf8f390),
        ("call", 0x786083, 0xf8f3a0),
    ],
    "native": {'header': 'native/core/ctor_slot_359_785f60.hpp', 'source': 'native/core/ctor_slot_359_785f60.cpp', 'test': 'native/tests/test_ctor_slot_359.cpp', 'cmake': 'native/lanes/m1.cmake', 'consts': {'kSlot359Rva': 7888736, 'kSlot359Size': 296}, 'header_tokens': ['kSlot359HandlerTag = 0x7836d0', 'kSlot359CommandDescriptionLength = 0x49', 'kSlot359ObjectRva = 0x3255c30'], 'source_order': ['init_fconcmd_object(objects.command, spec, concmd)'], 'test_tokens': ['kSlot359Rva == 0x785f60', 'section_alone()', 'section_failures()'], 'cmake_tokens': ['core/ctor_slot_359_785f60.cpp', 'add_executable(test_ctor_slot_359 tests/test_ctor_slot_359.cpp)', 'add_test(NAME ctor_slot_359 COMMAND test_ctor_slot_359)']},
}


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, 'Console command `giveItem` (flags 2, handler tag 0x7836d0, atexit 0x4b0).'))
