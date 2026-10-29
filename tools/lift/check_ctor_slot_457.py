#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 457 (RVA 0x45c440, 323 bytes), lane m3.

One bool console variable `showCollisionMeshTests` (object 0x2ef80c0, value 0, default 0, functor tag 0x15bad48). Same stores as slot 348, rip-relative encoding.

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an
original RVA. Pins: constructor-table word, whole-body sha256 and extent, every rip-relative operand, call
target, immediate store and indirect call of the body (generated once from the disassembly, re-verified here
from bytes), the referenced strings, the functor vtables and the hash of every shared callee. A mutation
self-test flips the first/last byte of every pin and perturbs every native token; each mutant must be rejected.
"""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import l4_slot_lib as lib  # noqa: E402

SPEC = {
    "slot": 457,
    "base": 0x45c440,
    "size": 323,
    "body_sha256": "1643e82d14dfe426fe2dc6a3f60fa17e0509ffa35a8f19b0fda63aab44c86954",
    "pad": "90909090909090909090909090",
    "callees": ['concmd_base_ctor', 'concmd_register', 'bool_dtor', 'release_f590', 'bool_fn_copy', 'bool_fn_clone', 'bool_fn_move', 'bool_fn_destroy', 'bool_slot3', 'bool_slot6'],
    "bool_tags": [0x15bad48],
    "float_tags": [],
    "strings": [
        (0xff6e30, b'showCollisionMeshTests'),
        (0xff6fda, b''),
    ],
    "lengths": [
        ('showCollisionMeshTests', 0x45c4a2, "48c7459816000000"),
    ],
    "pins": [
        ("rip", 0x45c451, 7, 3, 0x15fe518),
        ("rip", 0x45c458, 7, 3, 0x1594208),
        ("rip", 0x45c45f, 7, 3, 0x2ef80c0),
        ("rip", 0x45c466, 7, 3, 0xff6e30),
        ("rip", 0x45c46d, 7, 3, 0xff6fda),
        ("imm", 0x45c480, "b901000000"),
        ("imm", 0x45c4a2, "48c7459816000000"),
        ("rip", 0x45c4b4, 7, 3, 0x2ef80c0),
        ("imm", 0x45c4bb, "48c745c800000000"),
        ("call", 0x45c4c3, 0x20af0),
        ("rip", 0x45c4c8, 7, 3, 0x15940a0),
        ("rip", 0x45c4d2, 7, 3, 0x2ef8180),
        ("rip", 0x45c4dd, 7, 3, 0x15bad48),
        ("rip", 0x45c4e8, 7, 3, 0x2ef80c0),
        ("rip", 0x45c4ef, 7, 3, 0x2ef8120),
        ("rip", 0x45c4f6, 10, 2, 0x2ef8129),
        ("imm", 0x45c4f6, "c70529bca90200000000"),
        ("rip", 0x45c500, 8, 4, 0x2ef8130),
        ("imm", 0x45c500, "c5fc110528bca902"),
        ("rip", 0x45c508, 11, 3, 0x2ef8160),
        ("imm", 0x45c508, "48c7054dbca90200000000"),
        ("rip", 0x45c513, 7, 3, 0x2ef8170),
        ("rip", 0x45c51a, 11, 3, 0x2ef8178),
        ("imm", 0x45c51a, "48c70553bca90200000000"),
        ("call", 0x45c525, 0xf590),
        ("rip", 0x45c52d, 7, 3, 0x2ef80c0),
        ("rip", 0x45c534, 7, 2, 0x2ef8180),
        ("imm", 0x45c534, "c60545bca90200"),
        ("rip", 0x45c53b, 7, 2, 0x2ef8128),
        ("imm", 0x45c53b, "c605e6bba90200"),
        ("call", 0x45c542, 0x20d60),
        ("call", 0x45c54a, 0xf590),
        ("rip", 0x45c54f, 7, 3, 0x20b0),
        ("rip", 0x45c556, 7, 3, 0x1604218),
        ("call", 0x45c560, 0xf8f390),
        ("call", 0x45c57e, 0xf8f3a0),
    ],
    "native": {'header': 'native/core/ctor_slot_457_45c440.hpp', 'source': 'native/core/ctor_slot_457_45c440.cpp', 'test': 'native/tests/test_ctor_slot_457.cpp', 'cmake': 'native/lanes/m3.cmake', 'consts': {'kSlot457Rva': 4572224, 'kSlot457Size': 323}, 'header_tokens': ['kSlot457FnTag = 0x15bad48', 'kSlot457ObjectRva = 0x2ef80c0'], 'source_order': ['init_bool_var_site(objects.var, kSlot457Name, kSlot457FnTag, concmd, 0, 0)'], 'test_tokens': ['kSlot457Rva == 0x45c440', 'section_alone()', 'section_failures()'], 'cmake_tokens': ['core/ctor_slot_457_45c440.cpp', 'add_executable(test_ctor_slot_457 tests/test_ctor_slot_457.cpp)', 'add_test(NAME ctor_slot_457 COMMAND test_ctor_slot_457)']},
}


if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC, 'One bool console variable `showCollisionMeshTests` (value 0, default 0, functor tag 0x15bad48, flags 1): 0x20af0, f590(desc), 0x20d60, f590(name), atexit(0x20b0).'))
