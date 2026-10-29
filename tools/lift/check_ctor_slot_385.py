#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 385 (RVA 0x6b4cf0, 296 bytes), lane M1.

Console command `gCP10ExtractCP3SERandomTables` (flags 2, handler tag 0x6b4600, atexit 0x4b0).

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
    "slot": 385,
    "base": 0x6b4cf0,
    "size": 296,
    "body_sha256": "d30dd5cd3f42992ce29e0ff2f86e6295a78cec29c63bd8b23876f3532a4f6730",
    "pad": "9090909090909090",
    "callees": ['concmd_base_ctor', 'concmd_register', 'cmd_dtor', 'release_f590'],
    "bool_tags": [],
    "float_tags": [],
    "strings": [
        (0x127387d, b'gCP10ExtractCP3SERandomTables'),
        (0x127389b, b'Dumps se-random-def tables to output.'),
    ],
    "lengths": [
        ('Dumps se-random-def tables to output.', 0x6b4d59, "48c744243825000000"),
    ],
    "pins": [
        ("rip", 0x6b4d04, 7, 3, 0x15fe518),
        ("rip", 0x6b4d10, 7, 3, 0x127387d),
        ("call", 0x6b4d24, 0xf2f0),
        ("rip", 0x6b4d29, 7, 3, 0x3238340),
        ("rip", 0x6b4d30, 8, 4, 0x15fe5e0),
        ("rip", 0x6b4d38, 7, 3, 0x127389b),
        ("imm", 0x6b4d48, "b902000000"),
        ("imm", 0x6b4d59, "48c744243825000000"),
        ("rip", 0x6b4d6c, 7, 3, 0x6b4600),
        ("call", 0x6b4d81, 0x20af0),
        ("rip", 0x6b4d86, 7, 3, 0x1594dc0),
        ("imm", 0x6b4dc6, "48c7839000000000000000"),
        ("imm", 0x6b4dd1, "ff5020"),
        ("call", 0x6b4dd9, 0xf590),
        ("call", 0x6b4de3, 0xf590),
        ("rip", 0x6b4de8, 7, 3, 0x4b0),
        ("rip", 0x6b4def, 7, 3, 0x1604218),
        ("call", 0x6b4df9, 0xf8f390),
        ("call", 0x6b4e13, 0xf8f3a0),
    ],
    "native": {'header': 'native/core/ctor_slot_385_6b4cf0.hpp', 'source': 'native/core/ctor_slot_385_6b4cf0.cpp', 'test': 'native/tests/test_ctor_slot_385.cpp', 'cmake': 'native/lanes/m1.cmake', 'consts': {'kSlot385Rva': 7032048, 'kSlot385Size': 296}, 'header_tokens': ['kSlot385HandlerTag = 0x6b4600', 'kSlot385CommandDescriptionLength = 0x25', 'kSlot385ObjectRva = 0x3238340'], 'source_order': ['init_fconcmd_object(objects.command, spec, concmd)'], 'test_tokens': ['kSlot385Rva == 0x6b4cf0', 'section_alone()', 'section_failures()'], 'cmake_tokens': ['core/ctor_slot_385_6b4cf0.cpp', 'add_executable(test_ctor_slot_385 tests/test_ctor_slot_385.cpp)', 'add_test(NAME ctor_slot_385 COMMAND test_ctor_slot_385)']},
}


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, 'Console command `gCP10ExtractCP3SERandomTables` (flags 2, handler tag 0x6b4600, atexit 0x4b0).'))
