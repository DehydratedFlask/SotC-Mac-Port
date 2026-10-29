#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 376 (RVA 0x6e4930, 299 bytes), lane M1.

Single bool console variable `skippableEndSequence` (object 0x32415d0, value 0/0, functor tag 0x15cc9b8).

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
    "slot": 376,
    "base": 0x6e4930,
    "size": 299,
    "body_sha256": "f9890b809f294b926397904f9ca916ac423d80851a787732652561f24f3434b0",
    "pad": "9090909090",
    "callees": ['concmd_base_ctor', 'concmd_register', 'bool_dtor', 'release_f590', 'bool_fn_copy', 'bool_fn_clone', 'bool_fn_move', 'bool_fn_destroy', 'bool_slot3', 'bool_slot6'],
    "bool_tags": [22858168],
    "float_tags": [],
    "strings": [
        (0x1276990, b'skippableEndSequence'),
        (0x1276a2f, b''),
    ],
    "lengths": [
        ('skippableEndSequence', 0x6e4992, "48c7459814000000"),
    ],
    "pins": [
        ("rip", 0x6e4941, 7, 3, 0x15fe518),
        ("rip", 0x6e4948, 7, 3, 0x1594208),
        ("rip", 0x6e494f, 7, 3, 0x32415d0),
        ("rip", 0x6e4956, 7, 3, 0x1276990),
        ("rip", 0x6e495d, 7, 3, 0x1276a2f),
        ("imm", 0x6e4970, "b901000000"),
        ("imm", 0x6e4992, "48c7459814000000"),
        ("imm", 0x6e49a7, "48c745c800000000"),
        ("call", 0x6e49af, 0x20af0),
        ("rip", 0x6e49b4, 7, 3, 0x15940a0),
        ("rip", 0x6e49c9, 7, 3, 0x15cc9b8),
        ("imm", 0x6e49db, "c7436900000000"),
        ("imm", 0x6e49e7, "48c783a000000000000000"),
        ("imm", 0x6e49f9, "48c783b800000000000000"),
        ("call", 0x6e4a04, 0xf590),
        ("imm", 0x6e4a0f, "c683c000000000"),
        ("imm", 0x6e4a16, "c6436800"),
        ("call", 0x6e4a1a, 0x20d60),
        ("call", 0x6e4a22, 0xf590),
        ("rip", 0x6e4a27, 7, 3, 0x20b0),
        ("rip", 0x6e4a2e, 7, 3, 0x1604218),
        ("call", 0x6e4a38, 0xf8f390),
        ("call", 0x6e4a56, 0xf8f3a0),
    ],
    "native": {'header': 'native/core/ctor_slot_376_6e4930.hpp', 'source': 'native/core/ctor_slot_376_6e4930.cpp', 'test': 'native/tests/test_ctor_slot_376.cpp', 'cmake': 'native/lanes/m1.cmake', 'consts': {'kSlot376Rva': 7227696, 'kSlot376Size': 299}, 'header_tokens': ['kSlot376FnTag = 0x15cc9b8', 'kSlot376ObjectRva = 0x32415d0'], 'source_order': ['init_bool_var_site(objects.var, kSlot376Name, kSlot376FnTag, concmd, 0, 0)'], 'test_tokens': ['kSlot376Rva == 0x6e4930', 'section_alone()', 'section_failures()', 'section_with_neighbour()'], 'cmake_tokens': ['core/ctor_slot_376_6e4930.cpp', 'add_executable(test_ctor_slot_376 tests/test_ctor_slot_376.cpp)', 'add_test(NAME ctor_slot_376 COMMAND test_ctor_slot_376)']},
}


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, 'Single bool console variable `skippableEndSequence` (object 0x32415d0, value 0/0, functor tag 0x15cc9b8).'))
