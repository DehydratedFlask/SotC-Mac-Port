#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 387 (RVA 0x6acf00, 299 bytes), lane M1.

Single bool console variable `CP3FileResource_copy_mem` (object 0x3017890, value 1/1, functor tag 0x15cc748).

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
    "slot": 387,
    "base": 0x6acf00,
    "size": 299,
    "body_sha256": "f0f771ce500a790e06f9e8f0653328f02004adb938fe3d344709e8a7116e33a4",
    "pad": "9090909090",
    "callees": ['concmd_base_ctor', 'concmd_register', 'bool_dtor', 'release_f590', 'bool_fn_copy', 'bool_fn_clone', 'bool_fn_move', 'bool_fn_destroy', 'bool_slot3', 'bool_slot6'],
    "bool_tags": [22857544],
    "float_tags": [],
    "strings": [
        (0x1272eff, b'CP3FileResource_copy_mem'),
        (0x12730ef, b''),
    ],
    "lengths": [
        ('CP3FileResource_copy_mem', 0x6acf62, "48c7459818000000"),
    ],
    "pins": [
        ("rip", 0x6acf11, 7, 3, 0x15fe518),
        ("rip", 0x6acf18, 7, 3, 0x1594208),
        ("rip", 0x6acf1f, 7, 3, 0x3017890),
        ("rip", 0x6acf26, 7, 3, 0x1272eff),
        ("rip", 0x6acf2d, 7, 3, 0x12730ef),
        ("imm", 0x6acf40, "b901000000"),
        ("imm", 0x6acf62, "48c7459818000000"),
        ("imm", 0x6acf77, "48c745c800000000"),
        ("call", 0x6acf7f, 0x20af0),
        ("rip", 0x6acf84, 7, 3, 0x15940a0),
        ("rip", 0x6acf99, 7, 3, 0x15cc748),
        ("imm", 0x6acfab, "c7436900000000"),
        ("imm", 0x6acfb7, "48c783a000000000000000"),
        ("imm", 0x6acfc9, "48c783b800000000000000"),
        ("call", 0x6acfd4, 0xf590),
        ("imm", 0x6acfdf, "c683c000000001"),
        ("imm", 0x6acfe6, "c6436801"),
        ("call", 0x6acfea, 0x20d60),
        ("call", 0x6acff2, 0xf590),
        ("rip", 0x6acff7, 7, 3, 0x20b0),
        ("rip", 0x6acffe, 7, 3, 0x1604218),
        ("call", 0x6ad008, 0xf8f390),
        ("call", 0x6ad026, 0xf8f3a0),
    ],
    "native": {'header': 'native/core/ctor_slot_387_6acf00.hpp', 'source': 'native/core/ctor_slot_387_6acf00.cpp', 'test': 'native/tests/test_ctor_slot_387.cpp', 'cmake': 'native/lanes/m1.cmake', 'consts': {'kSlot387Rva': 6999808, 'kSlot387Size': 299}, 'header_tokens': ['kSlot387FnTag = 0x15cc748', 'kSlot387ObjectRva = 0x3017890'], 'source_order': ['init_bool_var_site(objects.var, kSlot387Name, kSlot387FnTag, concmd, 1, 1)'], 'test_tokens': ['kSlot387Rva == 0x6acf00', 'section_alone()', 'section_failures()', 'section_with_neighbour()'], 'cmake_tokens': ['core/ctor_slot_387_6acf00.cpp', 'add_executable(test_ctor_slot_387 tests/test_ctor_slot_387.cpp)', 'add_test(NAME ctor_slot_387 COMMAND test_ctor_slot_387)']},
}


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, 'Single bool console variable `CP3FileResource_copy_mem` (object 0x3017890, value 1/1, functor tag 0x15cc748).'))
