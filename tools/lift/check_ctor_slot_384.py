#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 384 (RVA 0x6b9260, 299 bytes), lane M1.

Single bool console variable `shadow_debugDrawDamageBox` (object 0x3238dc0, value 0/0, functor tag 0x15cc7b8).

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
    "slot": 384,
    "base": 0x6b9260,
    "size": 299,
    "body_sha256": "25932f8b8fab53c86be14d2661de44efef0c75e02d16badd4a4d938647edc73d",
    "pad": "9090909090",
    "callees": ['concmd_base_ctor', 'concmd_register', 'bool_dtor', 'release_f590', 'bool_fn_copy', 'bool_fn_clone', 'bool_fn_move', 'bool_fn_destroy', 'bool_slot3', 'bool_slot6'],
    "bool_tags": [22857656],
    "float_tags": [],
    "strings": [
        (0x1274340, b'shadow_debugDrawDamageBox'),
        (0x1274365, b''),
    ],
    "lengths": [
        ('shadow_debugDrawDamageBox', 0x6b92c2, "48c7459819000000"),
    ],
    "pins": [
        ("rip", 0x6b9271, 7, 3, 0x15fe518),
        ("rip", 0x6b9278, 7, 3, 0x1594208),
        ("rip", 0x6b927f, 7, 3, 0x3238dc0),
        ("rip", 0x6b9286, 7, 3, 0x1274340),
        ("rip", 0x6b928d, 7, 3, 0x1274365),
        ("imm", 0x6b92a0, "b901000000"),
        ("imm", 0x6b92c2, "48c7459819000000"),
        ("imm", 0x6b92d7, "48c745c800000000"),
        ("call", 0x6b92df, 0x20af0),
        ("rip", 0x6b92e4, 7, 3, 0x15940a0),
        ("rip", 0x6b92f9, 7, 3, 0x15cc7b8),
        ("imm", 0x6b930b, "c7436900000000"),
        ("imm", 0x6b9317, "48c783a000000000000000"),
        ("imm", 0x6b9329, "48c783b800000000000000"),
        ("call", 0x6b9334, 0xf590),
        ("imm", 0x6b933f, "c683c000000000"),
        ("imm", 0x6b9346, "c6436800"),
        ("call", 0x6b934a, 0x20d60),
        ("call", 0x6b9352, 0xf590),
        ("rip", 0x6b9357, 7, 3, 0x20b0),
        ("rip", 0x6b935e, 7, 3, 0x1604218),
        ("call", 0x6b9368, 0xf8f390),
        ("call", 0x6b9386, 0xf8f3a0),
    ],
    "native": {'header': 'native/core/ctor_slot_384_6b9260.hpp', 'source': 'native/core/ctor_slot_384_6b9260.cpp', 'test': 'native/tests/test_ctor_slot_384.cpp', 'cmake': 'native/lanes/m1.cmake', 'consts': {'kSlot384Rva': 7049824, 'kSlot384Size': 299}, 'header_tokens': ['kSlot384FnTag = 0x15cc7b8', 'kSlot384ObjectRva = 0x3238dc0'], 'source_order': ['init_bool_var_site(objects.var, kSlot384Name, kSlot384FnTag, concmd, 0, 0)'], 'test_tokens': ['kSlot384Rva == 0x6b9260', 'section_alone()', 'section_failures()', 'section_with_neighbour()'], 'cmake_tokens': ['core/ctor_slot_384_6b9260.cpp', 'add_executable(test_ctor_slot_384 tests/test_ctor_slot_384.cpp)', 'add_test(NAME ctor_slot_384 COMMAND test_ctor_slot_384)']},
}


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, 'Single bool console variable `shadow_debugDrawDamageBox` (object 0x3238dc0, value 0/0, functor tag 0x15cc7b8).'))
